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

#ifndef MENU_H
#define MENU_H

#include <list>
#include <string>

#include "MenuItem.h"
#include "glgooey/core/Vector2.h"

class MenuItem;

class Menu : public MenuItem
{
    std::list<MenuItem*> m_menuItems;
    bool m_isMenuHidden;
    MenuItem* m_highlightedMenuItem;
    
public:
    // Default Constructor
    Menu();
    // Constructor
    Menu(const std::string& text);
    // Destructor
    ~Menu();

    // Add an menu item at the end of the menu
    void addMenuItem(MenuItem* item);
    // Add an menu item at a specified index
    void addMenuItem(MenuItem* item, unsigned int index);
    // Remove a menu item by pointer
    void removeMenuItem(MenuItem* item);
    // Remove a menu item by index
    void removeMenuItem(unsigned int index);
    // Get a menu item at index
    MenuItem* menuItem(unsigned int index);
    // clear all menu items
    void clearMenuItems();
    // get the number of menu items
    unsigned int numMenuItems() { return m_menuItems.size(); }
    
    // Show menu
    void showMenu(Gooey::Core::Vector2 position);
    // Hide menu
    void hideMenu();

    // Returns whether the Menu is enabled
    bool isEnabled();

    // Perform
    void performMenuAction();
    void performAction();
    void performHighlight(Gooey::Core::Vector2 position);
    
    // Draw menu
    void drawMenu(double elapsed);
    
};

#endif // MENU_H
