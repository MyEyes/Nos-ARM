#include "kernel/proc/elf.h"
#include <stdio.h>

void elf_print_headerinfo(elf_header_t* header)
{
	if(header->magic != ELF_MAGIC_WORD)
	{
		printf("[%x] Not a valid elf header, MAGIC: %x\r\n", header, header->magic);
		return;
	}
	printf("\r\nELF\r\n\r\n");
	
	printf("Sections\r\n");
	elf_sheader_t* sh =(elf_sheader_t*)((char*)header + header->sh_start);
	for(int x=0; x<header->sect_head_count; x++)
		elf_print_sheaderinfo(header, sh+x);
	
	printf("\r\nSymbols\r\n");
	printf("Value\t\tName\r\n");
	elf_print_symtab(header);
}

elf_sheader_t* elf_get_sect_hdr(elf_header_t* header, int index)
{
	elf_sheader_t* sh =(elf_sheader_t*)((char*)header + header->sh_start);
	return sh+index;
}

void elf_print_sheaderinfo(elf_header_t* header, elf_sheader_t* section)
{
	printf("[%x] %s\r\n", section->addr, elf_get_section_name(header, section->name));
}

char* elf_get_section_name(elf_header_t* header, uint32_t index)
{
	if(!header->sect_name_entry)
		return 0;
	if(!index)
		return 0;
	
	char* snt = (char*)header + elf_get_sect_hdr(header, header->sect_name_entry)->offset;
	return snt + index;
}

void elf_print_symtab(elf_header_t* header)
{
	for(int x=0; x<header->sect_head_count; x++)
	{
		elf_sheader_t* shdr = elf_get_sect_hdr(header, x);
		if(shdr->type!=SHT_SYMTAB)
			continue;
		
		uint32_t entries = shdr->size/shdr->entsize;
		elf_sym_t* symbol = (elf_sym_t*)((char*)header + shdr->offset);
		
		for(uint32_t y=0; y<entries; y++)
			elf_print_symbol(header, shdr, symbol+y);
	}
}

void elf_print_symbol(elf_header_t* header, elf_sheader_t* sheader, elf_sym_t* symbol)
{
	printf("%x\t%s\r\n", elf_get_sym_value(header,sheader,symbol), elf_get_sym_name(header, sheader, symbol));
}

uint32_t elf_get_sym_value(elf_header_t* header, elf_sheader_t* sheader, elf_sym_t* symbol)
{	
	(void)sheader;
	if(symbol->shndx == ELF_SHN_ABS || header->type == ELF_TYPE_DYN || header->type==ELF_TYPE_EXEC)
		return symbol->value;
	else
	{
		elf_sheader_t *target = elf_get_sect_hdr(header, symbol->shndx);
		return symbol->value + target->offset;
	}
}

char* elf_get_sym_name(elf_header_t* header, elf_sheader_t* sheader, elf_sym_t* symbol)
{
	elf_sheader_t* strtab = elf_get_sect_hdr(header, sheader->link);
	return (char*) header+strtab->offset+symbol->name;
}

/*
R_ARM_PC24 Bits 0-23 encode a signed offset in units of 4-byte words.
R_ARM_PC13 Bits 0-11 encode an unsigned byte offset. Bit 23 encodes the direction of
the offset— 0 means to a lower address than P, 1 to a higher address.
R_ARM_ABS12 Bits 0-11 encode an unsigned byte offset.
R_ARM_AMP_VCALL9 Bits 11-19 encode an unsigned offset in units of 8-byte AMP instructions.
R_ARM_SWI24 Bits 0-23 encode the ARM SWI number.
R_ARM_XPC25 Bits 0-23 encode a signed offset in units of 4-byte words. Bit 24 encodes
bit 1 of the target Thumb address.
*/
uint32_t elf_calc_reloc(uint32_t orig, elf_rel_t* rel, uint32_t addend)
{
	(void) addend;
	(void) orig;
	uint32_t mask;
	switch(ELF_R_SYM(rel->info))
	{
		case R_ARM_PC24: mask = ((1<<24)-1); break;
		case R_ARM_PC13: mask = ((1<<12)-1) | 1<<23; break;
		case R_ARM_ABS12: mask = (1<<12)-1; break;
		case R_ARM_AMP_VCALL9: mask = ((1<<9)-1)<<11;
		case R_ARM_SWI24: mask = ((1<<24))-1;
		case R_ARM_XPC25: mask = ((1<<24))-1;
		default: mask = (uint32_t)-1;
	}
	return mask;
}