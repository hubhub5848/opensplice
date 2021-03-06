/*
 *                         Vortex OpenSplice
 *
 *   This software and documentation are Copyright 2006 to TO_YEAR ADLINK
 *   Technology Limited, its affiliated companies and licensors. All rights
 *   reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
/****************************************************************
 * Interface definition for thread management of SPLICE-DDS     *
 ****************************************************************/

#ifndef OS_THREAD_H
#define OS_THREAD_H

/** \file os_thread.h
 *  \brief Thread management - create threads
 */

#include "os_defs.h"

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef OSPL_BUILD_CORE
#define OS_API OS_API_EXPORT
#else
#define OS_API OS_API_IMPORT
#endif
/* !!!!!!!!NOTE From here no more includes are allowed!!!!!!! */

/* Number of slots in Thread Private Memory */

typedef enum os_threadMemoryIndex {
    OS_THREAD_CLOCK_OFFSET,
    OS_THREAD_UT_TRACE,
    OS_THREAD_JVM,
    OS_THREAD_PROTECT,
    OS_THREAD_API_INFO,
    OS_THREAD_WARNING,
    OS_THREAD_ALLOCATOR_STATE,
    OS_THREAD_NAME,
    OS_THREAD_REPORT_STACK,
    OS_THREAD_PROCESS_INFO,
    OS_THREAD_STATE, /* Used for monitoring thread progress */
    OS_THREAD_STR_ERROR,
    OS_THREAD_MEM_ARRAY_SIZE /* Number of slots in Thread Private Memory */
} os_threadMemoryIndex;

/** \brief Platform dependent thread identification
 */
typedef os_os_threadId os_threadId;

/** \brief Definition for a thread routine invoked on thread create. */
typedef void *(*os_threadRoutine)(void*);

/** \brief Definition of the thread attributes
 */
typedef struct os_threadAttr {
    /** Specifies the scheduling class */
    os_schedClass       schedClass;
    /** Specifies the thread priority */
    os_int32            schedPriority;
    /** Specifies the thread stack size */
    os_uint32           stackSize;
} os_threadAttr;

/** \brief Definition for hook callbacks */
typedef int (*os_threadCallback)(os_threadId, void *);

/** \brief Definition for thread hook */
typedef struct os_threadHook {
    /**
     * This callback is called before the thread main
     * function is called. When a non-zero value is returned
     * the 'stopCb' is called and the thread terminates.
     */
    os_threadCallback startCb;
    void              *startArg; /* User argument passed in the callback */
    /**
     * This callback is called after the thread main returns. The result value
     * is ignored.
     */
    os_threadCallback stopCb;
    void              *stopArg; /* User argument passed in the callback */
} os_threadHook;

/** \brief Set thread hook.
 *
 *  A thread hook is a collection of callback routines that
 *  are called during creation and termination of a thread.
 *
 *  The previous setting of the thread hook is returned.
 *
 *  NOTE: this function is not re-entrant!
 *
 *  Possible Results:
 *  - returns os_resultSuccess if
 *      the hooks are succesfully set
 *  - returns os_ResultFail if
 *      the hooks are not set
 */
OS_API os_result
os_threadModuleSetHook(
    os_threadHook *hook,
    os_threadHook *oldHook);

/** \brief Terminate the calling thread
 *
 * Terminate the calling thread passing the result
 * to any thread waiting for the termination
 */
OS_API void
os_threadExit(
    void *thread_result);

/** \brief Create a new thread
 *
 * Creates a new thread of control that executes concurrently with
 * the calling thread. The new thread applies the function start_routine
 * passing it arg as first argument.
 *
 * The new thread terminates either explicitely, by calling os_threadExit,
 * or implicitely, by returning from the start_routine function. The latter
 * case is equivelant to calling os_threadExit with the result returned by
 * start_routine as exit code. The cerated thread is identified by the
 * returned threadId.
 *
 * Possible Results:
 * - assertion failure: threadId = NULL || name = NULL ||
 *     threadAttr = NULL || start_routine = NULL
 * - returns os_resultSuccess if
 *     the thread is successfuly created
 * - returns os_resultFail if
 *     the thread is not created because of an failure
 */
OS_API os_result
os_threadCreate(
    os_threadId *threadId,
    const char *name,
    const os_threadAttr *threadAttr,
    os_threadRoutine start_routine,
    void *arg);

/** \brief Return the integer representation of the given thread ID
 *
 * Possible Results:
 * - returns the integer representation of the given thread ID
 */
OS_API os_ulong_int
os_threadIdToInteger(
    os_threadId id);

/** \brief Return the thread ID of the calling thread
 *
 * Possible Results:
 * - returns the tread ID of the calling thread
 */
OS_API os_threadId
os_threadIdSelf(void);

/** \brief Wait for the termination of the identified thread
 *
 * If the identified thread is still running, wait for its termination
 * else return immediately. In thread_result it returns the exit status
 * of the thread. If thread_result is passed as a NULL pointer,
 * no exit status is returned, but os_threadWaitExit still waits for the
 * thread to terminate.
 *
 * Possible Results:
 * - assertion failure: threadId = 0
 * - returns os_resultSuccess when
 *     the identified thread is not running
 * - returns os_threadFail if
 *     the services is aborted because of a failure
 */
OS_API os_result
os_threadWaitExit(
    os_threadId threadId,
    void **thread_result);

/** \brief Figure out the identity of the current thread
 *
 * Possible Results:
 * - returns the actual length of threadIdentity
 *
 * Postcondition:
 * - \b threadIdentity is ""
 *     the thread identity could not be determined
 * - \b threadIdentity is "<decimal number>" | "0x<hexadecimal number>"
 *     only the thread numeric identity could be determined
 * - \b threadIdentity is "<process name> (<decimal number>)" | "<process name> (0x<hexadecimal number>)"
 *     the thread name and numeric identity could be determined
 *
 * \b threadIdentity will not be filled beyond the specified \b threadIdentitySize
 */
OS_API os_int32
os_threadFigureIdentity(
    char *threadIdentity,
    os_uint32 threadIdentitySize);

/** \brief Get name of current thread
 *
 * Postcondition:
 * - \b name is ""
 *     the thread name could not be determined
 * - \b name is "<thread name>"
 *     the thread name could be determined
 *
 * \b name will not be filled beyond the specified \b length
 */
OS_API os_int32
os_threadGetThreadName(
    os_char *name,
    os_uint32 length);

/** \brief Set the default thread attributes
 *
 * Postcondition:
 * - thread scheduling class is OS_SCHED_DEFAULT
 * - thread priority is platform dependent
 *
 * Possible Results:
 * - assertion failure: threadAttr = NULL
 */
OS_API void
os_threadAttrInit(
        os_threadAttr *threadAttr)
    __nonnull_all__;


/** \brief Definition for thread private memory destructor */
typedef int (*os_threadPrivMemDestructor)(void* threadMem, void* userArg);

/** \brief Allocate thread private memory
 *
 * Allocate heap memory of the specified \b size and
 * relate it to the thread by storing the memory
 * reference in an thread specific reference array
 * indexed by \b index. If the indexed thread reference
 * array location already contains a reference, no
 * memory will be allocated and NULL is returned.
 * \b A registered destructor \b destructor will be
 * called if the private memory will be deleted.
 *
 * Possible Results:
 * - returns NULL if
 *     index < 0 || index >= OS_THREAD_MEM_ARRAY_SIZE
 * - returns NULL if
 *     no sufficient memory is available on heap
 * - returns NULL if
 *     os_threadMemGet (index) returns != NULL
 * - returns reference to allocated heap memory
 *     of the requested size if
 *     memory is successfully allocated
 */
OS_API void *
os_threadMemMalloc(
    os_int32 index,
    os_size_t size,
    os_threadPrivMemDestructor destructor,
    void* userArgs);

/** \brief Free thread private memory
 *
 * Free the memory referenced by the thread reference
 * array indexed location. If this reference is NULL,
 * or index is invalid, no action is taken.
 * The reference is set to NULL after freeing the
 * heap memory.
 *
 * Postcondition:
 * - os_threadMemGet (index) = NULL and allocated
 *   heap memory is freed
 */
OS_API void
os_threadMemFree(
    os_int32 index);

/** \brief Get thread private memory
 *
 * Possible Results:
 * - returns NULL if
 *     index < 0 || index >= OS_THREAD_MEM_ARRAY_SIZE
 * - returns NULL if
 *     No heap memory is related to the thread for
 *     the specified index
 * - returns a reference to the allocated memory
 */
OS_API void *
os_threadMemGet(
    os_int32 index);

OS_API os_result
os_threadProtect(void);

OS_API os_result
os_threadUnprotect(void);

OS_API void
os_threadMemExit(void);

#undef OS_API

#if defined (__cplusplus)
}
#endif

#endif /* OS_THREAD_H */
