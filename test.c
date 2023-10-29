#include"rice_modified.h"
// #include"rice_decode_module.h"
#include<stdio.h>
#include<string.h>
#include<stdio.h>


struct number
{
    /* data */
    int unsigned short num1:12;
};

int write_to_file(unsigned short int value, unsigned int sample_num){

    char *filename = "read_data2.txt";

    // open the file for writing
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }
    // write to the text file

    fprintf(fp, "%x\n", value);

    // close the file
    fclose(fp);
    return 0;
}

int main(){
    FILE *fptr = NULL;
    struct number num;
    unsigned short *data_pointer;
    int total_blocks_num = 4938;
    // int total_blocks_num = 2;
    unsigned short int data_block[total_blocks_num * encoder_block_size];

    //********** READ 12BITS FROM BINARY FILE TO TEST CODE

    FILE *file = fopen("science_data_example_new.tlm", "rb");  // Replace "input.bin" with your actual file name
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    unsigned char buffer;
    unsigned short int value = 0;
    int bitsRead = 0;
    int bitsToRead = 12;

    // int x = 2000;
    unsigned int sample_num = 0;
    while (1) {
        size_t bytesRead = fread(&buffer, 1, 1, file);
        if (bytesRead != 1) {
            if (feof(file)) {
                printf("End of file reached\n");
            } else if (ferror(file)) {
                perror("Error reading file");
            }
            break;
        }

        // Process each bit in the current byte
        for (int i = 7; i >= 0; i--) {
            value = (value << 1) | ((buffer >> i) & 1);
            bitsRead++;

            if (bitsRead == bitsToRead) {
                // printf("Read value: %u\n", value);
                // write_to_file(value, sample_num);
                data_block[sample_num] = value;
                sample_num++;
                value = 0;
                bitsRead = 0;
            }
        }
        
        // x--;
    }

    fclose(file);

    // process 16bits into 12 bits values (data_block_16bit to data_block_12bit)

    // unsigned short int data_block[16] = {4095, 4095, 4095, 4095, 4095, 4095, 4095, 4094, 4095, 4095, 4095, 4095, 4095, 4095, 4095, 4094};

    // ASSUMING ALL DATA IS LOADED ALREADY TO SOME DATA STRUCTURE

    // printf("This check is okay..-2\n");

    data_pointer = data_block;
    compress_loaded_data(data_pointer, total_blocks_num);
    return 0;

}

