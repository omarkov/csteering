# src/reader/Makefile
#

SUBDIRS =
DEVEL_HOME = ../..

OBJECTS = voxelization.o
LIB = libvoxelization.a

include $(DEVEL_HOME)/Makefile.incl

all: $(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
