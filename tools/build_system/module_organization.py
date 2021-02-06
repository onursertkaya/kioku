import abc
from pathlib import Path
from typing import Type

from tools.build_system.code_util import REPO_ROOT
from tools.build_system.typing import PathString


class ModuleOrganization(abc.ABC):
    """Base virtual class to represent how a C++ module is organized."""

    SOURCES_DIR = "src"
    INCLUDES_DIR = "include"
    INCLUDEPATH_PREFIX = "-I"

    class InvalidOrganization(Exception):
        """Exception to be raised when a source/header organization is not allowed."""

    @abc.abstractstaticmethod
    def includepath(header_file: PathString) -> str:
        pass

    @staticmethod
    def determine(
        source_file: PathString, header_file: PathString
    ) -> Type["ModuleOrganization"]:
        src_path = Path(source_file)
        hdr_path = Path(header_file)

        src_dir = src_path.parent
        hdr_dir = hdr_path.parent
        src_name = src_path.name
        hdr_name = hdr_path.name
        common = ""
        for char_hdr, char_src in zip(str(hdr_dir), str(src_dir)):
            if char_hdr == char_src:
                common += char_hdr
            else:
                break

        module_path = Path(common)
        module_name = module_path.name

        nested_header = (
            f"{module_path}/{ModuleOrganization.INCLUDES_DIR}/{module_name}/{hdr_name}"
        )
        nested_source = f"{module_path}/{ModuleOrganization.SOURCES_DIR}/{src_name}"

        at_base_header = f"{module_path}/{hdr_name}"
        at_base_source = f"{module_path}/{src_name}"

        if source_file == nested_source:
            if header_file == nested_header:
                return BothNested
            elif header_file == at_base_header:
                return RelativeNestedSource
        elif source_file == at_base_source:
            if header_file == nested_header:
                return RelativeNestedHeader
            elif header_file == at_base_header:
                return SameDirectory

        raise ModuleOrganization.InvalidOrganization


class SameDirectory(ModuleOrganization):
    """C++ header/source organization for unnested files.

    In this organization,
        `a/b/c.h`
    is included by
        `a/b/c.cpp`
    where the module name is b.
    """

    def includepath(header_file: PathString) -> str:
        return f"{ModuleOrganization.INCLUDEPATH_PREFIX}{Path(header_file).parent}"


class RelativeNestedHeader(ModuleOrganization):
    """C++ header/source organization for nested header files.

    In this organization,
        `a/b/include/b/c.h`
    is included by
        `a/b/c.cpp`
    where the module name is b.
    """

    def includepath(header_file: PathString) -> str:
        return f"{ModuleOrganization.INCLUDEPATH_PREFIX}{Path(header_file).parents[1]}"


class RelativeNestedSource(ModuleOrganization):
    """C++ header/source organization for nested source files.

    In this organization,
        `a/b/c.h`
    is included by
        `a/b/src/c.cpp`
    where the module name is b.
    """

    def includepath(header_file: PathString) -> str:
        return f"{ModuleOrganization.INCLUDEPATH_PREFIX}{REPO_ROOT}"


class BothNested(ModuleOrganization):
    """C++ header/source organization for nested files.

    In this organization,
        `a/b/include/b/c.h`
    is included by
        `a/b/src/c.cpp`
    where the module name is b.
    """

    def includepath(header_file: PathString) -> str:
        return f"{ModuleOrganization.INCLUDEPATH_PREFIX}{Path(header_file).parents[1]}"


class Inapplicable(ModuleOrganization):
    """Inapplicable header/source organization for test and main files."""

    def includepath(header_file: PathString) -> str:
        raise RuntimeError
