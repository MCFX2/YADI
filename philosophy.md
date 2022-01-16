I believe the greatest flaw with modern software development is that libraries do not respect their own scope. Features are added because "someone" may find them useful, with little consideration for how this impacts future users of the library.

Consequently, libraries grow increasingly opaque and hard to understand over time, as more features are added that most people don't need and won't use. But they're popular because, hey, _maybe_ they will be useful later.

I challenge this approach. Instead, I believe that the best libraries are as small and focused as possible, _but leave room for extensibility_. Don't give me a coffee machine library with functionality to handle 50 different kinds of drinks. Give me functionality to make just coffee and leave the interface open for me to make my own drink-making procedure. Chances are, as a library author, you are not going to correctly predict the exact behavior users want anyway.

The most powerful interface isn't one with a million options, it's one that lets you define your own behavior.
