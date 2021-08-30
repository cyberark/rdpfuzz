#define MSG_CATEGORY_SECURITY_STRING "Security"
#define MSG_INFO_PRODUCT_VERSION_STRING "%s (R) %s (R) %s %s installed."
#define MSG_INFO_PROCESS_START_STRING "Starting application %s (%s)\nMD5: %s"
#define MSG_INFO_PROCESS_START_CLIENT_STRING "Starting application %s (%s)"
#define MSG_INFO_PROCESS_STOP_STRING "Stopping application %s (%s)"
#define MSG_INFO_DETACHING_STRING "Detaching from application %s (%s)"
#define MSG_INFO_ATTACHED_STRING "Attached to %s threads in application %s (%s)"
#define MSG_INFO_RESET_IN_PROGRESS_STRING "Resetting caches and non-persistent memory @ %s fragments in application %s (%s)."
#define MSG_SEC_VIOLATION_TERMINATED_STRING "A security violation was intercepted in application %s (%s).\nThreat ID: %s.\nProgram terminated."
#define MSG_SEC_VIOLATION_CONTINUE_STRING "A security violation was intercepted in application %s (%s).\nThreat ID: %s.\nRunning in test mode - program continuing.\nYour system may have been compromised."
#define MSG_SEC_VIOLATION_THREAD_STRING "A security violation was intercepted in application %s (%s).\nThreat ID: %s.\nProgram continuing after terminating faulty thread.\nSome application functionality may be lost."
#define MSG_SEC_VIOLATION_EXCEPTION_STRING "A security violation was intercepted in application %s (%s).\nThreat ID: %s.\nProgram continuing after throwing an exception.\nApplication may terminate."
#define MSG_INTERNAL_SYSLOG_CRITICAL_STRING "Application %s (%s).  Internal Critical Error: %s"
#define MSG_INTERNAL_SYSLOG_ERROR_STRING "Application %s (%s).  Internal Error: %s"
#define MSG_INTERNAL_SYSLOG_WARNING_STRING "Application %s (%s).  Internal Warning: %s"
#define MSG_INTERNAL_SYSLOG_INFORMATION_STRING "Application %s (%s).  Internal Information: %s"
#define MSG_EXCEPTION_STRING "Application %s (%s).  %s %s at PC %s.  Please report this at %s.  Program aborted.%s"
#define MSG_CLIENT_EXCEPTION_STRING "Application %s (%s).  %s %s at PC %s.  Please report this at %s.  Program aborted.%s"
#define MSG_INVOKING_APP_HANDLER_STRING "Invoking fault handler for application %s (%s)."
#define MSG_APP_EXCEPTION_STRING "Application %s (%s).  Application exception at PC %s.  %s"
#define MSG_OUT_OF_MEMORY_STRING "Application %s (%s).  Out of memory.  Program aborted.  Source %s, type %s, code %s."
#define MSG_MAX_PENDING_SIGNALS_STRING "Application %s (%s).  Reached the soft maximum number (%s) of pending signals: further accumulation prior to delivery may cause problems.  Consider raising -max_pending_signals."
#define MSG_UNSUPPORTED_APPLICATION_STRING "Application %s (%s) is not supported due to dll %s.  Program aborted."
#define MSG_WSL_UNSUPPORTED_FATAL_STRING "Application %s (%s).  The Windows Subsystem for Linux is not yet supported due to missing segment support from the kernel.  Program aborted."
#define MSG_WARNING_REPORT_THRESHOLD_STRING "Application %s (%s) has reached its report threshold.  No more violations will be logged."
#define MSG_ERROR_UNABLE_TO_CREATE_BASEDIR_STRING "Application %s (%s) unable to create directory %s for forensics file"
#define MSG_WARNING_EMPTY_OR_NONEXISTENT_LOGDIR_KEY_STRING "Application %s (%s) has no directory specified for forensics files"
#define MSG_UNSUPPORTED_OS_VERSION_STRING "Application %s (%s) is running on an unsupported operating system (%s)"
#define MSG_SEC_FORENSICS_STRING "Forensics file for security violation in application %s (%s) created at %s"
#define MSG_ERROR_OPTION_TOO_LONG_TO_PARSE_STRING "Application %s (%s) option %s is too long to parse. %s"
#define MSG_ERROR_OPTION_BAD_NUMBER_FORMAT_STRING "Application %s (%s). Option parsing error : unrecognized number format %s. %s"
#define MSG_ERROR_OPTION_UNKNOWN_SIZE_SPECIFIER_STRING "Application %s (%s). Option parsing error : unrecognized size factor %s. %s"
#define MSG_ERROR_OPTION_UNKNOWN_TIME_SPECIFIER_STRING "Application %s (%s). Option parsing error : unrecognized time factor %s. %s"
#define MSG_ERROR_OPTION_UNKNOWN_STRING "Application %s (%s). Option parsing error : unknown option %s. %s"
#define MSG_ERROR_CONFIG_FILE_INVALID_STRING "Application %s (%s). Config file parsing error : invalid line %s."
#define MSG_BAD_OS_VERSION_STRING "Application %s (%s) %s does not run on %s"
#define MSG_OPTION_VERIFICATION_RECURSION_STRING "Application %s (%s) bad option string, unable to continue."
#define MSG_TAMPERED_NTDLL_STRING "Application %s (%s). System library ntdll.dll has been tampered with, unable to continue."
#define MSG_CHECK_RETURNS_SSE2_XMM_USED_STRING "Application %s (%s). Check returns using SSE2 assumption violated, app is using the xmm registers"
#define MSG_CHECK_RETURNS_SSE2_REQUIRES_SSE2_STRING "Application %s (%s). Check returns using SSE2 requires that the processor support SSE2"
#define MSG_EXTERNAL_ERROR_STRING "Application %s (%s) %s usage error : %s"
#define MSG_LDMP_STRING "Core dump file for application %s (%s) created at %s"
#define MSG_HOT_PATCH_VIOLATION_STRING "A security violation was intercepted in application %s (%s).\nThreat ID: %s."
#define MSG_HOT_PATCH_FAILURE_STRING "A LiveShield Sentry failure was intercepted in application %s (%s) at address %s."
#define MSG_ERROR_REGISTRY_PARAMETER_TOO_LONG_STRING "Application %s (%s). Error reading registry : registry parameter %s exceeds maximum length."
#define MSG_LOW_ON_VMM_MEMORY_STRING "Application %s (%s).  Potentially thrashing on low virtual memory; suggest increasing the -vm_size option for this application."
#define MSG_LOW_ON_COMMITTABLE_MEMORY_STRING "Application %s (%s).  Potentially thrashing on low available system memory; suggest increasing initial pagefile size or adding additional RAM to this machine."
#define MSG_PROC_CTL_HASH_LIST_TOO_LONG_STRING "Application %s (%s). Process control hash list %s has more than %s MD5 hashes, so process control has been disable.  Try doubling -pc_num_hashes to start process control again."
#define MSG_HEAP_CONTRAINTS_UNSATISFIABLE_STRING "Application %s (%s). Unable to place the heap in a manner that satisfies all 32bit displacement requirements.  Check -vm_base, -vm_offset, -heap_in_lower_4GB, and dll prefferred base addresses for issues."
#define MSG_UNSUPPORTED_PROCESSOR_LAHF_STRING "Application %s (%s). Unsupported processor: LAHF/SAHF instructions required in 64-bit mode."
#define MSG_CLIENT_LIBRARY_UNLOADABLE_STRING "Application %s (%s). Unable to load client library: %s%s."
#define MSG_CLIENT_LIBRARY_WRONG_BITWIDTH_STRING "Application %s (%s). Library has wrong bitwidth: %s."
#define MSG_CLIENT_VERSION_INCOMPATIBLE_STRING "Application %s (%s). Client library targets an incompatible API version and should be re-compiled."
#define MSG_INSTRUMENTATION_TOO_LARGE_STRING "Application %s (%s). Basic block or trace instrumentation exceeded maximum size.  Try lowering -max_bb_instrs and/or -max_trace_bbs."
#define MSG_SYSENTER_NOT_SUPPORTED_STRING "Application %s (%s). System calls using sysenter are not supported on this operating system."
#define MSG_WAITING_FOR_DEBUGGER_STRING "Application %s (%s). Waiting for debugger to attach."
#define MSG_DRVMKLIB_UNLOADABLE_STRING "Application %s (%s). Error loading or using vmklib library: %s."
#define MSG_TOO_MANY_TLS_MODS_STRING "Max number of modules with tls variables exceeded."
#define MSG_UNDEFINED_SYMBOL_STRING "WARNING! symbol lookup error: %s undefined symbol %s"
#define MSG_UNDEFINED_SYMBOL_REFERENCE_STRING "ERROR: using undefined symbol!"
#define MSG_FIXED_MAP_OVERLAPS_DR_STRING "Application %s (%s). A fixed memory map (%s) overlaps with DynamoRIO libraries."
#define MSG_CANNOT_FIND_VFP_FRAME_STRING "Application %s (%s). Cannot identify VFP frame offset."
#define MSG_FAILED_TO_SYNCHRONIZE_THREADS_STRING "Application %s (%s). Failed to synchronize with all threads when detaching."
#define MSG_FAILED_TO_HANDLE_SIGNAL_STRING "Application %s (%s). Cannot correctly handle received signal %s in thread %s."
#define MSG_FAILED_TO_TAKE_OVER_THREADS_STRING "Application %s (%s). Failed to take over all threads after multiple attempts."
#define MSG_FAILED_TO_FIND_DR_BOUNDS_STRING "Application %s (%s). Failed to find DynamoRIO library bounds."
#define MSG_FAILED_TO_SATISFY_W_XOR_X_STRING "Application %s (%s). Failed to satisfy W^X policies."
#define MSG_OUT_OF_VMM_CANNOT_USE_OS_STRING "Application %s (%s). Out of contiguous memory. %s"
#define MSG_AVX_512_SUPPORT_INCOMPLETE_STRING "Application %s (%s): AVX-512 was detected at PC %s. AVX-512 is not fully supported yet."
#define MSG_RSEQ_BEHAVIOR_UNSUPPORTED_STRING "Application %s (%s). Restartable sequence behavior is not supported: %s."
#define MSG_FAILED_TO_ALLOCATE_TLS_STRING "Application %s (%s). Unable to allocate TLS slots. %s"
#define MSG_PRIVATE_LIBRARY_TLS_LIMIT_CROSSED_STRING "Application %s (%s). Private library static TLS limit crossed: %s"
