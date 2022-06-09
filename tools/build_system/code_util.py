#!/usr/bin/python3
import hashlib
import subprocess
from functools import lru_cache
from pathlib import Path

from tools.build_system.constants import (
    HEADER_EXTENSIONS,
    PY_EXTENSION,
    SOURCE_EXTENSIONS,
)
from tools.build_system.typing import PathString, StringList

REPO_ROOT = (
    subprocess.check_output(["git", "rev-parse", "--show-toplevel"])
    .decode("utf-8")
    .strip()
)


def get_commit_hash():
    return subprocess.check_output(["git", "rev-parse", "HEAD"]).decode("utf-8").strip()


def calculate_checksum(file: PathString):
    with open(file) as f:
        file_content = f.read()
    return hashlib.md5(file_content.encode("utf-8")).hexdigest()


@lru_cache(maxsize=None)
def _scan_with_extensions(directory: Path, extensions: StringList) -> StringList:
    """Scan all files with extensions, starting from directory."""

    retval = []
    for ext in extensions:
        assert "." not in ext
        retval.extend(sorted([str(match) for match in directory.rglob("*." + ext)]))
    return retval


def _filter_by_extension(file_list: StringList, extension: str) -> StringList:
    """Given a file list, return a subset where file extensions match with extension."""
    return [f for f in file_list if f.endswith(extension)]


@lru_cache(maxsize=1)
def get_all_headers():
    all_cpp_files = _scan_with_extensions(
        Path(REPO_ROOT), HEADER_EXTENSIONS + SOURCE_EXTENSIONS
    )
    retval = []
    for header_ext in HEADER_EXTENSIONS:
        retval.extend(_filter_by_extension(all_cpp_files, header_ext))
    return retval


@lru_cache(maxsize=1)
def get_all_sources():
    all_cpp_files = _scan_with_extensions(
        Path(REPO_ROOT), HEADER_EXTENSIONS + SOURCE_EXTENSIONS
    )
    retval = []
    for src_ext in SOURCE_EXTENSIONS:
        retval.extend(_filter_by_extension(all_cpp_files, src_ext))
    return retval


@lru_cache(maxsize=1)
def get_all_py_files():
    return _scan_with_extensions(Path(REPO_ROOT), (PY_EXTENSION,))


@lru_cache(maxsize=None)
def get_system_include_paths(compiler: str):
    result = subprocess.run(
        [compiler, "-xc++", "-E", "-v", "/dev/null"],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )

    start = False
    paths = []
    for line in result.stderr.decode("utf-8").strip().split("\n"):
        if line == "#include <...> search starts here:":
            start = True
            continue

        if start:
            if line == "End of search list.":
                break
            paths.append(str(Path(line.strip()).resolve()))

    return paths
