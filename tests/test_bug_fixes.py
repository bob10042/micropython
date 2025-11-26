"""
Test cases for bug fixes in this branch

These tests verify the fixes for issues:
- #17714: virtpin crash with improper usage
- #17719: SSL module crash on closed socket
- #17827: str.center() bug with unicode
- #17710: uctypes element assignment through PTR
- #17726: uctypes calculates incorrect structure size
"""

# Test #17827: str.center() unicode bug
def test_str_center_unicode():
    """Test that str.center() correctly handles unicode characters"""
    result = '°'.center(4)
    assert len(result) == 4, f"Expected length 4, got {len(result)}"
    print("✓ Test #17827 passed: str.center() unicode")

# Test #17710: uctypes element assignment through PTR
def test_uctypes_ptr_assignment():
    """Test that uctypes allows assignment through PTR"""
    try:
        import uctypes

        # Find pointer size
        ptr_size = uctypes.sizeof((uctypes.PTR, uctypes.INT8))
        UINTPTR = uctypes.UINT64 if ptr_size == 8 else uctypes.UINT32

        # Descriptor for ptr test
        descr = {
            'ptr': (uctypes.PTR | 0, uctypes.INT8),
            'val': UINTPTR | 0
        }

        # Create buffers
        b1 = bytearray(uctypes.sizeof(descr))
        b2 = bytearray(8)
        x = uctypes.struct(uctypes.addressof(b1), descr)
        x.val = uctypes.addressof(b2)

        ptr = x.ptr

        # Test write through pointer
        for i in range(8):
            ptr[i] = i

        # Verify write succeeded
        for i in range(8):
            assert b2[i] == i, f"Expected b2[{i}] == {i}, got {b2[i]}"

        print("✓ Test #17710 passed: uctypes PTR assignment")
    except ImportError:
        print("⊘ Test #17710 skipped: uctypes not available")

# Test #17726: uctypes structure size
def test_uctypes_struct_size():
    """Test that uctypes calculates correct structure sizes"""
    try:
        import uctypes

        si32 = {'v': uctypes.INT32}
        si16 = {'v': uctypes.INT16}
        si = {'v1': (0, si32), 'v2': (4, si32), 'v3': (8, si16), 'v4': (10, si16)}

        size = uctypes.sizeof(si)
        # Should be 12 bytes, not 16
        assert size == 12, f"Expected size 12, got {size}"

        print("✓ Test #17726 passed: uctypes struct size")
    except ImportError:
        print("⊘ Test #17726 skipped: uctypes not available")

# Test #17714: virtpin crash (can't easily test without proper Pin implementation)
def test_virtpin_validation():
    """Test that virtpin validates objects properly"""
    try:
        import machine

        # This should raise TypeError, not segfault
        try:
            machine.time_pulse_us(object(), 1)
            print("✗ Test #17714 failed: Should have raised TypeError")
        except TypeError as e:
            print("✓ Test #17714 passed: virtpin validation")
    except (ImportError, AttributeError):
        print("⊘ Test #17714 skipped: machine.time_pulse_us not available")

# Test #17719: SSL crash on closed socket (requires SSL support)
def test_ssl_closed_socket():
    """Test that SSL methods don't crash on closed sockets"""
    try:
        import io
        import ssl

        class TestSocket(io.IOBase):
            def write(self, buf):
                return len(buf)
            def readinto(self, buf):
                return 0
            def ioctl(self, cmd, arg):
                return 0
            def setblocking(self, value):
                pass

        ss = ssl.wrap_socket(TestSocket(), server_side=1, do_handshake=0)
        ss.close()

        # This should raise OSError, not segfault
        try:
            ss.setblocking(0)
            print("✗ Test #17719 failed: Should have raised OSError")
        except OSError:
            print("✓ Test #17719 passed: SSL closed socket")
    except (ImportError, AttributeError):
        print("⊘ Test #17719 skipped: SSL not available")

if __name__ == "__main__":
    print("Running bug fix tests...\n")
    test_str_center_unicode()
    test_uctypes_ptr_assignment()
    test_uctypes_struct_size()
    test_virtpin_validation()
    test_ssl_closed_socket()
    print("\nAll tests completed!")
