import pickle
from dataclasses import dataclass
from typing import List

from tools.build_system.build_config import BuildConfig
from tools.build_system.constants import CACHE_FILE_NAME
from tools.build_system.target import Target


@dataclass(frozen=True)
class CacheState:

    valid: bool
    targets: List[Target]
    # targets_previous_build_successful: List[bool] # todo, implement
    build_config: BuildConfig

    def diff(self, other: "CacheState") -> List[Target]:
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
                    changed_targets.append(
                        current_target
                    )  # this is a newly added target
                    continue

                assert (
                    len(match) == 1
                ), f"Faulty cache, more than 1 targets with same exact name: {current_target.name}"

                old_target = match[0]
                if not current_target.checksums_match(old_target):
                    changed_targets.append(current_target)
        return changed_targets


class Cache:
    def __init__(self, build_config: BuildConfig) -> None:
        self.build_config = build_config
        self.cache_file_path = self.build_config.build_directory / CACHE_FILE_NAME

        self.build_config.build_directory.mkdir(exist_ok=True)

    def _load_cache(self) -> CacheState:
        if self.cache_file_path.exists():
            with open(self.cache_file_path, "rb") as f_handle:
                cache_state = pickle.load(f_handle)
            assert isinstance(
                cache_state, CacheState
            ), "[Cache] Load: Faulty cache file"
        else:
            cache_state = CacheState(False, [], BuildConfig())
        return cache_state

    def _save_cache(self, cache_state: CacheState) -> None:
        assert isinstance(cache_state, CacheState), "[Cache] Save: Faulty cache state"
        with open(self.cache_file_path, "wb") as f:
            pickle.dump(cache_state, f)

    def get_target_changelist(self, targets: List[Target]) -> List[Target]:
        previous_cache_state = self._load_cache()
        current_cache_state = CacheState(True, targets, self.build_config)
        self._save_cache(current_cache_state)

        return current_cache_state.diff(previous_cache_state)
