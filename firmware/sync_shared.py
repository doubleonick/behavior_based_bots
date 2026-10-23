"""
sync_shared.py - keep the shared classes identical in every Val sketch.

Arduino needs every .cpp a sketch uses to be inside that sketch's own folder,
so the shared classes are copied into each sketch. The copies in shared/ are
the originals: edit those, then run this script.

    python firmware/sync_shared.py           copy shared/ into every sketch that uses each file
    python firmware/sync_shared.py --check   report drift without changing anything
                                             (exit code 1 if any copy differs)

A sketch gets a shared file only if it already has a copy, so a sketch that
doesn't need (say) DistanceSensor isn't given one.
"""
import filecmp
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SHARED = ROOT / "shared"
SKETCHES = ["Val_Vehicle", "Val_Field", "Val_Brooks"]


def main():
    check_only = "--check" in sys.argv
    drift = 0
    for sketch in SKETCHES:
        folder = ROOT / sketch
        if not folder.is_dir():
            continue
        for original in sorted(SHARED.iterdir()):
            copy = folder / original.name
            if not copy.exists():
                continue
            if filecmp.cmp(original, copy, shallow=False):
                continue
            drift += 1
            if check_only:
                print(f"DRIFT  {sketch}/{original.name}")
            else:
                shutil.copy2(original, copy)
                print(f"synced {sketch}/{original.name}")
    if check_only:
        if drift:
            print(f"{drift} file(s) differ from shared/")
            sys.exit(1)
        print("all copies match shared/")


if __name__ == "__main__":
    main()
