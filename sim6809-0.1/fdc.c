/* vim: set noexpandtab ai ts=4 sw=4 tw=4:
   fdc.c -- emulation of 1772 FDC
   Copyright (C) 2012 Gordon JC Pearce

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "fdc.h"
#include "emu6809.h"

/*
   quick technical rundown of the FDC in the Mirage
   DRQ is inverted and drives /IRQ
   INTRQ is inverted and drives /NMI
   So to read data we wait for an interrupt and loop until NMI
   At least, this is how the Mirage ROM does it
*/

static FILE *disk;
static tt_u8 *diskdata;
static long fdc_cycles;

int fdc_init() {
	diskdata = malloc(80*5632);	// 440k disk image
	if (!diskdata) {
		fprintf(stderr, "Not enough memory to allocate disk data\n");
		abort();
	}
	
	disk = fopen("disk.img", "r");
	if (disk) {
		fread(diskdata, sizeof(char), 5632*80, disk);
		fclose(disk);
	} else {
		fprintf(stderr, "warning: couldn't read in disk image\n");
	}
	return 0;
}

void fdc_destroy() {
 	// free memory used by disk
	if (diskdata) free(diskdata);
}

void fdc_run() {
	// called every cycle
	if ((fdc.sr & 0x01) == 0) return;   // nothing to do
	if (cycles < fdc_cycles) return; // not ready yet
	printf("fdc_run()\n");
	
	switch (fdc.cr & 0xf0) {
		case 0x00:  // restore
			printf("fdc: restore\n");
			fdc.trk_r = 0;
			fdc.sr = 0x04;  // track at 0
			nmi();
			return;
	}
	fdc.sr &= 0xfe;	// stop
	//exit(1);
}

tt_u8 fdc_rreg(int reg) {
	// handle reads from FDC registers
	tt_u8 val;

	switch (reg & 0x03) {   // not fully mapped
		case FDC_SR:
			val = fdc.sr;
			break;
		case FDC_TRACK:
			val = fdc.trk_r;
			break;
		case FDC_SECTOR:
			val =  fdc.sec_r;
			break;
		case FDC_DATA:
			fdc.sr &= 0xfd; // clear IRQ bit
			val =  fdc.data_r;
			break;
	}
	return val;
}

void fdc_wreg(int reg, tt_u8 val) {
	// handle writes to FDC registers
	int cmd = (val & 0xf0)>>4;
	
	switch (reg & 0x03) {  // not fully mapped
		case FDC_CR:
			if ((val & 0xf0) == 0xd0) { // force interrupt
				printf("cmd %02x: force interrupt\n", val);
				fdc.sr &= 0xfe; // clear busy bit
				return;
			}
			switch(cmd) {
				case 0: // restore
					printf("cmd %02x: restore\n", val);
					fdc_cycles = cycles + 1000000;   // slow
					fdc.sr |= 0x01; // busy
					break;
				default:
					printf("cmd %02x: unknown\n", val);
					break;
			}
		case FDC_TRACK:
			fdc.trk_r = val;
			break;
		case FDC_SECTOR:
			fdc.sec_r = val;
			break;
		case FDC_DATA:
			fdc.data_r = val;
			break;
	}
}
