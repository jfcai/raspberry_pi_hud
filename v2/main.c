

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


void ShowKM(void){
    Fill_RAM(0x00);
    Display_Str(200,28,"km/h",24);
}


//显示车速
//@curSpeed  当前车速
//@lastSpeed 上次车速
void ShowSpeed(int curSpeed,int lastSpeed){


    //当前车速与上次车速一致，不重新显示
    if(curSpeed == lastSpeed) return;

    char strCurSpeed[10] = {0},strLastSpeed[10] = {0};
    sprintf(strCurSpeed,"%3d",curSpeed);
    sprintf(strLastSpeed,"%3d",lastSpeed);

    int i = 0,x1 = 14;
    for(i=0;i<3;i++){
        x1 += 8;
        if(strCurSpeed[i] == strLastSpeed[i]) continue;
        Show_Pattern(strCurSpeed[i],x1,x1 + 7,0,63);
    }
}


void ShowOled(void)
{
    int lastSpeed = 0;
    int curSpeed = 0;
    int ShowCount = 0;




    ShowKM();
    ShowSpeed(curSpeed,1);
    while(1)
    {

        //检查是否有通知信息
        if(ANSC_MSG[0] != 0){
            Fill_RAM(0x00);
            Display_Str(0,0,ANSC_MSG,16);
            memset(ANSC_MSG,0,4096);
            sleep(8);                   //显示8秒
            ShowKM();
        }


        //显示车速信息
        curSpeed = CarSpeed;
        if(curSpeed != lastSpeed)
        {

#ifdef  DEBUG
            if(ShowCount > 100){
                Fill_RAM(0x00);
                Display_Str(200,28,"km/h",24);
                ShowSpeed(curSpeed,999);
                ShowCount = 0;
            }else{
                ShowCount ++;
            }
#endif
            ShowSpeed(curSpeed,lastSpeed);
            lastSpeed = curSpeed;
        }
    }
}


//从 CAN线中读车速
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

    bcm2835_gpio_fsel(OLED_DC,    BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OLED_RST,   BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OLED_SCLK,  BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(OLED_SDIN,  BCM2835_GPIO_FSEL_OUTP);


    printf("初始化OLED屏...\n");
    OLED_INIT();

    //创建读Can 车速信息进程
    pthread_t id;
    pthread_create(&id,NULL,(void *)getCanSpeed,NULL);

    //开启蓝牙通知进程
    pthread_t id3;
    pthread_create(&id3,NULL,(void *)getANCS,NULL);

    //显示内容
    ShowOled();

    bcm2835_close();
    return 0;
}
