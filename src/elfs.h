// elfs.h - Course: System Programming
// author: Marin Markic
#ifndef _ELF_H
#define	_ELF_H

/* TYPE DEFINITIONS */

typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

/* Standard ELF types.  */

typedef uint16_t Elf32_Half; /* Type for a 16-bit quantity.  */
typedef uint32_t Elf32_Word; /* Types for signed and unsigned 32-bit quantities.  */
typedef uint32_t Elf32_Addr; /* Type of addresses.  */
typedef uint32_t Elf32_Off; /* Type of file offsets.  */
typedef uint16_t Elf32_Section; /* Type for section indices, which are 16-bit quantities.  */

/* The ELF file header.  This appears at the start of every ELF file.  */

#define EI_NIDENT (16)

struct Elf32_Ehdr
{
	unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
	Elf32_Half	e_type;			/* Object file type */
	Elf32_Half	e_machine;		/* Architecture */
	Elf32_Word	e_version;		/* Object file version */
	Elf32_Addr	e_entry;		/* Entry point virtual address */
	Elf32_Off	e_phoff;		/* Program header table file offset */
	Elf32_Off	e_shoff;		/* Section header table file offset */
	Elf32_Word	e_flags;		/* Processor-specific flags */
	Elf32_Half	e_ehsize;		/* ELF header size in bytes */
	Elf32_Half	e_phentsize;		/* Program header table entry size */
	Elf32_Half	e_phnum;		/* Program header table entry count */
	Elf32_Half	e_shentsize;		/* Section header table entry size */
	Elf32_Half	e_shnum;		/* Section header table entry count */
	Elf32_Half	e_shstrndx;		/* Section header string table index */
};

/* Section header.  */

struct Elf32_Shdr
{
	Elf32_Word	sh_name;		/* Section name (string tbl index) */
	Elf32_Word	sh_type;		/* Section type */
	Elf32_Word	sh_flags;		/* Section flags */
	Elf32_Addr	sh_addr;		/* Section virtual addr at execution */
	Elf32_Off	sh_offset;		/* Section file offset */
	Elf32_Word	sh_size;		/* Section size in bytes */
	Elf32_Word	sh_link;		/* Link to another section */
	Elf32_Word	sh_info;		/* Additional section information */
	Elf32_Word	sh_addralign;		/* Section alignment */
	Elf32_Word	sh_entsize;		/* Entry size if section holds table */
};

/* Legal values for sh_type (section type).  */

#define SHT_NULL	  0		/* Section header table entry unused */
#define SHT_PROGBITS	  1		/* Program data */
#define SHT_SYMTAB	  2		/* Symbol table */
#define SHT_STRTAB	  3		/* String table */
#define SHT_RELA	  4		/* Relocation entries with addends */
#define SHT_HASH	  5		/* Symbol hash table */
#define SHT_DYNAMIC	  6		/* Dynamic linking information */
#define SHT_NOTE	  7		/* Notes */
#define SHT_NOBITS	  8		/* Program space with no data (bss) */
#define SHT_REL		  9		/* Relocation entries, no addends */

/* Legal values for sh_flags (section flags).  */

#define SHF_WRITE	     (1 << 0)	/* Writable */
#define SHF_ALLOC	     (1 << 1)	/* Occupies memory during execution */
#define SHF_EXECINSTR	 (1 << 2)	/* Executable */

/* Symbol table entry.  */

struct Elf32_Sym
{
	Elf32_Word	st_name;		/* Symbol name (string tbl index) */
	Elf32_Addr	st_value;		/* Symbol value */
	Elf32_Word	st_size;		/* Symbol size */
	unsigned char	st_info;		/* Symbol type and binding */
	unsigned char	st_other;		/* Symbol visibility */
	Elf32_Section	st_shndx;		/* Section index */
};

/* Special section indices.  */

#define SHN_UNDEF	0		/* Undefined section */
#define SHN_ABS		0xfff1		/* Associated symbol is absolute */
#define SHN_COMMON	0xfff2		/* Associated symbol is common */

/* Relocation table entry without addend (in section of type SHT_REL).  */

struct Elf32_Rel
{
  Elf32_Addr	r_offset;		/* Address */
  Elf32_Word	r_info;			/* Relocation type and symbol index */
  Elf32_Word	sh_link;		/* The section header index of the associated symbol table */
  Elf32_Word	sh_info;		/* The section header index of the section to which the relocation applies */
};

/* How to extract and insert information held in the r_info field.  */

#define ELF32_R_SYM(val)		((val) >> 8)
#define ELF32_R_TYPE(val)		((val) & 0xff)
#define ELF32_R_INFO(sym, type)	(((sym) << 8) + ((type) & 0xff))

/* ARM relocs. */

#define R_ARM_NONE		0	/* No reloc */
#define R_ARM_PC24		1	/* PC relative 26 bit branch */
#define R_ARM_ABS32		2	/* Direct 32 bit  */
#define R_ARM_REL32		3	/* PC relative 32 bit */
#define R_ARM_PC13		4
#define R_ARM_ABS16		5	/* Direct 16 bit */
#define R_ARM_ABS12		6	/* Direct 12 bit */
#define R_ARM_THM_ABS5	7
#define R_ARM_ABS8		8	/* Direct 8 bit */

#define R_ARM_CALL      0x1c
#define R_ARM_JUMP24    0x1d
#define R_ARM_MOVW_ABS_NC       0x2b
#define R_ARM_MOVT_ABS          0x2c


#endif	/* elfs.h */
