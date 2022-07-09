"""Fancy CLI utilities."""
import shlex
import shutil
import subprocess
import sys
import time
from enum import Enum
from subprocess import CalledProcessError, check_call
from typing import Optional, Union

from tools.build_system.constants import BOLDBLUE, BOLDGREEN, BOLDRED, BOLDYELLOW, RESET
from tools.build_system.typing import StringList

LINE_BREAK_THRESHOLD = 40


class MessageType(Enum):
    """Message type when running fancy printing."""

    SUCCESS = 0
    WARNING = 1
    ERROR = 2
    OTHER = 3
    NONE = 4


def fancy_print(
    msg: str, msg_type: MessageType = MessageType.NONE, flash: bool = False
):
    """Print a message to stdout in a nice format."""
    assert isinstance(msg, str)

    if msg_type == MessageType.SUCCESS:
        selected_color = BOLDGREEN
    elif msg_type == MessageType.WARNING:
        selected_color = BOLDYELLOW
    elif msg_type == MessageType.ERROR:
        selected_color = BOLDRED
    elif msg_type == MessageType.OTHER:
        selected_color = BOLDBLUE
    elif msg_type == MessageType.NONE:
        selected_color = ""

    msg_colored = f"{selected_color}{msg}{RESET}"
    if flash:
        for _ in range(2):
            print(msg_colored, end="\r")
            time.sleep(0.5)
            print(" " * len(msg), end="\r")
            time.sleep(0.1)
    print(msg_colored)


def fancy_separator(length: Optional[int] = 0):
    """Print a message to stdout in a nice format."""
    length = length if length != 0 else _get_term_width() // 2
    fancy_print("=" * length, msg_type=MessageType.OTHER)


def fancy_run(
    cmd: Union[str, StringList],
    error_message: Optional[str] = "",
    silent: Optional[bool] = False,
    keep_running: Optional[bool] = False,
):
    """Message type when running fancy printing."""
    if isinstance(cmd, list):
        assert all([isinstance(item, str) for item in cmd])
    else:
        cmd = shlex.split(cmd)

    # TODO: no longer necessary in python3.8, convert to subprocess.run(capture_output=True)
    suppressing_kwargs = {}
    if not silent:
        msg = _format_line(cmd)
        fancy_print(msg, MessageType.NONE)
    else:
        suppressing_kwargs = {
            "stdout": subprocess.DEVNULL,
            "stderr": subprocess.DEVNULL,
        }

    try:
        check_call(cmd, **suppressing_kwargs)
    except CalledProcessError:
        fancy_print(error_message, msg_type=MessageType.ERROR)
        if not keep_running:
            sys.exit(-1)


def _get_term_width() -> int:
    return shutil.get_terminal_size().columns


def _format_line(cmd: StringList):
    retval = " ".join(cmd)
    if len(retval) >= _get_term_width() // 2:
        lines = []
        skip_next = False

        # word, word_next
        arg_pair = lambda w, w_n: (
            (w.startswith("--") or w.startswith("-"))
            and not ((w_n.startswith("--") or w_n.startswith("-")))
        )

        # word, word_next
        irrelevant_pair = lambda w, w_n: (
            not (w.startswith("-") or w.startswith("--"))
            and not (w_n.startswith("-") or w_n.startswith("--"))
        )

        for idx, word in enumerate(cmd):
            if skip_next:
                skip_next = False
                continue

            if idx >= len(cmd) - 2:
                lines.append(word)
                break
            next_word = cmd[idx + 1]

            if len(word) >= LINE_BREAK_THRESHOLD:
                lines.append(word)
            elif arg_pair(word, next_word) or irrelevant_pair(word, next_word):
                lines.append(word + " " + next_word)
                skip_next = True
            else:
                lines.append(word)
        retval = "\n\t".join(lines)

    return retval
