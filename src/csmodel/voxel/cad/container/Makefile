# src/cad/container/Makefile
#

SUBDIRS =
DEVEL_HOME = ../../..

OBJECTS = faces.o
LIB = libcadcont.a

include $(DEVEL_HOME)/Makefile.incl

all:	$(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
