"""
run.py - run the firmware's host tests.

Each test compiles a REAL sketch (firmware/<Sketch>/*.ino + .cpp) together with fake
Arduino libraries (tests/host/<test>/stub/) and a scripted story (main.cpp) that
presses the button and moves sensor readings over time. Its Serial output is then
compared with expected.txt.

    python tests/host/run.py            run all tests, compare with expected output
    python tests/host/run.py --update   accept the current output as the new expected

Needs a C++17 compiler (g++ or clang++). This checks LOGIC (setup flow, detection,
calibration, wiring sums, potential field); it cannot check timing on the real
ESP32, LED colours, or anything electrical. Say so when reporting results.
"""
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
FIRMWARE = HERE.parent.parent / "firmware"
TESTS = {"vehicle": "Val_Vehicle", "field": "Val_Field", "field_hub": "Val_Field"}


def normalise(text):
    # "-0.00" and "0.00" are the same number; don't fail on the sign of zero
    return text.replace("-0.00", "0.00")


def compiler():
    for name in ("g++", "clang++"):
        if shutil.which(name):
            return name
    sys.exit("No C++ compiler found (need g++ or clang++)")


def run(test, sketch, update):
    folder = HERE / test
    sketch_dir = FIRMWARE / sketch
    sources = [str(p) for p in sorted(sketch_dir.glob("*.cpp"))]
    with tempfile.TemporaryDirectory() as tmp:
        exe = Path(tmp) / "test"
        cmd = [compiler(), "-std=gnu++17", "-I", str(folder / "stub"), "-I", str(sketch_dir),
               "-o", str(exe), str(folder / "main.cpp")] + sources
        built = subprocess.run(cmd, capture_output=True, text=True)
        if built.returncode != 0:
            print(f"FAIL {test}: does not compile\n{built.stderr}")
            return False
        output = subprocess.run([str(exe)], capture_output=True, text=True, timeout=120).stdout
    expected_file = folder / "expected.txt"
    if update or not expected_file.exists():
        expected_file.write_text(output)
        print(f"wrote {expected_file.relative_to(HERE.parent.parent)}")
        return True
    if normalise(output) == normalise(expected_file.read_text()):
        print(f"ok   {test}")
        return True
    actual_file = folder / "actual.txt"
    actual_file.write_text(output)
    print(f"FAIL {test}: output differs; compare {expected_file.name} with {actual_file.name}")
    return False


def main():
    update = "--update" in sys.argv
    results = [run(test, sketch, update) for test, sketch in TESTS.items()]
    if not all(results):
        sys.exit(1)


if __name__ == "__main__":
    main()
