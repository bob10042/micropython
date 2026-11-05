# Test that a block device whose sync ioctl fails (it could not write its
# cached data out) makes the FAT file operation that asked for the sync fail.

try:
    import errno, os, vfs

    vfs.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBDevCached:
    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)
        self.sync_ret = 0

    def readblocks(self, n, buf):
        buf[:] = self.data[n * self.SEC_SIZE : n * self.SEC_SIZE + len(buf)]

    def writeblocks(self, n, buf):
        self.data[n * self.SEC_SIZE : n * self.SEC_SIZE + len(buf)] = buf

    def ioctl(self, op, arg):
        if op == 3:  # MP_BLOCKDEV_IOCTL_SYNC
            return self.sync_ret
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMBDevCached(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

vfs.VfsFat.mkfs(bdev)
fs = vfs.VfsFat(bdev)

# Sync succeeds: close works and the data reads back.
f = fs.open("ok.txt", "w")
f.write("hello")
f.close()
print(fs.open("ok.txt", "r").read())

# Sync fails: flush and close must raise, not report success.
f = fs.open("bad.txt", "w")
f.write("data")
bdev.sync_ret = -errno.EIO
try:
    f.flush()
    print("flush: no error")
except OSError as er:
    print("flush:", er.errno == errno.EIO)
bdev.sync_ret = 0
f.close()

f = fs.open("bad2.txt", "w")
f.write("data")
bdev.sync_ret = -errno.EIO
try:
    f.close()
    print("close: no error")
except OSError as er:
    print("close:", er.errno == errno.EIO)

# The device recovers: later operations work again.
bdev.sync_ret = 0
f = fs.open("ok2.txt", "w")
f.write("again")
f.close()
print(fs.open("ok2.txt", "r").read())
