#  (C) 2002-2009 - CHAI 3D
#  All Rights Reserved.
#
#  $Author: seb $
#  $Date: 2009-06-17 19:19:24 +1200 (Wed, 17 Jun 2009) $
#  $Rev: 262 $


TOP_DIR = ../..
SRC_DIR = ./src
BIN_DIR = $(TOP_DIR)/bin

include $(TOP_DIR)/Makefile.common

SOURCES   = $(wildcard $(SRC_DIR)/*.cpp)
PROGS     = $(BIN_DIR)/52-GEL-duck
CXXFLAGS += -DTETLIBRARY 

all: $(PROGS)

$(PROGS): $(LIB_TARGET)

$(PROGS): $(LIB_TARGET) $(SRC_DIR)/*.cpp
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $(PROGS)

tags:
	find ../.. -name \*.cpp -o -name \*h | xargs etags -o TAGS

clean:
	rm -f $(PROGS) *~ TAGS core *.bak #*#
