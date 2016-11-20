# src/Makefile
#

SUBDIRS = cad geom octree generator reader voxelization utils writer
DEVEL_HOME = ..
SGS_DIR = octree/sgs

PROGRAMS = raw2octree raw2voxel 
PACKAGES = $(LIB_DIR)/libreader.a $(LIB_DIR)/libcad.a $(LIB_DIR)/libvoxelization.a \
	$(LIB_DIR)/libcadobjs.a $(LIB_DIR)/libcadcont.a \
	$(LIB_DIR)/libgen.a $(LIB_DIR)/liboctree.a $(LIB_DIR)/libgeom.a \
	$(LIB_DIR)/libwriter.a $(LIB_DIR)/libutils.a

LIBS = -lvoxelization -lreader -lcad -lcadcont -lgen -loctree -lgeom \
	-lcadobjs -lwriter -lutils


include $(DEVEL_HOME)/Makefile.incl

#all:	raw2octree raw2voxel
all:
	cp lib/*.a ..

#clean:	
#	$(RM) *~ *.o $(LIB_DIR)/*.a

clean-all: clean
	$(RM) $(PROGRAMS) $(PACKAGES) *.pot $(SGS_DIR)/*.pot *.xpm 
