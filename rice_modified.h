#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include"option_selector.h"
#include"preprocessor_modified.h"
// #include"rice_decode_module.h"

int write_to_file_compressed_data(char bit_char){

    char *filename = "compressed_data1.txt";

    // open the file for writing
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }
    // write to the text file

    fprintf(fp, "%c", bit_char);

    // close the file
    fclose(fp);
    return 0;
}

char* fs_sequence_code(char *bit_seq, int nk_bits, int *bit_pos){
    /* This method converts the decimal value to FS */

    int n = nk_bits;
    int loop = 0; // loop flag is 1, of the loop is executed atleast once, else 0
    while(n > 0){
        *(bit_seq + *bit_pos) = (char)48; // n-k Zeros
        if(n == 1){
            *(bit_seq + *bit_pos + 1) = (char)49; // followed by last One
        }
        (*bit_pos)++;
        n--;
        loop = 1;
        }

    if(n == 0 && loop == 1){
        // after the above loop ends, the bit_pos need to be incremented by 1 i.e. to the next point, so that nxt time FS code will start from the same point
        (*bit_pos)++;
    }
    
    if(nk_bits == 0){  // if nk_bits = 0, then only add '1' because FS code for 0 is '1'
        *(bit_seq + *bit_pos) = (char)49;
        (*bit_pos)++;
    }

    return bit_seq;

}

char* k_bit_placement_as_is(char *bit_seq, int *bit_pos, int k, int k_bits, int word_size){
    /* This method takes the k-bits(starting from LSB) of the binary representation of the decimal value, and places As-Is to the bit sequence */
    int y = 0;
    while(y < k)
    {
        if((k_bits & ((int)pow(2, k - y - 1))) > 0){
            *(bit_seq + *bit_pos) = (char)49; // '1'
        }
        else{
            *(bit_seq + *bit_pos) = (char)48; // '0'
        }
        y++;
        (*bit_pos)++;
    }

    return bit_seq;

}

void copy_k_seq(char *bit_seq, char *k_seq, int k_pos, int *bit_pos){
    /* This method Copies the k-bits sequence into the actual output bit sequence */
    int i = 0;
    while(i <= k_pos){
        *(bit_seq + *bit_pos) = *(k_seq + i);
        (*bit_pos)++;
        i++;
    }

}

void initialize_char_array(char *seq, int size){
    /* This method just Initializes the sequence array with a value '2' (ASCII 50) just to make sure there is NO garbage value by default */
    int i = 0;
    while(i < size){
        *(seq + i) = (char)50;
        i++;
    }
}

void kth_split_sequence(unsigned short int data_stream[], char *bit_seq, 
                        int block_size, 
                        int k, int word_size, unsigned int *total_bits_count, int implement){

    /* This method compresses the data using k-Split compression techniques, based on the value of k */

    /* 1 loop for each data point */
    int total_bits = 0;
    int option_id;
    int option_bits = 4; // need to add this into mem_size
    /* This loop is for each data sample for the entire block of J-samples */
    // This loop will run J times i.e. number of times = block_size value (for all J samples)
    

    if(implement){ // implement flag is 1, if it has to compress data, else it will only count total bits required for the compression
        int mem_size = 0;
        int bit_pos = 0;
        int bit_pos_k_seq = 0;
        char *k_seq;

        // extract the total number of bits needed, so that we can calculate mem_size to contruct array of required length
        // value of k is used calcualte the index, then extract the total_bits
        if(k < word_size){
            mem_size = *(total_bits_count + 2 + k);
            total_bits = mem_size;
            option_id = k + 1;
        }
        else{
            mem_size = *(total_bits_count + k);  // no compression
            total_bits = mem_size;
            option_id = k - 1;
        }

        total_compressed_bits = total_bits; // store actual total compressed bits, when the data conpression actually happens

        // array of characters with size = mem_size (total bits required)
        char c_arr[mem_size]; // the last element is a null character '\0'
        bit_seq = c_arr;
        initialize_char_array(bit_seq, mem_size);
        char k_arr[block_size * k];
        k_seq = k_arr;
        initialize_char_array(k_seq, block_size * k);

        /* IMPLEMENT COMPRESSION K-SPLIT OR NO-COMPRESSION */
        add_ksplit_fs_option_id(bit_seq, &bit_pos, &option_id);


        for(int i=0; i < block_size; i++){   
            if(k < word_size){ // Executed for all k =0,1,....N-3 options of k-split encoding
                int nk_bits = data_stream[i] >> k;  // n-k MSB bits
                int k_bits = data_stream[i] & ((int)(pow(2, k)) - 1); // LSB bits
                option_id = k + 1;
                        
                // mem_size = (int)nk_bits + k + 1; // 1 is for the last '1' in the FS code 
                // bit_seq = (char *)malloc(mem_size * sizeof(char));
                // k_seq = (char *)malloc((k + 1) * sizeof(char));

                /* FS sequence of n-k bits */ 
                bit_seq = fs_sequence_code(bit_seq, nk_bits, &bit_pos); // Directly, add in bit_seq, because FS codes will be added at the beginning

                /* place k-bits as-is */
                k_seq = k_bit_placement_as_is(k_seq, &bit_pos_k_seq, k, k_bits, word_size);
                        
                // printf("%s For data >>>> %x \n", bit_seq, data_stream[i]);

                /* total number of bits (can be calculated irrespective of pointer or variables) */
                // int data_total_bits = nk_bits + k + 1;
                // total_bits += data_total_bits;
                        
                // printf("total bits for this data: %d\n", data_total_bits);

                /* conver to binary byte by byte and write to file */
                // for(int i = 0; i<data_total_bits; i++){
                //     bit_seq[i]
                // }
                // free(bit_seq);

            }
            else{  // for No-compression option, when the else part is executed, k = 16
                int nk_bits = 0;
                int k_bits = data_stream[i];
                option_id = k - 1; // example: for wordsize=16, option_id = 16-1 = 15...i.e '1111' after encoding
                int bit_pos = 0;

                // mem_size = (int)nk_bits + k + 1;
                // k_seq = (char *)malloc(mem_size * sizeof(char));


                /* place all k-bits as-is, k = word_size (No Compression) */
                k_seq = k_bit_placement_as_is(k_seq, &bit_pos_k_seq, k, k_bits, word_size);
                        
                // printf("%s For data >>>> %x \n", bit_seq, data_stream[i]);

                /* total number of bits */
                // int data_total_bits = nk_bits + k + 1;
                // total_bits += data_total_bits;
                        
                // printf("total bits for this data: %d\n", data_total_bits);

                /* conver to binary byte by byte and write to file */
                // for(int i = 0; i<data_total_bits; i++){
                //     bit_seq[i]
                // }
                // free(bit_seq);
            }


        }

        /* COPY k_seq TO bit_seq AS K-SPLIT BITS WILL AT THE END */
        copy_k_seq(bit_seq, k_seq, bit_pos_k_seq, &bit_pos);
        
        // Print the Data
        printf("\n Data Compressed!!!! \n");
        for(int i = 0; i < total_bits; i++){
            printf("%c", *(bit_seq + i));
            write_to_file_compressed_data(*(bit_seq + i));
        }

        // free(k_seq);

    }
    else{ // this segment only counts the total number of bits after the comression is done, used when we want to compare with other compression techniques
        total_bits+=4; // for option ID for k-split

        if(k < word_size){
            for(int i=0; i < block_size; i++){
                /* total number of bits (can be calculated irrespective of pointer or variables) */
                int nk_bits = data_stream[i] >> k; // when k = 16(no compression), nk_bits will automatically 0 (because word_size is 16), so only 'k' will remain
                int data_total_bits = nk_bits + k + 1;
                total_bits += data_total_bits;
            }
            *(total_bits_count + 2 + k) = total_bits;
        }
        else{  // no compression
            /* total number of bits (can be calculated irrespective of pointer or variables) */
            // when k = 16(no compression), nk_bits will automatically 0 (because word_size is 16), so only 'k' will remain
                total_bits += block_size * word_size;
                *(total_bits_count + k) = total_bits;  // no compression
            }
    } 
    
    // printf("grand total bits for k=%d: %d\n", k, total_bits);
}

void split_sample_encoding(char *bit_seq, unsigned short int data_stream[], 
                            int word_size, int block_size, 
                            unsigned int *total_bits_count, int implement){
    /* This method executes k-Split compression & No Compression */
    
    /* Loop for each option K except the No-compression*/
    for(int k = 0; k <= word_size - 3; k++)
    {
        /* implement kth split from k=0 to k=N-3 */
        kth_split_sequence(data_stream, bit_seq, block_size, k, word_size, total_bits_count, implement);
    }

    /* Implement No-Compression ith K = wordsize */
    kth_split_sequence(data_stream, bit_seq, block_size, word_size, word_size, total_bits_count, implement);

}

void second_extension(char *bit_seq, unsigned short int data_stream[], 
                        int block_size, unsigned int *total_bits_count, int implement){

    /* This method compresses using 2nd Extension technique */
    unsigned short int processed_data[block_size/2];
    int total_bits = 0;
    int option_id = 4; // FS code for 4 is '00001' 
    if(implement){
        int bit_pos = 0;
        total_bits = *(total_bits_count + 1); // get the total_bits to make an array

        total_compressed_bits = total_bits; // store actual total compressed bits, when the data conpression actually happens

        char c_arr[total_bits]; // the last element is a null character '\0'
        bit_seq = c_arr;
        initialize_char_array(bit_seq, total_bits);

        /* Add Option ID for 2nd Extension compression */
        bit_seq = fs_sequence_code(bit_seq, option_id, &bit_pos);
        // add_second_ext_id(bit_seq, &bit_pos, option_id);

        for(int i = 0; i < block_size; i= i + 2){
            int data_sample = data_stream[i];
            int nxt_data_sample = data_stream[i+1];
            processed_data[i/2] = (data_sample + nxt_data_sample) * (data_sample + nxt_data_sample + 1)/2 + nxt_data_sample;
            int nk_bits = processed_data[i/2];
            // bit_seq = (char *)malloc(mem_size * sizeof(char));
            bit_seq = fs_sequence_code(bit_seq, nk_bits, &bit_pos);
        }

        // Print the data after its compressed
        printf("\n2nd Extension compression is executed\n");
        for(int i=0; i<total_bits; i++){
            printf("%c", *(bit_seq + i));
            write_to_file_compressed_data(*(bit_seq + i));
        }
    }
    else{
        for(int i = 0; i < block_size; i = i + 2){
            processed_data[i/2] = (data_stream[i] + data_stream[i+1]) * (data_stream[i] + data_stream[i+1] + 1)/2 + data_stream[i+1];
            int nk_bits = processed_data[i/2];
            int mem_size = nk_bits + 1;
            total_bits+=mem_size;

        }
        total_bits+=5;
        *(total_bits_count + 1) = total_bits;
        // printf("Grand total bits >>> %d", total_bits);
    }
    
}

void zero_block_encoding(int zero_blks, int segments, int ros_blks, int ros_code_val, int ref_sample, int *ref_flag){
    int option_val = 5; // option ID is '00000', this only indicates the number of bits in the option ID
    char *bit_seq;
    int seg_code_val = 6; // indicates the number of bits required for SEG code "000001"
    /* 
    If the number of all 0-blocks == segment size, then only ROS code will be there.
    If the number of blocks < segment size, then FS code sequence will determine the length of of all 0-blocks
    */
    int mem_size = 0;
    // optionID + SEGCode(optional) + #NumberOfSegments(optional) + ROSCode(optional) + #ros_blks

    if(segments){ // if we have some segments then only we will have segment code, followed by number of segments
        mem_size = option_val + seg_code_val + (segments + 1);
    }
    else{ // if segments = 0, then no segment code
        mem_size += option_val;
    }
    // printf("This check is okay..1c1\n");

    if(ros_blks >= 5){ // if ROS blocks >= 5, then 1 extra ROS code is needed at the end 
        mem_size += ros_code_val + ros_blks;
    }
    else{
        mem_size += ros_blks;
    }
    // printf("This check is okay..1c2\n");

    // handle for ros block==0, at that time we dont need option ID also
    // printf("reference flag: %d\n",*ref_flag);

    if(*ref_flag){  // reference sample exist so the mem_size must contain that, ref_flag=1 only in case of the reference packet
        mem_size = mem_size + 16; // 16 is the word_size of the reference sample
        // printf("This check is okay..1c1c\n");
    }

    int bit_pos = 0; 
    char c_arr[mem_size]; // the last element is a null character '\0'
    bit_seq = c_arr;
    initialize_char_array(bit_seq, mem_size);

    /* Add zero ID option at the beginning */
    add_zero_id_option(bit_seq, &bit_pos);
    /* Add reference sample data as is in binary format */
    if(*ref_flag){
        bit_seq = k_bit_placement_as_is(bit_seq, &bit_pos, word_size, ref_sample, word_size);
        *ref_flag = 0;
    }

    /* if segments are available  */
    if(segments){
        /* FS sequence of SEG code i.e '000001' , so seg_code_val(6) - 1 = 5  is addaed */
        bit_seq = fs_sequence_code(bit_seq, seg_code_val - 1, &bit_pos); // SEG code is added, indicating the presence of segments
        bit_seq = fs_sequence_code(bit_seq, segments, &bit_pos); // FS sequence for #number of segments get added to bit sequence
    }

    /* For the Last segment, ROS code sequnce is already added */
    // check for ROS remaining blocks
    if(ros_blks){
        if(ros_blks >= 5){
            /* FS sequence of ROS code i.e '00001' */
            bit_seq = fs_sequence_code(bit_seq, ros_code_val - 1, &bit_pos); // add ROS code "00001"
            // if it has ros blocks >= 5, then FS code is different ex: for 6 blocks, the code "0000001" equivalent of 7
            bit_seq = fs_sequence_code(bit_seq, ros_blks + 1, &bit_pos);
        }
        else{
            /* FS sequence of ROS remaing zero blocks i.e for 3 blocks...it takes value like this '001' equivalent of 2 */
            bit_seq = fs_sequence_code(bit_seq, ros_blks - 1, &bit_pos);
        }
        
    }

    total_compressed_bits = mem_size; // store actual total compressed bits, when the data conpression actually happens
    
    printf("\n\n");
    printf("The zero blocks are coded as:   ");
    for(int i = 0; i<bit_pos; i++){
        printf("%c", bit_seq[i]);
        // write_to_file_compressed_data(bit_seq[i]);
    }
    printf("\n\n");
    // printf("Reference Data:%d\n", ref_sample_data);
    // free(bit_seq); 
}

void compress_data(char *bit_seq, int *min_index, unsigned short int data_stream[], int word_size,
                    int block_size, unsigned int *total_bits_count, int implement){
    
    int ksplit_index_min = word_size; //maximum value  of k=16

    /* PRACTICE & TESTING */
    // second_extension(bit_seq, data_stream, block_size, total_bits_count, implement);
    // kth_split_sequence(data_stream, bit_seq, block_size, word_size, word_size, total_bits_count, implement);



    // sort the array min_index in descending order
    for(int i = 0; i < 4; i++){
        // check for No-Compression, if available (No Cpmression will not be executed now onwards)
        if(*(min_index + i) == word_size){  // k=16
            // compress using No-compression
            break; // No-compression will not be executed as per requirement
            kth_split_sequence(data_stream, bit_seq, block_size, word_size, word_size, total_bits_count, implement);
            printf("\n No compression executed..\n");
            ksplit_index_min = -1;  // in order to avoid k-split compression after the Loop
            // break;
        }
        else if(*(min_index + i) == 1){
            // printf("This check is okay..5\n");
            // compress using 2nd-Extension
            second_extension(bit_seq, data_stream, block_size, total_bits_count, implement);
            printf("\n Second Extension compression executed..\n");
            ksplit_index_min = -1;  // in order to avoid k-split compression after the Loop
            break;
        }
        else{ //  0 <= k <= 16
            if(*(min_index + i) < ksplit_index_min && *(min_index + i) != -1){
                // swap to keep a track of minimum value of k in the array
                ksplit_index_min = *(min_index + i);
                // so that at the end of loop, the minimum value of k is preserved and k-split can be applied
            }
        }
    }

    // check if it is still -1, that means other compression algorithms are not yet used, so minimum value of k must be preserved
    if(ksplit_index_min != -1){
        /* Implement k-split with minimum k (ksplit_index_min) */

        // Extract K then send the value of K to split encoding
        int k = ksplit_index_min - 2;
        // printf("This check is okay..6\n");
        if(k >= 0 || k <= word_size - 3){
            kth_split_sequence(data_stream, bit_seq, block_size, k, word_size, total_bits_count, implement);
            printf("\n K-Split with k=%d, index=%d compression executed..\n", k, ksplit_index_min);
        }
    }

    printf("Reference Data:%d\n", ref_sample_data);
    // printf("This check is okay..7\n");

    // DATA COMPRESSION COMPLETE, PRINT DATA
    

}

void encode_data_other_options(struct data_point *data_point_array, int block_size)
{
    /* This method executes compression for k-split, FS sequence, No-compression, 2nd extension techniques except zero-block compression */

    // (*main_counter)++;
    unsigned short int data_stream[block_size];

    for(int i=0; i < block_size; i++){
        data_stream[i] = data_point_array[i].processed_dec_value;
    }
    int word_size = 16;  // word size in bits, size of each word or sample
    int actual_word_size = 12;  // actual word size in bits....as of now NOT USED
    char *bit_seq;
    int implement = 0;
    /* index-0: zero block
       index-1: second extension
       index-2: FS sequnce(k=0) split
       index-3: ksplit(k=1) ...[2 + (k=1)]
       index-4: ksplit(k=2) ...[2 + (k=2)]
       index-5: ksplit(k=3) ...[2 + (k=3)]
       .
       .
       .
       index-15: ksplit(k=13) ...[2 + (k=13)]
       index-16: for NO compression data // Not implemented in this code
       ----------------------------------------

       total 17 indices (16 + 1 i.e. word_size + 1)..from i=0 to 16
    */

    unsigned int total_bits_count[word_size + 1];  // this array keeps the total number of bits used after compression

    total_bits_count[0] = -1; // zero block is NOT included here
    total_bits_count[16] = -1;
    total_bits_count[15] = -1;
    total_bits_count[14] = -1;
    total_bits_count[13] = -1;
    total_bits_count[12] = -1;
    total_bits_count[11] = -1;

    /* zero block encoding */
    // zero_block_encoding(bit_seq, data_stream, &option_id, word_size, block_size, total_bits_count);
    
    /* implement k-split sequence & FS sequence including no-compression */
    split_sample_encoding(bit_seq, data_stream, word_size, block_size, total_bits_count, implement);
    /* implement second extension */
    second_extension(bit_seq, data_stream, block_size, total_bits_count, implement);  // the block size need to be even
    int x =0;

    // printf("This check is okay..3\n");
    while (x < word_size + 1)
    {
        printf("index: %d ... Total bits: %d \n", x, total_bits_count[x]);
        x++;
    }

    // SELECT THE COMPRESSION OPTION
    int min_index[4] = {-1,-1,-1,-1}; // this list will contain the indices
    select_options(total_bits_count, word_size, min_index);
    // printf("This check is okay..4\n");

    // print min index
    // for(int i=0;i<4;i++){
    //     printf("\n Compress Options available \n");
    //     printf("%d,", min_index[i]);
    // }

    // CALL FINAL COMPRESSION METHOD & RETURN BIT SEQUENCE
    implement = 1;
    compress_data(bit_seq, min_index, data_stream, word_size, block_size, total_bits_count, implement);
    printf("\n DATA COMPRESSED \n"); 

    /*****/


}

int get_ros_seg_enncoding(){
    /* This method makes the ros_code */
    int ros_num_dec = s_seg_size;
    if(s_seg_size >= 2){ // hopefully, segment size will always > 5 ...ideally s_seg_size
        ros_num_dec = 5; // ros_num
        return ros_num_dec;
    }
    else{
        return ros_num_dec;
    }
    
}

int is_all_zero_block(int required_data[], int encoder_block_size){
    /* checks if the given block is a zero block, if all samples have the same value then it will be a zero block */
    
    for(int i = 0; i < encoder_block_size; i++){
        if(required_data[i] - ref_sample_data != 0){  // some sample is noy equal, so return from there itself
            return 0;  // its NOT an All-Zero Block
        }
    }
    // printf("This check is okay..1a\n");
    return 1;  // otherwise its All-Zero Block
}

void compress_loaded_data(unsigned short *data_block, int total_blocks_num){
    // unsigned int main_counter = 0;
    /*
      
      compress_loaded_data method executes all compression techniques.
      data_block: its a pointer to the actual data array(unsigned short)
      total_blocks_num: int value indicating total number of data blocks(of J samples each) avialbale to process
    
    */

    struct data_point data_point_array[encoder_block_size];

    int zero_block_counter = 0; // counts the number of zero blocks
    int segment_counter = 0; // counts number of segments
    int ros_zero_blk = 0; // count the ROS sequnce blocks
    int blk_num, sample_num;

    // int first_data_flag = 1;

    // ros_code indicates the number of bits required for the FS code for ros decimal equivalent
    int ros_code = get_ros_seg_enncoding(); // gets ROS code decimal value, so that later it can be encoded in FS code

    int ref_flag = 0; // indicates that the refernce sample is present if value = 1

    // Loop-1 on the total blocks... block-by-block, blk_num=0 to total blocks
    for(blk_num = 0; blk_num < total_blocks_num; blk_num++){
        int required_data[encoder_block_size];
        // printf("This check is okay..-1\n");
        
        // update the reference flag, to check if its a normal data block or 1st data block
        if(ref_flag){
            ref_flag = 0;
        }
        
        // Loop-2 on the data J samples, ... data-by-data, sample_num=0 to block size(8)
        for(sample_num = 0; sample_num < encoder_block_size; sample_num++){
            // loading data from data_block(original data) to required_data(data to be processed) array
            required_data[sample_num] = *(data_block + encoder_block_size * blk_num + sample_num);
            // printf("This check is okay..0\n");

            // check if it is the reference block and save the reference sample data
            if(blk_num%r_ref_interval == 0 && sample_num == 0){
                ref_flag = 1; // make the reference flag as True(1)
                ref_sample_data = required_data[sample_num]; // extract reference sample
                // printf("This check is okay..1\n");
            }
        }

        /* Check for Zero-block and Encode */
        if(is_all_zero_block(required_data, encoder_block_size)){ // return 1, if its zero-block, else return 0
            zero_block_counter++;
            segment_counter = zero_block_counter / s_seg_size; // number of complete segments, each segment has s_seg_size 0-blocks 
            ros_zero_blk = zero_block_counter % s_seg_size;  // remainder-of-sequence.. remaining number of 0-blocks
            if(blk_num == total_blocks_num - 1){ // if this is the last block & its 0-block, then better compress this before ending the code
                // compress with 0-block option as its the last block
                if(zero_block_counter){
                    printf(" This is the last block");
                    zero_block_encoding(zero_block_counter, segment_counter, ros_zero_blk, ros_code, ref_sample_data, &ref_flag);
                    // main_counter++;
                    printf("\nZero-Block Compression for: %d segments, %d ROS blocks, Total %d zero-blocks \n\n", segment_counter, ros_zero_blk, zero_block_counter);
                }

            }
            continue;

            // encode_data_zero_blk(x, y, z);

            // req_data_zero_blk = (int *)malloc(encoder_block_size * sizeof(int));
            // required_data_zero_blk[j] = data_block[encoder_block_size * blk_num + j];
        }
        
        else{  // break from zero-block, encode using zero-block till now, and for current use other options
            
            // COMPRESS DATA FOR ZERO-BLOCK then MOVE TO NORMAL COMPRESSION
            // printf("This check is okay..1b\n");
            if(zero_block_counter){
                // printf("This check is okay..1c\n");
                zero_block_encoding(zero_block_counter, segment_counter, ros_zero_blk, ros_code, ref_sample_data, &ref_flag);
                // main_counter++;
                printf("\nZero-Block Compression for: %d segments, %d ROS blocks, Total %d zero-blocks \n\n", segment_counter, ros_zero_blk, zero_block_counter);
                // printf("This check is okay..1d\n");
            }
            segment_counter = 0;
            ros_zero_blk = 0;
            zero_block_counter = 0;

            // its NOT an All-Zero Block, then use other options
            // required data contains that particular block which is NOT All-Zero
            structure_raw_data(required_data, encoder_block_size, data_point_array);
            pre_process_raw_data(ref_sample_data, data_point_array, word_size, encoder_block_size); // temporary: wordsize=8
            // printf("This check is okay...2\n");
            encode_data_other_options(data_point_array, encoder_block_size);

        // select an encoding option out of all options based on the least number of bits after compression, for this block only

        }

    }

    // printf("\nMain Counter: %d\n", main_counter);
}