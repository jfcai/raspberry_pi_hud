#CC:= arm-none-linux-gnueabi-gcc
CC := gcc
SRC += ./main.c
SRC += ./blue.c
SRC += ./can.c
SRC += ./oled.c

OBJS = $(SRC:.c=.o)

INC := -I /Volumes/pi/arm-none-linux-gnueabi/include

LIB := -lbcm2835
LIB += -lpthread
LIB += -liconv
LIB += -L /Volumes/pi/arm-none-linux-gnueabi/lib

TARGET = ./hud_v2

#DEFS = -D DEBUG
#DEFS += -D HUD
all:$(OBJS)
	$(CC) $(OBJS) $(DEFS) $(LIB) -o $(TARGET)
$(OBJS):%.o:%.c
	$(CC) -c -Wall  $< $(DEFS) -o $@ $(INC)
clean:
	rm $(OBJS) $(TARGET)
