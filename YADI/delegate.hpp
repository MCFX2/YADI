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

namespace yadi
{
	template<typename... Args>
	class delegate : delegate_base
	{
	private:
		using callback_type = void(Args...);
	public:
		delegate() = default;
	};
}
#endif