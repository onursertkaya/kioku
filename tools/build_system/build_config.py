from dataclasses import dataclass, field
from pathlib import Path


@dataclass(frozen=True)
class BuildConfig:

    debug: bool = False
    compiler: str = None
    optimize: bool = False
    cpp_standard: str = None
    build_directory: Path = None
    target_directory: str = None
    test: bool = False
    thirdparty_dep_directory: Path = Path()

    # kioku-TIL:
    #     Exclude an attribute from the list of equality
    #     comparison (__eq__), with the flag `compare = False`
    #
    # Use-case Rationale:
    #     When deciding on the list of changed files, two instances
    #     of BuildConfig are compared: one generated for current
    #     compilation and one coming from the previous build's cache
    #     file.
    #     If force_build is requested, we ignore the cache file content
    #     and compile all the source from scratch, however we still
    #     would like the next non-force build to benefit from the cache
    #     regardless of this build's "forced status".
    #     Therefore when performing the equality check, we don't include
    #     `force_build` in the comparison, rather we explicitly check
    #     for it in a separate condition.
    #
    #     See `class CacheState` in `cache.py` for implementation.
    force_build: bool = field(default=False, compare=False)
