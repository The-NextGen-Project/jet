# Jet ✈️ Language Documentation

The language specification is listed below, everything from function variables, and the build systems that are planned
to be implemented or are already implemented are below.

> **Note:** To determine whether a feature currently exists check for the keywords: **Implemented**, **WIP**, and **Planned**.

## Outline

- [Primitives](#primitives)
- [Operators](#operators)
- [Modules](#modules)
- [Arrays](#arrays)
- [Variables](#variables)
- [Functions](#functions)
- [Structs](#struct-declaration)
- [Enum](#enum-declaration)
- [Match Statement](#match-statement)

## Primitives

Current Status: **WIP**

| Core Type                 | Description                                                                           |
| ------------------------- | ------------------------------------------------------------------------------------- |
| `i32`                     | By default, all integer types are designated to 32 bits unless specified differently. |
| `i64`                     | 64-bit integer is separate and is using for longer integer values.                    |
| `u32`                     | Unsigned 32-bit integer.                                                              |
| `u64`                     | Unsigned 64-bit integer.                                                              |
| `f32`                     | 32-bit decimal value.                                                                 |
| `f64`                     | 64-bit decimal value.                                                                 |
| `bool`                    | Boolean Value: true or false.                                                         |
| `fn(Arg Types...) -> Ret` | Function Lambda.                                                                      |
| `char`                    | A 1 byte character.                                                                   |
| `str`                     | Array of characters                                                                   |
| `None`                    | Optional value for `None`                                                             |

### Type Annotations

| Annotation | Description                                                                |
| ---------- | -------------------------------------------------------------------------- |
| `box`      | Designates a type as smart pointer value                                   |
| `*`        | Type is a pointer.                                                         |
| `?`        | Optional type. Either None or the value.                                   |
| `&`        | allocator reference to the location of an object (or a pass by reference). |

## Operators

Current Status: **Completed**

| Operator | Description            |
| -------- | ---------------------- |
| `+`      | Addition               |
| `-`      | Subtraction            |
| `/`      | Division               |
| `*`      | Multiplication         |
| `%`      | Modulus                |
| `^`      | XOR                    |
| `&`      | Bitwise AND            |
| `<<`     | Shift Left             |
| `>>`     | Shift Right            |
| `~`      | Bitwise Not and Concat |
| `**`     | Exponential Power      |
| `!`      | Logical Not            |
| `??`     | Then operator          |

## Variables

Current Status: **Completed**

The programming language is gradually typed. Static typing is only required when the compiler cannot infer the type. Variables may be declared in two different ways:

```v
my_variable := 23;
another_variable: bool = false;
```

All variables are immutable by default (except for lists), and require the `mut` keyword in order to change the way you interact with that variable.

```v
cant_change_me := 2324;
cant_change_me = 24354; // Compiler error!
mut change_me := 3342;
change_me = 23424; // Ok
```

Custom initialization is specified in with different primitive datatypes (ie: lists and tuples). Other than these two, the specification is standard for all data types.

## Arrays

Current Status: **WIP**

### Initialization

We wanted arrays to be simple and easy to initialize like in Python.

```rust
mut list := [1, 2, 3, 4, 5]; // Can now add values to the list
fixed: [5,i32] = [1, 2, 3, 4, 5];
```

### Designated Initializers

Current Status: **Planned**

Lists are mutable by default (unlike most of the other data types) because they are most commonly dynamic and require mutability. Since we like to acknowledge the strengths of some languages and weaknesses of others, one may consider a similar example of initializing an array using designated initializers in Jet.

```v
list: [256,i32] = [
  ['>'] = 2323, ['3'] = 343,
  ...
]
```

The advantages of such a method reduce excess array initialization code, and allow for better readability to the programmer. This feature is currently not available in Rust or in C++, while only existing as a C99 extension. We feel that this as a design principle is good for dealing with code that relies on jump comparisons.

### Length

Current Status: **Planned**

```python
len(list)
```

Length is checked in a similar global length function that can find the length of various types, including lists.

### Slices

Current Status: **Planned**

```zig
list[ Initial : End : IndexJump ]
```

List slicing works just as expected in Python and has the same exact effect. For example, `list[::-1]` reverses the list.

## Functions

Current Status: **WIP**

Functions like anything else are declared explicitly in structs or are declared in the global module scope. If functions are not exported, they are only viewable to the members in the module that it was declared in. The `export` keyword is equivalent to the `public` keyword in other languages.

### Function Declaration

Current Status: **WIP**

```zig
my_func => (a: i32, b: i32, c: str) {
...
}
```

parameters are ordered in the format of VariableName -> Typename. A variable of type `str` with a name of `name` would be written as `name str`. For those familiar with Golang, it follows a similar naming convention. Generic functions are also supported:

```zig
generics => ($a, $b) {
 ...
}
```

Generic function parameters have anonymous traits bounded to them which means that their functions and generated code is guaranteed to be valid otherwise a compile-time error will be thrown otherwise. This allows for a mix of static and dynamic typing flexibility for types that can be generalized for similar actions. To make it easier for anonymous trait building, it is best for the programmer to statically define their variables to make it easier for the compiler.

### Default Values

```zig
default_values := (a: int = 0xffff, b: str) {
...
}
```

Default values allow for flexibility in programming, especially when you want to handle a case without passing in a parameter. Currently, it is not planned to have default values for parameters that don't have their type explicitly stated.

### Lambdas

Current Status: **Planned**

Lambdas are small anonymous functions that can be passed in as callbacks into functions to be called. They are generally declared as a parameters by the following method:

```zig
func => (lambda: fn(i32) -> i32) {
  lambda(23); // Calling the function
}
```

You may pass in lambdas through generic parameters as well and call them as long as function definition generation works as intended. An Example for this function call may be (in reference to the above example):

```zig
func(|x| => {
  return x * x
})
```

## Modules

Current Status: **WIP**

The way a module system of a programming language dictates its entire package functionality structure. In order to be able to give the programmer a lot of control we have decided to make it explicit as to functions / types are exported out in the file's module.

### Imports

Current Status: **WIP**

```python
import SomeModuleName
```

This is how most modules will be imported directly.

> **Note:** Sub module structure like `std.func` are still in debate as of now, and this will be updated in the future to reflect language changes.

### GitHub Imports

Current Status: **Planned**

```python
import 'https://github.com/The-NextGen-Project/jet.git' as SomeModule
```

This is especially a very interesting part of the programming language. We will have support for open source imports. These projects will need to be Jet projects that can be recognizable and imported directly into your project. This may be a good way to consistently ensure that you have the latest stable version of the git repository imported into your programming language.

### Exporting values

Current Status: **Planned**

#### Functions

```zig
export my_func => (...) { ... }
```

#### Types

```zig
export MyStruct => struct {
  ...
}
```

#### Variables

```js
export
MyGlobalVariable
:
= 32;
```

Values are explicitly exported from the file (each file is a module with exported functions and types). This is denoted by the prefix of the `export` before the declaration.

### Static Compilation (Generally JIT Compiled)

Examples:

```zig
// Evaluated at compile-time
some_value := static {
  return calling_a_function();
}
```

### Match Statement

Match statements allow for expressing complex situations in shorter and more readable code. Examples:

```rust
match value {
  0 -> {
    // do seomthing
  }
  . -> {
    // default
  }
}
```

### Struct Declaration

Struct declarations will emulate C in which they are handled. The language aims to be as simple as possible while allowing for zero cost abstractions to take place. Examples:

```zig
MyStruct => struct {
  prop: str;
  another: i32;
}
```

### Enum Declaration

Enum declarations will be the core foundation for applying complex principles in the language. Tagged unions as well as constant values may be implemented.

#### Regular Enum

```zig
Opcodes => enum {
  Add,
  Sub,
  Mul,
  Div
}
```

#### Tagged Enums

```zig
Operation => enum {
  Add {
    lhs: i32
    rhs: i32
  },
  Sub {
    lhs: i32
    rhs: i32
  }
  // etc ...
}
```

## End

You made it to the end? Wow, thank you for the interest in reading the language. Before we lose your attention, please consider [contributing](https://github.com/The-NextGen-Project/jet/blob/main/CONTRIBUTING.md).
