import json
from pathlib import Path
from typing import Union

from tools.build_system.code_util import REPO_ROOT
from tools.build_system.constants import (
    KIOKU_CONFIG_FILE_NAME,
    KIOKU_CONFIG_SCHEMA_FILE_NAME,
    HostConfigKeys,
)

CONFIG_SCHEMA_PATH = Path(REPO_ROOT) / KIOKU_CONFIG_SCHEMA_FILE_NAME


def parse_host_config(custom_config: Union[Path, str, None] = None):
    """Parse a config from disk."""

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
