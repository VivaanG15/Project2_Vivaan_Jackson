#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#define D_SEAT GPIO_NUM_4
#define P_SEAT GPIO_NUM_5
#define D_SEATBELT GPIO_NUM_6
#define P_SEATBELT GPIO_NUM_7

#define GREEN GPIO_NUM_9
#define RED GPIO_NUM_10

#define BUZZER GPIO_NUM_11
#define IGNITION GPIO_NUM_12

void delay_ms(int t);
#define LOOP_DELAY_MS 25
void app_main(void)
{
    gpio_reset_pin(D_SEAT);
    gpio_set_direction(D_SEAT, GPIO_MODE_INPUT);
    gpio_pullup_en(D_SEAT);

    gpio_reset_pin(P_SEAT);
    gpio_set_direction(P_SEAT, GPIO_MODE_INPUT);
    gpio_pullup_en(P_SEAT);

    gpio_reset_pin(D_SEATBELT);
    gpio_set_direction(D_SEATBELT, GPIO_MODE_INPUT);
    gpio_pullup_en(D_SEATBELT);

    gpio_reset_pin(P_SEATBELT);
    gpio_set_direction(P_SEATBELT, GPIO_MODE_INPUT);
    gpio_pullup_en(P_SEATBELT);

    gpio_reset_pin(IGNITION);
    gpio_set_direction(IGNITION, GPIO_MODE_INPUT);
    gpio_pullup_en(IGNITION);

    gpio_reset_pin(BUZZER);
    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
    gpio_set_level(BUZZER, 0);

    gpio_reset_pin(GREEN);
    gpio_set_direction(GREEN, GPIO_MODE_OUTPUT);
    gpio_set_level(GREEN,0);

    gpio_reset_pin(RED);
    gpio_set_direction(RED, GPIO_MODE_OUTPUT);
    gpio_set_level(RED,0);

    bool d_seat, p_seat, d_belt, p_belt, ignit;
    bool green_enabled = false;

    //flags
    bool welcome_not_shown = true; 
    bool ignition_used = false; //not used yet

    while(1){
        
        d_seat = (gpio_get_level(D_SEAT) == 0);
        p_seat = (gpio_get_level(P_SEAT) == 0);
        d_belt = (gpio_get_level(D_SEATBELT) == 0);
        p_belt = (gpio_get_level(P_SEATBELT) == 0);
        ignit = (gpio_get_level(IGNITION) == 0);

        if (d_seat && welcome_not_shown) {
             printf("Welcome to enhanced alarm system model 218-W25\n");
             welcome_not_shown = false;
        }
        // Ignition enabled
        if (d_seat && p_seat && d_belt && p_belt && !ignition_used ){
            gpio_set_level(GREEN,1);
            green_enabled = true;
        }
        else {
            gpio_set_level(GREEN,0);
            green_enabled = false;
        }

        //Ignition pressed
        if (ignit && !ignition_used){
            ignition_used = true;
            if (green_enabled){
                gpio_set_level(GREEN,0);
                gpio_set_level(RED,1);
                printf("Engine started\n");
            }
            else {
                printf("Ignition inhibited\n");
                gpio_set_level(BUZZER, 1);

                if (!d_seat)
                    printf("Driver seat not occupied\n");
                if (!p_seat)
                    printf("Passenger seat not occupied\n");
                if (!d_belt)
                    printf("Driver's seatbelt not fastened\n");
                if (!p_belt)
                    printf("Passenger's seatbelt not fastened\n");
            }
        }
        delay_ms(25);
    }
}
void delay_ms(int t) {
    vTaskDelay(t /portTICK_PERIOD_MS);
}