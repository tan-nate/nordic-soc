/* Copyright 2017-2020 The MathWorks, Inc. */
#include <stdint.h>

#ifndef SYS_LIGHTWEIGHT_PROT
    #define SYS_LIGHTWEIGHT_PROT
#endif

typedef uint32_t INTR_FLG_TYPE;

#define SYS_ARCH_DECL_PROTECT(lev) INTR_FLG_TYPE lev
#define SYS_ARCH_PROTECT(lev) lev = sys_arch_protect()
#define SYS_ARCH_UNPROTECT(lev) sys_arch_unprotect(lev)

INTR_FLG_TYPE sys_arch_protect();
void sys_arch_unprotect(INTR_FLG_TYPE lev);
