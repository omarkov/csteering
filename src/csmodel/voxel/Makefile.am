SUBDIRS = cad geom octree generator reader utils writer .

LDADD = reader/libreader.a cad/libcad.a \
	cad/container/libcadcont.a generator/libgen.a octree/liboctree.a \
	geom/libgeom.a writer/libwriter.a cad/objects/libcadobjs.a \
	utils/libutils.a

bin_PROGRAMS = raw2octree
raw2octree_SOURCES = raw2octree.cpp

all:	raw2octree
