![Jet](https://github.com/The-NextGen-Project/jet/blob/main/.github/JETV4Large3.png)
[![GitHub license](https://img.shields.io/github/license/The-NextGen-Project/jet?color=%2320ba37&label=License&logo=%2320ba37&logoColor=%2320ba37&style=for-the-badge)](https://github.com/The-NextGen-Project/jet/blob/main/LICENSE) 
[![Linux](https://img.shields.io/github/workflow/status/The-NextGen-Project/jet/Linux?label=Linux&style=for-the-badge&logo=github)]()
[![Mac](https://img.shields.io/github/workflow/status/The-NextGen-Project/jet/MacOS?label=MacOS&style=for-the-badge&logo=github)]()
[![Windows](https://img.shields.io/github/workflow/status/The-NextGen-Project/jet/Windows?label=Windows&style=for-the-badge&logo=github)]()
#### Fast Language for Developing Consistent Native, Mobile App, and Web Applications.
> **NOTE:** The language is in its early development stages. Don't expect syntax to be fixed until v1.0. If you want to see the development branch, you can view it [here](https://github.com/The-NextGen-Project/jet/tree/dev). The main branch is the most stable with all tests passing on every platform.

> **UPDATE:** In order for faster development as well as for faster prototyping, parts of the language are being designed in Rust. Currently, the backend has been developed in Rust and is able to generate C code and executables.

Resources
-----------------

* [Download](#download)
* [Introduction To The Language](#a-test-flight)
* [Documentation](https://github.com/The-NextGen-Project/jet/blob/main/LANG.md)
* [Contributing](#contributing)
* [Discord Server](https://discord.gg/8cnuzFxZGx)
* [r/Programming Languages Discord Channel](https://discord.gg/tft9Nw5jm6)
* [License](#license)

Download 
---------
```
git clone https://github.com/The-NextGen-Project/jet.git
cd jet
git submodule init
git submodule update
cmake -DCMAKE_BUILD_TYPE=Release -GYourBuildGenerator
make
```
> **Note:** For building the project on windows, it can be done by simply building with MSVC or, if you have MinGW installed it will work with that C compiler as well. For setting up a build system that is fast, I reccommend using [Ninja](https://ninja-build.org/) which works on Windows, Linux, and Mac as a CMake generator.

A Test Flight
-------------
Offering a little taste of the lang, here is some syntax. A hello world program is as follows:
```zig
print("Hello, World!")
```
Fibonacci Implemented in the lang would be as follows:
```zig
fib => (n: i32) i32 {
  return match n {
    0 -> 0,
    1 -> 1,
    n -> fib(n - 1) + fib(n - 2)
  };
}
```
The language will be using advanced type-inference in order to limit the amount of static typing that is needed. Jet is by default, statically-typed, however, types are
usually inferred by the langugage most of the time, and generate clean and easy to read code. This way we get to appeal to both those who like static typing and those that
prefer mainly dynamic typing.

### Error Messages
Jet has **amazing** error messages. Even for simple error, Jet makes sure that the programmer knows the error and why their code
is causing the error is invalid. For example, look at a lexer error message pertaining to an **integer overflow**:

![](https://github.com/The-NextGen-Project/jet/blob/main/.github/LexErrorExample.png)

The error message points out the initial error, points out the offending line and character, gives a potential solution, and explains
why what the programmer wrote was invalid. These components identify the problem and give opportunities for a solution to help the 
programmer debug and fix their error. We took inspiration from both Rust and Elm to produce a hybrid error message type that 
encapsulates all that the programmer needs to identify the problem and solve it.

Let's have a look at another example, for an **unexpected token in the global scope**:

![](https://github.com/The-NextGen-Project/jet/blob/main/.github/ParseErrorExample1.png)

People, especially beginners, do not understand where and why they are getting errors. Written errors like this one help provide clarity
and provide potential solutions to a problem they may not understand. By telling the programmer what is directly valid, it prevents programmers to lookup documentation
unless it is necessary to explain an error. 


Progress
----------
- [x] Lexer 
- [x] Parser 
- [x] Semantic Analysis (**WIP**)
- [ ] IR-Gen
- [ ] IR-Optimization
- [ ] C-Codegen (**WIP**)
- [x] x64 Code Generation (**Testing Phase In Progress**)
- [ ] ARM64 Code Generation (**WIP**)
- [ ] Link

Contributing
-------------
We are very excited that you have considered contributing to Jet as part of the NextGen Project. This project is a community effort
and we would love to have many of you who are willing to help. Before your start your wonderful contribution, have a look at our
[Contributing Guidelines](https://github.com/The-NextGen-Project/jet/blob/main/CONTRIBUTING.md).

License
----------
We want the project to be able to integrate with other with a lot of other open source software and determined that
we will be using the **MIT License**.
