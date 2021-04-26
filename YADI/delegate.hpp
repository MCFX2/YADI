#ifndef YADI_DELEGATE_H
#define YADI_DELEGATE_H
/************************************************************************
 delegate :
	This contains the functionality for the basic yadi::delegate class.
	yadi::delegate has the following restrictions and features:

	- From the delegate perspective, this is simply a function call that
	  users can hook into and get called alongside.
	  - It's essentially an event that doesn't defer execution.

	- You must expose this class's subscribe() method somehow at minimum,
	  for users to be able to interact with this delegate. It is not
	  necessary to expose anything else.

	- Subscription lifetimes are managed for you automatically, but you
	  are given some control via the delegate itself as well.

	- No return values are allowed for delegate listeners.

*************************************************************************/

#include "delegate_core.hpp"

#include <map>

namespace yadi
{
	template<typename... Args>
	class delegate : delegate_base
	{
	private:
		using callback_type = void(Args...);

		//std::map is used because there is no point in optimizing the execution of these functions
		//calling a bunch of "random" functions already wreaks havoc on cache locality
		//instead, it makes far more sense to optimize for search/remove/insert
		std::map<delegate_handle*, std::function<callback_type>> m_callbacks;

	public:
		delegate() = default;

		/*
		* Given a member function pointer (&Coffee::Brew) and a pointer to an instance,
		* subscribe that function to this delegate. The resulting call from the delegate
		* will be the same as if you had done instance->fn(Args...).
		*
		* Params:
		* 	- fn
		*		The member function to subscribe. Its signature must match the one provided by the delegate.
		*	- instance
		*		A pointer to the object to call the function on. It must be valid to call the given member function on it.
		*
		* Returns:
		*	A delegate_handle representing the subscription. When it goes out of scope, the subscription will be removed.
		*	See the delegate_handle notes in delegate_core.hpp for more details.
		*/
		template<typename T>
		delegate_handle subscribe(void(T::* fn)(Args...), T* instance)
		{
			delegate_handle handle;
			m_callbacks.emplace(&handle, util::attach(fn, instance));
			notify_handle_subscribed(handle);
			return handle;
		}

		/*
		* Given a member function pointer (&Coffee::Brew) and an instance,
		* subscribe that function to this delegate. The resulting call from the delegate
		* will be the same as if you had done instance.fn(Args...).
		*
		* Params:
		* 	- fn
		*		The member function to subscribe. Its signature must match the one provided by the delegate.
		*	- instance
		*		An lvalue reference of the object to call the function on. It must be valid to call the given member function on it.
				You must ensure that the delegate_handle returned does not outlive the object.
		*
		* Returns:
		*	A delegate_handle representing the subscription. When it goes out of scope, the subscription will be removed.
		*	See the delegate_handle notes in delegate_core.hpp for more details.
		*/
		template<typename T>
		delegate_handle subscribe(void(T::* fn)(Args...), T& instance)
		{
			return subscribe(fn, &instance);
		}

		/*
		* Given any function object (lambda, function pointer, functor, etc), subscribe it to this delegate.
		* The resulting call from the delegate will be the same as if you had done fn(Args...).
		*
		* Params:
		* 	- fn
		*		The function to call. This can be any type that will construct a valid std::function.
		*		That includes function pointers, lambdas, and other functors.
		*
		* Returns:
		*	A delegate_handle representing the subscription. When it goes out of scope, the subscription will be removed.
		*	See the delegate_handle notes in delegate_core.hpp for more details.
		*/
		delegate_handle subscribe(std::function<callback_type> const& fn)
		{
			delegate_handle handle;
			//delegate_handle move ctor will ensure this entry stays valid
			m_callbacks.emplace(&handle, fn);
			notify_handle_subscribed(handle);
			return handle;
		}

		/*
		* Given a delegate_handle (representing a valid subscription),
		* remove the subscription and deactivate the handle. If the
		* handle doesn't belong to this delegate, do nothing.
		*
		* Params:
		*	- handle
		*		The handle representing the subscription.
		*/
		void unsubscribe(delegate_handle& handle) override
		{
			auto entry{ m_callbacks.find(&handle) };
			if (entry != m_callbacks.end())
			{
				m_callbacks.erase(entry);
				notify_handle_unsubscribed(handle);
			}
		}

		/*
		* Transfers ownership of a delegate subscription from one handle to another.
		* This assumes that the old handle is connected to this delegate already.
		* If it isn't, calling this has no effect.
		*
		* Params:
		*	- old_handle
		*		The handle to move the subscription away from. This handle should be subscribed to this delegate already.
		*	- new_handle
		*		The handle to move the subscription to. If it already owns another subscription, that one will be removed.
		*/
		void move_subscription(delegate_handle& old_handle, delegate_handle& new_handle) override
		{
			//Additionally, this is a very good template for how to implement this function in other delegate implementations.
			//This isn't part of the base class to allow flexibility in what underlying container you want to use.
			auto entry{ m_callbacks.extract(&old_handle) };
			if (!entry.empty())
			{
				notify_handle_unsubscribed(old_handle);
				entry.key() = &new_handle;
				m_callbacks.insert(std::move(entry));
				notify_handle_subscribed(new_handle);
			}
		}

		//Execute the underlying delegate, passing along the appropriate args.
		//This calls all subscribed functions.
		void operator()(Args... args)
		{
			for (auto& entry : m_callbacks)
			{
				entry.second(std::forward<Args>(args)...);
			}
		}

		//Returns the number of functions currently subscribed to this delegate.
		size_t subscriber_count() const
		{
			return m_callbacks.size();
		}

		//Force-removes all subscribers from this delegate immediately.
		void clear_all_subscriptions()
		{
			for (auto& item : m_callbacks)
			{
				notify_handle_unsubscribed(*item.first);
			}
			m_callbacks.clear();
		}
	};
}
#endif
