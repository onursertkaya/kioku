import argparse

from tools.build_system.constants import CLANG_LATEST, COMPILERS, CPP_STANDARDS

STORE_TRUE = "store_true"


class Modes:
    BUILD = "build"
    BUILD_DOCKER = "build_docker"
    DEBUG = "debug"
    DEPS = "deps"
    CODE_QUAL = "codequal"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()

    subparsers = parser.add_subparsers(help="", dest="subparser")

    # =========
    parser_build = subparsers.add_parser(Modes.BUILD, help="Build targets.")

    parser_build.add_argument(
        "-t",
        "--target",
        default=".",
        help="Path to directory that contains targets to build.",
    )

    parser_build.add_argument("--compiler", default=CLANG_LATEST, choices=COMPILERS)

    parser_build.add_argument(
        "-d", "--debug", action=STORE_TRUE, help="Add debug symbols to the build."
    )

    parser_build.add_argument(
        "--optimize", action=STORE_TRUE, help="Allow compiler optimizations."
    )

    parser_build.add_argument(
        "-f",
        "--force-build",
        action=STORE_TRUE,
        help="Forcefully build the target, ignoring the cache.",
    )

    parser_build.add_argument(
        "--test",
        action=STORE_TRUE,
        help="Compile and run all tests that are associated with the requested target.",
    )

    parser_build.add_argument("--cpp-standard", default="17", choices=CPP_STANDARDS)

    # =========
    subparsers.add_parser(
        Modes.DEBUG, help="Launch debugger in an interactive terminal."
    )

    # =========
    parser_code_qual = subparsers.add_parser(
        Modes.CODE_QUAL, help="Run code formatting, tidying and linting utilities."
    )

    parser_code_qual.add_argument("--clang-format", action=STORE_TRUE)
    parser_code_qual.add_argument("--clang-tidy", action=STORE_TRUE)
    parser_code_qual.add_argument("--cpplint", action=STORE_TRUE)
    parser_code_qual.add_argument("--header-guard", action=STORE_TRUE)
    parser_code_qual.add_argument("--py-check", action=STORE_TRUE)
    parser_code_qual.add_argument("--py-black", action=STORE_TRUE)
    parser_code_qual.add_argument("--py-isort", action=STORE_TRUE)
    parser_code_qual.add_argument("--py-test", action=STORE_TRUE)

    # =========
    subparsers.add_parser(Modes.DEPS, help="Manage dependencies.")

    # =========
    subparsers.add_parser(Modes.BUILD_DOCKER, help="Build the kioku docker image.")

    args = parser.parse_args()
    if getattr(args, "subparser", None) is None:
        raise ValueError("A command is required, see help for options.")
    return args
