# src/octree/Makefile
#

SUBDIRS = 
DEVEL_HOME = ../..

OBJECTS = oct_struct.o index_oct.o idx_holder.o fill_oct.o 
LIB = liboctree.a

include $(DEVEL_HOME)/Makefile.incl

all:	$(PROGRAMS) $(LIB)

clean:	
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
