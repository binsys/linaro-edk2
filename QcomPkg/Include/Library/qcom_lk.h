#ifndef QCOM_LK_COMMON_H
#define QCOM_LK_COMMON_H

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t)&((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, type, member) \
	((type *)((UINT32)(ptr) - offsetof(type, member)))
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


#define writel(v, a) MmioWrite32((UINTN)(a),(v))
#define readl(a) MmioRead32((UINTN)(a))

extern void udelay(unsigned usecs);


#endif
