CXX          := g++
CXXFLAGS     := -I include -std=c++11 -Wall  -O3 -lm
LD           := $(CXX)
LDFLAGS      := $(CXXFLAGS)
EXE          := pagerank.serial pagerank.parallel

## Add your source files here ##
SRC_DIR      := src
MAIN_SRC     := $(SRC_DIR)/main.cpp
PTHREAD_DIR  := $(SRC_DIR)/pthread
CPP_DIR      := $(SRC_DIR)/cpp
PTHREAD_SRC  := $(wildcard $(PTHREAD_DIR)/*.cpp)
CPP_SRC      := $(wildcard $(CPP_DIR)/*.cpp)
## End of source files lists ##

OBJ_DIR      := obj
PTHREAD_OBJS := $(patsubst $(PTHREAD_DIR)/%.cpp,$(OBJ_DIR)/%.pth.o,$(PTHREAD_SRC))
CPP_OBJS     := $(patsubst $(CPP_DIR)/%.cpp,$(OBJ_DIR)/%.cpp.o,$(CPP_SRC))

build: $(EXE)

pagerank.parallel: $(PTHREAD_OBJS) $(CPP_OBJS) $(MAIN_SRC)
	$(LD) $(LDFLAGS) -lpthread -o $@ $^ -DPAGERANK_PTHREAD

pagerank.serial: $(CPP_OBJS) $(MAIN_SRC)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.pth.o: $(PTHREAD_DIR)/%.cpp
	$(PTHREADCXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.cpp.o: $(CPP_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXE) $(PTHREAD_OBJS) $(CPP_OBJS)
