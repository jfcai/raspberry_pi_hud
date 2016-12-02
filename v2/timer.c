#include <stdio.h>        //printf()
#include <unistd.h>        //pause()
#include <signal.h>        //signal()
#include <string.h>        //memset()
#include <sys/time.h>    //struct itimerval, setitimer()

static int count = 0;

void printMes(int signo)
{
    printf("Get a SIGALRM, %d counts!\n", ++count);
    sleep(5);
    printf("sleep\n");
}

int main()
{
    int res = 0;
    struct itimerval tick;
    
    memset(&tick, 0, sizeof(tick));
    printf("%d\n",getitimer(ITIMER_REAL,&tick));

    signal(SIGALRM, printMes);
    


    //Timeout to run first time
    tick.it_value.tv_sec = 5;
    tick.it_value.tv_usec = 0;

    //After first, the Interval time for clock
    tick.it_interval.tv_sec = 0;
    tick.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
            printf("Set timer failed!\n");


    sleep(2);

    getitimer(ITIMER_REAL,&tick);
    printf("it_value.tvsec=%ld,it_interval.tv_sec=%ld\n",tick.it_value.tv_sec,tick.it_interval.tv_sec);


    //Timeout to run first time
    //tick.it_value.tv_sec = 0;
    //tick.it_value.tv_usec = 0;

    //After first, the Interval time for clock
    //tick.it_interval.tv_sec = 0;
    //tick.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
            printf("Set timer failed!\n");

    //When get a SIGALRM, the main process will enter another loop for pause()
    while(1)
    {
        pause();
    }
    return 0;
}