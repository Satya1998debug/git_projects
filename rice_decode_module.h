#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include"post_processor.h"


int fill_extract_extension_params(int gamma){
    /* The 2nd-Extension Table is constructued by this method */
    int index = 1;
    int ms = 1;
    int beta = 1;
    int params_array[gamma + 1];
    params_array[0] = 0;
    int params;

    for(int i = 1; i < gamma; i = ms){
        if(index > gamma){
            break;
        }
        // index = i; this is the case at the start of each block
        while (index >= i && index <= i + beta)
        {
            if(beta >= 4){
                params_array[index] = (beta * 100) + ms;
            }
            else{
                params_array[index] = (beta * 10) + ms;
            }
            index++;
        }
        beta++;
        ms = ms + beta;
    }

    params = params_array[gamma];
    return params;
    
}

void decompress_second_extension(char *bit_seq, int bit_pos, unsigned short int *required_data, int encoder_block_size){
    /* This method performs 2nd Extension Decoding  */
    bit_pos++; // move to next bit position, to start checking bit by bit
    int zero_count = 0;
    char character;
    int gamma = 0;
    int beta = 0;
    int ms = 0;
    int data_index = 0;
    
    // bit_pos < total_bits
    while(data_index < encoder_block_size){ // iterate till end of sequence, till we get all J samples
        character = bit_seq[bit_pos];
        
        // if the character is 0, then increment the zeros (keep counting for same sample)
        if(character == (char)48){
            zero_count++;
        }
        // if the character is 1, then zero counting for one sample is done, we need to move to next sample. 
        else if (character == (char)49)
        {
            // Extract the data value from the zero counts.
            gamma = zero_count; // number of 0's gives the decimal value of the sample data   

            // Initiate the zero counting of the next sample
            zero_count = 0; // reset the value of zero counter  

            // make the table and get paramters
            int param = fill_extract_extension_params(gamma);

            // Extract the values of beta & ms from the tabular data
            if(gamma > 9){
                beta = param / 100;
                ms = param % 100;
            }
            else{
                beta = param / 10;
                ms = param % 10;
            }

            required_data[data_index + 1] = gamma - ms;
            int data2 = required_data[data_index + 1];
            required_data[data_index] = beta - required_data[data_index + 1];
            int data1 = beta - required_data[data_index + 1];;
            data_index = data_index + 2;
        }
        // if nothing is there, may be empty space/null character, then pass or continue
        else{
            continue;
        };
        bit_pos++; // to move to next bit position
    }
}

int get_dec_value(char *bit_sequence, int seq_lenth){
    int i = 0;
    int num = 0; // decimal value of the given sequence
    while (i < seq_lenth)
    {
        if(bit_sequence[i] == (char)49){
            num += pow(2, seq_lenth - 1 - i);
        }
        i++;
    }
    return num; // for other values of k != 16
    
    
}

int get_k_value(char *bit_sequence, int seq_lenth){
    int i = 0;
    int num = 0; // decimal value of the given sequence
    while (i < seq_lenth)
    {
        if(bit_sequence[i] == (char)49){
            num += pow(2, seq_lenth - 1 - i);
        }
        i++;
    }

    if(num == pow(2, seq_lenth) - 1){ // means if num = 15 i.e '1111' that means "NO Compression", k = 16
        num = pow(2, seq_lenth) + 1; // num = 16 + 1.. so that outside this method k = num-1 = 17-1 = 16 i.e wordsize+1
        return num;
    }

    return num; // for other values of k != 16
    
    
}

void decompress_k_split(char *bit_seq, int bit_pos, unsigned short int *required_data, int encoder_block_size, int k){

    bit_pos++; // increments bit_pos so that now it points to the fisrt bit of the k-bits sequence
    int sample_num = 0;

    /* Then extract all the FS code sequences for all sample data points */
    int zero_count = 0; // stores the number of 0's, which is basically the equivalent decimal value of decompressed data
    int dec_val = 0;
    unsigned short int data1[8];

    if(k < word_size){ // k-split
        printf("K-Split De-Compression, K=%d\n", k);
        // now bit_pos is at the start of FS code sequnce
        while(sample_num < encoder_block_size){ // till we have all J samples
            
            // if the character is 0, then increment the zeros (keep counting for same sample)
            if(*(bit_seq + bit_pos) == (char)48){
                zero_count++;
            }
            // if the character is 1, then zero counting for one sample is done, we need to move to next sample. 
            else if (*(bit_seq + bit_pos) == (char)49)
            {
                // Extract the data value from the zero counts.
                dec_val = zero_count; // number of 0's gives the decimal value of the sample decompressed data   

                // Initiate the zero counting of the next sample
                zero_count = 0; // reset the value of zero counter 

                // calculate the decompressed data, club k-bits & nk-bits together
                /*
                    In this method, 
                        dec_val = contains the FS seq decimal equivalent, i.e number of 0's
                        required_data[sample_num] = already contains the decimal equivalent of k-bits
                
                */
                // required_data[sample_num] = dec_val * pow(2, k) + required_data[sample_num]; // unique method adotpted
                required_data[sample_num] = dec_val * pow(2, k); // unique method adotpted
                int x = required_data[sample_num];

                sample_num++; // increment sample number to keep a track on the number of data samples read already

            }
            // if nothing is there, may be empty space/null character, then pass or continue
            else{
                continue;
            };
            bit_pos++; // to move to next bit position
        }

        int end_pos_k_bits = bit_pos + encoder_block_size * k;

        
        /* Then Extract the k-bits for all sample data points */
        // now bit_pos at the start of k-bits 
        int bits_num = 1;
        char k_bits[k];
        sample_num = 0;
        while (bit_pos < end_pos_k_bits) // this condition can be changed with calcuated position
        {
            // read the bit to k-bits character array
            k_bits[bits_num - 1] = bit_seq[bit_pos];
            bits_num = bits_num % k; // when bits_num = k, the it will reset the bits_num to 0, this will then initiate binary to decimal conversion
            if(bits_num == 0){ // it will be 0, when we read all k-bits for a particular sample data in the block of J-samples

                // take full k_bits 
                // convert to decimal
                // place in required data array
                required_data[sample_num] += get_dec_value(k_bits, k); // convert the k-bits to decimal 
                sample_num++;
                
            }
            bits_num++;
            bit_pos++;
        }
    }
    else{ // NO Compression
        printf("No Compression, Skipped\n");
        // loop over exact no. of bits i.e k * encoder_blocksize = 16 * 8 of data
        /* Then Extract the k-bits for all sample data points */
        // now bit_pos at the start of k-bits 
        // int bits_num = 1;
        // char k_bits[word_size];
        // sample_num = 0;
        // int end_pos_k_bits = bit_pos + encoder_block_size * word_size;

        // while (bit_pos < end_pos_k_bits) // this condition can be changed with calcuated position
        // {
        //     // read the bit to k-bits character array
        //     k_bits[bits_num - 1] = bit_seq[bit_pos];
        //     bits_num = bits_num % word_size; // when bits_num = k, the it will reset the bits_num to 0, this will then initiate binary to decimal conversion
        //     if(bits_num == 0){ // it will be 0, when we read all k-bits for a particular sample data in the block of J-samples

        //         // take full k_bits 
        //         // convert to decimal
        //         // place in required data array
        //         data1[sample_num] = get_dec_value(k_bits, k);
        //         required_data[sample_num] += get_dec_value(k_bits, word_size); // convert the k-bits to decimal 
        //         sample_num++;
                
        //     }
        //     bits_num++;
        //     bit_pos++;
        // }
          
    }

    
    
    
    

}

int check_ros(char *bit_seq, int *bit_pos){
    /* 
        check_ros method check if the bit_seq is a ROS block or not.
        Returns 1 (if its a ROS block)
        Return 0 (if the block is Not ROS block)
    
    */
    int init_bit_pos = *bit_pos;
    int bits_to_check = 5;
    while (bits_to_check > 0)
    {
        if(*(bit_seq + *bit_pos) == (char)49){ // if 1 comes between 5 0s, means an error in bit sequence  and it will break the loop
            break;
        }
        bits_to_check--;
        (*bit_pos)++;
    }
    // printf("ros-check1...bits_to_check=%d\n", bits_to_check);

    // after breaking or completing, bits_to_check should be 1
    if(bits_to_check == 1){ // it has completed loop OR it has found '1' at the the end of sequence
        (*bit_pos) += bits_to_check - 1; // to reach the final value of bit_pos
        // printf("ros-check2...bits_to_check=%d\n", bits_to_check);
        return 1;
    }
    else{ // bits_to_check is not 1, menas it has not yet checked those many bits yet and found '1' before cehcking all 5 bits
        // means it is not a ROS code, so set bit_pos to initial value(value at the time of entering the method)
        *bit_pos = init_bit_pos; // bit reset to initial position
        return 0;
    }
    // printf("ros-check3...bits_to_check=%d\n", bits_to_check);
    

}

int check_zero_id(char *bit_seq, int *bit_pos){
    int bits_to_check = 5; // total number of bits to check
    while (bits_to_check > 0)
    {
        if(*(bit_seq + *bit_pos) == (char)49){ // if 1 comes between 5 0s, means its not ID because all bits ar 0s
            return 0;
        }
        bits_to_check--;
        (*bit_pos)++;
    }
    return 1; // if it successfully completes the loop, means all bits were 0s, so it is ID block
}

int check_seg_code(char *bit_seq, int *bit_pos){
    int bits_to_check = 6; // total number of bits to check
    while (bits_to_check > 0) // out of 6, 5 bits are 0s, so we just need to check 
    {
        if(*(bit_seq + *bit_pos) == (char)49){ // if 1 comes between 6 0s, means its not ID because all bits ar 0s
            if(bits_to_check == 1){ // fi its the last bit to check and its value is 1, means everything is fine and it is SEG code
                return 1;
            }
            else{
                return 0;
            }
        }
        bits_to_check--;
        (*bit_pos)++;
    }
    return 0; // incase any error occurs, it will return 0
}

int decompress_zero_block(char *bit_seq, int bit_pos, int encoder_block_size, int total_bits){
    bit_pos++; // at the start of the encoded sequence
    int segments = 0;
    int ros_blks = 0;
    int pause_flag = 0;
    int total_blocks = 0;

    // zero block ID is already check at the beginning of the sequnece, when this method was called
    // now bit_pos is at the begnining of the actual data sequnce

    /* Start reading the SEG code & segments in the sequnce, (If Available) */
    if(check_seg_code(bit_seq, &bit_pos)){ // if 1, then SEG code block
        // this means next set of block indicates the number of segments
        bit_pos++; // at the start of the new sequnce, ready to read segments
        while(!pause_flag){
            if(bit_seq[bit_pos] == (char)49){ // it will be 1, at the end of sequnce as its FS code, it will break at the end
                bit_pos++;
                pause_flag = 1;
                continue;
            }
            bit_pos++;
            segments++; // number of zeros = no. of segments, ecounter with each 0, the segment number will increment by 1
        }
    }
    // printf("segments=%d\n",segments);


    pause_flag = 0; // reset the pause flag, before we start again
    /* Read ROS code & remaining of sequence zero_blks (If Available), Else read the last remaining zero_blocks */
    if(check_ros(bit_seq, &bit_pos)){ // if its a ROS code, means zero_blks >=  5, after ROS code block & formula will be different
        bit_pos++;
        while(!pause_flag){
            if(bit_seq[bit_pos] == (char)49){ // it will be 1, at the end of sequnce as its FS code, it will break at the end
                // printf("bit-pos=%d\n", bit_pos);
                // printf("ros-blk ends\n");
                bit_pos++;
                pause_flag = 1;
                continue;
            }
            // printf("ros-blk continues\n");
            bit_pos++;
            ros_blks++; // number of zeros = no. of zero block remaining at the end, ecounter with each 0, the zero_blks will increment by 1
        }
        // printf("ros-blocks1=%d\n",ros_blks);
        ros_blks--; // actual zero blocks will be 1 less than no. of zeros (modified formula table) if zero_blks >= 5
        pause_flag = 0;

    }
    else{// ROS code is not there, so directly read the ros_blocks and ros_blks <= 4, the formula will be same as mentioned in book
        /* Check the end of bit sequnce */  
        // If end is reached, stop & exit, ROS blocks = 0 (because NO ROS code available and End of sequence is triggered)
        // total bits needed to check End of Sequence
        if(bit_pos == total_bits){
            ros_blks = 0;
        }
        else{
            // Else continues to check the ROS blocks       
            while(!pause_flag){
                if(bit_seq[bit_pos] == (char)49){ // it will be 1, at the end of sequnce as its FS code, it will break at the end
                    bit_pos++;
                    pause_flag = 1;
                    continue;
                }
                bit_pos++;
                ros_blks++; // number of zeros = no. of zero block remaining at the end, ecounter with each 0, the zero_blks will increment by 1
            }
            ros_blks++; // actual zero blocks will 1 more than no. of zeros (original formula table) if zero_blks <= 4

        }

    }
    // printf("ros-blocks2=%d\n",ros_blks);

    /* we have now ros_blks & segments , we can now generate full data*/
    
    total_blocks = segments * s_seg_size + ros_blks; // 1 segment has seg_size 0-blks + ros remaining blocks
    // unsigned short int req_data[(*total_blocks) * encoder_block_size]; //  Each 0-blk has ecoder_size(J) samples
    // int i = 0;
    
    return total_blocks;
}

void extract_raw_data_zero_blk(int ref_sample_data, unsigned short int *req_data, int total_blocks, int encoder_block_size){
    /*
        This methods extracts the original data from the pre-processed data.
        This module acts as post processor.
    */
    int i = 0;
    for(int i=0; i< total_blocks * encoder_block_size; i++){
        req_data[i] = ref_sample_data; // the actual data will be same as reference data 
    }
}

void decompress_data(char *bit_seq, int total_bits, int encoder_block_size, unsigned short ref_sample_data, 
                        unsigned short int *req_data, int *samples_to_read, int *zero_compress_flag){
    int option_id_val = -1; // default value, means invalid option, so no execution
    int bit_pos = 0;
    int option_bits_max = 5; // maximum bits that is used for option ID (here its 5 i.e for zero block '00000')
    char option_id[5];

    char zero_option_id[] = "00000"; // currently, its hardcoded only for 16 bit wordsize
    char second_ext_option_id[] = "00001";  // currently, its hardcoded only for 16 bit wordsize

    int total_blocks = 0;
    
    if(!strncmp(bit_seq, zero_option_id, option_bits_max)){ // check for 0-block extension
        *zero_compress_flag = 1;
        printf("Zero-Block De-Compression\n");
        // decompress zero block
        bit_pos = option_bits_max - 1; // now its at 5th index, bit_pos = 4
        // int total_bits = 17; // need mostly in case of 0-block decompression
        int total_blocks = 0;
        // decompress zero block
        total_blocks = decompress_zero_block(bit_seq, bit_pos, encoder_block_size, total_bits); // returns the total blocks 
        int blk_num = 0;
        *samples_to_read = total_blocks * encoder_block_size; // read (total_blocks x encoder_block_size) total samples
    }
    else if(!strncmp(bit_seq, second_ext_option_id, option_bits_max))  // check for 2nd extension
    {
        printf("Second Extension De-Compression\n");
        // decompress 2nd extension
        bit_pos = option_bits_max - 1; // now its at 5th index, bit_pos = 4
        decompress_second_extension(bit_seq, bit_pos, req_data, encoder_block_size);
        
        struct data_point data_point_array[encoder_block_size];
        extract_raw_data(ref_sample_data, req_data, data_point_array, word_size, encoder_block_size);

        // req_data = required_data;
        *samples_to_read = encoder_block_size; // read only 8 samples

    }
    else{ // then it must have been either FS, K-split or NO-compression
        char k_split_option_id[option_bits_max - 1];
        memcpy(k_split_option_id, bit_seq, 4); // copy fisrt 4 bits of charac
        // calculate value of k
        int k = get_k_value(k_split_option_id, option_bits_max - 1) - 1;
        bit_pos = option_bits_max - 2; // now its at 3rd index
        decompress_k_split(bit_seq, bit_pos, req_data, encoder_block_size, k);
        
        struct data_point data_point_array[encoder_block_size];
        extract_raw_data(ref_sample_data, req_data, data_point_array, word_size, encoder_block_size);

        // req_data = required_data;
        *samples_to_read = encoder_block_size; // read only 8 samples
    }

}

void decode_compressed_data(char *bit_seq, int total_bits, int encoder_block_size, unsigned short int ref_sample_data){

    unsigned short int *all_data;
    unsigned short int req_data[encoder_block_size]; // for 2nd sextension & k-split decompression as it is blockwise
    int samples_to_read = 0;
    int zero_compress_flag = 0;
    decompress_data(bit_seq, total_bits, encoder_block_size, ref_sample_data, req_data, 
                            &samples_to_read, &zero_compress_flag); // returns pointer to the array of data

    unsigned short int required_data_zb[samples_to_read]; // for 0-block decompression, as we dont know the number of blocks
    if(zero_compress_flag){
        int total_blocks = samples_to_read/encoder_block_size;
        printf("total blocks: %d \n", total_blocks);
        extract_raw_data_zero_blk(ref_sample_data, required_data_zb, total_blocks, encoder_block_size);
        all_data = required_data_zb;
    }
    else{
        all_data = req_data;
    }
    
    // print all data
    printf("Raw Data is: \n");
    for(int i=0;i<samples_to_read;i++){
        printf("%x,",*(all_data +i));
    }

    /******/
    

}

