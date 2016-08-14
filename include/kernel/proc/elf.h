#ifndef __ELF_H__
#define __ELF_H__
#include <stdint.h>

#define ELF_MAGIC_WORD 0x464C457F
#define ELF_SHN_UNDEF       0
#define ELF_SHN_LORESERVE   0xff00
#define ELF_SHN_LOPROC      0xff00
#define ELF_SHN_HIPROC      0xff1f
#define ELF_SHN_LIVEPATCH   0xff20
#define ELF_SHN_ABS         0xfff1
#define ELF_SHN_COMMON      0xfff2
#define ELF_SHN_HIRESERVE   0xffff

#define ELF_TYPE_NONE		0
#define ELF_TYPE_REL		1
#define ELF_TYPE_EXEC		2
#define ELF_TYPE_DYN		3
#define ELF_TYPE_CORE		4

typedef struct
{
	uint32_t 	magic;
	uint8_t 	bits;
	uint8_t 	endian;
	uint8_t 	version;
	uint8_t 	target_os;
	uint8_t 	abi_version;
	char		unused[7];
	uint16_t	type;
	uint16_t	target_machine;
	uint32_t	version2;
	uint32_t	entry;
	uint32_t	ph_start;
	uint32_t	sh_start;
	uint32_t	eflags;
	uint16_t	header_size;
	uint16_t	prog_head_size;
	uint16_t	prog_head_count;
	uint16_t	sect_head_size;
	uint16_t	sect_head_count;
	uint16_t	sect_name_entry;
} elf_header_t;

typedef struct
{
	uint32_t	type;
	uint32_t	offset;
	uint32_t	vaddr;
	uint32_t	paddr;
	uint32_t	filesz;
	uint32_t	memsz;
	uint32_t	flags;
	uint32_t	align;
} elf_pheader_t;

typedef enum
{
	SHT_NULL	= 0,   // Null section
	SHT_PROGBITS	= 1,   // Program information
	SHT_SYMTAB	= 2,   // Symbol table
	SHT_STRTAB	= 3,   // String table
	SHT_RELA	= 4,   // Relocation (w/ addend)
	SHT_NOBITS	= 8,   // Not present in file
	SHT_REL		= 9,   // Relocation (no addend)
} elf_sec_type;

typedef struct 
{
	uint32_t		name;
	uint32_t		value;
	uint32_t		size;
	uint8_t			info;
	uint8_t			other;
	uint16_t		shndx;
} elf_sym_t;

typedef enum
{
	LOCAL	= 0,
	GLOBAL	= 1,
	WEAK	= 2
} elf_sym_binding;

typedef enum
{
	NOTYPE = 0,
	OBJECT = 1,
	FUNC = 2	
} elf_sym_type;

#define ELF_SYM_BINDING(INFO) ((INFO)>>4)
#define ELF_SYM_TYPE(INFO) ((INFO)&0xF)

typedef struct
{
	uint32_t	name;
	uint32_t	type;
	uint32_t	flags;
	uint32_t	addr;
	uint32_t	offset;
	uint32_t	size;
	uint32_t	link;
	uint32_t	info;
	uint32_t	addr_align;
	uint32_t	entsize;
} elf_sheader_t;

void elf_print_headerinfo(elf_header_t* header);

elf_sheader_t* elf_get_sect_hdr(elf_header_t* hdr, int index);
void elf_print_sheaderinfo(elf_header_t* header, elf_sheader_t* sheader);
char* elf_get_section_name(elf_header_t* header, uint32_t index);

void elf_print_symtab(elf_header_t* header);
void elf_print_symbol(elf_header_t* header, elf_sheader_t* sheader, elf_sym_t* symbol);
uint32_t elf_get_sym_value(elf_header_t* header, elf_sheader_t* sheader, elf_sym_t* symbol);
char* elf_get_sym_name(elf_header_t* header, elf_sheader_t* sheader, elf_sym_t* symbol);

#endif