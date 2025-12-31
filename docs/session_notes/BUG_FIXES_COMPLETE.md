# MicroPython Bug Fixes Summary

This document summarizes all security and operational bug fixes applied to this fork.

## Overview

A total of **18 bugs** have been fixed across multiple sessions, addressing:
- Integer/buffer overflow vulnerabilities
- Null pointer dereferences and segfaults
- Type confusion vulnerabilities
- Unicode handling issues
- Input validation gaps

---

## Session 1 Fixes (8 bugs)

### 1. Integer Overflow in Array Constructor (Issue #18620)
**File:** `py/objarray.c`
**Problem:** `array_new()` could overflow when calculating buffer size for large arrays.
**Fix:** Added overflow check using `SIZE_MAX / n` before allocation.
```c
if (n > 0 && (size_t)typecode_size > SIZE_MAX / n) {
    mp_raise_msg(&mp_type_OverflowError, MP_ERROR_TEXT("array too large"));
}
```

### 2. Buffer Overflow in Bytearray Constructor (Issue #18617)
**File:** `py/objarray.c`
**Problem:** `array_extend_impl()` didn't validate iterator length against preallocated size.
**Fix:** Added bounds check when iterating.
```c
if (i >= len) {
    mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("iterator length mismatch"));
}
```

### 3. Type Confusion in __build_class__ (Issue #18618)
**File:** `py/modbuiltins.c`
**Problem:** `__build_class__` assumed cell argument was valid without type checking.
**Fix:** Added type validation before calling `mp_obj_cell_set()`.
```c
if (!mp_obj_is_type(cell, &mp_type_cell)) {
    mp_raise_TypeError(MP_ERROR_TEXT("__class__ cell is not a cell"));
}
```

### 4. Dict Equality Null Pointer Dereference (Issue #18619)
**File:** `py/objdict.c`
**Problem:** `dict_iter_next()` could dereference NULL if dict was modified during iteration.
**Fix:** Added NULL checks for `map->table` before and after key comparison.

### 5. JSON Namedtuple to List Conversion (Issue #18198)
**File:** `py/objnamedtuple.c`
**Problem:** JSON serialization of namedtuples produced invalid output.
**Fix:** Detect `PRINT_JSON` kind and delegate to `mp_obj_tuple_print()`.
```c
if (MICROPY_PY_JSON && kind == PRINT_JSON) {
    mp_obj_tuple_print(print, o_in, kind);
    return;
}
```

### 6. MICROPY_TIME_SUPPORT Undefined Variable (Issue #18203)
**File:** `py/mpconfig.h`
**Problem:** `MP_SSIZE_MAX` was used before being defined.
**Fix:** Changed to use `UINTPTR_MAX` which is always available.
```c
#if UINTPTR_MAX > 0xFFFFFFFF
```

### 7. Memoryview Safety on Array Resize (Issues #18171, #18168)
**Files:** `py/objarray.c`, `py/objarray.h`
**Problem:** Resizing an array could invalidate memoryviews pointing to its buffer.
**Fix:** Added `MP_OBJ_ARRAY_TYPECODE_FLAG_VIEW_EXPORTED` flag to track buffer exports and prevent resizing.

### 8. Non-UTF8 Identifiers Allowed (Issue #18609)
**File:** `py/lexer.c`
**Problem:** Invalid UTF-8 byte sequences could be used as identifiers.
**Fix:** Added UTF-8 validation functions for identifier parsing.
```c
static bool is_valid_utf8_lead(unichar c) {
    return (c >= 0xC2 && c <= 0xDF) ||
           (c >= 0xE0 && c <= 0xEF) ||
           (c >= 0xF0 && c <= 0xF4);
}
```

---

## Session 2 Fixes (4 new bugs + validation of 6 already fixed)

### 9. super() Improper Use Segfault (Issue #17728)
**File:** `py/objtype.c` (lines 1336-1339)
**Problem:** `super()` crashed when first argument was not a type object.
**Fix:** Added type validation at function entry.
```c
if (!mp_obj_is_type(args[0], &mp_type_type)) {
    mp_raise_TypeError(MP_ERROR_TEXT("super() argument 1 must be a type"));
}
```

### 10. compile() Assertion on Invalid String (Issue #17817)
**File:** `py/builtinevex.c` (lines 83-92)
**Problem:** `compile()` crashed on non-string arguments.
**Fix:** Added string type validation for all three arguments.
```c
if (!mp_obj_is_str(args[0])) {
    mp_raise_TypeError(MP_ERROR_TEXT("source must be a string"));
}
```

### 11. re.compile Crash with Invalid Pattern (Issue #17720)
**File:** `extmod/modre.c` (lines 450-453)
**Problem:** `re.compile()` crashed when given non-string pattern.
**Fix:** Added type validation for pattern argument.
```c
if (!mp_obj_is_str(args[0]) && !mp_obj_is_type(args[0], &mp_type_bytes)) {
    mp_raise_TypeError(MP_ERROR_TEXT("pattern must be a string or bytes"));
}
```

### 12. str.center() Unicode Bug (Issue #17827)
**File:** `py/objstr.c` (lines 945-977)
**Problem:** `str.center()` used byte length instead of character length for unicode.
**Fix:** Use `utf8_charlen()` for unicode strings and calculate padding correctly.
```c
#if MICROPY_PY_BUILTINS_STR_UNICODE
if (mp_obj_is_str(str_in)) {
    char_len = utf8_charlen(str, str_len);
}
#endif
```

---

## Previously Fixed (Validated in Session 2)

The following bugs were already fixed in previous sessions:
- uctypes.bytes_at segfault with address 0 (Issue #18172)
- uctypes.bytearray_at negative address (Issue #18166)
- Viper ptr8(0) SIGSEGV (Issue #18169)
- io.BufferedWriter crash (Issue #17727)
- websocket crash on corrupt data (Issue #17818)
- socket.recv() negative buffer size (Issue #17819)

---

## Files Modified

| File | Changes |
|------|---------|
| `py/objarray.c` | Overflow checks, bounds validation, memoryview safety |
| `py/objarray.h` | Added `MP_OBJ_ARRAY_TYPECODE_FLAG_VIEW_EXPORTED` |
| `py/objdict.c` | NULL pointer protection in iteration |
| `py/modbuiltins.c` | Type check for __build_class__ cell |
| `py/objcell.c` | Made mp_type_cell non-static |
| `py/obj.h` | Added extern declaration for mp_type_cell |
| `py/objnamedtuple.c` | JSON serialization fix |
| `py/mpconfig.h` | Fixed undefined MP_SSIZE_MAX |
| `py/lexer.c` | UTF-8 validation for identifiers |
| `py/objtype.c` | super() argument validation |
| `py/builtinevex.c` | compile() string validation |
| `extmod/modre.c` | re.compile pattern validation |
| `py/objstr.c` | str.center() unicode fix |

---

## Testing

See `tests/bug_fixes/` for test cases covering these fixes.

## Security Impact

These fixes address several security-relevant issues:
- **Memory safety:** Prevent buffer overflows and use-after-free
- **Denial of service:** Prevent crashes from malformed input
- **Type safety:** Ensure proper type checking before operations
