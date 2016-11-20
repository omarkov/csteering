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
 * file: voxel/octree/fill_oct.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */

#include <octree/fill_oct.h>
#include <iostream>

using namespace std;

#if !defined(CLASSIC_MODE) && defined(FILL_SOLIDS)

// ##### FillOct() ###################################################
FillOct::FillOct(IndexOct idxOct) 
    : IndexOct(idxOct), m_fillPoints(idxOct.getMaxTreeHeight()) 
    
    #ifdef LIMITED_STACK
    
    , m_keptPoint(NULL_NODE_VEC)
    
    #endif // LIMITED_STACK
{
}

// ##### add() #######################################################
void FillOct::add(NodeIndex idx) {
    assert (isIn(idx, idx.getHeight()));
    assert (isLeaf(idx));
    assert (!m_fillPoints.isFull());
  
    if (IndexOct::getColor(idx) == UNDEF_OBJ) {
        setColor(idx, m_fillColor);
        m_fillPoints.put(idx);
    }
}

// ##### fill() ######################################################
void FillOct::fill(OctGen* generator) {
    assert (generator != NULL);

    fillTree(getTree(), NodeIndex(0, 0, 0, getMaxTreeHeight()), generator);
    
    cout << endl;
}

// ##### fill() 
void FillOct::fill() {

    while (!m_fillPoints.isEmpty()) {
        NodeIndex idx = m_fillPoints.get();
        
        assert (isIn(idx, idx.getHeight()));
        assert (isLeaf(idx));
    
        AxIndex max = (1 << (getMaxTreeHeight() - idx.getHeight())) - 1;
        
        for (Axis ax = 0; ax < DIMENSIONS; ax++) {
            AxIndex pos = idx[ax];
            NodeIndex neighbor = idx;
      
            if (pos < max) {
                neighbor.setCoordinate(ax, pos + 1);
                fillParts(neighbor, ax, 0);
            }
            
            if (pos > 0) {
                neighbor.setCoordinate(ax, pos - 1);
                fillParts(neighbor, ax, 1);
            }
        }
    }
}

// ##### fillParts() #################################################
void FillOct::fillParts(NodeIndex myIdx,
    Axis axis, AxIndex partOfs) {
    _octree subtree;
    Height h = getExistNode(myIdx, subtree);
    myIdx.setHeight(h);
    fillParts(subtree, myIdx, axis, partOfs);
}

// ##### fillParts() 
void FillOct::fillParts(_octree subtree, NodeIndex myIdx,
    Axis axis, AxIndex partOfs) {
    
    if (OctStruct::isLeaf(*subtree)) {
        if (!m_fillPoints.isFull()) {
            
            assert (!fillPoints.isFull());
            
            add(myIdx);
    
        } else {
            if (isUndefObj(*subtree)) {
        
                #ifdef MARK_BORDER
        
                OctStruct::setBorderColor(*subtree, m_fillColor);

                #elif defined(LIMITED_STACK)
        
                m_fillFurther = true;
                m_keptPoint = myIdx;
        
                #endif // LIMITED_STACK && !MARK_BORDER
            }
        }
    } else {
        
        for(PartType i = 0; i < OCT_PARTS; i++) {
            if (getPartOfs(i, axis) == partOfs) {
                fillParts(OctStruct::getChild(subtree, i), getChild(myIdx, i), 
                    axis, partOfs);
            }
        }
    }
}

// ##### fillTree() ##################################################
void FillOct::fillTree(_octree subtree, NodeIndex myIdx, OctGen* generator) {
    assert (isIn(myIdx));

    if (OctStruct::isLeaf(*subtree)) {
    
        #ifndef NDEBUG
    
        Color c = OctStruct::getColor(*subtree);
    
        #endif // !NDEBUG
    
        #ifdef MARK_BORDER
    
        if (isBorderNode(*subtree)) {
            assert (c > UNDEF_OBJ && c < NO_OBJECT);
            
            m_fillColor = OctStruct::getBorderColor(*subtree);
            
            assert (fillColor >= 0);
            assert (fillPoints.isEmpty());

            OctStruct::setColor(*subtree, m_fillColor);
            add(myIdx);
            fill();
            return;
        }
    
        #endif // MARK_BORDER

        if (isUndefObj(*subtree)) {
            assert (c == UNDEF_OBJ);
            assert (isIn(myIdx, myIdx.getHeight()));
        
            #ifdef SAFE_FILL
        
            bool safeFill;
            m_fillColor = getColor(generator, myIdx, safeFill);
        
            assert (m_fillColor >= 0);
        
            cerr << ".";
        
            if (!safeFill) {
                OctStruct::setColor(*subtree, m_fillColor);
                return;
            }
        
            #else // !SAFE_FILL
        
            m_fillColor = generator->getColor(myIdx);
            assert (fillColor >= 0);
        
            #endif // !SAFE_FILL

            #ifdef LIMITED_STACK
        
            do {
                m_fillFurther = false;
        
                #endif // LIMITED_STACK
        
                assert (m_fillPoints.isEmpty());
                add(myIdx);
                fill();

                #ifdef LIMITED_STACK
        
                if (m_fillFurther) {
                    assert (fillPoints.isEmpty());
            
                    cerr << ".";
            
                    myIdx = m_keptPoint;
                }
            } while (m_fillFurther);
            
            #endif // LIMITED_STACK

        }
    } else {
        
        for (PartType i = 0; i < OCT_PARTS; i++) {
            fillTree(OctStruct::getChild(subtree, i), getChild(myIdx, i), 
                generator);
        }
    }
}

#ifdef SAFE_FILL
// ##### getColor() ##################################################
Color FillOct::getColor(OctGen* generator, NodeIndex p, bool &safeFill) {
    safeFill = true; 
    Color pColor = generator->getColor(p);

    if (p.getHeight() != BASE_NODE_HEIGHT) {
        return pColor;
    }

    // the node is on the lowest plane, 
    // there is only one neighbour in every direction
  
    AxIndex max = (1 << getMaxTreeHeight()) - 1;
  
    for (Axis i = 0; i < DIMENSIONS; i++) {
        AxIndex pos = p[i];
        NodeIndex neighbor = p;
    
        if (pos < max) {
            neighbor.setCoordinate(i, pos + 1);
            neighbor = getExistNode(neighbor);
            
            assert (isLeaf(neighbor));
            
            Color nColor = IndexOct::getColor(neighbor);
        
            if (nColor == UNDEF_OBJ) {
                nColor = generator->getColor(neighbor); 
            }
        
            if (nColor == NO_OBJECT) {
                safeFill = (pColor == NO_OBJECT);
                return pColor; 
            }
        
            if (nColor != pColor) {
                safeFill = false;
                return pColor;
            }
        }
    
        neighbor = p;
    
        if (pos > 0) {
            neighbor.setCoordinate(i, pos - 1);
            neighbor = getExistNode(neighbor);
        
            assert (isLeaf(neighbor));
        
            Color nColor = IndexOct::getColor(neighbor);
        
            if (nColor == UNDEF_OBJ) {
                nColor = generator->getColor(neighbor); 
            }
        
            if (nColor == NO_OBJECT) {
                safeFill = (pColor == NO_OBJECT);
                return pColor; 
            }
        
            if (nColor != pColor) {
                safeFill = false;
                return pColor;
            }
        }
    }
    
    return pColor;
}
#endif // SAFE_FILL

#endif // !CLASSIC_MODE && FILL_SOLIDS

// EOF: voxel/octree/fill_oct.cpp
