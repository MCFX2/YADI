/********************************************************************************
 delegate_util:
	This contains some functionality that's useful for different parts of YADI.
	You might some of these functions useful, too.
*********************************************************************************/

#include <functional>

namespace yadi
{
	namespace util
	{
		//performs type-erasure on a class member function and returns an std::function
		//with the same signature, bound to the given instance.
		//
		template<typename Ret_type, typename Src_type, typename Inst, typename... Args>
		std::function<Ret_type(Args...)> attach(Ret_type(Src_type::* func)(Args...), Inst* instance)
		{
			auto boundFunction{ [instance, func](Args... args)->Ret_type
				{
					return (instance->*func)(args...);
				} };
			return boundFunction;
		}

		//these are used to define some settings on delegate_handles
		class a;
	}
}