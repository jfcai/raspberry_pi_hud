#CC:= gcc
CC := arm-none-linux-gnueabi-gcc
SRC += ./main.c
SRC += ./blue.c
SRC += ./can.c
SRC += ./oled/oled.c
SRC += ./gps/gps.c
SRC += ./db/db.c
SRC += ./db/content.c
SRC += ./cJSON/cJSON.c
SRC += ./strdup/strdup.c
SRC += ./strsplit/strsplit.c

OBJS = $(SRC:.c=.o)

INC = -I ./oled
INC += -I ./gps
INC += -I ./db
INC += -I ./cJSON
INC += -I ./strdup
INC += -I ./strsplit

LIB := -lbcm2835
LIB += -liconv
LIB += -lpthread
LIB += -lsqlite3
LIB += -lm
#LIB += -lcjson
LIB += -lcurl

TARGET = ./hud

DEFS = -D DEBUG
#DEFS += -D HUD
all:$(OBJS)
	$(CC) $(OBJS) $(DEFS) $(LIB) -o $(TARGET)
$(OBJS):%.o:%.c
	$(CC) -c $< $(DEFS) -o $@ $(INC)
clean:
	rm $(OBJS) $(TARGET)
