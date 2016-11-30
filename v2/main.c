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
#include <pthread.h>
#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include "oled.h"
#include "blue.h"
#include "can.h"


char ANSC_MSG[4096] = {0};
int CarSpeed = 0;
int CarRpm = 0;
char strLastSpeed[30] = {0};


void ShowOled(void)
{
    int curSpeed = 0;
    int lastSpeed = 1;
    int count = 0;
    int msgid = 0;
    int ShowCount = 0;
    char str[30] = {0};
    //char msg[4096] = {0};
	int i = 0;

    Fill_RAM(0x00);
    Display_Str(200,28,"km/h",24);
	strcpy(strLastSpeed,"0");

    while(1)
    {
        if(ANSC_MSG[0] != 0){
            Fill_RAM(0x00);
            Display_Str(0,0,ANSC_MSG,16);
            memset(ANSC_MSG,0,4096);
            sleep(6);
            msgid = 1;
        }

        curSpeed = CarSpeed;
        if(curSpeed != lastSpeed)
        {

            //上次有显示内容，先清屏
            if(msgid > 0)
            {
                Fill_RAM(0x00);
                msgid = 0;
                Display_Str(200,28,"km/h",24);
            }

            if(ShowCount > 100){
                Fill_RAM(0x00);
                Display_Str(200,28,"km/h",24);
                ShowCount = 0;
            }

            sprintf(str,"%3d",curSpeed);
            //printf("当前速度 %s km/h\n",str);
           // Display_Number(88,0,str);
           // usleep(510000);
		   int x1 = 14;
		   for(i=0;i<strlen(str);i++){
			 x1 += 8;
			 if(str[i] == strLastSpeed[i]){
				 continue;
			 }

             Show_Pattern(str[i],x1,x1 + 7,0,63);
			 //nanosleep(1000);
		   }

//          Display_Str(120,28,str,24);       
//          Display_Bmp(88,0,str);    
            lastSpeed = curSpeed;
			strcpy(strLastSpeed,str);

            count = 0;
            ShowCount ++;

            continue;
        }

		continue;

        //printf("curSpeed=%d,lastSpeed=%d,count=%d\n",curSpeed,lastSpeed,count);


        if(curSpeed == 0 && lastSpeed == 0 && count > 20 && count % 20 == 0) msgid = 0;

        if(curSpeed == 0) count ++;

        //当停车状态超过3秒，从信息库中读取一条信息并显示，显示后删除
        // if(curSpeed == 0 && lastSpeed == 0 && msgid == 0 && count >= 20){
        //     msgid = get_1_Msg(msg);
        //     printf("%s\n",msg);
        //     if(msgid > 0){
        //         printf("显示内容:%s\n",msg);
        //         //OLED_INIT();
        //         Fill_RAM(0x00);
        //         Display_Str(0,0,msg,16);
        //         del_1_Msg(msgid);
        //     }
        // }

        //count ++;

        //printf("curSpeed=%d,lastSpeed=%d,count=%d\n",curSpeed,lastSpeed,count);
        
        usleep(500000);
    }
}

void getCanSpeed(void){
    int i;
    while(1)
    {
        if(read_can_info() == -1){
            printf("读取CAN速度失败，正在重试...\n");
            sleep(5);
            i ++;
        }

        if(i > 10)
        {
            Display_Str(0,0,"读取CAN速度失败，正在重试",24);
            i = 0;
        }
    }
}

int main(int argc, char **argv)
{

    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 0;
    }

    //if (!bcm2835_spi_begin())
    //{
    // printf("bcm2835_spi_begin failedg. Are you running as root??\n");
    //  return 1;
    //}

    //bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    //bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    //bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); // The default
    //bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
    //bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default

    //bcm2835_gpio_fsel(OLED_DC,    BCM2835_GPIO_FSEL_OUTP);
    //bcm2835_gpio_fsel(OLED_RST,   BCM2835_GPIO_FSEL_OUTP);
    printf("初始化OLED屏...\n");
    OLED_INIT();
    //Fill_RAM_DEALY(0xff,100000000);
    //Fill_RAM_DEALY(0x00,100000000);

    //Fill_RAM(0x00);
    //Display_Str(0,0,"系统正在启动...",16);
    //sleep(1);


    //显示欢迎转而
    //Fill_RAM(0x00);
    //Display_Str(0,0,"正在启动,请稍候...",24);
    //sleep(10);

    //printf("读取IP地址...\n");
    //getIP();
    //sleep(5);
    
   // exit(0);
    //创建读GPS信息进程
    pthread_t id;
    pthread_create(&id,NULL,(void *)getCanSpeed,NULL);

//    pthread_t id2;
//    pthread_create(&id2,NULL,(void *)getNews,NULL);

    //开启蓝牙进程
    pthread_t id3;
    pthread_create(&id3,NULL,(void *)getANCS,NULL);

    //显示内容
    ShowOled();

    //bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
