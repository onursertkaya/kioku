import subprocess
from pathlib import Path
from typing import List

from tools.build_system.fancy import MessageType, fancy_print


def scan_debuggable_files(directories: List[Path]) -> List[Path]:
    # sample `file` output:

    # ELF 64-bit LSB shared object,
    # x86-64,
    # version 1 (GNU/Linux),
    # dynamically linked,
    # interpreter /lib64/ld-linux-x86-64.so.2,
    # BuildID[sha1]=build_id_hash,
    # for GNU/Linux 3.2.0,
    # with debug_info, <<<
    # not stripped

    retval = []
    skipped = []
    for d in directories:
        executables = [str(f) for f in d.iterdir()]
        debuggable_executables = []
        skipped_executables = []
        for e in executables:
            file_output = subprocess.run(
                ["file", e], capture_output=True
            ).stdout.decode("utf-8")
            if "debug_info" in file_output:
                debuggable_executables.append(e)
            else:
                skipped_executables.append(e)

        retval.extend(debuggable_executables)
        skipped.extend(skipped_executables)

    fancy_print("Skipping the following files due to missing debug symbols:")
    fancy_print("\n\t- ".join(sorted(skipped)))
    return sorted(retval)


def choose_executable_to_debug(executables: List[Path]) -> Path:
    executable_list_str = ""
    for idx, exe in enumerate(executables):
        executable_list_str += f"\t [{idx}] {str(exe)}\n"

    fancy_print("Please select an executable to debug:")
    fancy_print(executable_list_str)
    fancy_print("(select executable or press q to exit)")
    user_input = str(input("> "))

    if user_input.lower() == "q" or not user_input:
        return Path()

    selected_idx = int(user_input)
    assert 0 <= selected_idx < len(executables)

    return executables[selected_idx]


def run_in_debugger(executable: Path):
    subprocess.run(["gdb", executable])


def run_tests(test_executables_directory: Path, under: str = ""):
    cmd = []

    # run under dynamic analysis tools; gdb, valgrind etc.
    if under:
        assert subprocess.run(["which", under]).returncode == 0
        cmd.append(under)

    failed_tests = []
    for test_exe in test_executables_directory.iterdir():
        result = subprocess.run(cmd + [test_exe], cwd=test_executables_directory)
        if result.returncode != 0:
            failed_tests.append((str(test_exe), str(result.returncode)))
    success = len(failed_tests) == 0

    msg = "[Kioku Tests]: "
    if success:
        msg += "Success."
    else:
        msg += "Failed\n"
        for test, return_code in failed_tests:
            msg += "- " + test + ": " + return_code + "\n"

    fancy_print(msg, msg_type=(MessageType.SUCCESS if success else MessageType.ERROR))

    return 0 if success else -1
