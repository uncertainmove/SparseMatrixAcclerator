CC = gcc
CPP = g++
RM = rm

SRC_PATH := .

TARGET := k_channel_trans

CFLAGS += $(foreach dir, $(INCLUDE_PATH), -I$(dir))
SRCS += $(wildcard $(SRC_PATH)/*.cpp)
OBJS := $(notdir $(SRCS:.cpp=.o))

build:
		$(CPP) -c $(CFLAGS) $(SRCS)
			$(CPP) $(CFLAGS) -o $(TARGET) $(OBJS)
				$(RM) $(OBJS)

clean:
		$(RM) $(OBJS) $(TARGET)

run:
		./$(TARGET) 
