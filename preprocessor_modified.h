
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include"config_processor.h"
#include"config.h"

/* main program for initiation */
void pre_process_raw_data(int init_predicted_val, struct data_point *data_point_array, int word_size, int encoder_block_size){
    
    /* input variables & initialization: 
        - raw data array(non-negative hexa-decimals), 
        - predictor array(decimals)
        - delta array(decimals), 
        - preprocessed data array(can be decimals or binary, depends on output we need),
        - number of data points
    */

    /* convert hex to decimals */
    int raw_max_val = (int)(pow(2, word_size) - 1), raw_min_val = 0, previous_val;
    previous_val = init_predicted_val; // previous value is the decimal eqv. of previous raw data
    for(int data_num = 0; data_num < encoder_block_size; data_num++){
        /* calculate & fill the array with delta based on predicted values >>> unit delay preprocessor */
        data_point_array[data_num].delta = data_point_array[data_num].raw_dec_val - previous_val;
        data_point_array[data_num].predicted_val = previous_val;
        previous_val = data_point_array[data_num].raw_dec_val;
        
        /* calculate theta (based on min & max value of data)>>> this will be used in every iteration and its constant for each data point*/
        data_point_array[data_num].theta = min(data_point_array[data_num].predicted_val - raw_min_val, raw_max_val - data_point_array[data_num].predicted_val);

        /* calculate & fill preprocessed data based on theta & conditions */
        if (data_point_array[data_num].delta >= 0 && data_point_array[data_num].delta <= data_point_array[data_num].theta){
            data_point_array[data_num].processed_dec_value = 2 * data_point_array[data_num].delta;

        }
        else if (data_point_array[data_num].delta <= 0 && data_point_array[data_num].delta >= (-1) * data_point_array[data_num].theta)
        {
            data_point_array[data_num].processed_dec_value = 2 * abs(data_point_array[data_num].delta) - 1;
        }
        else{
            data_point_array[data_num].processed_dec_value = data_point_array[data_num].theta + abs(data_point_array[data_num].delta);
        }
    
    }
     
    /* DATA DISPLAY */
    int i = 0;
    printf("Raw-Hex    Raw-Decimal    Predicted-value    Delta    Theta    Pre-processed-Value \n\n");
    while (i < 8)
    {
        printf("%x        %d         %d         %d          %d         %d \n", data_point_array[i].raw_dec_val, data_point_array[i].raw_dec_val, data_point_array[i].predicted_val, data_point_array[i].delta, data_point_array[i].theta, data_point_array[i].processed_dec_value);
        i++;
    }
    
    /* pre-processed data is ready for further use */

    /* convert data to binary or Hex or decimal (based on the output needed)*/
    // printf("%d \n",a);
}