# Proxemics-Engine Suite

---
#### Build system

We use CMake for our build system. Bear in mind that cmake enforces an
out of source build.

#### Development & Testing

The code has been developed and tested in the following environments:

- OS: [Ubuntu 18.10 LTS](http://releases.ubuntu.com/18.10/),
- Compiler: [GCC 8.2.0](https://gcc.gnu.org/gcc-8/),
- Build System:
  [CMake 3.13.3](https://cmake.org/download/#latest)
- Ninja: [Ninja build 1.8.2](https://ninja-build.org/)

#### Developer Documentation

We use [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html)
tool provided by
the [LLVM](http://llvm.org/)/[Clang](https://clang.llvm.org/)
community to enforce formatting of code. If you are hacking on this
codebase and interested in contributing, we recommend that you install
the clang-format tool and set it to read configuration in file mode. A
'.clang-format' file is already present at the root of the project and
hence, when you activate the tool, it will format the code base per
the requirements of rest of the code base with out ambiguity.

#### Support

For help, support, queries, shoot an email to <hnaik2@uic.edu>
