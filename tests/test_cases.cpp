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
//requires arg1.operator==(arg2)
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

namespace yadi
{
    namespace tests
    {
        //useful class for testing functionality
        struct example_class
        {
            inline static int global_value{ 0 };
            int local_value{ 0 };

            std::string growing_string{ "abc" };

            //simple function that increments a global and local variable to show its been run
            void basic_function()
            {
                local_value++;
                global_value++;
            }

            //simple function that increments a global and local variable by a specified amount
            void one_arg_function(int number)
            {
                local_value += number;
                global_value += number;
            }

            //simple function that increments a global and local by different amounts
            void two_arg_function(int local_increment, int global_increment)
            {
                local_value += local_increment;
                global_value += global_increment;
            }

            //simple function that appends to a string
            virtual void virtual_function(std::string const& text)
            {
                growing_string += text;
            }

        };

        //for testing virtual functions
        struct other_class : public example_class
        {
            //this method replaces the string with text instead of appending it
            void virtual_function(std::string const& text) override
            {
                growing_string = text;
            }
        };

        int free_increment = 0;
        //testing free functions

        //increment the global counter by 1
        void fn_no_args()
        {
            free_increment++;
        }

        //increment the global counter by a
        void fn_one_arg(int a)
        {
            free_increment += a;
        }

        //increment the global counter by a, then decrement it by b
        void fn_two_args(int a, int b)
        {
            free_increment += a - b;
        }

        //doubles the value you pass it (by reference)
        void fn_reference(int& a)
        {
            a *= 2;
        }

        /*
        * Test all helper functions in yadi::util
        */
        void util_functions()
        {
            //create instance and attach for each member function
            example_class testObject;

            auto basic_attached{ util::attach(&example_class::basic_function, &testObject) };
            auto one_arg_attached{ util::attach(&example_class::one_arg_function, &testObject) };

            //test that passing a reference works too
            auto two_arg_attached{ util::attach(&example_class::two_arg_function, testObject) };

            ASSERT_EQ(testObject.local_value, 0);
            ASSERT_EQ(example_class::global_value, 0);

            //test invoking functions with different numbers of arguments
            basic_attached();

            ASSERT_EQ(testObject.local_value, 1);
            ASSERT_EQ(example_class::global_value, 1);

            one_arg_attached(3);
            
            ASSERT_EQ(testObject.local_value, 4);
            ASSERT_EQ(example_class::global_value, 4);

            two_arg_attached(10, 100);

            ASSERT_EQ(testObject.local_value, 14);
            ASSERT_EQ(example_class::global_value, 104);

            example_class::global_value = 0;
        }

        /*
        * Test all basic subscribe functionality, particularly the following:
        *     - Subscribe during handle construction
        *     - Subscribe member class functions
        *     - Subscribe regular "free" function
        *     - Subscribe member virtual function
        *     - Subscribe lambda function
        *       - Test both with and without capture statements
        *     - Verify that statistics functions report correct numbers
        */
        void basic_subscribe()
        {
            ASSERT_EQ(example_class::global_value, 0);

            example_class testObject;
            testObject.local_value = 5;

            yadi::delegate<int> testDelegate;

            { //open a scope so we can control our handle getting destroyed
                delegate_handle handle{ testDelegate.subscribe(&example_class::one_arg_function, testObject) };
                //should run once
                testDelegate(5);

                ASSERT_EQ(example_class::global_value, 5);
                ASSERT_EQ(testObject.local_value, 10);
                ASSERT_EQ(testDelegate.subscriber_count(), 1);

                //let delegate_handle fall out of scope
            }

            testDelegate(500); //should have no effect - no live handles remain

            ASSERT_EQ(example_class::global_value, 5);
            ASSERT_EQ(testObject.local_value, 10);
            ASSERT_EQ(testDelegate.subscriber_count(), 0);

            //create several instances
            std::vector<delegate_handle> handles;
            for (auto i{ 0 }; i < 50; ++i)
            {
                handles.push_back(testDelegate.subscribe(&example_class::one_arg_function, testObject));
            }

            //now one object is subscribed 50 times

            //go ahead and throw a free function in there too
            delegate_handle temp_handle{ testDelegate.subscribe(&fn_one_arg) };

            ASSERT_EQ(free_increment, 0);

            //should increment global and local by 100
            //should increment free_increment by 2
            testDelegate(2);

            ASSERT_EQ(example_class::global_value, 105);
            ASSERT_EQ(testObject.local_value, 110);
            ASSERT_EQ(free_increment, 2);
            ASSERT_EQ(testDelegate.subscriber_count(), 51);

            //remove all handles
            handles.clear();

            //should be (once again) no subscriptions except the one free function
            testDelegate(10000);

            ASSERT_EQ(example_class::global_value, 105);
            ASSERT_EQ(testObject.local_value, 110);
            //this value should have been incremented- the subscription still exists
            ASSERT_EQ(free_increment, 10002);
            ASSERT_EQ(testDelegate.subscriber_count(), 1);

            //now test a virtual function subscription
            delegate<std::string const&> stringDelegate;

            other_class secondObject;

            //basic test
            {
                delegate_handle handle{ stringDelegate.subscribe(&other_class::virtual_function, secondObject) };

                ASSERT_EQ(secondObject.growing_string, "abc");
                ASSERT_EQ(stringDelegate.subscriber_count(), 1);

                stringDelegate("cat");
                
                ASSERT_EQ(secondObject.growing_string, "cat");
            }

            ASSERT_EQ(stringDelegate.subscriber_count(), 0);
            //test to see if virtual functions work properly
            {
                delegate_handle handle{ stringDelegate.subscribe(&example_class::virtual_function, static_cast<example_class*>(&secondObject)) };

                ASSERT_EQ(secondObject.growing_string, "cat");
                ASSERT_EQ(stringDelegate.subscriber_count(), 1);

                stringDelegate("dog");

                //if virtual lookup fails, this would be "catdog" instead
                ASSERT_EQ(secondObject.growing_string, "dog");
            }

            ASSERT_EQ(stringDelegate.subscriber_count(), 0);


            example_class::global_value = 0;
            free_increment = 0;
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
        *    - DelegateHandle-bound object moved between subscribe and unsubscribe
        *    - Combinations of the above
        */
        void class_moved()
        {

        }

        /*
        * Run a test and print the result.
        * Currently tests will fail asserts when something goes wrong, so this will only ever output success messages.
        * 
        * Params:
        *   - test
        *       A pointer to the test function to run.
        *   - label
        *       What the test should be called in the console.
        */
        void run_test(void(*test)(), std::string const& label)
        {
            std::cout << "=======RUNNING TEST " << label << "=======" << std::endl;
            test();
            std::cout << "|||||||TEST COMPLETE||||||" << std::endl; 
        }
    }
}