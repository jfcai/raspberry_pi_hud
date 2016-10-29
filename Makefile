CC:= gcc

SRC += ./main.c
SRC += ./oled/oled.c

OBJS = $(SRC:.c=.o)

INC = -I ./oled

LIB := -lbcm2835
LIB += -liconv

TARGET = ./hud

DEFS = -D DEBUG
DEFS += -D HUD
all:$(OBJS)
	$(CC) $(OBJS) $(DEFS) $(LIB) -o $(TARGET)
$(OBJS):%.o:%.c
	$(CC) -c $< $(DEFS) -o $@ $(INC)
clean:
	rm $(OBJS) $(TARGET)
