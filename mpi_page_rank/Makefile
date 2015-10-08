CXX       := g++
MPICXX    := mpicxx
CXXFLAGS  := -I include -std=c++11 -Wall  -O3 -lm #-DPAGERANK_MPI
LD        := mpicxx
LDFLAGS   := -O3
EXE       := pagerank

## Add your source files here ##
SRC_DIR   := src
MPI_SRC   := $(wildcard $(SRC_DIR)/mpi/*.cpp)
CPP_SRC   := $(wildcard $(SRC_DIR)/cpp/*.cpp)
## End of source files lists ##

OBJ_DIR   := obj
MPI_OBJS  := $(patsubst $(SRC_DIR)/%.mpicpp,$(OBJ_DIR)/%.mpi.o,$(C_SRC))
CPP_OBJS  := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.cpp.o,$(CPP_SRC))

build: $(EXE)

$(EXE): $(MPI_OBJS) $(CPP_OBJS)
	@mkdir -p $(OBJ_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.mpi.o: $(SRC_DIR)/%.mpicpp
	$(MPICXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXE) $(MPI_OBJS) $(CPP_OBJS)
