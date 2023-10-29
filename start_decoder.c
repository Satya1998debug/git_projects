#include"rice_decode_module.h"
#include<stdio.h>
#include<string.h>
#include<stdio.h>


int main(){    

    unsigned short int required_data[8] = {0};
    unsigned short int ref_sample_data = 1581;

    /* ############################ 2nd Extension ############################################ */
    
    // char character_seq[98] = "00001100000000100000000000000000000000000000000100000000000000000000000000000000000000000000000001"; 

    /* ########################## K-Split ############################################## */


    // char character_seq[28] = "0010111010100100100100101010"; // k-split, k=13
    char character_seq[75] = "011100000000000000011111111000010000101000001000110000111000010010000000111"; // k-split, k=2

    /* ############################## Zero-Block ########################################## */

    // char character_seq[28] = "00000000001000001"; // Zero Block: 1 segments(3 blks), 0 ros
    int total_bits = 75; // we can use total_compressed_bits from config.h file in real scenario

    /* ######################################################################## */

    char *bit_seq = character_seq;


    decode_compressed_data(bit_seq, total_bits, encoder_block_size, ref_sample_data);

    return 0;

}

