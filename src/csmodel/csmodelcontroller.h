#ifndef STDCSMODELCONTROLLER_H
#define STDCSMODELCONTROLLER_H
/***************************************************************************
 *   Copyright (C) 2004 by Jens Nausedat, Benjamin Jung                    *
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
#include <string>
#include "voxel/voxelization/voxelization.h"
#include "csmodeltriangulation.h"

/*! \class CSModelOCCTriangulation
	\brief This class offers simple interface to the model-component.
	\author Jens Nausedat
	\author Benjamin Jung
	\version 0.1
	\date 16.10.2004
*/
class CSModelController{
private:
	string m_cadFilename;               /*!< absolute path to a cad file */
	CADFILETYPE m_cadFileType;          /*!< type of the cad file (STEP, STL, ... */
	CSModelOCCTriangulation *csmdlTrgl; /*!< triangulation component */
	double m_rotMat[9];					/*!< matrix for the rotation of the model */
	
	FACE_TRGL *m_rotatedModel;			/*!< Array containing all triangulations etc. of all faces */
	int m_iFaceCountRotatedModel;		/*!< number of faces in the cad-model */
	
	/*!
		\brief Get the filetype from the filename
		\param filename absolute path to a cad file
		\param filetype type of the file (Step, IGES, ...)
		\return true if no error occured (matching to a filetype was possible)
		\return false otherwise
	*/
	bool GetFiletypeFromName(string filename, CADFILETYPE& filetype);
	
	/*!
		\brief Set the rotation matrix to the identity matrix
	*/
	void ResetRotationMatrix();
	
	/*!
		\brief Get a DEEP copy of a complete triangulation
		\param source source datastructur
		\param iFaceCount number of faces (size of the array)
		\param destination will contain a pointer on the first element of the copy
	*/
	void CopyTriangulationData(FACE_TRGL* source, int iFaceCount, FACE_TRGL*& destination);
	
	/*!
		\brief Rotate the model using the member rotation matrix
	*/
	void RotateModel();
	
	/*!
		\brief Rotate a point with a given rotation matrix
		\param point the point to rotate
		\param rotMat the rotation matrix
		\return the rotated point
	*/
	CSVERTEX RotatePoint(CSVERTEX point, double rotMat[9]);
	
	/*!
		\brief Delete the triangulation data for the rotated model
	*/
	void DeleteRotatedModel();

public:
	/*! constructor */
	CSModelController();
	/*! destructor */
	~CSModelController();

	/*!
		\brief get the cad-filename
		\return path to the cad-filename
	*/
	string getCADFilename();

	/*!
		\brief set the path to the cad file
		\param filename absolute path to a cad file
		\return true if no error occured
		\return false otherwise
	*/
	bool setCADFilename(string filename);
	
	/*!
		\brief set the matrix for the model rotation
		\param rotMat the matrix for the rotation
	*/
	void setRotationMatrix(double rotMat[9]);

	/*!
		\brief return the triangulation
		\param iFaceCount will contain the number of faces
		\return Array containing the complete triangulation for every face
				of the model
	*/
	FACE_TRGL* getTriangulation(int& iFaceCount);
	FACE_TRGL* getTriangulatedFaces(int& iFaceCount);
	FACE_TRGL *getRotatedModel();

	/*!
		\brief return the number of faces in the model
		\return number of faces in the model
		\remarks this function is needed to use the
		  data returned by getTriangulation
	*/
	int getNumberOfFaces();

	/*!
	   \brief get the triangulation (+control-points) for a single face
	   \param iFaceID id of the face
	   \param bSuccess will contain true if no error occured, false otherwise
	   \return triangulation for the face
	*/
	FACE_TRGL getTriangulationOfFace(int iFaceID, bool& bSuccess);

	/*!
	\brief set the deflection for the triangulation
	\param dbDefl deflection (0.0 <= dbDefl <= 1.00)
	\return true if no error occured
	\return false otherwise
	*/
	bool setDeflection(double dbDefl);

	/*!
	\brief get the deflection for the triangulation
	\return the deflection
	*/
	double getDeflection();

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
	
	/*!
	\brief Set all poles of a face
	\param iFaceID id of the face to change the poles for
	\return true, if no error occured
	\return false, otherwise
	*/
	bool changePoles(int iFaceID, CSPOLE *newPolePos);

	/*!
	\brief save the (probably changed) data in a cad file
	\param filename if bSaveAs=true, the file name in filename will be used,
	  otherwise the initial filename will be used
	\param bSaveAs set this to true to save the changes in a new file which
	  name you specified in filename
	\return true, if no error occured
	\return false, otherwise
	*/
	bool storeCADFile();
	bool storeCADFileAs(string filename);

	/*!
	\brief Get the voxelization from the triangulation
	\param h height of the octree
	\param voxels will contain the voxelization
	\return true, if no error occured
	\return false, otherwise
	*/
	Voxelization *getVoxelization(Height h);
	
	/*!
	\brief Get the voxelization from the triangulation
	\param s size of a voxel
	\param voxels will contain the voxelization
	\return true, if no error occured
	\return false, otherwise
	*/
	Voxelization *getVoxelization(double s);
};

#endif
