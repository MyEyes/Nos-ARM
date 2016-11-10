#include "devmap_bld.h"
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define DEV_NAME_MAX_LENGTH 20
#define DEV_TYPE_MAX_LENGTH 4

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("You need to specify an input and output file\r\n");
		printf("Usage: devmap_bld input_file output_file\r\n");
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
	
	uint32_t num_devs = 0;
	uint32_t total_size = sizeof(uint32_t) + sizeof(uint32_t);
	
	//Write the two header values of the devmap to file, so that we can overwrite
	//them with correct values later without having to move data around or change file size
	fwrite(&num_devs, sizeof(uint32_t), 1, output);
	fwrite(&total_size, sizeof(uint32_t), 1, output);
	
	char buffer[DEV_NAME_MAX_LENGTH];
	memset(buffer, 0, DEV_NAME_MAX_LENGTH+1);
	uint32_t address;
	char type[DEV_TYPE_MAX_LENGTH];
	memset(type, 0, DEV_TYPE_MAX_LENGTH+1);
	uint32_t curr_id=0;
		
	while(!feof(input))
	{
		
		if(fscanf(input, "%x %s %s", &address, type, buffer) == 3)
		{
			fwrite(&curr_id, sizeof(uint32_t), 1, output);
			fwrite(buffer, sizeof(char), DEV_NAME_MAX_LENGTH, output);
			fwrite(type, sizeof(char), DEV_TYPE_MAX_LENGTH, output);
			fwrite(&address, sizeof(uint32_t), 1, output);
			num_devs++;
			curr_id++;
			total_size += 2*sizeof(uint32_t) + DEV_NAME_MAX_LENGTH + DEV_TYPE_MAX_LENGTH;
			memset(buffer, 0, DEV_NAME_MAX_LENGTH);
			memset(type, 0, DEV_TYPE_MAX_LENGTH);
		}
	}
	
	//Go back to beginning of file so that we can 
	//Write the proper number of devices and total size out
	rewind(output);
	fwrite(&num_devs, sizeof(uint32_t), 1, output);
	fwrite(&total_size, sizeof(size_t), 1, output);
}
