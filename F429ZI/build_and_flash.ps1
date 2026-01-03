# STM32_Standalone Build and Flash Script
# Run:
#   .\build_and_flash.ps1            # clean + build + flash
#   .\build_and_flash.ps1 -BuildOnly # clean + build
#   .\build_and_flash.ps1 -FlashOnly # flash (requires existing ELF)
#   .\build_and_flash.ps1 -Clean     # remove build dir

param(
    [switch]$BuildOnly,
    [switch]$FlashOnly,
    [switch]$Clean
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  STM32_Standalone Build & Flash      " -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

function Add-ToPathIfExists {
    param([Parameter(Mandatory = $true)][string]$Dir)
    if ($Dir -and (Test-Path -LiteralPath $Dir)) {
        $env:PATH = "$Dir;" + $env:PATH
        return $true
    }
    return $false
}

function Resolve-FirstExistingPath {
    param([Parameter(Mandatory = $true)][string[]]$Candidates)
    foreach ($candidate in $Candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return $candidate
        }
    }
    return $null
}

# Project root = folder containing this script
$ProjectDir = $PSScriptRoot

# Optional overrides (recommended for portability):
#   $env:ARM_GCC_BIN = "C:\path\to\arm-gnu-toolchain\bin"
#   $env:MAKE_BIN    = "C:\path\to\make\bin"
#   $env:STM32PROG_CLI = "C:\Program Files\...\STM32_Programmer_CLI.exe"
if ($env:ARM_GCC_BIN) { Add-ToPathIfExists $env:ARM_GCC_BIN | Out-Null }
if ($env:MAKE_BIN) { Add-ToPathIfExists $env:MAKE_BIN | Out-Null }

# Toolchain resolution (only used if not already discoverable)
$DefaultArmGccBins = @(
    # Common standalone Arm GNU Toolchain install
    "C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.2 rel1\bin",
    "C:\Program Files\Arm GNU Toolchain arm-none-eabi\14.2 rel1\bin",

    # CubeIDE-bundled toolchains (versioned)
    "C:\ST\STM32CubeIDE_2.0.0\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.13.3.rel1.win32_1.0.100.202509120712\tools\bin"
)

$DefaultMakeBins = @(
    # MSYS2 make (commonly used on Windows)
    "C:\msys64\mingw64\bin",

    # CubeIDE-bundled make (versioned)
    "C:\ST\STM32CubeIDE_2.0.0\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.make.win32_2.1.100.202405291143\tools\bin"
)

if (-not (Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue)) {
    $armBin = Resolve-FirstExistingPath $DefaultArmGccBins
    if ($armBin) { Add-ToPathIfExists $armBin | Out-Null }
}

if (-not (Get-Command make -ErrorAction SilentlyContinue)) {
    $makeBin = Resolve-FirstExistingPath $DefaultMakeBins
    if ($makeBin) { Add-ToPathIfExists $makeBin | Out-Null }
}

$Programmer = $env:STM32PROG_CLI
if (-not $Programmer) {
    $Programmer = "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
}

if (-not (Test-Path -LiteralPath $Programmer)) {
    # Some setups put a portable CubeProgrammer folder on the Desktop
    $desktopCandidates = @(
        "$env:USERPROFILE\Desktop",
        [Environment]::GetFolderPath('Desktop')
    ) | Select-Object -Unique

    foreach ($desktop in $desktopCandidates) {
        if (-not (Test-Path -LiteralPath $desktop)) { continue }
        $found = Get-ChildItem -LiteralPath $desktop -Recurse -Filter "STM32_Programmer_CLI.exe" -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($found) {
            $Programmer = $found.FullName
            break
        }
    }
}

$ElfFile = Join-Path $ProjectDir "build\STM32_Standalone.elf"

Set-Location $ProjectDir

if ($Clean) {
    Write-Host "`n[CLEAN] Removing build directory..." -ForegroundColor Yellow
    Remove-Item -Path (Join-Path $ProjectDir "build") -Recurse -ErrorAction SilentlyContinue
    Write-Host "[CLEAN DONE]" -ForegroundColor Green
    if (-not $BuildOnly -and -not $FlashOnly) { return }
}

if (-not (Get-Command make -ErrorAction SilentlyContinue)) {
    throw "make not found. Install make (e.g., MSYS2) or set MAKE_BIN to the folder containing make.exe."
}

if (-not (Get-Command arm-none-eabi-gcc -ErrorAction SilentlyContinue)) {
    throw "arm-none-eabi-gcc not found. Install Arm GNU Toolchain or set ARM_GCC_BIN to the folder containing arm-none-eabi-gcc.exe."
}

if (-not $FlashOnly) {
    # Build
    Write-Host "`n[1/2] Cleaning..." -ForegroundColor Yellow
    Remove-Item -Path "build" -Recurse -ErrorAction SilentlyContinue

    Write-Host "[2/2] Building..." -ForegroundColor Yellow
    make all

    if (-not (Test-Path $ElfFile)) {
        throw "Build did not produce ELF: $ElfFile"
    }

    Write-Host "[BUILD SUCCESS]" -ForegroundColor Green
    arm-none-eabi-size $ElfFile

    if ($BuildOnly) {
        return
    }
}

# Build
Write-Host "`n[1/3] Cleaning..." -ForegroundColor Yellow
Remove-Item -Path "build" -Recurse -ErrorAction SilentlyContinue

Write-Host "[2/3] Building..." -ForegroundColor Yellow
make all 2>&1 | Out-Null

if (-not (Test-Path -LiteralPath $Programmer)) {
    throw "STM32_Programmer_CLI.exe not found. Set STM32PROG_CLI or install STM32CubeProgrammer."
}

if (-not (Test-Path -LiteralPath $ElfFile)) {
    throw "ELF not found for flashing: $ElfFile (run with no args or -BuildOnly first)"
}

# Flash
Write-Host "`n[FLASH] Flashing via STM32CubeProgrammer CLI..." -ForegroundColor Yellow
& $Programmer -c port=SWD -w $ElfFile -v -rst
Write-Host "[FLASH DONE]" -ForegroundColor Green

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  Done! LED should be blinking now.   " -ForegroundColor Cyan
Write-Host "  Open serial terminal at 115200 baud " -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
