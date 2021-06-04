#pragma once

/**
 *  Log
 * */

#define CFG_LOG_SYSCALL

// #define CFG_LOG_CPIO
// #define CFG_LOG_SHELL_SEARCH_FILE
// #define CFG_LOG_SHELL_CMD
// #define CFG_LOG_SHELL_BUFFER
// #define CFG_LOG_MEM_STARTUP
// #define CFG_LOG_MEM_KALLOC
// #define CFG_LOG_MEM_BUDDY
// #define CFG_LOG_MEM_SLAB
// #define CFG_LOG_PROC_TASK
// #define CFG_LOG_PROC_SCHED
// #define CFG_LOG_PROC_ARGV
// #define CFG_LOG_PROC_EXEC
#define CFG_LOG_VFS
// #define CFG_LOG_TMPFS
#define CFG_LOG_TMPFS_LOOKUP
#define CFG_LOG_TMPFS_DUMP_TREE
#define CFG_LOG_DEV_MBR
#define CFG_LOG_FAT

/**
 *  TEST
 * */
#define CFG_RUN_TEST

// Libs
#define CFG_RUN_LIB_STRING_TEST

// Shell
// #define CFG_RUN_SHELL_BUFFER_TEST
// #define CFG_RUN_SHELL_CMD_TEST

// Memory management
// #define CFG_RUN_STATUP_ALLOC_TEST

// PROC
// #define CFG_RUN_PROC_ARGV_TEST

// DEV
#define CFG_RUN_DEV_MBR_TEST

// FS
#define CFG_RUN_FS_VFS_TEST

#define CFG_RUN_FS_FAT_TEST