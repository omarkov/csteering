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

#include "csmodeltriangulation.h"
#include <Standard.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepMesh.hxx>
#include <BRepAlgo.hxx>
#include <Poly_Triangulation.hxx>
#include <Geom_BSplineSurface.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESControl_Reader.hxx>
#include <IGESControl_Writer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>
#include <VrmlAPI_Writer.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <ShapeFix_Shape.hxx>
#include <vector>

using namespace std;


CSModelOCCTriangulation::CSModelOCCTriangulation()
	:	m_faces(0), m_dbDeflection(0.5), m_iFaceCount(0)
{
}

CSModelOCCTriangulation::~CSModelOCCTriangulation() 
{
	cleanupTriangulation();
}

//*****************************************************************************
//*                          TRIANGULATION                                    *
//*****************************************************************************

bool CSModelOCCTriangulation::setDeflection(double dbDefl)
{
	if (0.0 >= dbDefl)
	return false;

	m_dbDeflection = dbDefl;
	return true;
}

double CSModelOCCTriangulation::getDeflection()
{
	return m_dbDeflection;
}

bool CSModelOCCTriangulation::startTriangulation ()
{
	BRepMesh::Mesh(m_shapeRoot, m_dbDeflection);

	if (0 < getNumberOfFaces())
		cleanupTriangulation(); // there was already an active triangulation

	m_iFaceCount = 0;
	TopExp_Explorer Ex;
	for (Ex.Init(m_shapeRoot, TopAbs_FACE); Ex.More(); Ex.Next())
	{
		m_iFaceCount++;
	}
	m_faces = new FACE_TRGL[m_iFaceCount];

	return true;
}

FACE_TRGL* CSModelOCCTriangulation::getTriangulatedFaces(int &iFaceCount)
{
	iFaceCount = getNumberOfFaces();
	return m_faces;
}

FACE_TRGL* CSModelOCCTriangulation::getTriangulation(int& iFaceCount)
{
	for (int i = 0; i < getNumberOfFaces(); ++i)
	{
		FACE_TRGL triangulation;
		if (!getTriangulationOfFace(i, triangulation))
			return NULL;
   
   		m_faces[i] = triangulation;
	}
	iFaceCount = getNumberOfFaces();
	
	return m_faces;
}

int CSModelOCCTriangulation::getNumberOfFaces()
{
	return m_iFaceCount;
}

bool CSModelOCCTriangulation::getTriangulationOfFace(int iFaceID, FACE_TRGL& triangulation)
{
	if (iFaceID < 0 || iFaceID > getNumberOfFaces() - 1)
		return false;

	// search face with matching ID
	TopExp_Explorer Ex;
	TopoDS_Face currentFace;
	int iFaceNum = 0;
	for (Ex.Init(m_shapeRoot, TopAbs_FACE); Ex.More(); Ex.Next())
	{
		if (iFaceID == iFaceNum)
		{
			currentFace = TopoDS::Face(Ex.Current());
			break;
		}
		iFaceNum++;
	}

	// There could be changed poles in the face -> retriangulate
	BRepTools::Clean(currentFace);
	BRepTools::Update(currentFace);
	BRepMesh::Mesh(currentFace, m_dbDeflection);

	TopLoc_Location Loc;
	Handle (Poly_Triangulation) facing = BRep_Tool::Triangulation(currentFace,Loc);

	// get all nodes of the face
	int iNumNodes = facing->NbNodes();
	TColgp_Array1OfPnt faceNodes(1, iNumNodes);
	faceNodes = facing->Nodes();

	CSVERTEX *vertexArr = new CSVERTEX[iNumNodes];
	int iOffset = faceNodes.Lower();
	for (int i = faceNodes.Lower(); i <= faceNodes.Upper(); ++i)
	{
		gp_Pnt Point = faceNodes.Value(i);
		vertexArr[i-iOffset].x = Point.X(); // OCC arrazs are 1-based!!
		vertexArr[i-iOffset].y = Point.Y();
		vertexArr[i-iOffset].z = Point.Z();
	}

	// get all triangles of the face
	int iNumTriangles = facing->NbTriangles();
	Poly_Array1OfTriangle faceTriangles(1, iNumTriangles);
	faceTriangles = facing->Triangles();

	CSTRIANGLE *triangleArr = new CSTRIANGLE[iNumTriangles];
	iOffset = faceTriangles.Lower();
	TopAbs_Orientation orient = (currentFace).Orientation();
	for (int i = faceTriangles.Lower(); i <= faceTriangles.Upper(); i++)
	{
		Poly_Triangle tmpTri = faceTriangles.Value(i);
		
		if (TopAbs_REVERSED == orient)
		{
			tmpTri.Get(triangleArr[i-iOffset].c,
			triangleArr[i-iOffset].b,
			triangleArr[i-iOffset].a);
		}
		else
		{
			tmpTri.Get(triangleArr[i-iOffset].a,
			triangleArr[i-iOffset].b,
			triangleArr[i-iOffset].c);
		}
	}

	// get all control-points (poles) of the face
	Handle_Geom_Surface surface = BRep_Tool::Surface(currentFace);
	
	if (!surface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
	{
		cout << "No B-Spline surface!" << endl;
		triangulation.iSizePolesU = 0;
		triangulation.iSizePolesV = 0;
		triangulation.poles = NULL;
		triangulation.iSizeVertices = iNumNodes;
		triangulation.vertices = vertexArr;
		triangulation.iSizeTriangles = iNumTriangles;
		triangulation.triangles = triangleArr;
		return true;
	}
	
	// Now we know that the surface is a bspline surface
	Handle(Geom_BSplineSurface) nurbs = Handle(Geom_BSplineSurface)::DownCast(surface);
	triangulation.iSizePolesU = nurbs->NbUPoles();
	triangulation.iSizePolesV = nurbs->NbVPoles();

	CSPOLE *poleArr;
	if (triangulation.iSizePolesU == 0 || triangulation.iSizePolesV == 0)
		poleArr = new CSPOLE[0];
	else 
	{
		poleArr = new CSPOLE[nurbs->NbUPoles() * nurbs->NbVPoles()];

		int iPos = 0;
		for (int i = 1; i <= nurbs->NbUPoles(); i++)
		{
			for (int j = 1; j <= nurbs->NbVPoles(); j++)
			{
				gp_Pnt pole = nurbs->Pole(i,j);
				CSPOLE poleTemp;
				poleTemp.x = pole.X();
				poleTemp.y = pole.Y();
				poleTemp.z = pole.Z();
				poleArr[iPos] = poleTemp;
				iPos += 1;
			}
		}
		triangulation.poles = poleArr;
	}

	triangulation.iSizeVertices = iNumNodes;
	triangulation.iSizeTriangles = iNumTriangles;
	triangulation.vertices = vertexArr;
	triangulation.triangles = triangleArr;
	return true;
}

void CSModelOCCTriangulation::cleanupTriangulation()
{
	for (int i=0; i < getNumberOfFaces(); ++i)
	{
		delete [] m_faces[i].vertices;
		delete [] m_faces[i].triangles;
		delete [] m_faces[i].poles;
	}
	if (0 != getNumberOfFaces())
	{
		delete [] m_faces;
		m_faces = NULL;
	};
}


//*****************************************************************************
//*                          CHANGING POLES                                   *
//*****************************************************************************

bool CSModelOCCTriangulation::changePoles(int iFaceID, CSPOLE *newPolePos)
{
	if (iFaceID < 0 || iFaceID > (getNumberOfFaces() - 1) )
		return false;

	// search face with matching ID
	TopExp_Explorer Ex;
	TopoDS_Face currentFace;
	int iFaceNum = 0;
	for (Ex.Init(m_shapeRoot, TopAbs_FACE); Ex.More(); Ex.Next())
	{
		if (iFaceID == iFaceNum)
		{
			currentFace = TopoDS::Face(Ex.Current());
			break;
		}
		iFaceNum++;
	}

	Handle_Geom_Surface surface = BRep_Tool::Surface(currentFace);
	if (surface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
	{
		Handle(Geom_BSplineSurface) nurbs = Handle(Geom_BSplineSurface)::DownCast(surface);
		int u = nurbs->NbUPoles() ; 
		int v = nurbs->NbVPoles() ; 
		for(int i=0; i<u*v;i++)
		{
			gp_Pnt newPole(newPolePos[i].x, newPolePos[i].y, newPolePos[i].z);
			nurbs->SetPole(i/v+1, i%v+1, newPole);
		}

		// re-triangulate face
		BRepMesh::Mesh(currentFace, m_dbDeflection);
		return true;
	}
	else
		return false;
}

bool CSModelOCCTriangulation::changePole(
	int iFaceID,
	int uPos,
	int vPos,
	CSPOLE newPolePos)
{
	if (iFaceID < 0 || iFaceID > (getNumberOfFaces() - 1) )
		return false;

	// search face with matching ID
	TopExp_Explorer Ex;
	TopoDS_Face currentFace;
	int iFaceNum = 0;
	for (Ex.Init(m_shapeRoot, TopAbs_FACE); Ex.More(); Ex.Next())
	{
		if (iFaceID == iFaceNum)
		{
			currentFace = TopoDS::Face(Ex.Current());
			break;
		}
	iFaceNum++;
	}

	Handle_Geom_Surface surface = BRep_Tool::Surface(currentFace);
	if (surface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
	{
		Handle(Geom_BSplineSurface) nurbs = Handle(Geom_BSplineSurface)::DownCast(surface);
		gp_Pnt newPole(newPolePos.x, newPolePos.y, newPolePos.z);
		nurbs->SetPole(uPos+1, vPos+1, newPole);

		// re-triangulate face
		BRepMesh::Mesh(currentFace, m_dbDeflection);
		return true;
	}
	else
		return false;
}


//*****************************************************************************
//*                          IMPORT / EXPORT                                  *
//*****************************************************************************

bool CSModelOCCTriangulation::setCADFile (string filename, CADFILETYPE type)
{
	cout << "reading " << filename << endl;
	return readCADFile( filename, type);
}

bool CSModelOCCTriangulation::readCADFile(string filename, CADFILETYPE type)
{
	bool bRet = false;
	switch(type)
	{
		case CADFILETYPE_STEP :
		{
			bRet = readSTEPFile(filename);	
		}
		break;
		case CADFILETYPE_IGES :
		{
			bRet = readIGESFile(filename);
		}
		break;
		case CADFILETYPE_STL  :
		{
			bRet = readSTLFile(filename);
		}
		break;
		case CADFILETYPE_VRML :
		{
			bRet = readVRMLFile(filename);
		}
		break;
		case CADFILETYPE_BREP :
		{
			bRet = readBREPFile(filename);		
		}
		break;
		default:
		{
			cout << "Unkown CAD-Filetype" << endl;
			return false;	
		}
	}

	if (!bRet)
	{
		cerr << "Error while loading " << filename << endl;
		cout << "Error while loading " << filename << endl;
		return false;
	}

	// Check if the model is valid and try to fix problems if necessary
	bool bFixNeeded = false;
	bool bFixSucceeded = false;
	if (!CheckAndFixModel(bFixNeeded, bFixSucceeded))
	{
		cerr << "There was a problems while loading " << filename << endl;
		cerr << "--> Internal problems of the model could not be fixed!" << endl;
		cout << "An error occured while loading the " << filename << endl;
	}

	return bRet;
}

bool CSModelOCCTriangulation::readSTEPFile(string filename)
{
	STEPControl_Reader reader;

	if (!CheckOCCRetVal(reader.ReadFile((char*)filename.c_str())) )
	{
		cout << "Error while reading the step-file: \"" << filename << "\"" << endl;
		return false;
	}
	reader.PrintCheckLoad(false, IFSelect_ResultCount);
	reader.PrintCheckTransfer(true, IFSelect_ItemsByEntity);
	// for some strange reason the following line is necessary
	// to initialize the reader !?! -> don't remove it!
	reader.NbRootsForTransfer();
	reader.TransferRoots();

	// now we can get the occ-tree from the reader
	m_shapeRoot = reader.OneShape();

	return true;
}

bool CSModelOCCTriangulation::readIGESFile(string filename)
{
	IGESControl_Reader reader;
	Standard_Integer nIgesFaces, nTransFaces;
	
	// Load the file
	if (!CheckOCCRetVal(reader.ReadFile((char*)filename.c_str())) )
	{
		cout << "Error while reading the IGES-file: \"" << filename << "\"" << endl;
		return false;
	}
	
	// Select all iges-faces in the file and put them into a list
	Handle(TColStd_HSequenceOfTransient) faceList = reader.GiveList("iges-faces");
	
	nIgesFaces = faceList->Length();
	nTransFaces = reader.TransferList(faceList);
	
	cout << nTransFaces << " IGES-faces transfered" << endl;
	
	// Now we can get the occ-tree from the reader
	m_shapeRoot = reader.OneShape();
	
	return true;	
}

bool CSModelOCCTriangulation::readSTLFile(string filename)
{
	StlAPI_Reader reader;
	reader.Read(m_shapeRoot, (char*)filename.c_str());
	return true;
}

bool CSModelOCCTriangulation::readVRMLFile(string filename)
{
	cout << "VRML reading is not implemented in OCC" << endl;
	return false;	
}

bool CSModelOCCTriangulation::readBREPFile(string filename)
{
	BRep_Builder builder;
	return BRepTools::Read(m_shapeRoot, (char*)filename.c_str(), builder);
}


bool CSModelOCCTriangulation::storeCADFile(string filename, CADFILETYPE type)
{
	switch (type)
	{
	case CADFILETYPE_STEP :
	{
	return writeSTEPFile(filename);
	}
	break;
	case CADFILETYPE_IGES:
	{
		return writeIGESFile(filename);
	}
	break;
	case CADFILETYPE_STL:
	{
		return writeSTLFile(filename);
	}
	break;
	case CADFILETYPE_VRML:
	{
		return writeVRMLFile(filename);
	}
	break;
	case CADFILETYPE_BREP:
	{
		return writeBREPFile(filename);
	}
	default:
		return false;
	}
	
	return true;
}

bool CSModelOCCTriangulation::writeSTEPFile(string filename)
{
	STEPControl_Writer writer;
	
	if (!CheckOCCRetVal( writer.Transfer(m_shapeRoot, STEPControl_AsIs)) )
	{
		cout << "Error while transfering root shape to step entities!" << endl;
		return false;
	}
	
	if (!CheckOCCRetVal( writer.Write((char*)filename.c_str())) )
	{
		cout << "Error while writing the stepfile: \"" << filename << "\"" << endl;
		return false;
	}

	return true;
}

bool CSModelOCCTriangulation::writeIGESFile(string filename)
{
	IGESControl_Controller::Init();
	IGESControl_Writer writer;
	
	// first we have to add our OCC tree to the writer
	if (!writer.AddShape(m_shapeRoot))
	{
		cout << "Error while transfering root shape to IGES entities!" << endl;
		return false;	
	}
	writer.ComputeModel();
		
	if (!writer.Write((char*)filename.c_str()) ) 
	{
		cout << "Error while writing the IGES file: \"" << filename << "\"" << endl;
		return false;
	}

	return true;	
}

bool CSModelOCCTriangulation::writeSTLFile(string filename)
{
	StlAPI_Writer writer;
	writer.Write(m_shapeRoot, (char*)filename.c_str());
	return true;	
}

bool CSModelOCCTriangulation::writeVRMLFile(string filename)
{
	VrmlAPI_Writer writer;
	writer.Write(m_shapeRoot, (char*)filename.c_str());
	return true;	
}

bool CSModelOCCTriangulation::writeBREPFile(string filename)
{
	return BRepTools::Write(m_shapeRoot, (char*)filename.c_str());
}


//*****************************************************************************
//*                          HELPER FUNCTIONS                                 *
//*****************************************************************************

bool CSModelOCCTriangulation::CheckOCCRetVal(IFSelect_ReturnStatus status)
{
	switch(status)
	{
	case IFSelect_RetVoid : // intended fallthrough
	case IFSelect_RetError:
	case IFSelect_RetFail :
	case IFSelect_RetStop :
	{
		cout << "An error occurred while performing an OCC operation." << endl;
		return false;
	}
	default: 
		return true;
	}
}

bool CSModelOCCTriangulation::CheckAndFixModel(bool& bFixNeeded, bool& bFixSucceeded)
{
	ShapeAnalysis_Shell occShapeAnalysisShell;
	if (!occShapeAnalysisShell.CheckOrientedShells(m_shapeRoot))
	{
		bFixNeeded = true;
		cerr << "Shell orientation error detected!" << endl;
		cout << "trying to fix shape...";
		ShapeFix_Shape occShapeFixShape(m_shapeRoot);
		if (occShapeFixShape.Perform())
		{
			bFixSucceeded = true;
			cout << "done successfully!" << endl;
		}
		else
		{
			bFixSucceeded = false;
			cout << "failed!" << endl;
			cerr << "Not capable to fix shells." << endl;
		}
	}

	return (!bFixNeeded || (bFixNeeded && bFixSucceeded));
}

