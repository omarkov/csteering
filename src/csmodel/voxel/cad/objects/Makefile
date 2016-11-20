# src/cad/objects/Makefile
#

SUBDIRS =
DEVEL_HOME = ../../..

OBJECTS = cad_object.o point.o face.o plane.o triangle.o 
LIB = libcadobjs.a

include $(DEVEL_HOME)/Makefile.incl

all:	$(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
