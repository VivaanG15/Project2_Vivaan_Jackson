#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

//Delay function declaration
void delay_ms(int t);

#define D_WEIGHT_BUTTON GPIO_NUM_4 //Button for Driver Weight Sensor
#define D_SEATBELT_BUTTON GPIO_NUM_14 //Button for Driver Seatbelt Sensor
#define P_WEIGHT_BUTTON GPIO_NUM_9 //Button for Passenger Weight Sensor
#define P_SEATBELT_BUTTON GPIO_NUM_16 //Button for Passenger Seatbelt Sensor
#define IGNITION_BUTTON GPIO_NUM_1

#define GREEN_LED GPIO_NUM_35
#define BLUE_LED GPIO_NUM_39
#define BUZZER GPIO_NUM_21

void app_main(void)
{
   //setup for pins as outputs
   gpio_reset_pin(GREEN_LED);
   gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);
   gpio_set_level(GREEN_LED, 0);
   gpio_reset_pin(BLUE_LED);
   gpio_set_direction(BLUE_LED, GPIO_MODE_OUTPUT);
   gpio_set_level(BLUE_LED, 0);
   gpio_reset_pin(BUZZER);
   gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
   gpio_set_level(BUZZER, 0);

   //setup for pins as inputs
   gpio_reset_pin(D_WEIGHT_BUTTON);
   gpio_set_direction(D_WEIGHT_BUTTON, GPIO_MODE_INPUT);
   gpio_reset_pin(D_SEATBELT_BUTTON);
   gpio_set_direction(D_SEATBELT_BUTTON, GPIO_MODE_INPUT);
   gpio_reset_pin(P_WEIGHT_BUTTON);
   gpio_set_direction(P_WEIGHT_BUTTON, GPIO_MODE_INPUT);
   gpio_reset_pin(P_SEATBELT_BUTTON);
   gpio_set_direction(P_SEATBELT_BUTTON, GPIO_MODE_INPUT);

   //variables to keep track of the green light and the ignition
   bool green_led_ready = true;
   bool green_led_on = false;
   bool ignition_pressed = false;
   bool welcome_txt = false;

   //main body loop
   while (!ignition_pressed)
   {
       if (!gpio_get_level(D_WEIGHT_BUTTON) && welcome_txt==false) //senses when driver sits down and welcome him/her to the system
       {
           printf("Welcome to enhanced alarm system model 218-W25\n");
           welcome_txt=true;
       }

       if (green_led_ready && ((!gpio_get_level(D_WEIGHT_BUTTON) && !gpio_get_level(D_SEATBELT_BUTTON)) &&
       (!gpio_get_level(P_WEIGHT_BUTTON) && !gpio_get_level(P_SEATBELT_BUTTON))))// senses if all conditions are met for ignition
       {
           gpio_set_level(GREEN_LED, 1);
           green_led_on = true;
       }
       else
       {
           green_led_on = false;
           gpio_set_level(GREEN_LED, 0);
       }

       if (green_led_on && !gpio_get_level(IGNITION_BUTTON))//ignition started and everything ready
       {
           ignition_pressed = true;
           gpio_set_level(BLUE_LED, 1);
           printf("Engine Started\n");
           green_led_ready = false;
           gpio_set_level(GREEN_LED, 0);  
       }
       else if (!green_led_on && !gpio_get_level(IGNITION_BUTTON))//ignition started and not everything ready
       {
           ignition_pressed = true;
           gpio_set_level(BUZZER, 1);
           printf("Ignition Prohibited\n");

           if(gpio_get_level(D_WEIGHT_BUTTON))
           {
               printf("driver seat not occupied\n");
           }
           if(gpio_get_level(D_SEATBELT_BUTTON))
           {
               printf("driver seatbelt not on\n");
           }
           if(gpio_get_level(P_WEIGHT_BUTTON))
           {
               printf("passenger seat not occupied\n");
           }
           if(gpio_get_level(P_SEATBELT_BUTTON))
           {
               printf("passenger seatbelt not on\n");
           }
       }
       delay_ms(20);
   }
}
//A function to specify delays in milliseconds
void delay_ms(int t) {
   vTaskDelay(t /portTICK_PERIOD_MS);
}