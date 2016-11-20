# src/utils/Makefile
#

SUBDIRS =
DEVEL_HOME = ../..

OBJECTS = global.o filename.o timer.o
LIB = libutils.a

include $(DEVEL_HOME)/Makefile.incl

all: $(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
