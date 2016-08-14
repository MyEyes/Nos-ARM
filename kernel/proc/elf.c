#include "kernel/proc/elf.h"
#include "std/stdio.h"

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