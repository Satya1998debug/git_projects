
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>


/* configurations for Encoder & Decoder */
unsigned short int ref_sample_data = 0;  // actual reference sample

int r_ref_interval = 20; // number of blocks b/w consecutive reference samples, 
                        // based on the reference sample interval, stays constant throughout the compression process
int s_seg_size = 64; // number of blocks in each segment, segment size in case of zero-block compression, 
                    // stays constant throughout the compression process

int max_seg_size = 64;
int word_size = 16; // size of each sample in bits(data is 12 bits but we make it 16 bits after adding 0000 to MSB, so that the decimal value is same in both cases)

int encoder_block_size = 8;  // block size for encoding, its the value of 'J', number of smaples in one block

unsigned int total_compressed_bits = 0; // contains the total number of bits after a successful compression