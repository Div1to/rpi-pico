#include "gpio_i2c.h"

int main(){
    // am2320 sensor example
    gpio_i2c bus;
    gpio_i2c_init(&bus,0,1,true);
    gpio_i2c_wake(&bus,0x5c,800);
    uint8_t data[]={0x03,0x00,0x02};
    uint8_t rdata[8];
    gpio_i2c_write(&bus,0x5c,data,3);
    sleep_us(1500);
    gpio_i2c_read(&bus,0x5c,rdata,8,2);
    return 1;
}