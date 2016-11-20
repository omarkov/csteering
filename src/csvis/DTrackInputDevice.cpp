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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <FANClasses.h>

#ifdef _WIN32
#  include <winsock.h>
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <sys/signal.h>
#  include <fcntl.h>
#endif

#ifdef __IRIX__
#include <unistd.h>
#endif

#include <osgGA/GUIEventAdapter>
#include <osg/GL>

#include "GUIController.h"
#include "InputManager.h"
#include "DTrackInputDevice.h"



// ----------------------------------------------------------------------------
// Utility functions
// ----------------------------------------------------------------------------

int set_nonblock_flag (int desc, int value)
{
#ifndef _WIN32
    int oldflags = fcntl (desc, F_GETFL, 0);
    /* If reading the flags failed, return error indication now. */
    if (oldflags < 0)
	    return oldflags;
    /* Set just the flag we want to set. */
    if (value != 0)
        oldflags |= O_NONBLOCK;
    else
        oldflags &= ~O_NONBLOCK;
	/* Store modified flag word in the descriptor. */
    return fcntl (desc, F_SETFL, oldflags);
#else
    return 0;
#endif
}



// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

DTrackInputDevice::DTrackInputDevice(const std::string& name)
    : InputDevice(name), m_inputSocket(-1), m_outputSocket(-1), m_oldButtonState(0)
{
    m_localPort = atoi(FAN::app->config->getValue("dtracklocalport", "15000"));

    m_trackerData.id = 0;
    m_trackerData.quality = 0.0f;
    m_trackerData.button = 0;
    for(int i=0; i<3; i++)
    {
        m_trackerData.location[i] = 0.0f;
        m_trackerData.angle[i] = 0.0f;
    }

    for(int i=0; i<9; i++)
    {
        m_trackerData.matrix[i] = 0.0f;
    }
 
}

DTrackInputDevice::~DTrackInputDevice()
{
    disconnect();
}


// ----------------------------------------------------------------------------
// Connection management
// ----------------------------------------------------------------------------

bool DTrackInputDevice::connect(const std::string& hostName, int port)
{
    struct sockaddr_in server_address;
    struct sockaddr_in local_address;
    struct hostent* host;

    // open outgoing control socket
    memset((char*)&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    
    if ((host = gethostbyname(hostName.c_str())) == NULL) {
        perror("host name error");
        return false;
    }

    memcpy((char*)&server_address.sin_addr, host->h_addr, host->h_length);

    if ((m_outputSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("error opening output socket");
        return false;
    }

    if (::connect(m_outputSocket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        perror("error connecting");
        return false;
    }

    // initialize tracker
    if (send(m_outputSocket, "dtrack 10 3", 12, 0) != 12)
        perror("initialization 1 failed");
    
    sleep(2);

    if (send(m_outputSocket, "dtrack 31", 10, 0) != 10)
        perror("initialization 2 failed");

    // open incoming data socket
    if ((m_inputSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("error opening input socket");
        return false;
    }

    // bind local address
    memset((char*)&local_address, 0, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(m_localPort);
    
    if (bind(m_inputSocket, (struct sockaddr*)&local_address, sizeof(local_address))) {
        perror("error binding input socket");
        return false;
    }
    
    set_nonblock_flag(m_inputSocket, 1);

    return true;
}

bool DTrackInputDevice::disconnect()
{
    bool success = true;

    if (send(m_outputSocket, "dtrack 32", 10, 0) != 10) {
        perror("tracker stop command failed");
        success = false;
    }

    sleep(1);

    if (send(m_outputSocket, "dtrack 10 0", 12, 0) != 12) {
        perror("tracker stop command failed");
        success = false;
    }

    close(m_inputSocket);
    close(m_outputSocket);

    return success;
}


// ----------------------------------------------------------------------------
// Packet processing
// ----------------------------------------------------------------------------

bool DTrackInputDevice::processUDPStack()
{
    int messageSize = 0;
    int sentinel = 0;
    bool messageArrived = false;

    /* read should be non-blocking and thus return -1 when buffer is emptied */
    while ((sentinel = read(m_inputSocket, m_trackerMessage, packetLength)) > 0) {
    /* TODO: check if order is important at this time, for ex. to track all pressed
     * and released buttons, at the moment order is completely ignored here */
      messageArrived = true;
      messageSize = sentinel;
    }

    if (messageArrived) {
      memset(&m_trackerMessage[messageSize], 0, packetLength - messageSize);
      
      char* line = strtok(m_trackerMessage, "\n");
      int numSticks = 0;

      while (line != NULL) {
        if (sscanf(line, "6df %i", &numSticks) == 1) {
          if (!numSticks)
            return false;

          sscanf(line, "6df 1 [%u %f %u] [%f %f %f %f %f %f] [%f %f %f %f %f %f %f %f %f]",
                 &m_trackerData.id,
                 &m_trackerData.quality,
                 &m_trackerData.button,
                 &m_trackerData.location[0],
                 &m_trackerData.location[1],
                 &m_trackerData.location[2],
                 &m_trackerData.angle[0],
                 &m_trackerData.angle[1],
                 &m_trackerData.angle[2],
                 &m_trackerData.matrix[0],
                 &m_trackerData.matrix[1],
                 &m_trackerData.matrix[2],
                 &m_trackerData.matrix[3],
                 &m_trackerData.matrix[4],
                 &m_trackerData.matrix[5],
                 &m_trackerData.matrix[6],
                 &m_trackerData.matrix[7],
                 &m_trackerData.matrix[8]);
          return true; // inserted
        }

        line = strtok(NULL, "\n");
      }
    }

    return false;//changed: messageArrived;
}


// ----------------------------------------------------------------------------
// Data processing
// ----------------------------------------------------------------------------

void DTrackInputDevice::update()
{
    // get data from the tracker
    if (processUDPStack()) {

        if (m_trackerData.quality >= 0.0f) {
            // handle position and rotation only when tracker found
          
            // calculate position
            m_position.set(m_trackerData.location[0] * m_sensitivityFactor,
                           (m_trackerData.location[1] - m_yOffset) * m_sensitivityFactor,
                           m_trackerData.location[2] * m_sensitivityFactor);

            osg::Matrixd rot;

            // calculate rotation
            rot.set(m_trackerData.matrix[0], m_trackerData.matrix[3], m_trackerData.matrix[6], 0.0,
                    m_trackerData.matrix[1], m_trackerData.matrix[4], m_trackerData.matrix[7], 0.0,
                    m_trackerData.matrix[2], m_trackerData.matrix[5], m_trackerData.matrix[8], 0.0,
                    0.0, 0.0, 0.0, 1.0);
            //rot = osg::Matrixd::inverse(rot);
            rot.get(m_rotation);

            // calculate camera position
            m_cposition.set(m_trackerData.location[0] * m_sensitivityFactor,
                            (m_trackerData.location[1] - m_yOffset) * m_sensitivityFactor,
                            m_trackerData.location[2] * m_sensitivityFactor);
    
            // calculate camera rotation
            rot.set(m_trackerData.matrix[0], m_trackerData.matrix[3], m_trackerData.matrix[6], 0.0,
                    m_trackerData.matrix[1], m_trackerData.matrix[4], m_trackerData.matrix[7], 0.0,
                    m_trackerData.matrix[2], m_trackerData.matrix[5], m_trackerData.matrix[8], 0.0,
                    0.0, 0.0, 0.0, 1.0);
            rot = osg::Matrixd::inverse(rot);
            rot.get(m_crotation);
    
            // update mouse position    
            unsigned int width, height, x, y;
            g_GUIController->screenSize(width, height);
            x = (unsigned int)(m_trackerData.location[0] * m_sensitivityFactor + (float)width/2.0f);
            y = (unsigned int)((m_trackerData.location[1] - m_yOffset) * -m_sensitivityFactor + (float)height*0.5f);

            g_InputManager->handlePointerMove(x, y);

        }
        
        // update buttons
        if ((m_oldButtonState & FLY_FRONT_BUTTON) && !(m_trackerData.button & FLY_FRONT_BUTTON))
          g_InputManager->handlePointerButtonUp(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);
        else if (!(m_oldButtonState & FLY_FRONT_BUTTON) && (m_trackerData.button & FLY_FRONT_BUTTON))
          g_InputManager->handlePointerButtonDown(osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON);

        if ((m_oldButtonState & FLY_RIGHT_BUTTON) && !(m_trackerData.button & FLY_RIGHT_BUTTON))
          g_InputManager->handlePointerButtonUp(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON);
        else if (!(m_oldButtonState & FLY_RIGHT_BUTTON) && (m_trackerData.button & FLY_RIGHT_BUTTON))
          g_InputManager->handlePointerButtonDown(osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON);

        if (!(m_oldButtonState & FLY_RIGHT) && (m_trackerData.button & FLY_RIGHT))
          setSensitivityFactor(m_sensitivityFactor * 1.1f);
        if (!(m_oldButtonState & FLY_LEFT) && (m_trackerData.button & FLY_LEFT))
          setSensitivityFactor(m_sensitivityFactor / 1.1f);
        if (!(m_oldButtonState & FLY_UP) && (m_trackerData.button & FLY_UP))
          setyOffset(m_yOffset - 20.0f);
        if (!(m_oldButtonState & FLY_DOWN) && (m_trackerData.button & FLY_DOWN))
          setyOffset(m_yOffset + 20.0f);

        m_oldButtonState = m_trackerData.button;
    }
}

// ----------------------------------------------------------------------------
// Represent on OSD
// ----------------------------------------------------------------------------

void DTrackInputDevice::draw()
{
    glLineWidth(1.0f);
    
    // pressed buttons
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    if (m_trackerData.button & FLY_LEFT_BUTTON)
    {
        glBegin(GL_POLYGON);
        glVertex2i(-28, -50);
        glVertex2i(-37, -55);
        glVertex2i(-37, -66);
        glVertex2i(-28, -61);
        glEnd();
    }
    if (m_trackerData.button & FLY_MIDDLE_BUTTON)
    {
        glBegin(GL_POLYGON);
        glVertex2i(-17, -50);
        glVertex2i(-28, -50);
        glVertex2i(-28, -61);
        glVertex2i(-17, -61);
        glEnd();
    }
    if (m_trackerData.button & FLY_RIGHT_BUTTON)
    {
        glBegin(GL_POLYGON);
        glVertex2i(-8, -55);
        glVertex2i(-17, -50);
        glVertex2i(-17, -61);
        glVertex2i(-8, -66);
        glEnd();
    }
    if (m_trackerData.button & FLY_FRONT_BUTTON)
    {
        glBegin(GL_POLYGON);
        glVertex2i(-18, -30);
        glVertex2i(-27, -30);
        glVertex2i(-27, -42);
        glVertex2i(-18, -42);
        glEnd();
    }
    
    // flystick
    if (m_trackerData.quality < 0.0f) {
        glLineWidth(3.0f);
        glColor4f(1.0f, 0.3f, 0.3f, 1.0f);
        g_GUIController->osd()->drawInputDeviceText("NO SIGNAL", 0, -90);
    } else
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);    
    glBegin(GL_LINE_LOOP);
    glVertex2i(-12, 0);
    glVertex2i(-33, 0);
    glVertex2i(-33, -40);
    glVertex2i(-45, -57);
    glVertex2i(-45, -66);
    glVertex2i(-36, -75);
    glVertex2i(-9, -75);
    glVertex2i(0, -66);
    glVertex2i(0, -57);
    glVertex2i(-12, -40);
    glEnd();
    // flystick (3 buttons)
    glBegin(GL_LINE_LOOP);
    glVertex2i(-8, -55);
    glVertex2i(-17, -50);
    glVertex2i(-28, -50);
    glVertex2i(-37, -55);
    glVertex2i(-37, -66);
    glVertex2i(-28, -61);
    glVertex2i(-17, -61);
    glVertex2i(-8, -66);
    glEnd();
    glBegin(GL_LINES);
    glVertex2i(-28, -50);
    glVertex2i(-28, -61);
    glVertex2i(-17, -50);
    glVertex2i(-17, -61);
    glEnd();
    // flystick (1 button)
    glBegin(GL_LINE_LOOP);
    glVertex2i(-18, -30);
    glVertex2i(-27, -30);
    glVertex2i(-27, -42);
    glVertex2i(-18, -42);
    glEnd();
    
    // lines and text
    glLineWidth(1.0f);
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    //glBegin(GL_LINE_STRIP);
    //glVertex2i(-34, -60);
    //glVertex2i(-44, -70);
    //glVertex2i(-60, -70);
    //glEnd();
    //g_GUIController->osd()->drawInputDeviceText("Dummy", -65, -70);
    glBegin(GL_LINE_STRIP);
    glVertex2i(-22, -55);
    glVertex2i(-60, -55);
    glEnd();
    g_GUIController->osd()->drawInputDeviceText("Camera", -65, -55);
    if (!g_GUIController->visibleWindows()) {
        glBegin(GL_LINE_STRIP);
        glVertex2i(-11, -60);
        glVertex2i(-31, -40);
        glVertex2i(-60, -40);
        glEnd();
        if (g_GUIController->activeMenu())
            g_GUIController->osd()->drawInputDeviceText("Close Menu", -65, -40);
        else 
            g_GUIController->osd()->drawInputDeviceText("Open Menu", -65, -40);
    }
    glBegin(GL_LINE_STRIP);
    glVertex2i(-22, -35);
    glVertex2i(-32, -25);
    glVertex2i(-60, -25);
    glEnd();
    g_GUIController->osd()->drawInputDeviceText("Select", -65, -25);
}
