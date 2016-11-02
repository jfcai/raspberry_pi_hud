#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
#include<errno.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<netinet/in.h>  
  
#define MAXLINE 4096
char gprmc[4096];
  
int gps_gprmc(char *gps)
{


	
	char speed[10];
	//sscanf(gps,"%*[^,],%[^,],%s",a,b,c);
	sscanf(gps,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,]",speed);

	return atoi(speed);
	
	//printf("%s\n",gps);
	//scanf("%s%s\n",time,status);
	//scanf(gps,"%s%s%s%s%s%s%s%s%*s%s",time,status,wd,ns,jd,ew,speed,date);
	//printf("%s\n,%s\n,%s\n,%s\n,%s\n,%s\n,%s\n,%s\n",time,status,wd,ns,jd,ew,speed,date );
}

int gps_speed(void)
{
  if(strlen(gprmc) == 0) return 0;
  
  char speed[10];
  sscanf(gprmc,"%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,]",speed);

  return atoi(speed) * 2;
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
  

    //连接服务器

  	
  	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 )
  	{
  		return -1;
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
	    	//gSpeed = gps_gprmc(p);
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
