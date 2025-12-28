# PyBoard Native Firmware Build Script
# =====================================
# This script builds the native C firmware for PyBoard v1.1 (STM32F405RG)
# Requires: STM32CubeIDE 2.0.0 installed at C:\ST\STM32CubeIDE_2.0.0

param(
    [switch]$Clean,
    [switch]$Rebuild
)

# ============================================================================
# ENVIRONMENT CONFIGURATION
# ============================================================================

$ErrorActionPreference = "Stop"

# STM32CubeIDE Installation Path
$CUBEIDE_PATH = "C:\ST\STM32CubeIDE_2.0.0\STM32CubeIDE"

# ARM GCC Toolchain Path (bundled with STM32CubeIDE)
$TOOLCHAIN_PATH = "$CUBEIDE_PATH\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.win32_1.0.100.202509120712\tools\bin"

# STM32CubeMX Path (standalone installation)
$CUBEMX_PATH = "C:\Users\bob43\AppData\Local\Programs\STM32CubeMX"

# STM32Cube Firmware Repository
$FIRMWARE_REPO = "C:\Users\bob43\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.0"

# Project Paths
$PROJECT_ROOT = $PSScriptRoot
$DEBUG_DIR = "$PROJECT_ROOT\Debug"
$CORE_SRC = "$PROJECT_ROOT\Core\Src"
$CORE_INC = "$PROJECT_ROOT\Core\Inc"
$DRIVERS_DIR = "$PROJECT_ROOT\Drivers"
$LINKER_SCRIPT = "$PROJECT_ROOT\STM32F405RGTX_FLASH.ld"

# ============================================================================
# VERIFY ENVIRONMENT
# ============================================================================

Write-Host "============================================" -ForegroundColor Cyan
Write-Host "PyBoard Native Firmware Build System" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan

# Check toolchain
if (-not (Test-Path "$TOOLCHAIN_PATH\arm-none-eabi-gcc.exe")) {
    Write-Host "ERROR: ARM GCC toolchain not found at:" -ForegroundColor Red
    Write-Host "  $TOOLCHAIN_PATH" -ForegroundColor Yellow
    Write-Host "Please install STM32CubeIDE 2.0.0 at C:\ST\STM32CubeIDE_2.0.0" -ForegroundColor Yellow
    exit 1
}

# Add toolchain to PATH
$env:PATH = "$TOOLCHAIN_PATH;$env:PATH"

Write-Host "Toolchain: ARM GCC 13.3.1" -ForegroundColor Green
Write-Host "Target:    STM32F405RG (Cortex-M4 @ 168MHz)" -ForegroundColor Green

# ============================================================================
# COMPILER FLAGS
# ============================================================================

# USB Device Library Paths
$USB_DEVICE_APP = "$PROJECT_ROOT\USB_DEVICE\App"
$USB_DEVICE_TARGET = "$PROJECT_ROOT\USB_DEVICE\Target"
$USB_CORE = "$PROJECT_ROOT\Middlewares\ST\STM32_USB_Device_Library\Core"
$USB_CDC = "$PROJECT_ROOT\Middlewares\ST\STM32_USB_Device_Library\Class\CDC"

# FatFS Library Path
$FATFS_SRC = "$PROJECT_ROOT\Middlewares\FatFs\src"

$CFLAGS = @(
    "-mcpu=cortex-m4",
    "-std=gnu11",
    "-g3",
    "-DDEBUG",
    "-DUSE_HAL_DRIVER",
    "-DSTM32F405xx",
    "-I$CORE_INC",
    "-I$DRIVERS_DIR\STM32F4xx_HAL_Driver\Inc",
    "-I$DRIVERS_DIR\STM32F4xx_HAL_Driver\Inc\Legacy",
    "-I$DRIVERS_DIR\CMSIS\Device\ST\STM32F4xx\Include",
    "-I$DRIVERS_DIR\CMSIS\Include",
    "-I$USB_DEVICE_APP",
    "-I$USB_DEVICE_TARGET",
    "-I$USB_CORE\Inc",
    "-I$USB_CDC\Inc",
    "-I$FATFS_SRC",
    "-O0",
    "-ffunction-sections",
    "-fdata-sections",
    "-Wall",
    "--specs=nano.specs",
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=hard",
    "-mthumb"
)

$LDFLAGS = @(
    "-mcpu=cortex-m4",
    "-T$LINKER_SCRIPT",
    "--specs=nosys.specs",
    "-Wl,-Map=$DEBUG_DIR\PyBoard_Native.map",
    "-Wl,--gc-sections",
    "-static",
    "--specs=nano.specs",
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=hard",
    "-mthumb",
    "-Wl,--start-group",
    "-lc",
    "-lm",
    "-Wl,--end-group"
)

# ============================================================================
# CLEAN
# ============================================================================

if ($Clean -or $Rebuild) {
    Write-Host "`nCleaning build directory..." -ForegroundColor Yellow
    if (Test-Path $DEBUG_DIR) {
        Remove-Item -Recurse -Force "$DEBUG_DIR\*" -ErrorAction SilentlyContinue
    }
    if ($Clean -and -not $Rebuild) {
        Write-Host "Clean complete." -ForegroundColor Green
        exit 0
    }
}

# ============================================================================
# CREATE BUILD DIRECTORIES
# ============================================================================

Write-Host "`nCreating build directories..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force -Path "$DEBUG_DIR\Core\Src" | Out-Null
New-Item -ItemType Directory -Force -Path "$DEBUG_DIR\Drivers\STM32F4xx_HAL_Driver\Src" | Out-Null
New-Item -ItemType Directory -Force -Path "$DEBUG_DIR\USB_DEVICE\App" | Out-Null
New-Item -ItemType Directory -Force -Path "$DEBUG_DIR\USB_DEVICE\Target" | Out-Null
New-Item -ItemType Directory -Force -Path "$DEBUG_DIR\Middlewares\USB_Core" | Out-Null
New-Item -ItemType Directory -Force -Path "$DEBUG_DIR\Middlewares\USB_CDC" | Out-Null
New-Item -ItemType Directory -Force -Path "$DEBUG_DIR\Middlewares\FatFs" | Out-Null

# ============================================================================
# COMPILE SOURCE FILES
# ============================================================================

Write-Host "`nCompiling source files..." -ForegroundColor Yellow

$sourceFiles = @(
    # Core sources
    @{Src="$CORE_SRC\main.c"; Obj="$DEBUG_DIR\Core\Src\main.o"},
    @{Src="$CORE_SRC\mma7660.c"; Obj="$DEBUG_DIR\Core\Src\mma7660.o"},
    @{Src="$CORE_SRC\stm32f4xx_it.c"; Obj="$DEBUG_DIR\Core\Src\stm32f4xx_it.o"},
    @{Src="$CORE_SRC\stm32f4xx_hal_msp.c"; Obj="$DEBUG_DIR\Core\Src\stm32f4xx_hal_msp.o"},
    @{Src="$CORE_SRC\system_stm32f4xx.c"; Obj="$DEBUG_DIR\Core\Src\system_stm32f4xx.o"},
    @{Src="$PROJECT_ROOT\STM32CubeIDE\Application\User\Core\syscalls.c"; Obj="$DEBUG_DIR\Core\syscalls.o"},
    @{Src="$PROJECT_ROOT\STM32CubeIDE\Application\User\Core\sysmem.c"; Obj="$DEBUG_DIR\Core\sysmem.o"},
    # USB Device App sources
    @{Src="$USB_DEVICE_APP\usb_device.c"; Obj="$DEBUG_DIR\USB_DEVICE\App\usb_device.o"},
    @{Src="$USB_DEVICE_APP\usbd_cdc_if.c"; Obj="$DEBUG_DIR\USB_DEVICE\App\usbd_cdc_if.o"},
    @{Src="$USB_DEVICE_APP\usbd_desc.c"; Obj="$DEBUG_DIR\USB_DEVICE\App\usbd_desc.o"},
    # USB Device Target sources
    @{Src="$USB_DEVICE_TARGET\usbd_conf.c"; Obj="$DEBUG_DIR\USB_DEVICE\Target\usbd_conf.o"},
    # USB Middleware Core sources
    @{Src="$USB_CORE\Src\usbd_core.c"; Obj="$DEBUG_DIR\Middlewares\USB_Core\usbd_core.o"},
    @{Src="$USB_CORE\Src\usbd_ctlreq.c"; Obj="$DEBUG_DIR\Middlewares\USB_Core\usbd_ctlreq.o"},
    @{Src="$USB_CORE\Src\usbd_ioreq.c"; Obj="$DEBUG_DIR\Middlewares\USB_Core\usbd_ioreq.o"},
    # USB Middleware CDC class sources
    @{Src="$USB_CDC\Src\usbd_cdc.c"; Obj="$DEBUG_DIR\Middlewares\USB_CDC\usbd_cdc.o"},
    # FatFS file system sources
    @{Src="$FATFS_SRC\ff.c"; Obj="$DEBUG_DIR\Middlewares\FatFs\ff.o"},
    @{Src="$FATFS_SRC\diskio.c"; Obj="$DEBUG_DIR\Middlewares\FatFs\diskio.o"}
)

foreach ($file in $sourceFiles) {
    $name = Split-Path $file.Src -Leaf
    Write-Host "  Compiling $name..." -NoNewline
    & arm-none-eabi-gcc -c $file.Src $CFLAGS -o $file.Obj 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host " FAILED" -ForegroundColor Red
        exit 1
    }
    Write-Host " OK" -ForegroundColor Green
}

# Compile startup assembly
Write-Host "  Compiling startup_stm32f405rgtx.s..." -NoNewline
& arm-none-eabi-gcc -c "$PROJECT_ROOT\STM32CubeIDE\Application\User\Startup\startup_stm32f405rgtx.s" `
    -mcpu=cortex-m4 --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb `
    -o "$DEBUG_DIR\Core\startup_stm32f405rgtx.o" 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host " FAILED" -ForegroundColor Red
    exit 1
}
Write-Host " OK" -ForegroundColor Green

# Compile HAL drivers (exclude template files)
$halFiles = Get-ChildItem "$DRIVERS_DIR\STM32F4xx_HAL_Driver\Src\*.c" | Where-Object { $_.Name -notlike "*_template.c" }
foreach ($file in $halFiles) {
    $outFile = "$DEBUG_DIR\Drivers\STM32F4xx_HAL_Driver\Src\$($file.BaseName).o"
    Write-Host "  Compiling $($file.Name)..." -NoNewline
    & arm-none-eabi-gcc -c $file.FullName $CFLAGS -o $outFile 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host " FAILED" -ForegroundColor Red
        exit 1
    }
    Write-Host " OK" -ForegroundColor Green
}

# ============================================================================
# LINK
# ============================================================================

Write-Host "`nLinking..." -ForegroundColor Yellow

$objFiles = (Get-ChildItem -Path "$DEBUG_DIR\Core\*.o" -Recurse | ForEach-Object { $_.FullName }) + 
            (Get-ChildItem -Path "$DEBUG_DIR\Drivers\*.o" -Recurse | ForEach-Object { $_.FullName }) +
            (Get-ChildItem -Path "$DEBUG_DIR\USB_DEVICE\*.o" -Recurse | ForEach-Object { $_.FullName }) +
            (Get-ChildItem -Path "$DEBUG_DIR\Middlewares\*.o" -Recurse | ForEach-Object { $_.FullName })

& arm-none-eabi-gcc -o "$DEBUG_DIR\PyBoard_Native.elf" $objFiles $LDFLAGS 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Linking FAILED" -ForegroundColor Red
    exit 1
}

# ============================================================================
# GENERATE OUTPUT FILES
# ============================================================================

Write-Host "`nGenerating output files..." -ForegroundColor Yellow

& arm-none-eabi-objcopy -O ihex "$DEBUG_DIR\PyBoard_Native.elf" "$DEBUG_DIR\PyBoard_Native.hex"
& arm-none-eabi-objcopy -O binary "$DEBUG_DIR\PyBoard_Native.elf" "$DEBUG_DIR\PyBoard_Native.bin"

# ============================================================================
# BUILD SUMMARY
# ============================================================================

Write-Host "`n============================================" -ForegroundColor Cyan
Write-Host "BUILD SUCCESSFUL" -ForegroundColor Green
Write-Host "============================================" -ForegroundColor Cyan

$sizeOutput = & arm-none-eabi-size "$DEBUG_DIR\PyBoard_Native.elf"
Write-Host $sizeOutput[0]
Write-Host $sizeOutput[1]

$hexSize = (Get-Item "$DEBUG_DIR\PyBoard_Native.hex").Length
$binSize = (Get-Item "$DEBUG_DIR\PyBoard_Native.bin").Length

Write-Host "`nOutput Files:" -ForegroundColor Yellow
Write-Host "  PyBoard_Native.hex  - $hexSize bytes (Intel HEX for flashing)"
Write-Host "  PyBoard_Native.bin  - $binSize bytes (Raw binary)"
Write-Host "  PyBoard_Native.elf  - Debug ELF with symbols"
Write-Host "  PyBoard_Native.map  - Linker map file"

Write-Host "`nTo flash: Use ST-Link or DFU bootloader" -ForegroundColor Yellow
Write-Host "Location: $DEBUG_DIR\PyBoard_Native.hex" -ForegroundColor Cyan
