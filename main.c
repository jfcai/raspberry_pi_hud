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
#include "oled.h"
#include "gps.h"
#include "db.h"
#include "content.h"

void ShowOled(void)
{
    int curSpeed = 0;
    int lastSpeed = 1;
    int count = 0;
    int msgid = 0;
    unsigned char str[30] = {0};
    unsigned char msg[4096] = {0};

    Fill_RAM(0x00);
    Display_Str(200,28,"km/h",24);

    while(1)
    {
        curSpeed = gps_speed();
        if(curSpeed != lastSpeed)
        {

            //上次有显示内容，先清屏
            if(msgid > 0)
            {
                Fill_RAM(0x00);
                msgid = 0;
                Display_Str(200,28,"km/h",16);
            }

            sprintf(str,"% 3d",curSpeed);
            printf("%s\n",str);
            Display_Number(48,0,str);
            lastSpeed = curSpeed;

            count = 0;
        }


        if(curSpeed == 0 && lastSpeed == 0 && count > 20 && count % 20 == 0) msgid = 0;

        //当停车状态超过3秒，从信息库中读取一条信息并显示，显示后删除
        if(curSpeed == 0 && lastSpeed == 0 && msgid == 0 && count >= 20){
            msgid = get_1_Msg(msg);
            if(msgid > 0){
                printf("显示内容:%s\n",msg);
                Fill_RAM(0x00);
                Display_Str(0,0,msg,16);
                del_1_Msg(msgid);
            }
        }

        count ++;

        //printf("curSpeed=%d,lastSpeed=%d,count=%d\n",curSpeed,lastSpeed,count);
        
        usleep(500000);
    }
}

void getNews(void)
{
    while(1)
    {
        getTopNews("top");
        getTopNews("keji");
        getTopNews("caijing");
        sleep(1800);
    }
}

void getGpsSpeed(void)
{
    while(1)
    {
        if(gps() == -1){
            printf("GOLO 连接失败，正在重试...\n");
        }
    }
}

int main(int argc, char **argv)
{
    OLED_INIT();
    Fill_RAM_DEALY(0xff,100000000);
    Fill_RAM_DEALY(0x00,100000000);

    Display_Number(0,0,"12345678");
    sleep(2);

    //显示欢迎转而
    Fill_RAM(0x00);
    Display_Str(0,0,"正在启动,请稍候...",24);
    sleep(3);
    
   // exit(0);
    //创建读GPS信息进程
    pthread_t id;
    pthread_create(&id,NULL,(void *)getGpsSpeed,NULL);

    pthread_t id2;
    pthread_create(&id2,NULL,(void *)getNews,NULL);

    //显示内容
    ShowOled();

    //bcm2835_spi_end();
    bcm2835_close();
    return 0;
}
