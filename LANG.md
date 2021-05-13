# Jet ✈️ Compiler Features
The language specification is listed below, everything from function variables, and the build systems that are planned to 
be implemented or are already implemented are below. 
> **Note:** To determine whether a feature currently exists check for the keywords: **Implemented**, **WIP**, and **Planned**.


Outline
-----------------

* [Primitives](#primitives)
* [Operators](#operators)
* [Arrays](#arrays)
* [Types](#types)
* [Functions](#functions)


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

Arrays
--------

### Initialization
We wanted arrays to be simple and easy to initalize like in Python.
```js
var list = [1, 2, 3, 4, 5]; // This by default is a dynamic array unless specefied like below
var fixed: [5,int] = [1, 2, 3, 4, 5];
```

### Designated Initializers
Lists are mutable by default (unlike most of the other data types) because they are most 
commonly dynamic and require mutability. Since we like to acknowledge the strengths of some
languages and weaknesses of others, one may consider a similar example of initalizing an array
using designated initalizers in Jet.
```c
var list: [256,int] = [
  ['>'] = 2323, ['3'] = 343,
  ...
]
```
The advantages of such a method reduce excess array initalization code, and allow for better readability to 
the programmer. This feature is currently not available in Rust or in C++, while only existing as a C99 extension.
We feel that this as a design principle is good for dealing with code that relies on jump comparisons. 

### Length
```python
len(list);
```
Length is checked in a similar global length function that can find the length of various types, including lists.

### Variable initialization
```python
var (a, b, c) = [1, 2, 3];
```
Similar to Python, this tuple like initialization is valid in Jet for convience and pretty clean and readable syntax.

### Slices
```
list[ Initial : End : IndexJump ]
```
List slicing works just as expected in Python and has the same exact effect. For example, `list[::-1]` reverses the list.


