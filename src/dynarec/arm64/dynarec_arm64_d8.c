#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#include "debug.h"
#include "box64context.h"
#include "box64cpu.h"
#include "emu/x64emu_private.h"
#include "x64emu.h"
#include "box64stack.h"
#include "callback.h"
#include "emu/x64run_private.h"
#include "x64trace.h"
#include "emu/x87emu_private.h"
#include "dynarec_native.h"

#include "arm64_printer.h"
#include "dynarec_arm64_private.h"
#include "../dynarec_helper.h"
#include "dynarec_arm64_functions.h"


uintptr_t dynarec64_D8(dynarec_arm_t* dyn, uintptr_t addr, uintptr_t ip, int ninst, rex_t rex, int rep, int* ok, int* need_epilog)
{
    (void)ip; (void)rep; (void)need_epilog;

    uint8_t nextop = F8;
    uint8_t ed;
    uint8_t u8;
    int64_t fixedaddress;
    int unscaled;
    int v1, v2;
    int s0;

    MAYUSE(s0);
    MAYUSE(v2);
    MAYUSE(v1);

    if(MODREG)
    switch(nextop) {
        case 0xC0:
        case 0xC1:
        case 0xC2:
        case 0xC3:
        case 0xC4:
        case 0xC5:
        case 0xC6:
        case 0xC7:
            INST_NAME("FADD ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(!BOX64ENV(dynarec_fastround))
                u8 = x87_setround(dyn, ninst, x1, x2, x4);
            if(ST_IS_F(0)) {
                FADDS(v1, v1, v2);
            } else {
                FADDD(v1, v1, v2);
                X87_CHECK_PRECISION(v1);
            }
            if(!BOX64ENV(dynarec_fastround))
                x87_restoreround(dyn, ninst, u8);
            break;
        case 0xC8:
        case 0xC9:
        case 0xCA:
        case 0xCB:
        case 0xCC:
        case 0xCD:
        case 0xCE:
        case 0xCF:
            INST_NAME("FMUL ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(!BOX64ENV(dynarec_fastround))
                u8 = x87_setround(dyn, ninst, x1, x2, x4);
            if(ST_IS_F(0)) {
                FMULS(v1, v1, v2);
            } else {
                FMULD(v1, v1, v2);
                X87_CHECK_PRECISION(v1);
            }
            if(!BOX64ENV(dynarec_fastround))
                x87_restoreround(dyn, ninst, u8);
            break;
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
            INST_NAME("FCOM ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(ST_IS_F(0)) {
                FCMPS(v1, v2);
            } else {
                FCMPD(v1, v2);
            }
            FCOM(x1, x2, x3);
            break;
        case 0xD8:
        case 0xD9:
        case 0xDA:
        case 0xDB:
        case 0xDC:
        case 0xDD:
        case 0xDE:
        case 0xDF:
            INST_NAME("FCOMP ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(ST_IS_F(0)) {
                FCMPS(v1, v2);
            } else {
                FCMPD(v1, v2);
            }
            FCOM(x1, x2, x3);
            X87_POP_OR_FAIL(dyn, ninst, x3);
            break;
        case 0xE0:
        case 0xE1:
        case 0xE2:
        case 0xE3:
        case 0xE4:
        case 0xE5:
        case 0xE6:
        case 0xE7:
            INST_NAME("FSUB ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(!BOX64ENV(dynarec_fastround))
                u8 = x87_setround(dyn, ninst, x1, x2, x4);
            if(ST_IS_F(0)) {
                FSUBS(v1, v1, v2);
            } else {
                FSUBD(v1, v1, v2);
                X87_CHECK_PRECISION(v1);
            }
            if(!BOX64ENV(dynarec_fastround))
                x87_restoreround(dyn, ninst, u8);
            break;
        case 0xE8:
        case 0xE9:
        case 0xEA:
        case 0xEB:
        case 0xEC:
        case 0xED:
        case 0xEE:
        case 0xEF:
            INST_NAME("FSUBR ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(!BOX64ENV(dynarec_fastround))
                u8 = x87_setround(dyn, ninst, x1, x2, x4);
            if(ST_IS_F(0)) {
                FSUBS(v1, v2, v1);
            } else {
                FSUBD(v1, v2, v1);
                X87_CHECK_PRECISION(v1);
            }
            if(!BOX64ENV(dynarec_fastround))
                x87_restoreround(dyn, ninst, u8);
            break;
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
            INST_NAME("FDIV ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(!BOX64ENV(dynarec_fastround))
                u8 = x87_setround(dyn, ninst, x1, x2, x4);
            if(ST_IS_F(0)) {
                FDIVS(v1, v1, v2);
            } else {
                FDIVD(v1, v1, v2);
                X87_CHECK_PRECISION(v1);
            }
            if(!BOX64ENV(dynarec_fastround))
                x87_restoreround(dyn, ninst, u8);
            break;
        case 0xF8:
        case 0xF9:
        case 0xFA:
        case 0xFB:
        case 0xFC:
        case 0xFD:
        case 0xFE:
        case 0xFF:
            INST_NAME("FDIVR ST0, STx");
            v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_COMBINE(0, nextop&7));
            v2 = x87_get_st(dyn, ninst, x1, x2, nextop&7, X87_COMBINE(0, nextop&7));
            if(!BOX64ENV(dynarec_fastround))
                u8 = x87_setround(dyn, ninst, x1, x2, x4);
            if(ST_IS_F(0)) {
                FDIVS(v1, v2, v1);
            } else {
                FDIVD(v1, v2, v1);
                X87_CHECK_PRECISION(v1);
            }
            if(!BOX64ENV(dynarec_fastround))
                x87_restoreround(dyn, ninst, u8);
            break;
        default:
            DEFAULT;
            break;
    } else
        switch((nextop>>3)&7) {
            case 0:
                INST_NAME("FADD ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(!BOX64ENV(dynarec_fastround))
                    u8 = x87_setround(dyn, ninst, x1, x5, x4);
                if(ST_IS_F(0)) {
                    FADDS(v1, v1, s0);
                } else {
                    FCVT_D_S(s0, s0);
                    FADDD(v1, v1, s0);
                    X87_CHECK_PRECISION(v1);
                }
                if(!BOX64ENV(dynarec_fastround))
                    x87_restoreround(dyn, ninst, u8);
                break;
            case 1:
                INST_NAME("FMUL ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(!BOX64ENV(dynarec_fastround))
                    u8 = x87_setround(dyn, ninst, x1, x5, x4);
                if(ST_IS_F(0)) {
                    FMULS(v1, v1, s0);
                } else {
                    FCVT_D_S(s0, s0);
                    FMULD(v1, v1, s0);
                    X87_CHECK_PRECISION(v1);
                }
                if(!BOX64ENV(dynarec_fastround))
                    x87_restoreround(dyn, ninst, u8);
                break;
            case 2:
                INST_NAME("FCOM ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(ST_IS_F(0)) {
                    FCMPS(v1, s0);
                } else {
                    FCVT_D_S(s0, s0);
                    FCMPD(v1, s0);
                    X87_CHECK_PRECISION(v1);
                }
                FCOM(x1, x2, x3);
                break;
            case 3:
                INST_NAME("FCOMP ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(ST_IS_F(0)) {
                    FCMPS(v1, s0);
                } else {
                    FCVT_D_S(s0, s0);
                    FCMPD(v1, s0);
                    X87_CHECK_PRECISION(v1);
                }
                FCOM(x1, x2, x3);
                X87_POP_OR_FAIL(dyn, ninst, x3);
                break;
            case 4:
                INST_NAME("FSUB ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(!BOX64ENV(dynarec_fastround))
                    u8 = x87_setround(dyn, ninst, x1, x5, x4);
                if(ST_IS_F(0)) {
                    FSUBS(v1, v1, s0);
                } else {
                    FCVT_D_S(s0, s0);
                    FSUBD(v1, v1, s0);
                    X87_CHECK_PRECISION(v1);
                }
                if(!BOX64ENV(dynarec_fastround))
                    x87_restoreround(dyn, ninst, u8);
                break;
            case 5:
                INST_NAME("FSUBR ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(!BOX64ENV(dynarec_fastround))
                    u8 = x87_setround(dyn, ninst, x1, x5, x4);
                if(ST_IS_F(0)) {
                    FSUBS(v1, s0, v1);
                } else {
                    FCVT_D_S(s0, s0);
                    FSUBD(v1, s0, v1);
                    X87_CHECK_PRECISION(v1);
                }
                if(!BOX64ENV(dynarec_fastround))
                    x87_restoreround(dyn, ninst, u8);
                break;
            case 6:
                INST_NAME("FDIV ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(!BOX64ENV(dynarec_fastround))
                    u8 = x87_setround(dyn, ninst, x1, x5, x4);
                if(ST_IS_F(0)) {
                    FDIVS(v1, v1, s0);
                } else {
                    FCVT_D_S(s0, s0);
                    FDIVD(v1, v1, s0);
                    X87_CHECK_PRECISION(v1);
                }
                if(!BOX64ENV(dynarec_fastround))
                    x87_restoreround(dyn, ninst, u8);
                break;
            case 7:
                INST_NAME("FDIVR ST0, float[ED]");
                v1 = x87_get_st(dyn, ninst, x1, x2, 0, X87_ST0);
                s0 = fpu_get_scratch(dyn, ninst);
                addr = geted(dyn, addr, ninst, nextop, &ed, x2, &fixedaddress, &unscaled, 0xfff<<2, 3, rex, NULL, 0, 0);
                VLD32(s0, ed, fixedaddress);
                if(!BOX64ENV(dynarec_fastround))
                    u8 = x87_setround(dyn, ninst, x1, x5, x4);
                if(ST_IS_F(0)) {
                    FDIVS(v1, s0, v1);
                } else {
                    FCVT_D_S(s0, s0);
                    FDIVD(v1, s0, v1);
                    X87_CHECK_PRECISION(v1);
                }
                if(!BOX64ENV(dynarec_fastround))
                    x87_restoreround(dyn, ninst, u8);
                break;
            default:
                DEFAULT;
        }
    return addr;
}
