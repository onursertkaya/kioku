"""Cache utilities to achieve build avoidance."""
from __future__ import annotations

import pickle
from dataclasses import dataclass
from typing import List

from tools.build_system.build_config import BuildConfig
from tools.build_system.target import Target


@dataclass(frozen=True)
class CacheState:
    """An instantaneous state representation of the repository."""

    valid: bool
    targets: List[Target]
    # targets_previous_build_successful: List[bool] # todo, implement
    build_config: BuildConfig

    def diff(self, other: CacheState) -> List[Target]:
        """Get the difference between two cache states."""
        changed_targets = []

        if (
            self.build_config != other.build_config
            or self.build_config.force_build
            or not other.valid
        ):
            changed_targets = self.targets
        else:
            for current_target in self.targets:
                match = list(
                    filter(lambda t: t.name == current_target.name, other.targets)
                )
                if not match:
                    # this is a newly added target
                    changed_targets.append(current_target)
                    continue

                assert (
                    len(match) == 1
                ), f"Faulty cache, more than 1 targets with same exact name: {current_target.name}"

                old_target = match[0]
                if not current_target.checksums_match(old_target):
                    changed_targets.append(current_target)
        return changed_targets


class Cache:
    """Build cache."""

    CACHE_FILE_NAME = "kioku_cache.pkl"

    def __init__(self, build_config: BuildConfig):
        """Create an instance."""
        self._build_config = build_config
        self._cache_file_path = (
            self._build_config.build_directory / Cache.CACHE_FILE_NAME
        )

        self._build_config.build_directory.mkdir(exist_ok=True)

    def _load_cache(self) -> CacheState:
        """Deserialize a previous cache state from disk."""
        if self._cache_file_path.exists():
            with open(self._cache_file_path, "rb") as f_handle:
                cache_state = pickle.load(f_handle)
            print(f"Successfully loaded cache file: {self._cache_file_path}")
            assert isinstance(
                cache_state, CacheState
            ), "[Cache] Load: Faulty cache file"
        else:
            cache_state = CacheState(False, [], BuildConfig())
            print("Cache file does not exist, it will be created.")
        return cache_state

    def _save_cache(self, cache_state: CacheState):
        """Serialize the current cache state to disk."""
        with open(self._cache_file_path, "wb") as f:
            pickle.dump(cache_state, f)

    def get_target_changelist(self, targets: List[Target]) -> List[Target]:
        """Compare the current list of targets with a previous version to get the difference."""
        previous_cache_state = self._load_cache()
        current_cache_state = CacheState(True, targets, self._build_config)
        self._save_cache(current_cache_state)

        return current_cache_state.diff(previous_cache_state)
