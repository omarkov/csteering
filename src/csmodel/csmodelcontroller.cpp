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
#include "csmodelcontroller.h"
#include <string>


CSModelController::CSModelController()
	: m_rotatedModel(0), m_iFaceCountRotatedModel(0)
	
{
	csmdlTrgl = new CSModelOCCTriangulation();
	
	// Initialize the rotation matrix with the identity matrix
	ResetRotationMatrix();

}

CSModelController::~CSModelController()
{
	delete csmdlTrgl;
	DeleteRotatedModel();
}

string CSModelController::getCADFilename()
{
	return m_cadFilename;
}

bool CSModelController::setCADFilename(string filename)
{
	m_cadFilename = filename;
	
	// Now we have to determine the type of the CAD-file
	if (!GetFiletypeFromName(filename, m_cadFileType))
		return false;
		
	// Reset the rotation matrix -> a new loaded model is
	// never rotated!
	ResetRotationMatrix();
	

	return csmdlTrgl->setCADFile(filename, m_cadFileType);
}


FACE_TRGL* CSModelController::getTriangulatedFaces(int& iFaceCount)
{
	return csmdlTrgl->getTriangulatedFaces(iFaceCount);
}

FACE_TRGL* CSModelController::getTriangulation(int& iFaceCount)
{
	FACE_TRGL* trglArray;
	if (!csmdlTrgl->startTriangulation())
		cout << "startTriangulation failed!" << endl;
	trglArray = csmdlTrgl->getTriangulation(iFaceCount);

	return trglArray;
}

void CSModelController::ResetRotationMatrix()
{
	m_rotMat[0] = 1.0;
	m_rotMat[1] = 0.0;
	m_rotMat[2] = 0.0;
	
	m_rotMat[3] = 0.0;
	m_rotMat[4] = 1.0;
	m_rotMat[5] = 0.0;
	
	m_rotMat[6] = 0.0;
	m_rotMat[7] = 0.0;
	m_rotMat[8] = 1.0;
}

void CSModelController::setRotationMatrix(double rotMat[9])
{
	memcpy(m_rotMat, rotMat, sizeof(rotMat));
	for (int i = 0; i < 9; ++i)
	{
		m_rotMat[i] = rotMat[i];
	}
}

int CSModelController::getNumberOfFaces()
{
	return csmdlTrgl->getNumberOfFaces();
}

FACE_TRGL CSModelController::getTriangulationOfFace(int iFaceID, bool& bSuccess)
{
	FACE_TRGL trgl;
	FACE_TRGL *faces;
	int size;
	
	bSuccess = (csmdlTrgl->getTriangulationOfFace(iFaceID, trgl));

	faces = csmdlTrgl->getTriangulatedFaces(size);

	if(faces != NULL && iFaceID < size)
	{
		delete [] faces[iFaceID].vertices;
		delete [] faces[iFaceID].triangles;
		delete [] faces[iFaceID].poles;

		faces[iFaceID] = trgl;
	}

	return trgl;
}

bool CSModelController::setDeflection(double dbDefl)
{
	return csmdlTrgl->setDeflection(dbDefl);
}

double CSModelController::getDeflection()
{
	return csmdlTrgl->getDeflection();
}

bool CSModelController::changePoles(int iFaceID, CSPOLE *newPolePos)
{
	return csmdlTrgl->changePoles(iFaceID, newPolePos);
}

bool CSModelController::changePole(int iFaceID, int uPos, int vPos, CSPOLE newPolePos)
{
	return csmdlTrgl->changePole(iFaceID, uPos, vPos, newPolePos);
}

bool CSModelController::storeCADFileAs(string filename )
{
	CADFILETYPE newFileType;
	if (!GetFiletypeFromName(filename, newFileType))
		return false;
	if (CADFILETYPE_UNDEFINED == newFileType)
		newFileType = m_cadFileType;
	return csmdlTrgl->storeCADFile(filename, newFileType);
}

bool CSModelController::storeCADFile()
{
	return csmdlTrgl->storeCADFile(m_cadFilename, m_cadFileType);
}

FACE_TRGL *CSModelController::getRotatedModel()
{
	int iSize;
	FACE_TRGL *trglArr = getTriangulatedFaces(iSize);
	// Prevent leakage
	DeleteRotatedModel();
	m_iFaceCountRotatedModel = iSize;
	
	// Create the "new", rotated triangulation
	CopyTriangulationData(trglArr, iSize, m_rotatedModel);
	
	// Rotate the model using the rotation matrix
	RotateModel();

	return m_rotatedModel;
}

Voxelization *CSModelController::getVoxelization(Height h)
{
	int iSize = getNumberOfFaces();

	return new Voxelization(getRotatedModel(), h, iSize);
}

Voxelization *CSModelController::getVoxelization(double s)
{
	int iSize = getNumberOfFaces();
	
	return new Voxelization(getRotatedModel(), s, iSize);
}

bool CSModelController::GetFiletypeFromName(string filename, CADFILETYPE& filetype)
{
	// Is it an IGES-File ?
	if (strstr((char*)filename.c_str(), ".iges") != NULL)
	{
		filetype = CADFILETYPE_IGES;
		return true;	
	}
	
	// Is it a STEP-File ?
	if ((strstr((char*)filename.c_str(), ".stp") != NULL) || (strstr((char*)filename.c_str(), ".step") != NULL) )
	{
		filetype = CADFILETYPE_STEP;
		return true;	
	}
	
	// Is it an VRML-File
	if (strstr((char*)filename.c_str(), ".wrl") != NULL)
	{
		filetype = CADFILETYPE_VRML;
		return true;	
	}
	
	// Or is it an STL-File ?
	if (strstr((char*)filename.c_str(), ".stl") != NULL)
	{
		filetype = CADFILETYPE_STL;
		return true;	
	}

	if (strstr((char*)filename.c_str(), ".brep") != NULL)
	{
		filetype = CADFILETYPE_BREP;
		return true;
	}
	
	// No filetype recognised
	filetype = CADFILETYPE_UNDEFINED;
	return false;	
}

void CSModelController::CopyTriangulationData(FACE_TRGL* source, int iFaceCount, FACE_TRGL*& destination)
{
	// First we have to create an new array containing the same number of faces as the source
	// data structure
	destination = new FACE_TRGL[iFaceCount];
	
	for (int iFace = 0; iFace < iFaceCount; ++iFace)
	{
		FACE_TRGL sourceFace = source[iFace];
		
		// Create a new array for the triangles and copy the source ones 
		CSTRIANGLE *newTriangleArr = new CSTRIANGLE[sourceFace.iSizeTriangles];
		for (int i = 0; i < sourceFace.iSizeTriangles; ++i)
		{
			newTriangleArr[i] = sourceFace.triangles[i];
		}
		
		// Create a new array for the vertices and copy the source ones
		CSVERTEX *newVertexArr = new CSVERTEX[sourceFace.iSizeVertices];
		for (int k = 0; k < sourceFace.iSizeVertices; ++k)
		{
			newVertexArr[k] = sourceFace.vertices[k];
		}
	
		// Create a new array for the poles and copy the source ones
		CSPOLE *newPoleArr = new CSPOLE[sourceFace.iSizePolesU * sourceFace.iSizePolesV];
		int iNumPoles = sourceFace.iSizePolesU * sourceFace.iSizePolesV;
		for (int l = 0; l < iNumPoles; ++l)
		{
			newPoleArr[l] = sourceFace.poles[l];
		}
		
		// Copy the new deep copies in to their final destination
		destination[iFace].iSizeVertices = sourceFace.iSizeVertices;
		destination[iFace].iSizeTriangles = sourceFace.iSizeTriangles;
		destination[iFace].iSizePolesU = sourceFace.iSizePolesU; 
		destination[iFace].iSizePolesV = sourceFace.iSizePolesV;
		destination[iFace].vertices = newVertexArr;
		destination[iFace].triangles = newTriangleArr;
		destination[iFace].poles = newPoleArr;		
	}
}

void CSModelController::RotateModel()
{
	for(int iFace = 0; iFace < m_iFaceCountRotatedModel; ++iFace)
	{
		FACE_TRGL faceTemp = m_rotatedModel[iFace];
		
		// Rotate the vertices
		for (int iVer = 0; iVer < faceTemp.iSizeVertices; ++iVer)
		{
			faceTemp.vertices[iVer] = RotatePoint( faceTemp.vertices[iVer], m_rotMat);
		}
		
		// Rotate the poles
		for (int iPole = 0; iPole < (faceTemp.iSizePolesU * faceTemp.iSizePolesV); ++iPole)
		{
			faceTemp.poles[iPole] = RotatePoint( faceTemp.poles[iPole], m_rotMat);	
		}
	}
}

CSVERTEX CSModelController::RotatePoint(CSVERTEX point, double rotMat[9])
{
	CSVERTEX resultPoint;
	resultPoint.x = rotMat[0] * point.x + rotMat[3] * point.y + rotMat[6] * point.z;
	resultPoint.y = rotMat[1] * point.x + rotMat[4] * point.y + rotMat[7] * point.z;
	resultPoint.z = rotMat[2] * point.x + rotMat[5] * point.y + rotMat[8] * point.z;
	return resultPoint;
}

void CSModelController::DeleteRotatedModel()
{
	for (int i=0; i < m_iFaceCountRotatedModel; ++i)
	{
		delete [] m_rotatedModel[i].vertices;
		delete [] m_rotatedModel[i].triangles;
		delete [] m_rotatedModel[i].poles;
	}
	if (0!= m_iFaceCountRotatedModel)
	{
		delete [] m_rotatedModel;
		m_rotatedModel = NULL;
	};
}
