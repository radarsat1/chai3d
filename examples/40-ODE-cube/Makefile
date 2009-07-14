#  (C) 2002-2009 - CHAI 3D
#  All Rights Reserved.
#
#  $Author: seb $
#  $Date: 2009-05-21 12:34:35 +1200 (Thu, 21 May 2009) $
#  $Rev: 198 $


TOP_DIR = ../..
SRC_DIR = ./src
BIN_DIR = $(TOP_DIR)/bin

include $(TOP_DIR)/Makefile.common

SOURCES  = $(wildcard $(SRC_DIR)/*.cpp)
PROGS    = $(patsubst %.cpp, $(BIN_DIR)/%, $(notdir $(SOURCES)))

all: $(PROGS)

$(PROGS): $(LIB_TARGET)

$(BIN_DIR)/% : $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

tags:
	find ../.. -name \*.cpp -o -name \*h | xargs etags -o TAGS

clean:
	rm -f $(PROGS) *~ TAGS core *.bak #*#

	