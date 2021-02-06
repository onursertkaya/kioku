from dataclasses import dataclass
from pathlib import Path
from typing import List, Tuple

from tools.build_system.build_config import BuildConfig
from tools.build_system.code_util import REPO_ROOT, calculate_checksum, get_all_sources
from tools.build_system.dependencies import Dependencies
from tools.build_system.include_resolution import IncludedHeaders
from tools.build_system.module_organization import Inapplicable, ModuleOrganization
from tools.build_system.source_resolution import SourceType, resolve_source_file_type
from tools.build_system.typing import PathString, StringList


@dataclass(frozen=True)
class Target:
    """A compilable translation unit."""

    source_file: PathString
    header_file: PathString  # TODO: or None! in case of tests

    external_includes: StringList
    internal_includes: StringList
    include_checksums: StringList

    @property
    def name(self) -> Path:
        base_stripped = str(Path(self.source_file).relative_to(Path(REPO_ROOT)))
        return Path(base_stripped.replace("/", "-"))

    @property
    def all_includes(self) -> StringList:
        return self.external_includes + self.internal_includes

    @property
    def source_checksum(self):
        return calculate_checksum(self.source_file)

    @property
    def source_type(self) -> SourceType:
        return resolve_source_file_type(self.source_file)

    @property
    def includepath_statement(self):
        return self._module_organization.includepath(self.header_file)

    @property
    def _module_organization(self) -> ModuleOrganization:
        if (self.source_type == SourceType.TEST) or (
            self.source_type == SourceType.MAIN
        ):
            return Inapplicable
        else:
            return ModuleOrganization.determine(self.source_file, self.header_file)

    def __str__(self) -> str:
        """Helper for directly printing a target object with a nice format."""
        print(f"- {self.name}")
        for k, v in self.__dict__.items():
            print(f"\t- {k}: {v}")
        return ""

    def make_objfile_path(self, build_directory: PathString) -> Path:
        return Path(build_directory) / self.name.with_suffix(".o")

    def make_executable_path(self, build_directory: PathString) -> Path:
        assert (
            self.source_type != SourceType.SRC
        ), f"{self.name} can not be an executable."
        return Path(build_directory) / self.name.with_suffix("")

    def checksums_match(self, other: "Target") -> bool:
        """Compare both file checksums and included files' checksums."""
        assert (
            self.name == other.name
        ), "Target names must match in order to compare checksums."
        return self.source_checksum == other.source_checksum and set(
            self.include_checksums
        ) == set(other.include_checksums)


class TargetExploration:
    """Explore compilable files in the repo based on the given config."""

    def __init__(self, build_config: BuildConfig):
        self.target_root = build_config.target_directory

        self.build_path = build_config.build_directory
        self.explore_tests = build_config.test

        self.dependencies = Dependencies(build_config.thirdparty_dep_directory)

    def _create_target_from_source_file(self, source_file: PathString) -> Target:
        includes = IncludedHeaders.get(source_file, self.dependencies)

        include_checksums = list(
            map(calculate_checksum, includes.internal + includes.external)
        )

        return Target(
            source_file=source_file,
            header_file=includes.own,
            external_includes=includes.external,
            internal_includes=includes.internal,
            include_checksums=include_checksums,
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
