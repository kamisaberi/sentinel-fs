from __future__ import annotations
import os
import sys
from pathlib import Path
from enum import Enum
from typing import Optional


class ObjectClass(str, Enum):
    SPKG_INSTALLED = "spkg/installed"
    SPKG_CACHE = "spkg/cache"
    SPKG_STAGING = "spkg/staging"
    SCENARIO_CATALOG = "scenarios/catalog"
    SCENARIO_WORKSPACE = "scenarios/workspaces"
    SCENARIO_SNAPSHOT = "scenarios/snapshots"
    BLUEPRINT_HARDWARE = "blueprints/hardware"
    BLUEPRINT_APPARATUS = "blueprints/apparatus"
    BLUEPRINT_TSN = "blueprints/tsn"
    MODEL_REGISTRY = "models/registry"
    MODEL_CALIBRATED = "models/calibrated"
    MODEL_FORGE = "models/forge/checkpoints"
    SLAB_EXPERIMENT = "slab/experiments"
    SLAB_BENCHMARK = "slab/benchmarks"
    SLAB_TRAJECTORY = "slab/trajectories"
    ARTIFACT_PCAP = "artifacts/pcaps"
    ARTIFACT_DOWNLOAD = "artifacts/downloads"
    PKI_CA = "pki/ca"
    PKI_CERTS = "pki/certs"
    PKI_TPM = "pki/tpm"
    RUNTIME_SOCKET = "runtime/sockets"
    RUNTIME_LOCK = "runtime/locks"
    CONFIG = "config"


class SentinelPathResolver:
    @staticmethod
    def resolve_root(override: Optional[Path | str] = None) -> Path:
        if override:
            return Path(override).resolve()
        env_root = os.environ.get("SENTINEL_ROOT")
        if env_root and env_root.strip():
            return Path(env_root.strip()).resolve()
        if hasattr(os, "geteuid") and os.geteuid() == 0:
            return Path("/var/lib/sentinel").resolve()
        home = os.environ.get("HOME")
        if home and home.strip():
            return (Path(home) / ".sentinel").resolve()
        return (Path.cwd() / ".sentinel").resolve()

    @classmethod
    def resolve_dir(cls, obj: ObjectClass, override: Optional[Path | str] = None) -> Path:
        return cls.resolve_root(override) / obj.value

    @classmethod
    def resolve_pack_uri(cls, uri: str, override: Optional[Path | str] = None) -> Path:
        prefix = "pack://"
        if not uri.startswith(prefix):
            raise ValueError(f"Malformed pack URI: '{uri}'. Expected 'pack://'")
        path_str = uri[len(prefix):]
        parts = path_str.split("/", 1)
        cartridge = parts[0]
        subpath = parts[1] if len(parts) > 1 else ""

        root = cls.resolve_root(override)

        candidate = root / "spkg" / "installed" / cartridge / subpath
        if candidate.exists():
            return candidate.resolve()

        candidate = Path("/opt/sentinel/packs") / cartridge / subpath
        if candidate.exists():
            return candidate.resolve()

        home = os.environ.get("HOME")
        if home:
            candidate = Path(home) / cartridge / subpath
            if candidate.exists():
                return candidate.resolve()

        return (root / "spkg" / "installed" / cartridge / subpath).resolve()

    @classmethod
    def resolve_scenario_workspace(cls, slug: str, instance_id: Optional[str] = None, override: Optional[Path | str] = None) -> Path:
        base = cls.resolve_dir(ObjectClass.SCENARIO_WORKSPACE, override)
        name = f"{slug}_{instance_id}" if instance_id else slug
        return base / name

    @classmethod
    def initialize_hierarchy(cls, override: Optional[Path | str] = None) -> Path:
        root = cls.resolve_root(override)
        for obj in ObjectClass:
            target = root / obj.value
            target.mkdir(parents=True, exist_ok=True)
        return root


def main():
    root = SentinelPathResolver.initialize_hierarchy()
    print(f"[SSFS:Python] Authoritative Root established at: {root}")
    for item in ObjectClass:
        print(f"  • {item.name:<24} -> {SentinelPathResolver.resolve_dir(item)}")


if __name__ == "__main__":
    main()