# src/geom/Makefile
#

SUBDIRS =
DEVEL_HOME = ../..

OBJECTS = node_index.o node_vec.o geom_point.o geom_vec.o \
	line.o polygon.o 
LIB = libgeom.a

include $(DEVEL_HOME)/Makefile.incl

all: $(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
