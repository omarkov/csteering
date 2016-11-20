#ifndef CSMODELTRIANGULATION_GUARD
#define CSMODELTRIANGULATION_GUARD
/***************************************************************************
 *   Copyright (C) 2004 by Jens Nausedat, Benjamin Jung                    *                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepMesh.hxx>
#include <Poly_Triangulation.hxx>
#include <STEPControl_Reader.hxx>
#include <vector>
#include <fstream>
#include <sstream>
#include "RemoteInterface.h"

/*! \class CSModelOCCTriangulation
 *  \brief This class offers a triangulation of cad-models for visualisation
      and simulation purposes. Additionally simple modification to the cad-model
      are supported by this class.
    \autor Jens Nausedat
    \author Benjamin Jung
    \version 0.1
    \date 16.10.2004
    \remarks This class makes extensive use of the Opencascade CAD-Kernel 5.2,
      for further information have a look at "http://www.opencascade.org".
*/
class CSModelOCCTriangulation {
public:
	/*! constructor */
	CSModelOCCTriangulation ();
	/*! destructor */
	~CSModelOCCTriangulation ();

	/*! 
	\brief set the cad file
	\param filename absolute path to cad-file
	\param type type of the cad-file (step, stl, ...)
	\return true if no error occured
	\return false otherwise
	*/
	bool setCADFile(string filename, CADFILETYPE type);

	/*!
	\brief set the deflection for the triangulation
	\param dbDefl deflection, must be positive!
	\return true if value was set successfully (not out of range...)
	\return false otherwise
	*/
	bool setDeflection(double dbDefl);

	/*!
	\brief get the deflection for the triangulation
	\return value for the deflection
	*/
	double getDeflection();

	/*!
	\brief starts the calculation of the triangulation
	\return true if no error occured
	\return false otherwise
	*/
	bool startTriangulation();

	/*!
	\brief return the triangulation of ALL faces
	\param iFaceCount will contain number of faces int the triangulation
	\return Array containing the complete triangulation for every face of the model
	*/
	FACE_TRGL* getTriangulation(int& iFaceCount);
	FACE_TRGL* getTriangulatedFaces(int &iFaceCount);

	/*!
	\brief calculate the triangulation of a face
	\param iFaceID id of the face
	\param triangulation will contain the triangulation
	\return true if no error occured, otherwise false
	*/
	bool getTriangulationOfFace(int iFaceID, FACE_TRGL& triangulation);

	/*!
	\brief return the number of faces in the model
	\return number of faces in the model
	\remarks this function is needed to use the data returned by getTriangulation
	*/
	int getNumberOfFaces();

	/*!
	\brief Change the position of a control point (pole)
	\param iFaceID id of the face to change the pole for
	\param uPos uPosition of the pole in the net
	\param vPos vPosition of the pole in the net
	\param newPolePos position of the new pole
	\return true, if no error occured
	\return false, otherwise
	*/
	bool changePole(int iFaceID, int uPos, int vPos, CSPOLE newPolePos);

	bool changePoles(int iFaceID, CSPOLE *newPolePos);

	/*!
	\brief save the (probably changed) data in a cad file
	\param filename  the file name in filename will be used,
	\param type type of the cad file (STEP, IGES, STL, ...)
	\return true, if no error occured
	\return false, otherwise
	*/
	bool storeCADFile(string filename, CADFILETYPE type);


private:
	FACE_TRGL *m_faces;			/*!< Array containing all triangulations etc. of all faces */
	double m_dbDeflection;		/*!< Deflection for the triangulation */
	int m_iFaceCount;			/*!< number of faces in the cad-model */
	TopoDS_Shape m_shapeRoot;	/*!< root of an OCC-Object tree */


	/*!
	\brief Cleanup the data of the last triangulation
	\param filename absolute path to a cad-file
	\param type type of the cad-file (STEP, IGES ,...)
	\return true if no error occured
	\return false otherwise
	\remarks needed for performing several triangulations one after another
	*/
	void cleanupTriangulation();

	/*!
	\brief reads a cad-file in OCC
	\param filename absolute path to a cad-file
	\param type type of the cad-file (STEP, IGES ,...)
	\return true if no error occured
	\return false otherwise
	*/
	bool readCADFile(string filename, CADFILETYPE type);

	/*! 
	\brief read the STEP-File in the OCC-tree
	\param filename absolute path to a step-file
	\return true if no error occured
	\return false otherwise
	*/
	bool readSTEPFile(string filename);

	/*! 
	\brief write the OCC tree in the step-file
	\param filename absolute path to a step-file
	\return true if no error occured
	\return false otherwise
	*/
	bool writeSTEPFile(string filename);
	
	/*! 
	\brief read the STL-File in the OCC-tree
	\param filename absolute path to a stl-file
	\return true if no error occured
	\return false otherwise
	*/
	bool readSTLFile(string filename);

	/*!
	\brief read the BREP-File in the OCC-tree
	\param filename absolute path to a brep-file
	\return true if no error occured
	\return false otherwise
	*/
	bool readBREPFile(string filename);
	
	/*! 
	\brief write the OCC tree in the STL-file
	\param filename absolute path to a STL-file
	\return true if no error occured
	\return false otherwise
	*/
	bool writeSTLFile(string filename);
	
	/*! 
	\brief read the IGES-File in the OCC-tree
	\param filename absolute path to a IGES-file
	\return true if no error occured
	\return false otherwise
	*/
	bool readIGESFile(string filename);

	/*!
	\brief write the OCC tree in the IGES-file
	\param filename absolute path to a IGES-file
	\return true if no error occured
	\return false otherwise
	*/
	bool writeIGESFile(string filename);   

	/*! 
	\brief read the VRML-File in the OCC-tree
	\param filename absolute path to a VRML-file
	\return true if no error occured
	\return false otherwise
	*/
	bool readVRMLFile(string filename);

	/*! 
	\brief write the OCC tree in the VRML-file
	\param filename absolute path to a VRML-file
	\return true if no error occured
	\return false otherwise
	*/
	bool writeVRMLFile(string filename);

	/*!
	\brief write the OCC tree in the BREP-file
	\param filename absolute path to a BREP-file
	\return true if no error occured
	\return false otherwise
	*/
	bool writeBREPFile(string filename);
	
	/*! 
	\brief Check the return value of an OCC operation
	\param status return value of the operation
	\return true if no error occured
	\return false otherwise
	*/
	bool CheckOCCRetVal(IFSelect_ReturnStatus status);

	/*!
	\brief Check the model for validity and try to fix problems
	\param bFixNeeded true if there was a problem detected
	\param bFixSucceeded true if a problem was detected and could be fixed
	\return true if no error occured
	\return false otherwise
	*/
	bool CheckAndFixModel(bool& bFixNeeded, bool& bFixSucceeded);
};

#endif
