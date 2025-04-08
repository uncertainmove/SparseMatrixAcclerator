CC = gcc
CPP = g++
RM = rm

SRC_PATH := .

TARGET := generate_trace

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

rmat20_run:
	./$(TARGET) 1048576 /data/zz_data/projects/ligra/apps/pr_tmp_res.txt /data/zz_data/projects/ligra/apps/deltapr_tmp_res.txt

rmat20_check:
	./$(TARGET) 1048576 /data/zz_data/projects/ligra/apps/pr_tmp_res.txt ../../acc_sim/build/debug_res.txt

rmat20_check2:
	./$(TARGET) 1048576 /data/zz_data/projects/ligra/apps/pr_tmp_res.txt ~/Projects/SparseMatrixAcclerator/simulator/build/res.txt

