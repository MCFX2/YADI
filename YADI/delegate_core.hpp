#ifndef YADI_DELEGATE_CORE_H
#define YADI_DELEGATE_CORE_H
/**************************************************************************************************
 delegate_core :
   This file contains the core functionality that all other delegate behavior is derived from.

   You probably don't want to directly include this, instead you likely want one of the following
   public interfaces:
	 delegate.hpp               - standard, simple, event-like multicast delegate
	 delegate_fast.hpp          - high-performance delegate with fewer subscription options
	 delegate_interruptible.hpp - delegate that can be interrupted by one of the callbacks

***************************************************************************************************/

#include "delegate_util.hpp"

namespace yadi
{
	//Forward declarations
	class delegate_handle;

	//we define a base delegate class and derive from it.
	//this lets us avoid making delegate_handle a template class,
	//which in turn makes the interface simpler.
	class delegate_base
	{
	public:
		delegate_base() = default;
		delegate_base(delegate_base const&) = default;

		/*
		* Given a delegate_handle (representing a valid subscription),
		* remove the subscription and deactivate the handle. If the
		* handle doesn't belong to this delegate, do nothing.
		*
		* Params:
		*	- handle
		*		The handle representing the subscription.
		*/
		virtual void unsubscribe(delegate_handle& handle) = 0;

		virtual ~delegate_base() = default;

	protected:
		//some useful functions for working with delegateHandles

		//notify the handle that its subscription was successful
		void notify_handle_subscribed(delegate_handle& handle);

		//notify the handle that its unsubscription was successful
		void notify_handle_unsubscribed(delegate_handle& handle);

		/*
		* Given two delegate handles, move a subscription from one
		* to the other. This does NOT change the underlying function, only
		* the instance.
		*
		* Effectively, this changes which handle refers to a given function.
		*
		* Params:
		*	- old_handle
		*		The handle that we want to remove.
		*	- new_handle
		*		The handle that we want to replace it with. The caller is
		*		responsible for ensuring that this handle is not already
		*		subscribed to something else first.
		*/
		virtual void move_subscription(delegate_handle& old_handle, delegate_handle& new_handle) = 0;
	private:
		//needed to give access to move_subscription
		friend class delegate_handle;
	};


	class delegate_handle
	{
	public:
		delegate_handle() = default;

		//delegate_handles cannot be copied (there is no meaningful way to handle this)
		delegate_handle(delegate_handle const&) = delete;
		//however, they can be moved
		delegate_handle(delegate_handle&& other) noexcept
			: m_boundDelegate{ other.m_boundDelegate }
		{
			if (m_boundDelegate)
			{
				m_boundDelegate->move_subscription(other, *this);
			}
		}

		//copy assignment is similarly illegal
		delegate_handle& operator=(delegate_handle const&) = delete;
		//and likewise, move assignment is also allowed
		delegate_handle& operator=(delegate_handle&& other) noexcept
		{
			if (m_boundDelegate)
			{
				m_boundDelegate->unsubscribe(*this);
			}
			if (other.m_boundDelegate)
			{
				other.m_boundDelegate->move_subscription(other, *this);
			}

			return *this;
		}

		//manually detach this handle from its parent delegate
		void unsubscribe()
		{
			if (m_boundDelegate)
			{
				m_boundDelegate->unsubscribe(*this);
			}
		}

		~delegate_handle()
		{
			unsubscribe();
		}
	private:
		delegate_base* m_boundDelegate{ nullptr };

		friend class delegate_base;
	};



	//implementation below

	void delegate_base::notify_handle_subscribed(delegate_handle& handle)
	{
		handle.m_boundDelegate = this;
	}

	void delegate_base::notify_handle_unsubscribed(delegate_handle& handle)
	{
		handle.m_boundDelegate = nullptr;
	}
}
#endif