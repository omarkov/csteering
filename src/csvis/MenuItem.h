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

#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>

#include "glgooey/core/Vector2.h"
#include "glgooey/StaticText.h"
#include "glgooey/sigslot.h"
#include "glgooey/services/glfont/TypeFace.h"
#include "MenuEventHandler.h"

#include "Menu.h"

class Menu;

class MenuItem : public Gooey::SlotHolder
{
    // The menu items text
    std::string m_text;
    // Parent menu item
    Menu* m_parent;   
    // Flag to indicate if the menu item is enabled
    bool m_isMenuItemEnabled;
    // Position of the menu item (only valid if menu item or menu is visible)
    Gooey::Core::Vector2 m_position;
    // Font
    Gooey::Font::TypeFace* m_font;
    // Slot
    Gooey::Signal0 m_pressed;
    // Time for pulsing
    double m_pulse;

public:
    // Default Constructor
    MenuItem();
    // Constructor
    MenuItem(const std::string& text, MenuEventHandler* ctrl, void (MenuEventHandler::*fkt)());
    // Destructor
    virtual ~MenuItem();

    // Set the menu items text
    void setText(const std::string& menuItemText);
    // Get the menu items text
    const std::string& text() { return m_text; }
    
    // Set the menu items parent
    void setParent(Menu* parent) { m_parent = parent; }
    // Get the menu items parent
    Menu* parent() { return m_parent; }

    // Set position
    void setPosition(const Gooey::Core::Vector2 position);
    // Get position
    const Gooey::Core::Vector2 position() { return m_position; }
    
    // Returns whether the MenuItem is enabled
    virtual bool isEnabled() { return m_isMenuItemEnabled; };
    // Enable menu item
    void enableMenuItem() { m_isMenuItemEnabled = true; };
    // Disable menu item
    void disableMenuItem() { m_isMenuItemEnabled = false; };

    // Perform
    virtual void performAction();

    // Draw menu item
    void drawMenuItem(bool highlighted, double elapsed);
};

#endif // MENUITEM_H
