"""Module for managing and maintaining repo dependencies."""
import os
from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Tuple

from tools.build_system.fancy import fancy_run, fancy_separator
from tools.build_system.typing import StringList


@dataclass(frozen=True)
class Dependency:
    """Representation of a dependency."""

    name: str
    github_url: str
    default_branch: str

    relative_include_path: str = ""
    include_statement: str = ""

    relative_built_files_prefix: str = ""

    built_files: StringList = field(default_factory=lambda: [])
    built_files_debug: StringList = field(default_factory=lambda: [])
    additional_flags: StringList = field(default_factory=lambda: [])

    @property
    def header_relpath(self) -> Path:
        """Get the relative path to the main header file."""
        return Path(self.name) / self.relative_include_path / self.include_statement

    def make_includepath_statement(self, dependencies_root: Path) -> str:
        """Get includepath statement for the compiler."""
        return f"-I{dependencies_root / self.name / self.relative_include_path}"

    def make_objfiles_dir_and_list(
        self, dependencies_path: Path, debug_build: bool
    ) -> Tuple[Path, StringList]:
        """Make an object file directory as well as a list of object file names."""
        built_files_dir = (
            Dependencies.DEFAULT_DEBUG_BUILD_DIR
            if debug_build
            else Dependencies.DEFAULT_BUILD_DIR
        ) / self.relative_built_files_prefix

        obj_library_path = dependencies_path / self.name / built_files_dir
        obj_file_list = self.built_files_debug if debug_build else self.built_files

        return obj_library_path, obj_file_list


class Dependencies:
    """Repo dependencies as code."""

    DEFAULT_BUILD_DIR = Path("build")
    DEFAULT_DEBUG_BUILD_DIR = Path("build_debug")

    def __init__(self, path: Path) -> None:
        """Make an instance."""
        self._path = path
        self._path.mkdir(exist_ok=True)

        # TODO: add rapidcheck
        self._deps = [
            Dependency(
                "googletest",
                "https://github.com/google/googletest.git",
                "main",
                "googletest/include",
                "gtest/gtest.h",
                "lib",
                ["libgtest.a", "libgtest_main.a"],
                ["libgtest.a", "libgtest_main.a"],
                ["-pthread"],
            ),
            Dependency(
                "libpng",
                "https://github.com/glennrp/libpng",
                "libpng16",
                ".",
                "png.h",
                "lib",
                ["libpng.a"],
                ["libpngd.a"],
            ),
        ]

        self.misc_deps = [
            Dependency(
                "onursertkaya.github.io",
                "https://github.com/onursertkaya/onursertkaya.github.io.git",
                "master",
            )
        ]

    def query_by_header(self, header: str) -> Dependency:
        return next(
            filter(
                lambda dep: str(self._path / dep.header_relpath) == header,
                self._deps,
            )
        )

    @property
    def get_list(self) -> List[Dependency]:
        """Get the list of dependencies."""
        return self._deps

    def build(self):
        """Build all the dependencies."""
        current_dir = Path.cwd()
        os.chdir(str(self._path))
        for dep_dir in self.path.iterdir():
            os.chdir(dep_dir)
            if dep_dir.name == "googletest":
                (Path.cwd() / self.DEFAULT_BUILD_DIR).mkdir(exist_ok=True)
                os.chdir(self.DEFAULT_BUILD_DIR)

                fancy_separator()
                fancy_run(["cmake", ".."])
                fancy_run(["make", "-j2"])

                os.chdir(
                    ".."
                )  # todo, add context manager for changing the directory temporarily
                (Path.cwd() / self.DEFAULT_DEBUG_BUILD_DIR).mkdir(exist_ok=True)
                os.chdir(self.DEFAULT_DEBUG_BUILD_DIR)

                fancy_separator()
                fancy_run(["cmake", "-DCMAKE_BUILD_TYPE=Debug", ".."])
                fancy_run(["make", "-j2"])
            elif dep_dir.name == "libpng":
                (Path.cwd() / self.DEFAULT_BUILD_DIR).mkdir(exist_ok=True)

                fancy_separator()
                fancy_run(["./configure", f"--prefix={str(Path.cwd() / self.DEFAULT_BUILD_DIR)}"])
                fancy_run(["make", "check"])
                fancy_run(["make", "install"])

        os.chdir(current_dir)

    def fetch(self):
        """Fetch the mandatory dependencies."""
        current_dir = Path.cwd()
        os.chdir(str(self._path))
        dir_content = [dep_dir.name for dep_dir in self.path.iterdir()]
        for dep in self.deps:
            if dep.name not in dir_content:
                cmd = ["git", "clone", dep.github_url]
                fancy_separator()
                fancy_run(cmd)
            else:
                os.chdir(dep.name)
                cmd = ["git", "checkout", dep.default_branch]
                fancy_separator()
                fancy_run(cmd)
                cmd = ["git", "pull", "origin", dep.default_branch]
                fancy_separator()
                fancy_run(cmd)
            os.chdir(str(self._path))
        os.chdir(current_dir)

    def fetch_misc_deps(self):
        """Fetch the dependencies in misc list."""
        current_dir = Path.cwd()
        os.chdir(str(self._path))
        for dep in self.misc_deps:
            cmd = ["git", "clone", dep.github_url]
            fancy_separator()
            fancy_run(cmd)
        os.chdir(current_dir)
