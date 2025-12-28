# PyBoard Native Firmware Development Notes

## CRITICAL: Lessons Learned (December 28, 2025)

### Issue: Firmware Hangs - No USB Response

**Symptoms:**
- USB enumerates as COM port (shows OK in Device Manager)
- No response to serial commands
- Board appears dead

**Root Causes Identified:**

#### 1. RTC Initialization Hang (CRITICAL)
```c
// THIS WILL HANG WITHOUT LSE/LSI CLOCK:
MX_RTC_Init();  // HAL_RTC_Init() waits forever for sync
```

**Solution:** RTC requires LSE (32.768kHz crystal) or LSI clock to be configured BEFORE calling `HAL_RTC_Init()`. Either:
- Configure LSE/LSI in SystemClock_Config()
- OR comment out MX_RTC_Init() until clock is configured

#### 2. Missing Interrupt Handlers (CRITICAL)
```c
// THESE WILL CRASH WITHOUT IRQ HANDLERS:
HAL_UART_Receive_IT(&huart2, ...);  // Needs USART2_IRQHandler
HAL_UART_Receive_IT(&huart6, ...);  // Needs USART6_IRQHandler
HAL_CAN_ActivateNotification(...);  // Needs CAN1_RX0_IRQHandler
```

**Solution:** Either:
- Add interrupt handlers to `stm32f4xx_it.c`
- OR use polling mode (don't call interrupt-enable functions)

#### 3. CAN Initialization Failure
```c
// THIS MAY HANG/FAIL WITHOUT CAN TRANSCEIVER:
if (HAL_CAN_Init(&hcan1) != HAL_OK) {
    Error_Handler();  // DON'T DO THIS - board hangs
}
```

**Solution:** Follow MicroPython's approach - don't call Error_Handler() for CAN:
```c
HAL_CAN_Init(&hcan1);  // Ignore return value, like MicroPython
```

---

## How MicroPython Handles Peripherals

MicroPython initializes peripherals **lazily** - only when the user requests them:

| Peripheral | At Boot | When Initialized |
|------------|---------|------------------|
| CAN | `pyb_can_init0()` = NULL pointers only | When user calls `pyb.CAN()` |
| UART | `uart_init0()` = clock config only | When user calls `pyb.UART()` |
| RTC | `rtc_init_start()` = checks existing | Full init only if needed |
| SPI | Not touched | When user calls `pyb.SPI()` |
| I2C | Not touched | When user calls `pyb.I2C()` |

**Key principle:** Don't enable interrupts or start peripherals at boot unless:
1. You have proper IRQ handlers
2. You've verified hardware is connected
3. Clock sources are configured

---

## Working Firmware Versions

| Version | Size | Features | Notes |
|---------|------|----------|-------|
| v1.0 | 170KB | Basic CLI, LEDs, Accel, ADC, DAC, PWM, GPIO | Stable baseline |
| v2.0 | 208KB | + Extended GPIO, SPI, UART, CAN, RTC commands | TIM3/TIM4/RTC disabled |

---

## File Locations

### Backups
```
PyBoard_Native/backups/
├── working_20251228_134353/     # v1.0 working source
├── release_v2.0_20251228_*/     # v2.0 release backup
└── broken_main.c                # For reference - DON'T USE
```

### Firmware Releases
```
PyBoard_Native/firmware_releases/
├── PyBoard_Native_v1.0_working.hex   # 170KB - stable
└── PyBoard_Native_v2.0_working.hex   # 208KB - full features
```

---

## Safe Development Checklist

Before adding new peripheral initialization:

- [ ] Check if clock source is configured (LSE for RTC, etc.)
- [ ] Check if MSP init enables the peripheral clock
- [ ] Check if IRQ handler exists in `stm32f4xx_it.c`
- [ ] Don't call `Error_Handler()` for optional peripherals
- [ ] Test incrementally - add one feature at a time

---

## Quick Recovery

If firmware breaks:

1. Flash working backup: `firmware_releases/PyBoard_Native_v2.0_working.hex`
2. Or restore source from: `backups/release_v2.0_*/main.c`

---

## Build Commands

```powershell
cd "C:\Users\bob43\Downloads\micropython\micropython testing software\PyBoard_Native"
.\build.ps1
```

Output: `Debug\PyBoard_Native.hex`

---

## Test Commands

```powershell
# Quick test
python -c "import serial; s = serial.Serial('COM11', 115200, timeout=2); s.write(b'help\r\n'); import time; time.sleep(0.5); print(s.read(s.in_waiting).decode()); s.close()"

# Full test suite
python "micropython testing software\test_native_firmware.py"
```
