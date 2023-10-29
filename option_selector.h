#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<limits.h>


void add_second_ext_id(char *bit_seq, int *bit_pos, int option_id){
    /* This method adds 2nd Extension compression Option ID */
    int x = option_id; // specifies the length of option ID block
    while (x > 0)
    {
        if(x == 1){
            *(bit_seq + *bit_pos) = (char)49;  // '1'
        }
        else{
            *(bit_seq + *bit_pos) = (char)48; // '0'
        }
        (*bit_pos)++;
        x--;
    }
}

void add_ksplit_fs_option_id(char *bit_seq, int *bit_pos, int *option_id){
    /* Assume that its a 4-bit number */
    int x = *option_id; // specifies the length of option ID block
    int b;
    *bit_pos = 3;  // the binary to decimal conversion takes place in a reverse order of the division, so initialized at 3
    while(x > 0){
        b = x % 2;
        x = x / 2;
        if(b){  // If b(binary bit) is 1, the add one else a 0 is added
            *(bit_seq + *bit_pos) = (char)49;   // '1'
            (*bit_pos)--;  // the bit position keeps on decreasing till 0

        }
        else{
            *(bit_seq + *bit_pos) = (char)48;  // '0'
            (*bit_pos)--;  // the bit position keeps on decreasing till 0
        }
    }
    // append 0 to the left till bit_pos = 0
    while (*bit_pos >= 0)
    {
        *(bit_seq + *bit_pos) = (char)48;
        (*bit_pos)--;
    }
    

    // now the bit_pos is '0', so it has to be initilazed to the value after the decimal conversion, i.e 4 (because 0,1,2,3 have binary bits of equivalent decimal after conversion)
    *bit_pos = 4;  // ToDo
}

void add_zero_id_option(char *bit_seq, int *bit_pos){
    /* This Method adds the Zero Block compression Option ID*/
    int x = 5; // specifies the length of option ID block which is 5 for '00000'
    while (x > 0)
    {
        *(bit_seq + *bit_pos) = (char)48; // Add '0' x times, like x=5... '00000'
        (*bit_pos)++;
        x--;
    }
}

void select_options(unsigned int *total_bits_count, int word_size, int *min_index){
    /* This method will SORT the min_index list(list of indices) as per descending order of Total number of bits of various compression techniques */

    // find index that has minimum value of total bits
    // we initialize this as the maximum value, so that it can never be graeter than this value
    // i.e J samples with each having maximum possible value with FS sequnce(k=0) ... ex: J(8) x (255+1)
    long long int min_index_bits = LLONG_MAX;
    int j = 0;
    // get the index having minimum bits, j =0 means its the index has the minimum total bits
    for(int i = 1; i < word_size; i++){ // its NOT "i <= wordsize" because we have to exclude NO-Compression case
        if(total_bits_count[i] < min_index_bits){
            min_index_bits = *(total_bits_count + i);
            *(min_index + j) = i;
        }
    }
    j++; // now the value of j = 1, initially j = 0

    // check if any other index with same minimum bits
    for(int i = 1; i < word_size; i++){ // its NOT "i <= wordsize" because we have to exclude NO-Compression case
        // if the bits are same and index are different then only consider the compression options
        if(total_bits_count[i] == min_index_bits && i != *(min_index)){
            *(min_index + j) = i;
            j++;
        }
    }

}



