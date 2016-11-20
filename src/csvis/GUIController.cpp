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
#include <fstream>
#include <stdarg.h>

#include <osg/GL>
#include <osg/Drawable>
#include <osgProducer/Viewer>

#include "glgooey/Renderer.h"
#include "glgooey/WindowManager.h"
#include "glgooey/PropertyScheme.h"
#include "glgooey/core/Vector2.h"

#include "common.h"
#include "FANClasses.h"

#include "Application.h"
#include "InputManager.h"
#include "GUIController.h"
#include "ProbeManager.h"
#include "MenuItem.h"
#include "Menu.h"
#include "MenuEventHandler.h"

#include "StandardWindow.h"
#include "LoginWindow.h"
#include "PreferencesWindow.h"
#include "LoadModelWindow.h"
#include "MessageWindow.h"
#include "RibbonProbeWindow.h"
#include "GlyphProbeWindow.h"
#include "SliceProbeWindow.h"
#include "SimulationWindow.h"
#include "AboutWindow.h"
#include "ConfirmationWindow.h"
#include "HelpWindow.h"

#include "NodeFlags.h"


GUIController* g_GUIController = NULL;



// ----------------------------------------------------------------------------
// The drawable responsible for GUI rendering
// ----------------------------------------------------------------------------

class GUIDrawable : public osg::Drawable
{
public:
    META_Object(csvis,GUIDrawable)

    GUIDrawable() // : Drawable()
    {
        setSupportsDisplayList(false);
    }

    GUIDrawable(const GUIDrawable& other, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY)
        // : Drawable(other, op)
    {
        setSupportsDisplayList(false);
    }

    void drawImplementation(osg::State& state) const
    {
        state.setActiveTextureUnit(0);
        g_GUIController->handleDraw();
    }
};


// ----------------------------------------------------------------------------
// Construction/Destruction
// ----------------------------------------------------------------------------

GUIController::GUIController()
    : m_screenWidth(0), m_screenHeight(0), m_windowManager(0), m_menuEventHandler(0),
      m_onscreenDisplay(0), m_activeMenu(0), m_visibleWindows(0), m_isCursorHidden(false)
{
    m_previousGUIState = DEFAULT_MODE;
    m_currentGUIState = DEFAULT_MODE;
    m_cursor = new Pointer;
}

GUIController::~GUIController()
{
    zap(m_cursor);

    // delete menus
    deleteMenus();

    // delete windows
    deleteAllWindows();
}


// ---------------------------------------------------------------------------
// Initialization/Shutdown
// ---------------------------------------------------------------------------

bool GUIController::init(osg::Group* rootNode)
{
    assert(rootNode);

    // add a node for drawing into
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->setNodeMask(DONT_IGNORE_FLAG);
    geode->setName("GUI Geode");
    geode->addDrawable(new GUIDrawable());
    rootNode->addChild(geode.get());

    // disable lighting and depth test
    osg::StateSet *stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    stateset->setRenderBinDetails(11, "RenderBin");

    // try to get the windowmanager
    if ((m_windowManager = &Gooey::WindowManager::instance()) == NULL)
        return false;

    // initialize GLGooey
    m_windowManager->initialize(g_Application->dataPath("fonts/andalemo.ttf"), isShiftPressed, isAltPressed, isCtrlPressed);
    loadSkin(FAN::app->config->getValue("VISSKIN", "redskin"));

    // initialize the OSD
    m_onscreenDisplay = new OnscreenDisplay();

    // initialize our menus
    if (!buildMenus())
        return false;
        
    // create our windows
    if (!createAllWindows())
        return false;

    m_startTime = osg::Timer::instance()->tick();

    return true;
}

// ---------------------------------------------------------------------------
// Basic controls
// ---------------------------------------------------------------------------

void GUIController::resizeCanvas(unsigned int width, unsigned int height)
{
    m_windowManager->applicationResized(width, height);
    m_onscreenDisplay->setScreenSize(width, height);
}

void GUIController::setScreenSize(unsigned int width, unsigned int height)
{
    m_screenWidth = width;
    m_screenHeight = height;

    resizeCanvas(width, height);
}

void GUIController::screenSize(unsigned int& width, unsigned int& height) const
{
    width = m_screenWidth;
    height = m_screenHeight;
}

void GUIController::setState(GUIState state)
{
    m_previousGUIState = m_currentGUIState;
    m_currentGUIState = state;

    m_onscreenDisplay->changedGUIState();
}

void GUIController::setPreviousState()
{
    m_currentGUIState = m_previousGUIState;
    m_previousGUIState = DEFAULT_MODE;

    m_onscreenDisplay->changedGUIState();
}


// ---------------------------------------------------------------------------
// Event handling
// ---------------------------------------------------------------------------

void GUIController::handleDraw()
{
    assert(m_cursor);

    // set up orthographic projection et. al
    Gooey::Renderer::preRender();

    m_frameTime = osg::Timer::instance()->tick();
    double frameDelta = osg::Timer::instance()->delta_m(m_startTime, m_frameTime);

    m_onscreenDisplay->draw(frameDelta);

    if (m_activeMenu)
        m_activeMenu->drawMenu(frameDelta);

    Gooey::WindowManager::instance().update();

    if (!m_isCursorHidden)
        m_cursor->draw(frameDelta);

    m_startTime = m_frameTime;

    Gooey::Renderer::postRender();
}

bool GUIController::handlePick()
{
    osgUtil::IntersectVisitor::HitList hitList;
    float x, y;

    // don't handle anything while in camera mode
    if (state() == CAMERA_MODE)
        return false;

    m_cursor->getPosition(x, y);

    x = (2.0f*x - (float)m_screenWidth) / (float)m_screenWidth;
    y = -((2.0f*y - (float)m_screenHeight) / (float)m_screenHeight);

    // compute intersections
    if (g_Application->viewer()->computeIntersections(x, y, (unsigned int)0, hitList, CLICKABLE_FLAG)) {
        osgUtil::IntersectVisitor::HitList::iterator it;
        
        // go through each hit
        for (it = hitList.begin(); it != hitList.end(); ++it) {
            // we need a valid and named node
            if (!it->_geode.valid())
                return false;

            std::string name = it->_geode->getName();

            if (name.empty())
                return false;

            // did we pick a probe?
            if (name.find("Probe") != std::string::npos) {
                g_Application->model()->deselectFace();
                std::string id = name.substr(6, name.length()-6);
                g_ProbeManager->selectProbe((unsigned int)strtol(id.c_str(), NULL, 10));
                return true;
            }

            // did we pick a control point?
            if (name.find("CP") != std::string::npos) {
                g_ProbeManager->deselectProbe();
                if (state() == EDIT_MODE) {
                    std::string index = name.substr(3, name.length()-3);
                    g_Application->model()->selectedFace()->selectControlPoint((unsigned int)strtol(index.c_str(), NULL, 10));
                }
                return true;
            }

            // did we pick a face?
            if (name.find("Face") != std::string::npos) {
                g_ProbeManager->deselectProbe();
                if (state() == EDIT_MODE) {
                    std::string id = name.substr(5, name.length()-5);
                    g_Application->model()->selectFace((unsigned int)strtol(id.c_str(), NULL, 10));
                }
                return true;
            }

        }
    }

    // we picked nothing
    g_ProbeManager->deselectProbe();
    g_Application->model()->deselectFace();

    return false;
}


void GUIController::resetCamera()
{
    m_menuEventHandler->cameraPositionFront();
}


// ---------------------------------------------------------------------------
// Menu Management
// ---------------------------------------------------------------------------

bool GUIController::buildMenus()
{
    m_menuEventHandler = new MenuEventHandler();

    Menu* root_menu = new Menu("");
    m_menus.insert(std::pair<std::string, Menu*>("ROOT", root_menu));
    addMenu("ROOT", "ROOT_MAIN", "");
    addMenu("ROOT", "ROOT_PROBE", "");

    const int buffer_size = 256; 
    char buffer[buffer_size]; 
    std::ifstream menuStructure("../etc/MenuStructure.conf");
    while (!menuStructure.eof()) {
        int type = 0;
        menuStructure.getline(buffer, buffer_size);
        std::string line(buffer);
        // does the line start with "addMenu(" or "addMenuItem("?
        if (line.substr(0, 8) == "addMenu(") {
            line = line.substr(8);
            type = 1;
        } else if (line.substr(0, 12) == "addMenuItem(") {
            line = line.substr(12);
            type = 2;
        }
        if (type > 0) {
            std::string parent = getFirstMenuString(line);
            std::string name = getFirstMenuString(line);
            std::string text = getFirstMenuString(line);
            if ((parent != "") && (name != "")) {
                if (type == 1)
                    addMenu(parent, name, text);
                else
                    addMenuItem(parent, name, text);
            } else {
                std::cout<<"Syntax Error in MenuStructure: " << buffer << std::endl;
            }
        }
    }
    menuStructure.close();

    disableMenuItems("SAVE_MODEL");
    disableMenuItems("CONFIRM_SAVE_MODEL");
    disableMenuItems("SAVE_MODEL_AS");
    disableMenuItems("EDIT_MODEL");
    
    disableMenuItems("NEW_PROBE_RIBBON");
    //disableMenuItems("NEW_PROBE_GLYPH");
    disableMenuItems("NEW_PROBE_SLICE");
    disableMenuItems("REMOVE_ALL_PROBES");
    disableMenuItems("CONFIRM_REMOVE_ALL_PROBES");
    
    disableMenuItems("DESELECT_PROBE");
    disableMenuItems("EDIT_PROBE");
    disableMenuItems("CONFIRM_REMOVE_PROBE");
    disableMenuItems("REMOVE_PROBE");
    disableMenuItems("SNAP_PROBE_ROTATION");

    return true;
}

std::string GUIController::getFirstMenuString(std::string& line)
{
    unsigned int quotePos = line.find("\"");
    if (quotePos >= line.size())
        return std::string("");
    line = line.substr(quotePos+1);
    quotePos = line.find("\"");
    if (quotePos >= line.size())
        return std::string("");
    std::string res = line.substr(0, quotePos);
    line = line.substr(quotePos+1);
    return res;
}

void GUIController::deleteMenus()
{
    // delete menus (recursive)
    Menu* root = getMenu("ROOT");
    zap(root);
    zap(m_menuEventHandler);
}

void GUIController::addMenu(const std::string& parentMenu, const std::string& name, const std::string& text)
{
    if (!getMenu(name)) {
        Menu* parent_menu = getMenu(parentMenu);
        if (parent_menu) {
            Menu* new_menu = new Menu(text);
            parent_menu->addMenuItem(new_menu);
            m_menus.insert(std::pair<std::string, Menu*>(name, new_menu));
        } else {
            std::cout<<"Error creating Menu '" << name <<
                       "': Parent Menu '" << parentMenu << "' does not exist" << std::endl;
        }
    } else {
        std::cout<<"Error creating Menu '" << name <<
                   "': Menu does already exist" << std::endl;
    }
}

void GUIController::addMenuItem(const std::string& parentMenu, const std::string& action, const std::string& text)
{
    Menu* parent_menu = getMenu(parentMenu);
    if (parent_menu) {
        MenuItem* new_menuItem = createMenuItem(action, text);
        parent_menu->addMenuItem(new_menuItem);
        m_menuItems.insert(std::pair<std::string, MenuItem*>(action, new_menuItem));
    } else {
        std::cout<<"Error creating MenuItem with Action '" << action <<
                   "': Parent Menu '" << parentMenu << "' does not exist" << std::endl;
    }
}

Menu* GUIController::getMenu(const std::string& name)
{
    std::map<std::string, Menu*>::iterator iter = m_menus.find(name);
    if (iter != m_menus.end()) {
        return iter->second;
    } else {
        return NULL;
    }
}

void GUIController::enableMenuItems(const std::string& action)
{
    std::map<std::string, MenuItem*>::iterator it;
    for (it = m_menuItems.lower_bound(action); it != m_menuItems.upper_bound(action); ++it)
        if (it->second)
            {
            (it->second)->enableMenuItem();
            }
}

void GUIController::disableMenuItems(const std::string& action)
{
    std::map<std::string, MenuItem*>::iterator it;
    for (it = m_menuItems.lower_bound(action); it != m_menuItems.upper_bound(action); ++it)
        if (it->second)
            {
            (it->second)->disableMenuItem();
            }
}

MenuItem* GUIController::createMenuItem(const std::string& action, const std::string& text)
{
    if (action == "SHOW_PREFERENCES")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::preferences);
    else if (action == "SHOW_HELP")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::help);
    else if (action == "SHOW_ABOUT")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::about);
    else if (action == "QUIT")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::quit);
    else if (action == "LOGIN")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::login);
    else if (action == "OPEN_MAIN_MENU")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::openMainMenu);
    else if (action == "CONFIRM_QUIT")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::confirmQuit);
    else if (action == "EDIT_SIMULATION")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::editSimulation);
    else if (action == "RESET_MINMAX")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::resetMinMax);
    else if (action == "START_STOP_SIMULATION")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::startStopSimulation);
    else if (action == "SAVE_MODEL")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::saveModel);
    else if (action == "CONFIRM_SAVE_MODEL")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::confirmSaveModel);
    else if (action == "SAVE_MODEL_AS")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::saveModelAs);
    else if (action == "LOAD_MODEL")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::loadModel);
    else if (action == "EDIT_MODEL")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::editModel);
    else if (action == "NEW_PROBE_RIBBON")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::newRibbonProbe);
    //else if (action == "NEW_PROBE_GLYPH")
    //    return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::newGlyphProbe);
    else if (action == "NEW_PROBE_SLICE")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::newSliceProbe);
    else if (action == "REMOVE_ALL_PROBES")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::removeAllProbes);
    else if (action == "CONFIRM_REMOVE_ALL_PROBES")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::confirmRemoveAllProbes);
    else if (action == "CAMERA_TOP")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::cameraPositionTop);
    else if (action == "CAMERA_RIGHT")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::cameraPositionRight);
    else if (action == "CAMERA_BACK")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::cameraPositionBack);
    else if (action == "CAMERA_BOTTOM")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::cameraPositionBottom);
    else if (action == "CAMERA_FRONT")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::cameraPositionFront);
    else if (action == "CAMERA_LEFT")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::cameraPositionLeft);
    else if (action == "EDIT_PROBE")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::editProbe);
    else if (action == "REMOVE_PROBE")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::removeProbe);
    else if (action == "CONFIRM_REMOVE_PROBE")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::confirmRemoveProbe);
    else if (action == "DESELECT_PROBE")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::deselectProbe);
    else if (action == "SNAP_PROBE_ROTATION_45")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::snapProbeRotation45);
    else if (action == "SNAP_PROBE_ROTATION_90")
        return new MenuItem(text, m_menuEventHandler, &MenuEventHandler::snapProbeRotation90);
    else
    {
        std::cout<<"Error creating MenuItem: Action '" << action << "' not found" << std::endl;
        return new MenuItem(text, NULL, NULL);
    }
}

// ---------------------------------------------------------------------------
// Window Management
// ---------------------------------------------------------------------------

void GUIController::enableAllWindows()
{
    std::list<StandardWindow*>::iterator it;
    for (it = m_windows.begin(); it != m_windows.end(); ++it) {
        if ((*it))
            (*it)->enableWindow();
    }
}

void GUIController::disableAllWindows(StandardWindow* exception)
{
    std::list<StandardWindow*>::iterator it;
    for (it = m_windows.begin(); it != m_windows.end(); ++it) {
        if ((*it) && ((*it) != exception))
            (*it)->disableWindow();
    }
}

bool GUIController::createAllWindows()
{
    m_windows.push_back(new LoginWindow());
    m_windows.push_back(new PreferencesWindow());
    m_windows.push_back(new LoadModelWindow());
    m_windows.push_back(new MessageWindow());
    m_windows.push_back(new RibbonProbeWindow());
    m_windows.push_back(new GlyphProbeWindow());
    m_windows.push_back(new SliceProbeWindow());
    m_windows.push_back(new SimulationWindow());
    m_windows.push_back(new AboutWindow());
    m_windows.push_back(new ConfirmationWindow());
    m_windows.push_back(new HelpWindow());
    return true;
}

void GUIController::deleteAllWindows()
{
    std::list<StandardWindow*>::iterator it;
    for (it = m_windows.begin(); it != m_windows.end(); ++it) {
        if ((*it))
            delete (*it);
    }
    m_windows.clear();
}

void GUIController::hideAllWindows()
{
    std::list<StandardWindow*>::iterator it;
    for (it = m_windows.begin(); it != m_windows.end(); ++it) {
        if ((*it) && ((*it) != g_MessageWindow))
            (*it)->hideWindow();
    }
}

void GUIController::loadSkin(const std::string& skin)
{
    m_windowManager->loadPropertyScheme(g_Application->dataPath("skins/") + skin + ".xml");
}
