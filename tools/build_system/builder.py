"""C++ program builder module."""
from pathlib import Path
from typing import List

from tools.build_system.build_config import BuildConfig
from tools.build_system.cache import Cache
from tools.build_system.dependencies import Dependencies
from tools.build_system.fancy import (
    MessageType,
    fancy_print,
    fancy_run,
    fancy_separator,
)
from tools.build_system.target import SourceType, Target, TargetExploration
from tools.build_system.typing import StringList

# todo,
# parallel build
# in-line log formatting (\r\n)


class Builder:
    """C++ program builder."""

    BIN_DIR = "bin"
    OBJ_DIR = "obj"
    SO_DIR = "so"
    TEST_DIR = "test"
    BUILD_DIR = [BIN_DIR, OBJ_DIR, SO_DIR, TEST_DIR]

    def __init__(self, config: BuildConfig):
        """Create an instance."""
        self._create_build_dir(config)

        # Prepare third party dependencies for compiler and linker.
        deps = Dependencies(config.thirdparty_dep_directory)

        # Explore targets from current state of the repo.
        target_explorer = TargetExploration(config)
        self._targets = target_explorer.scan_targets()

        # Read and update the cache, then compare with current targets to extract a build list.
        self._changelist = Cache(config).get_target_changelist(self._targets)

        self._compiler = Compiler(config, deps)
        self._linker = Linker(config, deps)

    def build(self):
        """Build C++ programs and libraries based on requested config."""
        self._compiler.build_translation_units(self._changelist)
        self._linker.link_translation_units(self._changelist)

    @staticmethod
    def _create_build_dir(config: BuildConfig):
        if config.build_directory:
            for dir_ in Builder.BUILD_DIR:
                Path(config.build_directory / dir_).mkdir(exist_ok=True, parents=True)


class Compiler:
    """C++ program compiler.

    Converts source code to object code.
    """

    # pylint: disable=too-few-public-methods

    def __init__(self, config: BuildConfig, deps: Dependencies):
        """Create an instance."""
        self._config = config
        self._deps = deps

    def build_translation_units(self, changelist: List[Target], parallel: bool = False):
        """Build all translation units provided in the change list."""
        target_list = (
            changelist
            if self._config.test
            else filter(lambda x: not x.source_type == SourceType.TEST, changelist)
        )

        for target in target_list:
            self._compile_target(target)

        fancy_print(
            "All compilation targets are up-to-date.",
            msg_type=MessageType.SUCCESS,
            flash=True,
        )

    def _compile_target(self, target: Target):
        compile_cmd = self._assemble_compile_command(target)
        fancy_separator()
        # todo: invalidate cache entry for this target, if compilation fails.
        fancy_run(compile_cmd, f"Compilation of target {target.name} failed.")

    def _assemble_compile_command(self, target: Target) -> StringList:
        includepaths = self._query_all_includepaths(target)

        flags = [
            f"-W{flag}"
            for flag in ("all", "error", "extra", "pedantic", "no-missing-braces")
        ]
        cpp_standard = f"-std=c++{self._config.cpp_standard}"
        output_path = str(
            target.make_objfile_path(self._config.build_directory / Builder.OBJ_DIR)
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

    def _query_all_includepaths(self, target: Target) -> set:
        includepaths = set()

        if target.source_type == SourceType.SRC:
            includepaths.add(target.own_includepath_statement)

        includepaths.update(target.internal_dependencies_includepath_statements)
        includepaths.update(
            self._scan_external_targets_for_includepath_statements(target)
        )

        return includepaths

    def _scan_external_targets_for_includepath_statements(self, target: Target) -> set:
        external_includepaths = set()
        for external_header in target.includes.external:
            found_dep = self._deps.query_by_header(external_header)
            external_includepaths.add(
                found_dep.make_includepath_statement(self._deps._path)
            )
        return external_includepaths


class Linker:
    """C++ program linker.

    Converts object code files to executables or shared object files.
    """

    def __init__(self, config: BuildConfig, deps: Dependencies):
        """Create an instance."""
        self._config = config
        self._deps = deps

    def link_translation_units(self, changelist: List[Target]):
        """Link all the object files, sources of which were compiled in changelist."""
        # filter out source modules, only keeping tests and executables
        # with main entrypoints.
        target_list = filter(
            lambda t: t.source_type != SourceType.SRC,
            changelist,
        )

        # furthermore, filter out tests if not requested.
        target_list = (
            target_list
            if self._config.test
            else filter(lambda t: not t.source_type == SourceType.TEST, target_list)
        )

        for target in target_list:
            link_cmd = self._assemble_link_command(target, changelist)
            fancy_separator()
            fancy_run(
                link_cmd, error_message=f"Linkage of target {target.name} failed."
            )

        fancy_print(
            "All executable targets are up-to-date.",
            msg_type=MessageType.SUCCESS,
            flash=True,
        )

    def _assemble_link_command(self, target: Target, all_targets: List[Target]):
        """Assemble command line arguments for linking a target.

        Raises:
            AssertionError: If `target` is not one of the types SourceType.{MAIN, TEST}.
        """
        assert target.source_type in [SourceType.MAIN, SourceType.TEST]

        out_subdir = (
            Builder.BIN_DIR
            if target.source_type == SourceType.MAIN
            else Builder.TEST_DIR
        )

        out_executable_path = target.make_executable_path(
            self._config.build_directory / out_subdir
        )

        object_files_to_be_linked = [
            str(
                target.make_objfile_path(self._config.build_directory / Builder.OBJ_DIR)
            )
        ]
        object_files_to_be_linked.extend(
            [
                str(dependee_path)
                for dependee_path in self._assemble_dependee_list_of_target(
                    target, all_targets
                )
            ]
        )

        libs_statement = self._assemble_libraries_statement(target)

        link_cmd = [
            self._config.compiler,
            "-o",
            str(out_executable_path),
            *object_files_to_be_linked,
            *libs_statement,
            "-pthread",
        ]

        return link_cmd

    def _assemble_libraries_statement(self, target: Target) -> StringList:
        libs_statement = []
        for external_header in target.includes.external:

            found_dep = self._deps.query_by_header(external_header)
            library_path, libraries = found_dep.make_objfiles_dir_and_list(
                self._deps._path, self._config.debug
            )
            library_path = f"-L{library_path}"
            libraries = [
                f'-l{lib.lstrip("lib").rstrip(".a").rstrip(".so")}' for lib in libraries
            ]

            libs_statement.append(library_path)
            libs_statement.extend(libraries)
        return libs_statement

    def _assemble_dependee_list_of_target(
        self, target: Target, all_targets: List[Target]
    ) -> StringList:
        dependees = []
        for internal_header in target.includes.internal:
            for internal_dependency in filter(
                lambda other_target: other_target.includes.own == internal_header,
                all_targets,
            ):
                dependees.append(
                    str(
                        internal_dependency.make_objfile_path(
                            self._config.build_directory / Builder.OBJ_DIR
                        )
                    )
                )
        return dependees
