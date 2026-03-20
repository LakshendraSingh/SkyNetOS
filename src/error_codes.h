#ifndef ERRORS_H
#define ERRORS_H

#define ERR_NULL_POINTER          1
#define ERR_OUT_OF_MEMORY         2
#define ERR_FILE_NOT_FOUND        3
#define ERR_DIV_BY_ZERO           4
#define UNKNOWN_ERR               5
#define UNKNOWN_CMD               6
#define ERR_SYS_CRASH             7
#define ERR_SYS_OUT_OF_MEM        8
#define sim_err                   9 //ERR_INVALID_ARGUMENT
#define ERR_FILE_EXISTS           10

#define System_Shutdown           101 //exit

/*
 * x86 CPU Exception Vectors
 * Mapped to standard hardware Interrupt Descriptor Table (IDT) entries.
 */
#define EXC_DIVIDE_ERROR                 0x00  // Fault (#DE)
#define EXC_DEBUG                        0x01  // Fault/Trap (#DB)
#define EXC_NON_MASKABLE_INTERRUPT       0x02  // Interrupt
#define EXC_BREAKPOINT                   0x03  // Trap (#BP)
#define EXC_OVERFLOW                     0x04  // Trap (#OF)
#define EXC_BOUND_RANGE_EXCEEDED         0x05  // Fault (#BR)
#define EXC_INVALID_OPCODE               0x06  // Fault (#UD)
#define EXC_DEVICE_NOT_AVAILABLE         0x07  // Fault (#NM)
#define EXC_DOUBLE_FAULT                 0x08  // Abort (#DF) - Pushes Error Code (0)
#define EXC_COPROCESSOR_SEG_OVERRUN      0x09  // Fault (Legacy)
#define EXC_INVALID_TSS                  0x0A  // Fault (#TS) - Pushes Error Code
#define EXC_SEGMENT_NOT_PRESENT          0x0B  // Fault (#NP) - Pushes Error Code
#define EXC_STACK_SEGMENT_FAULT          0x0C  // Fault (#SS) - Pushes Error Code
#define EXC_GENERAL_PROTECTION_FAULT     0x0D  // Fault (#GP) - Pushes Error Code
#define EXC_PAGE_FAULT                   0x0E  // Fault (#PF) - Pushes Error Code
// 0x0F (15) is Reserved by Intel
#define EXC_X87_FLOATING_POINT           0x10  // Fault (#MF)
#define EXC_ALIGNMENT_CHECK              0x11  // Fault (#AC) - Pushes Error Code
#define EXC_MACHINE_CHECK                0x12  // Abort (#MC)
#define EXC_SIMD_FLOATING_POINT          0x13  // Fault (#XM/#XF)
#define EXC_VIRTUALIZATION               0x14  // Fault (#VE)
#define EXC_CONTROL_PROTECTION           0x15  // Fault (#CP) - Pushes Error Code
// 0x16 to 0x1B (22-27) are Reserved by Intel
#define EXC_HYPERVISOR_INJECTION         0x1C  // Fault (#HV)
#define EXC_VMM_COMMUNICATION            0x1D  // Fault (#VC) - Pushes Error Code
#define EXC_SECURITY                     0x1E  // Fault (#SX) - Pushes Error Code
// 0x1F (31) is Reserved by Intel

/* Note: Triple Fault does not have a vector number as it resets the CPU. */

#endif