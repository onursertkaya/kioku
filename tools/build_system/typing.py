"""Type hint definitions for commonly used types."""
from pathlib import Path
from typing import List, Union

StringList = List[str]
PathString = Union[Path, str]

# Optionals
OptString = Union[str, None]
OptPathString = Union[PathString, None]
