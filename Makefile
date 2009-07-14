#  (C) 2002-2009 - CHAI 3D
#  All Rights Reserved.
#
#  $Author: seb $
#  $Date: 2009-06-22 12:21:41 +1200 (Mon, 22 Jun 2009) $
#  $Rev: 285 $


# global path
TOP_DIR = .

# commong settings
include Makefile.common

# directories
OBJ_ROOT = $(TOP_DIR)/obj
OBJ_DIR  = $(TOP_DIR)/obj/$(CFG)/$(OS)-$(ARCH)
SRC_DIR  = $(TOP_DIR)/src

# include paths
CXXFLAGS += -I$(SRC_DIR) -I$(DHD_EXT)/include

# main sources
SOURCES    = $(shell find $(SRC_DIR) -name '*.cpp')

# module sources
SOURCES   += $(wildcard $(GEL_DIR)/*.cpp)
SOURCES   += $(wildcard $(ODE_DIR)/*.cpp)

# include dependencies
INCLUDES   = $(shell find $(SRC_DIR) -name '*.h')

# module includes
INCLUDES  += $(wildcard $(GEL_DIR)/*.h)
INCLUDES  += $(wildcard $(ODE_DIR)/*.h)

# objects
OBJECTS    = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SOURCES)))

# build examples
SUBDIRS = examples

# build rules

all: $(LIB_TARGET) $(SUBDIRS)

$(LIB_TARGET): $(OBJ_DIR) $(LIB_DIR) $(LIB_TARGET)($(OBJECTS))

$(OBJECTS): $(INCLUDES)

$(LIB_DIR) $(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/*/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(OBJ_DIR)/%.o : $(GEL_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
$(OBJ_DIR)/%.o : $(ODE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@for T in $(SUBDIRS); do make -C $$T $@; done
	-rm -f $(OBJECTS) $(LIB_TARGET) *~ TAGS core *.bak #*#
	-rmdir $(LIB_DIR) $(OBJ_DIR)
