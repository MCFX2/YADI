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

//delegate and delegate_interruptible both use std::function extensively
#include <functional>

namespace yadi
{
	//useful helper functions needed across this library
	namespace helper
	{
		//performs type-erasure on a class member function and returns an std::function
		//with the same signature, bound to the given instance.
		template<typename Ret_type, typename Src_type, typename Inst, typename... Args>
		std::function<Ret_type(Args...)> attach(Ret_type(Src_type::* func)(Args...), Inst instance)
		{
			auto boundFunction{ [instance, func](Args... args)->Ret_type
				{
					return (instance->*func)(args...);
				}};
			return boundFunction;
		}
	}

	class delegate_base
	{
	private:
		friend class delegate_handle;

	public:
		delegate_base() = default;
		delegate_base(delegate_base const&) = delete;

		virtual delegate_handle subscribe();

		virtual ~delegate_base() {}
	};


	class delegate_handle
	{
	public:
		delegate_handle() {}

	private:
		delegate_base* m_boundDelegate{ nullptr };
	};
}
#endif