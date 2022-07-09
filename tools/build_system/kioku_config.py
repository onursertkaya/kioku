"""Utilities to parse the kioku config file."""
import json
from pathlib import Path
from typing import Optional, Union

from tools.build_system.code_util import REPO_ROOT
from tools.build_system.typing import PathString

KIOKU_CONFIG_FILE_NAME = ".kioku"
KIOKU_CONFIG_SCHEMA_FILE_NAME = ".kiokuschema.json"
CONFIG_SCHEMA_PATH = Path(REPO_ROOT) / KIOKU_CONFIG_SCHEMA_FILE_NAME


class HostConfigKeys:
    """Configuration file keys."""

    DEPENDENCIES_DIR = "dependencies_directory"
    BUILD_DIR = "build_directory"


def parse_host_config(custom_config: Optional[PathString] = None):
    """Parse a config file from disk."""
    default_config_path = Path(REPO_ROOT) / KIOKU_CONFIG_FILE_NAME
    config_file = Path(custom_config) if custom_config else default_config_path

    if config_file.is_file():
        with open(config_file) as c_file:
            conf = json.load(c_file)
    else:
        raise FileNotFoundError(f"Config file {config_file} does not exist.")

    # todo, enable the following, since it's called on host it raises an import error
    # from jsonschema import validate
    # with open(CONFIG_SCHEMA_PATH) as schema_file:
    #     schema = json.load(schema_file)

    for key in vars(HostConfigKeys):
        if "__" not in key:
            value = HostConfigKeys.__dict__[key]
            assert conf[value], f"{value} not found in config file: {str(config_file)}"

    assert Path(conf[HostConfigKeys.BUILD_DIR]).is_dir()
    assert Path(conf[HostConfigKeys.DEPENDENCIES_DIR]).is_dir()

    return conf
