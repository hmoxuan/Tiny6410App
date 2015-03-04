#包括头文件，执行模式规则
include $(MAKEROOT)/env.mk

SRCS := $(wildcard *.c)

#生成所有.c文件对应的.o文件
OBJS := $(patsubst %.c,%.o,$(SRCS))

.PHONY : all
all : $(OBJS)
