include ../makefile.env

TARGET = libsckit.a

#wildcard把 指定目录 ./ 和 ../ 下的所有后缀是cpp的文件全部展开。
SRCS := $(wildcard *.cpp)

#notdir把展开的文件去除掉路径信息
DIR   = $(notdir $(SRCS))
 
#以下是指定目标文件 所有当.cpp文件变成.o文件
OBJS := $(DIR:.cpp=.o)


all: $(OBJS)
	$(AR) $(TARGET) $^

#.c.o:
#	$(CXX) $(CXXFLAGS) -c $^

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $^

install:
	mv $(TARGET) $(LIB_PATH)
	cp *.h $(INC_PATH)

.PHONY:clean
clean:
	$(RM) $(OBJS) $(TARGET) *.o
