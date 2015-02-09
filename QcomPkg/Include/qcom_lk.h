#ifndef COMMON_H
#define COMMON_H

typedef unsigned int addr_t;

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#endif

#ifndef size_t
#define size_t unsigned int
#endif

#define ERR_INVALID_ARGS -8
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#define BIT(bit) (1U << (bit))
#define BIT_GET(x, bit) ((x)& (1 << (bit)))
#define BIT_SHIFT(x, bit) (((x) >> (bit)) & 1)
#define BITS(x, high, low) ((x)& (((1 << ((high)+1)) - 1) & ~((1 << (low)) - 1)))

#define ERR_NOT_VALID 1
#define NO_ERROR 1
#define ERROR -1


/* low level macros for accessing memory mapped hardware registers */
//#define REG64(addr) ((volatile uint64_t *)(addr))
//#define REG32(addr) ((volatile uint32_t *)(addr))
//#define REG16(addr) ((volatile uint16_t *)(addr))
//#define REG8(addr) ((volatile uint8_t *)(addr))

//#define RMWREG64(addr, startbit, width, val) *REG64(addr) = (*REG64(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))
//#define RMWREG32(addr, startbit, width, val) *REG32(addr) = (*REG32(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))
//#define RMWREG16(addr, startbit, width, val) *REG16(addr) = (*REG16(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))
//#define RMWREG8(addr, startbit, width, val) *REG8(addr) = (*REG8(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))

//#define writel(v, a) (*REG32(a) = (v))
//#define readl(a) (*REG32(a))
//#define writeb(v, a) (*REG8(a) = (v))
//#define readb(a) (*REG8(a))

#define writel(v, a) MmioWrite32((UINTN)(a),(v))
#define readl(a) MmioRead32((UINTN)(a))

extern void udelay(unsigned usecs);


#endif
