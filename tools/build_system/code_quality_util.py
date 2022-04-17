import re
from pathlib import Path

from tools.build_system.builder import BuildConfig, Builder
from tools.build_system.code_util import (
    REPO_ROOT,
    get_all_headers,
    get_all_py_files,
    get_all_sources,
    get_system_include_paths,
)
from tools.build_system.constants import (
    CLANG_FORMAT_LATEST,
    CLANG_LATEST,
    CLANG_TIDY_LATEST,
    CPP_DEFINE_STR,
    CPP_ENDIF_STR,
    CPP_IFNDEF_STR,
    IN_DOCKER_DEPS_DIR,
)
from tools.build_system.fancy import fancy_run
from tools.build_system.target import SourceType


def clang_format():
    all_files = get_all_headers() + get_all_sources()
    cmd = [CLANG_FORMAT_LATEST, "-i"] + all_files
    fancy_run(cmd)


def cpplint():
    all_files = get_all_headers() + get_all_sources()

    ignore_list = [
        "whitespace/braces",
        "legal/copyright",
        "whitespace/line_length",
        "whitespace/newline",
        "build/header_guard",
        "build/c++11",
    ]

    ignore_list = ["-" + e for e in ignore_list]

    cmd = [
        "cpplint",
        f'--filter={",".join(ignore_list)}',
        "--linelength=100",
        *all_files,
    ]

    fancy_run(cmd)


def clang_tidy():
    conf = BuildConfig(
        debug=False,
        compiler=CLANG_TIDY_LATEST,
        optimize=False,
        cpp_standard="c++17",
        build_directory=Path(),
        target_directory=REPO_ROOT,
        force_build=True,
        test=False,
        thirdparty_dep_directory=Path(IN_DOCKER_DEPS_DIR),
    )

    builder = Builder(conf)
    targets = builder._targets

    cmd_base = [
        CLANG_TIDY_LATEST,
        "-header-filter=.*",
        # "--fix",
    ]

    sys_includes = [
        f"-isystem{include_path}"
        for include_path in get_system_include_paths(CLANG_LATEST)
    ]

    for target in filter(lambda x: not x.source_type == SourceType.TEST, targets):
        includepaths = builder._query_all_includepaths(target)
        include_statements = [f"{incpath}" for incpath in includepaths]
        cpp_standard = f"-std={conf.cpp_standard}"
        cmd = (
            cmd_base
            + [target.source_file]
            + [
                "--",
                cpp_standard,
                *include_statements,
                *sys_includes,
            ]
        )
        fancy_run(cmd, keep_running=True)


def header_guard():
    start_pattern = f"^({CPP_IFNDEF_STR}|{CPP_DEFINE_STR})"
    end_pattern = f"^{CPP_ENDIF_STR}"

    for header in get_all_headers():
        lines = [l.rstrip("\n ") for l in open(header, "r")]

        guard = str(Path(header).relative_to(Path(REPO_ROOT))).upper()
        for repl in ["/", ".", "-"]:
            guard = guard.replace(repl, "_")

        edited = False
        start_pattern_match_found, end_pattern_match_found = False, False
        for idx, line in enumerate(lines):
            # prevent touching macros and string literals by looking
            # only for the format
            #     #define symbol_name
            # and skipping the format
            #     #define symbol_name symbol_value
            if re.match(rf"{start_pattern}\s\w+$", line):
                start_pattern_match_found = True
                if not re.match(start_pattern + f" {guard}$", line):
                    # Note, in next line: len('#ifndef') == len('#define')
                    lines[idx] = lines[idx][: len(CPP_IFNDEF_STR)] + " " + guard
                    edited = True

            if re.match(end_pattern + ".*", line):
                end_pattern_match_found = True
                if not re.match(end_pattern + f"  // {guard}$", line):
                    lines[idx] = lines[idx][: len(CPP_ENDIF_STR)] + "  // " + guard
                    edited = True

        if not start_pattern_match_found:
            lines.insert(0, CPP_DEFINE_STR + " " + guard)
            lines.insert(0, CPP_IFNDEF_STR + " " + guard)
            edited = True

        while lines[-1] == "":
            del lines[-1]
            edited = True

        if not end_pattern_match_found:
            lines.append(CPP_ENDIF_STR + "  // " + guard)
            edited = True

        if edited:
            with open(header, "w") as f:
                for q in lines:
                    f.write(q + "\n")


def py_check_all():
    all_files = get_all_py_files()

    from functools import partial

    non_exiting_fancy_run = partial(fancy_run, silent=False, keep_running=True)

    non_exiting_fancy_run(["python3", "-m", "pylint", *all_files])
    non_exiting_fancy_run(["python3", "-m", "pycodestyle", REPO_ROOT])
    non_exiting_fancy_run(["python3", "-m", "pydocstyle", REPO_ROOT])


def py_test():
    all_files = get_all_py_files()

    from functools import partial

    non_exiting_fancy_run = partial(fancy_run, silent=False, keep_running=True)

    non_exiting_fancy_run(["python3", "-m", "unittest", *all_files], keep_running=True)


def py_black():
    fancy_run(["python3", "-m", "black", REPO_ROOT], silent=False)


def py_isort():
    fancy_run(["python3", "-m", "isort", REPO_ROOT], silent=False)
