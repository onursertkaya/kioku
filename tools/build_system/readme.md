# Kioku Build System

This document summarizes the home-grown build system of kioku.

## Why create a new build system?

Build configuration files are annoying. As long as well-defined rules
are followed, having a repository without any code except the actual
source should be possible. Kioku build system is the product of the
efforts spent on the exploration of this argument.

## Pros and Cons

> \+ Gained experience (which was the initial goal.)
>
> \+ More control on compilation and formatting toolchain.
>
> \- External library integration is not trivial. Each library has to be
> taken care of specially, from scratch.

## Build Output Directory

Kioku first compiles all the translation units and creates the object
files under `obj/`, then depending on the target it links required
object files to create either executables, shared object files or test
executables.

```bash
bin/  # Executable targets, compiled from main() sources.
obj/  # Object files with .o extension, these are then linked.
so/   # Shared object files with .so extension.
test/ # Test executable targets, compiled from gtest's TEST() macros.
```

## Module Source Directory Structure

See the class `ModuleOrganization` and its subclasses in `module_organization.py`.
