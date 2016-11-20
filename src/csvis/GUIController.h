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

#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include <osg/Group>
#include <osg/Timer>
#include <osgProducer/Viewer>

#include "glgooey/WindowManager.h"
#include "StandardWindow.h"
#include "Pointer.h"
#include "Menu.h"
#include "MenuEventHandler.h"
#include "OnscreenDisplay.h"


enum GUIState {
    DEFAULT_MODE,
    EDIT_MODE,
    CAMERA_MODE
};


class GUIController
{
    unsigned int m_screenWidth;
    unsigned int m_screenHeight;

    GUIState m_previousGUIState;
    GUIState m_currentGUIState;

    // start timer
    osg::Timer_t m_startTime;
    // frame timer
    osg::Timer_t m_frameTime;

    // Pointer to the WindowManager instance
    Gooey::WindowManager* m_windowManager;
    // Pointer to the cursor instance
    Pointer* m_cursor;
    // Pointer to the menu event handler
    MenuEventHandler* m_menuEventHandler;

    OnscreenDisplay* m_onscreenDisplay;

    // Pointer to the active menu
    Menu* m_activeMenu;
    // Maps of all Menus and MenuItems
    std::multimap<std::string, MenuItem*> m_menuItems;
    std::map<std::string, Menu*> m_menus;

    // Pointer to all windows
    std::list<StandardWindow*> m_windows;
    // visible windows count
    unsigned int m_visibleWindows;
    
    bool m_isCursorHidden;

    // Menu Management
    bool buildMenus();
    std::string getFirstMenuString(std::string& line);
    void deleteMenus();
    void addMenu(const std::string& parentMenu, const std::string& name, const std::string& text);
    void addMenuItem(const std::string& parentMenu, const std::string& action, const std::string& text);
    MenuItem* createMenuItem(const std::string& action, const std::string& text);

    // Window Management
    bool createAllWindows();
    void deleteAllWindows();

    // event handling
    friend class InputManager;
    friend class GUIDrawable;
    void handleDraw();
    bool handlePick();

public:
    // Default Constructor
    GUIController();
    // Destructor
    ~GUIController();

    // Initialize GLGooey
    bool init(osg::Group* rootNode);

    void setState(GUIState state);
    void setPreviousState();
    GUIState state() const { return m_currentGUIState; }

    // Resize the drawing canvas for the GUI
    void resizeCanvas(unsigned int width, unsigned int height);

    void setScreenSize(unsigned int width, unsigned int height);
    void screenSize(unsigned int& width, unsigned int& height) const;

    void setCursorHidden(bool hidden) { m_isCursorHidden = hidden; }
    bool cursorHidden() const { return m_isCursorHidden; }

    // Return a ptr to the WindowManager instance
    Gooey::WindowManager* windowManager() const { return m_windowManager; }
    // Return a ptr to the cursor instance
    Pointer* cursor() const { return m_cursor; }

    void resetCamera();

    /// Return a pointer to the onscreen display
    OnscreenDisplay* osd() const { return m_onscreenDisplay; }

    // Menu Management
    // ---------------
    // Return Pointer to a menu specified by it's name
    Menu* getMenu(const std::string& name);
    // Return a ptr to the active menu
    Menu* activeMenu() const { return m_activeMenu; }
    // set active menu
    void setActiveMenu(Menu* activeMenu) { m_activeMenu = activeMenu; }
    //enable/disable MenuItems by their action
    void enableMenuItems(const std::string& action);
    void disableMenuItems(const std::string& action);
    
    // Window Management
    // -----------------
    // get visible windows count
    unsigned int visibleWindows() { return m_visibleWindows; }
    // set visible windows count
    void setVisibleWindows(unsigned int visibleWindows) { m_visibleWindows = visibleWindows; }
    // enabled and disable all windows
    void enableAllWindows();
    void disableAllWindows(StandardWindow* exception);
    // close Windows    
    void hideAllWindows();
    // Skin
    void loadSkin(const std::string& skin);
};

extern GUIController* g_GUIController;

#endif // GUICONTROLLER_H
