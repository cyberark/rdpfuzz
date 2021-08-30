/* **********************************************************
 * Copyright (c) 2010-2011 Google, Inc.  All rights reserved.
 * Copyright (c) 2002-2010 VMware, Inc.  All rights reserved.
 * **********************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of VMware, Inc. nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL VMWARE, INC. OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef _DR_TOOLS_H_
#define _DR_TOOLS_H_ 1

/**************************************************
 * STATE SWAPPING TYPES
 */

/**
 * Flags that control the behavior of dr_switch_to_app_state_ex()
 * and dr_switch_to_dr_state_ex().
 */
typedef enum {
#ifdef WINDOWS
    DR_STATE_PEB = 0x0001,          /**< Switch the PEB pointer. */
    DR_STATE_TEB_MISC = 0x0002,     /**< Switch miscellaneous TEB fields. */
    DR_STATE_STACK_BOUNDS = 0x0004, /**< Switch the TEB stack bounds fields. */
    DR_STATE_ALL = ~0,              /**< Switch all state. */
#else
    /**
     * On Linux, DR's own TLS can optionally be swapped, but this is risky
     * and not recommended as incoming signals are not properly handled when in
     * such a state.  Thus DR_STATE_ALL does *not* swap it.
     */
    DR_STATE_DR_TLS = 0x0001,
    DR_STATE_ALL = (~0) & (~DR_STATE_DR_TLS), /**< Switch all normal state. */
    DR_STATE_GO_NATIVE = ~0,                  /**< Switch all state.  Use with care. */
#endif
} dr_state_flags_t;


/**
 * Encodings of an application's command-line argument.
 */
typedef enum {
    /**
     * C String Encoding.
     */
    DR_APP_ARG_CSTR_COMPAT,
    /**
     * UTF 16 String Encoding.
     */
    DR_APP_ARG_UTF_16,
} dr_app_arg_encoding_t;

/**
 * Contains information regarding an application's command-line argument.
 */
typedef struct _dr_app_arg_t {
    /**
     * The start boundary where the content of the arg begins.
     */
    void *start;
    /**
     * The size, in bytes, of the argument.
     */
    size_t size;
    /**
     * The encoding of the argument.
     */
    dr_app_arg_encoding_t encoding;
} dr_app_arg_t;

/**************************************************
 * CLIENT AUXILIARY LIBRARY TYPES
 */

/**
 * A handle to a loaded client auxiliary library.  This is a different
 * type than module_handle_t and is not necessarily the base address.
 */
typedef void *dr_auxlib_handle_t;
/** An exported routine in a loaded client auxiliary library. */
typedef void (*dr_auxlib_routine_ptr_t)();
#if defined(WINDOWS) && !defined(X64)
/**
 * A handle to a loaded 64-bit client auxiliary library.  This is a different
 * type than module_handle_t and is not necessarily the base address.
 */
typedef uint64 dr_auxlib64_handle_t;
/** An exported routine in a loaded 64-bit client auxiliary library. */
typedef uint64 dr_auxlib64_routine_ptr_t;
#endif

/**************************************************
 * MEMORY INFORMATION TYPES
 */

#define DR_MEMPROT_NONE 0x00  /**< No read, write, or execute privileges. */
#define DR_MEMPROT_READ 0x01  /**< Read privileges. */
#define DR_MEMPROT_WRITE 0x02 /**< Write privileges. */
#define DR_MEMPROT_EXEC 0x04  /**< Execute privileges. */
#define DR_MEMPROT_GUARD 0x08 /**< Guard page (Windows only) */
/**
 * DR's default cache consistency strategy modifies the page protection of
 * pages containing code, making them read-only.  It pretends on application
 * and client queries that the page is writable.  On a write fault
 * to such a region by the application or by client-added instrumentation, DR
 * automatically handles the fault and makes the page writable.  This requires
 * flushing the code from the code cache, which can only be done safely when in
 * an application context.  Thus, a client writing to such a page is only
 * supported when these criteria are met:
 *
 * -# The client code must be in an application code cache context.  This rules
 *    out all event callbacks (including the basic block event) except for the
 *    pre and post system call events and the nudge event.
 * -# The client must not hold any locks.  An exception is a lock marked as
 *    an application lock (via dr_mutex_mark_as_app(), dr_rwlock_mark_as_app(),
 *    or dr_recurlock_mark_as_app()).
 * -# The client code must not rely on returning to a particular point in the
 *    code cache, as that point might be flushed and removed during the write
 *    fault processing.  This rules out a clean call (unless
 *    dr_redirect_execution() is used), but does allow something like
 *    drwrap_replace_native() which uses a continuation strategy.
 *
 * A client write fault that does not meet the first two criteria will result in
 * a fatal error report and an abort.  It is up to the client to ensure it
 * satisifies the third criterion.
 *
 * Even when client writes do meet these criteria, for performance it's best for
 * clients to avoid writing to such memory.
 */
#define DR_MEMPROT_PRETEND_WRITE 0x10
/**
 * In addition to the appropriate DR_MEMPROT_READ and/or DR_MEMPROT_EXEC flags,
 * this flag will be set for the VDSO and VVAR pages on Linux.
 * The VVAR pages may only be identified by DR on kernels that explicitly label
 * the pages in the /proc/self/maps file (kernel 3.15 and above).
 * In some cases, accessing the VVAR pages can cause problems
 * (e.g., https://github.com/DynamoRIO/drmemory/issues/1778).
 */
#define DR_MEMPROT_VDSO 0x20

/**
 * Flags describing memory used by dr_query_memory_ex().
 */
typedef enum {
    DR_MEMTYPE_FREE,     /**< No memory is allocated here */
    DR_MEMTYPE_IMAGE,    /**< An executable file is mapped here */
    DR_MEMTYPE_DATA,     /**< Some other data is allocated here */
    DR_MEMTYPE_RESERVED, /**< Reserved address space with no physical storage */
    DR_MEMTYPE_ERROR,    /**< Query failed for unspecified reason */
    /**
     * Query failed due to the address being located in Windows kernel space.
     * No further information is available so iteration must stop.
     */
    DR_MEMTYPE_ERROR_WINKERNEL,
} dr_mem_type_t;

/**
 * Describes a memory region.  Used by dr_query_memory_ex().
 */
typedef struct _dr_mem_info_t {
    /** Starting address of memory region */
    byte *base_pc;
    /** Size of region */
    size_t size;
    /** Protection of region (DR_MEMPROT_* flags) */
    uint prot;
    /** Type of region */
    dr_mem_type_t type;
} dr_mem_info_t;



/**************************************************
 * MODULE INFORMATION TYPES
 */

/**
 * Type used for dr_get_proc_address().  This can be obtained from the
 * #_module_data_t structure.  It is equivalent to the base address of
 * the module on both Windows and Linux.
 */
struct _module_handle_t;
typedef struct _module_handle_t *module_handle_t;

#ifdef WINDOWS

#define MODULE_FILE_VERSION_INVALID ULLONG_MAX

/**
 * Used to hold .rsrc section version number information. This number is usually
 * presented as p1.p2.p3.p4 by PE parsing tools.
 */
typedef union _version_number_t {
    uint64 version; /**< Representation as a 64-bit integer. */
    struct {
        uint ms;    /**< */
        uint ls;    /**< */
    } version_uint; /**< Representation as 2 32-bit integers. */
    struct {
        ushort p2;   /**< */
        ushort p1;   /**< */
        ushort p4;   /**< */
        ushort p3;   /**< */
    } version_parts; /**< Representation as 4 16-bit integers. */
} version_number_t;

#endif



/**
 * Holds the names of a module.  This structure contains multiple
 * fields corresponding to different sources of a module name.  Note
 * that some of these names may not exist for certain modules.  It is
 * highly likely, however, that at least one name is available.  Use
 * dr_module_preferred_name() on the parent _module_data_t to get the
 * preferred name of the module.
 */
typedef struct _module_names_t {
    const char *module_name; /**< On windows this name comes from the PE header exports
                              * section (NULL if the module has no exports section).  On
                              * Linux the name will come from the ELF DYNAMIC program
                              * header (NULL if the module has no SONAME entry). */
    const char *file_name; /**< The file name used to load this module. Note - on Windows
                            * this is not always available. */
#ifdef WINDOWS
    const char *exe_name;  /**< If this module is the main executable of this process then
                            * this is the executable name used to launch the process (NULL
                            * for all other modules). */
    const char *rsrc_name; /**< The internal name given to the module in its resource
                            * section. Will be NULL if the module has no resource section
                            * or doesn't set this field within it. */
#else                      /* UNIX */
    uint64 inode; /**< The inode of the module file mapped in. */
#endif
} module_names_t;




/**************************************************
 * TOP-LEVEL ROUTINES
 */
/**
 * @file dr_tools.h
 * @brief Main API routines, including transparency support.
 */


/**
 * Creates a DR context that can be used in a standalone program.
 * \warning This context cannot be used as the drcontext for a thread
 * running under DR control!  It is only for standalone programs that
 * wish to use DR as a library of disassembly, etc. routines.
 * \return NULL on failure, such as running on an unsupported operating
 * system version.
 */
void *
dr_standalone_init(void);

/**
 * Restores application state modified by dr_standalone_init(), which can
 * include some signal handlers.
 */
void
dr_standalone_exit(void);

/**
 * Use this dcontext for use with the standalone static decoder library.
 * Pass it whenever a decoding-related API routine asks for a context.
 */
#define GLOBAL_DCONTEXT ((void *)-1)

/**************************************************
 * UTILITY ROUTINES
 */

#ifdef WINDOWS
/**
 * If \p x is false, displays a message about an assertion failure
 * (appending \p msg to the message) and then calls dr_abort()
 */
#    define DR_ASSERT_MSG(x, msg)                                                   \
            ((void)((!(x)) ? (dr_messagebox("ASSERT FAILURE: %s:%d: %s (%s)", __FILE__, \
                                            __LINE__, #x, msg),                         \
                              dr_abort(), 0)                                            \
                           : 0))
#else
#    define DR_ASSERT_MSG(x, msg)                                                \
            ((void)((!(x)) ? (dr_fprintf(STDERR, "ASSERT FAILURE: %s:%d: %s (%s)\n", \
                                         __FILE__, __LINE__, #x, msg),               \
                              dr_abort(), 0)                                         \
                           : 0))
#endif

/**
 * If \p x is false, displays a message about an assertion failure and
 * then calls dr_abort()
 */
#define DR_ASSERT(x) DR_ASSERT_MSG(x, "")



/** Returns true if all DynamoRIO caches are thread private. */
bool
dr_using_all_private_caches(void);

/** \deprecated Replaced by dr_set_process_exit_behavior() */
void
dr_request_synchronized_exit(void);

/**
 * Returns the client-specific option string specified at client
 * registration.  \p client_id is the client ID passed to dr_client_main().
 *
 * \deprecated This routine is replaced by dr_client_main()'s arguments and
 * by dr_get_option_array().
 * The front-end \p drrun and other utilities now re-quote all tokens,
 * providing simpler option passing without escaping or extra quote layers.
 * This routine, for compatibility, strips those quotes off and returns
 * a flat string without any token-delimiting quotes.
 */
const char *
dr_get_options(client_id_t client_id);

/**
 * Returns the client-specific option string specified at client
 * registration, parsed into an array of \p argc separate option tokens
 * stored in \p argv.  This is the same array of arguments passed
 * to the dr_client_main() routine.
 */
bool
dr_get_option_array(client_id_t client_id, int *argc OUT, const char ***argv OUT);

/**
 * Read the value of a string DynamoRIO runtime option named \p option_name into
 * \p buf.  Options are listed in \ref sec_options.  DynamoRIO has many other
 * undocumented options which may be queried through this API, but they are not
 * officially supported.  The option value is truncated to \p len bytes and
 * null-terminated.
 * \return false if no option named \p option_name exists, and true otherwise.
 */
bool
dr_get_string_option(const char *option_name, char *buf OUT, size_t len);

/**
 * Read the value of an integer DynamoRIO runtime option named \p option_name
 * into \p val.  This includes boolean options.  Options are listed in \ref
 * sec_options.  DynamoRIO has many other undocumented options which may be
 * queried through this API, but they are not officially supported.
 * \warning Always pass a full uint64 for \p val even if the option is a smaller
 * integer to avoid overwriting nearby data.
 * \return false if no option named \p option_name exists, and true otherwise.
 */
bool
dr_get_integer_option(const char *option_name, uint64 *val OUT);

/**
 * Returns the client library name and path that were originally specified
 * to load the library.  If the resulting string is longer than #MAXIMUM_PATH
 * it will be truncated.  \p client_id is the client ID passed to a client's
 * dr_client_main() function.
 */
const char *
dr_get_client_path(client_id_t client_id);

/**
 * Returns the base address of the client library.  \p client_id is
 * the client ID passed to a client's dr_client_main() function.
 */
byte *
dr_get_client_base(client_id_t client_id);

/**
 * Sets information presented to users in diagnostic messages.
 * Only one name is supported, regardless of how many clients are in use.
 * If this routine is called a second time, the new values supersede
 * the original.
 * The \p report_URL is meant to be a bug tracker location where users
 * should go to report errors in the client end-user tool.
 */
bool
dr_set_client_name(const char *name, const char *report_URL);

/**
 * Sets the version string presented to users in diagnostic messages.
 * This has a maximum length of 96 characters; anything beyond that is
 * silently truncated.
 */
bool
dr_set_client_version_string(const char *version);

/**
 * Returns the error code of the last failed API routine. Users should check whether or
 * not the API routine that they just called has failed prior to calling this function.
 *
 * \warning Not all API routines currently support the registering of an error code upon
 * their failure. Therefore, check the routine's documentation to see whether it supports
 * setting error codes.
 */
dr_error_code_t
dr_get_error_code(void *drcontext);

/** Returns the image name (without path) of the current application. */
const char *
dr_get_application_name(void);

/**
 * Provides information about the app's arguments by setting \p args_array up to
 * the count denoted by \p args_count. Therefore, \p args_count is not the size of the
 * buffer in bytes but the number of #dr_app_arg_t values that \p args_array can store.
 * Returns the number of args set or -1 on error.
 *
 * Use dr_app_arg_as_cstring() to get the argument as a string.
 *
 * Use dr_num_app_args() to query the total number of command-line arguments passed to
 * the application.
 *
 * \note Currently, this function is only available on Unix with
 * early injection.
 *
 * \note An error code may be obtained via dr_get_error_code() when this routine fails.
 */
int
dr_get_app_args(OUT dr_app_arg_t *args_array, int args_count);

/**
 * Returns the number of command-line arguments passed to the application.
 *
 * \note Currently, this function is only available on Unix with
 * early injection.
 *
 * \note An error code may be obtained via dr_get_error_code() when this routine fails.
 */
int
dr_num_app_args(void);

/**
 * Returns the passed argument \p app_arg as a string. \p buf is used only if needed, and
 * therefore the caller should not assume that the string is in the \p buf. In other
 * words, always use the returned value to refer to the string. Returns NULL on error
 * such as when \p buf is needed as storage and the size of the buffer \p buf_size
 * is not sufficient.
 *
 * To obtain a suitable upper-bound size of the string buffer, get the size of the
 * argument from the #dr_app_arg_t value retrieved via dr_get_app_args().
 *
 * \note Currently, this function is only available on Unix with
 * early injection.
 *
 * \note An error code may be obtained via dr_get_error_code() when this routine fails.
 */
const char *
dr_app_arg_as_cstring(IN dr_app_arg_t *app_arg, char *buf, int buf_size);

/** Returns the process id of the current process. */
process_id_t
dr_get_process_id(void);

/**
 * Returns the process id of the process associated with drcontext \p drcontext.
 * The returned value may be different from dr_get_process_id() if the passed context
 * was created in a different process, which may happen in thread exit callbacks.
 */
process_id_t
dr_get_process_id_from_drcontext(void *drcontext);

/**
 * Returns the process id of the parent of the current process.
 * \note Linux only.
 */
process_id_t
dr_get_parent_id(void);
#ifdef WINDOWS

/** Windows versions */
/* http://msdn.microsoft.com/en-us/library/windows/desktop/ms724832(v=vs.85).aspx */
typedef enum {
    /** Windows 10 1803 major update. */
    DR_WINDOWS_VERSION_10_1803 = 105,
    /** Windows 10 1709 major update. */
    DR_WINDOWS_VERSION_10_1709 = 104,
    /** Windows 10 1703 major update. */
    DR_WINDOWS_VERSION_10_1703 = 103,
    /** Windows 10 1607 major update. */
    DR_WINDOWS_VERSION_10_1607 = 102,
    /**
     * Windows 10 TH2 1511.  For future Windows updates that change system call
     * numbers, we'll perform our own artificial minor version number update as
     * done here, and use the YYMM version as the sub-name, as officially the OS
     * version will supposedly remain 10.0 forever.
     */
    DR_WINDOWS_VERSION_10_1511 = 101,
    /** Windows 10 pre-TH2 */
    DR_WINDOWS_VERSION_10 = 100,
    /** Windows 8.1 */
    DR_WINDOWS_VERSION_8_1 = 63,
    /** Windows Server 2012 R2 */
    DR_WINDOWS_VERSION_2012_R2 = DR_WINDOWS_VERSION_8_1,
    /** Windows 8 */
    DR_WINDOWS_VERSION_8 = 62,
    /** Windows Server 2012 */
    DR_WINDOWS_VERSION_2012 = DR_WINDOWS_VERSION_8,
    /** Windows 7 */
    DR_WINDOWS_VERSION_7 = 61,
    /** Windows Server 2008 R2 */
    DR_WINDOWS_VERSION_2008_R2 = DR_WINDOWS_VERSION_7,
    /** Windows Vista */
    DR_WINDOWS_VERSION_VISTA = 60,
    /** Windows Server 2008 */
    DR_WINDOWS_VERSION_2008 = DR_WINDOWS_VERSION_VISTA,
    /** Windows Server 2003 */
    DR_WINDOWS_VERSION_2003 = 52,
    /** Windows XP 64-bit */
    DR_WINDOWS_VERSION_XP_X64 = DR_WINDOWS_VERSION_2003,
    /** Windows XP */
    DR_WINDOWS_VERSION_XP = 51,
    /** Windows 2000 */
    DR_WINDOWS_VERSION_2000 = 50,
    /** Windows NT */
    DR_WINDOWS_VERSION_NT = 40,
} dr_os_version_t;

/** Data structure used with dr_get_os_version() */
typedef struct _dr_os_version_info_t {
    /** The size of this structure.  Set this to sizeof(dr_os_version_info_t). */
    size_t size;
    /** The operating system version */
    dr_os_version_t version;
    /** The service pack major number */
    uint service_pack_major;
    /** The service pack minor number */
    uint service_pack_minor;
    /** The build number. */
    uint build_number;
    /** The release identifier (such as "1803" for a Windows 10 release). */
    char release_id[64];
    /** The edition (such as "Education" or "Professional"). */
    char edition[64];
} dr_os_version_info_t;


/**
 * Returns information about the version of the operating system.
 * Returns whether successful.  \note Windows only.  \note The Windows
 * API routine GetVersionEx may hide distinctions between versions,
 * such as between Windows 8 and Windows 8.1.  DR reports the true
 * low-level version.
 *
 */
bool
dr_get_os_version(dr_os_version_info_t *info);

/**
 * Returns true if this process is a 32-bit process operating on a
 * 64-bit Windows kernel, known as Windows-On-Windows-64, or WOW64.
 * Returns false otherwise.  \note Windows only.
 */
bool
dr_is_wow64(void);

/**
 * Returns a pointer to the application's Process Environment Block
 * (PEB).  DR swaps to a private PEB when running client code, in
 * order to isolate the client and its dependent libraries from the
 * application, so conventional methods of reading the PEB will obtain
 * the private PEB instead of the application PEB.  \note Windows only.
 */
void *
dr_get_app_PEB(void);

/**
 * Converts a process handle to a process id.
 * \return Process id if successful; INVALID_PROCESS_ID on failure.
 * \note Windows only.
 */
process_id_t
dr_convert_handle_to_pid(HANDLE process_handle);

/**
 * Converts a process id to a process handle.
 * \return Process handle if successful; INVALID_HANDLE_VALUE on failure.
 * \note Windows only.
 */
HANDLE
dr_convert_pid_to_handle(process_id_t pid);
#endif /* WINDOWS */


/** Retrieves the current time. */
void
dr_get_time(dr_time_t *time);

/**
 * Returns the number of milliseconds since Jan 1, 1601 (this is
 * the current UTC time).
 *
 * \note This is the Windows standard.  UNIX time functions typically
 * count from the Epoch (Jan 1, 1970).  The Epoch is 11644473600*1000
 * milliseconds after Jan 1, 1601.
 */
uint64
dr_get_milliseconds(void);

/**
 * Returns the number of microseconds since Jan 1, 1601 (this is
 * the current UTC time).
 *
 * \note This is the Windows standard.  UNIX time functions typically
 * count from the Epoch (Jan 1, 1970).  The Epoch is 11644473600*1000*1000
 * microseconds after Jan 1, 1601.
 */
uint64
dr_get_microseconds(void);

/**
 * Returns a pseudo-random number in the range [0..max).
 * The pseudo-random sequence can be repeated by passing the seed
 * used during a run to the next run via the -prng_seed runtime option.
 */
uint
dr_get_random_value(uint max);

/**
 * Sets the seed used for dr_get_random_value().  Generally this would
 * only be called during client initialization.
 */
void
dr_set_random_seed(uint seed);

/** Returns the seed used for dr_get_random_value(). */
uint
dr_get_random_seed(void);

/**
 * Aborts the process immediately without any cleanup (i.e., the exit event
 * will not be called).
 */
void
dr_abort(void);

/**
 * Aborts the process immediately without any cleanup (i.e., the exit event
 * will not be called) with the exit code \p exit_code.
 *
 * On Linux, only the bottom 8 bits of \p exit_code will be honored
 * for a normal exit.  If bits 9..16 are not all zero, DR will send an
 * unhandled signal of that signal number instead of performing a normal
 * exit.
 */
void
dr_abort_with_code(int exit_code);

/**
 * Exits the process, first performing a full cleanup that will
 * trigger the exit event (dr_register_exit_event()).  The process
 * exit code is set to \p exit_code.
 *
 * On Linux, only the bottom 8 bits of \p exit_code will be honored
 * for a normal exit.  If bits 9..16 are not all zero, DR will send an
 * unhandled signal of that signal number instead of performing a normal
 * exit.
 *
 * \note Calling this from \p dr_client_main or from the primary thread's
 * initialization event is not guaranteed to always work, as DR may
 * invoke a thread exit event where a thread init event was never
 * called.  We recommend using dr_abort_ex() or waiting for full
 * initialization prior to use of this routine.
 */
void
dr_exit_process(int exit_code);
/** Indicates the type of memory dump for dr_create_memory_dump(). */
typedef enum {
    /**
     * A "livedump", or "ldmp", DynamoRIO's own custom memory dump format.
     * The ldmp format does not currently support specifying a context
     * for the calling thread, so it will always include the call frames
     * to dr_create_memory_dump().  The \p ldmp.exe tool can be used to
     * create a dummy process (using the \p dummy.exe executable) which
     * can then be attached to by the debugger (use a non-invasive attach)
     * in order to view the memory dump contents.
     *
     * \note Windows only.
     */
    DR_MEMORY_DUMP_LDMP = 0x0001,
} dr_memory_dump_flags_t;

/** Indicates the type of memory dump for dr_create_memory_dump(). */
typedef struct _dr_memory_dump_spec_t {
    /** The size of this structure.  Set this to sizeof(dr_memory_dump_spec_t). */
    size_t size;
    /** The type of memory dump requested. */
    dr_memory_dump_flags_t flags;
    /**
     * This field only applies to DR_MEMORY_DUMP_LDMP.  This string is
     * stored inside the ldmp as the reason for the dump.
     */
    const char *label;
    /**
     * This field only applies to DR_MEMORY_DUMP_LDMP.  This is an optional output
     * field that, if non-NULL, will be written with the path to the created file.
     */
    char *ldmp_path;
    /**
     * This field only applies to DR_MEMORY_DUMP_LDMP.  This is the maximum size,
     * in bytes, of ldmp_path.
     */
    size_t ldmp_path_size;
} dr_memory_dump_spec_t;


/**
 * Requests that DR create a memory dump file of the current process.
 * The type of dump is specified by \p spec.
 *
 * \return whether successful.
 *
 * \note this function is only supported on Windows for now.
 */
bool
dr_create_memory_dump(dr_memory_dump_spec_t *spec);

/**************************************************
 * APPLICATION-INDEPENDENT MEMORY ALLOCATION
 */

/** Flags used with dr_custom_alloc() */
typedef enum {
    /**
     * If this flag is not specified, dr_custom_alloc() uses a managed
     * heap to allocate the memory, just like dr_thread_alloc() or
     * dr_global_alloc().  In that case, it ignores any requested
     * protection bits (\p prot parameter), and the location (\p addr
     * parameter) must be NULL.  If this flag is specified, a
     * page-aligned, separate block of memory is allocated, in a
     * similar fashion to dr_nonheap_alloc().
     */
    DR_ALLOC_NON_HEAP = 0x0001,
    /**
     * This flag only applies to heap memory (i.e., when
     * #DR_ALLOC_NON_HEAP is not specified).  If this flag is not
     * specified, global heap is used (just like dr_global_alloc())
     * and the \p drcontext parameter is ignored.  If it is specified,
     * thread-private heap specific to \p drcontext is used, just like
     * dr_thread_alloc().
     */
    DR_ALLOC_THREAD_PRIVATE = 0x0002,
    /**
     * Allocate memory that is 32-bit-displacement reachable from the
     * code caches and from the client library.  Memory allocated
     * through dr_thread_alloc(), dr_global_alloc(), and
     * dr_nonheap_alloc() is also reachable, but for
     * dr_custom_alloc(), the resulting memory is not reachable unless
     * this flag is specified.  If this flag is passed, the requested
     * location (\p addr parameter) must be NULL.  This flag is not
     * compatible with #DR_ALLOC_LOW_2GB, #DR_ALLOC_FIXED_LOCATION, or
     * #DR_ALLOC_NON_DR.
     */
    DR_ALLOC_CACHE_REACHABLE = 0x0004,
    /**
     * This flag only applies to non-heap memory (i.e., when
     * #DR_ALLOC_NON_HEAP is specified).  The flag requests that
     * memory be allocated at a specific address, given in the \p addr
     * parameter.  Without this flag, the \p addr parameter is not
     * honored.  This flag is not compatible with #DR_ALLOC_LOW_2GB or
     * #DR_ALLOC_CACHE_REACHABLE.
     */
    DR_ALLOC_FIXED_LOCATION = 0x0008,
    /**
     * This flag only applies to non-heap memory (i.e., when
     * #DR_ALLOC_NON_HEAP is specified) in 64-bit mode.  The flag
     * requests that memory be allocated in the low 2GB of the address
     * space.  If this flag is passed, the requested location (\p addr
     * parameter) must be NULL.  This flag is not compatible with
     * #DR_ALLOC_FIXED_LOCATION.
     */
    DR_ALLOC_LOW_2GB = 0x0010,
    /**
     * This flag only applies to non-heap memory (i.e., when
     * #DR_ALLOC_NON_HEAP is specified).  When this flag is specified,
     * the allocated memory is not considered to be DynamoRIO or tool
     * memory and thus is not kept separate from the application.
     * This is similar to dr_raw_mem_alloc().  Use of this memory is
     * at the client's own risk.  This flag is not compatible with
     * #DR_ALLOC_CACHE_REACHABLE.
     */
    DR_ALLOC_NON_DR = 0x0020,
#ifdef WINDOWS
    /**
     * This flag only applies to non-heap, non-DR memory (i.e., when
     * both #DR_ALLOC_NON_HEAP and #DR_ALLOC_NON_DR are specified) on
     * Windows.  When this flag is specified, the allocated memory is
     * reserved but not committed, just like the MEM_RESERVE Windows API
     * flag (the default is MEM_RESERVE|MEM_COMMIT).
     */
    DR_ALLOC_RESERVE_ONLY = 0x0040,
    /**
     * This flag only applies to non-heap, non-DR memory (i.e., when both
     * #DR_ALLOC_NON_HEAP and #DR_ALLOC_NON_DR are specified) on Windows.
     * This flag must be combined with DR_ALLOC_FIXED_LOCATION.  When this
     * flag is specified, previously allocated memory is committed, just
     * like the MEM_COMMIT Windows API flag (when this flag is not passed,
     * the effect is MEM_RESERVE|MEM_COMMIT).  When passed to
     * dr_custom_free(), this flag causes a de-commit, just like the
     * MEM_DECOMMIT Windows API flag.  This flag cannot be combined with
     * #DR_ALLOC_LOW_2GB and must include a non-NULL requested location (\p
     * addr parameter).
     */
    DR_ALLOC_COMMIT_ONLY = 0x0080,
#endif
} dr_alloc_flags_t;


/**
 * Allocates \p size bytes of memory from DR's memory pool specific to the
 * thread associated with \p drcontext.
 * This memory is only guaranteed to be aligned to the pointer size:
 * 8 byte alignment for 64-bit; 4-byte alignment for 32-bit.
 * (The wrapped malloc() guarantees the more standard double-pointer-size.)
 */
void *
dr_thread_alloc(void *drcontext, size_t size);

/**
 * Frees thread-specific memory allocated by dr_thread_alloc().
 * \p size must be the same as that passed to dr_thread_alloc().
 */
void
dr_thread_free(void *drcontext, void *mem, size_t size);

/**
 * Allocates \p size bytes of memory from DR's global memory pool.
 * This memory is only guaranteed to be aligned to the pointer size:
 * 8 byte alignment for 64-bit; 4-byte alignment for 32-bit.
 * (The wrapped malloc() guarantees the more standard double-pointer-size.)
 */
void *
dr_global_alloc(size_t size);

/**
 * Frees memory allocated by dr_global_alloc().
 * \p size must be the same as that passed to dr_global_alloc().
 */
void
dr_global_free(void *mem, size_t size);

/**
 * Allocates memory with the properties requested by \p flags.
 *
 * If \p addr is non-NULL (only allowed with certain flags), it must
 * be page-aligned.
 *
 * To make more space available for the code caches when running
 * larger applications, or for clients that use a lot of heap memory
 * that is not directly referenced from the cache, we recommend that
 * dr_custom_alloc() be called to obtain memory that is not guaranteed
 * to be reachable from the code cache (by not passing
 * #DR_ALLOC_CACHE_REACHABLE).  This frees up space in the reachable
 * region.
 *
 * Returns NULL on failure.
 */
void *
dr_custom_alloc(void *drcontext, dr_alloc_flags_t flags, size_t size, uint prot,
                void *addr);

/**
 * Frees memory allocated by dr_custom_alloc().  The same \p flags
 * and \p size must be passed here as were passed to dr_custom_alloc().
 */
bool
dr_custom_free(void *drcontext, dr_alloc_flags_t flags, void *addr, size_t size);

/**
 * Allocates \p size bytes of memory as a separate allocation from DR's
 * heap, allowing for separate protection.
 * The \p prot protection should use the DR_MEMPROT_READ,
 * DR_MEMPROT_WRITE, and DR_MEMPROT_EXEC bits.
 * When creating a region to hold dynamically generated code, use
 * this routine in order to create executable memory.
 */
void *
dr_nonheap_alloc(size_t size, uint prot);

/**
 * Frees memory allocated by dr_nonheap_alloc().
 * \p size must be the same as that passed to dr_nonheap_alloc().
 */
void
dr_nonheap_free(void *mem, size_t size);

/**
 * \warning This raw memory allocation interface is in flux and is subject to
 * change in the next release.  Consider it experimental in this release.
 *
 * Allocates \p size bytes (page size aligned) of memory as a separate
 * allocation at preferred base \p addr that must be page size aligned,
 * allowing for separate protection.
 * If \p addr is NULL, an arbitrary address is picked.
 *
 * The \p prot protection should use the DR_MEMPROT_READ,
 * DR_MEMPROT_WRITE, and DR_MEMPROT_EXEC bits.
 * The allocated memory is not considered to be DynamoRIO or tool memory and
 * thus is not kept separate from the application. Use of this memory is at the
 * client's own risk.
 *
 * The resulting memory is guaranteed to be initialized to all zeroes.
 *
 * Returns the actual address allocated or NULL if memory allocation at
 * preferred base fails.
 */
void *
dr_raw_mem_alloc(size_t size, uint prot, void *addr);

/**
 * Frees memory allocated by dr_raw_mem_alloc().
 * \p addr and \p size must be the same as that passed to dr_raw_mem_alloc()
 * on Windows.
 */
bool
dr_raw_mem_free(void *addr, size_t size);

/**
 * Calls mremap with the specified parameters and returns the result.
 * The old memory must be non-DR memory, and the new memory is also
 * considered to be non-DR memory (see #DR_ALLOC_NON_DR).
 * \note Linux-only.
 */
void *
dr_raw_mremap(void *old_address, size_t old_size, size_t new_size, int flags,
              void *new_address);

/**
 * Sets the program break to the specified value.  Invokes the SYS_brk
 * system call and returns the result.  This is the application's
 * program break, so use this system call only when deliberately
 * changing the application's behavior.
 * \note Linux-only.
 */
void *
dr_raw_brk(void *new_address);

/**
 * Allocates memory from DR's global memory pool, but mimics the
 * behavior of malloc.  Memory must be freed with __wrap_free().  The
 * __wrap routines are intended to be used with ld's -wrap option to
 * replace a client's use of malloc, realloc, and free with internal
 * versions that allocate memory from DR's private pool.  With -wrap,
 * clients can link to libraries that allocate heap memory without
 * interfering with application allocations.
 * The returned address is guaranteed to be double-pointer-aligned:
 * aligned to 16 bytes for 64-bit; aligned to 8 bytes for 32-bit.
 */
void *
__wrap_malloc(size_t size);

/**
 * Reallocates memory from DR's global memory pool, but mimics the
 * behavior of realloc.  Memory must be freed with __wrap_free().  The
 * __wrap routines are intended to be used with ld's -wrap option; see
 * __wrap_malloc() for more information.
 * The returned address is guaranteed to be double-pointer-aligned:
 * aligned to 16 bytes for 64-bit; aligned to 8 bytes for 32-bit.
 */
void *
__wrap_realloc(void *mem, size_t size);

/**
 * Allocates memory from DR's global memory pool, but mimics the
 * behavior of calloc.  Memory must be freed with __wrap_free().  The
 * __wrap routines are intended to be used with ld's -wrap option; see
 * __wrap_malloc() for more information.
 * The returned address is guaranteed to be double-pointer-aligned:
 * aligned to 16 bytes for 64-bit; aligned to 8 bytes for 32-bit.
 */
void *
__wrap_calloc(size_t nmemb, size_t size);

/**
 * Frees memory from DR's global memory pool.  Memory must have been
 * allocated with __wrap_malloc(). The __wrap routines are intended to
 * be used with ld's -wrap option; see __wrap_malloc() for more
 * information.
 */
void
__wrap_free(void *mem);

/**
 * Allocates memory for a new string identical to 'str' and copies the
 * contents of 'str' into the new string, including a terminating
 * null.  Memory must be freed with __wrap_free().  The __wrap
 * routines are intended to be used with ld's -wrap option; see
 * __wrap_malloc() for more information.
 * The returned address is guaranteed to be double-pointer-aligned:
 * aligned to 16 bytes for 64-bit; aligned to 8 bytes for 32-bit.
 */
char *
__wrap_strdup(const char *str);

/**************************************************
 * MEMORY QUERY/ACCESS ROUTINES
 */

#ifdef DR_PAGE_SIZE_COMPATIBILITY

#    undef PAGE_SIZE
/**
 * Size of a page of memory. This uses a function call so be careful
 * where performance is critical.
 */
#    define PAGE_SIZE dr_page_size()

/**
 * Convenience macro to align to the start of a page of memory.
 * It uses a function call so be careful where performance is critical.
 */
#    define PAGE_START(x) (((ptr_uint_t)(x)) & ~(dr_page_size() - 1))

#endif /* DR_PAGE_SIZE_COMPATIBILITY */



/** Returns the size of a page of memory. */
size_t
dr_page_size(void);

/**
 * Checks to see that all bytes with addresses in the range [\p pc, \p pc + \p size - 1]
 * are readable and that reading from that range won't generate an exception (see also
 * dr_safe_read() and DR_TRY_EXCEPT()).
 * \note Nothing guarantees that the memory will stay readable for any length of time.
 * \note On Linux, especially if the app is in the middle of loading a library
 * and has not properly set up the .bss yet, a page that seems readable can still
 * generate SIGBUS if beyond the end of an mmapped file.  Use dr_safe_read() or
 * DR_TRY_EXCEPT() to avoid such problems.
 */
bool
dr_memory_is_readable(const byte *pc, size_t size);

/**
 * An os neutral method for querying a memory address. Returns true
 * iff a memory region containing \p pc is found.  If found additional
 * information about the memory region is returned in the optional out
 * arguments \p base_pc, \p size, and \p prot where \p base_pc is the
 * start address of the memory region continaing \p pc, \p size is the
 * size of said memory region and \p prot is an ORed combination of
 * DR_MEMPROT_* flags describing its current protection.
 *
 * \note To examine only application memory, skip memory for which
 * dr_memory_is_dr_internal() or dr_memory_is_in_client() returns true.
 *
 * \note DR may mark writable code pages as read-only but pretend they're
 * writable.  When this happens, it will include both #DR_MEMPROT_WRITE
 * and #DR_MEMPROT_PRETEND_WRITE in \p prot.
 */
bool
dr_query_memory(const byte *pc, byte **base_pc, size_t *size, uint *prot);

/**
 * Provides additional information beyond dr_query_memory().
 * Returns true if it was able to obtain information (including about
 * free regions) and sets the fields of \p info.  This routine can be
 * used to iterate over the entire address space.  Such an iteration
 * should stop on reaching the top of the address space, or on
 * reaching kernel memory (look for #DR_MEMTYPE_ERROR_WINKERNEL) on
 * Windows.
 *
 * Returns false on failure and sets info->type to a DR_MEMTYPE_ERROR*
 * code indicating the reason for failure.
 *
 * \note To examine only application memory, skip memory for which
 * dr_memory_is_dr_internal() returns true.
 *
 * \note DR may mark writable code pages as read-only but pretend they're
 * writable.  When this happens, it will include both #DR_MEMPROT_WRITE
 * and #DR_MEMPROT_PRETEND_WRITE in \p info->prot.
 */
bool
dr_query_memory_ex(const byte *pc, OUT dr_mem_info_t *info);
#ifdef WINDOWS


/**
 * Equivalent to the win32 API function VirtualQuery().
 * See that routine for a description of
 * arguments and return values.  \note Windows only.
 *
 * \note DR may mark writable code pages as read-only but pretend they're
 * writable.  When this happens, this routine will indicate that the
 * memory is writable.  Call dr_query_memory() or dr_query_memory_ex()
 * before attempting to write to application memory to ensure it's
 * not read-only underneath.
 */
size_t
dr_virtual_query(const byte *pc, MEMORY_BASIC_INFORMATION *mbi, size_t mbi_size);
#endif


/**
 * Safely reads \p size bytes from address \p base into buffer \p
 * out_buf.  Reading is done without the possibility of an exception
 * occurring.  Returns true if the entire \p size bytes were read;
 * otherwise returns false and if \p bytes_read is non-NULL returns the
 * partial number of bytes read in \p bytes_read.
 * \note See also DR_TRY_EXCEPT().
 */
bool
dr_safe_read(const void *base, size_t size, void *out_buf, size_t *bytes_read);

/**
 * Safely writes \p size bytes from buffer \p in_buf to address \p
 * base.  Writing is done without the possibility of an exception
 * occurring.    Returns true if the entire \p size bytes were written;
 * otherwise returns false and if \p bytes_written is non-NULL returns the
 * partial number of bytes written in \p bytes_written.
 * \note See also DR_TRY_EXCEPT().
 */
bool
dr_safe_write(void *base, size_t size, const void *in_buf, size_t *bytes_written);

/** Do not call this directly: use the DR_TRY_EXCEPT macro instead. */
void
dr_try_setup(void *drcontext, void **try_cxt);

/** Do not call this directly: use the DR_TRY_EXCEPT macro instead. */
int
dr_try_start(void *buf);

/** Do not call this directly: use the DR_TRY_EXCEPT macro instead. */
void
dr_try_stop(void *drcontext, void *try_cxt);
/**
 * Simple try..except support for executing operations that might
 * fault and recovering if they do.  Be careful with this feature
 * as it has some limitations:
 * - do not use a return within a try statement (we do not have
 *   language support)
 * - any automatic variables that you want to use in the except
 *   block should be declared volatile
 * - no locks should be grabbed in a try statement (because
 *   there is no finally support to release them)
 * - nesting is supported, but finally statements are not
 *   supported
 *
 * For fault-free reads in isolation, use dr_safe_read() instead.
 * dr_safe_read() out-performs DR_TRY_EXCEPT.
 *
 * For fault-free writes in isolation, dr_safe_write() can be used,
 * although on Windows it invokes a system call and can be less
 * performant than DR_TRY_EXCEPT.
 */
#define DR_TRY_EXCEPT(drcontext, try_statement, except_statement)  \
        do {                                                           \
            void *try_cxt;                                             \
            dr_try_setup(drcontext, &try_cxt);                         \
            if (dr_try_start(try_cxt) == 0) {                          \
                try_statement dr_try_stop(drcontext, try_cxt);         \
            } else {                                                   \
                /* roll back first in case except faults or returns */ \
                dr_try_stop(drcontext, try_cxt);                       \
                except_statement                                       \
            }                                                          \
        } while (0)


/**
 * Modifies the memory protections of the region from \p start through
 * \p start + \p size.  Modification of memory allocated by DR or of
 * the DR or client libraries themselves is allowed under the
 * assumption that the client knows what it is doing.  Modification of
 * the ntdll.dll library on Windows is not allowed.  Returns true if
 * successful.
 */
bool
dr_memory_protect(void *base, size_t size, uint new_prot);

/**
 * Returns true iff pc is memory allocated by DR for its own
 * purposes, and would not exist if the application were run
 * natively.
 */
bool
dr_memory_is_dr_internal(const byte *pc);

/**
 * Returns true iff pc is located inside a client library, an Extension
 * library used by a client, or an auxiliary client library (see
 * dr_load_aux_library()).
 */
bool
dr_memory_is_in_client(const byte *pc);

/**************************************************
 * CLIENT AUXILIARY LIBRARIES
 */


/**
 * Loads the library with the given path as an auxiliary client
 * library.  The library is not treated as an application module but
 * as an extension of DR.  The library will be included in
 * dr_memory_is_in_client() and any faults in the library will be
 * considered client faults.  The bounds of the loaded library are
 * returned in the optional out variables.  On failure, returns NULL.
 *
 * If only a filename and not a full path is given, this routine will
 * search for the library in the standard search locations for DR's
 * private loader.
 */
dr_auxlib_handle_t
dr_load_aux_library(const char *name, byte **lib_start /*OPTIONAL OUT*/,
                    byte **lib_end /*OPTIONAL OUT*/);

/**
 * Looks up the exported routine with the given name in the given
 * client auxiliary library loaded by dr_load_aux_library().  Returns
 * NULL on failure.
 */
dr_auxlib_routine_ptr_t
dr_lookup_aux_library_routine(dr_auxlib_handle_t lib, const char *name);

/**
 * Unloads the given library, which must have been loaded by
 * dr_load_aux_library().  Returns whether successful.
 */
bool
dr_unload_aux_library(dr_auxlib_handle_t lib);

#if defined(WINDOWS) && !defined(X64)


/**
 * Similar to dr_load_aux_library(), but loads a 64-bit library for
 * access from a 32-bit process running on a 64-bit Windows kernel.
 * Fails if called from a 32-bit kernel or from a 64-bit process.
 * The library will be located in the low part of the address space
 * with 32-bit addresses.
 * Functions in the library can be called with dr_invoke_x64_routine().
 *
 * \warning Invoking 64-bit code is fragile.  Currently, this routine
 * uses the system loader, under the assumption that little isolation
 * is needed versus application 64-bit state.  Consider use of this routine
 * experimental: use at your own risk!
 *
 * \note Windows only.
 *
 * \note Currently this routine does not support loading kernel32.dll
 * or any library that depends on it.
 * It also does not invoke the entry point for any dependent libraries
 * loaded as part of loading \p name.
 *
 * \note Currently this routine does not support Windows 8 or higher.
 */
dr_auxlib64_handle_t
dr_load_aux_x64_library(const char *name);

/**
 * Looks up the exported routine with the given name in the given
 * 64-bit client auxiliary library loaded by dr_load_aux_x64_library().  Returns
 * NULL on failure.
 * The returned function can be called with dr_invoke_x64_routine().
 *
 * \note Windows only.
 *
 * \note Currently this routine does not support Windows 8.
 */
dr_auxlib64_routine_ptr_t
dr_lookup_aux_x64_library_routine(dr_auxlib64_handle_t lib, const char *name);

/**
 * Unloads the given library, which must have been loaded by
 * dr_load_aux_x64_library().  Returns whether successful.
 *
 * \note Windows only.
 */
bool
dr_unload_aux_x64_library(dr_auxlib64_handle_t lib);

/**
 * Must be called from 32-bit mode.  Switches to 64-bit mode, calls \p
 * func64 with the given parameters, switches back to 32-bit mode, and
 * then returns to the caller.  Requires that \p func64 be located in
 * the low 4GB of the address space.  All parameters must be 32-bit
 * sized, and all are widened via sign-extension when passed to \p
 * func64.
 *
 * \return -1 on failure; else returns the return value of \p func64.
 *
 * \warning Invoking 64-bit code is fragile.  The WOW64 layer assumes
 * there is no other 64-bit code that will be executed.
 * dr_invoke_x64_routine() attempts to save the WOW64 state, but it
 * has not been tested in all versions of WOW64.  Also, invoking
 * 64-bit code that makes callbacks is not supported, as not only a
 * custom wrapper to call the 32-bit code in the right mode would be
 * needed, but also a way to restore the WOW64 state in case the
 * 32-bit callback makes a system call.  Consider use of this routine
 * experimental: use at your own risk!
 *
 * \note Windows only.
 */
int64
dr_invoke_x64_routine(dr_auxlib64_routine_ptr_t func64, uint num_params, ...);

#endif /* WINDOWS && !X64 */


/**************************************************
 * LOCK SUPPORT
 */


/**
 * Initializes a mutex.
 *
 * Warning: there are restrictions on when DR-provided mutexes, and
 * locks in general, can be held by a client: no lock should be held
 * while application code is executing in the code cache.  Locks can
 * be used while inside client code reached from clean calls out of
 * the code cache, but they must be released before returning to the
 * cache.  A lock must also be released by the same thread that acquired
 * it.  Failing to follow these restrictions can lead to deadlocks.
 */
void *
dr_mutex_create(void);

/** Deletes \p mutex. */
void
dr_mutex_destroy(void *mutex);

/** Locks \p mutex.  Waits until the mutex is successfully held. */
void
dr_mutex_lock(void *mutex);

/**
 * Unlocks \p mutex.  Asserts that mutex is currently locked by the
 * current thread.
 */
void
dr_mutex_unlock(void *mutex);

/** Tries once to lock \p mutex and returns whether or not successful. */
bool
dr_mutex_trylock(void *mutex);

/**
 * Returns true iff \p mutex is owned by the calling thread.
 * This routine is only available in debug builds.
 * In release builds it always returns true.
 */
bool
dr_mutex_self_owns(void *mutex);

/**
 * Instructs DR to treat this lock as an application lock.  Primarily
 * this avoids debug-build checks that no DR locks are held in situations
 * where locks are disallowed.
 *
 * \warning Any one lock should either be a DR lock or an application lock.
 * Use this routine with caution and do not call it on a DR lock that is
 * used in DR contexts, as it disables debug checks.
 *
 * \warning This routine is not sufficient on its own to prevent deadlocks
 * during scenarios where DR wants to suspend all threads such as detach or
 * relocation. See dr_app_recurlock_lock() and dr_mark_safe_to_suspend().
 *
 * \return whether successful.
 */
bool
dr_mutex_mark_as_app(void *mutex);

/**
 * Creates and initializes a read-write lock.  A read-write lock allows
 * multiple readers or alternatively a single writer.  The lock
 * restrictions for mutexes apply (see dr_mutex_create()).
 */
void *
dr_rwlock_create(void);

/** Deletes \p rwlock. */
void
dr_rwlock_destroy(void *rwlock);

/** Acquires a read lock on \p rwlock. */
void
dr_rwlock_read_lock(void *rwlock);

/** Releases a read lock on \p rwlock. */
void
dr_rwlock_read_unlock(void *rwlock);

/** Acquires a write lock on \p rwlock. */
void
dr_rwlock_write_lock(void *rwlock);

/** Releases a write lock on \p rwlock. */
void
dr_rwlock_write_unlock(void *rwlock);

/** Tries once to acquire a write lock on \p rwlock and returns whether successful. */
bool
dr_rwlock_write_trylock(void *rwlock);

/** Returns whether the calling thread owns the write lock on \p rwlock. */
bool
dr_rwlock_self_owns_write_lock(void *rwlock);

/**
 * Instructs DR to treat this lock as an application lock.  Primarily
 * this avoids debug-build checks that no DR locks are held in situations
 * where locks are disallowed.
 *
 * \warning Any one lock should either be a DR lock or an application lock.
 * Use this routine with caution and do not call it on a DR lock that is
 * used in DR contexts, as it disables debug checks.
 *
 * \return whether successful.
 */
bool
dr_rwlock_mark_as_app(void *rwlock);

/**
 * Creates and initializes a recursive lock.  A recursive lock allows
 * the same thread to acquire it multiple times.  The lock
 * restrictions for mutexes apply (see dr_mutex_create()).
 */
void *
dr_recurlock_create(void);

/** Deletes \p reclock. */
void
dr_recurlock_destroy(void *reclock);

/** Acquires \p reclock, or increments the ownership count if already owned. */
void
dr_recurlock_lock(void *reclock);

/**
 * Acquires \p reclock, or increments the ownership count if already owned.
 * Calls to this method which block (i.e. when the lock is already held) are
 * marked safe to suspend AND transfer; in that case the provided mcontext \p mc
 * will overwrite the current thread's mcontext. \p mc must have a valid PC
 * and its flags must be DR_MC_ALL.
 *
 * This routine must be used in clients holding application locks to prevent
 * deadlocks in a way similar to dr_mark_safe_to_suspend(), but this routine
 * is intended to be called by a clean call and may return execution to the
 * provided mcontext rather than returning normally.
 *
 * If this routine is called from a clean call, callers should not return
 * normally. Instead, dr_redirect_execution() or dr_redirect_native_target()
 * should be called to to prevent a return into a flushed code page.
 */
void
dr_app_recurlock_lock(void *reclock, dr_mcontext_t *mc);

/** Decrements the ownership count of \p reclock and releases if zero. */
void
dr_recurlock_unlock(void *reclock);

/** Tries once to acquire \p reclock and returns whether successful. */
bool
dr_recurlock_trylock(void *reclock);

/** Returns whether the calling thread owns \p reclock. */
bool
dr_recurlock_self_owns(void *reclock);

/**
 * Instructs DR to treat this lock as an application lock.  Primarily
 * this avoids debug-build checks that no DR locks are held in situations
 * where locks are disallowed.
 *
 * \warning Any one lock should either be a DR lock or an application lock.
 * Use this routine with caution and do not call it on a DR lock that is
 * used in DR contexts, as it disables debug checks.
 *
 * \return whether successful.
 */
bool
dr_recurlock_mark_as_app(void *reclock);

/** Creates an event object on which threads can wait and be signaled. */
void *
dr_event_create(void);

/** Destroys an event object. */
bool
dr_event_destroy(void *event);

/** Suspends the current thread until \p event is signaled. */
bool
dr_event_wait(void *event);

/** Wakes up at most one thread waiting on \p event. */
bool
dr_event_signal(void *event);

/** Resets \p event to no longer be in a signaled state. */
bool
dr_event_reset(void *event);

/**
 * Use this function to mark a region of code as safe for DR to suspend
 * the client while inside the region.  DR will not relocate the client
 * from the region and will resume it at precisely the suspend point.
 *
 * This function must be used in client code that acquires application locks.
 * Use this feature with care!  Do not mark code as safe to suspend that has
 * a code cache return point.  I.e., do not call this routine from a clean
 * call. For acquiring application locks from a clean call, see
 * dr_app_recurlock_lock().
 *
 * No DR locks can be held while in a safe region.  Consequently, do
 * not call this routine from any DR event callback.  It may only be used
 * from natively executing code.
 *
 * Always invoke this routine in pairs, with the first passing true
 * for \p enter and the second passing false, thus delimiting the
 * region.
 */
bool
dr_mark_safe_to_suspend(void *drcontext, bool enter);

/**
 * Atomically adds \p val to \p *dest and returns the sum.
 * \p dest must not straddle two cache lines.
 */
int
dr_atomic_add32_return_sum(volatile int *dest, int val);

/**
 * Atomically adds \p val to \p *dest and returns the sum.
 * \p dest must not straddle two cache lines.
 * Currently 64-bit-build only.
 */
int64
dr_atomic_add64_return_sum(volatile int64 *dest, int64 val);

/** Atomically and visibly loads the value at \p src and returns it. */
int
dr_atomic_load32(volatile int *src);

/** Atomically and visibly stores \p val to \p dest. */
void
dr_atomic_store32(volatile int *dest, int val);

/**
 * Atomically and visibly loads the value at \p src and returns it.
 * Currently 64-bit-build only.
 */
int64
dr_atomic_load64(volatile int64 *src);

/**
 * Atomically and visibly stores \p val to \p dest.
 * Currently 64-bit-build only.
 */
void
dr_atomic_store64(volatile int64 *dest, int64 val);
/**************************************************
 * MODULE INFORMATION ROUTINES
 */

/** For dr_module_iterator_* interface */
typedef void *dr_module_iterator_t;

#ifdef UNIX
/** Holds information on a segment of a loaded module. */
typedef struct _module_segment_data_t {
    app_pc start;  /**< Start address of the segment, page-aligned backward. */
    app_pc end;    /**< End address of the segment, page-aligned forward. */
    uint prot;     /**< Protection attributes of the segment */
    uint64 offset; /**< Offset of the segment from the beginning of the backing file */
} module_segment_data_t;
#endif

/**
 * Holds information about a loaded module. \note On Linux the start address can be
 * cast to an Elf32_Ehdr or Elf64_Ehdr. \note On Windows the start address can be cast to
 * an IMAGE_DOS_HEADER for use in finding the IMAGE_NT_HEADER and its OptionalHeader.
 * The OptionalHeader can be used to walk the module sections (among other things).
 * See WINNT.H. \note On MacOS the start address can be cast to mach_header or
 * mach_header_64.
 * \note When accessing any memory inside the module (including header fields)
 * user is responsible for guarding against corruption and the possibility of the module
 * being unmapped.
 */
struct _module_data_t {
    union {
        app_pc start;           /**< starting address of this module */
        module_handle_t handle; /**< module_handle for use with dr_get_proc_address() */
    };                          /* anonymous union of start address and module handle */
    /**
     * Ending address of this module.  If the module is not contiguous
     * (which is common on MacOS, and can happen on Linux), this is the
     * highest address of the module, but there can be gaps in between start
     * and end that are either unmapped or that contain other mappings or
     * libraries.   Use the segments array to examine each mapped region,
     * and use dr_module_contains_addr() as a convenience routine, rather than
     * checking against [start..end).
     */
    app_pc end;

    app_pc entry_point; /**< entry point for this module as specified in the headers */

    uint flags; /**< Reserved, set to 0 */

    module_names_t names; /**< struct containing name(s) for this module; use
                           * dr_module_preferred_name() to get the preferred name for
                           * this module */

    char *full_path; /**< full path to the file backing this module */

#ifdef WINDOWS
    version_number_t file_version;    /**< file version number from .rsrc section */
    version_number_t product_version; /**< product version number from .rsrc section */
    uint checksum;                    /**< module checksum from the PE headers */
    uint timestamp;                   /**< module timestamp from the PE headers */
    /** Module internal size (from PE headers SizeOfImage). */
    size_t module_internal_size;
#else
    bool contiguous;   /**< whether there are no gaps between segments */
    uint num_segments; /**< number of segments */
    /**
     * Array of num_segments entries, one per segment.  The array is sorted
     * by the start address of each segment.
     */
    module_segment_data_t *segments;
    uint timestamp;               /**< Timestamp from ELF Mach-O headers. */
#    ifdef MACOS
    uint current_version;         /**< Current version from Mach-O headers. */
    uint compatibility_version;   /**< Compatibility version from Mach-O headers. */
    byte uuid[16];                /**< UUID from Mach-O headers. */
#    endif
#endif
};



/**
 * Looks up the module containing \p pc.  If a module containing \p pc is found
 * returns a module_data_t describing that module.  Returns NULL if \p pc is
 * outside all known modules, which is the case for most dynamically generated
 * code.  Can be used to obtain a module_handle_t for dr_lookup_module_section()
 * or dr_get_proc_address() via the \p handle field inside module_data_t.
 *
 * \note Returned module_data_t must be freed with dr_free_module_data().
 */
module_data_t *
dr_lookup_module(byte *pc);

/**
 * Looks up the module with name \p name ignoring case.  If an exact name match is found
 * returns a module_data_t describing that module else returns NULL.  User must call
 * dr_free_module_data() on the returned module_data_t once finished. Can be used to
 * obtain a module_handle_t for dr_get_proc_address().
 * \note Returned module_data_t must be freed with dr_free_module_data().
 */
module_data_t *
dr_lookup_module_by_name(const char *name);

/**
 * Looks up module data for the main executable.
 * \note Returned module_data_t must be freed with dr_free_module_data().
 */
module_data_t *
dr_get_main_module(void);

/**
 * Initialize a new module iterator.  The returned module iterator contains a snapshot
 * of the modules loaded at the time it was created.  Use dr_module_iterator_hasnext()
 * and dr_module_iterator_next() to walk the loaded modules.  Call
 * dr_module_iterator_stop() when finished to release the iterator. \note The iterator
 * does not prevent modules from being loaded or unloaded while the iterator is being
 * walked.
 */
dr_module_iterator_t *
dr_module_iterator_start(void);

/**
 * Returns true if there is another loaded module in the iterator.
 */
bool
dr_module_iterator_hasnext(dr_module_iterator_t *mi);

/**
 * Retrieves the module_data_t for the next loaded module in the iterator. User must call
 * dr_free_module_data() on the returned module_data_t once finished.
 * \note Returned module_data_t must be freed with dr_free_module_data().
 */
module_data_t *
dr_module_iterator_next(dr_module_iterator_t *mi);

/**
 * User should call this routine to free the module iterator.
 */
void
dr_module_iterator_stop(dr_module_iterator_t *mi);

/**
 * Makes a copy of \p data.  Copy must be freed with dr_free_module_data().
 * Useful for making persistent copies of module_data_t's received as part of
 * image load and unload event callbacks.
 */
module_data_t *
dr_copy_module_data(const module_data_t *data);

/**
 * Frees a module_data_t returned by dr_module_iterator_next(), dr_lookup_module(),
 * dr_lookup_module_by_name(), or dr_copy_module_data(). \note Should NOT be used with
 * a module_data_t obtained as part of a module load or unload event.
 */
void
dr_free_module_data(module_data_t *data);

/**
 * Returns the preferred name for the module described by \p data from
 * \p data->module_names.
 */
const char *
dr_module_preferred_name(const module_data_t *data);

/**
 * Returns whether \p addr is contained inside any segment of the module \p data.
 * We recommend using this routine rather than checking against the \p start
 * and \p end fields of \p data, as modules are not always contiguous.
 */
bool
dr_module_contains_addr(const module_data_t *data, app_pc addr);
/**
 * Iterator over the list of modules that a given module imports from.  Created
 * by calling dr_module_import_iterator_start() and must be freed by calling
 * dr_module_import_iterator_stop().
 *
 * \note On Windows, delay-loaded DLLs are not included yet.
 *
 * \note ELF does not import directly from other modules.
 */
struct _dr_module_import_iterator_t;
typedef struct _dr_module_import_iterator_t dr_module_import_iterator_t;

/**
 * Descriptor used to iterate the symbols imported from a specific module.
 */
struct _dr_module_import_desc_t;
typedef struct _dr_module_import_desc_t dr_module_import_desc_t;

/**
 * Module import data returned from dr_module_import_iterator_next().
 *
 * String fields point into the importing module image.  Robust clients should
 * use DR_TRY_EXCEPT while inspecting the strings in case the module is
 * partially mapped or the app racily unmaps it.  The iterator routines
 * themselves handle faults by stopping the iteration.
 *
 * \note ELF does not import directly from other modules.
 */
typedef struct _dr_module_import_t {
    /**
     * Specified name of the imported module or API set.
     */
    const char *modname;

    /**
     * Opaque handle that can be passed to dr_symbol_import_iterator_start().
     * Valid until the original module is unmapped.
     */
    dr_module_import_desc_t *module_import_desc;
} dr_module_import_t;


/**
 * Creates a module import iterator.  Iterates over the list of modules that a
 * given module imports from.
 *
 * \note ELF does not import directly from other modules.
 */
dr_module_import_iterator_t *
dr_module_import_iterator_start(module_handle_t handle);

/**
 * Returns true if there is another module import in the iterator.
 *
 * \note ELF does not import directly from other modules.
 */
bool
dr_module_import_iterator_hasnext(dr_module_import_iterator_t *iter);

/**
 * Advances the passed-in iterator and returns the current module import in the
 * iterator.  The pointer returned is only valid until the next call to
 * dr_module_import_iterator_next() or dr_module_import_iterator_stop().
 *
 * \note ELF does not import directly from other modules.
 */
dr_module_import_t *
dr_module_import_iterator_next(dr_module_import_iterator_t *iter);

/**
 * Stops import iteration and frees a module import iterator.
 *
 * \note ELF does not import directly from other modules.
 */
void
dr_module_import_iterator_stop(dr_module_import_iterator_t *iter);
/**
 * Symbol import iterator data type.  Can be created by calling
 * dr_symbol_import_iterator_start() and must be freed by calling
 * dr_symbol_import_iterator_stop().
 */
struct _dr_symbol_import_iterator_t;
typedef struct _dr_symbol_import_iterator_t dr_symbol_import_iterator_t;

/**
 * Symbol import data returned from dr_symbol_import_iterator_next().
 *
 * String fields point into the importing module image.  Robust clients should
 * use DR_TRY_EXCEPT while inspecting the strings in case the module is
 * partially mapped or the app racily unmaps it.
 */
typedef struct _dr_symbol_import_t {
    const char *name;    /**< Name of imported symbol, if available. */
    const char *modname; /**< Preferred name of module (Windows only). */
    bool delay_load;     /**< This import is delay-loaded (Windows only). */
    bool by_ordinal;     /**< Import is by ordinal, not name (Windows only). */
    ptr_uint_t ordinal;  /**< Ordinal value (Windows only). */

} dr_symbol_import_t;


/**
 * Creates an iterator over symbols imported by a module.  If \p from_module is
 * NULL, all imported symbols are yielded, regardless of which module they were
 * imported from.
 *
 * On Windows, from_module is obtained from a \p dr_module_import_t and used to
 * iterate over all of the imports from a specific module.
 *
 * The iterator returned is invalid until after the first call to
 * dr_symbol_import_iterator_next().
 *
 * \note On Windows, symbols imported from delay-loaded DLLs are not included
 * yet.
 */
dr_symbol_import_iterator_t *
dr_symbol_import_iterator_start(module_handle_t handle,
                                dr_module_import_desc_t *from_module);

/**
 * Returns true if there is another imported symbol in the iterator.
 */
bool
dr_symbol_import_iterator_hasnext(dr_symbol_import_iterator_t *iter);

/**
 * Returns the next imported symbol.  The returned pointer is valid until the
 * next call to dr_symbol_import_iterator_next() or
 * dr_symbol_import_iterator_stop().
 */
dr_symbol_import_t *
dr_symbol_import_iterator_next(dr_symbol_import_iterator_t *iter);

/**
 * Stops symbol import iteration and frees the iterator.
 */
void
dr_symbol_import_iterator_stop(dr_symbol_import_iterator_t *iter);
/**
 * Symbol export iterator data type.  Can be created by calling
 * dr_symbol_export_iterator_start() and must be freed by calling
 * dr_symbol_export_iterator_stop().
 */
struct _dr_symbol_export_iterator_t;
typedef struct _dr_symbol_export_iterator_t dr_symbol_export_iterator_t;

/**
 * Symbol export data returned from dr_symbol_export_iterator_next().
 *
 * String fields point into the exporting module image.  Robust clients should
 * use DR_TRY_EXCEPT while inspecting the strings in case the module is
 * partially mapped or the app racily unmaps it.
 *
 * On Windows, the address in \p addr may not be inside the exporting module if
 * it is a forward and has been patched by the loader.  In that case, \p forward
 * will be NULL.
 */
typedef struct _dr_symbol_export_t {
    const char *name;    /**< Name of exported symbol, if available. */
    app_pc addr;         /**< Address of the exported symbol. */
    const char *forward; /**< Forward name, or NULL if not forwarded (Windows only). */
    ptr_uint_t ordinal;  /**< Ordinal value (Windows only). */
    /**
     * Whether an indirect code object (see dr_export_info_t).  (Linux only).
     */
    bool is_indirect_code;
    bool is_code; /**< Whether code as opposed to exported data (Linux only). */

} dr_symbol_export_t;


/**
 * Creates an iterator over symbols exported by a module.
 * The iterator returned is invalid until after the first call to
 * dr_symbol_export_iterator_next().
 *
 * \note To iterate over all symbols in a module and not just those exported,
 * use the \ref page_drsyms.
 */
dr_symbol_export_iterator_t *
dr_symbol_export_iterator_start(module_handle_t handle);

/**
 * Returns true if there is another exported symbol in the iterator.
 */
bool
dr_symbol_export_iterator_hasnext(dr_symbol_export_iterator_t *iter);

/**
 * Returns the next exported symbol.  The returned pointer is valid until the
 * next call to dr_symbol_export_iterator_next() or
 * dr_symbol_export_iterator_stop().
 */
dr_symbol_export_t *
dr_symbol_export_iterator_next(dr_symbol_export_iterator_t *iter);

/**
 * Stops symbol export iteration and frees the iterator.
 */
void
dr_symbol_export_iterator_stop(dr_symbol_export_iterator_t *iter);
#ifdef WINDOWS


/**
 * Returns whether \p pc is within a section within the module in \p section_found and
 * information about that section in \p section_out. \note Not yet available on Linux.
 */
bool
dr_lookup_module_section(module_handle_t lib, byte *pc,
                         IMAGE_SECTION_HEADER *section_out);
#endif /* WINDOWS */


/**
 * Set whether or not the module referred to by \p handle should be
 * instrumented.  If \p should_instrument is false, code from the module will
 * not be passed to the basic block event.  If traces are enabled, code from the
 * module will still reach the trace event.  Must be called from the module load
 * event for the module referred to by \p handle.
 * \return whether successful.
 *
 * \warning Turning off instrumentation for modules breaks clients and
 * extensions, such as drwrap, that expect to see every instruction.
 */
bool
dr_module_set_should_instrument(module_handle_t handle, bool should_instrument);

/**
 * Return whether code from the module should be instrumented, meaning passed
 * to the basic block event.
 */
bool
dr_module_should_instrument(module_handle_t handle);

/**
 * Returns the entry point of the exported function with the given
 * name in the module with the given base.  Returns NULL on failure.
 *
 * On Linux, when we say "exported" we mean present in the dynamic
 * symbol table (.dynsym).  Global functions and variables in an
 * executable (as opposed to a library) are not exported by default.
 * If an executable is built with the \p -rdynamic flag to \p gcc, its
 * global symbols will be present in .dynsym and dr_get_proc_address()
 * will locate them.  Otherwise, the drsyms Extension (see \ref
 * page_drsyms) must be used to locate the symbols.  drsyms searches
 * the debug symbol table (.symtab) in addition to .dynsym.
 *
 * \note On Linux this ignores symbol preemption by other modules and only
 * examines the specified module.
 * \note On Linux, in order to handle indirect code objects, use
 * dr_get_proc_address_ex().
 */
generic_func_t
dr_get_proc_address(module_handle_t lib, const char *name);
/**
 * Data structure used by dr_get_proc_address_ex() to retrieve information
 * about an exported symbol.
 */
typedef struct _dr_export_info_t {
    /**
     * The entry point of the export as an absolute address located
     * within the queried module.  This address is identical to what
     * dr_get_proc_address_ex() returns.
     */
    generic_func_t address;
    /**
     * Relevant for Linux only.  Set to true iff this export is an
     * indirect code object, which is a new ELF extension allowing
     * runtime selection of which implementation to use for an
     * exported symbol.  The address of such an export is a function
     * that takes no arguments and returns the address of the selected
     * implementation.
     */
    bool is_indirect_code;
} dr_export_info_t;


/**
 * Returns information in \p info about the symbol \p name exported
 * by the module \p lib.  Returns false if the symbol is not found.
 * See the information in dr_get_proc_address() about what an
 * "exported" function is on Linux.
 *
 * \note On Linux this ignores symbol preemption by other modules and only
 * examines the specified module.
 */
bool
dr_get_proc_address_ex(module_handle_t lib, const char *name, dr_export_info_t *info OUT,
                       size_t info_len);
/** Flags for use with dr_map_executable_file(). */
typedef enum {
    /**
     * Requests that writable segments are not mapped, to save address space.
     * This may be ignored on some platforms and may only be honored for
     * a writable segment that is at the very end of the loaded module.
     */
    DR_MAPEXE_SKIP_WRITABLE = 0x0002,
} dr_map_executable_flags_t;


/**
 * Loads \p filename as an executable file for examination, rather
 * than for execution.  No entry point, initialization, or constructor
 * code is executed, nor is any thread-local storage or other
 * resources set up.  Returns the size (which may include unmappped
 * gaps) in \p size.  The return value of the function is the base
 * address at which the file is mapped.
 *
 * \note Not currently supported on Mac OSX.
 */
byte *
dr_map_executable_file(const char *filename, dr_map_executable_flags_t flags,
                       size_t *size OUT);

/**
 * Unmaps a file loaded by dr_map_executable_file().
 */
bool
dr_unmap_executable_file(byte *base, size_t size);
/**************************************************
 * SYSTEM CALL PROCESSING ROUTINES
 */

/**
 * Data structure used to obtain or modify the result of an application
 * system call by dr_syscall_get_result_ex() and dr_syscall_set_result_ex().
 */
typedef struct _dr_syscall_result_info_t {
    /** The caller should set this to the size of the structure. */
    size_t size;
    /**
     * Indicates whether the system call succeeded or failed.  For
     * dr_syscall_set_result_ex(), this requests that DR set any
     * additional machine state, if any, used by the particular
     * plaform that is not part of \p value to indicate success or
     * failure (e.g., on MacOS the carry flag is used to indicate
     * success).
     *
     * For Windows, the success result from dr_syscall_get_result_ex()
     * should only be relied upon for ntoskrnl system calls.  For
     * other Windows system calls (such as win32k.sys graphical
     * (NtGdi) or user (NtUser) system calls), computing success
     * depends on each particular call semantics and is beyond the
     * scope of this routine (consider using the "drsyscall" Extension
     * instead).
     *
     * For Mach syscalls on MacOS, the success result from
     * dr_syscall_get_result_ex() should not be relied upon.
     * Computing success depends on each particular call semantics and
     * is beyond the scope of this routine (consider using the
     * "drsyscall" Extension instead).
     */
    bool succeeded;
    /**
     * The raw main value returned by the system call.
     * See also the \p high field.
     */
    reg_t value;
    /**
     * On some platforms (such as MacOS), a 32-bit application's
     * system call can return a 64-bit value.  For such calls,
     * this field will hold the top 32 bit bits, if requested
     * by \p use_high.  It is up to the caller to know which
     * system calls have 64-bit return values.  System calls that
     * return only 32-bit values do not clear the upper bits.
     * Consider using the "drsyscall" Extension in order to obtain
     * per-system-call semantic information, including return type.
     */
    reg_t high;
    /**
     * This should be set by the caller, and only applies to 32-bit
     * system calls.  For dr_syscall_get_result_ex(), this requests
     * that DR fill in the \p high field.  For
     * dr_syscall_set_result_ex(), this requests that DR set the high
     * 32 bits of the application-facing result to the value in the \p
     * high field.
     */
    bool use_high;
    /**
     * This should be set by the caller.  For dr_syscall_get_result_ex(),
     * this requests that DR fill in the \p errno_value field.
     * For dr_syscall_set_result_ex(), this requests that DR set the
     * \p value to indicate the particular error code in \p errno_value.
     */
    bool use_errno;
    /**
     * If requested by \p use_errno, if a system call fails (i.e., \p
     * succeeded is false) dr_syscall_get_result_ex() will set this
     * field to the absolute value of the error code returned (i.e.,
     * on Linux, it will be inverted from what the kernel directly
     * returns, in order to facilitate cross-platform clients that
     * operate on both Linux and MacOS).  For Linux and Macos, when
     * \p succeeded is true, \p errno_value is set to 0.
     *
     * If \p use_errno is set for dr_syscall_set_result_ex(), then
     * this value will be stored as the system call's return value,
     * negated if necessary for the underlying platform.  In that
     * case, \p value will be ignored.
     */
    uint errno_value;
} dr_syscall_result_info_t;


/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event())
 * event.  Returns the value of system call parameter number \p param_num.
 *
 * It is up to the caller to ensure that reading this parameter is
 * safe: this routine does not know the number of parameters for each
 * system call, nor does it check whether this might read off the base
 * of the stack.
 *
 * \note On some platforms, notably MacOS, a 32-bit application's
 * system call can still take a 64-bit parameter (typically on the
 * stack).  In that situation, this routine will consider the 64-bit
 * parameter to be split into high and low parts, each with its own
 * parameter number.
 */
reg_t
dr_syscall_get_param(void *drcontext, int param_num);

/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event())
 * event, or from a post-syscall (dr_register_post_syscall_event())
 * event when also using dr_syscall_invoke_another().  Sets the value
 * of system call parameter number \p param_num to \p new_value.
 *
 * It is up to the caller to ensure that writing this parameter is
 * safe: this routine does not know the number of parameters for each
 * system call, nor does it check whether this might write beyond the
 * base of the stack.
 *
 * \note On some platforms, notably MacOS, a 32-bit application's
 * system call can still take a 64-bit parameter (typically on the
 * stack).  In that situation, this routine will consider the 64-bit
 * parameter to be split into high and low parts, each with its own
 * parameter number.
 */
void
dr_syscall_set_param(void *drcontext, int param_num, reg_t new_value);

/**
 * Usable only from a post-syscall (dr_register_post_syscall_event())
 * event.  Returns the return value of the system call that will be
 * presented to the application.
 *
 * \note On some platforms (such as MacOS), a 32-bit application's
 * system call can return a 64-bit value.  Use dr_syscall_get_result_ex()
 * to obtain the upper bits in that case.
 *
 * \note On some platforms (such as MacOS), whether a system call
 * succeeded or failed cannot be determined from the main result
 * value.  Use dr_syscall_get_result_ex() to obtain the success result
 * in such cases.
 */
reg_t
dr_syscall_get_result(void *drcontext);

/**
 * Usable only from a post-syscall (dr_register_post_syscall_event())
 * event.  Returns whether it successfully retrieved the results
 * of the system call into \p info.
 *
 * The caller should set the \p size, \p use_high, and \p use_errno fields
 * of \p info prior to calling this routine.
 * See the fields of #dr_syscall_result_info_t for details.
 */
bool
dr_syscall_get_result_ex(void *drcontext, dr_syscall_result_info_t *info INOUT);

/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event()) or
 * post-syscall (dr_register_post_syscall_event()) event.
 * For pre-syscall, should only be used when skipping the system call.
 * This sets the return value of the system call that the application sees
 * to \p value.
 *
 * \note On MacOS, do not use this function as it fails to set the
 * carry flag and thus fails to properly indicate whether the system
 * call succeeded or failed: use dr_syscall_set_result_ex() instead.
 */
void
dr_syscall_set_result(void *drcontext, reg_t value);

/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event()) or
 * post-syscall (dr_register_post_syscall_event()) event.
 * For pre-syscall, should only be used when skipping the system call.
 *
 * This sets the returned results of the system call as specified in
 * \p info.  Returns whether it successfully did so.
 * See the fields of #dr_syscall_result_info_t for details.
 */
bool
dr_syscall_set_result_ex(void *drcontext, dr_syscall_result_info_t *info);

/**
 * Usable only from a pre-syscall (dr_register_pre_syscall_event())
 * event, or from a post-syscall (dr_register_post_syscall_event())
 * event when also using dr_syscall_invoke_another().  Sets the system
 * call number of the system call about to be invoked to \p new_num.
 */
void
dr_syscall_set_sysnum(void *drcontext, int new_num);

/**
 * Usable only from a post-syscall (dr_register_post_syscall_event())
 * event.  An additional system call will be invoked immediately,
 * using the current values of the parameters, which can be set with
 * dr_syscall_set_param().  The system call to be invoked should be
 * specified with dr_syscall_set_sysnum().
 *
 * Use this routine with caution.  Especially on Windows, care must be
 * taken if the application is expected to continue afterward.  When
 * system call parameters are stored on the stack, modifying them can
 * result in incorrect application behavior, particularly when setting
 * more parameters than were present in the original system call,
 * which will result in corruption of the application stack.
 *
 * On Windows, when the first system call is interruptible
 * (alertable), the additional system call may be delayed.
 *
 * DR will set key registers such as r10 for 64-bit or xdx for
 * sysenter or WOW64 system calls.  However, DR will not set ecx for
 * WOW64; that is up to the client.
 */
void
dr_syscall_invoke_another(void *drcontext);

/**
 * Must be invoked from dr_client_main().  Requests that the named ntoskrnl
 * system call be intercepted even when threads are native (e.g., due
 * to #DR_EMIT_GO_NATIVE).  Only a limited number of system calls
 * being intercepted while native are supported.  This routine will
 * fail once that limit is reached.
 *
 * @param[in] name      The system call name.  The name must match an exported
 *   system call wrapper in \p ntdll.dll.
 * @param[in] sysnum    The system call number (the value placed in the eax register).
 * @param[in] num_args  The number of arguments to the system call.
 * @param[in] wow64_index  The value placed in the ecx register when this system
 *   call is executed in a WOW64 process.  This value should be obtainable
 *   by examining the system call wrapper.
 *
 * \note Windows only.
 */
bool
dr_syscall_intercept_natively(const char *name, int sysnum, int num_args,
                              int wow64_index);
/**************************************************
 * PLATFORM-INDEPENDENT FILE SUPPORT
 *
 * Since a FILE cannot be used outside of the DLL it was created in,
 * we have to use HANDLE on Windows.
 * We hide the distinction behind the file_t type
 */


/**
 * Creates a new directory.  Fails if the directory already exists
 * or if it can't be created.
 * Relative path support on Windows is identical to that described in dr_open_file().
 */
bool
dr_create_dir(const char *fname);

/**
 * Deletes the given directory.  Fails if the directory is not empty.
 * Relative path support on Windows is identical to that described in dr_open_file().
 */
bool
dr_delete_dir(const char *fname);

/**
 * Returns the current directory for this process in \p buf.
 * On Windows, reading the current directory is considered unsafe
 * except during initialization, as it is stored in user memory and
 * access is not controlled via any standard synchronization.
 */
bool
dr_get_current_directory(char *buf, size_t bufsz);

/**
 * Checks for the existence of a directory.
 * Relative path support on Windows is identical to that described in dr_open_file().
 */
bool
dr_directory_exists(const char *fname);

/**
 * Checks the existence of a file.
 * Relative path support on Windows is identical to that described in dr_open_file().
 */
bool
dr_file_exists(const char *fname);

/* flags for use with dr_open_file() */
/** Open with read access. */
#define DR_FILE_READ 0x1
/** Open with write access, but do not open if the file already exists. */
#define DR_FILE_WRITE_REQUIRE_NEW 0x2
/**
 * Open with write access.  If the file already exists, set the file position to the
 * end of the file.
 */
#define DR_FILE_WRITE_APPEND 0x4
/**
 * Open with write access.  If the file already exists, truncate the
 * file to zero length.
 */
#define DR_FILE_WRITE_OVERWRITE 0x8
/**
 * Open with large (>2GB) file support.  Only applicable on 32-bit Linux.
 * \note DR's log files and tracedump files are all created with this flag.
 */
#define DR_FILE_ALLOW_LARGE 0x10
/** Linux-only.  This file will be closed in the child of a fork. */
#define DR_FILE_CLOSE_ON_FORK 0x20
/**
 * Open with write-only access.  Meant for use with pipes.  Linux-only.
 * Mutually exclusive with DR_FILE_WRITE_REQUIRE_NEW, DR_FILE_WRITE_APPEND, and
 * DR_FILE_WRITE_OVERWRITE.
 */
#define DR_FILE_WRITE_ONLY 0x40


/**
 * Opens the file \p fname. If no such file exists then one is created.
 * The file access mode is set by the \p mode_flags argument which is drawn from
 * the DR_FILE_* defines ORed together.  Returns INVALID_FILE if unsuccessful.
 *
 * On Windows, \p fname is safest as an absolute path (when using Windows system
 * calls directly there is no such thing as a relative path).  A relative path
 * passed to this routine will be converted to absolute on a best-effort basis
 * using the current directory that was set at process initialization time.
 * (The most recently set current directory can be retrieved (albeit with no
 * safety guarantees) with dr_get_current_directory().)  Drive-implied-absolute
 * paths ("\foo.txt") and other-drive-relative paths ("c:foo.txt") are not
 * supported.
 *
 * On Linux, the file descriptor will be marked as close-on-exec.  The
 * DR_FILE_CLOSE_ON_FORK flag can be used to automatically close a
 * file on a fork.
 *
 * \note No more then one write mode flag can be specified.
 *
 * \note On Linux, DR hides files opened by clients from the
 * application by using file descriptors that are separate from the
 * application's and preventing the application from closing
 * client-opened files.
 */
file_t
dr_open_file(const char *fname, uint mode_flags);

/** Closes file \p f. */
void
dr_close_file(file_t f);

/**
 * Renames the file \p src to \p dst, replacing an existing file named \p dst if
 * \p replace is true.
 * Atomic if \p src and \p dst are on the same filesystem.
 * Returns true if successful.
 */
bool
dr_rename_file(const char *src, const char *dst, bool replace);

/**
 * Deletes the file referred to by \p filename.
 * Returns true if successful.
 * On both Linux and Windows, if filename refers to a symlink, the symlink will
 * be deleted and not the target of the symlink.
 * On Windows, this will fail to delete any file that was not opened with
 * FILE_SHARE_DELETE and is still open.
 * Relative path support on Windows is identical to that described in dr_open_file().
 */
bool
dr_delete_file(const char *filename);

/** Flushes any buffers for file \p f. */
void
dr_flush_file(file_t f);

/**
 * Writes \p count bytes from \p buf to file \p f.
 * Returns the actual number written.
 */
ssize_t
dr_write_file(file_t f, const void *buf, size_t count);

/**
 * Reads up to \p count bytes from file \p f into \p buf.
 * Returns the actual number read.
 */
ssize_t
dr_read_file(file_t f, void *buf, size_t count);

/* For use with dr_file_seek(), specifies the origin at which to apply the offset. */
#define DR_SEEK_SET 0 /**< start of file */
#define DR_SEEK_CUR 1 /**< current file position */
#define DR_SEEK_END 2 /**< end of file */


/**
 * Sets the current file position for file \p f to \p offset bytes
 * from the specified origin, where \p origin is one of the DR_SEEK_*
 * values.  Returns true if successful.
 */
bool
dr_file_seek(file_t f, int64 offset, int origin);

/**
 * Returns the current position for the file \p f in bytes from the start of the file.
 * Returns -1 on an error.
 */
int64
dr_file_tell(file_t f);

/**
 * Returns a new copy of the file handle \p f.
 * Returns INVALID_FILE on error.
 */
file_t
dr_dup_file_handle(file_t f);

/**
 * Determines the size of the file \p fd.
 * On success, returns the size in \p size.
 * \return whether successful.
 */
bool
dr_file_size(file_t fd, OUT uint64 *size);

/* flags for use with dr_map_file() */
enum {
    /**
     * If set, changes to mapped memory are private to the mapping process and
     * are not reflected in the underlying file.  If not set, changes are visible
     * to other processes that map the same file, and will be propagated
     * to the file itself.
     */
    DR_MAP_PRIVATE = 0x0001,
#ifdef UNIX
    /**
     * If set, indicates that the passed-in start address is required rather than a
     * hint.  On Linux, this has the same semantics as mmap with MAP_FIXED: i.e.,
     * any existing mapping in [addr,addr+size) will be unmapped.  This flag is not
     * supported on Windows.
     */
    DR_MAP_FIXED = 0x0002,
#endif
#ifdef WINDOWS
    /**
     * If set, loads the specified file as an executable image, rather than a data
     * file.  This flag is not supported on Linux.
     */
    DR_MAP_IMAGE = 0x0004,
#endif
    /**
     * If set, loads the specified file at a location that is reachable from
     * the code cache and client libraries by a 32-bit displacement.  If not
     * set, the mapped file is not guaranteed to be reachable from the cache.
     */
    DR_MAP_CACHE_REACHABLE = 0x0008,
};


/**
 * Memory-maps \p size bytes starting at offset \p offs from the file \p f
 * at address \p addr with privileges \p prot.
 *
 * @param[in] f The file to map.
 * @param[in,out] size The requested size to map.  Upon successful return,
 *   contains the actual mapped size.
 * @param[in] offs The offset within the file at which to start the map.
 * @param[in] addr The requested start address of the map.  Unless \p fixed
 *   is true, this is just a hint and may not be honored.
 * @param[in] prot The access privileges of the mapping, composed of
 *   the DR_MEMPROT_READ, DR_MEMPROT_WRITE, and DR_MEMPROT_EXEC bits.
 * @param[in] flags Optional DR_MAP_* flags.
 *
 * \note Mapping image files for execution is not supported.
 *
 * \return the start address of the mapping, or NULL if unsuccessful.
 */
void *
dr_map_file(file_t f, INOUT size_t *size, uint64 offs, app_pc addr, uint prot,
            uint flags);

/**
 * Unmaps a portion of a file mapping previously created by dr_map_file().
 * \return whether successful.
 *
 * @param[in]  map   The base address to be unmapped. Must be page size aligned.
 * @param[in]  size  The size to be unmapped.
 *                   All pages overlapping with the range are unmapped.
 *
 * \note On Windows, the whole file will be unmapped instead.
 *
 */
bool
dr_unmap_file(void *map, size_t size);

/**************************************************
 * PRINTING
 */


/**
 * Writes to DR's log file for the thread with drcontext \p drcontext if the current
 * loglevel is >= \p level and the current \p logmask & \p mask != 0.
 * The mask constants are the DR_LOG_* defines below.
 * Logging is disabled for the release build.
 * If \p drcontext is NULL, writes to the main log file.
 */
void
dr_log(void *drcontext, uint mask, uint level, const char *fmt, ...);

/* The log mask constants */
#define DR_LOG_NONE 0x00000000     /**< Log no data. */
#define DR_LOG_STATS 0x00000001    /**< Log per-thread and global statistics. */
#define DR_LOG_TOP 0x00000002      /**< Log top-level information. */
#define DR_LOG_THREADS 0x00000004  /**< Log data related to threads. */
#define DR_LOG_SYSCALLS 0x00000008 /**< Log data related to system calls. */
#define DR_LOG_ASYNCH 0x00000010   /**< Log data related to signals/callbacks/etc. */
#define DR_LOG_INTERP 0x00000020   /**< Log data related to app interpretation. */
#define DR_LOG_EMIT 0x00000040     /**< Log data related to emitting code. */
#define DR_LOG_LINKS 0x00000080    /**< Log data related to linking code. */
#define DR_LOG_CACHE                                           \
        0x00000100 /**< Log data related to code cache management. \
                    */
#define DR_LOG_FRAGMENT                                     \
        0x00000200 /**< Log data related to app code fragments. \
                    */
#define DR_LOG_DISPATCH                                                           \
        0x00000400                    /**< Log data on every context switch dispatch. \
                                       */
#define DR_LOG_MONITOR 0x00000800 /**< Log data related to trace building. */
#define DR_LOG_HEAP 0x00001000    /**< Log data related to memory management. */
#define DR_LOG_VMAREAS 0x00002000 /**< Log data related to address space regions. */
#define DR_LOG_SYNCH 0x00004000   /**< Log data related to synchronization. */
#define DR_LOG_MEMSTATS                                                            \
        0x00008000                         /**< Log data related to memory statistics. \
                                            */
#define DR_LOG_OPTS 0x00010000         /**< Log data related to optimizations. */
#define DR_LOG_SIDELINE 0x00020000     /**< Log data related to sideline threads. */
#define DR_LOG_SYMBOLS 0x00040000      /**< Log data related to app symbols. */
#define DR_LOG_RCT 0x00080000          /**< Log data related to indirect transfers. */
#define DR_LOG_NT 0x00100000           /**< Log data related to Windows Native API. */
#define DR_LOG_HOT_PATCHING 0x00200000 /**< Log data related to hot patching. */
#define DR_LOG_HTABLE 0x00400000       /**< Log data related to hash tables. */
#define DR_LOG_MODULEDB 0x00800000 /**< Log data related to the module database. */
#define DR_LOG_ALL 0x00ffffff      /**< Log all data. */
#ifdef DR_LOG_DEFINE_COMPATIBILITY
#    define LOG_NONE DR_LOG_NONE         /**< Identical to #DR_LOG_NONE. */
#    define LOG_STATS DR_LOG_STATS       /**< Identical to #DR_LOG_STATS. */
#    define LOG_TOP DR_LOG_TOP           /**< Identical to #DR_LOG_TOP. */
#    define LOG_THREADS DR_LOG_THREADS   /**< Identical to #DR_LOG_THREADS. */
#    define LOG_SYSCALLS DR_LOG_SYSCALLS /**< Identical to #DR_LOG_SYSCALLS. */
#    define LOG_ASYNCH DR_LOG_ASYNCH     /**< Identical to #DR_LOG_ASYNCH. */
#    define LOG_INTERP DR_LOG_INTERP     /**< Identical to #DR_LOG_INTERP. */
#    define LOG_EMIT DR_LOG_EMIT         /**< Identical to #DR_LOG_EMIT. */
#    define LOG_LINKS DR_LOG_LINKS       /**< Identical to #DR_LOG_LINKS. */
#    define LOG_CACHE DR_LOG_CACHE       /**< Identical to #DR_LOG_CACHE. */
#    define LOG_FRAGMENT DR_LOG_FRAGMENT /**< Identical to #DR_LOG_FRAGMENT. */
#    define LOG_DISPATCH DR_LOG_DISPATCH /**< Identical to #DR_LOG_DISPATCH. */
#    define LOG_MONITOR DR_LOG_MONITOR   /**< Identical to #DR_LOG_MONITOR. */
#    define LOG_HEAP DR_LOG_HEAP         /**< Identical to #DR_LOG_HEAP. */
#    define LOG_VMAREAS DR_LOG_VMAREAS   /**< Identical to #DR_LOG_VMAREAS. */
#    define LOG_SYNCH DR_LOG_SYNCH       /**< Identical to #DR_LOG_SYNCH. */
#    define LOG_MEMSTATS DR_LOG_MEMSTATS /**< Identical to #DR_LOG_MEMSTATS. */
#    define LOG_OPTS DR_LOG_OPTS         /**< Identical to #DR_LOG_OPTS. */
#    define LOG_SIDELINE DR_LOG_SIDELINE /**< Identical to #DR_LOG_SIDELINE. */
#    define LOG_SYMBOLS DR_LOG_SYMBOLS   /**< Identical to #DR_LOG_SYMBOLS. */
#    define LOG_RCT DR_LOG_RCT           /**< Identical to #DR_LOG_RCT. */
#    define LOG_NT DR_LOG_NT             /**< Identical to #DR_LOG_NT. */
#    define LOG_HOT_PATCHING \
            DR_LOG_HOT_PATCHING              /**< Identical to #DR_LOG_HOT_PATCHING. */
#    define LOG_HTABLE DR_LOG_HTABLE     /**< Identical to #DR_LOG_HTABLE. */
#    define LOG_MODULEDB DR_LOG_MODULEDB /**< Identical to #DR_LOG_MODULEDB. */
#    define LOG_ALL DR_LOG_ALL           /**< Identical to #DR_LOG_ALL. */
#endif



/**
 * Returns the log file for the thread with drcontext \p drcontext.
 * If \p drcontext is NULL, returns the main log file.
 */
file_t
dr_get_logfile(void *drcontext);

/**
 * Returns true iff the -stderr_mask runtime option is non-zero, indicating
 * that the user wants notification messages printed to stderr.
 */
bool
dr_is_notify_on(void);

/** Returns a handle to stdout. */
file_t
dr_get_stdout_file(void);

/** Returns a handle to stderr. */
file_t
dr_get_stderr_file(void);

/** Returns a handle to stdin. */
file_t
dr_get_stdin_file(void);
#ifdef WINDOWS


/**
 * Displays a message in a pop-up window. \note Windows only. \note On Windows Vista
 * most Windows services are unable to display message boxes.
 */
void
dr_messagebox(const char *fmt, ...);
#endif


/**
 * Stdout printing that won't interfere with the
 * application's own printing.
 * It is not buffered, which means that it should not be used for
 * very frequent, small print amounts: for that the client should either
 * do its own buffering or it should use printf from the C library
 * via DR's private loader.
 * \note On Windows 7 and earlier, this routine is not able to print
 * to the \p cmd window
 * unless dr_enable_console_printing() is called ahead of time, and
 * even then there are limitations: see dr_enable_console_printing().
 * \note This routine supports printing wide characters via the ls
 * or S format specifiers.  On Windows, they are assumed to be UTF-16,
 * and are converted to UTF-8.  On Linux, they are converted by simply
 * dropping the high-order bytes.
 * \note If the data to be printed is large it will be truncated to
 * an internal buffer size.  Use dr_snprintf() and dr_write_file() for
 * large output.
 * \note When printing floating-point values, the caller's code should
 * use proc_save_fpstate() or be inside a clean call that
 * has requested to preserve the floating-point state.
 */
void
dr_printf(const char *fmt, ...);

/**
 * Printing to a file that won't interfere with the
 * application's own printing.
 * It is not buffered, which means that it should not be used for
 * very frequent, small print amounts: for that the client should either
 * do its own buffering or it should use printf from the C library
 * via DR's private loader.
 * \note On Windows 7 and earlier, this routine is not able to print to STDOUT or
 * STDERR in the \p cmd window unless dr_enable_console_printing() is
 * called ahead of time, and even then there are limitations: see
 * dr_enable_console_printing().
 * \note This routine supports printing wide characters via the ls
 * or S format specifiers.  On Windows, they are assumed to be UTF-16,
 * and are converted to UTF-8.  On Linux, they are converted by simply
 * dropping the high-order bytes.
 * \note If the data to be printed is large it will be truncated to
 * an internal buffer size.  Use dr_snprintf() and dr_write_file() for
 * large output.
 * \note On Linux this routine does not check for errors like EINTR.  Use
 * dr_write_file() if that is a concern.
 * \note When printing floating-point values, the caller's code should
 * use proc_save_fpstate() or be inside a clean call that
 * has requested to preserve the floating-point state.
 * On success, the number of bytes written is returned.
 * On error, -1 is returned.
 */
ssize_t
dr_fprintf(file_t f, const char *fmt, ...);

/**
 * Identical to dr_fprintf() but exposes va_list.
 */
ssize_t
dr_vfprintf(file_t f, const char *fmt, va_list ap);

/**
 * Enables dr_printf() and dr_fprintf() to work with a legacy console
 * window (viz., \p cmd on Windows 7 or earlier).  Loads a private
 * copy of kernel32.dll (if not
 * already loaded) in order to accomplish this.  To keep the default
 * DR lean and mean, loading kernel32.dll is not performed by default.
 *
 * This routine must be called during client initialization (\p dr_client_main()).
 * If called later, it will fail.
 *
 * Without calling this routine, dr_printf() and dr_fprintf() will not
 * print anything in a console window on Windows 7 or earlier, nor will they
 * print anything when running a graphical application.
 *
 * Even after calling this routine, there are significant limitations
 * to console printing support in DR:
 *
 *  - On Windows versions prior to Vista, and for WOW64 applications
 *    on Vista, it does not work from the exit event.  Once the
 *    application terminates its state with csrss (toward the very end
 *    of ExitProcess), no output will show up on the console.  We have
 *    no good solution here yet as exiting early is not ideal.
 *  - In the future, with earliest injection (Issue 234), writing to the
 *    console may not work from the client init event on Windows 7 and
 *    earlier (it will work on Windows 8).
 *
 * These limitations stem from the complex arrangement of the console
 * window in Windows (prior to Windows 8), where printing to it
 * involves sending a message
 * in an undocumented format to the \p csrss process, rather than a
 * simple write to a file handle.  We recommend using a terminal
 * window such as cygwin's \p rxvt rather than the \p cmd window, or
 * alternatively redirecting all output to a file, which will solve
 * all of the above limitations.
 *
 * Returns whether successful.
 * \note Windows only.
 */
bool
dr_enable_console_printing(void);

/**
 * Returns true if the current standard error handle belongs to a
 * legacy console window (viz., \p cmd on Windows 7 or earlier).  DR's
 * dr_printf() and dr_fprintf()
 * do not work with such console windows unless
 * dr_enable_console_printing() is called ahead of time, and even then
 * there are limitations detailed in dr_enable_console_printing().
 * This routine may result in loading a private copy of kernel32.dll.
 * \note Windows only.
 */
bool
dr_using_console(void);

/**
 * Utility routine to print a formatted message to a string.  Will not
 * print more than max characters.  If successful, returns the number
 * of characters printed, not including the terminating null
 * character.  If the number of characters to write equals max, then
 * the caller is responsible for supplying a terminating null
 * character.  If the number of characters to write exceeds max, then
 * max characters are written and -1 is returned.  If an error
 * occurs, a negative value is returned.
 * \note This routine supports printing wide characters via the ls
 * or S format specifiers.  On Windows, they are assumed to be UTF-16,
 * and are converted to UTF-8.  On Linux, they are converted by simply
 * dropping the high-order bytes.
 * \note When printing floating-point values, the caller's code should
 * use proc_save_fpstate() or be inside a clean call that
 * has requested to preserve the floating-point state.
 */
int
dr_snprintf(char *buf, size_t max, const char *fmt, ...);

/**
 * Wide character version of dr_snprintf().  All of the comments for
 * dr_snprintf() apply, except for the hs or S format specifiers.
 * On Windows, these will assume that the input is UTF-8, and will
 * convert to UTF-16.  On Linux, they will widen a single-byte
 * character string into a wchar_t character string with zero as the
 * high-order bytes.
 */
int
dr_snwprintf(wchar_t *buf, size_t max, const wchar_t *fmt, ...);

/**
 * Identical to dr_snprintf() but exposes va_list.
 */
int
dr_vsnprintf(char *buf, size_t max, const char *fmt, va_list ap);

/**
 * Identical to dr_snwprintf() but exposes va_list.
 */
int
dr_vsnwprintf(wchar_t *buf, size_t max, const wchar_t *fmt, va_list ap);

/**
 * Utility routine to parse strings that match a pre-defined format string,
 * similar to the sscanf() C routine.
 *
 * @param[in] str   String to parse.
 * @param[in] fmt   Format string controlling parsing.
 * @param[out] ...  All remaining parameters interpreted as output parameter
 *                  pointers.  The type of each parameter must match the type
 *                  implied by the corresponding format specifier in \p fmt.
 * \return The number of specifiers matched.
 *
 * The benefit of using dr_sscanf() over native sscanf() is that DR's
 * implementation is standalone, signal-safe, and cross-platform.  On Linux,
 * sscanf() has been observed to call malloc().  On Windows, sscanf() will call
 * strlen(), which can break when using mapped files.
 *
 * The behavior of dr_sscanf() is mostly identical to that of the sscanf() C
 * routine.
 *
 * Supported format specifiers:
 * - \%s: Matches a sequence of non-whitespace characters.  The string is copied
 *   into the provided output buffer.  To avoid buffer overflow, the caller
 *   should use a width specifier.
 * - \%c: Matches any single character.
 * - \%d: Matches a signed decimal integer.
 * - \%u: Matches an unsigned decimal integer.
 * - \%x: Matches an unsigned hexadecimal integer, with or without a leading 0x.
 * - \%p: Matches a pointer-sized hexadecimal integer as %x does.
 * - \%%: Matches a literal % character.  Does not store output.
 *
 * Supported format modifiers:
 * - *: The * modifier causes the scan to match the specifier, but not store any
 *   output.  No output parameter is consumed for this specifier, and one should
 *   not be passed.
 * - 0-9: A decimal integer preceding the specifier gives the width to match.
 *   For strings, this indicates the maximum number of characters to copy.  For
 *   integers, this indicates the maximum number of digits to parse.
 * - h: Marks an integer specifier as short.
 * - l: Marks an integer specifier as long.
 * - ll: Marks an integer specifier as long long.  Use this for 64-bit integers.
 *
 * \warning dr_sscanf() does \em not support parsing floating point numbers yet.
 */
int
dr_sscanf(const char *str, const char *fmt, ...);

/**
 * Utility function that aids in tokenizing a string, such as a client
 * options string from dr_get_options().  The function scans \p str
 * until a non-whitespace character is found.  It then starts copying
 * into \p buf until a whitespace character is found denoting the end
 * of the token.  If the token begins with a quote, the token
 * continues (including across whitespace) until the matching end
 * quote is found.  Characters considered whitespace are ' ', '\\t',
 * '\\r', and '\\n'; characters considered quotes are '\\'', '\\"', and
 * '`'.
 *
 * @param[in]  str     The start of the string containing the next token.
 * @param[out] buf     A buffer to store a null-terminated copy of the next token.
 * @param[in]  buflen  The capacity of the buffer, in characters.  If the token
 *                     is too large to fit, it will be truncated and null-terminated.
 *
 * \return a pointer to the end of the token in \p str.  Thus, to retrieve
 * the subsequent token, call this routine again with the prior return value
 * as the new value of \p str.  Returns NULL when the end of \p str is reached.
 */
const char *
dr_get_token(const char *str, char *buf, size_t buflen);

/** Prints \p msg followed by the instruction \p instr to file \p f. */
void
dr_print_instr(void *drcontext, file_t f, instr_t *instr, const char *msg);

/** Prints \p msg followed by the operand \p opnd to file \p f. */
void
dr_print_opnd(void *drcontext, file_t f, opnd_t opnd, const char *msg);

/**************************************************
 * THREAD SUPPORT
 */


/**
 * Returns the DR context of the current thread.
 */
void *
dr_get_current_drcontext(void);

/** Returns the thread id of the thread with drcontext \p drcontext. */
thread_id_t
dr_get_thread_id(void *drcontext);
#ifdef WINDOWS


/**
 * Returns a Windows handle to the thread with drcontext \p drcontext.
 * This handle is DR's handle to this thread (it is not a separate
 * copy) and as such it should not be closed by the caller; nor should
 * it be used beyond the thread's exit, as DR's handle will be closed
 * at that point.
 *
 * The handle should have THREAD_ALL_ACCESS privileges.
 * \note Windows only.
 */
HANDLE
dr_get_dr_thread_handle(void *drcontext);
#endif


/**
 * Returns the user-controlled thread-local-storage field.  To
 * generate an instruction sequence that reads the drcontext field
 * inline in the code cache, use dr_insert_read_tls_field().
 */
void *
dr_get_tls_field(void *drcontext);

/**
 * Sets the user-controlled thread-local-storage field.  To
 * generate an instruction sequence that reads the drcontext field
 * inline in the code cache, use dr_insert_write_tls_field().
 */
void
dr_set_tls_field(void *drcontext, void *value);

/**
 * Get DR's thread local storage segment base pointed at by \p tls_register.
 * It can be used to get the base of the thread-local storage segment
 * used by #dr_raw_tls_calloc.
 *
 * \note It should not be called on thread exit event,
 * as the thread exit event may be invoked from other threads.
 * See #dr_register_thread_exit_event for details.
 */
void *
dr_get_dr_segment_base(IN reg_id_t tls_register);

/**
 * Allocates \p num_slots contiguous thread-local storage (TLS) slots that
 * can be directly accessed via an offset from \p tls_register.
 * If \p alignment is non-zero, the slots will be aligned to \p alignment.
 * These slots will be initialized to 0 for each new thread.
 * The slot offsets are [\p offset .. \p offset + (num_slots - 1)].
 * These slots are disjoint from the #dr_spill_slot_t register spill slots
 * and the client tls field (dr_get_tls_field()).
 * Returns whether or not the slots were successfully obtained.
 * The linear address of the TLS base pointed at by \p tls_register can be obtained
 * using #dr_get_dr_segment_base.
 * Raw TLs slots can be read directly using dr_insert_read_raw_tls() and written
 * using dr_insert_write_raw_tls().
 *
 * Supports passing 0 for \p num_slots, in which case \p tls_register will be
 * written but no other action taken.
 *
 * \note These slots are useful for thread-shared code caches.  With
 * thread-private caches, DR's memory pools are guaranteed to be
 * reachable via absolute or rip-relative accesses from the code cache
 * and client libraries.
 *
 * \note These slots are a limited resource.  On Windows the slots are
 * shared with the application and reserving even one slot can result
 * in failure to initialize for certain applications.  On Linux they
 * are more plentiful and transparent but currently DR limits clients
 * to no more than 64 slots.
 *
 * \note On Mac OS, TLS slots may not be initialized to zero.
 */
bool
dr_raw_tls_calloc(OUT reg_id_t *tls_register, OUT uint *offset, IN uint num_slots,
                  IN uint alignment);

/**
 * Frees \p num_slots raw thread-local storage slots starting at
 * offset \p offset that were allocated with dr_raw_tls_calloc().
 * Returns whether or not the slots were successfully freed.
 */
bool
dr_raw_tls_cfree(uint offset, uint num_slots);

/**
 * Returns an operand that refers to the raw TLS slot with offset \p
 * tls_offs from the TLS base \p tls_register.
 */
opnd_t
dr_raw_tls_opnd(void *drcontext, reg_id_t tls_register, uint tls_offs);

/**
 * Inserts into ilist prior to "where" instruction(s) to read into the
 * general-purpose full-size register \p reg from the raw TLS slot with offset
 * \p tls_offs from the TLS base \p tls_register.
 */
void
dr_insert_read_raw_tls(void *drcontext, instrlist_t *ilist, instr_t *where,
                       reg_id_t tls_register, uint tls_offs, reg_id_t reg);

/**
 * Inserts into ilist prior to "where" instruction(s) to store the value in the
 * general-purpose full-size register \p reg into the raw TLS slot with offset
 * \p tls_offs from the TLS base \p tls_register.
 */
void
dr_insert_write_raw_tls(void *drcontext, instrlist_t *ilist, instr_t *where,
                        reg_id_t tls_register, uint tls_offs, reg_id_t reg);

/**
 * Creates a new thread that is marked as a non-application thread (i.e., DR
 * will let it run natively and not execute its code from the code cache).  The
 * thread will terminate automatically simply by returning from \p func; if
 * running when the application terminates its last thread, the client thread
 * will also terminate when DR shuts the process down.
 *
 * Init and exit events will not be raised for this thread (instead simply place
 * init and exit code in \p func).
 *
 * The new client thread has a drcontext that can be used for thread-private
 * heap allocations.  It has a stack of the same size as the DR stack used by
 * application threads.
 *
 * On Linux, this thread is guaranteed to have its own private itimer
 * if dr_set_itimer() is called from it.  However this does mean it
 * will have its own process id.
 *
 * A client thread should refrain from spending most of its time in
 * calls to other libraries or making blocking or long-running system
 * calls as such actions may incur performance or correctness problems
 * with DR's synchronization engine, which needs to be able to suspend
 * client threads at safe points and cannot determine whether the
 * aforementioned actions are safe for suspension.  Calling
 * dr_sleep(), dr_thread_yield(), dr_messagebox(), or using DR's locks
 * are safe.  If a client thread spends a lot of time holding locks,
 * consider marking it as un-suspendable by calling
 * dr_client_thread_set_suspendable() for better performance.
 *
 * Client threads, whether suspendable or not, must never execute from
 * the code cache as the underlying fragments might be removed by another
 * thread.
 *
 * Client threads are suspended while DR is not executing the application.
 * This includes initialization time: the client thread's \p func code will not
 * execute until DR starts executing the application.
 *
 * \note Thread creation via this routine is not yet fully
 * transparent: on Windows, the thread will show up in the list of
 * application threads if the operating system is queried about
 * threads.  The thread will not trigger a DLL_THREAD_ATTACH message.
 * On Linux, the thread will not receive signals meant for the application,
 * and is guaranteed to have a private itimer.
 */
bool
dr_create_client_thread(void (*func)(void *param), void *arg);

/**
 * Can only be called from a client thread: returns false if called
 * from a non-client thread.
 *
 * Controls whether a client thread created with dr_create_client_thread()
 * will be suspended by DR for synchronization operations such as
 * flushing or client requests like dr_suspend_all_other_threads().
 * A client thread that spends a lot of time holding locks can gain
 * greater performance by not being suspended.
 *
 * A client thread \b will be suspended for a thread termination
 * operation, including at process exit, regardless of its suspendable
 * requests.
 */
bool
dr_client_thread_set_suspendable(bool suspendable);

/** Current thread sleeps for \p time_ms milliseconds. */
void
dr_sleep(int time_ms);

/** Current thread gives up its time quantum. */
void
dr_thread_yield(void);
/** Flags controlling the behavior of dr_suspend_all_other_threads_ex(). */
typedef enum {
    /**
     * By default, native threads are not suspended by
     * dr_suspend_all_other_threads_ex().  This flag requests that native
     * threads (including those temporarily-native due to actions such as
     * #DR_EMIT_GO_NATIVE) be suspended as well.
     */
    DR_SUSPEND_NATIVE = 0x0001,
} dr_suspend_flags_t;


/**
 * Suspends all other threads in the process and returns an array of
 * contexts in \p drcontexts with one context per successfully suspended
 * thread.  The contexts can be passed to routines like dr_get_thread_id()
 * or dr_get_mcontext().  However, the contexts may not be modified:
 * dr_set_mcontext() is not supported.  dr_get_mcontext() can be called on
 * the caller of this routine, unless in a Windows nudge callback.
 *
 * The \p flags argument controls which threads are suspended and may
 * add further options in the future.
 *
 * The number of successfully suspended threads, which is also the length
 * of the \p drcontexts array, is returned in \p num_suspended, which is a
 * required parameter.  The number of un-successfully suspended threads, if
 * any, is returned in the optional parameter \p num_unsuspended.  The
 * calling thread is not considered in either count.  DR can fail to
 * suspend a thread for privilege reasons (e.g., on Windows in a
 * low-privilege process where another process injected a thread).  This
 * function returns true iff all threads were suspended, in which case \p
 * num_unsuspended will be 0.
 *
 * The caller must invoke dr_resume_all_other_threads() in order to resume
 * the suspended threads, free the \p drcontexts array, and release
 * coarse-grain locks that prevent new threads from being created.
 *
 * This routine may not be called from any registered event callback
 * other than the nudge event or the pre- or post-system call event.
 * It may be called from clean calls out of the cache.
 * This routine may not be called while any locks are held that
 * could block a thread processing a registered event callback or cache
 * callout.
 *
 * \note A client wishing to invoke this routine from an event callback can
 * queue up a nudge via dr_nudge_client() and invoke this routine from the
 * nudge callback.
 */
bool
dr_suspend_all_other_threads_ex(OUT void ***drcontexts, OUT uint *num_suspended,
                                OUT uint *num_unsuspended, dr_suspend_flags_t flags);

/** Identical to dr_suspend_all_other_threads_ex() with \p flags set to 0. */
bool
dr_suspend_all_other_threads(OUT void ***drcontexts, OUT uint *num_suspended,
                             OUT uint *num_unsuspended);

/**
 * May only be used after invoking dr_suspend_all_other_threads().  This
 * routine resumes the threads that were suspended by
 * dr_suspend_all_other_threads() and must be passed the same array and
 * count of suspended threads that were returned by
 * dr_suspend_all_other_threads().  It also frees the \p drcontexts array
 * and releases the locks acquired by dr_suspend_all_other_threads().  The
 * return value indicates whether all resumption attempts were successful.
 */
bool
dr_resume_all_other_threads(IN void **drcontexts, IN uint num_suspended);

/**
 * Returns whether the thread represented by \p drcontext is currently
 * executing natively (typically due to an earlier #DR_EMIT_GO_NATIVE
 * return value).
 */
bool
dr_is_thread_native(void *drcontext);

/**
 * Causes the thread owning \p drcontext to begin executing in the
 * code cache again once it is resumed.  The thread must currently be
 * suspended (typically by dr_suspend_all_other_threads_ex() with
 * #DR_SUSPEND_NATIVE) and must be currently native (typically from
 * #DR_EMIT_GO_NATIVE).  \return whether successful.
 */
bool
dr_retakeover_suspended_native_thread(void *drcontext);

/**
 * Installs an interval timer in the itimer sharing group that
 * contains the calling thread.
 *
 * @param[in] which Must be one of ITIMER_REAL, ITIMER_VIRTUAL, or ITIMER_PROF
 * @param[in] millisec The frequency of the timer, in milliseconds.  Passing
 *   0 disables the timer.
 * @param[in] func The function that will be called each time the
 *   timer fires.  It will be passed the context of the thread that
 *   received the itimer signal and its machine context, which has not
 *   been translated and so may contain raw code cache values.  The function
 *   will be called from a signal handler that may have interrupted a
 *   lock holder or other critical code, so it must be careful in its
 *   operations: keep it as simple as possible, and avoid any non-reentrant actions
 *   such as lock usage. If a general timer that does not interrupt client code
 *   is required, the client should create a separate thread via
 *   dr_create_client_thread() (which is guaranteed to have a private
 *   itimer) and set the itimer there, where the callback function can
 *   perform more operations safely if that new thread never acquires locks
 *   in its normal operation.
 *
 * Itimer sharing varies by kernel.  Prior to 2.6.12 itimers were
 * thread-private; after 2.6.12 they are shared across a thread group,
 * though there could be multiple thread groups in one address space.
 * The dr_get_itimer() function can be used to see whether a thread
 * already has an itimer in its group to avoid re-setting an itimer
 * set by an earlier thread.  A client thread created by
 * dr_create_client_thread() is guaranteed to not share its itimers
 * with application threads.
 *
 * The itimer will operate successfully in the presence of an
 * application itimer of the same type.
 *
 * Additional itimer signals are blocked while in our signal handler.
 *
 * The return value indicates whether the timer was successfully
 * installed (or uninstalled if 0 was passed for \p millisec).
 *
 * \note Linux-only.
 */
bool
dr_set_itimer(int which, uint millisec,
              void (*func)(void *drcontext, dr_mcontext_t *mcontext));

/**
 * If an interval timer is already installed in the itimer sharing group that
 * contains the calling thread, returns its frequency.  Else returns 0.
 *
 * @param[in] which Must be one of ITIMER_REAL, ITIMER_VIRTUAL, or ITIMER_PROF
 *
 * \note Linux-only.
 */
uint
dr_get_itimer(int which);

/**
 * Should be called during process initialization.  Requests more accurate
 * tracking of the #dr_where_am_i_t value for use with dr_where_am_i().  By
 * default, if this routine is not called, DR avoids some updates to the value
 * that incur extra overhead, such as identifying clean callees.
 */
void
dr_track_where_am_i(void);

/**
 * Returns whether DR is using accurate tracking of the #dr_where_am_i value.
 * Typically this is enabled by calling dr_track_where_am_i().
 */
bool
dr_is_tracking_where_am_i(void);

/**
 * Returns the #dr_where_am_i_t value indicating in which area of code \p pc
 * resides.  This is meant for use with dr_set_itimer() for PC sampling for
 * profiling purposes.  If the optional \p tag is non-NULL and \p pc is inside a
 * fragment in the code cache, the fragment's tag is returned in \p tag.  It is
 * recommended that the user of this routine also call dr_track_where_am_i()
 * during process initialization for more accurate results.
 */
dr_where_am_i_t
dr_where_am_i(void *drcontext, app_pc pc, OUT void **tag);

/****************************************************************************
 * ADAPTIVE OPTIMIZATION SUPPORT
 */


/**
 * Replaces the fragment with tag \p tag with the instructions in \p
 * ilist.  This routine is only valid with the -thread_private option;
 * it replaces the fragment for the current thread only.  After
 * replacement, the existing fragment is allowed to complete if
 * currently executing.  For example, a clean call replacing the
 * currently executing fragment will safely return to the existing
 * code.  Subsequent executions will use the new instructions.
 *
 * \note The routine takes control of \p ilist and all responsibility
 * for deleting it.  The client should not keep, use, or reference,
 * the instrlist or any of the instrs it contains after passing.
 *
 * \note This routine supports replacement for the current thread
 * only.  \p drcontext must be from the current thread and must
 * be the drcontext used to create the instruction list.
 * This routine may not be called from the thread exit event.
 *
 * \return false if the fragment does not exist and true otherwise.
 */
bool
dr_replace_fragment(void *drcontext, void *tag, instrlist_t *ilist);

/**
 * Deletes the fragment with tag \p tag.  This routine is only valid
 * with the -thread_private option; it deletes the fragment in the
 * current thread only.  After deletion, the existing fragment is
 * allowed to complete execution.  For example, a clean call deleting
 * the currently executing fragment will safely return to the existing
 * code.  Subsequent executions will cause DynamoRIO to reconstruct
 * the fragment, and therefore call the appropriate fragment-creation
 * event hook, if registered.
 *
 * \note This routine supports deletion for the current thread
 * only.  \p drcontext must be from the current thread and must
 * be the drcontext used to create the instruction list.
 * This routine may not be called from the thread exit event.
 *
 * \note Other options of removing the code fragments from code cache include
 * dr_flush_region(), dr_unlink_flush_region(), and dr_delay_flush_region().
 *
 * \return false if the fragment does not exist and true otherwise.
 */
bool
dr_delete_fragment(void *drcontext, void *tag);

/**
 * Flush all fragments containing any code from the region [\p start, \p start + \p size).
 * Once this routine returns no execution will occur out of the fragments flushed.
 * This routine may only be called during a clean call from the cache, from a nudge
 * event handler, or from a pre- or post-system call event handler.
 * It may not be called from any other event callback.  No locks can
 * held when calling this routine.  If called from a clean call, caller can NOT return
 * to the cache (the fragment that was called out of may have been flushed even if it
 * doesn't apparently overlap the flushed region). Instead the caller must redirect
 * execution via dr_redirect_execution() (or DR_SIGNAL_REDIRECT from a signal callback)
 * after this routine to continue execution.  Returns true if successful.
 *
 * \note This routine may not be called from any registered event callback other than
 * the nudge event, the pre- or post-system call events, the exception event, or
 * the signal event; clean calls
 * out of the cache may call this routine.
 *
 * \note If called from a clean call, caller must continue execution by calling
 * dr_redirect_execution() after this routine, as the fragment containing the callout may
 * have been flushed. The context to use can be obtained via
 * dr_get_mcontext() with the exception of the pc to continue at which must be passed as
 * an argument to the callout (see instr_get_app_pc()) or otherwise determined.
 *
 * \note This routine may not be called while any locks are held that could block a thread
 * processing a registered event callback or cache callout.
 *
 * \note dr_delay_flush_region() has fewer restrictions on use, but is less synchronous.
 *
 * \note Use \p size == 1 to flush fragments containing the instruction at address
 * \p start. A flush of \p size == 0 is not allowed.
 *
 * \note Use flush_completion_callback to specify logic to be executed after the flush
 * and before the threads are resumed. Use NULL if not needed.
 *
 * \note As currently implemented, dr_delay_flush_region() with no completion callback
 * routine specified can be substantially more performant.
 */
bool
dr_flush_region_ex(app_pc start, size_t size,
                   void (*flush_completion_callback)(void *user_data), void *user_data);

/** Equivalent to dr_flush_region_ex(start, size, NULL). */
bool
dr_flush_region(app_pc start, size_t size);

/**
 * Flush all fragments containing any code from the region [\p start, \p start + \p size).
 * Control will not enter a fragment containing code from the region after this returns,
 * but a thread already in such a fragment will finish out the fragment.  This includes
 * the current thread if this is called from a clean call that returns to the cache.
 * This routine may only be called during a clean call from the cache, from a nudge
 * event handler, or from a pre- or post-system call event handler.
 * It may not be called from any other event callback.  No locks can be
 * held when calling this routine.  Returns true if successful.
 *
 * \note This routine may not be called from any registered event callback other than
 * the nudge event, the pre- or post-system call events, the exception event, or
 * the signal event; clean calls
 * out of the cache may call this routine.
 * \note This routine may not be called while any locks are held that could block a thread
 * processing a registered event callback or cache callout.
 * \note dr_delay_flush_region() has fewer restrictions on use, but is less synchronous.
 * \note Use \p size == 1 to flush fragments containing the instruction at address
 * \p start. A flush of \p size == 0 is not allowed.
 * \note This routine is only available with either the -thread_private
 * or -enable_full_api options.  It is not available when -opt_memory is specified.
 */
bool
dr_unlink_flush_region(app_pc start, size_t size);

/**
 * Request a flush of all fragments containing code from the region
 * [\p start, \p start + \p size).  The flush will be performed at the next safe
 * point in time (usually before any new code is added to the cache after this
 * routine is called). If \p flush_completion_callback is non-NULL, it will be
 * called with the \p flush_id provided to this routine when the flush completes,
 * after which no execution will occur out of the fragments flushed. Returns true
 * if the flush was successfully queued.
 *
 * \note dr_flush_region() and dr_unlink_flush_region() can give stronger guarantees on
 * when the flush will occur, but have more restrictions on use.
 * \note Use \p size == 1 to flush fragments containing the instruction at address
 * \p start.  A flush of \p size == 0 is not allowed.
 * \note As currently implemented there may be a performance penalty for requesting a
 * \p flush_completion_callback; for most performant usage set
 * \p flush_completion_callback to NULL.
 */
bool
dr_delay_flush_region(app_pc start, size_t size, uint flush_id,
                      void (*flush_completion_callback)(int flush_id));

/** Returns whether or not there is a fragment in code cache with tag \p tag. */
bool
dr_fragment_exists_at(void *drcontext, void *tag);

/**
 * Returns true if a basic block with tag \p tag exists in the code cache.
 */
bool
dr_bb_exists_at(void *drcontext, void *tag);

/**
 * Looks up the fragment with tag \p tag.
 * If not found, returns 0.
 * If found, returns the total size occupied in the cache by the fragment.
 */
uint
dr_fragment_size(void *drcontext, void *tag);

/** Retrieves the application PC of a fragment with tag \p tag. */
app_pc
dr_fragment_app_pc(void *tag);

/**
 * Given an application PC, returns a PC that contains the application code
 * corresponding to the original PC.  In some circumstances on Windows DR
 * inserts a jump on top of the original code, which the client will not
 * see in the bb and trace hooks due to DR replacing it there with the
 * displaced original application code in order to present the client with
 * an unmodified view of the application code.  A client should use this
 * routine when attempting to decode the original application instruction
 * that caused a fault from the translated fault address, as the translated
 * address may actually point in the middle of DR's jump.
 *
 * \note Other applications on the system sometimes insert their own hooks,
 * which will not be hidden by DR and will appear to the client as jumps
 * and subsequent displaced code.
 */
app_pc
dr_app_pc_for_decoding(app_pc pc);

/**
 * Given a code cache pc, returns the corresponding application pc.
 * This involves translating the state and thus may incur calls to
 * the basic block and trace events (see dr_register_bb_event()).
 * If translation fails, returns NULL.
 * This routine may not be called from a thread exit event.
 */
app_pc
dr_app_pc_from_cache_pc(byte *cache_pc);

/**
 * Returns whether the given thread indicated by \p drcontext
 * is currently using the application version of its system state.
 * \sa dr_switch_to_dr_state(), dr_switch_to_app_state().
 *
 * This function does not indicate whether the machine context
 * (registers) contains application state or not.
 *
 * On Linux, DR very rarely switches the system state, while on
 * Windows DR switches the system state to the DR and client version
 * on every event callback or clean call.
 */
bool
dr_using_app_state(void *drcontext);

/** Equivalent to dr_switch_to_app_state_ex(drcontext, DR_STATE_ALL). */
void
dr_switch_to_app_state(void *drcontext);

/**
 * Swaps to the application version of any system state for the given
 * thread.  This is meant to be used prior to examining application
 * memory, when private libraries are in use and there are two
 * versions of system state.  Invoking non-DR library routines while
 * the application state is in place can lead to unpredictable
 * results: call dr_switch_to_dr_state() (or the _ex version) before
 * doing so.
 *
 * This function does not affect whether the current machine context
 * (registers) contains application state or not.
 *
 * The \p flags argument allows selecting a subset of the state to swap.
 */
void
dr_switch_to_app_state_ex(void *drcontext, dr_state_flags_t flags);

/** Equivalent to dr_switch_to_dr_state_ex(drcontext, DR_STATE_ALL). */
void
dr_switch_to_dr_state(void *drcontext);

/**
 * Should only be called after calling dr_switch_to_app_state() (or
 * the _ex version), or in certain cases where a client is running its
 * own code in an application state.  Swaps from the application
 * version of system state for the given thread back to the DR and
 * client version.
 *
 * This function does not affect whether the current machine context
 * (registers) contains application state or not.
 *
 * A client must call dr_switch_to_dr_state() in order to safely call
 * private library routines if it is running in an application context
 * where dr_using_app_state() returns true.  On Windows, this is the
 * case for any application context, as the system state is always
 * swapped.  On Linux, however, execution of application code in the
 * code cache only swaps the machine context and not the system state.
 * Thus, on Linux, while in the code cache, dr_using_app_state() will
 * return false, and it is safe to invoke private library routines
 * without calling dr_switch_to_dr_state().  Only if client or
 * client-invoked code will examine a segment selector or descriptor
 * does the state need to be swapped.  A state swap is much more
 * expensive on Linux (it requires a system call) than on Windows.
 *
 * The same flags that were passed to dr_switch_to_app_state_ex() should
 * be passed here.
 */
void
dr_switch_to_dr_state_ex(void *drcontext, dr_state_flags_t flags);

/**
 * Intended to be called between dr_app_setup() and dr_app_start() to
 * pre-create code cache fragments for each basic block address in the
 * \p tags array.  This speeds up the subsequent attach when
 * dr_app_start() is called.
 * If any code in the passed-in tags array is not readable, it is up to the
 * caller to handle any fault, as DR's own signal handlers are not enabled
 * at this point.
 * Returns whether successful.
 */
bool
dr_prepopulate_cache(app_pc *tags, size_t tags_count);
/**
 * Specifies the type of indirect branch for use with dr_prepopulate_indirect_targets().
 */
typedef enum {
    DR_INDIRECT_RETURN, /**< Return instruction type. */
    DR_INDIRECT_CALL,   /**< Indirect call instruction type. */
    DR_INDIRECT_JUMP,   /**< Indirect jump instruction type. */
} dr_indirect_branch_type_t;


/**
 * Intended to augment dr_prepopulate_cache() by populating DR's indirect branch
 * tables, avoiding trips back to the dispatcher during initial execution.  This is only
 * effective when one of the the runtime options -shared_trace_ibt_tables and
 * -shared_bb_ibt_tables (depending on whether traces are enabled) is turned on, as
 * this routine does not try to populate tables belonging to threads other than the
 * calling thread.
 *
 * This is meant to be called between dr_app_setup() and dr_app_start(), immediately
 * after calling dr_prepopulate_cache().  It adds entries for each target address in
 * the \p tags array to the indirect branch table for the branch type \p branch_type.
 *
 * Returns whether successful.
 */
bool
dr_prepopulate_indirect_targets(dr_indirect_branch_type_t branch_type, app_pc *tags,
                                size_t tags_count);

/**
 * Retrieves various statistics exported by DR as global, process-wide values.
 * The API is not thread-safe.
 * The caller is expected to pass a pointer to a valid, initialized #dr_stats_t
 * value, with the size field set (see #dr_stats_t).
 * Returns false if stats are not enabled.
 */
bool
dr_get_stats(dr_stats_t *drstats);

/****************************************************************************
 * CUSTOM TRACE SUPPORT
 */



/**
 * Marks the fragment associated with tag \p tag as
 * a trace head.  The fragment need not exist yet -- once it is
 * created it will be marked as a trace head.
 *
 * DR associates a counter with a trace head and once it
 * passes the -hot_threshold parameter, DR begins building
 * a trace.  Before each fragment is added to the trace, DR
 * calls the client's end_trace callback to determine whether
 * to end the trace.  (The callback will be called both for
 * standard DR traces and for client-defined traces.)
 *
 * \note Some fragments are unsuitable for trace heads. DR will
 * ignore attempts to mark such fragments as trace heads and will return
 * false. If the client marks a fragment that doesn't exist yet as a trace
 * head and DR later determines that the fragment is unsuitable for
 * a trace head it will unmark the fragment as a trace head without
 * notifying the client.
 *
 * \note Some fragments' notion of trace heads is dependent on
 * which previous block targets them.  For these fragments, calling
 * this routine will only mark as a trace head for targets from
 * the same memory region.
 *
 * Returns true if the target fragment is marked as a trace head.
 */
bool
dr_mark_trace_head(void *drcontext, void *tag);

/**
 * Checks to see if the fragment (or future fragment) with tag \p tag
 * is marked as a trace head.
 */
bool
dr_trace_head_at(void *drcontext, void *tag);

/** Checks to see that if there is a trace in the code cache at tag \p tag. */
bool
dr_trace_exists_at(void *drcontext, void *tag);

/****************************************************************************
 * BINARY TRACE DUMP FORMAT
 */

/**<pre>
 * Binary trace dump format:
 * the file starts with a tracedump_file_header_t
 * then, for each trace:
     struct _tracedump_trace_header
     if num_bbs > 0 # tracedump_origins
       foreach bb:
           app_pc tag;
           int bb_code_size;
           byte code[bb_code_size];
     endif
     foreach exit:
       struct _tracedump_stub_data
       if linkcount_size > 0 # deprecated
         linkcount_type_t count; # sizeof == linkcount_size
       endif
       if separate from body
       (i.e., exit_stub < cache_start_pc || exit_stub >= cache_start_pc+code_size):
           byte stub_code[15]; # all separate stubs are 15
       endif
     endfor
     byte code[code_size];
</pre>
 */
typedef struct _tracedump_file_header_t {
    int version;        /**< The DynamoRIO version that created the file. */
    bool x64;           /**< Whether a 64-bit DynamoRIO library created the file. */
    int linkcount_size; /**< Size of the linkcount (linkcounts are deprecated). */
} tracedump_file_header_t;

/** Header for an individual trace in a binary trace dump file. */
typedef struct _tracedump_trace_header_t {
    int frag_id;           /**< Identifier for the trace. */
    app_pc tag;            /**< Application address for start of trace. */
    app_pc cache_start_pc; /**< Code cache address of start of trace. */
    int entry_offs;        /**< Offset into trace of normal entry. */
    int num_exits;         /**< Number of exits from the trace. */
    int code_size;         /**< Length of the trace in the code cache. */
    uint num_bbs;          /**< Number of constituent basic blocks making up the trace. */
    bool x64;              /**< Whether the trace contains 64-bit code. */
} tracedump_trace_header_t;

/** Size of tag + bb_code_size fields for each bb. */
#define BB_ORIGIN_HEADER_SIZE (sizeof(app_pc) + sizeof(int))

/**< tracedump_stub_data_t.stub_size will not exceed this value. */
#define SEPARATE_STUB_MAX_SIZE IF_X64_ELSE(23, 15)

/** The format of a stub in a trace dump file. */
typedef struct _tracedump_stub_data {
    int cti_offs; /**< Offset from the start of the fragment. */
    /* stub_pc is an absolute address, since can be separate from body. */
    app_pc stub_pc; /**< Code cache address of the stub. */
    app_pc target;  /**< Target of the stub. */
    bool linked;    /**< Whether the stub is linked to its target. */
    int stub_size;  /**< Length of stub_code array */
    /****** the rest of the fields are optional and may not be present! ******/
    union {
        uint count32;   /**< 32-bit exit execution count. */
        uint64 count64; /**< 64-bit exit execution count. */
    } count;            /**< Which field is present depends on the first entry in
                         * the file, which indicates the linkcount size. */
    /** Code for exit stubs.  Only present if:
     *   stub_pc < cache_start_pc ||
     *   stub_pc >= cache_start_pc+code_size).
     * The actual size of the array varies and is indicated by the stub_size field.
     */
    byte stub_code[1 /*variable-sized*/];
} tracedump_stub_data_t;

/** The last offset into tracedump_stub_data_t of always-present fields. */
#define STUB_DATA_FIXED_SIZE (offsetof(tracedump_stub_data_t, count))

/****************************************************************************/



#endif /* _DR_TOOLS_H_ */
