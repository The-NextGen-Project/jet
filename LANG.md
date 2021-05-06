# Jet ✈️ Compiler Features
The language specification is listed below, everything from function variables, and the build systems that are planned to 
be implemented or are already implemented are below. 
> **Note:** To determine whether a feature currently exists check for the keywords: **Implemented**, **WIP**, and **Planned**.


Outline
-----------------

* [Primitives](#primitives)
* [Operators](#operators)
* [Keywords](#keywords)
* [Arrays](#documentation)
* [Functions](#subdirectory-configuration)
* [Type System](#type-system)


Primitives
--------------
Current Status: **WIP**
| Core Type     | Description |
| ------------- | ------------- |
| `int`         | By default, all integer types are designated to 32 bits unless specified differently.  |
| `int64`       | 64 bit integer is separate and is using for longer integer values.  |
| `uint`        | Unsigned 32 bit integer.  |
| `uint64`      | Unsigned 64 bit integer. |
| `float`       | 32 bit decimal value. |
| `float64`     | 64 bit decimal value. |
| `bool`        | Boolean Value: true or false.  |
| `fn(Arg Types...) -> Ret`       | Function Lambda.   |
| `utf8`        | A valid utf8 character. |
| `char`        | A 1 byte character. |
| `str`         | Alias for `^utf8` |

### Type Annotations
| Annotation     | Description |
| -------------  | ------------- |
| `^`            | Designates a type as a smart pointer (a little more into that later).  |
| `*`            | Type is a pointer. |
| `?`            | Optional type. Either None or the value. |
| `&`            | Memory reference to the location of an object (or a pass by reference). |

Operators
-----------
Current Status: **WIP**
| Operator       | Description |
| -------------  | ------------- |
| `+`            | Addition.  |
| `-`            | Subtraction. |
| `/`            | Division. |
| `*`            | Multiplication. |
| `%`            | Modulus. |
| `^`            | XOR. |
| `&`            | Bitwise AND. |
| `<<`           | Shift Left. |
| `>>`           | Shift Right. |
| `~`            | Bitwise Not and Concat. |
| `**`           | Exponential Power. |
| `!`            | Logical Not. |
> **Note:** The operators described above all have the combination operator with `=`, for example, `+` has `+=`. More operators may come
> in the future, but the list is not likely to change much at all.


### Operator Overloading
Current Status: **Planned**
> **Note:** Outline is not available yet, but will be soon.


