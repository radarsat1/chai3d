
.PHONY: all clean
all: ../../bin/earth_linux ../../bin/reflections_linux ../../bin/viewmesh_linux ../../lib/linux/libchai3d_linux.a

../../bin/earth_linux: ../bcpp6/earth/main.cpp
	$(MAKE) -C earth

../../bin/reflections_linux: ../bcpp6/reflections/main.cpp
	$(MAKE) -C reflections

../../bin/viewmesh_linux: ../cygwin/viewmesh/main.cpp
	$(MAKE) -C viewmesh

clean:
	$(MAKE) -C earth clean
	$(MAKE) -C reflections clean
	$(MAKE) -C viewmesh clean
