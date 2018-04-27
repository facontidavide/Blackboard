# Blackboard

The goals of this class is to provide a Blackboard mechanism for this library https://github.com/miccol/Behavior-Tree.

- We want an abstract class Blackboard which allow different backends, such as a simple key/value storage, a persistent SQLite database, ROS Param Server, YARP, SmartSoft, etc

- The abstract class __must not require__ any of these external dependencies to be linked statically, when the library ** behavior_tree_core** is compiled.

- It should be possible to load a backend at run-time using plugins / shared libraries. 

- Allow a potential infinite number of types (not only numbers and strings), being typesafe, i.e. preventing undefined conversion to a wrong type. Nite that the fact that the front-end allows any type, doesn't imply that the backend can!

## Solution

The only feasible solution seems to be type erasure, achieved either with an equivalent of [std::any](http://en.cppreference.com/w/cpp/utility/any) or [std::variant](http://en.cppreference.com/w/cpp/utility/variant).

Currently we are also using our own class AnyNumber, that can store any arithmetic value (8 to 64 bits) and takes care of safe conversion of type.

By "safe conversion" we mean that:

- Negative numbers can not be converted to unsigned.
- An integer can be converted to another one only if there isn't any overflow. For example, 50.000 can not be converted to __short__ or __char__.


