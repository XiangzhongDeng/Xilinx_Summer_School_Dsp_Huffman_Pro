
#include "huffman.h"

#include <stdio.h>
#include <stdlib.h>


void file_to_array(const char *filename, ap_uint<16> *&array, int array_length)
{
	printf("Start reading file [%s]\n", filename);
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Cannot find the input file\n");
		exit(1);
	}
	int file_value = 0;

	int count = 0;

	array = (ap_uint<16> *)malloc(array_length * sizeof(ap_uint<16>));
	while (1)
	{
		int eof_check = fscanf(file, "%x", &file_value);
		if (eof_check == EOF)
			break;
		else
		{
			array[count++] = (ap_uint<16>)file_value;
		}
	}

	fclose(file);

	if (count != array_length)
		exit(1);
}


void my_file_to_array(const char *filename, ap_uint<16> *&array_value, ap_uint<16> *&array_rank, int array_length)
{
	printf("Start reading file [%s]\n", filename);
	FILE *file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Cannot find the input file\n");
		exit(1);
	}
	int file_value = 0;
	int file_rank = 0 ;

	int count = 0;

	array_value = (ap_uint<16> *)malloc(array_length * sizeof(ap_uint<16>));
	array_rank = (ap_uint<16> *)malloc(array_length * sizeof(ap_uint<16>));

	while (1)
	{
		int eof_check = fscanf(file, "%d,%x", &file_rank, &file_value);
		if (eof_check == EOF)
			break;
		else
		{
			array_value[count] = (ap_uint<16>)file_value ;
			array_rank[count] = (ap_uint<16>)file_rank;
			//printf("%x\n", array_value[count]);
			count++;

		}
	}

	fclose(file);

	if (count != array_length)
		exit(1);
}

int main()
{
	printf("Starting canonical Huffman encoding testbench\n");
	FILE *output_file;
	FILE *golden_file;
	FILE *diff_file;
	int return_val = 0;
	ap_uint<16> *frequencies = NULL;
	ap_uint<16> *golgen_encoding_value = NULL;
	ap_uint<16> *golgen_encoding_rank = NULL;

	file_to_array("D:/Downloads/pynq_z2_v2.6.0/pp4fpgas-cn-hls/hw/ip/huffman/huffman256.txt", frequencies, INPUT_SYMBOL_SIZE);
	my_file_to_array("D:/Downloads/pynq_z2_v2.6.0/pp4fpgas-cn-hls/hw/ip/huffman/huffman256_golden.txt",golgen_encoding_value, golgen_encoding_rank, INPUT_SYMBOL_SIZE);

	ap_uint<SYMBOL_BITS> s_value_V[INPUT_SYMBOL_SIZE];

	ap_uint<32> s_frequency_V[INPUT_SYMBOL_SIZE];

	for (ap_uint<10> i = 0; i < INPUT_SYMBOL_SIZE; i++)
	{
		s_frequency_V[i] = frequencies[i];
		s_value_V[i] = i;
	}

	int num_nonzero_symbols;
	PackedCodewordAndLength encoding[INPUT_SYMBOL_SIZE];

	huffman(s_value_V, s_frequency_V, encoding, &num_nonzero_symbols);

	output_file = fopen("D:/Downloads/pynq_z2_v2.6.0/pp4fpgas-cn-hls/hw/ip/huffman/huffman256out.txt", "w");
	diff_file = fopen("D:/Downloads/pynq_z2_v2.6.0/pp4fpgas-cn-hls/hw/ip/huffman/huffman256diffout.txt", "w");
	for (int i = 0; i < INPUT_SYMBOL_SIZE; i++){
		fprintf(output_file, "%d, %x\n", i, (unsigned int)encoding[i]);
        if(golgen_encoding_value[i] != encoding[i]){
        	fprintf(diff_file, "%d, %x\n", i, (unsigned int)encoding[i]);
        	return_val++ ;  //不相等则 return_val 自加 并将结果输出到 huffman256diffout.txt
        }
	}

	fclose(diff_file);
	fclose(output_file);

	printf("\n***************Comparing against output data*************** \n\n");
	if(!return_val)
	{
		fprintf(stdout, "*******************************************\n");
		fprintf(stdout, "FAIL: Output DOES NOT match the golden output\n");
		fprintf(stdout, "*******************************************\n");
	}
	else
	{
		fprintf(stdout, "*******************************************\n");
		fprintf(stdout, " PASS: The output matches the golden output\n");
		fprintf(stdout, "*******************************************\n");
		return_val = 0;
	}

	printf("Ending canonical Huffman encoding testbench\n");

	return return_val;
}
