#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include "strsplit.h"
#include <time.h>
  
#define MAXLINE 4096
char gprmc[4096];
 
int Speed = 0; 
int gps_gprmc(char *gps)
{
  char str[4096] = {0};
  char speed[10];
  char *parts[2];
  
  strcpy(str,gps);
//  printf("%s\n",str);
  size_t size = strsplit(str,parts,",");
  int i= 0;
//  
//  for(i=0;i < size; i++){
//   printf("parts:%d:%s\n",i,parts[i]); 
//}

  if(size < 8) return 0;

  strcpy(speed,parts[7]);

	//sscanf(gps,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,]",speed);
  Speed = (int)(atof(speed) * 2);
  return 0;	
	//printf("%s\n",gps);
	//scanf("%s%s\n",time,status);
	//scanf(gps,"%s%s%s%s%s%s%s%s%*s%s",time,status,wd,ns,jd,ew,speed,date);
	//printf("%s\n,%s\n,%s\n,%s\n,%s\n,%s\n,%s\n,%s\n",time,status,wd,ns,jd,ew,speed,date );
}

int gps_speed(void)
{
 // if(strlen(gprmc) == 0) return 0;
 // 
 // char speed[10];
 // sscanf(gprmc,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,]",speed);
 //
 // return atoi(speed) * 2;
  //srand( (unsigned)time( NULL ));
  //return rand() % 100;
  return Speed;
}

int gps(void)  
{  
    int    sockfd, n,rec_len;  
    char    recvline[4096];  
    char    buf[MAXLINE];  
    char    *p = NULL;
    struct sockaddr_in    servaddr;  
  
  	//gps_gprmc("$GPRMC,104337.000,A,2823.0783,N,12121.3476,E,0.000,316.13,311016,,,A*5E\r\n");

  	//return 0;

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
    printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);  
    exit(0);  
    }  
  
  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(7000);  
    if( inet_pton(AF_INET, "192.168.43.1", &servaddr.sin_addr) <= 0){  
    exit(0);  
    }  
  

    //设置为非阻塞模式
    //unsigned long ul = 1;
    //ioctl(sockfd,FIONBIO,&ul);

    //连接服务器
  	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 )
  	{
      close(sockfd);
  		return -1;
  	}

    //设置发送和接收超时间
  //struct timeval tv_out={3,0};
  //setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv_out,sizeof(tv_out));
  //setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&tv_out,sizeof(tv_out));

  	
  
  
 	while(1)
 	{
 		if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {  
	       continue;
	    } 

//	    printf("%s\n",buf);
	    p = strstr(buf,"$GPRMC");
	    if(p != NULL)
	    {
	    	//printf("%d",gps_gprmc("$GPRMC,104337.000,A,2823.0783,N,12121.3476,E,11.000,316.13,311016,,,A*5E\r\n"));
	      gps_gprmc(p);
        strcpy(gprmc,p);
	    }
 	} 
    
    //buf[rec_len]  = '\0';  
    //close(sockfd);  
    //exit(0);  
  return -1;
}  

/*
int main(int argc, char** argv)  
{  
    int    sockfd, n,rec_len;  
    char    recvline[4096];  
    char    buf[MAXLINE];  
    char    *p = NULL;
    struct sockaddr_in    servaddr;  
  
  	//gps_gprmc("$GPRMC,104337.000,A,2823.0783,N,12121.3476,E,0.000,316.13,311016,,,A*5E\r\n");

  	//return 0;

    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){  
    printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);  
    exit(0);  
    }  
  
  
    memset(&servaddr, 0, sizeof(servaddr));  
    servaddr.sin_family = AF_INET;  
    servaddr.sin_port = htons(7000);  
    if( inet_pton(AF_INET, "192.168.43.1", &servaddr.sin_addr) <= 0){  
    printf("inet_pton error for %s\n",argv[1]);  
    exit(0);  
    }  
  

    //连接服务器
  	while(1)
  	{
  		if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) >= 0)
  		{
  			break;
  		}
  	}
  
  
 	while(1)
 	{
 		if((rec_len = recv(sockfd, buf, MAXLINE,0)) == -1) {  
	       continue;
	    } 

	    //printf("%s\n",buf);
	    p = strstr(buf,"$GPRMC");
	    if(p != NULL)
	    {
	    	//printf("%d",gps_gprmc("$GPRMC,104337.000,A,2823.0783,N,12121.3476,E,11.000,316.13,311016,,,A*5E\r\n"));
	    	printf("%d\n",gps_gprmc(p));
	    }
 	} 
    
    buf[rec_len]  = '\0';  
    close(sockfd);  
    exit(0);  
}  
*/
