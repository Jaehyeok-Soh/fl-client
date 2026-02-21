import os
import json
import shutil
import hashlib
from dataclasses import dataclass
from datetime import datetime, date, timedelta
from pathlib import Path
from typing import Dict, Optional, Tuple, List


SCRIPT_DIR = Path(__file__).parent
CONFIG_PATH = SCRIPT_DIR / "config.json"
SNAPSHOT_PATH = SCRIPT_DIR / ".last_sync.v2.json"

EXCLUDE_FOLDERS: List[str] = []
EXCLUDE_FILES: List[str] = []


@dataclass(frozen=True)
class FileMeta:
    mtime: float
    size: int


def now_str() -> str:
    return datetime.now().strftime("%Y%m%d-%H%M%S")


def load_config() -> dict:
    if not CONFIG_PATH.exists():
        print("[오류] config.json 파일이 없습니다!")
        raise SystemExit(1)
    with open(CONFIG_PATH, "r", encoding="utf-8") as f:
        return json.load(f)


def find_valid_paths(config: dict) -> Tuple[str, str]:
    paths = config.get("paths", [])
    for idx, pair in enumerate(paths):
        local_path = pair.get("local_path", "")
        drive_path = pair.get("drive_path", "")
        if local_path and drive_path and Path(local_path).exists():
            print(f"[경로 발견] paths[{idx}] 사용")
            return local_path, drive_path

    print("[오류] 유효한 local_path를 찾을 수 없습니다!")
    for idx, pair in enumerate(paths):
        print(f"  [{idx}] {pair.get('local_path', '(없음)')}")
    raise SystemExit(1)


def atomic_write_json(path: Path, payload: dict) -> None:
    tmp = path.with_suffix(path.suffix + ".tmp")
    with open(tmp, "w", encoding="utf-8") as f:
        json.dump(payload, f, ensure_ascii=False, indent=2)
    tmp.replace(path)


def load_snapshot() -> Dict[str, FileMeta]:
    if not SNAPSHOT_PATH.exists():
        return {}

    try:
        with open(SNAPSHOT_PATH, "r", encoding="utf-8") as f:
            raw = json.load(f)

        # v2 format
        if isinstance(raw, dict) and raw.get("version") == 2 and isinstance(raw.get("files"), dict):
            out: Dict[str, FileMeta] = {}
            for rel, meta in raw["files"].items():
                if not isinstance(meta, dict):
                    continue
                out[rel] = FileMeta(mtime=float(meta.get("mtime", 0.0)), size=int(meta.get("size", 0)))
            return out

        # legacy {rel: mtime}
        if isinstance(raw, dict):
            out: Dict[str, FileMeta] = {}
            for rel, m in raw.items():
                try:
                    out[rel] = FileMeta(mtime=float(m), size=-1)
                except Exception:
                    pass
            return out

    except Exception as e:
        print(f"[경고] 스냅샷 로드 실패: {e}")
        return {}

    return {}


def save_snapshot(drive_files: Dict[str, FileMeta], options: dict) -> None:
    payload = {
        "version": 2,
        "created_at": datetime.now().isoformat(timespec="seconds"),
        "mode": "drive_to_local_fast",
        "options": {
            "time_tolerance_sec": float(options["time_tolerance_sec"]),
            "hash_max_mb": float(options["hash_max_mb"]),
            "hash_compare_on_update": bool(options["hash_compare_on_update"]),
        },
        "files": {
            rel: {"mtime": meta.mtime, "size": meta.size}
            for rel, meta in drive_files.items()
        }
    }
    atomic_write_json(SNAPSHOT_PATH, payload)


def setup_logger(log_folder_name: str) -> Path:
    log_dir = SCRIPT_DIR / log_folder_name
    log_dir.mkdir(parents=True, exist_ok=True)
    return log_dir / f"sync-{now_str()}.log"


def log_append(log_path: Path, text: str) -> None:
    with open(log_path, "a", encoding="utf-8") as f:
        f.write(text + "\n")


def sha256_of_file(path: Path) -> str:
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def is_rawdata_file(rel: str, raw_exts_lower: List[str]) -> bool:
    return Path(rel).suffix.lower() in raw_exts_lower


def build_backup_path(backup_root: Path, rel: str, ts: str) -> Path:
    # overwritten/YYYY-MM-DD/<relative_path>/<filename>.vTS.ext
    date_dir = datetime.now().strftime("%Y-%m-%d")
    dst = backup_root / date_dir / rel
    return dst.with_name(f"{dst.stem}.v{ts}{dst.suffix}")


def archive_local_overwritten(local_file: Path, rel: str, options: dict, raw_exts_lower: List[str], dry_run: bool, log_path: Path):
    if not options["backup_enable"]:
        return
    if not options["backup_local_before_overwrite"]:
        return
    if not is_rawdata_file(rel, raw_exts_lower):
        return
    if not local_file.exists():
        return

    backup_root = SCRIPT_DIR / options["backup_folder_name"]
    ts = now_str()
    dst = build_backup_path(backup_root, rel, ts)

    msg = f"    -> backup(overwritten local): {dst.as_posix()}"
    print(msg)
    log_append(log_path, msg)

    if dry_run:
        return

    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(local_file, dst)


def cleanup_empty_dirs(root: Path, start: Path, dry_run: bool) -> None:
    cur = start
    while True:
        if cur == root or cur == cur.parent:
            break
        try:
            if cur.exists() and cur.is_dir() and not any(cur.iterdir()):
                if not dry_run:
                    cur.rmdir()
            else:
                break
        except Exception:
            break
        cur = cur.parent


def move_to_trash_local(local_base: str, rel: str, trash_name: str, dry_run: bool) -> Path:
    base_root = Path(local_base)
    src = base_root / rel
    trash_root = base_root / trash_name
    dst = trash_root / rel

    ts = now_str()
    dst = dst.with_name(dst.name + f".deleted-{ts}")

    if dry_run:
        return dst

    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.move(str(src), str(dst))
    cleanup_empty_dirs(base_root, src.parent, dry_run=False)
    return dst


def dir_size_bytes(path: Path) -> int:
    total = 0
    for p in path.rglob("*"):
        try:
            if p.is_file():
                total += p.stat().st_size
        except Exception:
            continue
    return total


def list_backup_date_dirs(backup_root: Path) -> List[Tuple[date, Path, int]]:
    out: List[Tuple[date, Path, int]] = []
    if not backup_root.exists() or not backup_root.is_dir():
        return out
    for child in backup_root.iterdir():
        if not child.is_dir():
            continue
        try:
            d = datetime.strptime(child.name, "%Y-%m-%d").date()
        except Exception:
            continue
        out.append((d, child, dir_size_bytes(child)))
    out.sort(key=lambda x: x[0])  # old -> new
    return out


def prune_backup_by_days(backup_root: Path, keep_days: int, dry_run: bool, log_path: Path) -> None:
    if keep_days <= 0:
        return
    today = date.today()
    cutoff = today - timedelta(days=keep_days - 1)
    for d, p, sz in list_backup_date_dirs(backup_root):
        if d < cutoff:
            msg = f"[backup_prune_days] remove: {p.as_posix()} (date={d.isoformat()}, size={sz}B, cutoff={cutoff.isoformat()})"
            print(msg)
            log_append(log_path, msg)
            if not dry_run:
                shutil.rmtree(p, ignore_errors=True)


def prune_backup_by_size(backup_root: Path, max_bytes: int, dry_run: bool, log_path: Path, keep_days: int) -> None:
    if max_bytes <= 0:
        return
    dirs = list_backup_date_dirs(backup_root)
    total = sum(sz for _, _, sz in dirs)

    msg0 = f"[backup_prune_size] total={total}B, max={max_bytes}B"
    print(msg0)
    log_append(log_path, msg0)

    if total <= max_bytes:
        return

    today = date.today()
    cutoff = today - timedelta(days=keep_days - 1)

    for phase in ("older_than_keep_days", "force_within_keep_days"):
        dirs = list_backup_date_dirs(backup_root)
        total = sum(sz for _, _, sz in dirs)
        if total <= max_bytes:
            return

        if phase == "force_within_keep_days":
            warn = f"[backup_prune_size][WARNING] size limit forces deletion within keep-days window. (keep_days={keep_days})"
            print(warn)
            log_append(log_path, warn)

        for d, p, sz in dirs:
            if total <= max_bytes:
                break
            if phase == "older_than_keep_days" and d >= cutoff:
                continue

            msg = f"[backup_prune_size] remove: {p.as_posix()} (date={d.isoformat()}, size={sz}B, phase={phase})"
            print(msg)
            log_append(log_path, msg)
            if not dry_run:
                shutil.rmtree(p, ignore_errors=True)
            total -= sz


def scan_drive_fast(base_path: str, exclude_folders: List[str], exclude_files: List[str]) -> Dict[str, FileMeta]:
    """
    os.scandir 기반 고속 스캔 (Drive만 '전체 스캔'하고, Local은 필요할 때만 stat/hash)
    exclude_folders: 폴더명 기준(경로 구성요소)으로 스킵
    """
    base = Path(base_path)
    base_str = str(base)
    base_len = len(base_str)

    ex_folders = set(exclude_folders)
    ex_files = set(exclude_files)

    result: Dict[str, FileMeta] = {}

    stack = [base_str]
    while stack:
        cur = stack.pop()
        try:
            with os.scandir(cur) as it:
                for ent in it:
                    name = ent.name
                    try:
                        if ent.is_dir(follow_symlinks=False):
                            if name in ex_folders:
                                continue
                            stack.append(ent.path)
                        elif ent.is_file(follow_symlinks=False):
                            if name in ex_files:
                                continue
                            st = ent.stat()
                            rel = ent.path[base_len + 1:].replace("\\", "/")

                            # (수정) 제외 폴더명 검사는 '디렉토리 파트'에만 적용
                            parts = rel.split("/")
                            if any(p in ex_folders for p in parts[:-1]):
                                continue

                            result[rel] = FileMeta(mtime=st.st_mtime, size=st.st_size)
                    except OSError:
                        continue
        except OSError:
            continue

    return result


def get_local_meta(local_base: str, rel: str, cache: Dict[str, Optional[FileMeta]]) -> Optional[FileMeta]:
    if rel in cache:
        return cache[rel]
    p = Path(local_base) / rel
    try:
        if not p.exists() or not p.is_file():
            cache[rel] = None
            return None
        st = p.stat()
        meta = FileMeta(mtime=st.st_mtime, size=st.st_size)
        cache[rel] = meta
        return meta
    except OSError:
        cache[rel] = None
        return None


def sync_drive_to_local_fast():
    global EXCLUDE_FOLDERS, EXCLUDE_FILES

    config = load_config()

    EXCLUDE_FOLDERS = config.get("exclude_folders", []) or []
    EXCLUDE_FILES = config.get("exclude_files", []) or []

    local_path, drive_path = find_valid_paths(config)

    options = config.get("options", {}) or {}
    # defaults + normalize
    opt = {
        "time_tolerance_sec": float(options.get("time_tolerance_sec", 2.0)),
        "dry_run": bool(options.get("dry_run", False)),
        "hash_max_mb": float(options.get("hash_max_mb", 64)),  # 덮어쓰기 후보에 대해 해시 비교 허용 크기
        # (수정) config 반영
        "hash_compare_on_update": bool(options.get("hash_compare_on_update", True)),
        "trash_folder_name": str(options.get("trash_folder_name", ".sync_trash")),
        "log_folder_name": str(options.get("log_folder_name", "sync_logs")),
        "backup_enable": bool(options.get("backup_enable", True)),
        "backup_folder_name": str(options.get("backup_folder_name", "overwritten")),
        "backup_local_before_overwrite": bool(options.get("backup_local_before_overwrite", True)),
        "backup_raw_exts": [str(x).lower() for x in (options.get("backup_raw_exts", []) or [])],
        "backup_keep_days": int(options.get("backup_keep_days", 5)),
        "backup_max_gb": float(options.get("backup_max_gb", 10)),
    }

    # trash는 스캔 제외 (루프/불필요 방지)
    if opt["trash_folder_name"] not in EXCLUDE_FOLDERS:
        EXCLUDE_FOLDERS.append(opt["trash_folder_name"])

    dry_run = opt["dry_run"]
    tol = opt["time_tolerance_sec"]
    hash_max_bytes = int(opt["hash_max_mb"] * 1024 * 1024)

    log_path = setup_logger(opt["log_folder_name"])
    log_append(log_path, f"[START] {datetime.now().isoformat(timespec='seconds')}")
    log_append(log_path, f"mode=drive_to_local_fast")
    log_append(log_path, f"local={local_path}")
    log_append(log_path, f"drive={drive_path}")
    log_append(log_path, f"dry_run={dry_run}, tol={tol}, hash_max_mb={opt['hash_max_mb']}")
    log_append(log_path, f"hash_compare_on_update={opt['hash_compare_on_update']}")
    log_append(log_path, f"backup_enable={opt['backup_enable']}, backup_local_before_overwrite={opt['backup_local_before_overwrite']}")
    log_append(log_path, f"backup_folder={opt['backup_folder_name']}, raw_exts={opt['backup_raw_exts']}")

    print("=" * 60)
    print("  단방향 동기화(고속): Drive(원본) → Local(미러)")
    print("=" * 60)
    print(f"[Local] {local_path}")
    print(f"[Drive] {drive_path}")
    print()

    if not Path(drive_path).exists():
        print(f"[오류] Drive 경로가 존재하지 않습니다: {drive_path}")
        raise SystemExit(1)

    snapshot = load_snapshot()

    print("[1/4] Drive 스캔(고속) 중...")
    drive_files = scan_drive_fast(drive_path, EXCLUDE_FOLDERS, EXCLUDE_FILES)
    print(f"  - Drive: {len(drive_files)}개")
    print(f"  - Snapshot: {len(snapshot)}개")

    local_cache: Dict[str, Optional[FileMeta]] = {}

    to_copy: List[str] = []
    to_trash: List[str] = []
    skipped_local_newer: List[str] = []
    skipped_hash_same: List[str] = []
    skipped_deleted_but_local_modified: List[str] = []

    # 1) Drive 기준 업데이트/신규
    print("[2/4] 변경 분석 중(Drive 기준)...")
    for rel, dmeta in drive_files.items():
        lmeta = get_local_meta(local_path, rel, local_cache)

        if lmeta is None:
            to_copy.append(rel)
            continue

        dt = dmeta.mtime - lmeta.mtime
        if dt > tol:
            # Drive가 최신 -> 덮어쓰기 후보
            # 해시 비교는 "크기 제한" 내에서만 (너무 큰 파일은 해시가 오히려 병목)
            if opt["hash_compare_on_update"] and dmeta.size == lmeta.size and dmeta.size <= hash_max_bytes:
                try:
                    dh = sha256_of_file(Path(drive_path) / rel)
                    lh = sha256_of_file(Path(local_path) / rel)
                    if dh == lh:
                        skipped_hash_same.append(rel)
                        continue
                except Exception:
                    # 해시 실패 시 Drive 우선
                    pass

            to_copy.append(rel)
            continue

        if (-dt) > tol:
            # Local이 더 최신 -> 덮어쓰기 금지
            skipped_local_newer.append(rel)
            continue

        # tolerance 이내 -> noop

    # 2) 삭제(Drive에 없어진 것) 반영: snapshot 기준으로만 판단
    print("[2/4] 삭제 반영 분석 중(Snapshot 기준)...")
    for rel, smeta in snapshot.items():
        if rel in drive_files:
            continue  # 여전히 존재
        lmeta = get_local_meta(local_path, rel, local_cache)
        if lmeta is None:
            continue  # 로컬에도 없으면 이미 정리됨

        # 로컬이 스냅샷 이후 바뀌었으면(수정됨) 삭제(휴지통)하지 않음
        # size=-1(구스냅샷)인 경우 mtime만으로 판단
        if smeta.size != -1:
            changed = (abs(lmeta.mtime - smeta.mtime) > tol) or (lmeta.size != smeta.size)
        else:
            changed = (abs(lmeta.mtime - smeta.mtime) > tol)

        if changed:
            skipped_deleted_but_local_modified.append(rel)
            continue

        to_trash.append(rel)

    print()
    print("[3/4] 작업 계획")
    print(f"  - Drive → Local 복사/갱신: {len(to_copy)}")
    print(f"  - Drive 삭제 반영(Local 휴지통): {len(to_trash)}")
    print(f"  - 스킵(Local 최신): {len(skipped_local_newer)}")
    print(f"  - 스킵(해시 동일): {len(skipped_hash_same)}")
    print(f"  - 스킵(Drive 삭제지만 Local 수정됨): {len(skipped_deleted_but_local_modified)}")
    print()

    if dry_run:
        print("[DRY-RUN] 실제 파일 작업은 수행하지 않습니다.\n")

    errors: List[str] = []

    # 실행
    print("[4/4] 작업 실행 중...")
    raw_exts_lower = opt["backup_raw_exts"]

    for rel in to_copy:
        try:
            src = Path(drive_path) / rel
            dst = Path(local_path) / rel

            print(f"[COPY] {rel}")
            log_append(log_path, f"[COPY] {rel}")

            # 덮어쓰기 전 Local 백업(규약)
            archive_local_overwritten(dst, rel, opt, raw_exts_lower, dry_run, log_path)

            if not dry_run:
                dst.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(src, dst)

        except Exception as e:
            emsg = f"[ERROR] COPY {rel}: {e}"
            print(emsg)
            log_append(log_path, emsg)
            errors.append(emsg)

    for rel in to_trash:
        try:
            print(f"[TRASH] {rel}")
            log_append(log_path, f"[TRASH] {rel}")
            moved = move_to_trash_local(local_path, rel, opt["trash_folder_name"], dry_run)
            log_append(log_path, f"    -> trashed_to: {moved.as_posix()}")
        except Exception as e:
            emsg = f"[ERROR] TRASH {rel}: {e}"
            print(emsg)
            log_append(log_path, emsg)
            errors.append(emsg)

    # 스냅샷 저장(Drive 메타가 곧 원본 상태)
    if not dry_run:
        save_snapshot(drive_files, opt)

    # 백업 정리
    if opt["backup_enable"]:
        backup_root = SCRIPT_DIR / opt["backup_folder_name"]
        keep_days = opt["backup_keep_days"]
        max_bytes = int(opt["backup_max_gb"] * 1024 * 1024 * 1024)
        prune_backup_by_days(backup_root, keep_days, dry_run, log_path)
        prune_backup_by_size(backup_root, max_bytes, dry_run, log_path, keep_days)

    print("\n" + "=" * 60)
    print("완료: Drive → Local 단방향 동기화(고속)")
    print(f"  적용(복사/갱신): {len(to_copy)}")
    print(f"  삭제 반영(휴지통): {len(to_trash)}")
    print(f"  스킵(Local 최신): {len(skipped_local_newer)}")
    print(f"  스킵(해시 동일): {len(skipped_hash_same)}")
    print(f"  스킵(Drive 삭제지만 Local 수정됨): {len(skipped_deleted_but_local_modified)}")
    print(f"  Snapshot: {len(drive_files)}")
    print(f"  Log: {log_path}")
    if errors:
        print(f"  [경고] 오류 {len(errors)}건 발생(로그 확인)")
    print("=" * 60)

    log_append(log_path, f"[END] {datetime.now().isoformat(timespec='seconds')}")
    if errors:
        log_append(log_path, f"errors={len(errors)}")


if __name__ == "__main__":
    try:
        sync_drive_to_local_fast()
    except Exception as e:
        print(f"\n[오류 발생] {e}")
    input("\n아무 키나 누르면 종료...")