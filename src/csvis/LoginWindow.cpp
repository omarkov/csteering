// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Clemens Spenrath <clemens-mail@gmx.de> 
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

#include "Application.h"

#include "CommonServer.h"
#include "FANClasses.h"
#include "Server.h"
#include "common.h"

#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/Panel.h"
#include "glgooey/Button.h"
#include "glgooey/StaticText.h"
#include "glgooey/EditField.h"
#include "glgooey/ComboBox.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "LoginWindow.h"

using namespace Gooey;
using namespace Gooey::Core;

LoginWindow* g_LoginWindow = NULL;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

LoginWindow::LoginWindow()
{
    g_LoginWindow = this;
    asprintf(&m_port, "%d", modelPort);

    this->setSize(Vector2(500, 270));
    this->setText("Login");

    // Create objects
    m_greetingText = new StaticText(NULL, "Welcome to STORM");
    m_loginText = new StaticText(NULL, "Please login to a simulation server");
    m_serverText = new StaticText(NULL, "Server:");
    m_serverEdit = new EditField(NULL, modelHost);
    m_portText = new StaticText(NULL, "Port:");
    m_portEdit = new EditField(NULL, m_port);
    m_cancelButton = new Gooey::Button(NULL, "Cancel");
    m_loginButton = new Gooey::Button(NULL, "Login");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_cancelButton->pressed.connect(this, &LoginWindow::cancelButtonPressed);
    m_loginButton->pressed.connect(this, &LoginWindow::loginButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(100.0f);
    widths.push_back(0.0f);
    widths.push_back(80.0f);
    widths.push_back(100.0f);    
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(20.0f); // space
    heights.push_back(30.0f);
    heights.push_back(20.0f); // space
    heights.push_back(30.0f);
    heights.push_back(10.0f); //space
    heights.push_back(40.0f);
    heights.push_back(10.0f); // space
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 4; info.verticalSpan = 1;
    m_panel->addChildWindow(m_greetingText, info);
    info.columnIndex = 0; info.rowIndex = 3; info.horizontalSpan = 4; info.verticalSpan = 1;
    m_panel->addChildWindow(m_loginText, info);
    info.columnIndex = 0; info.rowIndex = 5; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_serverText, info);
    info.columnIndex = 1; info.rowIndex = 5; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_serverEdit, info);
    info.columnIndex = 2; info.rowIndex = 5; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_portText, info);
    info.columnIndex = 3; info.rowIndex = 5; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_portEdit, info);
    info.columnIndex = 0; info.rowIndex = 7; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cancelButton, info);
    info.columnIndex = 1; info.rowIndex = 7; info.horizontalSpan = 3; info.verticalSpan = 1;
    m_panel->addChildWindow(m_loginButton, info);
}

LoginWindow::~LoginWindow()
{
    g_LoginWindow = NULL;

    free(m_port);
    delete m_loginButton;
    delete m_cancelButton;
    delete m_portEdit;
    delete m_portText;
    delete m_serverEdit;
    delete m_serverText;
    delete m_loginText;
    delete m_greetingText;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void LoginWindow::showWindow()
{
    this->showWindow(getScreenCenter());
}

void LoginWindow::showWindow(Vector2 position)
{
    if (!(this->isHidden()))
        return;

    // window-specific stuff
    // ..

    StandardWindow::showWindow(position);
}

void LoginWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();

    // window-specific stuff
    // ..
}

void LoginWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_loginButton->enable();
    m_cancelButton->enable();
    m_portEdit->enable();
    m_portText->enable();
    m_serverEdit->enable();
    m_serverText->enable();
    m_loginText->enable();
    m_greetingText->enable();
    
    StandardWindow::enableWindow();
}

void LoginWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    // FIXME: dirty workaround for an unsolved problem:
    // When the MessageWindow is opened in a sigslot-button-function, the Window can't be controlled while the button is disabled.
    // So we just don't disable the button. Problem: The User can click again while the MessageWindow is displayed.
    // Therefore we check this on Login-Button-Click.
    //m_loginButton->disable();
    m_cancelButton->disable();
    m_portEdit->disable();
    m_portText->disable();
    m_serverEdit->disable();
    m_serverText->disable();
    m_loginText->disable();
    m_greetingText->disable();
}

// ---------------------------------------------------------------------------
// Events connected to the Gooey-Items
// ---------------------------------------------------------------------------

void LoginWindow::cancelButtonPressed()
{
    this->hideWindow();
    g_Application->quit(0);
}

void LoginWindow::loginButtonPressed()
{
    if (!this->isEnabled())
        return; // see LoginWindow::disableWindow() for explanation why we need this

    char *host = g_strdup(m_serverEdit->text().c_str());
    char *port = g_strdup(m_portEdit->text().c_str());

    if(host != NULL && port != NULL && strlen(host) != 0 && strlen(port) != 0)
    {
	    if(modelHost != NULL)free(modelHost);
	    modelHost = host;
	    modelPort = atoi(port);
	if(modelConn != NULL) 
	{
            modelConn->rpc("model::logout");
	    delete modelConn;
	    modelConn = NULL;
	}
        modelConn = getModelConn();
        if(modelConn != NULL && 
           modelConn->rpc("model::login", 2, g_fan->config->getValue("VISHOST"), g_fan->config->getValue("VISPORT"))
	)
        {

            this->hideWindow();
        }else
        {
            error("Can't connect to modelserver (%s:%s).", host, port);
        }
    }else
    {
       error("Please insert server and port.");
    }
}
