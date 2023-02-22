from glob import glob
import re

_IGNORE_FILES_ = (
    'src/main.cpp',
    )

def count_print_out(filename: str, fmt: str = 'C++'):
  with open(filename, 'r') as fin:
    ret = 0
    if "C++" in fmt.upper():
      ret += len(re.findall(r"cout *\<\<", fin.read()))
    if "C" in fmt.upper():
      ret += len(re.findall(r"[^sf]printf *\( *\"", fin.read()))
    if "PY" in fmt.upper():
      ret += len(re.findall(r"print *\(", fin.read()))

    return ret


def test_src():
  for fname in glob("src/*.cpp"):
    if fname in _IGNORE_FILES_: continue 
    c = count_print_out(fname, 'C/C++') 
    assert c == 0, f"Found {c} print-out in {fname}"

def test_include():
  for fname in glob("SQLamarr/*.h") + glob("SQLamarr/*.hpp"):
    if fname in _IGNORE_FILES_: continue 
    c = count_print_out(fname, 'C/C++') 
    assert c == 0, f"Found {c} print-out in {fname}"

def test_python():
  for fname in glob("python/SQLamarr/*.py"):
    if fname in _IGNORE_FILES_: continue 
    c = count_print_out(fname, 'py') 
    assert c == 0, f"Found {c} print-out in {fname}"

def test_setup():
  c = count_print_out("setup.py", 'py') 
  assert c == 0, f"Found {c} print-out in setup.py"


