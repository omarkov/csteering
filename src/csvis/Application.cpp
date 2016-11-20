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

#include <assert.h>

#include "common.h"

#include "Application.h"
#include "GUIController.h"
#include "ProbeManager.h"
#include "InputManager.h"
#include "EventHandler.h"
#include "TriangulatedModel.h"
#include "InteractiveTrackballManipulator.h"
#include "InteractiveManipulator.h"
#include "MenuEventHandler.h"
#include "glgooey/core/Vector2.h"
#include "LoginWindow.h"

#include <unistd.h>     /* standard unix functions, like getpid()       */
#include <sys/types.h>  /* various type definitions, like pid_t         */
#include <signal.h>     /* signal name macros, and the kill() prototype */

#include "Server.h"



Application* g_Application = NULL;


// ---------------------------------------------------------------------------
// Construction/Deconstruction
//
// No initialization is done here, only (de)allocation.
// ---------------------------------------------------------------------------

Application::Application(osg::ArgumentParser* args)
    : m_arguments(0), m_viewer(0)
{
    m_arguments = args;
    m_shouldQuit = false;

    g_Application = this;

    init();
}

Application::~Application()
{
    zap(m_viewer);

    zap(g_InputManager);
    zap(g_ProbeManager);
    zap(g_GUIController);
}


// ---------------------------------------------------------------------------
// Initialization/Shutdown
// ---------------------------------------------------------------------------

void Application::init()
{
    // allocate the global managers
    g_InputManager = new InputManager;
    g_ProbeManager = new ProbeManager;
    g_GUIController = new GUIController;

    initDataPath();
    initViewer();
    initScene();

    if (!g_InputManager->init())
        return;

    if (!g_ProbeManager->init(m_rootNode.get()))
        return;

    if (!g_GUIController->init(m_rootNode.get()))
        return;

    // report any errors if they have occured when parsing the program aguments.
    if (m_arguments->errors())
        m_arguments->writeErrorMessages(std::cout);
}

void Application::quit(int status)
{
    m_shouldQuit = true;
#ifndef __IRIX__
    kill(getpid(), SIGINT);
#endif
}

void Application::initDataPath()
{
    // default
    m_dataPath = "../data/";

    // check the environment first
    char *env = getenv("CSVIS_DATA_PATH");
    if (env)
        setDataPath(env);

    // check arguments
    std::string path;
    if (m_arguments->read("-d", path) || m_arguments->read("--data-path", path))
        setDataPath(path);
}

void Application::initViewer()
{
    // don't initialize twice
    assert(!m_viewer);

    // construct and setup the viewer
    m_viewer = new osgProducer::Viewer();

    m_viewer->setUpViewer(osgProducer::Viewer::STATE_MANIPULATOR |
                          osgProducer::Viewer::HEAD_LIGHT_SOURCE |
                          osgProducer::Viewer::STATS_MANIPULATOR |
                          osgProducer::Viewer::VIEWER_MANIPULATOR);
    m_viewer->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

    
    if(!FAN::app->config->checkKey("visinputdevice", "dtrack"))
    {
        m_cameraManipulator = new InteractiveTrackballManipulator();
    }else
    {
        m_cameraManipulator = new InteractiveManipulator();
    }

    int pos = m_viewer->addCameraManipulator(m_cameraManipulator.get());
    m_viewer->selectCameraManipulator(pos);

    // add our event handler
    m_viewer->getEventHandlerList().push_front(new EventHandler(m_viewer));
}


// ----------------------------------------------------------------------------
// Path management
// ----------------------------------------------------------------------------

void Application::setDataPath(const std::string& path)
{
    // check for an appended slash and add one if it is missing
    if (path[path.size()] != '/')
        m_dataPath = path + "/";
    else
        m_dataPath = path;
}

std::string Application::dataPath(const std::string& path) const
{
    // we need at least one character to have a meaningful result
    if (path.empty())
        return m_dataPath;

    // remove a prepended slash
    if (path[0] == '/')
        return m_dataPath + path.substr(1, path.size()-1);
    else
        return m_dataPath + path;
}


// ----------------------------------------------------------------------------
// Scene management
// ----------------------------------------------------------------------------

void Application::clearScene()
{
//    m_rootNode = new osg::Group();
//    m_rootNode->setName("Root node");
}

void Application::activateScene()
{
    g_GUIController->resetCamera();
}

void Application::loadModel(osg::ref_ptr<TriangulatedModel> model)
{
    // do we already have a model?
    if (m_model.valid()) {
        // replace it
        m_rootNode->replaceChild(m_model.get(), model.get());
        m_model.release();
    } else {
        m_rootNode->addChild(model.get());
    }

    m_model = model;
    
    g_GUIController->enableMenuItems("SAVE_MODEL");
    g_GUIController->enableMenuItems("CONFIRM_SAVE_MODEL");
    g_GUIController->enableMenuItems("SAVE_MODEL_AS");
    g_GUIController->enableMenuItems("EDIT_MODEL");
    
    g_GUIController->enableMenuItems("NEW_PROBE_RIBBON");
    g_GUIController->enableMenuItems("NEW_PROBE_GLYPH");
    g_GUIController->enableMenuItems("NEW_PROBE_SLICE");
    g_GUIController->enableMenuItems("REMOVE_ALL_PROBES");
    g_GUIController->enableMenuItems("CONFIRM_REMOVE_ALL_PROBES");
}

void Application::initScene(void)
{

    m_rootNode = new osg::Group();
    m_rootNode->setName("Root node");

    // clearScene();

    // load a model
    osg::ref_ptr<TriangulatedModel> model = new TriangulatedModel();

    loadModel(model);

    // add the simulation space
    m_simulationSpace = new SimulationSpace();
    m_rootNode->addChild(m_simulationSpace.get());

    m_viewer->setSceneData(m_rootNode.get());

    activateScene();
}


// ---------------------------------------------------------------------------
// Running
//
// This just runs through the osgProducer::Viewer event loop.
// ---------------------------------------------------------------------------

void Application::run(void)
{
    // create the windows and run the threads.
    m_viewer->realize();

    // tell the window manager the size of our window
    Producer::RenderSurface* rs =  m_viewer->getCamera(0)->getRenderSurface();
    assert(rs);

    unsigned int width = rs->getWindowWidth();
    unsigned int height = rs->getWindowHeight();

    g_GUIController->resizeCanvas(width, height);

    g_LoginWindow->showWindow();

    while (!m_shouldQuit && !m_viewer->done()) {
        m_viewer->sync();
        m_viewer->update();
        m_viewer->frame();
    }

    m_viewer->sync();    
   zap(g_InputManager);
}

