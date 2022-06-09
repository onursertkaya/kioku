from __future__ import annotations

import abc
from pathlib import Path
from typing import Type

from tools.build_system.code_util import REPO_ROOT
from tools.build_system.typing import OptPathString, PathString


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
        source_file: OptPathString, header_file: OptPathString
    ) -> Type[ModuleOrganization]:
        # todo: check type hint is correct, looks wrong.
        src_path = Path(source_file or "")
        hdr_path = Path(header_file or "")

        if src_path == Path():
            assert hdr_path != Path()
            return HeaderOnly

        src_dir = src_path.parent
        hdr_dir = hdr_path.parent
        src_name = src_path.name
        hdr_name = hdr_path.name
        common_substr = ""
        for char_hdr, char_src in zip(str(hdr_dir), str(src_dir)):
            if char_hdr == char_src:
                common_substr += char_hdr
            else:
                break

        module_path = Path(common_substr)
        module_name = module_path.name

        nested_header = (
            module_path / ModuleOrganization.INCLUDES_DIR / module_name / hdr_name
        )
        nested_source = module_path / ModuleOrganization.SOURCES_DIR / src_name

        at_base_header = module_path / hdr_name
        at_base_source = module_path / src_name

        if src_path == nested_source:
            if hdr_path == nested_header:
                return BothNested
            elif hdr_path == at_base_header:
                return RelativeNestedSource
        elif src_path == at_base_source:
            if hdr_path == nested_header:
                return RelativeNestedHeader
            elif hdr_path == at_base_header:
                return SameDirectory

        raise ModuleOrganization.InvalidOrganization


class HeaderOnly(ModuleOrganization):
    """C++ organization for header-only setup.

    In this organization there is no source file. Header is included by other
    modules.
    """

    @staticmethod
    def includepath(header_file: PathString) -> str:
        return f"{ModuleOrganization.INCLUDEPATH_PREFIX}{Path(header_file).parents[1]}"


class SameDirectory(ModuleOrganization):
    """C++ header/source organization for unnested files.

    In this organization,
        `a/b/c.h`
    is included by
        `a/b/c.cpp`
    where the module name is b.
    """

    @staticmethod
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

    @staticmethod
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

    @staticmethod
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

    @staticmethod
    def includepath(header_file: PathString) -> str:
        return f"{ModuleOrganization.INCLUDEPATH_PREFIX}{Path(header_file).parents[1]}"


class Inapplicable(ModuleOrganization):
    """Inapplicable header/source organization for test and main files."""

    @staticmethod
    def includepath(header_file: PathString) -> str:
        raise RuntimeError
