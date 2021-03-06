#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>      
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <signal.h> 
#include <sys/time.h>
#include <string.h> 
#include "oled.h"
#include "blue.h"
#include "can.h"
#include "db.h"

//#ifdef BCM2835_CORE_CLK_HZ
//  #undef BCM2835_CORE_CLK_HZ
//  #define BCM2835_CORE_CLK_HZ 1000000
//#endif

char ANSC_MSG[4096] = {0};
int CarSpeed = 0;
int CarRpm = 0;
char strLastSpeed[30] = {0};


//显示停车时的信息
void ShowStopInfo(int signo){
    char str[1024] = {0};
    int msgID = 0;

    if (CarSpeed == 0){
        if((msgID = get_1_Msg(str)) > 0){
            Fill_RAM(0x00);
            Display_Str(0,0,str,16);
        }  
    }
}

//显示单位
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

void SetTimer(int sec){
    struct itimerval tick;
    memset(&tick, 0, sizeof(tick));

    //Timeout to run first time
    tick.it_value.tv_sec = sec;
    tick.it_value.tv_usec = 0;

    //After first, the Interval time for clock
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
        printf("Set timer failed!\n");

}

void ShowOled(void)
{
    int lastSpeed = 0;
    int curSpeed = 0;

#ifdef DEBUG
    int ShowCount = 0;
#endif


    ShowKM();
    ShowSpeed(curSpeed,1);
    while(1)
    {
        curSpeed = CarSpeed;

        //检查是否有通知信息
        if(ANSC_MSG[0] != 0){
            Fill_RAM(0x00);
            Display_Str(0,0,ANSC_MSG,16);
            memset(ANSC_MSG,0,4096);
            sleep(8);                   //显示8秒
            ShowKM();
			ShowSpeed(curSpeed,999);
        }


        //显示车速信息
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
            continue;
        }


        //检查是否停车状态,如果为停车状态则启动定时器
        if(curSpeed == 0){
            struct itimerval tick;
            getitimer(ITIMER_REAL,&tick);
            if(tick.it_value.tv_sec == 0){
                printf("进入定时器\n");
                SetTimer(6);
            }
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

    //sleep(1);
    printf("初始化OLED屏...\n");
    OLED_INIT();
	Fill_RAM(0x00);

    
	//创建读Can 车速信息进程
    pthread_t id;
    pthread_create(&id,NULL,(void *)getCanSpeed,NULL);

    //开启蓝牙通知进程
    pthread_t id3;
    pthread_create(&id3,NULL,(void *)getANCS,NULL);

    //注册信号
    signal(SIGALRM, ShowStopInfo);

    //显示内容
    ShowOled();

    bcm2835_close();
    return 0;
}
