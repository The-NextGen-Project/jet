# Contributing

👍 Thanks for considering to contribute to the project, we appreciate it. 👍

Outline
-----------------

* [Code of Conduct](https://github.com/The-NextGen-Project/jet/blob/main/CODE_OF_CONDUCT.md)
* [Spread the Word About Us](#spread-the-word)
* [Add your own Feature](#add-your-own-features)
* [Code Style Guidelines](#style-guidelines)
* [Commit Style Guidelines](#committing-guidelines)
* [Bug Reports](https://github.com/The-NextGen-Project/jet/blob/main/.github/ISSUE_TEMPLATE/jet----bug-report.md)
* [Pull Requests](#pull-requests)
* [Licensing](#licensing)

Code of Conduct.
----------------
Before starting the rest of the process, please take time read and understand the contributing
[Code of Conduct](https://github.com/The-NextGen-Project/jet/blob/main/CODE_OF_CONDUCT.md).


Spread the Word
---------------

Another way to contribute is to write about Jet and the NextGen Project, or speak about Jet on
some online forums to encourage a friendly discussion. The language is at its early stages, so 
we want as much exposure as possible. We have currently have a channel on the [Programming Language Discord Server](https://discord.gg/tft9Nw5jm6)
where we discuss new changes and possible integrations with new technologies.

Add your own Features
---------------------
Want to add amazing language features yourself? Submit your 
own language syntax ideas [here](https://github.com/The-NextGen-Project/jet/issues) and if 
selected, the syntax/feature will be named after you! (or an appropriate name of your choosing).

> **Note:** Existing Language Features/Syntax suggested will not be named after you. We love suggestions
> of new features of the language, but please be mindful to see if your suggestion has been already been
> accepted or an existing or planned language [feature](https://github.com/The-NextGen-Project/jet/blob/main/LANG.md).

Style Guidelines
----------------
All programs under the NextGen Project follow the following C++ Style Guidlines.

* Getter and Setter functions are to be named using `camelCase`
* Functions (including in namespaces and in classes) should be denoted with `MyFunction` notation
* Class and Struct member **variables** should be deonoted with `MyVariable`
* Don't be *too* generous with your comments. Use them appropriately, especially when describing a complicated task
* Preprocessors have a `\t` between the '#'. For example: `#  include <iostream>`. This is done to be consistant with config
* Lines should be no longer than **80 characters**. We like to limit this line length for readibility
* Mark implicit constructors with `/*implicit*/` indication
* Class and Struct names in-general should be denoted with `MyClass` notation unless we follow specific format guidlines with a library
* Use the `// STD NAMING` convention appropriately. This means that we are to use only `snake_case` when naming items in that scope, for example,
 a class with that name requires all its methods and variables to be denoted as `snake_case`.

When writing nested namespace use the following setup to be consistent with other files:
```c++
namespace A { namespace B { namespace C
{
    // ...
}}}
```

> **Note:** There are a few exceptions to the rules above: If we create an STL-like object, keep styling
> based on STL styling, and just make sure to use common sense. If something has a very long typename and 
> has better readibility using trailing return type, use it, **prioritize readibility** over anything else.

Committing Guidelines
---------------------
All commits to the project's repository are follow the following Guidelines.

* Please limit your commit messages to no more than **10** words
* Start all of your commit messages with a capital letter after the designated icon (see below)
* **Don't** end your commiting messages with `.`
* Please use the following icons before your commit message to indicate changes made:
    * :art:               `:art:`               when improving the format/structure of the code
    * :heavy_plus_sign:   `:heavy_plus_sign:`   when adding new features to existing code
    * :zap:               `:zap:`               when improving performance
    * :book:              `:book:`              when writing docs
    * :penguin:           `:penguin:`           when updating portability
    * :bug:               `:bug:`               when fixing a bug
    * :fire:              `:fire:`              when source files are moved, changed, or deleted
    * :wrench:            `:wrench:`            when fixing build system (CMake, CI)
    * :white_check_mark:  `:white_check_mark:`  when working on tests

Pull Requests
---------------
Pull Requests are pretty straightforward. You have a change to the existing source code, great :), now you want create
a pull request directly. If workflow succeeds and code is checked by the team, then you are ready to 
merge into the branch you requested for. 


Licensing
----------
As a contributor, any changes you contribute are to be done under the terms of the project's license, unless otherwise specified.
