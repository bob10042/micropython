# test that compile() records the file name it is given (AUD-038)

try:
    compile
except NameError:
    print("SKIP")
    raise SystemExit

import io, sys

for name in ("file_one", "another_file_name_that_is_long"):
    c = compile("x = 1\n1 // 0\n", name, "exec")
    try:
        exec(c)
    except ZeroDivisionError as e:
        s = io.StringIO()
        sys.print_exception(e, s)
        print(name in s.getvalue())
    try:
        print(c.co_filename == name)
    except AttributeError:
        print(True)
