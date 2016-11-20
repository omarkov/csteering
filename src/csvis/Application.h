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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

#include <osg/ArgumentParser>
#include <osg/Group>
#include <osgProducer/Viewer>

#include "FANClasses.h"
#include "TriangulatedModel.h"
#include "SimulationSpace.h"


/**
 * This class holds the main application logic and is mainly responsible
 * for initializing the viewer, gui, input devices and the scene itself.
 */
class Application
{
    osg::ArgumentParser* m_arguments;
    osgProducer::Viewer* m_viewer;
    osg::ref_ptr<osg::Group> m_rootNode;
    osg::ref_ptr<TriangulatedModel> m_model;
    osg::ref_ptr<SimulationSpace> m_simulationSpace;
    osg::ref_ptr<osgGA::MatrixManipulator> m_cameraManipulator;

    bool m_shouldQuit;

    std::string m_dataPath;

    /// Initialize the application
    void init();

    /// Initialize the data path
    void initDataPath();
    /// Initialize the Viewer object
    void initViewer();
    /// Initialize the GUI
    void initGUI();
    /// Initialize the scene
    void initScene();

public:
    /// Constructor
    Application(osg::ArgumentParser* args);
    /// Destructor
    ~Application();

    /// Quit the application
    void quit(int status);
    /// Run the event-loop
    void run();

    /// Sets the datapath
    void setDataPath(const std::string& path);
    /// Return a new path relative to the data path
    std::string dataPath(const std::string& path) const;

    /// Return the passed arguments
    osg::ArgumentParser* arguments() const { return m_arguments; }
    /// Return the viewer object
    osgProducer::Viewer* viewer() const { return m_viewer; }
    /// Return the root node
    osg::ref_ptr<osg::Group> rootNode() const { return m_rootNode; }
    /// Return the triangulated model
    osg::ref_ptr<TriangulatedModel> model() const { return m_model; }
    /// Return the simulation space
    osg::ref_ptr<SimulationSpace> simulationSpace() const { return m_simulationSpace; }
    /// Return the current camera manipulator
    osg::ref_ptr<osgGA::MatrixManipulator> cameraManipulator() const { return m_cameraManipulator; }

    /// Clear the main scene
    void clearScene();
    /// Activate the scene after loading
    void activateScene();
    /// Load a triangulated model
    void loadModel(osg::ref_ptr<TriangulatedModel>);
};

extern Application* g_Application;

#endif // APPLICATION_H

