int is_ELF64(Elf64_Ehdr eh) {
    if(!strncmp((char*)eh.e_ident, "\177ELF", 4)) {
        return 1;
    } else {
        return 0;
    }
}
