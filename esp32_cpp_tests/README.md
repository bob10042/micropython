# ESP32 C++ Examples

This directory contains C++ examples for the ESP32 using the Espressif IoT Development Framework (ESP-IDF).

## Prerequisites

You need to have **ESP-IDF** installed and configured.

### 1. Install ESP-IDF

If you haven't installed it yet, follow the official guide:
- [ESP-IDF Get Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

### 2. Set up Environment

Before building, you must export the ESP-IDF environment variables.

**Windows (PowerShell):**
```powershell
# Run the export script from your ESP-IDF installation directory
# Example:
C:\Users\bob43\esp\esp-idf\export.ps1
```

**Linux/macOS:**
```bash
. $HOME/esp/esp-idf/export.sh
```

## Building and Flashing

Navigate to an example directory (e.g., `01_blink`) and run the following commands:

### 1. Set Target (First time only)
If you are using an ESP32-S3, C3, etc., set the target first. Default is `esp32`.
```bash
idf.py set-target esp32
```

### 2. Build
```bash
idf.py build
```

### 3. Flash
Replace `PORT` with your serial port (e.g., `COM3` on Windows, `/dev/ttyUSB0` on Linux).
```bash
idf.py -p PORT flash
```

### 4. Monitor Output
To see the serial output:
```bash
idf.py -p PORT monitor
```

You can combine flash and monitor:
```bash
idf.py -p PORT flash monitor
```

## Directory Structure

```
esp32_cpp_tests/
├── 01_blink/
│   ├── CMakeLists.txt      # Project CMake file
│   └── main/
│       ├── CMakeLists.txt  # Component CMake file
│       └── main.cpp        # C++ Source code
└── README.md               # This file
```
