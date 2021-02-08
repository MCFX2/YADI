/*********************************************************************************************

	This is a series of tests to verify that subscription/unsubscription work in all cases.

	For details on what's tested specifically, see the function comments.

**********************************************************************************************/

#include "../YADI/delegate.hpp"

#include <iostream>

/* Testing definitions
*     TODO: Migrate to GTests or similar
*/
//values must be equal
//requires arg1.operator=(arg2)
#define ASSERT_EQ(x, y) _ASSERT(x == y)
//arg1 must be less than arg2
//requires arg1::operator<(arg2)
#define ASSERT_LT(x, y) _ASSERT(x < y)
//arg1 must be greater than arg2
//requires arg1::operator>(arg2)
#define ASSERT_GT(x, y) _ASSERT(x > y)
//values must not be equal
//requires arg1::operator!=(arg2)
#define ASSERT_NE(x, y) _ASSERT(x != y)
//arg1 must be less than or equal to arg2
//requires arg1::operator<=(arg2)
#define ASSERT_LTEQ(x, y) _ASSERT(x <= y)
//arg1 must be greater than or equal to arg2
//requires arg1::operator>=(arg2)
#define ASSERT_GTEQ(x, y) _ASSERT(x >= y)
//value must be truthy (i.e. nonzero)
#define ASSERT_TRUE(x) _ASSERT(x)
//value must be falsey (i.e. zero)
//requires value::operator!()
#define ASSERT_FALSE(x) _ASSERT(!x)

namespace testing
{
    /*
    *  Test all basic subscribe functionality, particularly the following:
    *     - Subscribe during handle construction
    *     - Subscribe member class functions
    *     - Subscribe regular "free" function
    *       - Test with both regular function and operator+=
    *     - Subscribe member virtual function
    *     - Subscribe lambda function
    *       - Test both with and without capture statements
    *     - Verify that statistics functions report correct numbers
    */
    void basic_subscribe()
    {

    }

    /*
    * Test all basic unsubscribe functionality, particularly the following:
    *    - Unsubscribe by force (.unsubscribe() method)
    *      - Both by function-match and handle ID
    *    - Unsubscribe automatically
    *      - Both by delegate_handle destructor and by overwriting delegatehandle subscription
    *    - Verify that statistics functions report correct numbers
    */
    void basic_unsubscribe()
    {

    }

    /*
    * Test functionality for delayed subscription and other less-optimistic usage, particularly the following:
    *    - Default-constructed delegate handle, subscribed at a future time
    *    - Attempted unsubscribe of empty delegate handle
    *    - Attempted execution of delegate with no listeners
    */
    void delayed_subscribe()
    {

    }

    /*
    * Test more advanced subscribe and execute functionality, particularly the following:
    *    - Referenceness in arguments
    *       - Test un-constructible objects for lvalue referenceness
    *       - Test move-only objects for rvalue referenceness
    *    - Return values
    *       - Test InterruptibleDelegate interrupting on value different from specified (default false-y)
    *       - Test multiple return value specification
    *    - FastDelegate
    *       - Benchmark and prove it is actually faster than normal delegates
    */
    void various_execute()
    {

    }

    /*
    * Test in more class-moved scenarios, particularly the following:
    *    - DelegateHandle moved between subscribe and unsubscribe
    *    - Delegate moved between subscribe and unsubscribe
    *    - Both moved between subscribe and unsubscribe
    */
    void class_moved()
    {

    }
}