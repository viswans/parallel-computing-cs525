CXX       := g++
MPICXX    := mpicxx
CXXFLAGS  := -I include -std=c++11 -Wall  -O3 -lm #-DPAGERANK_MPI
LD        := mpicxx
LDFLAGS   := -O3
EXE       := pagerank

## Add your source files here ##
SRC_DIR   := src
MPI_DIR   := $(SRC_DIR)/mpi
CPP_DIR   := $(SRC_DIR)/cpp
# MPI_SRC   := $(wildcard $(MPI_DIR)/*.cpp)
CPP_SRC   := $(wildcard $(CPP_DIR)/*.cpp)
## End of source files lists ##

OBJ_DIR   := obj
MPI_OBJS  := $(patsubst $(MPI_DIR)/%.cpp,$(OBJ_DIR)/%.mpi.o,$(MPI_SRC))
CPP_OBJS  := $(patsubst $(CPP_DIR)/%.cpp,$(OBJ_DIR)/%.cpp.o,$(CPP_SRC))

build: $(EXE)

$(EXE): $(MPI_OBJS) $(CPP_OBJS)
	@mkdir -p $(OBJ_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.mpi.o: $(MPI_DIR)/%.cpp
	$(MPICXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.cpp.o: $(CPP_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(EXE) $(MPI_OBJS) $(CPP_OBJS)
