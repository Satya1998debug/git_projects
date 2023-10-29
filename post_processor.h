
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include"config_processor.h"
#include"config.h"

/* main program for initiation */
void extract_raw_data(int init_predicted_val, unsigned short int *req_data, struct data_point *data_point_array, int word_size, int array_size){
    
    /* input variables & initialization: 
        - raw data array(non-negative hexa-decimals), 
        - predictor array(decimals)
        - delta array(decimals), 
        - preprocessed data array(can be decimals or binary, depends on output we need),
        - number of data points
    */

    /* convert hex to decimals */
    int raw_max_val = (int)(pow(2, word_size) - 1);
    int raw_min_val = 0;
    int previous_val = 0;
    previous_val = init_predicted_val;

    for(int data_num = 0; data_num < array_size; data_num++){
        /* calculate & fill the array with delta based on predicted values >>> unit delay preprocessor */
        data_point_array[data_num].processed_dec_value = req_data[data_num];

        /* calculate Theta(i) */
        // calculate theta (based on min & max value of data)
        data_point_array[data_num].theta = min(previous_val - raw_min_val, raw_max_val - previous_val);

        // check the conditions for Theta(i)
        // calculate Delta(i), based on Theta(i) conditions
        if(data_point_array[data_num].processed_dec_value <= 2 * data_point_array[data_num].theta){
            if(data_point_array[data_num].processed_dec_value % 2 == 0){ // processed value is even
                data_point_array[data_num].delta = data_point_array[data_num].processed_dec_value / 2;
            }
            else{ // processed value is odd
                data_point_array[data_num].delta = -1 * (data_point_array[data_num].processed_dec_value + 1)/ 2;
            }

        }
        else{
            if(data_point_array[data_num].theta == previous_val - raw_min_val){
                data_point_array[data_num].delta = data_point_array[data_num].processed_dec_value - data_point_array[data_num].theta;
            }
            else if(data_point_array[data_num].theta == raw_max_val - previous_val){
                data_point_array[data_num].delta = data_point_array[data_num].theta - data_point_array[data_num].processed_dec_value;
            }

        }

        // calcuate original samples X(i) from Delta(i)
        data_point_array[data_num].raw_dec_val = data_point_array[data_num].delta + previous_val;
        req_data[data_num] = data_point_array[data_num].raw_dec_val; // get raw data value
        data_point_array[data_num].predicted_val = previous_val;
        previous_val = data_point_array[data_num].raw_dec_val; // assign previous value, that will be the predicted value for next sample
        
    
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