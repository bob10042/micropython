# test that a native .mpy file with bad relocation data is refused (AUD-029)
# requires: slice

try:
    import sys, io, vfs

    sys.implementation._mpy
    io.IOBase
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

mpy_arch = sys.implementation._mpy >> 8
if mpy_arch >> 2 == 0:
    # This system does not support .mpy files containing native code
    print("SKIP")
    raise SystemExit


class UserFile(io.IOBase):
    def __init__(self, data):
        self.data = memoryview(data)
        self.pos = 0

    def readinto(self, buf):
        n = min(len(buf), len(self.data) - self.pos)
        buf[:n] = self.data[self.pos : self.pos + n]
        self.pos += n
        return n

    def ioctl(self, req, arg):
        if req == 4:  # MP_STREAM_CLOSE
            return 0
        return -1


class UserFS:
    def __init__(self, files):
        self.files = files

    def mount(self, readonly, mksfs):
        pass

    def umount(self):
        pass

    def stat(self, path):
        if path in self.files:
            return (32768, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        raise OSError

    def open(self, path, mode):
        return UserFile(self.files[path])


# these are the test .mpy files
small_int_bits = 30
valid_header = bytes([77, 6, (mpy_arch & 0x3F), small_int_bits])


def viper_mod(name, reloc):
    # a module with one viper child of 8 bytes of dummy code that needs relocating
    return (
        valid_header
        + b"\x02"  # n_qstr
        + b"\x00"  # n_obj
        + bytes([(len(name) + 3) << 1])
        + name
        + b".py\x00"  # qstr0 = name + ".py"
        + b"\x0aouter\x00"  # qstr1 = "outer"
        + b"\x2c"  # 5 bytes, have children, bytecode
        + b"\x00\x02"  # prelude
        + b"\x01"  # simple name (qstr index)
        + b"\x51"  # LOAD_CONST_NONE
        + b"\x63"  # RETURN_VALUE
        + b"\x01"  # 1 child
        + b"\x42"  # 8 bytes, no children, viper code
        + b"\x00\x00\x00\x00\x00\x00\x00\x00"  # dummy machine code
        + b"\x10"  # scope_flags: VIPERRELOC
        + reloc  # relocation data, ended by the end of the file
    )


user_files = {
    # valid: no adjustments
    "/mod0.mpy": viper_mod(b"mod0", b""),
    # valid: one adjustment to the first word of text
    "/mod1.mpy": viper_mod(b"mod1", b"\x01\x00"),
    # an adjustment before any location was set
    "/mod2.mpy": viper_mod(b"mod2", b"\x00"),
    # a location far outside the text
    "/mod3.mpy": viper_mod(b"mod3", b"\x01\x80\x80\x04"),
    # a location inside the text but too many adjustments
    "/mod4.mpy": viper_mod(b"mod4", b"\x03\x00\x80\x08"),
    # an entry far past the end of mp_fun_table
    "/mod5.mpy": viper_mod(b"mod5", b"\xfd\x00"),
}

# create and mount a user filesystem
vfs.mount(UserFS(user_files), "/userfs")
sys.path.append("/userfs")

# import .mpy files from the user filesystem
for i in range(len(user_files)):
    mod = "mod%u" % i
    try:
        __import__(mod)
        print(mod, "OK")
    except ValueError as er:
        print(mod, "ValueError", er)

# unmount and undo path addition
vfs.umount("/userfs")
sys.path.pop()
