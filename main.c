// spi.c
//
// Example program for bcm2835 library
// Shows how to interface with SPI to transfer a byte to and from an SPI device
//
// After installing bcm2835, you can build this 
// with something like:
// gcc -o spi spi.c -l bcm2835
// sudo ./spi
//
// Or you can test it before installing with:
// gcc -o spi -I ../../src ../../src/bcm2835.c spi.c
// sudo ./spi
//
// Author: Mike McCauley
// Copyright (C) 2012 Mike McCauley
// $Id: RF22.h,v 1.21 2012/05/30 01:51:25 mikem Exp $
#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "oled.h"


void OLED_TEST(void)
{
    Fill_RAM(0x00);
    Display_Str(0,20,"欢迎使用HUD,谨慎驾驶",24);
}

void Car_Speed(void)
{
    unsigned char str[30] = {0};
    Fill_RAM(0x00);
    Display_Str(0,20,"当前车速:",24);

    srand(time(0));
    while(1)
    {
        sprintf(str,"%  d KM/H  ",rand()%200 + 1);
        Display_Str(112,20,str,24);
        sleep(1);
    }
}

int main(int argc, char **argv)
{
    OLED_INIT();
    OLED_TEST();

    sleep(5);
    Car_Speed();
    //bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
