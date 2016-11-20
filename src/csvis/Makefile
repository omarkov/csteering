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

include $(TOPDIR)/environment.mk

# The options passed to the compiler/linker
INCLUDES+=-I. -Iglgooey -I../common/fan/include -I../common $(GLIB_INCLUDES) $(OSG_INCLUDES)
LIBS+=$(OSG_LIBS) $(FREETYPE_LIBS) $(GLIB_LIBS) $(LIBPNG_LIBS) -lX11 -lm -lGL -lGLU -lpthread

# Our source files
SOURCES = Application.cpp ControlPoint.cpp ControlPointGrid.cpp Dragger.cpp DTrackInputDevice.cpp \
	  EventHandler.cpp GUIController.cpp GlyphProbe.cpp InputManager.cpp \
	  InputNotificationMixin.cpp InteractiveManipulator.cpp OnscreenDisplay.cpp \
	  StandardWindow.cpp LoadModelWindow.cpp LoginWindow.cpp Menu.cpp MenuEventHandler.cpp \
	  MenuItem.cpp ModelFace.cpp MouseInputDevice.cpp Pointer.cpp SimulationSpace.cpp \
	  PreferencesWindow.cpp Probe.cpp ProbeManager.cpp RibbonProbe.cpp \
	  RotationDragger.cpp Server.cpp SliceProbe.cpp \
	  TranslationDragger.cpp TransformDragger.cpp TriangulatedModel.cpp \
	  MessageWindow.cpp RibbonProbeWindow.cpp SelectableMixin.cpp common.cpp \
	  SimulationWindow.cpp InteractiveTrackballManipulator.cpp AboutWindow.cpp \
	  ConfirmationWindow.cpp HelpWindow.cpp GlyphProbeWindow.cpp SliceProbeWindow.cpp \
      main.cpp

OBJS = $(SOURCES:.cpp=.o)
TARGET = csvis


all:	$(TARGET)

$(TARGET): libGLGooey libFAN libCommonServer $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -L. -L../common -L../common/fan -lGLGooey -lGLGooeyCore -lGLGooeyServices -lCommonServer -lFAN $(LIBS) -o ../../bin/$@

libGLGooey:
	$(MAKE) -C glgooey

libFAN:
	$(MAKE) -C ../common/fan

libCommonServer:
	$(MAKE) -C ../common

dep:
	$(RM) -f .dep
	makedepend -f- -- $(INCLUDES) -- $(SOURCES) > .dep

clean:
	$(MAKE) -C glgooey clean
	$(RM) -rf *.o *.a *.gch *~ core ii_files $(TARGET)


include .dep
