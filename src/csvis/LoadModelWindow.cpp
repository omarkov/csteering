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

#include "GUIController.h"
#include "CommonServer.h"
#include "FANClasses.h"
#include <dirent.h>

#include "glgooey/ComplexGridLayouter.h"
#include "glgooey/Panel.h"
#include "glgooey/Button.h"
#include "glgooey/ListBox.h"
#include "glgooey/EditField.h"

#include "glgooey/core/Vector2.h"
#include "glgooey/core/Rectangle.h"

#include "LoadModelWindow.h"
#include "Application.h"
#include "TriangulatedModel.h"

using namespace Gooey;
using namespace Gooey::Core;

LoadModelWindow* g_LoadModelWindow = NULL;

// ---------------------------------------------------------------------------
// Construction/Destruction
// ---------------------------------------------------------------------------

LoadModelWindow::LoadModelWindow()
{
    g_LoadModelWindow = this;

    this->setSize(Vector2(600, 400));
    this->setText("<load/save model>");

    // Create objects
    m_modelBox = new ListBox(NULL, Rectangle(10, 10, 20, 20));
    m_fileEdit = new EditField(NULL, "");
    m_incrementButton = new Gooey::Button(NULL, "Increment");
    m_cancelButton = new Gooey::Button(NULL, "Cancel");
    m_loadSaveButton = new Gooey::Button(NULL, "<load/save>");

    // create the layout object and tell the panel to use it
    m_layouter = new ComplexGridLayouter;
    m_panel->setLayouter(m_layouter);

    // Connect
    m_modelBox->selectionChanged.connect(this, &LoadModelWindow::modelBoxSelectionChanged);
    m_incrementButton->pressed.connect(this, &LoadModelWindow::incrementButtonPressed);
    m_cancelButton->pressed.connect(this, &LoadModelWindow::cancelButtonPressed);
    m_loadSaveButton->pressed.connect(this, &LoadModelWindow::loadSaveButtonPressed);

    //  choose the widths of the columns
    std::vector<float> widths;
    widths.push_back(270.0f);
    widths.push_back(140.0f);
    widths.push_back(0.0f);
    m_layouter->setColumnWidths(widths);

    // set the row heights
    std::vector<float> heights;
    heights.push_back(0.0f);
    heights.push_back(40.0f);
    heights.push_back(40.0f);
    m_layouter->setRowHeights(heights);

    // create a ComplexGridCellInfo instance
    ComplexGridCellInfo info;

    // put everything in the panel
    info.columnIndex = 0; info.rowIndex = 0; info.horizontalSpan = 3; info.verticalSpan = 1;
    m_panel->addChildWindow(m_modelBox, info);
    info.columnIndex = 0; info.rowIndex = 1; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_fileEdit, info);
    info.columnIndex = 2; info.rowIndex = 1; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_incrementButton, info);
    info.columnIndex = 0; info.rowIndex = 2; info.horizontalSpan = 1; info.verticalSpan = 1;
    m_panel->addChildWindow(m_cancelButton, info);
    info.columnIndex = 1; info.rowIndex = 2; info.horizontalSpan = 2; info.verticalSpan = 1;
    m_panel->addChildWindow(m_loadSaveButton, info);
}

LoadModelWindow::~LoadModelWindow()
{
    g_LoadModelWindow = NULL;

    delete m_loadSaveButton;
    delete m_cancelButton;
    delete m_modelBox;
}

// ---------------------------------------------------------------------------
// Window-Content
// ---------------------------------------------------------------------------

void LoadModelWindow::showWindow()
{
    this->showWindow(false);
}

void LoadModelWindow::showWindow(Vector2 position)
{
    this->showWindow(position, false);
}

void LoadModelWindow::showWindow(bool save)
{
    this->showWindow(getScreenCenter(), save);
}

void LoadModelWindow::showWindow(Vector2 position, bool save)
{
    if (!(this->isHidden()))
        return;

    m_save = save;
    if (save)
    {
        this->setText("Save Model");
        m_loadSaveButton->setText("Save Model");
        m_fileEdit->show();
        m_incrementButton->show();
    } else
    {
        this->setText("Load Model");
        m_loadSaveButton->setText("Load Model");
        m_fileEdit->hide();
        m_incrementButton->hide();
    }
    
    m_modelBox->removeAllStrings();

    if(modelConn != NULL && modelConn->rpc("model::getModelList"))
    {
       int count = atoi(modelConn->returnHash->getValue("count", 0));
       for(int i=0; i < count; i++)
       {
	   char *idx;
           asprintf(&idx, "file%d", i);
           m_modelBox->addString(modelConn->returnHash->getValue(idx));
	   free(idx);
       }
    }else
    {
	    /*
       DIR * dir_p;
       struct dirent * dir_entry_p;

       dir_p = opendir(g_Application->dataPath("test_models").c_str());
       while( NULL != (dir_entry_p = readdir(dir_p)))
       {
          char *name = dir_entry_p->d_name;
          if(strstr(name, ".raw") != NULL)
          {
             m_modelBox->addString(name);
          }
       }
       closedir(dir_p);
       */
       FAN_postMessage(masterCom, "login", NULL, NULL);
    }

    StandardWindow::showWindow(position);
}

void LoadModelWindow::hideWindow()
{
    if (this->isHidden())
        return;

    StandardWindow::hideWindow();

    // window-specific stuff
    // ..
}

void LoadModelWindow::enableWindow()
{
    if (!(this->isDisabled()))
        return;

    m_loadSaveButton->enable();
    m_cancelButton->enable();
    m_incrementButton->enable();
    m_fileEdit->enable();
    m_modelBox->enable();
    
    StandardWindow::enableWindow();
}

void LoadModelWindow::disableWindow()
{
    if (this->isDisabled())
        return;

    StandardWindow::disableWindow();

    m_loadSaveButton->disable();
    m_cancelButton->disable();
    m_incrementButton->disable();
    m_fileEdit->disable();
    m_modelBox->disable();
}

//////////////////////////////////////////////////////////
// Events connected to the Gooey-Items
//////////////////////////////////////////////////////////

void LoadModelWindow::modelBoxSelectionChanged(int index)
{
    m_fileEdit->setText(m_modelBox->stringAt(index));
}

void LoadModelWindow::incrementButtonPressed()
{
    unsigned int versionNumber = 0;
    
    std::string fileName;
    fileName = m_fileEdit->text();
    unsigned int dot = fileName.rfind(".");
    unsigned int bracket_open = fileName.rfind("(");
    unsigned int bracket_close = fileName.rfind(")");
    
    if ((bracket_close == dot-1) && (bracket_close > bracket_open+1))
    {
        std::string numberText = fileName.substr(bracket_open+1, bracket_close-bracket_open-1);
        versionNumber = atoi(numberText.c_str());
        // remove old number (with brackets)
        fileName.erase(bracket_open, bracket_close-bracket_open+1);
    }
    dot = fileName.rfind(".");    

    versionNumber++;
    
    char text[10];
    sprintf(text, "%i", versionNumber);
    fileName.insert(dot, "()");
    fileName.insert(dot+1, text);
    m_fileEdit->setText(fileName);
}
   
void LoadModelWindow::cancelButtonPressed()
{
    this->hideWindow();
}

void LoadModelWindow::loadSaveButtonPressed()
{
    if (m_save)
    {
        if(modelConn != NULL)
        {
            if(modelConn->rpc("model::saveAs", 1, m_fileEdit->text().c_str()))
            {
        	 // ...
            }
	} else
	{
            FAN_postMessage(masterCom, "login", NULL, NULL);
	}
    } else
    {
        if(m_modelBox->selectedIndex() >= 0)
        {
            if(modelConn != NULL)
            {
                if(modelConn->rpc("model::loadFile", 1, m_modelBox->stringAt(m_modelBox->selectedIndex()).c_str()))
                {
        	 // ...
                }
            }else
            {
               FAN_postMessage(masterCom, "login", NULL, NULL);
	     /*
               g_Application->clearScene();
    
               // load a model
               osg::ref_ptr<TriangulatedModel> model = new TriangulatedModel();
               model->readFromFile(g_Application->dataPath("test_models/" + m_modelBox->stringAt(m_modelBox->selectedIndex())).c_str());
               model->setName(m_modelBox->stringAt(m_modelBox->selectedIndex()));
               g_Application->loadModel(model);
    
               g_Application->activateScene();
    
        // ....
 	 */
            }
        }
    }
    
    this->hideWindow();
}
