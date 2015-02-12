PROJECT = main
SRCS    = $(wildcard *.cpp)
OBJS    = $(SRCS:.cpp=.o)

CXXFLAGS  += -Wall -Wextra -g -ggdb
LDLIBS += -lpthread

$(PROJECT): $(OBJS) $(SYSTEMC)/lib$(ARCH_SUFFIX)/libsystemc.a
	$(CXX) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS) 2>&1

clean:
	rm -f $(PROJECT) *.o
.PHONY: clean
