/*
 *   Octeon Bootbus flash setup
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2007 Cavium Networks
 */
#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include "hal.h"

static struct map_info flash_map;
static struct mtd_info *mymtd;
#ifdef CONFIG_MTD_PARTITIONS
static int nr_parts;
static struct mtd_partition *parts;
static const char *part_probe_types[] = {
	"cmdlinepart",
#ifdef CONFIG_MTD_REDBOOT_PARTS
	"RedBoot",
#endif
	NULL
};
#endif

/**
 * Module/ driver initialization.
 *
 * @return Zero on success
 */
static int __init flash_init(void)
{
	/* Read the bootbus region 0 setup to determine where the base of flash
	   is set for */
	cvmx_mio_boot_reg_cfgx_t region_cfg;
	region_cfg.u64 = cvmx_read_csr(CVMX_MIO_BOOT_REG_CFG0);
	if (region_cfg.s.en) {
		/* The bootloader always takes the flash and sets its address
		   so the entire flash fits below 0x1fc00000. This way the
		   flash aliases to 0x1fc00000 for booting. Software can access
		   the full flash at the true address, while core boot can
		   access 4MB */
		flash_map.name = "phys_mapped_flash";	/* Use this name so old
							   part lines work */
		flash_map.phys = region_cfg.s.base << 16;
		flash_map.size = 0x1fc00000 - flash_map.phys;
		flash_map.bankwidth = 1;
		flash_map.virt = ioremap(flash_map.phys, flash_map.size);
		printk("Bootbus flash: Setting flash for %luMB flash at 0x%08lx\n", flash_map.size >> 20, flash_map.phys);
		simple_map_init(&flash_map);
		mymtd = do_map_probe("cfi_probe", &flash_map);
		if (mymtd) {
			mymtd->owner = THIS_MODULE;

#ifdef CONFIG_MTD_PARTITIONS
			nr_parts =
				parse_mtd_partitions(mymtd, part_probe_types,
						     &parts, 0);
			if (nr_parts > 0) {
				add_mtd_partitions(mymtd, parts, nr_parts);
			} else
				add_mtd_device(mymtd);
#else
			add_mtd_device(mymtd);
#endif
		} else
			printk("Failed to register MTD device for flash\n");
	}
	return 0;
}

late_initcall(flash_init);
