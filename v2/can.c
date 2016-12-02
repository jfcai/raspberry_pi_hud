

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

//#include "terminal.h"

extern int CarSpeed;
extern int CarRpm;


static volatile int running = 1;


int read_can_info(void)
{
	fd_set rdfs;
	int s;
	int ret;

//	int k;
//	for(k = 0;k<999;k++){
//		CarSpeed = k;
//		if (k >= 990)
//			k = 0;
//	}

	struct sockaddr_can addr;
	//char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
	//struct iovec iov;
	//struct msghdr msg;

	struct can_frame frame;
	int nbytes;
	struct ifreq ifr;

//设置过滤
	struct can_filter rfilter[4];
    rfilter[0].can_id   = 0x123;
    rfilter[0].can_mask = CAN_SFF_MASK;

	rfilter[1].can_id   = 0x2b7;
    rfilter[1].can_mask = CAN_SFF_MASK; 

    rfilter[2].can_id   = 0x351;
    rfilter[2].can_mask = CAN_SFF_MASK;

    rfilter[3].can_id   = 0x35B;
    rfilter[3].can_mask = CAN_SFF_MASK;
       



	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0){
		perror("socket");
			return -1;
	}

	memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	strcpy(ifr.ifr_name, "can0");

	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		return -1;
	}
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return -1;
	}
	

	while (running) {

		FD_ZERO(&rdfs);
		FD_SET(s, &rdfs);

		if ((ret = select(s+1, &rdfs, NULL, NULL, NULL)) < 0) {
			//perror("select");
			running = 0;
			continue;
		}



		if (FD_ISSET(s,&rdfs)) {

			nbytes = read(s, &frame, sizeof(struct can_frame));	

			if (nbytes < sizeof(struct can_frame)) {
				printf("read: incomplete CAN frame\n");
				return -1;
			}

			//printf("recvmsg.len = %d\nframe.can_id = %03x\nframe.can_dlc = %d\nframe.data = ",nbytes,frame.can_id,frame.can_dlc);
			//for(i=0;i<frame.can_dlc;i++){
			//	printf("%02x ",frame.data[i]);
			//}
				
			
			//printf("\n");

			if(frame.can_id == 0x351){
				CarSpeed = (frame.data[2] * 256 + frame.data[1] - 1) / 192;
			}
			else if(frame.can_id == 0x35B){
				CarRpm = 0.25*(256*frame.data[2]+frame.data[1]);
			}

			//nbytes = write(s, &frame, sizeof(struct can_frame));
		}
	}
	
	close(s);
	return 0;
}
