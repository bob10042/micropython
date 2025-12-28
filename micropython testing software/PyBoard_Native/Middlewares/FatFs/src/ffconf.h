/*---------------------------------------------------------------------------/
/  FatFs Configuration for PyBoard Native Firmware
/  Standalone configuration (no MicroPython dependencies)
/---------------------------------------------------------------------------*/

#ifndef FFCONF_DEF
#define FFCONF_DEF  86604

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY  0
/* 0:Read/Write or 1:Read-only */

#define FF_FS_MINIMIZE  0
/* 0: Full function, 1-3: Reduced function levels */

#define FF_USE_STRFUNC  1
/* 0:Disable, 1:Enable without LF-CRLF, 2:Enable with LF-CRLF */

#define FF_USE_FIND     0
/* 0:Disable, 1:Enable f_findfirst/f_findnext */

#define FF_USE_MKFS     1
/* 0:Disable, 1:Enable f_mkfs() */

#define FF_USE_FASTSEEK 0
/* 0:Disable, 1:Enable */

#define FF_USE_EXPAND   0
/* 0:Disable, 1:Enable f_expand() */

#define FF_USE_CHMOD    0
/* 0:Disable, 1:Enable f_chmod()/f_utime() */

#define FF_USE_LABEL    1
/* 0:Disable, 1:Enable f_getlabel()/f_setlabel() */

#define FF_USE_FORWARD  0
/* 0:Disable, 1:Enable f_forward() */

/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE    437
/* OEM code page: 437=US, 850=Latin1, etc. */

#define FF_USE_LFN      0
/* 0:Disable LFN, 1:Static buffer, 2:Stack, 3:Heap */
/* Note: LFN disabled to avoid needing large ffunicode.c tables */
/* With LFN=0, only 8.3 format filenames are supported */

#define FF_MAX_LFN      127
/* Max LFN length (12-255). 127 saves RAM while supporting most filenames */

#define FF_LFN_UNICODE  0
/* 0:ANSI/OEM, 1:UTF-16, 2:UTF-8, 3:UTF-32 */

#define FF_LFN_BUF      127
#define FF_SFN_BUF      12

#define FF_STRF_ENCODE  0
/* String encoding on file: 0:ANSI/OEM, 1:UTF-16LE, 2:UTF-16BE, 3:UTF-8 */

#define FF_FS_RPATH     0
/* 0:Disable relative path, 1:Enable, 2:Enable with f_getcwd() */

/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define FF_VOLUMES      1
/* Number of volumes (1-10) */

#define FF_STR_VOLUME_ID    0
#define FF_VOLUME_STRS      "SD"

#define FF_MULTI_PARTITION  1
/* 0:Single partition, 1:Multiple partitions */
/* Enabled for fdisk/partition command support */

#define FF_MIN_SS       512
#define FF_MAX_SS       512
/* Sector size (512, 1024, 2048, 4096) */

#define FF_USE_TRIM     0
/* 0:Disable, 1:Enable ATA-TRIM */

#define FF_FS_NOFSINFO  0
/* 0:Use FSINFO, 1:Don't trust free count, 2:Don't trust last cluster */

/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_TINY      1
/* 0:Normal, 1:Tiny (smaller FIL object, uses shared buffer) */

#define FF_FS_EXFAT     0
/* 0:Disable exFAT, 1:Enable (requires LFN) */

#define FF_FS_NORTC     1
/* 0:Use RTC, 1:Don't use RTC (fixed timestamp) */

#define FF_NORTC_MON    12
#define FF_NORTC_MDAY   28
#define FF_NORTC_YEAR   2025
/* Fixed date when FF_FS_NORTC=1 */

#define FF_FS_LOCK      0
/* Number of files that can be opened simultaneously with file lock (0=disable) */

#define FF_FS_REENTRANT 0
/* 0:Disable re-entrancy, 1:Enable (requires OS sync functions) */

#if FF_FS_REENTRANT
#define FF_FS_TIMEOUT   1000
#define FF_SYNC_t       void*
#endif

/*---------------------------------------------------------------------------/
/ Memory functions (only needed if FF_USE_LFN=3)
/---------------------------------------------------------------------------*/

/* Not needed since we use FF_USE_LFN=1 (static buffer) */

#endif /* FFCONF_DEF */
