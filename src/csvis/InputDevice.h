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

#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <string>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/Matrixd>

class InputDevice
{
    std::string m_deviceName;
    bool m_isEnabled;

protected:
    osg::Vec3 m_position;
    osg::Quat m_rotation;
    osg::Vec3 m_cposition;
    osg::Quat m_crotation;

    float m_yOffset;
    float m_sensitivityFactor;
    float m_accelerationFactor;

public:
    InputDevice(const std::string& name)
        : m_isEnabled(false), m_sensitivityFactor(1.0f), m_accelerationFactor(1.0f)
    {
        m_deviceName = name;
    }

    virtual ~InputDevice() {}

    // enable or disable this device
    void setEnabled(bool enabled) { m_isEnabled = enabled; }
    // check if the device is enabled
    bool isEnabled() { return m_isEnabled; }

    // set the name of this device
    void setDeviceName(const std::string& name) { m_deviceName = name; }
    // get the name of this device
    const std::string& deviceName() { return m_deviceName; }

    void setyOffset(float offset) { m_yOffset = offset; }
    float yOffset() { return m_yOffset; }
    void setSensitivityFactor(float factor) { m_sensitivityFactor = factor; }
    float sensitivityFactor() { return m_sensitivityFactor; }
    void setAccelerationFactor(float factor) { m_accelerationFactor = factor; }
    float accelerationFactor() { return m_accelerationFactor; }

    // get the 3D position of the device
    osg::Vec3 position() const { return m_position; }
    // get the orientation of the device
    osg::Quat rotation() const { return m_rotation; }
  
    osg::Matrixd matrix() const
    {
        return osg::Matrixd::rotate(m_rotation) * 
               osg::Matrixd::translate(m_position);
    }

    osg::Matrixd inverseMatrix() const
    {
        return osg::Matrixd::inverse(osg::Matrixd::rotate(m_rotation) * 
                                     osg::Matrixd::translate(m_position));
    }

    
    osg::Matrixd cameraMatrix() const
    {
        return osg::Matrixd::rotate(m_crotation) * 
               osg::Matrixd::translate(m_cposition);
    }

    osg::Matrixd inverseCameraMatrix() const
    {
        return osg::Matrixd::inverse(osg::Matrixd::rotate(m_crotation) * 
                                     osg::Matrixd::translate(m_cposition));
    }

  
    // update device data
    virtual void update() = 0;

    // does the device provide 3D data?
    virtual bool provides3DData() = 0;
    
    // draw device information
    virtual void draw() = 0;
};

#endif // INPUTDEVICE_H
