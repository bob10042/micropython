# MicroPython on Raspberry Pi 5

## Important: MicroPython vs Raspberry Pi Boards

There's often confusion about "Raspberry Pi" and MicroPython. Here's the breakdown:

### Two Different Things:

#### 1. **Raspberry Pi Pico/Pico W/Pico 2** (RP2040/RP2350 chips)
- **Microcontrollers** (~$4-6)
- **Full native MicroPython support** via `rp2` port
- Runs MicroPython firmware directly on bare metal
- 264KB-520KB RAM, 2MB+ flash
- **This IS what MicroPython is designed for** ✅

#### 2. **Raspberry Pi 3/4/5** (BCM2711/BCM2712 chips)
- **Full Linux computers** ($35-$80)
- Run Raspberry Pi OS (Debian Linux)
- 2GB-8GB RAM, uses SD card storage
- **NOT a primary MicroPython target** ⚠️

## MicroPython on Raspberry Pi 5

### Current Status: **No Dedicated Port**

**Raspberry Pi 5 is NOT supported as a native MicroPython platform.** Here's why:

1. **It's a Linux computer**, not a microcontroller
2. **MicroPython targets bare-metal microcontrollers**
3. Pi 5 runs full Debian Linux with Python already installed

### How to Run MicroPython on Pi 5

You have **two options**:

#### Option 1: Unix Port (Standard Approach)
Run MicroPython as a Linux application:

```bash
# On Raspberry Pi 5 running Raspberry Pi OS
sudo apt install build-essential git python3 pkg-config libffi-dev
git clone https://github.com/micropython/micropython.git
cd micropython
make -C mpy-cross
cd ports/unix
make submodules
make

# Run MicroPython
./build-standard/micropython
```

**What you get:**
- ✅ MicroPython REPL and interpreter
- ✅ Python compatibility
- ❌ NO GPIO access (no `machine` module)
- ❌ NO hardware control (designed for desktop use)

#### Option 2: Use CPython Instead (Recommended)
Raspberry Pi 5 already has Python 3.11+ installed:

```bash
# Use regular Python with GPIO libraries
pip3 install gpiozero RPi.GPIO lgpio

# Or use modern GPIO library
pip3 install gpiod
```

**For Raspberry Pi 5, use CPython, not MicroPython!**

## What About GPIO on Pi 5?

If you want to control GPIO on Raspberry Pi 5:

### **Don't use MicroPython** - Use these instead:

```python
# Option 1: gpiozero (easiest, recommended)
from gpiozero import LED
led = LED(17)
led.on()

# Option 2: RPi.GPIO (legacy)
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(17, GPIO.OUT)
GPIO.output(17, GPIO.HIGH)

# Option 3: python-periphery (modern)
from periphery import GPIO
gpio = GPIO("/dev/gpiochip0", 17, "out")
gpio.write(True)
```

## Issues Related to Pi 5

### MicroPython Repository:
**Zero issues specifically for Raspberry Pi 5** - Because it's not a supported platform!

### Related Issues (for context):

**#17399** - Unix port build documentation
- Request: Better docs for building Unix port
- Status: Open
- Note: Would apply to Pi 5 if you build Unix port

**#16803** - Unix port missing socket.sendall()
- Request: Add missing socket method
- Status: Open
- Would affect Pi 5 if using Unix port

## Why MicroPython Doesn't Target Pi 5

### Design Philosophy:
MicroPython is designed for **resource-constrained microcontrollers**:
- 16KB - 1MB RAM
- Limited flash storage
- Bare-metal execution
- Real-time hardware control

### Raspberry Pi 5 has:
- 4-8GB RAM (4,000x more than a Pico!)
- Full Linux OS
- Multi-core 2.4GHz processor
- Already has CPython 3.11

**It would be like using a sports car to deliver pizza** - massive overkill!

## What You SHOULD Use on Pi 5

### For Python on Pi 5:
```bash
# Already installed!
python3 --version  # Python 3.11+

# Install GPIO libraries
pip3 install gpiozero RPi.GPIO lgpio
```

### For MicroPython Development:
**Use a Raspberry Pi Pico** ($4) connected to your Pi 5:

```bash
# On Pi 5:
pip3 install mpremote

# Connect Pico via USB
mpremote connect /dev/ttyACM0

# Or use Thonny IDE
sudo apt install thonny
```

## The Right Tool for the Job

| Device | Use For | Programming |
|--------|---------|-------------|
| **Raspberry Pi 5** | Linux projects, AI, desktop apps, servers | **CPython 3.11+** |
| **Raspberry Pi Pico W** | IoT, sensors, motors, low-power | **MicroPython** |
| **Raspberry Pi Pico** | Embedded systems, robotics, education | **MicroPython** |

## Summary

**For Raspberry Pi 5:**
- ❌ Don't use MicroPython (wrong tool)
- ✅ Use CPython 3.11+ (already installed)
- ✅ Use gpiozero/RPi.GPIO for hardware
- ✅ Use Pi 5 to *program* Pico boards running MicroPython

**For Embedded Projects:**
- ✅ Use Raspberry Pi Pico ($4)
- ✅ Run MicroPython firmware
- ✅ Connect to Pi 5 for development

**MicroPython on Pi 5 = Using a sledgehammer to crack a nut!** 🔨🥜

The Pi 5 is too powerful and already has Python. MicroPython is for the little guys (Pico, ESP32, etc.)!
