import re
from dataclasses import dataclass
from pathlib import Path
from typing import Tuple

from tools.build_system.code_util import REPO_ROOT, get_all_headers
from tools.build_system.constants import CPP_INCLUDE_STR, HEADER_EXTENSIONS
from tools.build_system.dependencies import Dependencies
from tools.build_system.module_organization import ModuleOrganization
from tools.build_system.source_resolution import SourceType, resolve_source_file_type
from tools.build_system.typing import MaybeString, PathString, StringList


@dataclass(frozen=True)
class IncludedHeaders:
    """Full paths to included headers in a source file."""

    own: str
    internal: StringList
    external: StringList

    @classmethod
    def get(
        cls, source_file_path: PathString, dependencies: Dependencies
    ) -> "IncludedHeaders":
        """Get list of full paths to internal and external headers included in a source file."""
        own_header_candidates = []
        internal_includes_paths, external_includes_paths = [], []

        for include_statement in _parse_include_statements(source_file_path):
            found_own = _search_for_own_header(source_file_path, include_statement)
            if found_own:
                own_header_candidates.append(found_own)

            found_external = _search_for_external_headers(
                include_statement, dependencies
            )
            if found_external:
                external_includes_paths.append(found_external)

            found_internals = _search_for_internal_headers(include_statement)
            if found_internals:
                internal_includes_paths.extend(found_internals)

            if not found_own and not found_external and not found_internals:
                raise FileNotFoundError(
                    f"Inclusion {include_statement} in {source_file_path}"
                    + " "
                    + "is not found in the repository."
                )

        if len(own_header_candidates) == 0:
            # If a source file does not have a candidate, it is either a test or a main file.
            # Check if "test" or "main" is in the filename as a cheap operation.
            if not ("test" in str(source_file_path) or "main" in str(source_file_path)):
                # If there's no indication of being a test or main file, resolve by reading
                # the file content as a last resort.
                if resolve_source_file_type(source_file_path) == SourceType.SRC:
                    raise ModuleOrganization.InvalidOrganization(
                        f"{source_file_path} could not find its header."
                    )
        elif len(own_header_candidates) > 1:
            raise ModuleOrganization.InvalidOrganization(
                f"{source_file_path} has multiple candidates: {', '.join(own_header_candidates)}."
            )

        own_header = own_header_candidates[0] if own_header_candidates else None

        return cls(
            own_header,
            list(set(internal_includes_paths)),
            list(set(external_includes_paths)),
        )


def _parse_include_statements(source_file_path: PathString) -> StringList:
    headers = []
    with open(source_file_path, "r") as f:
        for line in f.read().splitlines():
            header_exts_regex_group = "(" + "|".join(HEADER_EXTENSIONS) + ")"
            include_candidate = re.match(
                f'^{CPP_INCLUDE_STR} ".*\.{header_exts_regex_group}"$', line
            )
            if include_candidate:
                header = include_candidate.string
                for substr_to_remove in [CPP_INCLUDE_STR, '"']:
                    header = header.replace(substr_to_remove, "")
                headers.append(header.strip())
    return headers


def _find_header_relpath_with_include_statement(include_statement: str) -> MaybeString:
    for header in get_all_headers():
        # check for substring
        if include_statement in header:
            return header
    return None


def _search_for_own_header(
    source_file_path: PathString, include_statement: str
) -> MaybeString:
    for ext in HEADER_EXTENSIONS:
        own_header_candidate = Path(source_file_path).with_suffix(f".{ext}").name
        # check for substring
        if own_header_candidate in include_statement:
            return _find_header_relpath_with_include_statement(include_statement)
    return None


def _search_for_internal_headers(include_statement: str) -> StringList:
    """Accumulate a list of headers by recursively searching all inclusions."""

    def collect_headers_recursively(header_file_path: str):
        inclusions = _parse_include_statements(header_file_path)
        for inc in inclusions:
            inc_relpath = _find_header_relpath_with_include_statement(inc)
            all_found_headers.add(inc_relpath)
            collect_headers_recursively(inc_relpath)

    all_found_headers = set()

    found_header = _find_header_relpath_with_include_statement(include_statement)
    if found_header:
        all_found_headers.add(found_header)
        collect_headers_recursively(found_header)

    return sorted(all_found_headers)


def _search_for_external_headers(
    include_statement: str, dependencies: Dependencies
) -> MaybeString:
    """Scan include statement in external dependencies' include statements."""
    deps = dependencies
    for dep in deps.get_list:
        # check for substring
        if include_statement in dep.include_statement:
            return str(deps.path / dep.header_relpath)
    return None
