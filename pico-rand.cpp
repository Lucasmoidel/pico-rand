#include <iostream>
#include <string>
#include <sstream>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "pico/rand.h"
#include <random>
extern "C" {
#include "ssd1306.h"
#include "font.h"
}

void update(int x, int y);

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16  
#define I2C_SCL 17

ssd1306_t disp;

int arr[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
int len = sizeof(arr) / sizeof(arr[0]);
int main(){
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    gpio_init(7);
    gpio_set_dir(7, GPIO_IN);
    gpio_pull_up(7);

    gpio_init(12);
    gpio_set_dir(12, GPIO_IN);
    gpio_pull_up(12);

    gpio_init(11);
    gpio_set_dir(11, GPIO_IN);
    gpio_pull_up(11);

    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c0);
    ssd1306_clear(&disp);

    ssd1306_draw_string_with_font(&disp, 0, 0, 4, font_8x5, "!");

    ssd1306_show(&disp);

    bool generate = false;
    bool changeup = false;
    bool changedn = false;
    int x = 0;
    int students = 30;
   
    while (true){

        if (!changeup && !gpio_get(11)){
            changeup = true;
        }
        if (changeup && gpio_get(11)){
            changeup = false;
            students--;
            
            update(x, students);
        }

        if (!changedn && !gpio_get(12)){
            changedn = true;
        }
        if (changedn && gpio_get(12)){
            changedn = false;
            students++;
            
            update(x, students);
        }

        if (!generate && !gpio_get(7)){
            generate = true;
        }
        if (generate && gpio_get(7)){
            generate = false;
            for (int i = 0; i < len-1; i++){
                arr[i] = arr[i+1];
            }
            arr[len-1] = x;
            x = (get_rand_32()%students)+1;
            while(x == arr[len-1]){
                x = (get_rand_32()%students)+1;
            }
            
            update(x, students);
        }
        sleep_ms(50);
    }
    return 0;
}

void update(int x, int y){
    ssd1306_clear(&disp);
    int scale = 5;

    std::stringstream stream;
    stream.str("");
    stream.clear();
    stream << x;

    ssd1306_draw_string_with_font(&disp, (128 - ((5*scale)*stream.str().length())+((stream.str().length()-1)*scale))/2, (64 - (8*scale))/2, scale, font_8x5, stream.str().c_str());

    std::stringstream num;
    stream.str("");
    stream.clear();
    stream << y;

    ssd1306_draw_string_with_font(&disp, 128 - (((5*2)*stream.str().length())+((stream.str().length()-1)*2)), (64 - (8*2))/2, 2, font_8x5, stream.str().c_str());

    for (int i = 0; i < 3; i++){
        stream.str("");
        stream.clear();
        if (arr[i+5] > 0){
            stream << arr[i];
        }
        ssd1306_draw_string_with_font(&disp, 0, (i*(8*2))+(i*8), 2, font_8x5, stream.str().c_str());
    }


    
    ssd1306_show(&disp);

}