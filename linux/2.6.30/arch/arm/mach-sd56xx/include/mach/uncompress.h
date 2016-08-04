#ifndef __HSAN_UNCOMPRESS_H__
#define __HSAN_UNCOMPRESS_H__

#define AMBA_UART_DR	(*(volatile unsigned int *)0x1010F000)
//#define AMBA_UART_LCRH	(*(volatile unsigned char *)0x1010F)
//#define AMBA_UART_CR	(*(volatile unsigned char *)0x1010F)
#define AMBA_UART_TFL	(*(volatile unsigned int *)0x1010F07C)
#define AMBA_UART_FR	(*(volatile unsigned int *)0x1010F014)

/*
 * This does not append a newline
 */
static inline void putc(int c)
{
	while ((AMBA_UART_FR & 0x20) != 0x20)
		barrier();

	AMBA_UART_DR = c;
}

static inline void flush(void)
{
	while (AMBA_UART_TFL & 0x1)
		barrier();
}

/*
 * nothing to do
 */
#define arch_decomp_setup()
#define arch_decomp_wdog()

#endif