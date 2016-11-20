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

# generic definitions
MAKE=make
MPI=mpic++
CFLAGS=-g -Wall
# CFLAGS=-O2 -Wall
INCLUDES=
# LDFLAGS=-pg
LDFLAGS=
LIBS=

# specific libraries
OSG_INCLUDES=
OSG_LIBS=-losgProducer -lProducer -lOpenThreads -losgGA -losgDB -losgText -losgSim -losgUtil -losg 
FREETYPE_INCLUDES=$(shell freetype-config --cflags)
FREETYPE_LIBS=$(shell freetype-config --libs)
GLIB_INCLUDES=$(shell glib-config --cflags)
GLIB_LIBS=$(shell glib-config --libs)
LIBPNG_INCLUDES=$(shell libpng-config --cflags)
LIBPNG_LIBS=$(shell libpng-config --libs)

# OCC
# LIB_X= -lX11 -lXext
LIB_X= -lX11 -lXext -lXmu 
OCC_LDFLAGS=-L$(CSROOT)/Linux/lib
LIB_OCC= -lTKernel -lTKMath -lTKG2d -lTKG3d -lTKGeomBase -lTKBRep -lTKGeomAlgo -lTKTopAlgo \
	 -lTKPrim -lTKBool -lTKFeat -lTKFillet -lTKOffset -lTKHLR \
	 -lTKService -lTKV2d -lTKV3d -lTKPCAF -lTKCDF -lTKCAF \
         -lPTKernel -lTKIGES -lTKSTEP -lTKSTL -lTKVRML -lTKShHealing \
         -lTKXSBase -lTKPShape -lTKShapeSchema -lTKBO 

OCC_DFLAGS=$(OCC_LDFLAGS) -L/usr/X11R6/lib -L$(CASROOT)/Linux/lib
OCC_LIBS= -lm -ldl -lc $(LIB_OCC) $(LIB_X)
OCCFLAGS=${OCC_DFLAGS} ${OCC_LIBS}

# generic rules
include $(TOPDIR)/rules.mk

# Crude OS detection
OS = $(shell uname)

ifeq ($(OS),Darwin)
	include $(TOPDIR)/darwin.mk
endif
ifeq ($(OS),IRIX64)
	include $(TOPDIR)/irix64.mk
endif
ifeq ($(OS),Linux)
	include $(TOPDIR)/linux.mk
endif
ifeq ($(OS),FreeBSD)
	include $(TOPDIR)/freebsd.mk
endif


