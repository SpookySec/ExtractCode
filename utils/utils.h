/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  a small file for text extract functions
 *
 *        Version:  1.0
 *        Created:  12/06/2020 02:33:45 PM
 *       Compiler:  gcc
 *
 *         Author:  SPOOKY (@spooky_sec), https://discord.gg/UTVBvJs
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <elf.h>
#include <malloc.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>

#include "./msg.h"
#include "./hexdump.h"

char *read_section64(int fd, Elf64_Shdr sh) {
	char *buff = (char*)malloc(sh.sh_size);
	if(!buff) {
		printf("%s:Failed to allocate %ldbytes\n",
				__func__, sh.sh_size);
	}

	assert(buff != NULL);
	assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
	assert(read(fd, (void *)buff, sh.sh_size) == sh.sh_size);

	return buff;
}

void save_text_section64(int fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[], int debug) {
	uint i;
	int fd2;
	char* sh_str;
	char* buf;

	char *pwd = getcwd(NULL, (size_t)NULL);
	pwd = (char*)realloc(pwd, strlen(pwd)+8);

	// don't use strcat...
	strcat(pwd,"/text.s");
	printf(SUCCESS "Saving to: %s\n", pwd);

	sh_str = read_section64(fd, sh_table[eh.e_shstrndx]);

	for(i=0; i<eh.e_shnum; i++) {
		if(!strcmp(".text", (sh_str + sh_table[i].sh_name))) {
		    if (debug) {
				printf("\n");
				printf(SUCCESS "Found section\t\".text\"\n");
				printf(SUCCESS "At offset\t\t0x%08lX\n", sh_table[i].sh_offset);
				printf(SUCCESS "Of size\t\t0x%08lX\n", sh_table[i].sh_size);
				printf("\n");
			}

			break;
		}
	}

	assert(lseek(fd, sh_table[i].sh_offset, SEEK_SET)==sh_table[i].sh_offset);
	buf = (char*)malloc(sh_table[i].sh_size);
	if(!buf) {
		printf("Failed to allocate %ldbytes\n", sh_table[i].sh_size);

        // exit
        close(fd2);
        free(pwd);
        return;
	}
	assert(read(fd, buf, sh_table[i].sh_size)==sh_table[i].sh_size);
	fd2 = open(pwd, O_RDWR|O_SYNC|O_CREAT, 0644);
	write(fd2, buf, sh_table[i].sh_size);

	if (debug) {
		printf(SUCCESS "Buffer Size: %ld\n", sh_table[i].sh_size);
		printf(SUCCESS "Hex Dump: \n\n");
		
		// Custom function made by rip meep
		hexdump(buf, sh_table[i].sh_size, 16, 1, 2);
	}

	fsync(fd2);
}

void read_section_header_table64(int fd, Elf64_Ehdr eh, Elf64_Shdr sh_table[]) {
	int i;

	assert(lseek(fd, (off_t)eh.e_shoff, SEEK_SET) == (off_t)eh.e_shoff);

	for(i=0; i<eh.e_shnum; i++) {
		assert(read(fd, (void *)&sh_table[i], eh.e_shentsize)
				== eh.e_shentsize);
	}
}

void read_elf_header64(int fd, Elf64_Ehdr *elf_header) {
	assert(elf_header != NULL);
	assert(lseek(fd, (off_t)0, SEEK_SET) == (off_t)0);
	assert(read(fd, (void *)elf_header, sizeof(Elf64_Ehdr)) == sizeof(Elf64_Ehdr));
}


// optional section dump function for debugging (not used in the file)
void print_section_headers64(int fd, Elf64_Ehdr eh, Elf64_Shdr *sh_table) {
	int i;
	char* sh_str;

	sh_str = read_section64(fd, sh_table[eh.e_shstrndx]);

	printf("========================================");
	printf("========================================\n");
	printf(" idx offset     load-addr  size       algn"
			" flags      type       section\n");
	printf("========================================");
	printf("========================================\n");

	for(i=0; i<eh.e_shnum; i++) {
		printf(" %03d ", i);
		printf("0x%08lX ", sh_table[i].sh_offset);
		printf("0x%08lX ", sh_table[i].sh_addr);
		printf("0x%08lX ", sh_table[i].sh_size);
		printf("%4ld ", sh_table[i].sh_addralign);
		printf("0x%08lX ", sh_table[i].sh_flags);
		printf("0x%08X ", sh_table[i].sh_type);
		printf("%s\t", (sh_str + sh_table[i].sh_name));
		printf("\n");
	}
	printf("========================================");
	printf("========================================\n");
	printf("\n");
}
