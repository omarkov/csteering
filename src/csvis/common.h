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

#ifndef COMMON_H
#define COMMON_H

#include <osg/Node>
#include <osg/Vec3>
#include <osg/Quat>
#include "../common/simRemoteTypes.h"

// macros
#define zap(p) if(p) { delete p; p = NULL; }
#define zap_array(p) if(p) { delete[] p; p = NULL; }

void vec2vertex(const osg::Vec3 &vec, vertex_t *vertex);

void osg2sym(osg::Vec3& in, osg::Vec3& out);
void sym2osg(osg::Vec3& in, osg::Vec3& out);

void osg2asym(osg::Vec3& in, osg::Vec3& out);
void asym2osg(osg::Vec3& in, osg::Vec3& out);

void quat2euler(osg::Quat& in, double& outX, double& outY, double& outZ);
void euler2quat(double inX, double inY, double inZ, osg::Quat& out);

void printPositionToObjectInfo(const std::string& title, const osg::Vec3& pos);

void warning(char *msg, ...);
void error(char *msg, ...);

osg::Matrixd worldTransformForNode(osg::Node* node);

#endif // COMMON_H
