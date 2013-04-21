/*
 * Copyright 2004-2012 Analog Devices Inc.
 * Licensed under the ADI BSD license.
 *   https://docs.blackfin.uclinux.org/doku.php?id=adi_bsd
 */

/* This file should be up to date with:
 *  - Revision A, 15/06/2012; ADSP-BF609 Blackfin Processor Anomaly List
 */

#if __SILICON_REVISION__ < 0
# error will not work on BF609 silicon version
#endif

#ifndef _MACH_ANOMALY_H_
#define _MACH_ANOMALY_H_

/* TRU_STAT.ADDRERR and TRU_ERRADDR.ADDR May Not Reflect the Correct Status */
#define ANOMALY_16000003 (1)
/* The EPPI Data Enable (DEN) Signal is Not Functional */
#define ANOMALY_16000004 (1)
/* Using L1 Instruction Cache with Parity Enabled is Unreliable */
#define ANOMALY_16000005 (1)
/* SEQSTAT.SYSNMI Clears Upon Entering the NMI ISR */
#define ANOMALY_16000006 (1)
/* DDR2 Memory Reads May Fail Intermittently */
#define ANOMALY_16000007 (1)
/* Instruction Memory Stalls Can Cause IFLUSH to Fail */
#define ANOMALY_16000008 (1)
/* TestSET Instruction Cannot Be Interrupted */
#define ANOMALY_16000009 (1)
/* IFLUSH Instruction at End of Hardware Loop Causes Infinite Stall */
#define ANOMALY_16000010 (1)
/* False Hardware Error when RETI Points to Invalid Memory */
#define ANOMALY_16000011 (1)
/* Speculative Fetches of Indirect-Pointer Inst Can Cause False Hw Errors */
#define ANOMALY_16000012 (1)
/* False Hardware Errors Caused by Fetches at the Boundary of Reserved Memory */
#define ANOMALY_16000013 (1)
/* False Hardware Error from an Access in the Shadow of a Conditional Branch */
#define ANOMALY_16000014 (1)
/* Multi-Issue Inst with dsp32shiftimm in slot1 and P in slot2 Not Supported */
#define ANOMALY_16000015 (1)
/* Speculative Fetches Can Cause Undesired External FIFO Operations */
#define ANOMALY_16000017 (1)
/* RSI Boot Cleanup Routine Does Not Clear Registers */
#define ANOMALY_16000018 (1)
/* SPI Master Boot Device Auto-detection Frequency is Set Incorrectly */
#define ANOMALY_16000019 (1)
/* rom_SysControl() Fails to Set DDR0_CTL.INIT for Wakeup From Hibernate */
#define ANOMALY_16000020 (1)
/* rom_SysControl() Fails to Save and Restore DDR0_PHYCTL3 for Hb/Wk Sequence */
#define ANOMALY_16000021 (1)
/* Boot Code Fails to Enable Parity Fault Detection */
#define ANOMALY_16000022 (1)
/* USB DMA interrupt status do not show the DMA channel intr in the DMA ISR */
#define ANOMALY_16000027 (1)
/* Interrupted Core Reads of MMRs May Cause Data Loss */
#define ANOMALY_16000030 (1)

/* Anomalies that don't exist on this proc */
#define ANOMALY_05000158 (0)
#define ANOMALY_05000189 (0)
#define ANOMALY_05000198 (0)
#define ANOMALY_05000219 (0)
#define ANOMALY_05000230 (0)
#define ANOMALY_05000231 (0)
#define ANOMALY_05000244 (0)
#define ANOMALY_05000261 (0)
#define ANOMALY_05000263 (0)
#define ANOMALY_05000273 (0)
#define ANOMALY_05000274 (0)
#define ANOMALY_05000278 (0)
#define ANOMALY_05000281 (0)
#define ANOMALY_05000287 (0)
#define ANOMALY_05000311 (0)
#define ANOMALY_05000312 (0)
#define ANOMALY_05000323 (0)
#define ANOMALY_05000353 (1)
#define ANOMALY_05000363 (0)
#define ANOMALY_05000386 (0)
#define ANOMALY_05000480 (0)
#define ANOMALY_05000481 (1)

/* Reuse BF5xx anomalies IDs for the same anomaly in BF60x */
#define ANOMALY_05000491 ANOMALY_16000008
#define ANOMALY_05000477 ANOMALY_16000009
#define ANOMALY_05000443 ANOMALY_16000010
#define ANOMALY_05000461 ANOMALY_16000011
#define ANOMALY_05000426 ANOMALY_16000012
#define ANOMALY_05000310 ANOMALY_16000013
#define ANOMALY_05000245 ANOMALY_16000014
#define ANOMALY_05000074 ANOMALY_16000015
#define ANOMALY_05000416 ANOMALY_16000017


#endif
