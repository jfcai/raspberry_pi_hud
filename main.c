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
#include "oled.h"


void OLED_TEST(void)
{
    Set_Column_Address(Shift+0,Shift+0+5); // 设置列坐标，shift为列偏移量由1322决定。3为16/4-1
    Set_Row_Address(0,23); 
    Set_Write_RAM();

    //0x20,0x50,0x88,0x88,0xF8,0x88,0x88,0x00
    //Con_4_byte(0x20);
    //Con_4_byte(0x50);
    //Con_4_byte(0x88);
    //Con_4_byte(0x88);
    //Con_4_byte(0xF8);
    //Con_4_byte(0x88);
    //Con_4_byte(0x88);
    //Con_4_byte(0x00);
   // OLED_WR_Byte(0xff,OLED_DATA);
   // OLED_WR_Byte(0x00,OLED_DATA);

    unsigned char GB_24[] =        // 数据表
{
      0x00,0x00,0x00,0x00,0xF8,0x00,0x7E,0xFF,
      0xFE,0x7E,0xDF,0xFE,0x66,0xD8,0x0C,0x66,
      0xD8,0x0C,0x66,0xDB,0xEC,0x66,0xF3,0xEC,
      0x66,0xF3,0x6C,0x66,0xF3,0x6C,0x66,0xF3,
      0x6C,0x66,0xDB,0x6C,0x66,0xDB,0x6C,0x66,
      0xDB,0x6C,0x7E,0xDB,0xEC,0x7E,0xDB,0xEC,
      0x7E,0xFB,0xEC,0x66,0xFB,0x6C,0x60,0xF3,
      0x0C,0x60,0xC0,0x0C,0x00,0xC0,0x0C,0x00,
      0xC0,0x7C,0x00,0xC0,0x3C,0x00,0x00,0x00
};

    int i;
    for(i=0;i<72;i++)
    {
        Con_4_byte(GB_24[i]);
    }
    // for (i=23;i>0;i--)
    // {
    //     Con_4_byte(GB_24[i*3+0]);
    //     Con_4_byte(GB_24[i*3+1]);
    //     Con_4_byte(GB_24[i*3+2]);
    // }
/*
     HZ24_24(0,30,"夫");
     HZ24_24(24,30,"唯");
     HZ24_24(48,30,"不");
     HZ24_24(72,30,"争");
     HZ24_24(96,30,"则");
     HZ24_24(120,30,"天");
     HZ24_24(144,30,"下");
     HZ24_24(168,30,"莫");
     HZ24_24(192,30,"能");
     HZ24_24(216,30,"与");


     HZ24_24(0,  0,"之");
     HZ24_24(24, 0,"争");
     HZ24_24(48, 0,"这");
     HZ24_24(72, 0,"个");
     HZ24_24(96, 0,"是");
     HZ24_24(120,0,"老");
     HZ24_24(144,0,"子");
     HZ24_24(168,0,"说");
     HZ24_24(192,0,"的");
     HZ24_24(216,0,"不");
     HZ24_24(240,0,"是");
  
*/
    
    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xF0,OLED_DATA);

    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xF0,OLED_DATA);

    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xF0,OLED_DATA);

    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xF0,OLED_DATA);

    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xF0,OLED_DATA);

    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xF0,OLED_DATA);

    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xFF,OLED_DATA);

    // OLED_WR_Byte(0xFF,OLED_DATA);
    // OLED_WR_Byte(0xFF,OLED_DATA);
    //OLED_WR_Byte(0x20,OLED_DATA);
    //OLED_WR_Byte(0x20,OLED_DATA);
    //OLED_WR_Byte(0x20,OLED_DATA);
    //OLED_WR_Byte(0x20,OLED_DATA);
    //OLED_WR_Byte(0x20,OLED_DATA);
   /* int i;
    for (i=0;i<10000;i++)
    {
        unsigned char str[100] = {0};
        sprintf(str,"%03d",i);
        Asc24_48(0,0,str);
    }
    */
   // Asc20_40(0,0,"Hello Wrod!");

}
int main(int argc, char **argv)
{
    // If you call this, it will not actually access the GPIO
// Use for testing
 //       bcm2835_set_debug(1);


    OLED_INIT();
    OLED_TEST();

    //bcm2835_spi_end();
    //bcm2835_close();
    return 0;
}
