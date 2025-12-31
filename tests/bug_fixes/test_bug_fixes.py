# Test cases for MicroPython bug fixes
# Run with: micropython test_bug_fixes.py

import sys

def test_passed(name):
    print(f"PASS: {name}")

def test_failed(name, msg=""):
    print(f"FAIL: {name} - {msg}")

# ============================================================================
# Test 1: super() with invalid first argument (Issue #17728)
# ============================================================================
def test_super_invalid_arg():
    """super() should raise TypeError when first arg is not a type"""
    try:
        super("not a type", object())
        test_failed("super_invalid_arg", "should have raised TypeError")
    except TypeError as e:
        test_passed("super_invalid_arg")
    except Exception as e:
        test_failed("super_invalid_arg", f"wrong exception: {type(e).__name__}")

# ============================================================================
# Test 2: compile() with invalid arguments (Issue #17817)
# ============================================================================
def test_compile_invalid_source():
    """compile() should raise TypeError for non-string source"""
    try:
        compile(123, "test", "exec")
        test_failed("compile_invalid_source", "should have raised TypeError")
    except TypeError:
        test_passed("compile_invalid_source")
    except Exception as e:
        test_failed("compile_invalid_source", f"wrong exception: {type(e).__name__}")

def test_compile_invalid_filename():
    """compile() should raise TypeError for non-string filename"""
    try:
        compile("pass", 123, "exec")
        test_failed("compile_invalid_filename", "should have raised TypeError")
    except TypeError:
        test_passed("compile_invalid_filename")
    except Exception as e:
        test_failed("compile_invalid_filename", f"wrong exception: {type(e).__name__}")

def test_compile_invalid_mode():
    """compile() should raise TypeError for non-string mode"""
    try:
        compile("pass", "test", 123)
        test_failed("compile_invalid_mode", "should have raised TypeError")
    except TypeError:
        test_passed("compile_invalid_mode")
    except Exception as e:
        test_failed("compile_invalid_mode", f"wrong exception: {type(e).__name__}")

def test_compile_valid():
    """compile() should work with valid arguments"""
    try:
        code = compile("x = 1", "test", "exec")
        test_passed("compile_valid")
    except Exception as e:
        test_failed("compile_valid", str(e))

# ============================================================================
# Test 3: re.compile() with invalid pattern (Issue #17720)
# ============================================================================
def test_re_compile_invalid_pattern():
    """re.compile() should raise TypeError for non-string pattern"""
    try:
        import re
        re.compile(123)
        test_failed("re_compile_invalid_pattern", "should have raised TypeError")
    except TypeError:
        test_passed("re_compile_invalid_pattern")
    except ImportError:
        print("SKIP: re_compile_invalid_pattern (re module not available)")
    except Exception as e:
        test_failed("re_compile_invalid_pattern", f"wrong exception: {type(e).__name__}")

def test_re_compile_valid():
    """re.compile() should work with valid pattern"""
    try:
        import re
        pattern = re.compile(r"\d+")
        if pattern.match("123"):
            test_passed("re_compile_valid")
        else:
            test_failed("re_compile_valid", "pattern didn't match")
    except ImportError:
        print("SKIP: re_compile_valid (re module not available)")
    except Exception as e:
        test_failed("re_compile_valid", str(e))

# ============================================================================
# Test 4: str.center() with unicode (Issue #17827)
# ============================================================================
def test_str_center_ascii():
    """str.center() should work correctly with ASCII"""
    result = "abc".center(7)
    if result == "  abc  ":
        test_passed("str_center_ascii")
    else:
        test_failed("str_center_ascii", f"got '{result}'")

def test_str_center_unicode():
    """str.center() should use character count, not byte count for unicode"""
    # Unicode string with multi-byte characters
    # Each character is 1 character but may be multiple bytes
    try:
        s = "\u4e2d\u6587"  # Chinese characters (2 chars, 6 bytes in UTF-8)
        result = s.center(6)
        # Should be "  \u4e2d\u6587  " (2 spaces + 2 chars + 2 spaces = 6 chars)
        if len(result) == 6:
            test_passed("str_center_unicode")
        else:
            test_failed("str_center_unicode", f"len={len(result)}, expected 6")
    except Exception as e:
        test_failed("str_center_unicode", str(e))

def test_str_center_emoji():
    """str.center() with emoji characters"""
    try:
        s = "\U0001F600"  # Grinning face emoji (1 char, 4 bytes)
        result = s.center(5)
        if len(result) == 5:
            test_passed("str_center_emoji")
        else:
            test_failed("str_center_emoji", f"len={len(result)}, expected 5")
    except Exception as e:
        # Some builds may not support 4-byte unicode
        print(f"SKIP: str_center_emoji ({e})")

# ============================================================================
# Test 5: Array overflow protection (Issue #18620)
# ============================================================================
def test_array_overflow():
    """array should reject sizes that would overflow"""
    try:
        import array
        # Try to create an impossibly large array
        # This should raise OverflowError or MemoryError, not crash
        a = array.array('q', range(2**62))  # 'q' is 8 bytes each
        test_failed("array_overflow", "should have raised an error")
    except (OverflowError, MemoryError, ValueError):
        test_passed("array_overflow")
    except ImportError:
        print("SKIP: array_overflow (array module not available)")
    except Exception as e:
        # Any exception is acceptable as long as it doesn't crash
        test_passed("array_overflow")

# ============================================================================
# Test 6: Bytearray extend validation (Issue #18617)
# ============================================================================
def test_bytearray_extend_mismatch():
    """bytearray.extend() should handle iterator length mismatch"""
    class BadIter:
        def __init__(self):
            self.count = 0
        def __len__(self):
            return 2  # Claims to have 2 items
        def __iter__(self):
            return self
        def __next__(self):
            self.count += 1
            if self.count > 5:  # Actually yields 5 items
                raise StopIteration
            return self.count

    try:
        ba = bytearray()
        ba.extend(BadIter())
        # Either succeeds with correct data or raises ValueError
        test_passed("bytearray_extend_mismatch")
    except ValueError:
        test_passed("bytearray_extend_mismatch")
    except Exception as e:
        test_failed("bytearray_extend_mismatch", str(e))

# ============================================================================
# Test 7: Dict modification during iteration (Issue #18619)
# ============================================================================
def test_dict_modify_during_iter():
    """dict should handle modification during iteration safely"""
    d = {i: i for i in range(10)}
    try:
        for k in d:
            if k == 5:
                d.clear()  # Modify during iteration
        # Should either complete or raise RuntimeError, not crash
        test_passed("dict_modify_during_iter")
    except RuntimeError:
        test_passed("dict_modify_during_iter")
    except Exception as e:
        test_failed("dict_modify_during_iter", str(e))

# ============================================================================
# Test 8: Namedtuple JSON serialization (Issue #18198)
# ============================================================================
def test_namedtuple_json():
    """namedtuple should serialize to JSON as a list"""
    try:
        from collections import namedtuple
        import json

        Point = namedtuple('Point', ['x', 'y'])
        p = Point(1, 2)
        result = json.dumps(p)
        # Should be "[1, 2]" not "Point(x=1, y=2)"
        if result == "[1, 2]":
            test_passed("namedtuple_json")
        else:
            test_failed("namedtuple_json", f"got {result}")
    except ImportError:
        print("SKIP: namedtuple_json (json or collections not available)")
    except Exception as e:
        test_failed("namedtuple_json", str(e))

# ============================================================================
# Run all tests
# ============================================================================
if __name__ == "__main__":
    print("=" * 60)
    print("MicroPython Bug Fix Tests")
    print("=" * 60)

    # super() tests
    test_super_invalid_arg()

    # compile() tests
    test_compile_invalid_source()
    test_compile_invalid_filename()
    test_compile_invalid_mode()
    test_compile_valid()

    # re.compile() tests
    test_re_compile_invalid_pattern()
    test_re_compile_valid()

    # str.center() tests
    test_str_center_ascii()
    test_str_center_unicode()
    test_str_center_emoji()

    # Array tests
    test_array_overflow()
    test_bytearray_extend_mismatch()

    # Dict tests
    test_dict_modify_during_iter()

    # Namedtuple tests
    test_namedtuple_json()

    print("=" * 60)
    print("Tests complete")
    print("=" * 60)
