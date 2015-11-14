CXX          := g++
CXXFLAGS     := -I include -std=c++11 -Wall  -O3 -lm -lpthread
LD           := $(CXX)
LDFLAGS      := $(CXXFLAGS)
EXE          := randomwalker

## Add your source files here ##
SRC_DIR      := src
CPP_DIR      := $(SRC_DIR)
CPP_SRC      := $(wildcard $(CPP_DIR)/*.cpp)
## End of source files lists ##

OBJ_DIR      := obj
CPP_OBJS     := $(patsubst $(CPP_DIR)/%.cpp,$(OBJ_DIR)/%.cpp.o,$(CPP_SRC))

build: $(EXE)

randomwalker : $(CPP_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.cpp.o: $(CPP_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXE) $(CPP_OBJS)
