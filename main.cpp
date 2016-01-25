#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "script.h"
#include "device.h"
#include "crc32.h"

void hexdump(char *desc, void *addr, int len)
{
	int i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char *)addr;

	// Output description if given.
	if (desc != NULL)
		printf("%s:\r\n", desc);

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				printf("  %s\r\n", buff);

			// Output the offset.
			printf("  %04x ", i);
		}
		// Now the hex code for the specific character.
		printf(" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		printf("   ");
		i++;
	}

	// And print the final ASCII bit.
	printf("  %s\r\n", buff);
}

static uint8_t hdr[16] = { 'N','E','S',0x1A,0,0,0,0,0,0,0,0,0,0,0,0 };
static int prgbanksize, chrbanksize;
static uint8_t prg[0x10000 * 16];
static uint8_t chr[0x10000 * 16];
static int prgsize, chrsize, mapper, submapper;

//read cpu data to buffer
void dump_start(int pbs,int cbs, int map)
{
	prgbanksize = pbs;
	chrbanksize = cbs;
	mapper = map;
	submapper = 0;
	prgsize = 0;
	chrsize = 0;
}

void set_prg_bank_size(int s)
{
	prgbanksize = s;
}

void set_chr_bank_size(int s)
{
	chrbanksize = s;
}

void set_mapper(int s)
{
	mapper = s;
}

void set_submapper(int s)
{
	submapper = s;
}

void cpu_write(uint32_t addr, uint8_t data)
{
	device_write_cpu(addr, data);
}

void cpu_read(uint32_t addr, uint32_t len)
{
	if (prgsize == 0) {
		printf("\ndumping prg");
	}
	else {
		printf(".");
	}
	device_read_cpu(addr, len, prg + prgsize);
	prgsize += len;
}

void ppu_read(uint32_t addr, uint32_t len)
{
	if (chrsize == 0) {
		printf("\ndumping chr");
	}
	else {
		printf(".");
	}
	device_read_ppu(addr, len, chr + chrsize);
	chrsize += len;
}

void save_dump(char *fn)
{
	FILE *fp;
	char filename[1024];

	hdr[4] = prgsize / 16384;
	hdr[5] = chrsize / 8192;
	hdr[6] = (mapper << 4);
	hdr[7] = (mapper & 0xF0);

	sprintf(filename, "%s.prg", fn);
	if ((fp = fopen(filename, "wb")) == 0) {
		printf("error opening '%s'\n", filename);
	}
	else {
		fwrite(prg, prgsize, 1, fp);
		fclose(fp);
	}

	sprintf(filename, "%s.chr", fn);
	if ((fp = fopen(filename, "wb")) == 0) {
		printf("error opening '%s'\n", filename);
	}
	else {
		fwrite(chr, chrsize, 1, fp);
		fclose(fp);
	}

	if ((fp = fopen(fn, "wb")) == 0) {
		printf("error opening '%s'\n", fn);
	}
	else {
		fwrite(hdr, 16, 1, fp);
		fwrite(prg, prgsize, 1, fp);
		fwrite(chr, chrsize, 1, fp);
		fclose(fp);
	}
}

void check_mirror()
{
	int i;

	i = prgsize / 2;
	while (memcmp(prg, prg + i, i) == 0) {
		printf("prg was mirrored, truncating (%d bytes -> %d bytes).\n", prgsize, i);
		prgsize -= i;
		i = prgsize / 2;
	}
}


int main(int argc, char *argv[])
{
	char *scriptfn = "mmc1_slrom.lua";
	char *outputfn = "dump.nes";

	if (argc < 3) {
		printf("usage: dumper-cli script.lua output.nes\n");
		return(1);
	}
	crc32_gentab();
		
	scriptfn = argv[1];
	outputfn = argv[2];

	prgbanksize = 0x4000;
	chrbanksize = 0x2000;
	prgsize = chrsize = 0;

	if (device_open() == false) {
		printf("error opening device\n");
		return(-1);
	}
	printf("device opened\n");

	if (script_init() == false) {
		printf("error initializing lua\n");
		device_close();
		return(-1);
	}

	if (script_load(scriptfn) == false) {
		printf("error loading script\n");
		script_kill();
		device_close();
		return(-1);
	}

	prgsize = 0;
	chrsize = 0;

	script_setup();
	script_dump();

	printf("\n\n");
	printf("read %d prg bytes (crc32 = $%08X)\n", prgsize, crc32(prg, prgsize));
	if (chrsize) {
		printf("read %d chr bytes (crc32 = $%08X)\n", chrsize, crc32(chr, chrsize));
	}
	else {
		printf("no chr read\n");
	}

	printf("checking for mirrored rom...\n");
	check_mirror();

	printf("writing prg/chr/nes files...\n");
	save_dump(outputfn);

	script_kill();
	device_close();
	return(0);
}
