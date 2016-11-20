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
 * file: voxel/raw2voxelvs.cpp
 *
 * @author Mikhail Prokharau
 * @date 2004
 */


#include <iostream>
#include <fstream>

#include <reader/reader.h>
#include <generator/oct_gen.h>
#include <writer/writer.h>
#include <filename.h>

#include <voxelization/voxelization.h>

using namespace std;

/**
 * default maximal tree depth
 */
#define STD_MAX_DEPTH 6
    
/**
 * writes help message to the console
 */
int usage(const char* progname) {
    cout << progname << " [-q] [-s voxel-size] input-file -o output-file" << endl;
    cout << "-s voxel-size    size of the voxels to be generated" << endl;
    //cout << "-d depth         maximum depth of the octree to be generated" << endl;
    //cout << "                 values between 2 and " << MAX_HEIGHT 
    //     << " are accepted"<< endl;
    //cout << "                 the default depth is " << STD_MAX_DEPTH << endl;
    cout << "input-file       raw-file to read." << endl;
    cout << "-o output-file   vxl-file to write." << endl;
    
    return 1;
}

/**
 * @param inFile file to be read
 * @param outFile filr to be written
 * @param voxelSize voxel size
 * @return \em -1, if something went wrong
 *         \em  0, otherwise
 */
int convert(const char* inFile, const char* outFile, 
    double voxelSize)
        throw() {
    
    int error = 0;
  
    try {
        
        Voxelization* v = new Voxelization(inFile, voxelSize);
        Voxels* voxel = v->getVoxels();
        
        ofstream ofs(outFile);
      
        if (ofs == NULL) {
            cerr << "Failed to open file for writing" << endl;
            exit(1);
        }
        
        ofs << "--------------------------- Section x ---------------------------" 
		    << endl << endl;
        
        for (unsigned int x = 0; x < voxel->size(); x++) {
        
            ofs << "############################# x: " << x 
                << " #############################" << endl;
            
            for (unsigned int y = 0; y < (*voxel)[x].size(); y++) {
        
                for (unsigned int z = 0; z < (*voxel)[x][y].size(); z++) {
        
                    Point p = v->getPoint(x, y, z);
                    GeomPoint gp = v->getGeomPoint(p);
                    NodeIndex ni = v->getNodeIndex(p.getX(), p.getY(), 
                    	p.getZ());
                                            
                    if ((*voxel)[x][y][z] != 0) {
                        ofs << "*";
                    } else {
                        ofs << " ";
                    }
                }
                
                ofs << endl;
            }
        }

        ofs << "--------------------------- Section y ---------------------------" 
		    << endl << endl;

        for (unsigned int y = 0; y < (*voxel)[0].size(); y++) {
            
            ofs << "############################# y: " << y 
                << " #############################" << endl;
            
            for (unsigned int x = 0; x < voxel->size(); x++) {
        	
        	    for (unsigned int z = 0; z < (*voxel)[x][0].size(); z++) {
        
                    Point p = v->getPoint(x, y, z);
                    GeomPoint gp = v->getGeomPoint(p);
                    NodeIndex ni = v->getNodeIndex(p.getX(), p.getY(), 
                    	p.getZ());
                    
                    if ((*voxel)[x][y][z] != 0) {
                        ofs << "*";
                    } else {
                        ofs << " ";
                    }
                }
                
                ofs << endl;
            }
        }
        
        ofs << "--------------------------- Section z ---------------------------" 
		    << endl << endl;        
        
        for (unsigned int z = 0; z < (*voxel)[0][0].size(); z++) {
            
            ofs << "############################# z: " << z 
                << " #############################" << endl;
            
            for (unsigned int x = 0; x < voxel->size(); x++) {        	
        	    
        	    for (unsigned int y = 0; y < (*voxel)[x].size(); y++) {        
        
                    Point p = v->getPoint(x, y, z);
                    GeomPoint gp = v->getGeomPoint(p);
                    NodeIndex ni = v->getNodeIndex(p.getX(), p.getY(), 
                    	p.getZ());
                    
                    if ((*voxel)[x][y][z] != 0) {
                        ofs << "*";
                    } else {
                        ofs << " ";
                    }
                }
                
                ofs << endl;
            }
        }
        
        delete v;
        
    } catch (ModelExc &e) {
		cerr << "Model exception, update failed" << endl;
	} catch (Exception* e) {
        if (e == NULL) {
            cerr << "Oops... Something went wrong :-(" << endl;
            return -1;
        }
        
        cerr << e->getMsg() << endl;
        error = -1;
    } catch (...) {
        cerr << "Oops... Something went wrong :-(" << endl;
        error= -1;
    }
    
    return error;
}

/**
 * main function
 * @return -1, if there was an error\n
 *          1, if the help message was shown\n
 *          0, otherwise
 */
int main(int argc, char *argv[]) {
    const char* inFile= NULL;
    const char* outFile= NULL;
    double voxelSize = 0.5;

    for (int i= 1; i < argc; ++i) {

        if (argv[i][0] != '-') {

            if (inFile != NULL) {
                return usage(argv[0]);
            }
        
            inFile = argv[i];
        } else {
            switch (argv[i][1]) {
            
            case 'o':
                i++;

                if (i >= argc || outFile != NULL) {
                    return usage(argv[0]);
                }
        
                outFile = argv[i];
                break;
      
            case 's':
                i++;

                if (i >= argc) {
                    return usage(argv[0]);
                }
    
                voxelSize = atof(argv[i]);
                break;
         
            default:        
                return usage(argv[0]);
            }
        }
    }

    if (inFile == NULL ) {
        return usage(argv[0]);
    }

    if (outFile == NULL) {
        string outName = getPurName(inFile);
        outName += ".vxl";
        outFile = outName.c_str();
    }

    return convert(inFile, outFile, voxelSize);
}

// EOF: voxel/raw2voxelvs.cpp
