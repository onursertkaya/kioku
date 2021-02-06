# Backlog

A messy document for "personal use".

## fixme

- don't use init_list in ctors, anywhere. use variadics and/or container/array types instead.

- clean up `kioku.py`'s argparse.

- add/convert `const` and `constexpr` where needed, as much as possible.

- utilize `noexcept`, `nothrow` and `nodiscard`.

- "last build succeded" flag per target: revise how cache is written, i.e. write even if error
  occurs for other targets.

- build system pytests

- tests "pass" if built stuff is loaded from cache. fix it.

- when -t is used, targets that are outside of the passed directory are not built.

## todo

### ideas

- A job script to scan the statement `kioku-TIL` throughout the repository
  comments and generate a summary document. It will also scan for `todo`
  and `fixme` statements for standing bugs, calls for better
  implementations and new ideas.

- Doxygen job and inline documentation

### topics

- math
  - dot
  - cross
  - elemwise ops
  - conv & filtering (laplacian, gaussian)
  - quaternions
  - lerp

- algo
  - search
  - sort
  - single/double linked-lists
  - "advanced" dstructs
    - hash table
    - queue
    - tree(s)

- metaheuristics
  - genetic algorithm
  - evolutionary strategies
  - bee colony
  - ant colony
  - ...

- ml
  - linear regression on real datasets. check kaggle for inspiration.
  - logistic reg
  - dec tree
  - forest

- signal processing
  - fft
  - kalman filter

- base stuff
  - logging
  - assertions
  - dockerization of everything

- os & hardware interfacing
  - file i/o, trivial construct->serialization etc
  - sockets
  - shmem
  - pipes
  - audio
  - GL/video
  - cuda
  - usb comms
  - vectorization, avx & sse
  - ...

- patterns
  - pimpl
  - abstract factory
  - factory
  - singleton
  - ...

- static analysis & linting
  - cpp, clang toolchain
  - pylint, pycodestyle, pydocstyle
  - python black formatter

- dynamic analysis
  - valgrind/callgrind
  - *san

- debugging
  - gdb, lldb

- profiling
  - cpp: gprof, perf, flamegraph
  - py: cprofile, memory-profiler

- monitoring
  - strace, ftrace
  - dstat, netcat, netstat
