# src/cad/Makefile
#

SUBDIRS = objects container
DEVEL_HOME = ../..

OBJECTS = cad_model.o
LIB = libcad.a

include $(DEVEL_HOME)/Makefile.incl

all:	$(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
