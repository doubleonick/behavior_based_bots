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
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
FIRMWARE = HERE.parent.parent / "firmware"
TESTS = {"vehicle": "Val_Vehicle", "vehicle_emotions": "Val_Vehicle", "vehicle_units": "Val_Vehicle", "vehicle_light": "Val_Vehicle",
         "field": "Val_Field",
         "field_hub": "Val_Field", "brooks": "Val_Brooks", "brooks_keys": "Val_Brooks", "unit_tester": "Unit_Tester"}


def normalise(text):
    # "-0.00" and "0.00" are the same number; don't fail on the sign of zero
    return text.replace("-0.00", "0.00")


def compiler():
    for name in ("g++", "clang++"):
        if shutil.which(name):
            return name
    sys.exit("No C++ compiler found (need g++ or clang++)")


def prototype_problems(sketch_dir):
    """The Arduino IDE writes a declaration for every function in a .ino above the first
    function it finds. A function whose return type or parameters use a type the .ino
    declares further down then fails to compile on the robot, though it compiles here.
    Report every such function."""
    problems = []
    for ino in sorted(sketch_dir.glob("*.ino")):
        lines = ino.read_text().split("\n")
        types = {}                        # type name -> line it is declared on
        first_function = None
        signature = re.compile(r"^([A-Za-z_][\w:<>\*& ]*?)\b([A-Za-z_]\w*)\s*\(([^;{]*)\)\s*(const)?\s*\{")
        for n, line in enumerate(lines):
            m = re.match(r"^(?:enum|struct|class)\s+(\w+)", line)
            if m:
                types[m.group(1)] = n
            if first_function is None and not line.startswith((" ", "\t", "#", "//")):
                if signature.match(line) and not re.match(r"^(if|for|while|switch|else|return)\b", line):
                    first_function = n
        if first_function is None:
            continue
        for n, line in enumerate(lines):
            m = signature.match(line)
            if not m or line.startswith((" ", "\t")):
                continue
            used = set(re.findall(r"\w+", m.group(1) + " " + m.group(3)))
            for t in used:
                if t in types and types[t] > first_function:
                    problems.append(f"{ino.name}:{n + 1}: {m.group(2)}() uses {t}, declared on line "
                                    f"{types[t] + 1}, after the first function (line {first_function + 1})")
    return problems


def run(test, sketch, update):
    folder = HERE / test
    sketch_dir = FIRMWARE / sketch
    problems = prototype_problems(sketch_dir)
    if problems:
        print(f"FAIL {test}: would not compile in the Arduino IDE (move the type up, above every function)")
        for p in problems:
            print("  " + p)
        return False
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
