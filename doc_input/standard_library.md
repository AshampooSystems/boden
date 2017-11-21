Duplicating functionality from the C++ standard library
=======================================

In the Boden framework there are several classes that provide functionality that is also
provided by the C++ standard library. In some cases the Boden version simply derives from the
standard classes and provides aliases (different names) for some of the functions.
In other cases functionality is aggregated, moved or provided in a different way.

This article discusses why the decision was made to provide different solutions for
problems that the standard library also solves.

Design philosophies
-------------------

The C++ standard library is designed in a different way than Boden:

* the standard library puts a much higher emphasis on speed
* it tries to find very generic solutions that fit all use cases
* it accepts higher complexity to achieve the speed and generality goals
* it does not have the goal to be easy to use
* its documentation is highly technical and often discusses implementation details instead of
  guidelines on the intended way to use the constructs
* it accepts that newcomers require considerable effort to learn the concepts

Boden on the other hand:

* tries to provide convenient solutions
* emphasizes simplicity, ease of use and ease of maintenance
* tries to provide concise documentation with usage guidelines
* accepts that special use cases may require more effort to solve since the simple constructs
  might not match as well
* tries to be easily accessible and understandable to newcomers


Naming conventions
------------------

The Boden library uses much different naming conventions than the standard library. This does not only refer
to the way the names are represented (Boden's camelCase vs. the standard library underscore_names ).
A much more severe difference is that the standard library often uses abbreviations and short labels in an effort
to create shorter names. It is often necessary to consult the documentation to understand what a piece of code
actually does. Boden on the other hand tries to use self explanatory names that are often a little
longer, but that try to describe better what the concepts actually do.


Single point of reference
-------------------------

Boden tries to provide all the basic tools to create good applications. As such it is helpful when the
functionality provided by the standard library is also provided in Boden - simply because then you only have
one place to look when you try to find a solution for a given problem.


Bugs
----

The C++ standard library is very complex and different versions of it (depending on compiler and platform)
often have different problems and bugs.

By wrapping or reshaping the functionality Boden has the opportunity to work around known bugs and provide
a more reliable platform for applications that works the same on all platforms.


C++ versions and features
-------------------------

C++ has been undergoing some rapid changes in recent years in an effort to keep pace with other programming
languages. As such, some useful new constructs are not supported by all compilers and platforms.

Boden has the ability to fill in missing modern functionality on platforms where the standard library
lags behind and provide a consistent basis for application developers


Compatibility and mixing the libraries
--------------------------------------

Despite the different goals, Boden tries to be as compatible as possible to the standard library, so
that experienced developers can keep using their accustomed tools and so that existing C++ code and
libraries combine well with Boden.

For example, in the case of collection classes (Array, List, etc.)
each Boden collection derives from the corresponding standard library collection class.
It is 100% compatible and Boden collections can be mixed and matches with standard library collections.
Boden simply adds aliases and additional convenience functionality on top of the standard functionality.








