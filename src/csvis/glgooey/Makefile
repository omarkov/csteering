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

TOPDIR = ../..

include $(TOPDIR)/environment.mk

# The options passed to the compiler/linker
INCLUDES+=-I../ -I./

# Our source files
SOURCES = BasicButton.cpp BasicButtonGroup.cpp Button.cpp CheckBox.cpp \
	  CheckBoxGroup.cpp Color.cpp ComboBox.cpp ComplexGridLayouter.cpp \
	  Dock.cpp EditField.cpp FlowLayouter.cpp FrameWindow.cpp GridLayouter.cpp \
	  KeyMessage.cpp ListBox.cpp ListControl.cpp ListWindow.cpp Message.cpp \
	  MouseMessage.cpp MultiTextButton.cpp Panel.cpp ProgressBar.cpp \
	  PropertyScheme.cpp Renderer.cpp ScrollBar.cpp Slideable.cpp Slider.cpp \
	  SolidSurfaceDesc.cpp SpinBox.cpp StaticText.cpp TimeManager.cpp Timer.cpp \
	  Window.cpp WindowManager.cpp BoxLayouter.cpp
OBJS = $(SOURCES:.cpp=.o)
TARGET = libGLGooey.a


all:	$(TARGET)

$(TARGET): libGLGooeyCore.a libGLGooeyServices.a $(OBJS)
	$(AR) rs $(TARGET) $(OBJS)
	cp $(TARGET) libGLGooeyCore.a libGLGooeyServices.a ..

libGLGooeyCore.a:
	$(MAKE) -C core

libGLGooeyServices.a:
	$(MAKE) -C services

clean:
	$(MAKE) -C core clean
	$(MAKE) -C services clean
	$(RM) -r *.o *~ ii_files $(TARGET) ../$(TARGET) ../libGLGooeyCore.a ../libGLGooeyServices.a
