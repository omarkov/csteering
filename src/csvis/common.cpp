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

#include <stdarg.h>

#include "FAN.h"
#include "Application.h"
#include "GUIController.h"
#include "MessageWindow.h"
#include "common.h"

const double PI = 3.14159f;

void vec2vertex(const osg::Vec3 &vec, vertex_t *vertex) 
{
    vertex->x = vec.x();
    vertex->y = vec.y();
    vertex->z = vec.z();
}

void osg2asym(osg::Vec3& in, osg::Vec3& out)
{
    double fx, fy, fz;
    g_Application->model()->scalingFactors(fx, fy, fz);

    out.set(in.x() / fx, in.y() / fy, in.z() / fz);
}

void asym2osg(osg::Vec3& in, osg::Vec3& out)
{
    double fx, fy, fz;
    g_Application->model()->scalingFactors(fx, fy, fz);

    out.set(in.x() * fx, in.y() * fy, in.z() * fz);
}

void osg2sym(osg::Vec3& in, osg::Vec3& out)
{
    float f = g_Application->model()->scalingFactor();

    out.set(in.x() / f, in.y() / f, in.z() / f);
}

void sym2osg(osg::Vec3& in, osg::Vec3& out)
{
    float f = g_Application->model()->scalingFactor();

    out.set(in.x() * f, in.y() * f, in.z() * f);
}

void quat2euler(osg::Quat& in, double& outX, double& outY, double& outZ)
{
/*Local sint#, cost#, sinv#, cosv#, sinf#, cosf# 
Local cost_temp# 
 
    float sint = (2.0f * in.w() * in.y()) - (2.0f * in.x() * in.z());
    float cost = 1.0f - (sint * sint);

    if abs(cost) > QuatToEulerAccuracy
cost = Sqr(cost_temp) 
Else 
cost = 0 
EndIf 
 
If Abs(cost) > QuatToEulerAccuracy 
sinv = ((2. * src\y * src\z) + (2 * src\w * src\x)) / cost 
cosv = (1. - (2. * src\x * src\x) - (2 * src\y * src\y)) / cost 
sinf = ((2. * src\x * src\y) + (2 * src\w * src\z)) / cost 
cosf = (1. - (2. * src\y * src\y) - (2 * src\z * src\z)) / cost 
Else 
sinv = (2 * src\w * src\x) - (2 * src\y * src\z) 
cosv = 1 - (2 * src\x * src\x) - (2 * src\z * src\z) 
sinf = 0 
cosf = 1 
EndIf 
 
; Generate the output rotation 
out\roll = -ATan2(sinv, cosv) ; inverted due to change in handedness of coordinate system 
out\pitch = ATan2(sint, cost) 
out\yaw = ATan2(sinf, cosf) 
*/

    double heading, attitude, bank;
    double sqw = in.w()*in.w();
    double sqx = in.x()*in.x();
    double sqy = in.y()*in.y();
    double sqz = in.z()*in.z();
	double unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	double test = in.x()*in.y() + in.z()*in.w();
	if (test > 0.499*unit) { // singularity at north pole
		heading = 2.0f * atan2(in.x(),in.w());
		attitude = PI/2.0f;
		bank = 0;
	} else if (test < -0.499*unit) { // singularity at south pole
		heading = -2.0f * atan2(in.x(),in.w());
		attitude = PI/2.0f;
		bank = 0;
	} else {
        heading = atan2(2.0f*in.y()*in.w()-2.0f*in.x()*in.z() , sqx - sqy - sqz + sqw);
	    attitude = asin(2.0f*test/unit);
    	bank = atan2(2.0f*in.x()*in.w()-2.0f*in.y()*in.z() , -sqx + sqy - sqz + sqw);
    }
    outX = heading*180.0f/PI;
    outY = attitude*180.0f/PI;
    outZ = bank*180.0f/PI;

}

void euler2quat(double inX, double inY, double inZ, osg::Quat& out)
{
/*    float roll = inX * 180.0f / PI;
    float pitch = inY * 180.0f / PI;
    float yaw = inZ * 180.0f / PI;

    // NB roll is inverted due to change in handedness of coordinate systems
    float cr = cos(-roll/2);
    float cp = cos(pitch/2);
    float cy = cos(yaw/2);

    float sr = sin(-roll/2);
    float sp = sin(pitch/2);
    float sy = sin(yaw/2);

    // These variables are only here to cut down on the number of multiplications
    float cpcy = cp * cy;
    float spsy = sp * sy;
    float spcy = sp * cy;
    float cpsy = cp * sy;

    // Generate the output quat
    out.w = cr * cpcy + sr * spsy;
    out.x = sr * cpcy - cr * spsy;
    out.y = cr * spcy + sr * cpsy;
    out.z = cr * cpsy - sr * spcy;*/
    
    double heading = inX*PI/180.0f;
    double attitude = inY*PI/180.0f;
    double bank = inZ*PI/180.0f;
    double c1 = cos(heading/2.0f);
    double s1 = sin(heading/2.0f);
    double c2 = cos(attitude/2.0f);
    double s2 = sin(attitude/2.0f);
    double c3 = cos(bank/2.0f);
    double s3 = sin(bank/2.0f);
    double c1c2 = c1*c2;
    double s1s2 = s1*s2;
    out.w() = c1c2*c3 - s1s2*s3;
    out.x() = c1c2*s3 + s1s2*c3;
    out.y() = s1*c2*c3 + c1*s2*s3;
    out.z() = c1*s2*c3 - s1*c2*s3;
}

void printPositionToObjectInfo(const std::string& title, const osg::Vec3& pos)
{
    char x_pos[15], y_pos[15], z_pos[15];

    snprintf(x_pos, sizeof(x_pos), "X: [%5.2f]", pos.x());
    snprintf(y_pos, sizeof(y_pos), "Y: [%5.2f]", pos.y());
    snprintf(z_pos, sizeof(z_pos), "Z: [%5.2f]", pos.z());

    g_GUIController->osd()->showObjectInfo(title, x_pos, y_pos, z_pos);
}



void warning(char *fmt, ...)
{
    char msg[1024];
    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 1024, fmt, args);
    va_end(args);

    g_MessageWindow->showWindow(MESSAGETYPE_WARNING, msg);
}

void error(char *fmt, ...)
{
    char msg[1024];
    va_list args;

    va_start(args, fmt);
    vsnprintf(msg, 1024, fmt, args);
    va_end(args);

    g_MessageWindow->showWindow(MESSAGETYPE_ERROR, msg);
    FAN_err(msg);
}



class WorldTransformVisitor : public osg::NodeVisitor
{
    bool m_finished;

public:

    osg::Matrixd matrix;

    WorldTransformVisitor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_PARENTS)
    {
        m_finished = false;
    }

    virtual void apply(osg::Node &node)
    {
        if (!m_finished) {
            if (!node.getNumParents()) {
                matrix.set(osg::computeLocalToWorld(getNodePath()));
                m_finished = true;
            }

            traverse(node);
        }
    }
};

osg::Matrixd worldTransformForNode(osg::Node* node)
{
    WorldTransformVisitor visitor;

    if (node) {
        node->accept(visitor);
        return visitor.matrix;
    } else {
        return osg::Matrixd::identity();
    }
}
