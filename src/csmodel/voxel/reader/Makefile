# src/reader/Makefile
#

SUBDIRS =
DEVEL_HOME = ../..

OBJECTS = rawreader.o reader.o 
LIB = libreader.a

include $(DEVEL_HOME)/Makefile.incl

all: $(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
