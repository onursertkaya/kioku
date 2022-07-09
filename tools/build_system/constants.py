"""Repo-level constants."""

HEADER_EXTENSIONS = ("hpp", "h")
SOURCE_EXTENSIONS = ("cpp", "cxx", "c")

PY_EXTENSION = "py"

CPP_IFNDEF_STR = "#ifndef"
CPP_DEFINE_STR = "#define"
CPP_ENDIF_STR = "#endif"
CPP_INCLUDE_STR = "#include"
CPP_STANDARDS = (
    "11",
    "14",
    "17",
    "20",
)

CLANG_LATEST_VERSION_NO = "14"
CLANG_LATEST = f"clang++-{CLANG_LATEST_VERSION_NO}"
CLANG_TIDY_LATEST = f"clang-tidy-{CLANG_LATEST_VERSION_NO}"
CLANG_FORMAT_LATEST = f"clang-format-{CLANG_LATEST_VERSION_NO}"
GNU_LATEST = "g++"
COMPILERS = (CLANG_LATEST, GNU_LATEST)


# todo, put coloring utils into a constants class
RESET = "\033[0m"
BOLD = "\033[1m"
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
BLUE = "\033[34m"

BOLDRED = f"{BOLD}{RED}"
BOLDGREEN = f"{BOLD}{GREEN}"
BOLDYELLOW = f"{BOLD}{YELLOW}"
BOLDBLUE = f"{BOLD}{BLUE}"

IN_DOCKER_BUILD_DIR = "/kioku_build"
IN_DOCKER_DEPS_DIR = "/kioku_dependencies"
IN_DOCKER_SRC_DIR = "/kioku_src"
IN_DOCKER_ENV_VAR_KEY = "KIOKU_IN_DOCKER"
IN_DOCKER_ENV_VAR_VAL = "true"

KIOKU_IMAGE_NAME = "kioku"
KIOKU_IMAGE_VERSIONS = [
    "2022-02-05",
    "2022-01-28",
    "2021-11-16",
    "2021-09-12",
    "2021-07-04",
]
