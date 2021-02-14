# YADI
Yet Another (C++) Delegate Implementation

This is a pretty straightforward system. [See the test cases](https://github.com/MCFX2/YADI/blob/main/tests/test_cases.cpp) for a code example. For a higher-level overview, read on.

# What are delegates?
Delegates are a way to implement event-like behavior. Essentially, when you'd normally call a function (like `OnCoffeeDoneBrewing()`), which would then be hardcoded with all the other systems that care about it, you'd instead call your delegate (`CoffeeDoneBrewingDelegate()`) and not worry about who's listening. The delegate will go ahead and notify all listeners that the event happened, and pass along any data you wish.

# What does your implementation do differently?
Modern delegate implementations are plagued with one universal issue: They try to do too much. It's common for C++ programmers to reach for an insanely powerful, complex and intricate solution. However, flaws in these systems can be hard to spot early on, and expensive to fix. [This award-winning C++ delegate implementation](https://www.codeproject.com/Articles/1160934/Asynchronous-Multicast-Delegates-in-Cplusplus) requires the end user to learn twelve classes and read about forty pages worth of documentation. If you run into a problem with it, good luck figuring out where among the _over 6,400 lines_ of code the issue resides. This is a fantastic solution if you need all fifteen features it supports, but most people don't. They just need a quick, lightweight, drop-in solution for solving encapsulation between an event and its listeners.

Yet Another Delegate Implementation (YADI) is exactly that and nothing more. All of the usage is designed to be as straightforward and unambiguous as possible. You can get a pretty good idea of what any function does in under thirty seconds. It lives in three header files, each with simple, reusable, and easy-to-understand functionality. You shouldn't have to be a C++ expert just to understand what your libraries are doing. With YADI, you don't have to be. Even the copyright of this library is granted under the flexible and easy-to-understand MIT license.

# How do I use this in my project?
It's super simple. Just drop the `YADI` folder somewhere your project can see it and `#include <YADI/delegate.hpp>` to start using the basic `yadi::delegate` class. All the code examples you need are in the [test cases.](https://github.com/MCFX2/YADI/blob/main/tests/test_cases.cpp)

# I need X feature. Can it be added?
Of course! Anything that drives from `yadi::delegate_base` and implements the needed functions can be used by the given `delegate_handle` class without affecting user code whatsoever. The core functionality is very thoroughly-documented and easy to extend. `yadi::delegate`, a fully-functional delegate class, is also a simple example of how you might implement the base functionality.
