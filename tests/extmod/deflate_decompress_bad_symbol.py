# test that a Huffman code missing from the tree is a data error (AUD-035)

try:
    import deflate, io
except ImportError:
    print("SKIP")
    raise SystemExit

# a corrupt raw stream (found by fuzzing) in which tinf_decode_symbol() fails
data = bytes.fromhex("25ff70b170684b7c68ff000070b1680070b168")

for wbits in (8, 10, 14, 15):
    try:
        deflate.DeflateIO(io.BytesIO(data), deflate.RAW, wbits).read()
        print(wbits, "no error")
    except OSError as e:
        print(wbits, "OSError", e.errno)
    except Exception as e:
        print(wbits, type(e).__name__)
