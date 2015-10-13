CXX       := g++
MPICXX    := mpicxx
CXXFLAGS  := -I include -std=c++11 -Wall  -O3 -lm
LD        := mpicxx
LDFLAGS   := $(CXXFLAGS)
EXE       := pagerank.serial pagerank.parallel

## Add your source files here ##
SRC_DIR   := src
MAIN_SRC  := $(SRC_DIR)/main.cpp
MPI_DIR   := $(SRC_DIR)/mpi
CPP_DIR   := $(SRC_DIR)/cpp
MPI_SRC   := $(wildcard $(MPI_DIR)/*.cpp)
CPP_SRC   := $(wildcard $(CPP_DIR)/*.cpp)
## End of source files lists ##

OBJ_DIR   := obj
MPI_OBJS  := $(patsubst $(MPI_DIR)/%.cpp,$(OBJ_DIR)/%.mpi.o,$(MPI_SRC))
CPP_OBJS  := $(patsubst $(CPP_DIR)/%.cpp,$(OBJ_DIR)/%.cpp.o,$(CPP_SRC))

build: $(EXE)

pagerank.parallel: $(MPI_OBJS) $(CPP_OBJS) $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	$(LD) $(LDFLAGS) -o $@ $^ -DPAGERANK_MPI

pagerank.serial: $(CPP_OBJS) $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.mpi.o: $(MPI_DIR)/%.cpp
	$(MPICXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.cpp.o: $(CPP_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXE) $(MPI_OBJS) $(CPP_OBJS)
