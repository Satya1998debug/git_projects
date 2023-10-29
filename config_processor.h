
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>

/* Configuration of the Pre-Processor && Post-Processor */
struct data_point
{
    /* data points characteristics*/
    int raw_dec_val, predicted_val, delta, theta, processed_dec_value;
};

int min(int a, int b){
    return (a > b) ? b : a;
}

void structure_raw_data(int data_block[], int encoder_block_size, struct data_point data_point_array[])
{
    int i = 0;
    while(i < encoder_block_size){
        data_point_array[i].raw_dec_val = data_block[i];
        data_point_array[i].delta = 0;
        data_point_array[i].theta = 0;
        data_point_array[i].predicted_val = 0;
        data_point_array[i].processed_dec_value = 0;
        i++;
    }
}