#!/usr/bin/python3
from pathlib import Path

from tools.build_system.build_config import BuildConfig
from tools.build_system.cache import Cache
from tools.build_system.dependencies import Dependencies, Dependency
from tools.build_system.fancy import (
    MessageType,
    fancy_print,
    fancy_run,
    fancy_separator,
)
from tools.build_system.target import SourceType, Target, TargetExploration

# todo,
# parallel build
# in-line log formatting (\r\n)

# todo: strip out compiler, build directory, objfile creation and linker
#       as separate small classes.


class Builder:

    BIN_DIR = "bin"
    OBJ_DIR = "obj"
    SO_DIR = "so"
    TEST_DIR = "test"
    BUILD_DIR = [BIN_DIR, OBJ_DIR, SO_DIR, TEST_DIR]

    def __init__(self, config: BuildConfig):
        self._config = config
        self._create_build_dir()

        # Prepare third party dependencies for compiler and linker.
        self.deps = Dependencies(self._config.thirdparty_dep_directory)

        # Explore targets from current state of the repo.
        target_explorer = TargetExploration(self._config)
        self._targets = target_explorer.scan_targets()

        # Read and update the cache, then compare with current targets to extract a build list.
        self.changelist = Cache(self._config).get_target_changelist(self._targets)

    def _create_build_dir(self):
        if self._config.build_directory:
            for dir_ in self.BUILD_DIR:
                Path(self._config.build_directory / dir_).mkdir(
                    exist_ok=True, parents=True
                )

    def _query_internal_target_by_header(self, header: str) -> Target:
        return next(filter(lambda t: t.header_file == header, self._targets))

    def _query_external_target_by_header(self, header: str) -> Dependency:
        return next(
            filter(
                lambda d: str(self.deps.path / d.header_relpath) == header,
                self.deps.get_list,
            )
        )

    def _query_all_includepaths(self, target: Target) -> set:
        includepaths = set()
        for included_header in target.internal_includes:
            includepaths.add(
                self._query_internal_target_by_header(
                    included_header
                ).includepath_statement
            )

        for included_header in target.external_includes:
            dep = self._query_external_target_by_header(included_header)
            includepaths.add(f"{dep.make_includepath_statement(self.deps.path)}")

        return includepaths

    def _assemble_compile_command(self, target: Target):
        includepaths = self._query_all_includepaths(target)

        flags = [
            f"-W{flag}"
            for flag in ("all", "error", "extra", "pedantic", "no-missing-braces")
        ]
        cpp_standard = f"-std=c++{self._config.cpp_standard}"
        output_path = str(
            target.make_objfile_path(self._config.build_directory / self.OBJ_DIR)
        )
        cmd = [
            self._config.compiler,
            "-o",
            output_path,
            "-c",
            cpp_standard,
            # todo: add fPIC / fPIE when .so file is requested.
            *flags,
            *includepaths,
            target.source_file,
        ]

        if self._config.debug:
            cmd.append("-ggdb3")
        if self._config.optimize:
            cmd.append("-O3")
        # TODO: -O3 is not added to the tests, figure out why.

        return cmd

    def _compile_target(self, target: Target):
        compile_cmd = self._assemble_compile_command(target)
        fancy_separator()
        # todo: invalidate cache entry for this target, if compilation fails.
        fancy_run(compile_cmd, f"Compilation of target {target.name} failed.")

    def build_translation_units(self, also_build_tests: bool, parallel: bool = False):
        target_list = (
            self.changelist
            if also_build_tests
            else filter(lambda x: not x.source_type == SourceType.TEST, self.changelist)
        )

        for target in target_list:
            self._compile_target(target)

        fancy_print(
            "All compilation targets are up-to-date.",
            msg_type=MessageType.SUCCESS,
            flash=True,
        )

    def _assemble_link_command(self, target: Target):
        """Assemble command line arguments for linking a target.

        Precondition:
            target must be of type SourceType.{MAIN, TEST}."""
        out_subdir = (
            self.BIN_DIR if target.source_type == SourceType.MAIN else self.TEST_DIR
        )

        out_executable_path = target.make_executable_path(
            self._config.build_directory / out_subdir
        )

        objfiles_dir_path = self._config.build_directory / self.OBJ_DIR
        out_objfile_path = target.make_objfile_path(objfiles_dir_path)

        # Gather internal object files.
        object_files_to_be_linked = set()
        for included_header in target.internal_includes:
            object_files_to_be_linked.add(
                str(
                    self._query_internal_target_by_header(
                        included_header
                    ).make_objfile_path(objfiles_dir_path)
                )
            )

        libs_statement = []
        for included_header in target.external_includes:
            library_path, libraries = self._query_external_target_by_header(
                included_header
            ).make_objfiles_dir_and_list(self.deps.path, self._config.debug)
            library_path = f"-L{library_path}"
            libraries = [
                f'-l{lib.lstrip("lib").rstrip(".a").rstrip(".so")}' for lib in libraries
            ]

            libs_statement.append(library_path)
            libs_statement.extend(libraries)

        link_cmd = [
            self._config.compiler,
            "-o",
            str(out_executable_path),
            str(out_objfile_path),
            *object_files_to_be_linked,
            *libs_statement,
            "-pthread",
        ]

        return link_cmd

    def link_translation_units(self, also_link_tests: bool):
        # filter out source modules, only keeping tests and executables
        # with main entrypoints.
        target_list = filter(
            lambda x: x.source_type != SourceType.SRC,
            self.changelist,
        )

        # furthermore, filter out tests if not requested.
        target_list = (
            target_list
            if also_link_tests
            else filter(lambda x: not x.source_type == SourceType.TEST, target_list)
        )

        for target in target_list:
            link_cmd = self._assemble_link_command(target)
            fancy_separator()
            fancy_run(
                link_cmd, error_message=f"Linkage of target {target.name} failed."
            )

        fancy_print(
            "All executable targets are up-to-date.",
            msg_type=MessageType.SUCCESS,
            flash=True,
        )

    def build(self):
        self.build_translation_units(self._config.test)
        self.link_translation_units(self._config.test)
