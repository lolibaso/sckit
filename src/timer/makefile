
include ../../makefile.env

TARGET = libsc_timer.a

# iOS no epoller.o
OBJS = priority_queue_timer.o wheel_timer.o

all: $(OBJS)
	$(AR) $(TARGET) $^

.c.o:
	$(CXX) $(CXXFLAGS) -c $^

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $^

clean:
	$(RM) $(OBJS) $(TARGET) 

install:
	mv $(TARGET) $(LIB_PATH)
	cp *.h $(INC_PATH)
	rm *.o
