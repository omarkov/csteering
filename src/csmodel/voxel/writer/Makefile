# src/writer/Makefile
#

SUBDIRS =
DEVEL_HOME = ../..

OBJECTS = writer.o pot_writer.o xpm_writer.o
LIB = libwriter.a

include $(DEVEL_HOME)/Makefile.incl

all: $(LIB)

clean:
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
