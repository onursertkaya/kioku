#!/usr/bin/python3
"""Main script to interact with the repository."""

import argparse
import pathlib
import sys

from tools.build_system.code_util import REPO_ROOT
from tools.build_system.constants import (
    IN_DOCKER_BUILD_DIR,
    IN_DOCKER_DEPS_DIR,
    IN_DOCKER_SRC_DIR,
)
from tools.build_system.docker import is_in_docker
from tools.build_system.kioku_args import Modes, parse_args
from tools.build_system.kioku_config import parse_host_config


def merge_args_and_config() -> argparse.Namespace:
    """Combine cli arguments and disk-loaded configuration.

    Priority will be given to cli arguments for duplicate fields.
    """
    host_config = parse_host_config()
    args = parse_args()

    extract_pairs_with_valid_values = lambda x: {
        k: v for k, v in x.items() if v is not None
    }
    # Unpack args in second place so that it overwrites config for
    # duplicate keys.
    merged = {
        **extract_pairs_with_valid_values(host_config),
        **extract_pairs_with_valid_values(args.__dict__),
    }

    return argparse.Namespace(**merged)


def host_main():
    """Run main function that is invoked on host."""

    def forward_to_docker(config):
        """Run requested command and arguments in docker."""
        from tools.build_system.docker import run as run_in_docker

        rw_volumes = {
            REPO_ROOT: IN_DOCKER_SRC_DIR,
            config.dependencies_directory: IN_DOCKER_DEPS_DIR,
        }

        if config.subparser in (Modes.BUILD, Modes.DEBUG):
            rw_volumes = {
                **rw_volumes,
                config.build_directory: IN_DOCKER_BUILD_DIR,
            }

        run_in_docker("python3", sys.argv, rw_volumes=rw_volumes)

    config = merge_args_and_config()

    if config.subparser == Modes.BUILD_DOCKER:
        from tools.build_system.docker import build as build_docker_image

        build_docker_image()
    else:
        forward_to_docker(config)


def docker_main():
    """Run main function that is invoked inside docker container."""
    args = parse_args()
    # pylint: disable=import-outside-toplevel
    # pylint: disable=too-many-branches
    # pylint: disable=too-many-locals

    if args.subparser == Modes.DEPS:
        from tools.build_system.dependencies import Dependencies

        dep_manager = Dependencies(pathlib.Path(IN_DOCKER_DEPS_DIR))
        dep_manager.fetch()
        dep_manager.build()

    elif args.subparser == Modes.CODE_QUAL:
        # TODO: unify similar jobs under common commands, fix the import scheme.
        if args.clang_format:
            from tools.build_system.code_quality_util import clang_format

            clang_format()
        elif args.clang_tidy:
            from tools.build_system.code_quality_util import clang_tidy

            clang_tidy()
        elif args.cpplint:
            from tools.build_system.code_quality_util import cpplint

            cpplint()
        elif args.header_guard:
            from tools.build_system.code_quality_util import header_guard

            header_guard()
        elif args.py_check:
            from tools.build_system.code_quality_util import py_check_all

            py_check_all()
        elif args.py_format:
            from tools.build_system.code_quality_util import py_format

            py_format()
        elif args.py_test:
            from tools.build_system.code_quality_util import py_test

            py_test()
        else:
            raise ValueError(f"{Modes.CODE_QUAL} command needs a job as a parameter.")

    elif args.subparser == Modes.BUILD:
        from tools.build_system.build_config import BuildConfig
        from tools.build_system.builder import Builder

        config = BuildConfig(
            debug=args.debug,
            compiler=args.compiler,
            optimize=args.optimize,
            cpp_standard=args.cpp_standard,
            build_directory=pathlib.Path(IN_DOCKER_BUILD_DIR),
            target_directory=args.target,
            test=args.test,
            thirdparty_dep_directory=pathlib.Path(IN_DOCKER_DEPS_DIR),
            force_build=args.force_build,
        )

        kioku_builder = Builder(config)
        kioku_builder.build()

        if config.test:
            from tools.build_system.test_and_debug_util import run_tests

            sys.exit(run_tests(pathlib.Path(IN_DOCKER_BUILD_DIR) / Builder.TEST_DIR))
    elif args.subparser == Modes.DEBUG:
        from tools.build_system.builder import Builder
        from tools.build_system.test_and_debug_util import (
            choose_executable_to_debug,
            run_in_debugger,
            scan_debuggable_files,
        )

        bins_dir = pathlib.Path(IN_DOCKER_BUILD_DIR) / Builder.BIN_DIR
        tests_dir = pathlib.Path(IN_DOCKER_BUILD_DIR) / Builder.TEST_DIR

        tests_and_binaries = scan_debuggable_files([bins_dir, tests_dir])
        selected = choose_executable_to_debug(tests_and_binaries)

        run_in_debugger(selected)


def main():
    """Entrypoint for the main program."""
    if is_in_docker():
        docker_main()
    else:
        host_main()


if __name__ == "__main__":
    main()
