# src/octree/Makefile
#

SUBDIRS = 
DEVEL_HOME = ../..

TOPDIR=../../..

include $(TOPDIR)/environment.mk

INCLUDES+=-I$(TOPDIR)/common -I$(TOPDIR)/common/fan/include $(GLIB_INCLUDES)

OBJECTS = oct_gen.o scan_line.o gen_help.o 
LIB = libgen.a

include $(DEVEL_HOME)/Makefile.incl

all:	$(PROGRAMS) $(LIB)

clean:	
	$(RM) *~ *.o

clean-all: clean
	$(RM) $(LIB_DIR)/$(LIB)
