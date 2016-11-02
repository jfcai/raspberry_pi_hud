CC:= gcc

SRC += ./main.c
SRC += ./oled/oled.c
SRC += ./gps/gps.c
SRC += ./db/db.c
SRC += ./db/content.c
SRC += ./cJSON/cJSON.c

OBJS = $(SRC:.c=.o)

INC = -I ./oled
INC += -I ./gps
INC += -I ./db
INC += -I ./cJSON

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
