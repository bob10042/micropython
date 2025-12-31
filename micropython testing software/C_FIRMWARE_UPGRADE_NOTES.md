# PyBoard Native C Firmware vs MicroPython Comparison

## Test Date: December 28, 2025

---

## Speed Comparison (100K iterations)

| Implementation | Time | Speedup vs Bytecode |
|----------------|------|---------------------|
| MicroPython Bytecode | 784,081 µs | 1.0x |
| MicroPython Native | 586,403 µs | 1.3x |
| **MicroPython Viper** | **25,635 µs** | **30.6x** |
| Native C Firmware | 232,000 µs | 3.4x |

**Key Finding**: MicroPython Viper is actually **9x faster** than the C firmware for pure compute loops!

---

## Feature Comparison

### ✅ Features in BOTH (C firmware matches MicroPython)

| Feature | C Firmware Command | MicroPython Equivalent |
|---------|-------------------|----------------------|
| LED Control | `led N [0\|1]` | `pyb.LED(n).on()/off()/toggle()` |
| LED PWM (LED4) | `intensity N` | `pyb.LED(4).intensity(n)` |
| Accelerometer | `accel` | `pyb.Accel().x(), .y(), .z()` |
| ADC Read | `adc` | `pyb.ADC('X1').read()` |
| DAC Write | `dac N` | `pyb.DAC(1).write(n)` |
| PWM Output | `pwm N` | `pyb.Timer(2).channel(1)` |
| GPIO Read/Write | `x1 1`, `y5 0` | `pyb.Pin('X1').value(1)` |
| GPIO Mode | `mode xN in\|out\|od` | `pyb.Pin('X1', Pin.OUT)` |
| GPIO Pull | `pull xN up\|down\|none` | `pyb.Pin('X1', pull=Pin.PULL_UP)` |
| User Button | `button` | `pyb.Switch()()` |
| I2C Scan | `i2c1`, `i2c2` | `pyb.I2C(1).scan()` |
| SPI Transfer | `spi1 send XX` | `pyb.SPI(1).send_recv()` |
| UART TX/RX | `uart2 send/recv` | `pyb.UART(2).write()/read()` |
| CAN Bus | `can send/recv/status` | `pyb.CAN(1).send()/recv()` |
| RTC | `rtc`, `rtc set` | `pyb.RTC().datetime()` |
| SD Card Info | `sd`, `sd init` | `pyb.SDCard()` |
| FatFS | `mount`, `ls`, `cat`, `write` | `os.mount()`, `open()` |
| Speed Test | `speed`, `speedgpio` | Custom Python code |

---

## ❌ Features MISSING from C Firmware

### High Priority (Common MicroPython Features)

| Feature | MicroPython API | Difficulty | Notes |
|---------|----------------|------------|-------|
| **Network/WiFi** | `network.WLAN()` | N/A | PyBoard has no WiFi hardware |
| **Bluetooth** | `bluetooth.BLE()` | N/A | PyBoard has no BLE hardware |
| **Timer Callbacks** | `Timer(callback=fn)` | Medium | Need interrupt-driven callbacks |
| **External Interrupts** | `Pin.irq(handler=fn)` | Medium | ExtInt with callbacks |
| **Multiple ADC Channels** | `ADC('X2')..ADC('X8')` | Easy | Currently only X1/PA0 |
| **Multiple DAC Channels** | `DAC(2)` (X6/PA5) | Easy | Currently only X5/PA4 |
| **PWM on Multiple Pins** | Various Timer channels | Medium | Need more TIM channels |
| **I2C Read/Write Data** | `i2c.readfrom()`, `writeto()` | Easy | Currently only scan |
| **Sleep Modes** | `machine.lightsleep()`, `deepsleep()` | Medium | Power management |
| **WDT** | `machine.WDT()` | Easy | Watchdog timer |
| **Unique ID** | `machine.unique_id()` | Easy | Read STM32 UID |
| **Reset Cause** | `machine.reset_cause()` | Easy | Check reset flags |
| **Frequency Control** | `machine.freq()` | Medium | Dynamic clock scaling |

### Medium Priority (Advanced Features)

| Feature | MicroPython API | Difficulty | Notes |
|---------|----------------|------------|-------|
| **DMA Transfers** | Implicit in SPI/I2C | Hard | Currently polling mode |
| **USB HID/MSC** | `usb.device.HID()` | Hard | USB device classes |
| **Internal Flash FS** | `/flash` filesystem | Medium | Need flash wear leveling |
| **Frozen Modules** | `import module` | N/A | Not applicable to C |
| **REPL** | Interactive Python | N/A | C is CLI, not REPL |
| **Scripting** | Run `.py` files | N/A | Fundamental difference |

### Low Priority (Rarely Used)

| Feature | MicroPython API | Notes |
|---------|----------------|-------|
| LCD160CR Support | `lcd160cr` module | Specific display |
| Servo Control | `pyb.Servo()` | Can use PWM instead |
| NeoPixel | `neopixel` module | Bit-banging protocol |
| OneWire | `onewire` module | DS18B20 temperature |
| DHT Sensors | `dht` module | Temperature/humidity |

---

## 🔧 Recommended Upgrades for C Firmware

### Phase 1: Easy Wins (1-2 hours each)

1. **Add Multiple ADC Channels**
   - Configure ADC1 for channels 0-7 (X1-X8)
   - Add command: `adc N` to read specific channel
   - Code location: `MX_ADC1_Init()` and `CLI_Process()`

2. **Add Second DAC Channel**
   - Enable DAC_CHANNEL_2 (X6/PA5)
   - Add command: `dac2 N`
   - Code location: `MX_DAC_Init()`

3. **Add Machine Unique ID**
   - Read from `0x1FFF7A10` (STM32 UID)
   - Add command: `uid`
   ```c
   uint32_t uid[3];
   uid[0] = *(uint32_t*)0x1FFF7A10;
   uid[1] = *(uint32_t*)0x1FFF7A14;
   uid[2] = *(uint32_t*)0x1FFF7A18;
   ```

4. **Add Watchdog Timer**
   - Use IWDG for reliability
   - Add commands: `wdt start`, `wdt feed`

5. **Add Reset Cause**
   - Read RCC_CSR flags
   - Add command: `resetcause`

### Phase 2: Medium Effort (4-8 hours each)

6. **I2C Data Read/Write**
   - Add: `i2c1 read <addr> <len>`
   - Add: `i2c1 write <addr> <bytes>`
   ```c
   HAL_I2C_Master_Transmit(&hi2c1, addr<<1, data, len, 100);
   HAL_I2C_Master_Receive(&hi2c1, addr<<1, data, len, 100);
   ```

7. **External Interrupts**
   - Configure EXTI for button callbacks
   - Add: `exti xN rising|falling|both`
   - Requires interrupt handlers in `stm32f4xx_it.c`

8. **More PWM Channels**
   - TIM1: PA8 (X17), PA9 (X18), PA10 (X19), PA11 (X20)
   - TIM3: PB4 (LED4), PB5 (Y11), PC6 (Y1), PC7 (Y2)
   - Add: `pwm xN duty`

9. **Sleep Modes**
   - `HAL_PWR_EnterSLEEPMode()` for light sleep
   - `HAL_PWR_EnterSTOPMode()` for deep sleep
   - Add: `sleep`, `deepsleep`

### Phase 3: Major Features (Days of work)

10. **Timer Callbacks**
    - Need a simple scheduler
    - Store function pointers for timer ISRs
    - Limited without scripting

11. **DMA for SPI/I2C**
    - Use `HAL_SPI_Transmit_DMA()`
    - Significant performance boost for large transfers

12. **Internal Flash Filesystem**
    - Use sectors 1-3 (48KB) for user storage
    - Implement wear leveling

---

## Code Modifications Needed

### File: `main.c`

```c
// Add to CLI_Process():

/* Multiple ADC channels */
else if (strncmp(cmd, "adc ", 4) == 0) {
    int ch;
    if (sscanf(cmd + 4, "%d", &ch) == 1 && ch >= 1 && ch <= 8) {
        ADC_ChannelConfTypeDef sConfig = {0};
        sConfig.Channel = ADC_CHANNEL_0 + ch - 1;  // X1=CH0, X2=CH1, etc.
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 100);
        uint16_t val = HAL_ADC_GetValue(&hadc1);
        sprintf(response, "ADC CH%d = %d (%.2fV)\r\n", ch, val, val * 3.3f / 4095.0f);
        CLI_SendString(response);
    }
}

/* Unique ID */
else if (strcmp(cmd, "uid") == 0) {
    uint32_t *uid = (uint32_t*)0x1FFF7A10;
    sprintf(response, "UID: %08lX-%08lX-%08lX\r\n", uid[0], uid[1], uid[2]);
    CLI_SendString(response);
}

/* I2C read */
else if (strncmp(cmd, "i2c1 read ", 10) == 0) {
    int addr, len;
    if (sscanf(cmd + 10, "%x %d", &addr, &len) == 2 && len <= 32) {
        uint8_t buf[32];
        if (HAL_I2C_Master_Receive(&hi2c1, addr<<1, buf, len, 100) == HAL_OK) {
            sprintf(response, "I2C1 read 0x%02X: ", addr);
            CLI_SendString(response);
            for (int i = 0; i < len; i++) {
                sprintf(response, "%02X ", buf[i]);
                CLI_SendString(response);
            }
            CLI_SendString("\r\n");
        } else {
            CLI_SendString("I2C read failed\r\n");
        }
    }
}

/* I2C write */
else if (strncmp(cmd, "i2c1 write ", 11) == 0) {
    int addr;
    uint8_t data[16];
    int n = 0;
    char *p = (char*)(cmd + 11);
    if (sscanf(p, "%x", &addr) == 1) {
        while (*p && *p != ' ') p++;
        while (*p == ' ') p++;
        while (*p && n < 16) {
            int val;
            if (sscanf(p, "%x", &val) == 1) {
                data[n++] = val;
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
            } else break;
        }
        if (n > 0) {
            if (HAL_I2C_Master_Transmit(&hi2c1, addr<<1, data, n, 100) == HAL_OK) {
                sprintf(response, "I2C1 write 0x%02X: %d bytes OK\r\n", addr, n);
                CLI_SendString(response);
            } else {
                CLI_SendString("I2C write failed\r\n");
            }
        }
    }
}
```

---

## Summary

The C firmware already has **excellent coverage** of PyBoard hardware:
- ✅ All 4 LEDs with PWM
- ✅ Accelerometer
- ✅ ADC/DAC
- ✅ GPIO (all X and Y pins)
- ✅ I2C, SPI, UART, CAN
- ✅ RTC
- ✅ SD Card with FatFS

The biggest gaps are:
1. **I2C data read/write** (easy to add)
2. **Multiple ADC channels** (easy to add)
3. **Interrupt callbacks** (medium effort)
4. **Sleep modes** (medium effort)

For pure speed, **Viper still wins** at 30x faster than bytecode, while C firmware is only 3.4x faster due to CLI overhead.
