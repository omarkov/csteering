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

#include "math.h"

#include "GUIController.h"
#include "MenuItem.h"
#include "Menu.h"

const double PI=3.14159265f;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

Menu::Menu()
: MenuItem(), m_isMenuHidden(true)
{
}

Menu::Menu(const std::string& text)
: MenuItem(text, NULL, NULL), m_isMenuHidden(true)
{
}

Menu::~Menu()
{
    // delete all child menu items
    this->clearMenuItems();
}


// ---------------------------------------------------------------------------
// Menu item management
// ---------------------------------------------------------------------------

void Menu::addMenuItem(MenuItem* item)
{
    // we need a valid item
    if (!item)
        return;

    m_menuItems.push_back(item);
    item->setParent(this);
}

void Menu::addMenuItem(MenuItem* item, unsigned int index)
{
    // we need a valid item
    if (!item)
        return;

    // go through the whole list while counting down the index value
    std::list<MenuItem*>::iterator it;
    unsigned int i;

    for (it = m_menuItems.begin(), i = 0; it != m_menuItems.end(); ++it, ++i) {
        if (i == index) {
            m_menuItems.insert(it, item);
            return;
        }
    }

    item->setParent(this);
}

void Menu::removeMenuItem(MenuItem* item)
{
    // we need a valid item
    if (!item)
        return;
    
    // go through all items
    std::list<MenuItem*>::iterator it;

    for (it = m_menuItems.begin(); it != m_menuItems.end(); ++it) {
        if ((*it) == item) {
            delete item;
            m_menuItems.erase(it);
            return;
        }
    }
    
    item->setParent(NULL);
}

void Menu::removeMenuItem(unsigned int index)
{
    // FIXME: we're going through the list twice, but I'm lazy...
    removeMenuItem(menuItem(index));
}

MenuItem* Menu::menuItem(unsigned int index)
{
    // do some bounds-checking
    if (index > numMenuItems())
        return NULL;

    // go through the whole list while counting down the index value
    std::list<MenuItem*>::iterator it;
    unsigned int i;
    
    for (it = m_menuItems.begin(), i = 0; it != m_menuItems.end(); ++it, ++i)
        if (i == index)
            return (*it);

    return NULL;
}

void Menu::clearMenuItems()
{
    // delete all menu items
    std::list<MenuItem*>::iterator it;

    for (it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
        if ((*it))
            delete (*it);

    // reset the container
    m_menuItems.clear();
}


// ---------------------------------------------------------------------------
// Show / Hide menu
// ---------------------------------------------------------------------------


void Menu::showMenu(Gooey::Core::Vector2 position)
{
    if (numMenuItems()==0)
    {
        std::cout<<"Warning: Trying to show empty menu."<<std::endl;
        return;
    }

    if (!m_isMenuHidden)
        return;
    // there is only one visible menu possible
    if (g_GUIController->activeMenu())
        return;

    g_GUIController->setActiveMenu(this);
    this->setPosition(position);
    m_highlightedMenuItem = NULL;

    // show all menu items in a circle around position
    std::list<MenuItem*>::iterator it;
    unsigned int pos = 0;
    unsigned int max = this->numMenuItems();
    unsigned int radiusX = 100 + max*25;
    unsigned int radiusY = 30 + max*15;
    Gooey::Core::Vector2 itemPosition;
    for (it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
        if ((*it))
            {
            itemPosition  = Gooey::Core::Vector2((unsigned int)(position.x()+radiusX*sin(2*PI*pos/max)),
                                                 (unsigned int)(position.y()-radiusY*cos(2*PI*pos/max)));
            (*it)->setPosition(itemPosition);
            pos++;
            }

    m_isMenuHidden = false;
}

void Menu::hideMenu()
{
    if (m_isMenuHidden)
        return;

    g_GUIController->setActiveMenu(NULL);

    m_isMenuHidden = true;
}

bool Menu::isEnabled()
{
    // a menu is enabled if any child (menu or menuItem) is enabled
    bool enabled = false;
    std::list<MenuItem*>::iterator it;
    for (it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
        if ((*it))
            {
            enabled = enabled || ((*it)->isEnabled());
            }
    return enabled;
}

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------


void Menu::performMenuAction()
{
    if (numMenuItems()==0)
    {
        return;
    }

    if (m_highlightedMenuItem)
        m_highlightedMenuItem->performAction();
}

void Menu::performAction()
{
    this->parent()->hideMenu();
    this->showMenu(this->parent()->position());
}

void Menu::performHighlight(Gooey::Core::Vector2 position)
{
    if (numMenuItems()==0)
    {
        return;
    }

    float x = position.x() - this->position().x();
    float y = this->position().y()- position.y();
    
    if (y == 0.0f)
        y = 0.00001f;
    float angle = 0.0f;
    if ((x >= 0.0f) && (y > 0.0f))
        angle = atan(x/y);
    else if ((x >= 0.0f) && (y < 0.0f))
        angle = PI + atan(x/y);
    else if ((x <= 0.0f) && (y < 0.0f))
        angle = PI + atan(x/y);
    else if ((x <= 0.0f) && (y > 0.0f))
        angle = 2*PI + atan(x/y);

    angle = angle + PI/this->numMenuItems();
    unsigned int number = (unsigned int)(this->numMenuItems() * angle/(2*PI));
    if (number >= this->numMenuItems())
        number = 0;

    if (menuItem(number)->isEnabled())
        m_highlightedMenuItem = menuItem(number);
    else
        m_highlightedMenuItem = NULL;
}
    
void Menu::drawMenu(double elapsed)
{
    if (numMenuItems()==0)
    {
        return;
    }

    if (m_isMenuHidden)
        return;

    std::list<MenuItem*>::iterator it;
    for (it = m_menuItems.begin(); it != m_menuItems.end(); ++it)
        if ((*it))
            {
                (*it)->drawMenuItem(((*it) == m_highlightedMenuItem), elapsed);
            }
}
