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

#include "MenuEventHandler.h"
#include "glgooey/core/Vector2.h"

#include "CommonServer.h"
#include "FANClasses.h"
#include "common.h"

#include "glgooey/WindowManager.h"
#include "glgooey/PropertyScheme.h"

#include "ProbeManager.h"
#include "Probe.h"
#include "Application.h"
#include "GUIController.h"
#include "Pointer.h"

#include "LoadModelWindow.h"
#include "PreferencesWindow.h"
#include "MessageWindow.h"
#include "RibbonProbeWindow.h"
#include "GlyphProbeWindow.h"
#include "LoginWindow.h"
#include "SliceProbeWindow.h"
#include "SimulationWindow.h"
#include "AboutWindow.h"
#include "ConfirmationWindow.h"
#include "HelpWindow.h"

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

MenuEventHandler::MenuEventHandler()
{

}

MenuEventHandler::~MenuEventHandler()
{

}

// ---------------------------------------------------------------------------
// Helper Functions
// ---------------------------------------------------------------------------

void MenuEventHandler::snapAngle(double& angle, double snapGrid, double tolerance)
{
    for (double a = -180.0f; a <= 180.0f; a = a + snapGrid) {
        if ((angle > a-tolerance) && (angle < a+tolerance))
            angle = a;
    }
}

// ---------------------------------------------------------------------------
// System Events
// ---------------------------------------------------------------------------

void MenuEventHandler::preferences()
{
    g_PreferencesWindow->showWindow();
}

void MenuEventHandler::help()
{
    g_HelpWindow->showWindow();
}

void MenuEventHandler::about()
{
    g_AboutWindow->showWindow();
}

void MenuEventHandler::login()
{
    g_LoginWindow->showWindow();
}

void MenuEventHandler::quit()
{
    if(modelConn != NULL)
    {
        modelConn->rpc("model::logout");
    }

    g_Application->quit(0);
}

void MenuEventHandler::confirmQuit()
{
    g_ConfirmationWindow->showWindow("Really quit?", this,
                                     &MenuEventHandler::quit, NULL);
}

void MenuEventHandler::openMainMenu()
{
    float x, y;
    g_GUIController->cursor()->getPosition(x, y);
    g_GUIController->getMenu("ROOT_MAIN")->showMenu(Gooey::Core::Vector2((int)x, (int)y));
}

// ---------------------------------------------------------------------------
// Model Events
// ---------------------------------------------------------------------------

void MenuEventHandler::loadModel()
{
    g_LoadModelWindow->showWindow(false);
}

void MenuEventHandler::saveModel()
{
    if(modelConn != NULL)
    {
        modelConn->rpc("model::saveAs", 0);
    }
}

void MenuEventHandler::confirmSaveModel()
{
    g_ConfirmationWindow->showWindow("Save model. Are you sure?", this,
                                     &MenuEventHandler::saveModel, NULL);
}

void MenuEventHandler::saveModelAs()
{
    g_LoadModelWindow->showWindow(true);
}

void MenuEventHandler::editModel()
{
    if (g_GUIController->state() == EDIT_MODE)
    {
        g_Application->model()->deselectFace();
        g_GUIController->setState(DEFAULT_MODE);
    } else
        g_GUIController->setState(EDIT_MODE);
}

// ---------------------------------------------------------------------------
// Simulation Events
// ---------------------------------------------------------------------------

void MenuEventHandler::editSimulation()
{
    g_SimulationWindow->showWindow();
}

void MenuEventHandler::resetMinMax()
{
    if(modelConn != NULL) {
        modelConn->rpc("sim::resetMinMax", 0);
    }
}

void MenuEventHandler::startStopSimulation()
{
    if(modelConn != NULL) {
        if(!g_GUIController->osd()->isSimulationRunning())
            modelConn->rpc("sim::start");
        else
            modelConn->rpc("sim::pause");
    }
}


// ---------------------------------------------------------------------------
// Probe Events
// ---------------------------------------------------------------------------

void MenuEventHandler::newRibbonProbe()
{
    g_ProbeManager->addProbe(PROBETYPE_RIBBON);
}

void MenuEventHandler::newSliceProbe()
{
    g_ProbeManager->addProbe(PROBETYPE_PLANE);
}

void MenuEventHandler::newGlyphProbe()
{
    g_ProbeManager->addProbe(PROBETYPE_GLYPH);
}

void MenuEventHandler::removeAllProbes()
{
    g_ProbeManager->removeAllProbes();
}

void MenuEventHandler::confirmRemoveAllProbes()
{
    g_ConfirmationWindow->showWindow("Remove ALL probes?", this,
                                     &MenuEventHandler::removeAllProbes, NULL);
}


// ----------------------------------------------------------------------------
// Camera events
// ----------------------------------------------------------------------------

float sphereFactor = 2.7f;

void MenuEventHandler::cameraPositionFront()
{
    osg::BoundingSphere sphere = g_Application->model()->getBound();
    osg::Matrixd matrix;
    matrix.makeLookAt(sphere.center() + osg::Vec3(-sphereFactor*sphere.radius(), 0.0f, 0.0f),
                      sphere.center(),
                      osg::Vec3(0.0f, 0.0f, 1.0f));
    matrix = osg::Matrix::inverse(matrix);
    g_Application->cameraManipulator()->setByMatrix(matrix);
}

void MenuEventHandler::cameraPositionBack()
{
    osg::BoundingSphere sphere = g_Application->model()->getBound();
    osg::Matrixd matrix;
    matrix.makeLookAt(sphere.center() + osg::Vec3(sphereFactor*sphere.radius(), 0.0f, 0.0f),
                      sphere.center(),
                      osg::Vec3(0.0f, 0.0f, 1.0f));
    matrix = osg::Matrix::inverse(matrix);
    g_Application->cameraManipulator()->setByMatrix(matrix);
}

void MenuEventHandler::cameraPositionLeft()
{
    osg::BoundingSphere sphere = g_Application->model()->getBound();
    osg::Matrixd matrix;
    matrix.makeLookAt(sphere.center() + osg::Vec3(0.0f, sphereFactor*sphere.radius(), 0.0f),
                      sphere.center(),
                      osg::Vec3(0.0f, 0.0f, 1.0f));
    matrix = osg::Matrix::inverse(matrix);
    g_Application->cameraManipulator()->setByMatrix(matrix);
}

void MenuEventHandler::cameraPositionRight()
{
    osg::BoundingSphere sphere = g_Application->model()->getBound();
    osg::Matrixd matrix;
    matrix.makeLookAt(sphere.center() + osg::Vec3(0.0f, -sphereFactor*sphere.radius(), 0.0f),
                      sphere.center(),
                      osg::Vec3(0.0f, 0.0f, 1.0f));
    matrix = osg::Matrix::inverse(matrix);
    g_Application->cameraManipulator()->setByMatrix(matrix);
}

void MenuEventHandler::cameraPositionTop()
{
    osg::BoundingSphere sphere = g_Application->model()->getBound();
    osg::Matrixd matrix;
    matrix.makeLookAt(sphere.center() + osg::Vec3(0.0f, 0.0f, sphereFactor*sphere.radius()),
                      sphere.center(),
                      osg::Vec3(1.0f, 0.0f, 0.0f));
    matrix = osg::Matrix::inverse(matrix);
    g_Application->cameraManipulator()->setByMatrix(matrix);
}

void MenuEventHandler::cameraPositionBottom()
{
    osg::BoundingSphere sphere = g_Application->model()->getBound();
    osg::Matrixd matrix;
    matrix.makeLookAt(sphere.center() + osg::Vec3(0.0f, 0.0f, -sphereFactor*sphere.radius()),
                      sphere.center(),
                      osg::Vec3(-1.0f, 0.0f, 0.0f));
    matrix = osg::Matrix::inverse(matrix);
    g_Application->cameraManipulator()->setByMatrix(matrix);
}

// ---------------------------------------------------------------------------
// Probe Events
// ---------------------------------------------------------------------------

void MenuEventHandler::editProbe()
{
    if (!(g_ProbeManager->selectedProbe()))
        return;

    switch (g_ProbeManager->selectedProbe()->probeType()) {
        case PROBETYPE_GLYPH:
            g_GlyphProbeWindow->showWindow((GlyphProbe*)g_ProbeManager->selectedProbe());
            break;

        case PROBETYPE_RIBBON:
            g_RibbonProbeWindow->showWindow((RibbonProbe*)g_ProbeManager->selectedProbe());
            break;

        case PROBETYPE_PLANE:
            g_SliceProbeWindow->showWindow((SliceProbe*)g_ProbeManager->selectedProbe());
            break;
            
        default:
            break;
    }
}

void MenuEventHandler::removeProbe()
{
    if (!(g_ProbeManager->selectedProbe()))
        return;

    g_ProbeManager->removeProbe(g_ProbeManager->selectedProbe()->id());
}

void MenuEventHandler::confirmRemoveProbe()
{
    if (!(g_ProbeManager->selectedProbe()))
        return;

    m_probeIdToRemove = g_ProbeManager->selectedProbe()->id();
    g_ConfirmationWindow->showWindow("Remove probe?", this,
                                     &MenuEventHandler::removeProbeWithStoredId, NULL);
}

void MenuEventHandler::removeProbeWithStoredId()
{
    g_ProbeManager->removeProbe(m_probeIdToRemove);
}

void MenuEventHandler::deselectProbe()
{
    if (!(g_ProbeManager->selectedProbe()))
        return;

    g_ProbeManager->deselectProbe();
}

void MenuEventHandler::snapProbeRotation45()
{
    if (!(g_ProbeManager->selectedProbe()))
        return;

    osg::Quat rot = g_ProbeManager->selectedProbe()->attitude();
    double x, y, z;
    quat2euler(rot, x, y, z);
    snapAngle(x, 45.0f, 22.5f);
    snapAngle(y, 45.0f, 22.5f);
    snapAngle(z, 45.0f, 22.5f);
    euler2quat(x, y, z, rot);
    g_ProbeManager->selectedProbe()->setAttitude(rot);
}

void MenuEventHandler::snapProbeRotation90()
{
    if (!(g_ProbeManager->selectedProbe()))
        return;

    osg::Quat rot = g_ProbeManager->selectedProbe()->attitude();
    double x, y, z;
    quat2euler(rot, x, y, z);
    snapAngle(x, 90.0f, 45.0f);
    snapAngle(y, 90.0f, 45.0f);
    snapAngle(z, 90.0f, 45.0f);
    euler2quat(x, y, z, rot);
    g_ProbeManager->selectedProbe()->setAttitude(rot);
}
