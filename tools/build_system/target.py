from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Set

from tools.build_system.build_config import BuildConfig
from tools.build_system.code_util import REPO_ROOT, calculate_checksum, get_all_sources
from tools.build_system.dependencies import Dependencies
from tools.build_system.include_resolution import IncludedHeaders
from tools.build_system.module_organization import Inapplicable, ModuleOrganization
from tools.build_system.source_resolution import SourceType, resolve_source_file_type
from tools.build_system.typing import OptPathString, PathString, StringList


@dataclass(frozen=True)
class Target:
    """A compilable translation unit."""

    source_file: OptPathString
    includes: IncludedHeaders
    source_checksum: str
    include_checksums: Set[str]

    @classmethod
    def make(cls, source_file: OptPathString, includes: IncludedHeaders):
        assert source_file
        return cls(
            source_file=source_file,
            includes=includes,
            source_checksum=calculate_checksum(source_file),
            include_checksums=(
                set(map(calculate_checksum, includes.all)) if includes.all else set()
            ),
        )

    @property
    def name(self) -> Path:
        base_stripped = str(Path(self.source_file).relative_to(Path(REPO_ROOT)))
        return Path(base_stripped.replace("/", "-"))

    @property
    def source_type(self) -> SourceType:
        return resolve_source_file_type(self.source_file)

    @property
    def own_includepath_statement(self) -> str:
        module_org = (
            Inapplicable
            if (self.source_type == SourceType.TEST)
            or (self.source_type == SourceType.MAIN)
            else ModuleOrganization.determine(self.source_file, self.includes.own)
        )

        return module_org.includepath(self.includes.own)

    @property
    def internal_dependencies_includepath_statements(self) -> StringList:
        return [
            ModuleOrganization.determine(None, internal_include).includepath(
                internal_include
            )
            for internal_include in self.includes.internal
        ]

    def __str__(self) -> str:
        """Helper for directly printing a target object with a nice format."""
        string = f"- {self.name}\n"
        for k, v in self.__dict__.items():
            if isinstance(v, IncludedHeaders):
                string += f"\t- {k}:\n\t\t{v}"
            else:
                string += f"\t- {k}: {v}\n"
        return string

    def make_objfile_path(self, build_directory: PathString) -> Path:
        return Path(build_directory) / self.name.with_suffix(".o")

    def make_executable_path(self, build_directory: PathString) -> Path:
        assert (
            self.source_type != SourceType.SRC
        ), f"{self.name} can not be an executable."
        return Path(build_directory) / self.name.with_suffix("")

    def checksums_match(self, other: Target) -> bool:
        """Compare both file checksums and included files' checksums."""
        assert (
            self.name == other.name
        ), "Target names must match in order to compare checksums."

        source_match = self.source_checksum == other.source_checksum
        includes_match = self.include_checksums == other.include_checksums

        return source_match and includes_match


class TargetExploration:
    """Explore compilable files in the repo based on the given config."""

    def __init__(self, build_config: BuildConfig):
        self.target_root = build_config.target_directory

        self.build_path = build_config.build_directory
        self.explore_tests = build_config.test

        self.dependencies = Dependencies(build_config.thirdparty_dep_directory)

    def _create_target_from_source_file(self, source_file: PathString) -> Target:
        includes = IncludedHeaders.get(source_file, self.dependencies)

        return Target.make(
            source_file=source_file,
            includes=includes,
        )

    def scan_targets(self) -> List[Target]:
        targets = []
        target_source_files = [f for f in get_all_sources() if self.target_root in f]
        for source_file in target_source_files:
            targets.append(self._create_target_from_source_file(source_file))

        return targets

    def scan_shared_object_libs(self):
        raise NotImplementedError

    def scan_static_libs(self):
        raise NotImplementedError
