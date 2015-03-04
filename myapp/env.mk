#define cross compiler
CROSS_COMPILE=/opt/FriendlyARM/toolschain/4.5.1/bin/arm-linux-

CC := $(CROSS_COMPILE)gcc

INCLUDE_DIR := \
	-I$(MAKEROOT)/leds \
	-I$(MAKEROOT)/main_routine \

CFLAGS := $(INCLUDE_DIR)

%.o : %.c
	${CC} ${CFLAGS} -c $< -o $(MAKEROOT)/$@
