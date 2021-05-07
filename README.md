# Jet ✈️
#### Turbo boost your code on any platform.

Table of Contents
-----------------

* [Download](#download)
* [Documentation](https://github.com/The-NextGen-Project/jet/blob/main/LANG.md)
* [About](#about)
* [Contributing](#contributing)
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

About
-------
### What is the NextGen Project? 
The NextGen Project serves to be the hub for creating and synthesizing new and old open source projects
into a unified toolchain under the Jet Programming Language. When I learned about the many new compilers
and languages being implemented, I wanted a piece of each one to be a part of something unified. 

### Why even attempt to create a unified language when there are so many existing ones designed for their platform?
Backends. It is not necessary to target every single architecture that currently exists *although thats what we want* but 
we can instead generate code for different backend tools. For example, the we can target the JDK and now can work on Android
applications. Integerate with LLVM and clang and work closely into developing IOS apps with Jet. Target WASM and JavaScript to 
work with those applications. In the end, we compile to the same transformed IL (Intermediate Language) that can output to our
current platform. Languages such as **Haxe** already attempt to do such a thing, but with nearly every sector being targetted by 
a large open source community, we should be able to reach a unification range where it is possible to integrate different technologies
and have them outputted by the compiler.

### How do the authors intend on gathering support?
We would first like to add our language to the [Programming Language Discord](#about) server and then proceed as the language gains gradual 
maturity. At the function stage, we would like to gather contributor and feature support. This is in the critical part of some of the language's 
core features that will be developed. This makes it quite important that everyone will have an oppurtunity to suggest and give ideas for the 
language.

Contributing
-------------
We are very excited that you have considered contributing to Jet as part of the NextGen Project. This project is a community effort
and we would love to have many of you who are willing to help. Before your start your wonderful contribution, have a look at our
[Contributing Guidelines](https://github.com/The-NextGen-Project/jet/blob/main/CONTRIBUTING.md).

License
----------
We want the project to be able to integrate with other with a lot of other open source software and determined that
this clause of the GPL will allow us to do so. 
The license of this project, unless specifically specified, is under the **GPLv2 or (at your option), any later version**.
