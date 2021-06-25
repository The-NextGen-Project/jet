![Jet](https://github.com/The-NextGen-Project/jet/blob/main/.github/JETV4Large3.png)
[![GitHub license](https://img.shields.io/github/license/The-NextGen-Project/jet?color=%2320ba37&label=License&logo=%2320ba37&logoColor=%2320ba37&style=for-the-badge)](https://github.com/The-NextGen-Project/jet/blob/main/LICENSE) 
[![Linux](https://img.shields.io/github/workflow/status/The-NextGen-Project/jet/Linux?label=Linux&style=for-the-badge&logo=github)]()
[![Mac](https://img.shields.io/github/workflow/status/The-NextGen-Project/jet/MacOS?label=MacOS&style=for-the-badge&logo=github)]()
[![Windows](https://img.shields.io/github/workflow/status/The-NextGen-Project/jet/Windows?label=Windows&style=for-the-badge&logo=github)]()
#### Fast Language for Developing Consistent Native, Mobile App, and Web Applications.
> **NOTE:** The language is in its early development stages. Progress and goals for the language will be updated accordingly on Github, be sure to look out for updates.
> While quite a lot of syntax and features have been planned out, they are subject to rapid change and removal, please be considerate as the feedback we receive from the
> community may influence some of our syntax. 

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
fn fib(n) {
  return n.match {
    0 or 1 => n
    . => fib(n-1) + fib(n-2)
  }
}
```
The language will be using advanced type-inference in order to limit the amount of static typing that is needed. Jet is by default, statically-typed, however, types are
usually inferred by the langugage most of the time, and generate clean and easy to read code. This way we get to appeal to both those who like static typing and those that
prefer mainly dynamic typing.

### Error Messages
Jet has **amazing** error messages. Even for simple error, Jet makes sure that the programmer knows the error and why their code
is causing the error is invalid. For example, look at a lexer error message pertaining to an **integer overflow**:

![](https://github.com/The-NextGen-Project/jet/blob/main/.github/LexErrorExample1.png)

The error message points out the initial error, points out the offending line and character, gives a potential solution, and explains
why what the programmer wrote was invalid. These components identify the problem and give opportunities for a solution to help the 
programmer debug and fix their error. We took inspiration from both Rust and Elm to produce a hybrid error message type that 
encapsulates all that the programmer needs to identify the problem and solve it.

Let's have a look at another example, for an **invalid string escape**:

![](https://github.com/The-NextGen-Project/jet/blob/main/.github/LexErrorExample2.png)

Beginners learning to program may not know what is valid in complicated escapes like a hexadecimal escape, therefore we cater to beginners
offering exclusive and clear error messages. By telling the programmer what is directly valid, it prevents programmers to lookup documentation
unless it is necessary to explain an error. 


Progress
----------
- [x] Lexer (with edge cases handled later)
- [ ] Parser
- [ ] Semantic Analysis
- [ ] Lower-AST
- [ ] IR-Gen
- [ ] IR-Optimization
- [ ] Code Generation
- [ ] C-Interop
- [ ] Code Generation (JS)
- [ ] Code Generation (WASM)
- [ ] Standard Library
- [ ] Python Integration

Contributing
-------------
We are very excited that you have considered contributing to Jet as part of the NextGen Project. This project is a community effort
and we would love to have many of you who are willing to help. Before your start your wonderful contribution, have a look at our
[Contributing Guidelines](https://github.com/The-NextGen-Project/jet/blob/main/CONTRIBUTING.md).

License
----------
We want the project to be able to integrate with other with a lot of other open source software and determined that
we will be using the **MIT License**.
