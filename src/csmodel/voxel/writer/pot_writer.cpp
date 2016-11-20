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
 * file: voxel/writer/pot_writer.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <writer/pot_writer.h>

#define GROUND_DEPTH -1

// ##### PotWriter() #################################################
PotWriter::PotWriter(const char* fileName) : m_fileName(fileName) {
    m_wBinary = 0;
    m_wCount = 0;
}

// ##### ~PotWriter() ################################################
PotWriter::~PotWriter() {
    clear();
}

// ##### binWrite() ##################################################
void PotWriter::binWrite(Data data) {
    if (m_wCount >= 8 * getCacheSize()) {
        flushWrite();
    }
  
    m_wBinary = m_wBinary << 1;
    m_wBinary += data;
    m_wCount++;
}

// ##### flushWrite() ################################################
void PotWriter::flushWrite() {
    
    while (m_wCount < 8 * getCacheSize()) {
        m_wBinary = m_wBinary << 1;
        m_wCount++;
    }
  
    fwrite(&m_wBinary, getCacheSize(), 1, m_potFile);
    m_wBinary = 0;
    m_wCount = 0;
}

// ##### getCachSize() ###############################################
const size_t PotWriter::getCacheSize() {
    return sizeof(Data);
}

// ##### getFormatName() #############################################
const char* PotWriter::getFormatName() {
    return "POT";
}

// ##### getFormatType() #############################################
const Writer::FormatType PotWriter::getFormatType() {
    return POT;
}

// ##### writeFile() #################################################
void PotWriter::writeFile(IndexOct* tree)
    throw (WriteFileException*) {
    
    if (tree == NULL) {
        return;
    }
    
    setTree(tree->getTree());
    assert (getTree() != NULL);

    m_potFile = fopen(m_fileName, "w"); 
    
    if (m_potFile == NULL) {
        throw new WriteFileException(m_fileName);
    }

    fprintf(m_potFile, "%d\n", tree->getMaxTreeHeight());
    writeTree(*getTree());
    flushWrite();
    fprintf(m_potFile, "\n");

    fclose(m_potFile);
}

// ##### writeInnerNode() ############################################
void PotWriter::writeInnerNode() {
    binWrite(INNER_NODE);
}

// ##### writeLeaf() #################################################
void PotWriter::writeLeaf(Color color) {
    assert (color >= NO_OBJECT);
  
    binWrite(LEAF);
    binWrite( (color == NO_OBJECT ? 0 : 1) );
}

// ##### writeTree() #################################################
void PotWriter::writeTree(Node node) {
    
    if (isLeaf(node)) {
        writeLeaf(node.flag);
        return;
    }
  
    writeInnerNode();
    
    for (PartType i = 0; i < OCT_PARTS; i++) {
        writeTree(node.parts[i]);
    }
}

// EOF: voxel/writer/pot_writer.cpp
