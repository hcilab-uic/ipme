# Interaction Proxemics in Multi-Device Ecologies

---
## [Project Internal Wiki](https://bitbucket.org/hcilab-uic/ipme/wiki/Home)

---
## A brief intro to the code base

### Coding conventions

Ever since the first release of C++ progrmming language in 1983, there
had been no recommended set of conventions when it came to naming and
layout. But that changed in September 2015
when
[Bjarne Stroustrup](https://en.wikipedia.org/wiki/Bjarne_Stroustrup)
announced
the
[C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md). These
are a set of guidelines recommended by the ISO C++ Standards committee
for easy, maintainable and safe C++ programming. In our code base we
will try and follow the recommendations of the committee to the extent
possible.

#### Naming

Since there was no recommended way of naming for 32 years (like
say [PEP-8](https://www.python.org/dev/peps/pep-0008/) for Python),
several styles emerged. There was the standard used by the standard
library (with underscores). Microsoft also popularized one style
with
[Hungarian notation](https://en.wikipedia.org/wiki/Hungarian_notation)
and UpperCamelCase function
names. [Google style guide](https://google.github.io/styleguide/cppguide.html) also
recommended this convention. This often became a matter of contention
between contributers as to what convention to follow.

With the availability of the core guidelines, we can now put that
debate to rest. So we will hereby follow the ISO standards committee
guideline to the extent possible. As a result you might notice strange
not so familiar syntaxes like 'My_awesome_class' for class names, but
that is what the standards recommend and hence shall follow.

#### Style

We use [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)
tool provided by
the [LLVM](http://llvm.org/)/[Clang](https://clang.llvm.org/)
community to enforce formatting of code. If you are hacking on this
codebase and interested in contributing, we recommend that you install
the clang-format tool and set it to read configuration in file mode. A
'.clang-format' file is already present at the root of the project and
hence, when you activate the tool, it will format the code base per
the requirements of rest of the code base with out ambiguity.

#### Build system

We use CMake for our build system. Bear in mind that cmake enforces an
out of source build.

#### Development & Testing

The code has been developed and tested in the following environments:
* OS: [Ubuntu 16.04 LTS](http://releases.ubuntu.com/16.04/),
  Compiler: [GCC 5.4.0](https://gcc.gnu.org/gcc-5/), Build
  System:
  [CMake 3.8.1](https://cmake.org/files/v3.8/cmake-3.8.1.tar.gz)

#### Support

For help, support, queries, shoot an email to <hnaik2@uic.edu>