/*
 * (C) Copyright 2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef __PPC__
/*
 * At least on G2 PowerPC cores, sequential accesses to non-existent
 * memory must be synchronized.
 */
# include <asm/io.h>	/* for sync() */
#else
# define sync()		/* nothing */
#endif

/*
 * Check memory range for valid RAM. A simple memory test determines
 * the actually available RAM size between addresses `base' and
 * `base + maxsize'.
 */
unsigned long get_ram_size(unsigned long *base, unsigned long maxsize)
{
	volatile unsigned long *addr;
	unsigned long           save[32];
	unsigned long           cnt;
	unsigned long           val;
	unsigned long           size;
	int                     i = 0;

	for (cnt = (maxsize / sizeof (unsigned long)) >> 1; cnt > 0; cnt >>= 1) {
		addr = base + cnt;	/* pointer arith! */
		sync ();
		save[i++] = *addr;
		sync ();
		*addr = ~cnt;
	}

	addr = base;
	sync ();
	save[i] = *addr;
	sync ();
	*addr = 0;

	sync ();
	if ((val = *addr) != 0) {
		/* Restore the original data before leaving the function.
		 */
		sync ();
		*addr = save[i];
		for (cnt = 1; cnt < maxsize / sizeof(unsigned long); cnt <<= 1) {
			addr  = base + cnt;
			sync ();
			*addr = save[--i];
		}
		return (0);
	}

	for (cnt = 1; cnt < maxsize / sizeof (unsigned long); cnt <<= 1) {
		addr = base + cnt;	/* pointer arith! */
		val = *addr;
		*addr = save[--i];
		if (val != ~cnt) {
			size = cnt * sizeof (unsigned long);
			/* Restore the original data before leaving the function.
			 */
			for (cnt <<= 1; cnt < maxsize / sizeof (unsigned long); cnt <<= 1) {
				addr  = base + cnt;
				*addr = save[--i];
			}
			return (size);
		}
	}

	return (maxsize);
}

phys_size_t __weak get_effective_memsize(void)
{
#ifndef CONFIG_VERY_BIG_RAM
	return gd->ram_size;
#else
	/* limit stack to what we can reasonable map */
	return ((gd->ram_size > CONFIG_MAX_MEM_MAPPED) ?
		CONFIG_MAX_MEM_MAPPED : gd->ram_size);
#endif
}
