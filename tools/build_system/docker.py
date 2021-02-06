import datetime
import os
import re
from pathlib import Path
from typing import Dict

from tools.build_system.constants import (
    IN_DOCKER_ENV_VAR_KEY,
    IN_DOCKER_ENV_VAR_VAL,
    IN_DOCKER_SRC_DIR,
    KIOKU_IMAGE_NAME,
    KIOKU_IMAGE_VERSIONS,
)
from tools.build_system.fancy import MessageType, fancy_print, fancy_run

CONSTANTS_FILE = "constants.py"
CONSTANTS_FILE_PATH = Path(__file__).parent / CONSTANTS_FILE


def is_in_docker():
    """Check if the current environment is the container or the host."""
    return os.getenv(IN_DOCKER_ENV_VAR_KEY) == IN_DOCKER_ENV_VAR_VAL


def update_versions_list(version_tag: str):
    """Append an entry with today's date in the list of docker kioku image tags."""
    assert CONSTANTS_FILE_PATH.is_file()

    with open(CONSTANTS_FILE_PATH) as c_file:
        c_file_content = c_file.read().splitlines()

    update_successful = False
    for line_no, line in enumerate(c_file_content):
        if re.match("^KIOKU_IMAGE_VERSIONS = \[$", line):
            today_formatted = f'    "{version_tag}",'
            if c_file_content[line_no + 1] != today_formatted:
                c_file_content.insert(line_no + 1, today_formatted)
                update_successful = True
                break

    if update_successful:
        with open(CONSTANTS_FILE_PATH, "w") as c_file:
            for l in c_file_content:
                c_file.write(l + "\n")


def run(cmd, args, ro_volumes: Dict[str, str] = {}, rw_volumes: Dict[str, str] = {}):
    """Run command in Kioku docker container."""
    assert not is_in_docker()

    host_cache = f"{Path.home()}/.cache"
    host_tmp = f"/tmp"
    host_group_file = "/etc/group"
    host_passwd_file = "/etc/passwd"

    default_rw_volumes = {host_cache: host_cache, host_tmp: host_tmp}
    default_ro_volumes = {
        host_passwd_file: host_passwd_file,
        host_group_file: host_group_file,
    }

    ro, rw = [], []
    for k, v in {**ro_volumes, **default_ro_volumes}.items():
        ro.append("-v")
        ro.append(f"{k}:{v}:ro")

    for k, v in {**rw_volumes, **default_rw_volumes}.items():
        rw.append("-v")
        rw.append(f"{k}:{v}:rw")

    uid = os.geteuid()
    gid = os.getegid()

    current_image_tag = KIOKU_IMAGE_VERSIONS[0]

    docker_cmd = [
        "docker",
        "run",
        "-it",
        "--rm",
        *ro,
        *rw,
        "-u",
        f"{uid}:{gid}",
        "-w",
        IN_DOCKER_SRC_DIR,
        "-e",
        f"{IN_DOCKER_ENV_VAR_KEY}={IN_DOCKER_ENV_VAR_VAL}",
        f"{KIOKU_IMAGE_NAME}:{current_image_tag}",
        cmd,
        *args,
    ]
    fancy_print("[Kioku] Running:", msg_type=MessageType.OTHER)
    fancy_run(docker_cmd)


def build():
    """Build a new kioku docker image."""
    assert not is_in_docker()

    import subprocess

    from tools.build_system.code_util import REPO_ROOT

    os.chdir(Path(REPO_ROOT) / "tools")

    today_tag = datetime.date.today().isoformat()
    update_versions_list(today_tag)

    # todo, fancy_run
    result = subprocess.run(
        ["docker", "build", ".", "-t", f"{KIOKU_IMAGE_NAME}:{today_tag}"]
    )

    if result.returncode == 0:
        fancy_print(
            "Please ensure that the entry\n"
            f"\t{today_tag}\n"
            "is correctly added to the file\n"
            f"\t{str(CONSTANTS_FILE_PATH)}\n"
            "then commit the changes.",
            msg_type=MessageType.WARNING,
        )
    else:
        fancy_print("Error occurred during docker build.", msg_type=MessageType.ERROR)
