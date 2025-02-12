/*
* Copyright 2023 The MathWorks, Inc.
*
*/

#ifndef XCP_PLATFORM_CUSTOM_H
#define XCP_PLATFORM_CUSTOM_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sys_arch.h"

#define XCP_MUTEX_DEFINE(lock)    uint32_t lock
#define XCP_MUTEX_INIT(lock)      lock = 0
#define XCP_MUTEX_LOCK(lock)      lock = sys_arch_protect()
#define XCP_MUTEX_UNLOCK(lock)    sys_arch_unprotect(lock)

#define PRAGMA(n)   _Pragma(#n)
#define XCP_PRAGMA_PACK_BEGIN(n)     PRAGMA(pack(push, n))
#define XCP_PRAGMA_PACK_END()        PRAGMA(pack(pop))
#define XCP_ATTRIBUTE_ALIGNED(n)
#define XCP_ATTRIBUTE_PACKED

#define XCP_ADDRESS_GET(addressExtension, address)  (uint8_T*) ((uintptr_t) address)

#define XCP_SLEEP(s, us)          do { int32_t i; for(i = 0; i < s; i++) __NOP();} while(0);

#define XCP_MEM_ALIGNMENT 4

#ifndef XCP_MEM_BLOCK_3_NUMBER
    #if defined(XCP_MEM_DAQ_RESERVED_POOLS_NUMBER) && (XCP_MEM_DAQ_RESERVED_POOLS_NUMBER > 0)
        /* @note if the memory allocator is compiled for External Mode
        this number needs to match the number of times the ALLOC_ODT_ENTRY
        command is invoked (odtCount * daqCount). Typically odtCount = 1.
        Each rate has an associated DAQ list. Profiling uses another DAQ list.
        Adding 2 extra ones to take into account of other XCP events on the target. */

        #if defined(EXTMODE_CODE_EXEC_PROFILING) && !defined(EXTMODE_CODE_EXEC_PROFILING_CUSTOM) &&\
        (XCP_MEM_BLOCK_2_SIZE < XCP_MAX_PROFILING_BUFFER_SIZE)
        /* Since they were not added to the BLOCK 2 set, additional blocks
        have been added and available for the transfer of Profiling data. */
        #define XCP_MEM_BLOCK_3_NUMBER (XCP_MEM_DAQ_RESERVED_POOLS_NUMBER + 3 + XCP_MAX_PROFILING_BUFFERS_NUMBER)
        #else
        #define XCP_MEM_BLOCK_3_NUMBER (XCP_MEM_DAQ_RESERVED_POOLS_NUMBER + 3)
        #endif
    #else
        /* Default value if not compiled for External Mode */
        #define XCP_MEM_BLOCK_3_NUMBER 10
    #endif
#endif /* XCP_MEM_BLOCK_3_NUMBER */


#ifndef XCP_TIMESTAMP_BASED_ON_SIMULATION_TIME
    uint32_t xcpGetTimestamp(void);
    #define profileTimerRead() xcpGetTimestamp()
    #define XCP_TIMESTAMP_GET() xcpGetTimestamp()
    #define XCP_TIMESTAMP_UNIT XCP_TIMESTAMP_UNIT_1US
#endif
    
#endif

/* LocalWords:  ODT odt
 */
