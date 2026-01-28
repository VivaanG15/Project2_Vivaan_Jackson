#include "freertos/FreeRTOS.h"
#include "esp_adc/adc_oneshot.h"


#define ADC_CHANNEL     ADC_CHANNEL_4
#define ADC_ATTEN       ADC_ATTEN_DB_12
#define BITWIDTH        ADC_BITWIDTH_12
#define DELAY_MS        10                  // Loop delay (ms)
#define NUM_SAMPLES     1000                // Number of samples
#define BUF_SIZE        16                  // Number of points to average


void app_main(void)
{
   
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };                                                  // Unit configuration
    adc_oneshot_unit_handle_t adc1_handle;              // Unit handle
    adc_oneshot_new_unit(&init_config1, &adc1_handle);  // Populate unit handle
   
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = BITWIDTH
    };                                                  // Channel config
    adc_oneshot_config_channel                          // Configure the chan
    (adc1_handle, ADC_CHANNEL, &config);
   
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .chan = ADC_CHANNEL,
        .atten = ADC_ATTEN,
        .bitwidth = BITWIDTH
    };                                                  // Calibration config
    adc_cali_handle_t adc1_cali_chan_handle;            // Calibration handle
    adc_cali_create_scheme_curve_fitting                // Populate cal handle
    (&cali_config, &adc1_cali_chan_handle);
   
    int index = 0;                      // Index of newest entry into buffer
    int buffer[BUF_SIZE] = {0};         // Circular buffer of mV readings (mV)
   
    for (int i = 0; i < NUM_SAMPLES; i++) {             // Loop for NUM_SAMPLES
       
        int adc_bits;                                   // ADC reading (bits)
        adc_oneshot_read
        (adc1_handle, ADC_CHANNEL, &adc_bits);          // Read ADC bits
       
        int adc_mv;                                     // ADC reading (mV)
        adc_cali_raw_to_voltage
        (adc1_cali_chan_handle, adc_bits, &adc_mv);     // Convert to mV
       
        buffer[index] = adc_mv;                         // Store mV reading
        index = (index + 1) % BUF_SIZE;                 // Increment and wrap
       
        int mv_filt = 0;                                // Sum of data (mV)
       
        int navg;                       // Number of points to average
        if (i < BUF_SIZE){              // Until the buffer is full
            navg = i + 1;               // only average the number read so far.
        } else {                        // Otherwise,
            navg = BUF_SIZE;            // use the full buffer.
        }


        for (int j = 0; j < navg; j++) {            // Loop through buffer
             mv_filt = mv_filt + buffer[j];         // summing the data
        }
       
        mv_filt = mv_filt / navg;                   // Compute average (mV)
       
        int time_ms = i * DELAY_MS;                 // Loop time (ms)
       
        printf("%d %d %d\n", time_ms, adc_mv, mv_filt);
       
        vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
    }
   
}
