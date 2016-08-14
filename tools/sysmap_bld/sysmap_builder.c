#include <stdio.h>
#include <string.h>
#include <byteswap.h>
#include "../../include/kernel/proc/sysmap.h"

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("You need to specify an input and output file\r\n");
		return -1;
	}
	
		
	FILE *input;
	input = fopen(argv[1], "r");
	if(!input)
	{
		printf("Couldn't open input file\r\n");
		return -1;
	}
	
	FILE *output = fopen(argv[2], "w");
	if(!output)
	{
		printf("Couldn't open output file\n");
		return -1;
	}
	
	
	unsigned int str_offset = 0;
	
	//Write 12 bytes now so that they are reserved, we will fill in the header at the end
	uint32_t num_entries = 0;
	uint32_t total_size = 0;
	uint32_t name_off = 0;
	uint32_t magic = SYSMAP_MAGIC;
	
	fwrite(&magic, sizeof(uint32_t), 1, output);
	fseek(output, sizeof(uint32_t)*4, SEEK_SET);
	
	while(!feof(input))
	{
		
		char buffer[160];
		uint32_t address;
		uint32_t type;
		
		if(fscanf(input, "%x %1c %s", &address, &type, buffer) == 3)
		{
			uint32_t str_offset_w = str_offset;
		
			fwrite(&address, sizeof(uint32_t), 1, output);
			fwrite(&type, sizeof(uint32_t), 1, output);
			fwrite(&str_offset_w, sizeof(uint32_t), 1, output);
			str_offset += strlen(buffer)+1;
			num_entries++;
		}
	}
	
	rewind(input);
	while(!feof(input))
	{
		
		char buffer[160];
		uint32_t address;
		uint32_t type;
		
		if(fscanf(input, "%x %1c %s", &address, &type, buffer)==3)
		{
			fwrite(buffer, sizeof(char), strlen(buffer)+1, output);
		}
	}
	
	name_off = num_entries*sizeof(sysmap_entry_t) + 4*sizeof(uint32_t);
	total_size = name_off + str_offset;
	
	total_size = total_size;
	name_off = name_off;
	num_entries = num_entries;
	
	
	
	fseek(output, sizeof(uint32_t), SEEK_SET);
	fwrite(&total_size, sizeof(uint32_t), 1, output);
	fwrite(&num_entries, sizeof(uint32_t), 1, output);
	fwrite(&name_off, sizeof(uint32_t), 1, output);
	
	//printf("End of file reached\n");
	return 0;
}