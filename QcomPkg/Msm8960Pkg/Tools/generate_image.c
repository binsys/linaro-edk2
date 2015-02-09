/** @file
 The data structures in this code come from:
 OMAP35x Applications Processor Technical Reference Manual chapter 25
 OMAP34xx Multimedia Device Technical Reference Manual chapter 26.4.8.

 You should use the OMAP35x manual when possible. Some things, like SectionKey,
 are not defined in the OMAP35x manual and you have to use the OMAP34xx manual
 to find the data.

 Copyright (c) 2008 - 2010, Apple Inc. All rights reserved.<BR>

 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>

#ifdef _WIN32  
#ifndef _UNISTD_H  
#define _UNISTD_H  
#include <io.h>  
#include <process.h>  
#endif /* _UNISTD_H */ 
#else  
#include <unistd.h>
#endif  

typedef struct boot_img_hdr boot_img_hdr;

#define BOOT_MAGIC "ANDROID!"
#define BOOT_MAGIC_SIZE 8
#define BOOT_NAME_SIZE 16
#define BOOT_ARGS_SIZE 512
//#define BOOT_EXTRA_ARGS_SIZE 1024

struct boot_img_hdr
{
	unsigned char magic[BOOT_MAGIC_SIZE];

	unsigned kernel_size;  /* size in bytes */
	unsigned kernel_addr;  /* physical load addr */

	unsigned ramdisk_size; /* size in bytes */
	unsigned ramdisk_addr; /* physical load addr */

	unsigned second_size;  /* size in bytes */
	unsigned second_addr;  /* physical load addr */

	unsigned tags_addr;    /* physical addr for kernel tags */
	unsigned page_size;    /* flash page size we assume */
	unsigned dt_size;      /* device tree in bytes */
	unsigned unused;       /* future expansion: should be 0 */
	unsigned char name[BOOT_NAME_SIZE]; /* asciiz product name */

	unsigned char cmdline[BOOT_ARGS_SIZE];

	unsigned id[8]; /* timestamp / checksum / sha1 / etc */

	/* Supplemental command line data; kept here to maintain
	* binary compatibility with older versions of mkbootimg */
	//unsigned char extra_cmdline[BOOT_EXTRA_ARGS_SIZE];
};

/*
** +-----------------+
** | boot header     | 1 page
** +-----------------+
** | kernel          | n pages
** +-----------------+
** | ramdisk         | m pages
** +-----------------+
** | second stage    | o pages
** +-----------------+
** | device tree     | p pages
** +-----------------+
**
** n = (kernel_size + page_size - 1) / page_size
** m = (ramdisk_size + page_size - 1) / page_size
** o = (second_size + page_size - 1) / page_size
** p = (dt_size + page_size - 1) / page_size
**
** 0. all entities are page_size aligned in flash
** 1. kernel and ramdisk are required (size != 0)
** 2. second is optional (second_size == 0 -> no second)
** 3. load each element (kernel, ramdisk, second) at
**    the specified physical address (kernel_addr, etc)
** 4. prepare tags at tag_addr.  kernel_args[] is
**    appended to the kernel commandline in the tags.
** 5. r0 = 0, r1 = MACHINE_TYPE, r2 = tags_addr
** 6. if second_size != 0: jump to second_addr
**    else: jump to kernel_addr
*/

static void *load_file(const char *fn, unsigned *_sz)
{
	char *data;
	int sz;
	//int fd;
	FILE *fp;

	data = 0;
	fp = fopen(fn, "r+");

	if (fp == NULL) return 0;


	sz = fseek(fp, 0, SEEK_END);
	sz = ftell(fp);


	if (sz < 0) goto oops;

	//fseek(fp, 0, SEEK_SET);
	fseek(fp, 0, 0);

	data = (char*)malloc(sz);

	if (data == NULL) 
	{
		fprintf(stderr, "malloc error\n");
		goto oops;
	}
		

	fread(data, sz, 1, fp);
	//if (fread(data, sz, 1, fp))
	//{
	//	fprintf(stderr, "fread error %s\n", strerror(errno));
	//	goto oops;
	//}

	fclose(fp);


	if (_sz) *_sz = sz;

	return data;

oops:

	fclose(fp);


	return 0;
}

int usage(void)
{
	fprintf(stderr, "usage: mkbootimg\n"
		"       --kernel <filename>\n"
		"       --ramdisk <filename>\n"
		"       [ --second <2ndbootloader-filename> ]\n"
		"       [ --cmdline <kernel-commandline> ]\n"
		"       [ --board <boardname> ]\n"
		"       [ --base <address> ]\n"
		"       [ --pagesize <pagesize> ]\n"
		"       [ --dt <filename> ]\n"
		"       -o|--output <filename>\n"
		);
	return 1;
}



static unsigned char padding[131072] = { 0, };

int write_padding(FILE * fp, unsigned pagesize, unsigned itemsize)
{
	unsigned pagemask = pagesize - 1;
	size_t count;

	if ((itemsize & pagemask) == 0) {
		return 0;
	}

	count = pagesize - (itemsize & pagemask);

	if (fwrite(padding, count, 1, fp) != 1) {
		return -1;
	}
	else {
		return 0;
	}
}

int main(int argc, char **argv)
{
	boot_img_hdr hdr;

	char *kernel_fn = 0;
	void *kernel_data = 0;
	char *ramdisk_fn = 0;
	void *ramdisk_data = 0;
	char *second_fn = 0;
	void *second_data = 0;
	char *cmdline = "";
	char *bootimg = 0;
	char *board = "";
	char *dt_fn = 0;
	void *dt_data = 0;
	unsigned pagesize = 2048;
	FILE * fp;

	unsigned base           = 0x10000000;
	unsigned tags_offset    = 0x00000100;
	unsigned kernel_offset  = 0x00008000;
	unsigned second_offset  = 0x00f00000;
	unsigned ramdisk_offset = 0x01000000;
	
	size_t cmdlen;

	argc--;
	argv++;

	memset(&hdr, 0, sizeof(hdr));

	while (argc > 0){
		char *arg = argv[0];
		char *val = argv[1];
		if (argc < 2) {
			return usage();
		}
		argc -= 2;
		argv += 2;
		if (!strcmp(arg, "--output") || !strcmp(arg, "-o")) {
			bootimg = val;
		}
		else if (!strcmp(arg, "--kernel")) {
			kernel_fn = val;
		}
		else if (!strcmp(arg, "--ramdisk")) {
			ramdisk_fn = val;
		}
		else if (!strcmp(arg, "--second")) {
			second_fn = val;
		}
		else if (!strcmp(arg, "--cmdline")) {
			cmdline = val;
		}
		else if (!strcmp(arg, "--base")) {
			base = strtoul(val, 0, 16);
		}
		else if (!strcmp(arg, "--kernel_offset")) {
			kernel_offset = strtoul(val, 0, 16);
		}
		else if (!strcmp(arg, "--ramdisk_offset")) {
			ramdisk_offset = strtoul(val, 0, 16);
		}
		else if (!strcmp(arg, "--second_offset")) {
			second_offset = strtoul(val, 0, 16);
		}
		else if (!strcmp(arg, "--tags_offset")) {
			tags_offset = strtoul(val, 0, 16);
		}
		else if (!strcmp(arg, "--board")) {
			board = val;
		}
		else if (!strcmp(arg, "--pagesize")) {
			pagesize = strtoul(val, 0, 10);
			if ((pagesize != 2048) && (pagesize != 4096)
				&& (pagesize != 8192) && (pagesize != 16384)
				&& (pagesize != 32768) && (pagesize != 65536)
				&& (pagesize != 131072)) {
				fprintf(stderr, "error: unsupported page size %d\n", pagesize);
				return -1;
			}
		}
		else if (!strcmp(arg, "--dt")) {
			dt_fn = val;
		}
		else {
			return usage();
		}
	}
	hdr.page_size = pagesize;

	hdr.kernel_addr = base + kernel_offset;
	hdr.ramdisk_addr = base + ramdisk_offset;
	hdr.second_addr = base + second_offset;
	hdr.tags_addr = base + tags_offset;

	if (bootimg == 0) {
		fprintf(stderr, "error: no output filename specified\n");
		return usage();
	}

	if (kernel_fn == 0) {
		fprintf(stderr, "error: no kernel image specified\n");
		return usage();
	}

	if (ramdisk_fn == 0) {
		fprintf(stderr, "error: no ramdisk image specified\n");
		return usage();
	}

	if (strlen(board) >= BOOT_NAME_SIZE) {
		fprintf(stderr, "error: board name too large\n");
		return usage();
	}

	strcpy((char *)hdr.name, board);

	memcpy(hdr.magic, BOOT_MAGIC, BOOT_MAGIC_SIZE);

	cmdlen = strlen(cmdline);
	if (cmdlen > (BOOT_ARGS_SIZE - 2)) {
		fprintf(stderr, "error: kernel commandline too large\n");
		return 1;
	}
	/* Even if we need to use the supplemental field, ensure we
	* are still NULL-terminated */
	strncpy((char *)hdr.cmdline, cmdline, BOOT_ARGS_SIZE - 1);
	hdr.cmdline[BOOT_ARGS_SIZE - 1] = '\0';


	kernel_data = load_file(kernel_fn, &hdr.kernel_size);
	if (kernel_data == 0) {
		fprintf(stderr, "error: could not load kernel '%s'\n", kernel_fn);
		return 1;
	}

	if (!strcmp(ramdisk_fn, "NONE")) {
		ramdisk_data = 0;
		hdr.ramdisk_size = 0;
	}
	else {
		ramdisk_data = load_file(ramdisk_fn, &hdr.ramdisk_size);
		if (ramdisk_data == 0) {
			fprintf(stderr, "error: could not load ramdisk '%s'\n", ramdisk_fn);
			return 1;
		}
	}

	if (second_fn) {
		second_data = load_file(second_fn, &hdr.second_size);
		if (second_data == 0) {
			fprintf(stderr, "error: could not load secondstage '%s'\n", second_fn);
			return 1;
		}
	}

	if (dt_fn) {
		dt_data = load_file(dt_fn, &hdr.dt_size);
		if (dt_data == 0) {
			fprintf(stderr, "error: could not load device tree image '%s'\n", dt_fn);
			return 1;
		}
	}


	int aa = 0;

	fp = fopen(bootimg, "wb+");
	if (fp == NULL) {
		fprintf(stderr, "error: could not create '%s'\n", bootimg);
		return 1;
	}


	aa = fwrite(&hdr, sizeof(hdr), 1, fp);

	if (aa != 1) goto fail;
	

	if (write_padding(fp, pagesize, sizeof(hdr))) goto fail;

	aa = fwrite(kernel_data, hdr.kernel_size, 1, fp);

	if (aa != 1) goto fail;

	if (write_padding(fp, pagesize, hdr.kernel_size)) goto fail;

	if (fwrite( ramdisk_data, hdr.ramdisk_size, 1, fp) != 1) goto fail;

	if (write_padding(fp, pagesize, hdr.ramdisk_size)) goto fail;

	if (second_data) {
		if (fwrite(second_data, hdr.second_size, 1, fp) != 1) goto fail;

		if (write_padding(fp, pagesize, hdr.second_size)) goto fail;
	}

	if (dt_data) {
		if (fwrite(dt_data, hdr.dt_size, 1, fp) != 1) goto fail;
		if (write_padding(fp, pagesize, hdr.dt_size)) goto fail;
	}

	return 0;

fail:
	//unlink(bootimg);
	fclose(fp);
	fprintf(stderr, "error: failed writing '%s': %s\n", bootimg,
		strerror(errno));
	return 1;
}