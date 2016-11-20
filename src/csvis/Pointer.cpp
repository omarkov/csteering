// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Oliver Markovic <entrox@entrox.org> 
//   All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  o Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer. 
//  o Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution. 
//  o Neither the name of the author nor the names of the contributors may be
//    used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <osg/GL>

#include "Pointer.h"
#include "GUIController.h"
#include "FANClasses.h"


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

Pointer::Pointer() : m_positionX(0), m_positionY(0), m_style(1), m_size(100)
{
    m_style = atoi(FAN::app->config->getValue("VISCURSORSTYLE", "1"));
    m_size = atoi(FAN::app->config->getValue("VISCURSORSIZE", "100"));
}

Pointer::~Pointer()
{
}

// ----------------------------------------------------------------------------
// Position query
// ----------------------------------------------------------------------------

void Pointer::setPosition(float x, float y)
{
    m_positionX = x;
    m_positionY = y;
}

void Pointer::getPosition(float& x, float& y) const
{
    x = m_positionX;
    y = m_positionY;
}


// ----------------------------------------------------------------------------
// Drawing
// ----------------------------------------------------------------------------

void Pointer::setStyle(unsigned int style)
{
    m_style = style;
}

void Pointer::setSize(unsigned int size)
{
    m_size = size;
}

void Pointer::draw(double elapsed)
{
    float size = m_size/100.0f;
    switch(m_style)
    {
        case 0:
            // arrow
            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX, m_positionY);
            glVertex2f(m_positionX+size*16.0f, m_positionY+size*4.0f);
            glVertex2f(m_positionX+size*4.0f, m_positionY+size*16.0f);
            glEnd();
            glBegin(GL_QUADS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX+size*12.0f, m_positionY+size*8.0f);
            glVertex2f(m_positionX+size*22.0f, m_positionY+size*18.0f);
            glVertex2f(m_positionX+size*18.0f, m_positionY+size*22.0f);
            glVertex2f(m_positionX+size*8.0f, m_positionY+size*12.0f);
            glEnd();
            break;
        case 1:
            // normal cross
            glBegin(GL_QUADS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*2, m_positionY+size*22);
            glVertex2f(m_positionX+size*2, m_positionY+size*22);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX+size*2, m_positionY+size*7);
            glVertex2f(m_positionX-size*2, m_positionY+size*7);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*2, m_positionY-size*22);
            glVertex2f(m_positionX+size*2, m_positionY-size*22);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX+size*2, m_positionY-size*7);
            glVertex2f(m_positionX-size*2, m_positionY-size*7);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX+size*22, m_positionY-size*2);
            glVertex2f(m_positionX+size*22, m_positionY+size*2);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX+size*7, m_positionY+size*2);
            glVertex2f(m_positionX+size*7, m_positionY-size*2);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*22, m_positionY-size*2);
            glVertex2f(m_positionX-size*22, m_positionY+size*2);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX-size*7, m_positionY+size*2);
            glVertex2f(m_positionX-size*7, m_positionY-size*2);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*2, m_positionY-size*2);
            glVertex2f(m_positionX-size*2, m_positionY+size*2);
            glVertex2f(m_positionX+size*2, m_positionY+size*2);
            glVertex2f(m_positionX+size*2, m_positionY-size*2);
            glEnd();
            break;
        case 2:
            // diagonal cross
            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX+size*5, m_positionY+size*5);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX+size*25, m_positionY+size*15);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX+size*15, m_positionY+size*25);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX-size*5, m_positionY+size*5);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*25, m_positionY+size*15);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*15, m_positionY+size*25);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX-size*5, m_positionY-size*5);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*25, m_positionY-size*15);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX-size*15, m_positionY-size*25);
            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex2f(m_positionX+size*5, m_positionY-size*5);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX+size*25, m_positionY-size*15);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex2f(m_positionX+size*15, m_positionY-size*25);
            glEnd();
            break;
        case 3:
            // crossing lines
            unsigned int x, y;
            g_GUIController->screenSize(x, y);
            glBegin(GL_QUADS);
            glColor3f(0.0f, 1.0f, 0.0f);
            glVertex2f(0.0f, m_positionY-size*1.0f);
            glVertex2f(0.0f, m_positionY+size*1.0f);
            glVertex2f(x, m_positionY+size*1.0f);
            glVertex2f(x, m_positionY-size*1.0f);
            glVertex2f(m_positionX-size*1.0f, 0.0f);
            glVertex2f(m_positionX+size*1.0f, 0.0f);
            glVertex2f(m_positionX+size*1.0f, y);
            glVertex2f(m_positionX-size*1.0f, y);
            glEnd();
            break;
    }
}
