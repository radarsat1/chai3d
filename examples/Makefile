#  (C) 2002-2009 - CHAI 3D
#  All Rights Reserved.
#
#  $Author: seb $
#  $Date: 2009-05-21 12:34:35 +1200 (Thu, 21 May 2009) $
#  $Rev: 198 $


SUBDIRS = 01-devices \
	        10-oring \
	        11-effects \
	        12-polygons \
	        20-map \
	        21-object \
	        22-chrome \
	        23-tooth \
	        25-cubic \
	        40-ODE-cube \
	        41-ODE-pool \
	        42-ODE-mesh \
	        50-GEL-membrane \
	        51-GEL-cell \
	        52-GEL-duck

all: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@for T in $(SUBDIRS); do make -C $$T $@; done
	-rm -f core *~ *.bak #*
