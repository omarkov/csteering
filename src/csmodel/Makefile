# -*- Mode: Makefile; indent-tabs-mode: t -*-
#
# Copyright (c) 2004, Oliver Markovic <entrox@entrox.org>
#   All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  o Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#  o Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  o Neither the name of the author nor the names of the contributors may be
#    used to endorse or promote products derived from this software without
#    specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

TOPDIR = ..

VOXELLIBS = libvoxelization.a libreader.a libcad.a libcadcont.a libgen.a liboctree.a libgeom.a libcadobjs.a libwriter.a libutils.a
VOXEL_INC_DIR=./voxel

include $(TOPDIR)/environment.mk
OCC_INCLUDES=-I$(CASROOT)/inc -I. 

# The options passed to the compiler/linker
INCLUDES+=-I. -I.. -I../common/fan/include -I../common -I../cssim $(GLIB_INCLUDES) -I$(VOXEL_INC_DIR) -I$(VOXEL_INC_DIR)/utils -I$(VOXEL_INC_DIR)/geom \
	-I$(VOXEL_INC_DIR)/cad/objects -I/usr/local/include

INCLUDES+=-DCSFDB -DNO_CXX_EXCEPTION -DNo_Exception -fpermissive -DHAVE_WOK_CONFIG_H -DHAVE_IOSTREAM \
	 -DLIN -DLININTEL $(OCC_INCLUDES) 

# Our source files
SOURCES = ModelServer.cpp csmodeloccvoxelization.cpp start.cpp csmodelcontroller.cpp csmodeltriangulation.cpp

OBJS = $(SOURCES:.cpp=.o)
TARGET = modelserver

all:	$(TARGET)

ModelServer.o: ModelServer.cpp
	$(MPI) -c -O2 $(INCLUDES) ModelServer.cpp

modelserver: voxelization sim libFAN libCommonServer libModelServer
	$(MPI) libModelServer.a $(VOXELLIBS) ${GLIB_LIBS} $(LDFLAGS) $(OCCFLAGS) -L../common -L../common/fan -L../cssim -lCommonServer -lFAN -lsim -o $@ -o $@
	cp modelserver ../../bin

libModelServer: $(OBJS) 
	$(AR) rs libModelServer.a $(OBJS)

libFAN:
	$(MAKE) -C ../common/fan

libCommonServer:
	$(MAKE) -C ../common
	
voxelization:
	$(MAKE) -C voxel

sim:
	$(MAKE) -C ../cssim libsim

clean:
	$(MAKE) -C voxel clean
	$(MAKE) -C ../cssim clean
	$(RM) -rf *.o *.gch *~ core ii_files $(TARGET)
