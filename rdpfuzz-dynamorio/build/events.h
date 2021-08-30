// **********************************************************
// Copyright (c) 2012-2020 Google, Inc.  All rights reserved.
// Copyright (c) 2003-2010 VMware, Inc.  All rights reserved.
// **********************************************************
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of VMware, Inc. nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
// Copyright (c) 2011 Google, Inc.
// Copyright (c) 2003-2007 Determina Corp.
// events.mc messages for Event logging
// FIXME: I don't seem to get the !d! format specifiers on Win2k - will need to rework this
// CHECK: is there a buffer overflow possibility in these facilities?
// CHECK: should we always have the * specifier for the width component of a %!s!
// TODO: We have to figure how to get all of this export something nice for Linux syslog
// DO not use TABS or COMMAS (,) in the messages
// ADD NEW MESSAGES AT THE END
// defaults
// FIXME: unclear on can we override these..
// default
//FIXME: can't override - I still want to change the name of the .BIN files
//LanguageNames=(English=1:EVMSG001)
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
// Categories
// unused: we may be happy with message types for now
// FIXME: if the FacilityName thing works, otherwise we'll use these
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: MSG_CATEGORY_SECURITY
//
// MessageText:
//
// Security
//
#define MSG_CATEGORY_SECURITY            ((uint)0x00000001L)

//;;;;;;;;;;;;;;;;;;; Core
// Info messages
//
// MessageId: MSG_INFO_PRODUCT_VERSION
//
// MessageText:
//
// %1!s! (R) %2!s! (R) %3!s! %4!s! installed.
//
#define MSG_INFO_PRODUCT_VERSION         ((uint)0x40FF03E8L)

// Info messages
// NOTE: All message regarding applications should have app name and pid as the
// first 2 arguments - otherwise controller will break.
//
// MessageId: MSG_INFO_PROCESS_START
//
// MessageText:
//
// Starting application %1!s! (%2!s!)
// MD5: %3!s!
//
#define MSG_INFO_PROCESS_START           ((uint)0x40FF03E9L)

// Avoid the MD5 output for CLIENT_INTERFACE
//
// MessageId: MSG_INFO_PROCESS_START_CLIENT
//
// MessageText:
//
// Starting application %1!s! (%2!s!)
//
#define MSG_INFO_PROCESS_START_CLIENT    ((uint)0x40FF03EAL)

// Info messages
// empty MessageId value means previous +1
//
// MessageId: MSG_INFO_PROCESS_STOP
//
// MessageText:
//
// Stopping application %1!s! (%2!s!)
//
#define MSG_INFO_PROCESS_STOP            ((uint)0x40FF03EBL)

// Info messages
//
// MessageId: MSG_INFO_DETACHING
//
// MessageText:
//
// Detaching from application %1!s! (%2!s!)
//
#define MSG_INFO_DETACHING               ((uint)0x40FF03ECL)

// Info messages
//
// MessageId: MSG_INFO_ATTACHED
//
// MessageText:
//
// Attached to %1!s! threads in application %2!s! (%3!s!)
//
#define MSG_INFO_ATTACHED                ((uint)0x40FF03EDL)

//
// MessageId: MSG_INFO_RESET_IN_PROGRESS
//
// MessageText:
//
// Resetting caches and non-persistent memory @ %1!s! fragments in application %2!s! (%3!s!).
//
#define MSG_INFO_RESET_IN_PROGRESS       ((uint)0x40FF03EEL)

//;;;;;;;;;;;;;;;;;;; Security
//
// MessageId: MSG_SEC_VIOLATION_TERMINATED
//
// MessageText:
//
// A security violation was intercepted in application %1!s! (%2!s!).
// Threat ID: %3!s!.
// Program terminated.
//
#define MSG_SEC_VIOLATION_TERMINATED     ((uint)0x47FF0001L)

//
// MessageId: MSG_SEC_VIOLATION_CONTINUE
//
// MessageText:
//
// A security violation was intercepted in application %1!s! (%2!s!).
// Threat ID: %3!s!.
// Running in test mode - program continuing.
// Your system may have been compromised.
//
#define MSG_SEC_VIOLATION_CONTINUE       ((uint)0x47FF0002L)

//
// MessageId: MSG_SEC_VIOLATION_THREAD
//
// MessageText:
//
// A security violation was intercepted in application %1!s! (%2!s!).
// Threat ID: %3!s!.
// Program continuing after terminating faulty thread.
// Some application functionality may be lost.
//
#define MSG_SEC_VIOLATION_THREAD         ((uint)0x47FF0003L)

//
// MessageId: MSG_SEC_VIOLATION_EXCEPTION
//
// MessageText:
//
// A security violation was intercepted in application %1!s! (%2!s!).
// Threat ID: %3!s!.
// Program continuing after throwing an exception.
// Application may terminate.
//
#define MSG_SEC_VIOLATION_EXCEPTION      ((uint)0x47FF0004L)

//
// MessageId: MSG_INTERNAL_SYSLOG_CRITICAL
//
// MessageText:
//
// Application %1!s! (%2!s!).  Internal Critical Error: %3!s!
//
#define MSG_INTERNAL_SYSLOG_CRITICAL     ((uint)0xC0FF03EFL)

//
// MessageId: MSG_INTERNAL_SYSLOG_ERROR
//
// MessageText:
//
// Application %1!s! (%2!s!).  Internal Error: %3!s!
//
#define MSG_INTERNAL_SYSLOG_ERROR        ((uint)0xC0FF03F0L)

//
// MessageId: MSG_INTERNAL_SYSLOG_WARNING
//
// MessageText:
//
// Application %1!s! (%2!s!).  Internal Warning: %3!s!
//
#define MSG_INTERNAL_SYSLOG_WARNING      ((uint)0x80FF03F1L)

//
// MessageId: MSG_INTERNAL_SYSLOG_INFORMATION
//
// MessageText:
//
// Application %1!s! (%2!s!).  Internal Information: %3!s!
//
#define MSG_INTERNAL_SYSLOG_INFORMATION  ((uint)0x40FF03F2L)

//
// MessageId: MSG_EXCEPTION
//
// MessageText:
//
// Application %1!s! (%2!s!).  %3!s! %4!s! at PC %5!s!.  Please report this at %6!s!.  Program aborted.%7!s!
//
#define MSG_EXCEPTION                    ((uint)0xC0FF03F3L)

//
// MessageId: MSG_CLIENT_EXCEPTION
//
// MessageText:
//
// Application %1!s! (%2!s!).  %3!s! %4!s! at PC %5!s!.  Please report this at %6!s!.  Program aborted.%7!s!
//
#define MSG_CLIENT_EXCEPTION             ((uint)0xC0FF03F4L)

//
// MessageId: MSG_INVOKING_APP_HANDLER
//
// MessageText:
//
// Invoking fault handler for application %1!s! (%2!s!).
//
#define MSG_INVOKING_APP_HANDLER         ((uint)0x80FF03F5L)

//
// MessageId: MSG_APP_EXCEPTION
//
// MessageText:
//
// Application %1!s! (%2!s!).  Application exception at PC %3!s!.  %4!s!
//
#define MSG_APP_EXCEPTION                ((uint)0x80FF03F6L)

//
// MessageId: MSG_OUT_OF_MEMORY
//
// MessageText:
//
// Application %1!s! (%2!s!).  Out of memory.  Program aborted.  Source %3!s!, type %3!s!, code %4!s!.
//
#define MSG_OUT_OF_MEMORY                ((uint)0xC0FF03F7L)

//
// MessageId: MSG_MAX_PENDING_SIGNALS
//
// MessageText:
//
// Application %1!s! (%2!s!).  Reached the soft maximum number (%3!s!) of pending signals: further accumulation prior to delivery may cause problems.  Consider raising -max_pending_signals.
//
#define MSG_MAX_PENDING_SIGNALS          ((uint)0x80FF03F8L)

//
// MessageId: MSG_UNSUPPORTED_APPLICATION
//
// MessageText:
//
// Application %1!s! (%2!s!) is not supported due to dll %3!s!.  Program aborted.
//
#define MSG_UNSUPPORTED_APPLICATION      ((uint)0xC0FF03F9L)

//
// MessageId: MSG_WSL_UNSUPPORTED_FATAL
//
// MessageText:
//
// Application %1!s! (%2!s!).  The Windows Subsystem for Linux is not yet supported due to missing segment support from the kernel.  Program aborted.
//
#define MSG_WSL_UNSUPPORTED_FATAL        ((uint)0xC0FF03FAL)

//
// MessageId: MSG_WARNING_REPORT_THRESHOLD
//
// MessageText:
//
// Application %1!s! (%2!s!) has reached its report threshold.  No more violations will be logged.
//
#define MSG_WARNING_REPORT_THRESHOLD     ((uint)0x80FF03FBL)

//
// MessageId: MSG_ERROR_UNABLE_TO_CREATE_BASEDIR
//
// MessageText:
//
// Application %1!s! (%2!s!) unable to create directory %3!s! for forensics file
//
#define MSG_ERROR_UNABLE_TO_CREATE_BASEDIR ((uint)0xC0FF03FCL)

//
// MessageId: MSG_WARNING_EMPTY_OR_NONEXISTENT_LOGDIR_KEY
//
// MessageText:
//
// Application %1!s! (%2!s!) has no directory specified for forensics files
//
#define MSG_WARNING_EMPTY_OR_NONEXISTENT_LOGDIR_KEY ((uint)0x80FF03FDL)

//
// MessageId: MSG_UNSUPPORTED_OS_VERSION
//
// MessageText:
//
// Application %1!s! (%2!s!) is running on an unsupported operating system (%3!s!)
//
#define MSG_UNSUPPORTED_OS_VERSION       ((uint)0x80FF03FEL)

//
// MessageId: MSG_SEC_FORENSICS
//
// MessageText:
//
// Forensics file for security violation in application %1!s! (%2!s!) created at %3!s!
//
#define MSG_SEC_FORENSICS                ((uint)0x47FF0005L)

//
// MessageId: MSG_ERROR_OPTION_TOO_LONG_TO_PARSE
//
// MessageText:
//
// Application %1!s! (%2!s!) option %3!s! is too long to parse. %4!s!
//
#define MSG_ERROR_OPTION_TOO_LONG_TO_PARSE ((uint)0xC0FF03FFL)

//
// MessageId: MSG_ERROR_OPTION_BAD_NUMBER_FORMAT
//
// MessageText:
//
// Application %1!s! (%2!s!). Option parsing error : unrecognized number format %3!s!. %4!s!
//
#define MSG_ERROR_OPTION_BAD_NUMBER_FORMAT ((uint)0xC0FF0400L)

//
// MessageId: MSG_ERROR_OPTION_UNKNOWN_SIZE_SPECIFIER
//
// MessageText:
//
// Application %1!s! (%2!s!). Option parsing error : unrecognized size factor %3!s!. %4!s!
//
#define MSG_ERROR_OPTION_UNKNOWN_SIZE_SPECIFIER ((uint)0xC0FF0401L)

//
// MessageId: MSG_ERROR_OPTION_UNKNOWN_TIME_SPECIFIER
//
// MessageText:
//
// Application %1!s! (%2!s!). Option parsing error : unrecognized time factor %3!s!. %4!s!
//
#define MSG_ERROR_OPTION_UNKNOWN_TIME_SPECIFIER ((uint)0xC0FF0402L)

//
// MessageId: MSG_ERROR_OPTION_UNKNOWN
//
// MessageText:
//
// Application %1!s! (%2!s!). Option parsing error : unknown option %3!s!. %4!s!
//
#define MSG_ERROR_OPTION_UNKNOWN         ((uint)0xC0FF0403L)

//
// MessageId: MSG_ERROR_CONFIG_FILE_INVALID
//
// MessageText:
//
// Application %1!s! (%2!s!). Config file parsing error : invalid line %3!s!.
//
#define MSG_ERROR_CONFIG_FILE_INVALID    ((uint)0xC0FF0404L)

//
// MessageId: MSG_BAD_OS_VERSION
//
// MessageText:
//
// Application %1!s! (%2!s!) %3!s! does not run on %4!s!
//
#define MSG_BAD_OS_VERSION               ((uint)0xC0FF0405L)

//
// MessageId: MSG_OPTION_VERIFICATION_RECURSION
//
// MessageText:
//
// Application %1!s! (%2!s!) bad option string, unable to continue.
//
#define MSG_OPTION_VERIFICATION_RECURSION ((uint)0xC0FF0406L)

//
// MessageId: MSG_TAMPERED_NTDLL
//
// MessageText:
//
// Application %1!s! (%2!s!). System library ntdll.dll has been tampered with, unable to continue.
//
#define MSG_TAMPERED_NTDLL               ((uint)0xC0FF0407L)

#ifdef CHECK_RETURNS_SSE2
//
// MessageId: MSG_CHECK_RETURNS_SSE2_XMM_USED
//
// MessageText:
//
// Application %1!s! (%2!s!). Check returns using SSE2 assumption violated, app is using the xmm registers
//
#define MSG_CHECK_RETURNS_SSE2_XMM_USED  ((uint)0xC0FF0408L)

//
// MessageId: MSG_CHECK_RETURNS_SSE2_REQUIRES_SSE2
//
// MessageText:
//
// Application %1!s! (%2!s!). Check returns using SSE2 requires that the processor support SSE2
//
#define MSG_CHECK_RETURNS_SSE2_REQUIRES_SSE2 ((uint)0xC0FF0409L)

#endif
//
// MessageId: MSG_EXTERNAL_ERROR
//
// MessageText:
//
// Application %1!s! (%2!s!) %3!s! usage error : %4!s!
//
#define MSG_EXTERNAL_ERROR               ((uint)0xC0FF040AL)

//
// MessageId: MSG_LDMP
//
// MessageText:
//
// Core dump file for application %1!s! (%2!s!) created at %3!s!
//
#define MSG_LDMP                         ((uint)0x47FF0006L)

#ifdef HOT_PATCHING_INTERFACE
//
// MessageId: MSG_HOT_PATCH_VIOLATION
//
// MessageText:
//
// A security violation was intercepted in application %1!s! (%2!s!).
// Threat ID: %3!s!.
//
#define MSG_HOT_PATCH_VIOLATION          ((uint)0x47FF0007L)

//
// MessageId: MSG_HOT_PATCH_FAILURE
//
// MessageText:
//
// A LiveShield Sentry failure was intercepted in application %1!s! (%2!s!) at address %3!s!.
//
#define MSG_HOT_PATCH_FAILURE            ((uint)0xC0FF040BL)

#endif
//
// MessageId: MSG_ERROR_REGISTRY_PARAMETER_TOO_LONG
//
// MessageText:
//
// Application %1!s! (%2!s!). Error reading registry : registry parameter %3!s! exceeds maximum length.
//
#define MSG_ERROR_REGISTRY_PARAMETER_TOO_LONG ((uint)0xC0FF040CL)

// FIXME - do we want a more cryptic error messages (such as the out of memory one) that
// requires going back to Determina for resolution, or do we want the message to be
// potentially actionable by the customer? FIXME - warning or error?
//
// MessageId: MSG_LOW_ON_VMM_MEMORY
//
// MessageText:
//
// Application %1!s! (%2!s!).  Potentially thrashing on low virtual memory; suggest increasing the -vm_size option for this application.
//
#define MSG_LOW_ON_VMM_MEMORY            ((uint)0x80FF040DL)

//
// MessageId: MSG_LOW_ON_COMMITTABLE_MEMORY
//
// MessageText:
//
// Application %1!s! (%2!s!).  Potentially thrashing on low available system memory; suggest increasing initial pagefile size or adding additional RAM to this machine.
//
#define MSG_LOW_ON_COMMITTABLE_MEMORY    ((uint)0x80FF040EL)

#ifdef PROCESS_CONTROL
//
// MessageId: MSG_PROC_CTL_HASH_LIST_TOO_LONG
//
// MessageText:
//
// Application %1!s! (%2!s!). Process control hash list %3!s! has more than %4!s! MD5 hashes, so process control has been disable.  Try doubling -pc_num_hashes to start process control again.
//
#define MSG_PROC_CTL_HASH_LIST_TOO_LONG  ((uint)0xC0FF040FL)

#endif
#ifdef X64
//
// MessageId: MSG_HEAP_CONTRAINTS_UNSATISFIABLE
//
// MessageText:
//
// Application %1!s! (%2!s!). Unable to place the heap in a manner that satisfies all 32bit displacement requirements.  Check -vm_base, -vm_offset, -heap_in_lower_4GB, and dll prefferred base addresses for issues.
//
#define MSG_HEAP_CONTRAINTS_UNSATISFIABLE ((uint)0xC0FF0410L)

//
// MessageId: MSG_UNSUPPORTED_PROCESSOR_LAHF
//
// MessageText:
//
// Application %1!s! (%2!s!). Unsupported processor: LAHF/SAHF instructions required in 64-bit mode.
//
#define MSG_UNSUPPORTED_PROCESSOR_LAHF   ((uint)0xC0FF0411L)

#endif
#ifdef CLIENT_INTERFACE
//
// MessageId: MSG_CLIENT_LIBRARY_UNLOADABLE
//
// MessageText:
//
// Application %1!s! (%2!s!). Unable to load client library: %3!s!%4!s!.
//
#define MSG_CLIENT_LIBRARY_UNLOADABLE    ((uint)0xC0FF0412L)

#endif
//
// MessageId: MSG_CLIENT_LIBRARY_WRONG_BITWIDTH
//
// MessageText:
//
// Application %1!s! (%2!s!). Library has wrong bitwidth: %3!s!.
//
#define MSG_CLIENT_LIBRARY_WRONG_BITWIDTH ((uint)0xC0FF0413L)

#ifdef CLIENT_INTERFACE
//
// MessageId: MSG_CLIENT_VERSION_INCOMPATIBLE
//
// MessageText:
//
// Application %1!s! (%2!s!). Client library targets an incompatible API version and should be re-compiled.
//
#define MSG_CLIENT_VERSION_INCOMPATIBLE  ((uint)0xC0FF0414L)

#endif
#ifdef CLIENT_INTERFACE
//
// MessageId: MSG_INSTRUMENTATION_TOO_LARGE
//
// MessageText:
//
// Application %1!s! (%2!s!). Basic block or trace instrumentation exceeded maximum size.  Try lowering -max_bb_instrs and/or -max_trace_bbs.
//
#define MSG_INSTRUMENTATION_TOO_LARGE    ((uint)0xC0FF0415L)

#endif
//
// MessageId: MSG_SYSENTER_NOT_SUPPORTED
//
// MessageText:
//
// Application %1!s! (%2!s!). System calls using sysenter are not supported on this operating system.
//
#define MSG_SYSENTER_NOT_SUPPORTED       ((uint)0xC0FF0416L)

#ifdef CLIENT_INTERFACE
//
// MessageId: MSG_WAITING_FOR_DEBUGGER
//
// MessageText:
//
// Application %1!s! (%2!s!). Waiting for debugger to attach.
//
#define MSG_WAITING_FOR_DEBUGGER         ((uint)0xC0FF0417L)

#endif
#ifdef VMX86_SERVER
//
// MessageId: MSG_DRVMKLIB_UNLOADABLE
//
// MessageText:
//
// Application %1!s! (%2!s!). Error loading or using vmklib library: %3!s!.
//
#define MSG_DRVMKLIB_UNLOADABLE          ((uint)0xC0FF0418L)

#endif
#ifdef CLIENT_INTERFACE
//
// MessageId: MSG_TOO_MANY_TLS_MODS
//
// MessageText:
//
// Max number of modules with tls variables exceeded.
//
#define MSG_TOO_MANY_TLS_MODS            ((uint)0xC0FF0419L)

#endif
#if defined(UNIX) && defined(CLIENT_INTERFACE)
//
// MessageId: MSG_UNDEFINED_SYMBOL
//
// MessageText:
//
// WARNING! symbol lookup error: %1!s! undefined symbol %2!s!
//
#define MSG_UNDEFINED_SYMBOL             ((uint)0x80FF041AL)

//
// MessageId: MSG_UNDEFINED_SYMBOL_REFERENCE
//
// MessageText:
//
// ERROR: using undefined symbol!
//
#define MSG_UNDEFINED_SYMBOL_REFERENCE   ((uint)0xC0FF041BL)

#endif
#ifdef UNIX
//
// MessageId: MSG_FIXED_MAP_OVERLAPS_DR
//
// MessageText:
//
// Application %1!s! (%2!s!). A fixed memory map (%3!s!) overlaps with DynamoRIO libraries.
//
#define MSG_FIXED_MAP_OVERLAPS_DR        ((uint)0xC0FF041CL)

#endif
#ifdef UNIX
//
// MessageId: MSG_CANNOT_FIND_VFP_FRAME
//
// MessageText:
//
// Application %1!s! (%2!s!). Cannot identify VFP frame offset.
//
#define MSG_CANNOT_FIND_VFP_FRAME        ((uint)0xC0FF041DL)

#endif
//
// MessageId: MSG_FAILED_TO_SYNCHRONIZE_THREADS
//
// MessageText:
//
// Application %1!s! (%2!s!). Failed to synchronize with all threads when detaching.
//
#define MSG_FAILED_TO_SYNCHRONIZE_THREADS ((uint)0xC0FF041EL)

#ifdef UNIX
//
// MessageId: MSG_FAILED_TO_HANDLE_SIGNAL
//
// MessageText:
//
// Application %1!s! (%2!s!). Cannot correctly handle received signal %3!s! in thread %4!s!.
//
#define MSG_FAILED_TO_HANDLE_SIGNAL      ((uint)0xC0FF041FL)

#endif
//
// MessageId: MSG_FAILED_TO_TAKE_OVER_THREADS
//
// MessageText:
//
// Application %1!s! (%2!s!). Failed to take over all threads after multiple attempts.
//
#define MSG_FAILED_TO_TAKE_OVER_THREADS  ((uint)0xC0FF0420L)

#ifdef UNIX
//
// MessageId: MSG_FAILED_TO_FIND_DR_BOUNDS
//
// MessageText:
//
// Application %1!s! (%2!s!). Failed to find DynamoRIO library bounds.
//
#define MSG_FAILED_TO_FIND_DR_BOUNDS     ((uint)0xC0FF0421L)

#endif
//
// MessageId: MSG_FAILED_TO_SATISFY_W_XOR_X
//
// MessageText:
//
// Application %1!s! (%2!s!). Failed to satisfy W^X policies.
//
#define MSG_FAILED_TO_SATISFY_W_XOR_X    ((uint)0xC0FF0422L)

//
// MessageId: MSG_OUT_OF_VMM_CANNOT_USE_OS
//
// MessageText:
//
// Application %1!s! (%2!s!). Out of contiguous memory. %3!s!
//
#define MSG_OUT_OF_VMM_CANNOT_USE_OS     ((uint)0xC0FF0423L)

//
// MessageId: MSG_AVX_512_SUPPORT_INCOMPLETE
//
// MessageText:
//
// Application %1!s! (%2!s!): AVX-512 was detected at PC %3!s!. AVX-512 is not fully supported yet.
//
#define MSG_AVX_512_SUPPORT_INCOMPLETE   ((uint)0xC0FF0424L)

//
// MessageId: MSG_RSEQ_BEHAVIOR_UNSUPPORTED
//
// MessageText:
//
// Application %1!s! (%2!s!). Restartable sequence behavior is not supported: %3!s!.
//
#define MSG_RSEQ_BEHAVIOR_UNSUPPORTED    ((uint)0xC0FF0425L)

//
// MessageId: MSG_FAILED_TO_ALLOCATE_TLS
//
// MessageText:
//
// Application %1!s! (%2!s!). Unable to allocate TLS slots. %3!s!
//
#define MSG_FAILED_TO_ALLOCATE_TLS       ((uint)0xC0FF0426L)

//
// MessageId: MSG_PRIVATE_LIBRARY_TLS_LIMIT_CROSSED
//
// MessageText:
//
// Application %1!s! (%2!s!). Private library static TLS limit crossed: %3!s!
//
#define MSG_PRIVATE_LIBRARY_TLS_LIMIT_CROSSED ((uint)0xC0FF0427L)

// ADD NEW MESSAGES HERE
