/*
 *   Octeon Watchdog driver
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2007 Cavium Networks
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/string.h>
#include <linux/delay.h>
#include "hal.h"

int timeout = 0;		/* A zero value is replaced with the max
				   timeout */
module_param(timeout, int, 0444);
MODULE_PARM_DESC(timeout,
		 " Watchdog timeout in milliseconds. First timeout causes a poke, \n"
		 "\t\tthe second causes an NMI, and the third causes a soft reset.");

/**
 * Poke the watchdog when an interrupt is received
 *
 * @param cpl
 * @param dev_id
 * @param regs
 *
 * @return
 */
static irqreturn_t watchdog_poke_irq(int cpl, void *dev_id)
{
	/* We're alive, poke the watchdog */
	// printk("Poking watchdog on core %d\n", cvmx_get_core_num());
	cvmx_write_csr(CVMX_CIU_PP_POKEX(cvmx_get_core_num()), 1);
	return IRQ_HANDLED;
}


/**
 * In the NMI handler we assume everything is broken. This
 * routine is used to send a raw byte out the serial port
 * with as minimal dependencies as possible.
 *
 * @param uart_index Uart to write to (0 or 1)
 * @param ch         Byte to write
 */
static inline void uart_write_byte(int uart_index, uint8_t ch)
{
	cvmx_uart_lsr_t lsrval;

	/* Spin until there is room */
	do {
		lsrval.u64 = cvmx_read_csr(CVMX_MIO_UARTX_LSR(uart_index));
	}
	while (lsrval.s.thre == 0);

	/* Write the byte */
	cvmx_write_csr(CVMX_MIO_UARTX_THR(uart_index), ch);
}


/**
 * Write a string to the uart
 *
 * @param uart_index Uart to use (0 or 1)
 * @param str        String to write
 */
static void uart_write_string(int uart_index, const char *str)
{
	/* Just loop writing one byte at a time */
	while (*str)
		uart_write_byte(uart_index, *str++);
}


/**
 * Write a hex number out of the uart
 *
 * @param uart_index Uart to use (0 or 1)
 * @param value      Number to display
 * @param digits     Number of digits to print (1 to 16)
 */
static void uart_write_hex(int uart_index, uint64_t value, int digits)
{
	int d;
	int v;
	for (d = 0; d < digits; d++) {
		v = (value >> ((digits - d - 1) * 4)) & 0xf;
		if (v >= 10)
			uart_write_byte(0, 'a' + v - 10);
		else
			uart_write_byte(0, '0' + v);
	}
}


/**
 * NMI stage 3 handler. NMIs are handled in the following manner:
 * 1) The first NMI handler enables CVMSEG and transfers from
 * the bootbus region into normal memory. It is careful to not
 * destroy any registers.
 * 2) The second stage handler uses CVMSEG to save the registers
 * and create a stack for C code. It then calls the third level
 * handler with one argument, a pointer to the register values.
 * 3) The third, and final, level handler is the following C
 * function that prints out some useful infomration.
 *
 * @param reg    Pointer to register state before the NMI
 */
void octeon_watchdog_nmi_stage3(uint64_t reg[32])
{
	const char *reg_name[] =
		{ "$0", "at", "v0", "v1", "a0", "a1", "a2", "a3",
		"a4", "a5", "a6", "a7", "t0", "t1", "t2", "t3",
		"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
		"t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"
	};
	uint64_t i;
	/* Save status and cause early to get them before any changes might
	   happen */
	uint64_t cp0_cause = read_c0_cause();
	uint64_t cp0_status = read_c0_status();

	/* Delay so all cores output is jumbled */
	cvmx_wait(100000000ull * cvmx_get_core_num());

	uart_write_string(0, "\r\n*** NMI Watchdog interrupt on Core 0x");
	uart_write_hex(0, cvmx_get_core_num(), 1);
	uart_write_string(0, " ***\r\n");
	for (i = 0; i < 32; i++) {
		uart_write_string(0, "\t");
		uart_write_string(0, reg_name[i]);
		uart_write_string(0, "\t0x");
		uart_write_hex(0, reg[i], 16);
		if (i & 1)
			uart_write_string(0, "\r\n");
	}
	uart_write_string(0, "\tepc\t0x");
	uart_write_hex(0, read_c0_epc(), 16);
	uart_write_string(0, "\r\n");

	uart_write_string(0, "\tstatus\t0x");
	uart_write_hex(0, cp0_status, 16);
	uart_write_string(0, "\tcause\t0x");
	uart_write_hex(0, cp0_cause, 16);
	uart_write_string(0, "\r\n");

	uart_write_string(0, "\tsum0\t0x");
	uart_write_hex(0,
		       cvmx_read_csr(CVMX_CIU_INTX_SUM0
				     (cvmx_get_core_num() * 2)), 16);
	uart_write_string(0, "\ten0\t0x");
	uart_write_hex(0,
		       cvmx_read_csr(CVMX_CIU_INTX_EN0
				     (cvmx_get_core_num() * 2)), 16);
	uart_write_string(0, "\r\n");
#if 0
	uart_write_string(0, "Code around epc\r\n");
	clear_c0_status(0x00400004);
	for (i = read_c0_epc() - 16; i < read_c0_epc() + 20; i += 4) {
		uart_write_string(0, "\t0x");
		uart_write_hex(0, i, 16);
		uart_write_string(0, "\t");
		uart_write_hex(0, *(uint32_t *) i, 16);
		uart_write_string(0, "\r\n");
	}
#endif
#if 0
	uart_write_string(0, "TLB\r\n");
	for (i = 0; i < 64; i++) {
		write_c0_index(i);
		asm volatile ("tlbr");
		uart_write_string(0, "\tHi 0x");
		uart_write_hex(0, read_c0_entryhi(), 16);
		uart_write_string(0, "\tPageMask 0x");
		uart_write_hex(0, read_c0_pagemask(), 16);
		uart_write_string(0, "\tLo0 0x");
		uart_write_hex(0, read_c0_entrylo0(), 16);
		uart_write_string(0, "\tLo1 0x");
		uart_write_hex(0, read_c0_entrylo1(), 16);
		uart_write_string(0, "\r\n");
	}
#endif
	uart_write_string(0, "*** Chip soft reset soon ***\r\n");
}


static void setup_interrupt(void *arg)
{
	cvmx_ciu_wdogx_t ciu_wdog;
	int core = cvmx_get_core_num();
	unsigned long threshold = (unsigned long) arg;

	request_irq(OCTEON_IRQ_WDOG0 + core, watchdog_poke_irq, SA_SHIRQ,
		    "watchdog", watchdog_poke_irq);

	/* Poke the watchdog to clear out its state */
	cvmx_write_csr(CVMX_CIU_PP_POKEX(core), 1);

	/* Finally enable the watchdog now that all handlers are installed */
	ciu_wdog.u64 = 0;
	ciu_wdog.s.len = threshold;
	ciu_wdog.s.mode = 3;	/* 3 = Interrupt + NMI + Soft-Reset */
	cvmx_write_csr(CVMX_CIU_WDOGX(core), ciu_wdog.u64);
}

/**
 * Module/ driver initialization.
 *
 * @return Zero on success
 */
static int __init watchdog_init(void)
{
	uint64_t threshold;
	int i;
	extern void octeon_watchdog_nmi(void);

	/* Watchdog time expiration length = The 16 bits of LEN represent the
	   most significant bits of a 24 bit decrementer that decrements every
	   256 cycles. */
	if (timeout)
		threshold = timeout * octeon_get_clock_rate() / 1000 >> (8 + 8);
	else {
		threshold = 65535;
		timeout =
			(65535ull << (8 + 8)) * 1000 / octeon_get_clock_rate();
	}
	if ((threshold < 10) || (threshold >= 65536)) {
		printk("Illegal watchdog timeout. Timeout must be between %u and %u.\n", (int) ((10ull << (8 + 8)) * 1000 / octeon_get_clock_rate()), (int) ((65535ull << (8 + 8)) * 1000 / octeon_get_clock_rate()));
		return -1;
	}

	/* Install the NMI handler */
	for (i = 0; i < 16; i++) {
		uint64_t *ptr = (uint64_t *) octeon_watchdog_nmi;
		cvmx_write_csr(CVMX_MIO_BOOT_LOC_ADR, i * 8);
		cvmx_write_csr(CVMX_MIO_BOOT_LOC_DAT, ptr[i]);
	}
	cvmx_write_csr(CVMX_MIO_BOOT_LOC_CFGX(0), 0x81fc0000);

	on_each_cpu(setup_interrupt, (void *) threshold, 1, 0);
	printk("Octeon watchdog driver loaded with a timeout of %d ms.\n",
	       timeout);
	return 0;
}


/**
 * Module / driver shutdown
 */
static void __exit watchdog_cleanup(void)
{
	int cpu;
	preempt_disable();
	for (cpu = 0; cpu < NR_CPUS; cpu++) {
		if (cpu_online(cpu)) {
#ifdef CONFIG_SMP
			int core = cpu_logical_map(cpu);
#else
			int core = cvmx_get_core_num();
#endif
			/* Disable the watchdog */
			cvmx_write_csr(CVMX_CIU_WDOGX(core), 0);
			/* Free the interrupt handler */
			free_irq(OCTEON_IRQ_WDOG0 + core, watchdog_poke_irq);
		}
	}
	preempt_enable();

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cavium Networks <support@caviumnetworks.com>");
MODULE_DESCRIPTION("Cavium Networks Octeon Watchdog driver.");
module_init(watchdog_init);
module_exit(watchdog_cleanup);
