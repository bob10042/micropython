# MicroPython & PyBoard Native Firmware - Master Index

**Repository:** `c:\Users\bob43\Downloads\micropython`  
**Last Updated:** December 28, 2025

## Quick Reference

### Flashing Firmware

**MicroPython (COM3 board):**
```powershell
# Using DFU mode (hold DFU button, plug USB)
python -m dfu --util -D PYBV11-v1.28-PATCHED.dfu

# Or using mpremote
mpremote connect COM3 bootloader
# Then use STM32CubeProgrammer
```

**C Firmware (COM11 board):**
```powershell
# Using STM32CubeProgrammer or ST-Link
STM32_Programmer_CLI -c port=SWD -w "PyBoard_Native_v3.0.hex" -v -rst
```

---

## Firmware Files

### MicroPython Firmware
| File | Description | Size |
|------|-------------|------|
| `PYBV11-v1.28-PATCHED.hex` | Bytecode MicroPython | 368KB |
| `PYBV11-v1.28-PATCHED.dfu` | DFU format | 377KB |
| `ports/stm32/build-PYBV11/firmware.hex` | Native compiled | 410KB |

### C Firmware (in `micropython testing software/PyBoard_Native/firmware_releases/`)
| File | Version | Size | Description |
|------|---------|------|-------------|
| `PyBoard_Native_v1.0_working.hex` | 1.0 | 170KB | Initial release |
| `PyBoard_Native_v2.0_working.hex` | 2.0 | 204KB | Full peripherals |
| `PyBoard_Native_v2.1_LED4_PWM.hex` | 2.1 | 204KB | LED PWM |
| `PyBoard_Native_v3.0_MicroPython_Compatible.hex` | 3.0 | 283KB | **Latest** |

---

## Documentation

### Main Documentation
| File | Description |
|------|-------------|
| [README.md](micropython%20testing%20software/PyBoard_Native/README.md) | C Firmware overview |
| [RELEASE_NOTES_v3.0.md](micropython%20testing%20software/PyBoard_Native/RELEASE_NOTES_v3.0.md) | v3.0 features & commands |
| [SESSION_28_DEC_2025.md](SESSION_28_DEC_2025.md) | Today's session summary |

### Build & Development
| File | Description |
|------|-------------|
| [BUILD_ENVIRONMENT.md](BUILD_ENVIRONMENT.md) | WSL2/Ubuntu setup |
| [BUILD_NATIVE_FIRMWARE.md](BUILD_NATIVE_FIRMWARE.md) | Native compilation |
| [FIRMWARE_DEVELOPMENT_NOTES.md](micropython%20testing%20software/PyBoard_Native/FIRMWARE_DEVELOPMENT_NOTES.md) | C firmware notes |

### Bug Fixes
| File | Description |
|------|-------------|
| [BUGS_FIXED_SUMMARY.md](BUGS_FIXED_SUMMARY.md) | All bug fixes |
| [BUG_FIXES_SESSION3.md](BUG_FIXES_SESSION3.md) | Native emitter fix |

---

## Hardware

### COM3 - MicroPython Board
- PyBoard v1.1
- STM32F405RG @ 168MHz
- MicroPython v1.28.0-preview
- SD card: Working

### COM11 - C Firmware Board
- PyBoard v1.1  
- STM32F405RG @ 168MHz
- Native C Firmware v3.0
- SD card: Broken (hardware)

---

## Key Directories

```
c:\Users\bob43\Downloads\micropython\
├── micropython testing software\
│   └── PyBoard_Native\           # C firmware project
│       ├── Core\Src\main.c       # Main source
│       ├── Debug\                # Build output
│       ├── firmware_releases\    # Release archives
│       └── backups\              # Timestamped backups
├── ports\stm32\                  # MicroPython STM32 port
├── py\                           # MicroPython core
└── mpy-cross\                    # Native compiler
```

---

## Build Commands

### MicroPython (WSL2)
```bash
cd ~/micropython/ports/stm32
make BOARD=PYBV11 -j4
```

### C Firmware (PowerShell)
```powershell
cd "micropython testing software\PyBoard_Native"
.\build.ps1 -Rebuild
```

---

## Test Scripts

| Script | Purpose |
|--------|---------|
| `test_viper.py` | Test @viper and @native |
| `test_c_firmware.py` | Test all C firmware commands |
| `pyboard_terminal.py` | Interactive serial terminal |

---

## Performance Benchmarks

### 10K Loop Iteration Test
| Implementation | Time | Speedup |
|----------------|------|---------|
| MicroPython bytecode | 993,000 µs | 1x |
| MicroPython @native | 16,000 µs | 62x |
| MicroPython @viper | 2,500 µs | 397x |
| Native C firmware | 2,262 µs | **439x** |
