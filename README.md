# Kioku

Kioku (*to remember* in Japanese) is a repository to implement some of
the algorithms and concepts I find interesting, important and worth
remembering.

## Rationale

The aim is to expose myself to various programming and software
engineering problems as much as possible, in order to grow and develop.
The effort is not for reinventing the wheel, therefore the code is far
from looking like a perfect circle. I'm happy as long as it keeps me
moving forward.

Some files contain the statement `kioku-TIL` (Today I Learned), where I
tried to document the obstacles, pitfalls and issues I've encountered as
well as the solutions to them. This can be anything from the correct
usage of an STL type to an algorithm, as well as unexpected issues such
as weird behavior caused by compiler optimization flags.

## Build System

The repository has a home-grown build system with the same name, which
involves python scripting, globbing, subprocessing, regular expressions
and containers. Most of the time it's barely functioning and far from
perfect, however as it does not require any build configuration files
(`*.cmake`, `*.bazel` etc.) it achieves features such as automatic
include resolution and caching, with implementations from scratch. It
was a fun project to work with!
See [build system document](tools/build_system/readme.md) for details.

## Code Review Pitfall

When one both writes and reviews the code, it can be challenging to
strictly maintain code quality. On the bright side, this challenge
forces sticking to "Test Driven Development" even more, which becomes
something more than a best practice over time!

Ironically, it can be hard to keep track of what happens at which side
of the repository. As development of this repo is a spare-time effort
with limited allocated time, one can easily forget chunks of
commented-out code here and there, which might slip in the next commit.
There's nobody to check, complain or fix!

## Long Term Plan

My plan for personal development consists of levels of studies, i.e.

- Level-1
  - Linear Algebra
  - Probability, Statistics & Stochastic Processes
  - Algorithms & Data Structures

- Level-2
  - Machine Learning
  - Image Processing & Computer Vision
  - Domain Algorithms (Control, Signal Processing, Classical AI)

- Level-3
  - Deep Learning

This is reflected in the repository as:

- Level-1: `src/core`
- Level-2: `src/projects`
- Level-3: N/A, planned for another dedicated repository.
