from __future__ import annotations
import hashlib
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Optional
from .resolver import SentinelPathResolver, ObjectClass


@dataclass(frozen=True)
class CasObject:
    sha256: str
    path: Path
    size_bytes: int


class CasStore:
    def __init__(self, root_override: Optional[Path | str] = None):
        self.root = SentinelPathResolver.resolve_root(root_override)

    @staticmethod
    def compute_sha256(path: Path) -> str:
        hasher = hashlib.sha256()
        with open(path, "rb") as f:
            while chunk := f.read(65536):
                hasher.update(chunk)
        return hasher.hexdigest()

    def lookup(self, sha256: str) -> Optional[CasObject]:
        candidate = self.root / ObjectClass.MODEL_REGISTRY.value / sha256
        if candidate.is_file():
            return CasObject(sha256=sha256, path=candidate, size_bytes=candidate.stat().st_size)
        return None

    def store_file(self, source: Path | str, category: ObjectClass = ObjectClass.MODEL_REGISTRY) -> CasObject:
        src_path = Path(source).resolve()
        if not src_path.is_file():
            raise FileNotFoundError(f"Source file does not exist: {src_path}")

        digest = self.compute_sha256(src_path)
        dest_dir = SentinelPathResolver.resolve_dir(category, self.root)
        dest_dir.mkdir(parents=True, exist_ok=True)
        dest_file = dest_dir / digest

        if not dest_file.exists():
            shutil.copy2(src_path, dest_file)

        return CasObject(sha256=digest, path=dest_file, size_bytes=dest_file.stat().st_size)

    def verify_integrity(self, obj: CasObject) -> bool:
        if not obj.path.is_file():
            return False
        return self.compute_sha256(obj.path) == obj.sha256