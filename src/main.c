// main.c - Course: System Programming
// author: Marin Markic
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "elfs.h"
#include "main.h"

extern void run(int argc, char **arvg);

// global variables
void *runAddr;
void *stack;
int argc;
char **argv;

void initErrStruct(){
	err.cannotOpen = "Error: File %s cannot be opened!\n\n\n";
	err.cannotAlloc = "Error: Memory allocation failure!\n\n\n";
	err.cannotRead = "Error: Cannot read file!\n\n\n";
	err.notElfFile = "Error: Input file is not in ELF format!\n\n\n";
	err.mapFailed = "Error: Cannot memory map ELF section!\n\n\n";
	err.munFailed = "Error: Cannot unmap memory section!\n\n\n";
}

// returns string from ID until NULL from String Table Section
char *getSectionNameFromTable(uint16_t id){
    char *res = (char*) malloc(10);
    int cnt = 0;

    while(1){
        res[cnt] = file.buffer[file.sectionStrTabOff + id++];
        if(res[cnt++] == '\0') return res;
    }
}

// returns string from ID until NULL from String Table Section
char *getSymbolNameFromTable(uint16_t id){
    char *res = (char*) malloc(50);
    int cnt = 0;

    while(1){
        res[cnt] = file.buffer[file.symbolStrTabOff + id++];
        if(res[cnt++] == '\0') return res;
    }
}

// parse input
int load()
{
    argc = 0;
	argv = 0;

    // load all until end of line
	char *s = malloc(100);
	argv = (char **) malloc (20 * sizeof(char *));

	printf ("Enter name of elf file and parameters or exit.\n");
	scanf(" %[^\n]", s);

    argv[argc] = strtok(s, " ");

    argv[argc] = s;
	file.name = s;
    if ( !strcmp(argv[argc],"exit") ) return 0;

    do{ argv[++argc] = strtok (NULL, " "); } while (argv[argc] != NULL);

	return 1;
}

int main()
{
	initErrStruct();

	while(1)
	{
		// variables
		unsigned int cnt = 0;
		unsigned long shoff = 0;
		struct Elf32_Ehdr elfHeader;
		struct Elf32_Shdr *elfSectionHeaders = 0;
		struct Elf32_Sym *elfSymbolTable = 0;
		struct Elf32_Rel *elfRelTable = 0;

		if ( !load() ) break; // parse input

		// open object file
		file.ptr = fopen (file.name,"rb");
		if (file.ptr == NULL) { fprintf (stderr,err.cannotOpen,file.name); continue; }

		// obtain file size
		fseek (file.ptr , 0 , SEEK_END);
		file.size = ftell (file.ptr);
		rewind (file.ptr);

		// allocate memory to contain the whole file
		file.buffer = (char*) malloc (sizeof(char) * file.size);
		if (file.buffer == NULL) error(err.cannotAlloc)

		// copy the file into the buffer:
		cnt = fread (file.buffer, 1, file.size, file.ptr);
		if (cnt != file.size) error(err.cannotRead)

		fclose (file.ptr);

		// check if file is in ELF
		if (file.buffer[0] != 127 || file.buffer[1] != 'E' || file.buffer[2] != 'L' || file.buffer[3] != 'F')
			error(err.notElfFile)

		// load Elf header from OM buffer
		memcpy (&elfHeader, &file.buffer[0], sizeof (struct Elf32_Ehdr));
		shoff = elfHeader.e_shoff; // section headers offset
		file.sectionStrTabOff = 0;

		// alloc array of section header entries
		elfSectionHeaders = (struct Elf32_Shdr*) malloc ((elfHeader.e_shnum) * sizeof (struct Elf32_Shdr));
		if (elfSectionHeaders == NULL) error(err.cannotAlloc)

		// load section header entries, load symbol table and relocs, map sections
		for (cnt = 0; cnt < elfHeader.e_shnum; cnt++)
		{
			// load Elf Section Header Table Entry from OM buffer
			memcpy (&elfSectionHeaders[cnt], &file.buffer[shoff], sizeof (struct Elf32_Shdr));
			shoff += elfHeader.e_shentsize;

			unsigned long offset = elfSectionHeaders[cnt].sh_offset;
			if(offset == SHT_NOBITS) continue;

			size_t size = elfSectionHeaders[cnt].sh_size;
			if(size == 0) continue;

			// Alloc and Exec sections - example .TEXT
			if (elfSectionHeaders[cnt].sh_flags == (SHF_ALLOC | SHF_EXECINSTR))
			{
			    void *addr = mmap (NULL, size + 16, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);
			    if (addr == MAP_FAILED) error(err.mapFailed)

			    memcpy (addr, file.buffer + offset, size); //dst, src, size
			    elfSectionHeaders[cnt].sh_addr = (uint32_t) addr;
			    file.mainVA = addr;
			    runAddr = addr;

			    unsigned long printfAddr = (unsigned long) &printf;
                unsigned long scanfAddr = (unsigned long) &scanf;
                unsigned long instrHex = 0xE59FF000; // instruction - ldr pc, [pc]

                memcpy (addr + size, &instrHex, sizeof (unsigned long));
                memcpy (addr + size + sizeof (unsigned long), &instrHex, sizeof (unsigned long));
                memcpy (addr + size + 2 * sizeof (unsigned long), &printfAddr, sizeof (unsigned long));
                memcpy (addr + size + 3 * sizeof (unsigned long), &scanfAddr, sizeof (unsigned long));

                file.myPrintfAddr = addr + size;
                file.myScanfAddr = addr + size + sizeof(unsigned long);
            }

            // Alloc and Write sections - example .DATA
            if (elfSectionHeaders[cnt].sh_flags == (SHF_ALLOC | SHF_WRITE))
			{
                void* addr = mmap (NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);
                if (addr == MAP_FAILED) error(err.mapFailed)

                memcpy (addr, file.buffer + offset, size); //dst, src, size
                elfSectionHeaders[cnt].sh_addr = (uint32_t)addr;
            }

            // Alloc sections - read only - example .RODATA
            if (elfSectionHeaders[cnt].sh_flags == SHF_ALLOC)
            {
                void* addr = mmap (NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1 ,0);
                if (addr == MAP_FAILED) error(err.mapFailed)

                memcpy (addr, file.buffer + offset, size); //dst, src, size
                elfSectionHeaders[cnt].sh_addr = (uint32_t)addr;
            }

            // Relocation table
            if (elfSectionHeaders[cnt].sh_type == SHT_REL)
            {
                unsigned long i;
                file.numOfRelocs = size / elfSectionHeaders[cnt].sh_entsize;

                elfRelTable = (struct Elf32_Rel*) malloc ((file.numOfRelocs) * sizeof (struct Elf32_Rel));
                if (elfRelTable == NULL) error(err.cannotAlloc)

                // load all reloc entries from OM buffer
                for (i = 0; i < file.numOfRelocs; i++){
                    memcpy (&elfRelTable[i], &file.buffer[offset], sizeof (struct Elf32_Rel));
                    elfRelTable[i].sh_link = elfSectionHeaders[cnt].sh_link;
                    elfRelTable[i].sh_info = elfSectionHeaders[cnt].sh_info;
                    offset += elfSectionHeaders[cnt].sh_entsize;
                }
            }

            // Symbol table
            if (elfSectionHeaders[cnt].sh_type == SHT_SYMTAB)
            {
                unsigned long i;
                file.numOfSymbols = size / elfSectionHeaders[cnt].sh_entsize;
                file.symTableOff = offset;

                elfSymbolTable = (struct Elf32_Sym*) malloc (file.numOfSymbols * sizeof (struct Elf32_Sym));
                if (elfSymbolTable == NULL) error(err.cannotAlloc)

                // load all symbol table entries from OM buffer
                for (i = 0; i < file.numOfSymbols; i++){
                    memcpy (&elfSymbolTable[i], &file.buffer[offset], sizeof (struct Elf32_Sym));
                    offset += elfSectionHeaders[cnt].sh_entsize;
                }
            }

            // String tables
            if (elfSectionHeaders[cnt].sh_type == SHT_STRTAB )
            {
                if (!file.sectionStrTabOff){
                    file.sectionStrTabOff = offset;
                    continue;
                }

                file.symbolStrTabOff = offset;
            }
        } // end of for - all section headers loaded

        // relocations
        for (cnt = 0; cnt < file.numOfRelocs; cnt++)
        {
            char type = ELF32_R_TYPE (elfRelTable[cnt].r_info);
            unsigned long symId = ELF32_R_SYM (elfRelTable[cnt].r_info); // symbol table index
            unsigned long sectionId = elfSymbolTable[symId].st_shndx; // symbol is defined in this section

            // offset to symbol + VA of section in which symbol is used (example .TEXT)
            unsigned long usingAddr = elfRelTable[cnt].r_offset + elfSectionHeaders[ elfRelTable[cnt].sh_info ].sh_addr;

            if (type == R_ARM_ABS32)
            {
                unsigned long symValue = 0; // offset to symbol in its defined section (example .DATA)
                memcpy ( &symValue, (void*) usingAddr, sizeof (unsigned long)); // old value - just offset

                symValue += elfSectionHeaders[sectionId].sh_addr; // add VA of section - where symbol is defined (example .DATA)
                memcpy ( (void*) usingAddr, &symValue, sizeof (unsigned long)); // write offset  + VA back
                continue;
            }

            if (type == R_ARM_MOVW_ABS_NC || type == R_ARM_MOVT_ABS)
            {
                unsigned long instruction = 0;
                memcpy (&instruction, (void*) usingAddr, sizeof (unsigned long));
                instruction &= 0xFFF0F000; //clear bits 19-16 and 11-0

                unsigned long adr = elfSymbolTable[symId].st_value + elfSectionHeaders[sectionId].sh_addr; // VA

                if (type == R_ARM_MOVT_ABS) adr >>= 16; // "Up is down" CJS
                adr &= 0xFFFF; // 2B

                unsigned long up = (adr & 0xF000) << 4; // bits 19-16
                unsigned long down = adr & 0x0FFF; // bits 11-0

                instruction = instruction + up + down;
                memcpy ((void*) usingAddr, &instruction, sizeof (unsigned long));
                continue;
            }


            char *symbolName = getSymbolNameFromTable (elfSymbolTable[symId].st_name);
            if (symbolName == 0x00){ free (symbolName); continue; }

            if( !strcmp (symbolName, "main") )  runAddr += elfSymbolTable[symId].st_value;

            if ( !strcmp (symbolName, "printf") )
            {
                unsigned long temp = (unsigned long) file.myPrintfAddr - usingAddr - 8;
                temp = temp/4;
                memcpy ( (void*) usingAddr, &temp, 3);
            }

            if ( !strcmp (symbolName, "scanf") )
            {
                unsigned long temp = (unsigned long) file.myScanfAddr - usingAddr - 8;
                temp = temp/4;
                memcpy ( (void*) usingAddr, &temp, 3);
            }

            free (symbolName);
        }

        // change protection for readonly sections
        for (cnt = 0; cnt < elfHeader.e_shnum; cnt++)
            if (elfSectionHeaders[cnt].sh_flags == SHF_ALLOC)
                mprotect ((void*) elfSectionHeaders[cnt].sh_addr, elfSectionHeaders[cnt].sh_size, PROT_READ);

		// create 1MB stack
		stack = malloc (STACK_SIZE);
		if (stack == 0) error(err.cannotAlloc)
		stack = stack + STACK_SIZE - 4; // last location

		// run program - jump to symbol main
		run (argc, argv);

        // remove sections from memory maped space
        for (cnt = 0; cnt < elfHeader.e_shnum; cnt++)
            if (elfSectionHeaders[cnt].sh_addr != 0)
                if (munmap ((void*) elfSectionHeaders[cnt].sh_addr, elfSectionHeaders[cnt].sh_size) == -1) error(err.munFailed)

		printf ("\n\n\nProgram is successfully executed.\n");

		// free
		free (stack - STACK_SIZE + 4);
        free (argv[0]);
        free (argv);
        free (file.buffer);
        free (elfSectionHeaders);
        free (elfSymbolTable);
        free (elfRelTable);
	} // end of while

	return 0;
}
