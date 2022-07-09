"""Utilities to represent and determine source file type."""
import re
from enum import Enum

from tools.build_system.constants import (
    CPP_INCLUDE_STR,
    HEADER_EXTENSIONS,
    SOURCE_EXTENSIONS,
)
from tools.build_system.fancy import MessageType, fancy_print
from tools.build_system.typing import PathString

CPP_MAIN_STR = "main"
CPP_MAIN_RETURN_TYPES = ("int", "void")
MATCHING_FUNC_PARENTHESIS_REGEXP = r"\(.*\)"
GTEST_TEST_MACROS = ("TEST", "TEST_F")


class SourceType(Enum):
    """Enumeration to represent source file type."""

    SRC = 0
    MAIN = 1
    TEST = 2

    class UnknownSourceType(Exception):
        """Exception to be raised when a source type cannot be determined."""

    class InvalidSourceExtension(Exception):
        """Exception to be raised when a source extension is not valid."""


def _is_main_entrypoint(source_file_path: PathString) -> bool:
    """Determine if a source file contains a main entrypoint."""
    with open(source_file_path, "r") as f:
        re_query = f"^({'|'.join(CPP_MAIN_RETURN_TYPES)}) {CPP_MAIN_STR}{MATCHING_FUNC_PARENTHESIS_REGEXP}"
        for line in f.read().splitlines():
            if re.match(re_query, line):
                return True
    return False


def _is_gtest_file(source_file_path: PathString) -> bool:
    """Determine if a source file is a googletest file."""
    with open(source_file_path, "r") as f:
        for line in f.read().splitlines():
            re_query = (
                f"^({'|'.join(GTEST_TEST_MACROS)}){MATCHING_FUNC_PARENTHESIS_REGEXP}.*"
            )
            gtest_candidate = re.match(re_query, line)
            if gtest_candidate:
                if "test" not in str(source_file_path):
                    fancy_print(
                        f"The file {source_file_path} seems to be a googletest file,"
                        + " "
                        + "however it does not have the prefix test_* in the filename."
                        + " "
                        + "Having this format is suggested as it helps kioku to resolve"
                        + " "
                        + "dependencies.",
                        MessageType.WARNING,
                    )
                return True
    return False


def _is_regular_translation_unit(source_file_path: str) -> bool:
    """Determine whether source file is a non-main, non-test file.

    Assumes at least one include statement is present in the file.
    """
    with open(source_file_path, "r") as f:
        for line in f.read().splitlines():
            re_query = f'^{CPP_INCLUDE_STR}.*({"|".join(HEADER_EXTENSIONS)})"$'
            regular_src_file_candidate = re.match(re_query, line)
            if regular_src_file_candidate:
                return True
    return False


def resolve_source_file_type(source_file_path: PathString) -> SourceType:
    """Resolve a source file type based on file content."""

    if not any([str(source_file_path).endswith(ext) for ext in SOURCE_EXTENSIONS]):
        raise SourceType.InvalidSourceExtension(
            f"Invalid file extension for the file {source_file_path} as a target."
        )
    # todo, the call order below matters as is_regular_translation_unit just checks
    # for regular includes.
    if _is_gtest_file(source_file_path):
        return SourceType.TEST
    elif _is_main_entrypoint(source_file_path):
        return SourceType.MAIN
    elif _is_regular_translation_unit(source_file_path):
        return SourceType.SRC
    else:
        raise SourceType.UnknownSourceType(
            f"Could not determine the target type of the file: {source_file_path}"
        )
