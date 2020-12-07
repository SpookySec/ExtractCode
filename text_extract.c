/*
 * =====================================================================================
 *
 *       Filename:  text_extract.c
 *
 *    Description:  a small program to extract .text sections from ELF binaries
 *
 *        Version:  1.0
 *        Created:  12/05/2020 01:26:17 PM
 *       Compiler:  gcc
 *
 *         Author:  SPOOKY (@spooky_sec), https://discord.gg/UTVBvJs
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*#define DEBUG*/

#include "./utils/utils.h"
#include "./utils/msg.h"
#include "./utils/is-elf.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf(WARN "Usage: %s <ELF file>\n", argv[0]);
        exit(0);
    } 

    int debug = 0;
    char *file = argv[1];
    int opt; 

    // check if the file exists first... seg faults bad >:[
    if (access(file, F_OK)) {
        printf(ERROR "File %s not found\n", file);
        exit(1);
    }

    int fd = open(file, O_RDONLY);
    Elf64_Ehdr eh64;
    read_elf_header64(fd, &eh64);

    printf(SUCCESS "MADE BY: @spooky_sec\n");


    if (is_ELF64(eh64)) {

        if(debug) {
            printf(SUCCESS "Looks like an ELF file!\n");
        }

        Elf64_Shdr *sh_tbl;


        sh_tbl = malloc(eh64.e_shentsize * eh64.e_shnum);

        if(!sh_tbl) {
                printf("Failed to allocate %d bytes\n",
                        (eh64.e_shentsize * eh64.e_shnum));
        }

        read_section_header_table64(fd, eh64, sh_tbl);

        read(fd, &eh64, sizeof eh64);

        save_text_section64(fd, eh64, sh_tbl, debug);

    } else {
        printf(ERROR "Doesn't look like an ELF file!\n");
    }
} 
