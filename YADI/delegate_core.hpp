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
		//you may want to replace this with your own ID solution
		//however, this approach is sufficient for most use cases
		using handle_id_type = long long unsigned;
		static const handle_id_type invalid_id{ static_cast<handle_id_type>(-1) };

		delegate_base() = default;
		delegate_base(delegate_base const&) = default;

		//all delegates must implement subscribe/unsubscribe methods
		virtual delegate_handle subscribe(delegate_handle&) = 0;
		virtual void unsubscribe(delegate_handle&) = 0;

		virtual ~delegate_base() = default;

	protected:
		//some useful functions for working with delegateHandles

		//notify the handle that its subscription was successful
		void notify_handle_subscribed(delegate_handle& handle);

		//notify the handle that its unsubscription was successful
		void notify_handle_unsubscribed(delegate_handle& handle);

	private:
		//we need a persistent way to identify unique delegate handles
		//this approach is simple and has sufficient ID space for most reasonable
		//use cases
		inline static handle_id_type current_id{ 0 };
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
			, m_id{ other.m_id }
		{
			//remove subscription from old (invalid) instance
			other.m_id = delegate_base::invalid_id;
			other.m_boundDelegate = nullptr;
		}

		~delegate_handle()
		{
			if (m_boundDelegate)
			{
				m_boundDelegate->unsubscribe(*this);
			}
		}
	private:
		delegate_base* m_boundDelegate{ nullptr };
		//do not use up ID space until we actually need it
		delegate_base::handle_id_type m_id{ delegate_base::invalid_id };

		friend class delegate_base;
	};



	//implementation below

	void delegate_base::notify_handle_subscribed(delegate_handle& handle)
	{
		handle.m_id = current_id++;
		handle.m_boundDelegate = this;
	}

	void delegate_base::notify_handle_unsubscribed(delegate_handle& handle)
	{
		handle.m_id = invalid_id;
		handle.m_boundDelegate = nullptr;
	}
}
#endif