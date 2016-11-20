// -*- Mode: C++; indent-tabs-mode: nil -*-
//
// Copyright (c) 2004, Mikhail Prokharau, StuPro A - CS 
//  <csteering-devel@duck.informatik.uni-stuttgart.de>
//  All rights reserved.
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

/**
 * file: voxel/writer/xpm_writer.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <writer/xpm_writer.h>
#include <filename.h>

using namespace std;

/**
 * sign for an object voxel
 */
#define IS_OBJ_COLOR_CHAR '*'

/**
 * sign for a voxel that belongs to no object
 */
#define NO_OBJ_COLOR_CHAR ' '

// ##### XpmWriter() #################################################
XpmWriter::XpmWriter(const char* fileName)
    : m_extension(getExtension(fileName)), m_fileName(getPurName(fileName)) {
}

// ##### ~XpmWriter() ################################################
XpmWriter::~XpmWriter() {
    if (m_out.is_open()) {
        m_out.close();
    }
}

// ##### getColorChar() ##############################################
char XpmWriter::getColorChar(AxIndex h, AxIndex i[2]) {
    assert (DIMENSIONS == 3);
    assert (0 <= sectAxis && sectAxis < DIMENSIONS);

    AxIndex coord[3];
    int iAxis = 0;
    
    for (Axis axis = 0; axis < 3; axis++) {
        
        if (axis == m_sectAxis) {
            coord[axis] = h;
        } else {
            coord[axis] = i[iAxis];
            i++;
        }
    }

    NodeIndex p = NodeIndex(coord[0], coord[1], coord[2], BASE_NODE_HEIGHT);
    p = m_octree->getExistNode(p);
    Color color = m_octree->getColor(p);
    assert (color > UNDEF_OBJ);
    return (color == NO_OBJECT ? NO_OBJ_COLOR_CHAR : IS_OBJ_COLOR_CHAR);
}

// ##### getFileName() ###############################################
const char* XpmWriter::getFileName(int number) {
    string pre = m_fileName;
    char buf[31];
    char axNum[2];
    axNum[0] = (char)(m_sectAxis + 'x');
    axNum[1] = (char)0;
    sprintf(buf, "_%s%.3d.", axNum, number);
    
    return strdup((pre + buf + string(m_extension)).c_str());
}

// ##### getFormatName() #############################################
const char* XpmWriter::getFormatName() {
    return "XPM";
}

// ##### getFormatType() #############################################
const Writer::FormatType XpmWriter::getFormatType() {
    return XPM;
}

// ##### getWidth() ##################################################
AxIndex XpmWriter::getWidth() {
    return 1 << m_octree->getMaxTreeHeight();
}

// ##### write() #####################################################
void XpmWriter::write(char colorChar) {
    m_out << colorChar;
}

// ##### writeFooter() ###############################################
void XpmWriter::writeFooter() {
    m_out << "};" << endl;
}

// ##### writeHeader() ###############################################
void XpmWriter::writeHeader(int number) {
    m_out << "/* XPM */" << endl;
    m_out << "static char *cad2octree_XpmWriterFile_" 
        << (char)(m_sectAxis + 'x') << number << "[]= {" << endl;
    m_out << '"' << getWidth() << " " << getWidth() << " 2 1" << '"' << ","
        << endl;
    m_out << "/* columns rows colors chars-per-pixel */" << endl;
    m_out << '"' << NO_OBJ_COLOR_CHAR << " m white c white " << '"' << "," 
        << endl;
    m_out << '"' << IS_OBJ_COLOR_CHAR << " m black c black " << '"' << "," 
        << endl;
    m_out << "/* pixels */" << endl;
}

// ##### writeLineBegin() ############################################
void XpmWriter::writeLineBegin() {
    m_out << '"';
}

// ##### writeLineEnd() ##############################################
void XpmWriter::writeLineEnd(bool isLast) {
    m_out << '"';
  
    if (!isLast) {
        m_out << "," << endl;
    }
}

// ##### writeFile() #################################################
void XpmWriter::writeFile(IndexOct* tree) throw (WriteFileException*) {
    assert (DIMENSIONS == 3);
    assert (tree != NULL);

    m_octree = tree;

    for (m_sectAxis = 0; m_sectAxis < DIMENSIONS; m_sectAxis++) {
        cout << "cut along the " << (char)(m_sectAxis + 'x') << "-axis" 
            << endl;

        int i[2];

        for (int h = 0; h < getWidth(); h++) {
            m_out.open(getFileName(h));
            writeHeader(h);
            
            for (i[1] = 0; i[1] < getWidth(); i[1]++) {
                writeLineBegin();
                
                for (i[0] = 0; i[0] < getWidth(); i[0]++) {
                    write(getColorChar(h, i));
                }
        
                writeLineEnd((i[1] + 1) == getWidth());
            }
      
            writeFooter();
            m_out.flush();
            m_out.close();
        
            cout << "progress " << (100 * h) / (getWidth() - 1) << "%" 
                << char(13); 
        }
    }
}

// EOF: voxel/writer/xpm_writer.cpp
