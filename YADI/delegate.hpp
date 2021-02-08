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

		std::map<delegate_handle*, std::function<callback_type>> m_callbacks;

	public:
		delegate() = default;

		delegate_handle subscribe(void(*fn)(Args...))
		{
			delegate_handle handle;
			//delegate_handle move ctor will ensure this entry stays valid
			m_callbacks.emplace(&handle, fn);
			notify_handle_subscribed(handle);
			return handle;
		}

		template<typename T>
		delegate_handle subscribe(void(T::* fn)(Args...), T* instance)
		{
			delegate_handle handle;
			m_callbacks.emplace(&handle, util::attach(fn, instance));
			notify_handle_subscribed(handle);
			return handle;
		}

		template<typename T>
		delegate_handle subscribe(void(T::* fn)(Args...), T& instance)
		{
			return subscribe(fn, &instance);
		}

		void unsubscribe(delegate_handle& handle)
		{
			auto entry{ m_callbacks.find(&handle) };
			if (entry != m_callbacks.end())
			{
				m_callbacks.erase(entry);
				notify_handle_unsubscribed(handle);
			}
		}

		void move_subscription(delegate_handle& old_handle, delegate_handle& new_handle) override
		{
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

		//Returns the number of functions subscribed to this delegate.
		size_t subscriber_count() const
		{
			return m_callbacks.size();
		}

		//Force-removes all subscribers from this delegate immediately.
		void force_clear()
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