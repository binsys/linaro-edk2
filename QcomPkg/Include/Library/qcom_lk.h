#ifndef QCOM_LK_COMMON_H
#define QCOM_LK_COMMON_H

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
	((type *)((UINT32)(ptr) - offsetof(type, member)))
#endif

#ifndef ROUND_TO_PAGE
#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))
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

//#define ERR_NOT_VALID 1

#define NO_ERROR 0
#define ERROR -1
#define ERR_NOT_FOUND -2
#define ERR_NOT_READY -3
#define ERR_NO_MSG -4
#define ERR_NO_MEMORY -5
#define ERR_ALREADY_STARTED -6
#define ERR_NOT_VALID -7
#define ERR_INVALID_ARGS -8
#define ERR_NOT_ENOUGH_BUFFER -9
#define ERR_NOT_SUSPENDED -10
#define ERR_OBJECT_DESTROYED -11
#define ERR_NOT_BLOCKED -12
#define ERR_TIMED_OUT -13
#define ERR_ALREADY_EXISTS -14
#define ERR_CHANNEL_CLOSED -15
#define ERR_OFFLINE -16
#define ERR_NOT_ALLOWED -17
#define ERR_BAD_PATH -18
#define ERR_ALREADY_MOUNTED -19
#define ERR_IO -20
#define ERR_NOT_DIR -21
#define ERR_NOT_FILE -22
#define ERR_RECURSE_TOO_DEEP -23
#define ERR_NOT_SUPPORTED -24
#define ERR_TOO_BIG -25


#define writel(v, a) MmioWrite32((UINTN)(a),(v))
#define readl(a) MmioRead32((UINTN)(a))
#define dmb ArmDataMemoryBarrier
#define dsb ArmDataSyncronizationBarrier

// define a macro that unconditionally swaps
#define SWAP_32(x) \
	(((UINT32)(x) << 24) | (((UINT32)(x) & 0xff00) << 8) |(((UINT32)(x) & 0x00ff0000) >> 8) | ((UINT32)(x) >> 24))
#define SWAP_16(x) \
	((((UINT16)(x) & 0xff) << 8) | ((UINT16)(x) >> 8))
	
#define LE32(val) (val)
#define LE16(val) (val)
#define BE32(val) SWAP_32(val)
#define BE16(val) SWAP_16(val)

#define LE32SWAP(var) (var) = LE32(var);
#define LE16SWAP(var) (var) = LE16(var);
#define BE32SWAP(var) (var) = BE32(var);
#define BE16SWAP(var) (var) = BE16(var);

/* classic network byte swap stuff */
#define ntohs(n) BE16(n)
#define htons(h) BE16(h)
#define ntohl(n) BE32(n)
#define htonl(h) BE32(h)

extern void udelay(unsigned usecs);

/* low level macros for accessing memory mapped hardware registers */
#define REG64(addr) ((volatile UINT64 *)(addr))
#define REG32(addr) ((volatile UINT32 *)(addr))
#define REG16(addr) ((volatile UINT16 *)(addr))
#define REG8(addr) ((volatile UINT8 *)(addr))

#define RMWREG64(addr, startbit, width, val) *REG64(addr) = (*REG64(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))
#define RMWREG32(addr, startbit, width, val) *REG32(addr) = (*REG32(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))
#define RMWREG16(addr, startbit, width, val) *REG16(addr) = (*REG16(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))
#define RMWREG8(addr, startbit, width, val) *REG8(addr) = (*REG8(addr) & ~(((1<<(width)) - 1) << (startbit))) | ((val) << (startbit))


#endif
