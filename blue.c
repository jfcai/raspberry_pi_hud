#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <stdlib.h>


#define TRUE 1
#define FALSE 0

#define BUFFER_LENG 4096
extern char ANSC_MSG[4096];

/* for baud rate and parity configuration */
static int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);

        if (tcgetattr (fd, &tty) != 0)
        {
                perror("set_interface_attribs: error from tcgetattr");
                return -1;
        }

        tcflush(fd,TCIOFLUSH);  //刷清输入、输出队列
        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        //tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
                                        // 
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
    //    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
    //    tty.c_cflag &= ~CSTOPB;
    //    tty.c_cflag &= ~CRTSCTS;
    

        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 0;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl



        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                perror("set_interface_attribs: error from tcsetattr");
                return -1;
        }
        return 0;
}




/* open the serial port */
static int open_port(char *dev)
{
   	
  	int fd; // File descriptor for the port 

  	fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
  	if (fd == -1)
  	{
    	perror("open_port: Unable to open this device");
  	}
  	else{
    	fcntl(fd, F_SETFL, 0);
  	}

	//last_mid = randomStartId();

    return (fd);
}

void getContent(const char *str,int len,char *content){
    int i = 0;
    int headLen = 8;
    int dataLen = 0;
    int p = 0;
    char strLen[3] = {0};

    for(i = 0;i < len;i ++)
    {
        if(str[i] == ':' && str[i-1] == 'S' && str[i-2] == 'C' && str[i-3] == 'N' && str[i-4] == 'A' && str[i-5] == '+' && str[i-6] == 'K' && str[i-7] == 'O'){
            strncpy(strLen,&str[i+1],2);
            dataLen = strtol(strLen,NULL,16);

            memcpy(&content[p],&str[i+headLen+3],dataLen - headLen);
            p += dataLen - headLen;

            //printf("content=%s,i=%d,headLen=%d,dataLen=%d,strLen=%s\n",content,i,headLen,dataLen,strLen);

            i = i + dataLen;
            headLen = 0;
        }
    }
}

int getANCS(void)
{
	int fd;
	int nread;
	char buff[BUFFER_LENG] = {0};
    char buff2[BUFFER_LENG] = {0};
    char msgID[5] = {0};
	fd_set rd;

// | O_NOCTTY | O_NDELAY

	printf("open uart\n");
	fd = open_port("/dev/ttyAMA0");
	if(fd == -1)
	{
		printf("打开串口失败");
		close(fd);
		exit(0);
	}
	printf("open sucess\n");

	set_interface_attribs(fd,B9600,1);
	


	while(1){
		while((nread = read(fd,buff,1024)) > 0){
			buff[nread] = '\0';
			//printf("收到字符串(%d):%s\n",nread,buff);

			if(strncmp(buff,"OK+ANCS8",8) == 0){
                if(buff[8] == '0'){
                    char msg[1024] = {0};
                    strncpy(msgID,&buff[12],4);



                    
                    sprintf(buff,"AT+ANCS%s199",msgID);
                    nread = write(fd,buff,14);
                    //printf("写入字符串(%d):%s\n",nread,buff);
                    memset(buff,0,BUFFER_LENG);
                    usleep(300000);
                    if((nread = read(fd,buff,1024)) <= 0) continue;
                    //printf("收到字符串(%d):%s\n",nread,buff);
                    getContent(buff,nread,msg);
                    strcpy(buff2,msg);
                    //printf("\n返回字符串:%s\n",buff2);



                    sprintf(buff,"AT+ANCS%s299",msgID);
                    nread = write(fd,buff,14);
                   // printf("写入字符串(%d):%s\n",nread,buff);
                    memset(buff,0,BUFFER_LENG);
                    usleep(300000);
                    if((nread = read(fd,buff,1024)) <= 0) continue;
                    //printf("收到字符串(%d):%s\n",nread,buff);
                    memset(msg,0,1024);
                    getContent(buff,nread,msg);
                    strcat(buff2,msg);
                    //printf("\n返回字符串:%s\n",buff2);



                    sprintf(buff,"AT+ANCS%s399",msgID);
                    nread = write(fd,buff,14);
                    //printf("写入字符串(%d):%s\n",nread,buff);
                    memset(buff,0,BUFFER_LENG);
                    usleep(300000);
                    if((nread = read(fd,buff,1024)) <= 0) continue;
                    memset(msg,0,1024);
                    getContent(buff,nread,msg);
                    strcat(buff2,msg);
                    //printf("\n返回字符串:%s\n",buff2);

                    strcpy(ANSC_MSG,buff2);
                    printf("消息内容：%s\n",ANSC_MSG);

                }

				
			}
		}
        sleep(1);
	}

	close(fd);

	return 0;
}



