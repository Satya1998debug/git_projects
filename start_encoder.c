#include"rice_modified.h"
#include<stdio.h>
#include<string.h>
#include<stdio.h>
// #include"config.h"
// #include"config_processor.h"


// struct number
// {
//     /* data */
//     unsigned short int num1;
// };

// void structure_raw_data(int data_block[], int encoder_block_size, struct data_point data_point_array[])
// {
//     int i = 0;
//     while(i < encoder_block_size){
//         data_point_array[i].raw_dec_val = data_block[i];
//         data_point_array[i].delta = 0;
//         data_point_array[i].theta = 0;
//         data_point_array[i].predicted_val = 0;
//         data_point_array[i].processed_dec_value = 0;
//         i++;
//     }
// }

// int is_all_zero_block(int required_data[], int encoder_block_size){
//     for(int i = 0; i < encoder_block_size; i++){
//         if(required_data[i] - ref_sample_data != 0){  // some sample is noy equal, so return from there itself
//             return 0;  // its NOT an All-Zero Block
//         }
//     }
//     return 1;  // otherwise its All-Zero Block
// }

// void compress_loaded_data(unsigned short *data_block, int total_blocks_num){

//     struct data_point data_point_array[encoder_block_size];

//     int zero_block_counter = 0; // counts the number of zero blocks
//     int segment_counter = 0; // counts number of segments
//     int ros_zero_blk = 0; // count the ROS sequnce blocks
//     // int first_data_flag = 1;

//     // ros_code indicates the number of bits required for the FS code for ros decimal equivalent
//     int ros_code = get_ros_seg_enncoding(); // gets ROS code decimal value, so that later it can be encoded in FS code

//     int ref_flag = 0; // indicates that the refernce sample is present if value = 1

//     // Loop-1 on the total blocks... block-by-block, blk_num=0 to total blocks
//     for(int blk_num = 0; blk_num < total_blocks_num; blk_num++){
//         int required_data[encoder_block_size];
        
//         // update the reference flag, to check if its a normal data block or 1st data block
//         if(ref_flag){
//             ref_flag = 0;
//         }
        
//         // Loop-2 on the data J samples, ... data-by-data, sample_num=0 to block size(8)
//         for(int sample_num = 0; sample_num < encoder_block_size; sample_num++){
//             // loading data from data_block(original data) to required_data(data to be processed) array
//             required_data[sample_num] = *(data_block + encoder_block_size * blk_num + sample_num);

//             // check if it is the reference block and save the reference sample data
//             if(blk_num%r_ref_interval == 0 && sample_num == 0){
//                 ref_flag = 1; // make the reference flag as True(1)
//                 ref_sample_data = required_data[sample_num]; // extract reference sample
//             }
//         }

//         /* Check for Zero-block and Encode */
//         if(is_all_zero_block(required_data, encoder_block_size)){ // return 1, if its zero-block, else return 0
//             zero_block_counter++;
//             segment_counter = zero_block_counter / s_seg_size; // number of complete segments, each segment has s_seg_size 0-blocks 
//             ros_zero_blk = zero_block_counter % s_seg_size;  // remainder-of-sequence.. remaining number of 0-blocks
//             if(blk_num == total_blocks_num - 1){ // if this is the last block & its 0-block, then better compress this before ending the code
//                 // compress with 0-block option as its the last block
//                 if(zero_block_counter){
//                     printf(" This is the last block");
//                     zero_block_encoding(zero_block_counter, segment_counter, ros_zero_blk, ros_code, ref_sample_data, &ref_flag);
//                     printf("\nZero-Block Compression for: %d segments, %d ROS blocks, Total %d zero-blocks \n\n", segment_counter, ros_zero_blk, zero_block_counter);
//                 }

//             }
//             continue;

//             // encode_data_zero_blk(x, y, z);

//             // req_data_zero_blk = (int *)malloc(encoder_block_size * sizeof(int));
//             // required_data_zero_blk[j] = data_block[encoder_block_size * blk_num + j];
//         }
        
//         else{  // break from zero-block, encode using zero-block till now, and for current use other options
            
//             // COMPRESS DATA FOR ZERO-BLOCK then MOVE TO NORMAL COMPRESSION
//             if(zero_block_counter){
//                 zero_block_encoding(zero_block_counter, segment_counter, ros_zero_blk, ros_code, ref_sample_data, &ref_flag);
//                 printf("\nZero-Block Compression for: %d segments, %d ROS blocks, Total %d zero-blocks \n\n", segment_counter, ros_zero_blk, zero_block_counter);
//             }
//             segment_counter = 0;
//             ros_zero_blk = 0;
//             zero_block_counter = 0;

//             // its NOT an All-Zero Block, then use other options
//             // required data contains that particular block which is NOT All-Zero
//             structure_raw_data(required_data, encoder_block_size, data_point_array);
//             pre_process_raw_data(ref_sample_data, data_point_array, 8, encoder_block_size); // temporary: wordsize=8
//             encode_data_other_options(data_point_array, encoder_block_size);

//         // select an encoding option out of all options based on the least number of bits after compression, for this block only

//         }

//     }
// }

int main(){
    FILE *fptr = NULL;
    // struct number num;
    // fptr = fopen("science_data_example.bin", "rb");
    // fgetc(fptr) != EOF
    
    // data_block = (int *)malloc(encoder_block_size * sizeof(int));
    // while(i < 64){
    //     // fread(&num, sizeof(struct number), 1, fptr);
    //     fread(&num, sizeof(struct number), 1, fptr);
    //     printf("the content is: %hu >>> %x\n", num.num1, num.num1);
    //     data_block[counter - 1] = num.num1;
    //     if(counter % block_size == 0)
    //     {
    //         counter = 0;
    //         // structure the data before passing to preprocessor >> pass 16bytes data
    //         struct data_point data_point_array = structure_raw_data(data_block, encoder_block_size);
    //         // call preprocessor, pass the structured data
    //         pre_process_raw_data(0, data_point_array, encoder_block_size)
    //     }
    //     i++;
    //     counter++;
    // }
    // 2 bytes are zero-blocks
    // ASSUMING ALL DATA IS LOADED ALREADY TO SOME DATA STRUCTURE
    // data_block = []

    // int data_block[8] = {101, 101, 100, 101, 99, 101, 223, 100};

    // int data_block[8] = {101, 101, 100, 101, 99, 101, 223, 100};
    // int data_block[8] = {100, 100, 100, 100, 100, 100, 100, 100};
    unsigned short int *data_pointer;

    unsigned short int data_block[88] = {101, 101, 100, 101, 99, 101, 98, 100, // NZ (reference block)
    101, 101, 101, 101, 101, 101, 101, 101, // z - ref 101
    101, 101, 101, 101, 101, 101, 101, 101, // z - ref 101
    101, 101, 101, 101, 101, 101, 101, 101, // z - ref 101
    101, 101, 101, 101, 101, 101, 101, 101, // z - ref 101
    101, 101, 101, 101, 101, 101, 101, 101, // z - ref 101
    103, 104, 101, 98, 99, 100, 225, 100,  // NZ
    103, 103, 103, 103, 103, 103, 103, 103, // z - (reference block) 103
    103, 104, 101, 98, 99, 100, 225, 100,  // NZ
    103, 103, 103, 103, 103, 103, 103, 103, // z - ref 103
    101, 101, 101, 101, 101, 101, 101, 101}; // NZ ref-103

    int total_blocks_num = sizeof(data_block)/(sizeof(unsigned short int) * encoder_block_size); // total number of blocks in the data

    data_pointer = data_block;
    compress_loaded_data(data_pointer, total_blocks_num); // Main Method for Data Compression
    

    
    // fclose(fptr);
    return 0;

}