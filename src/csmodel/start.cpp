#include "csmodelcontroller.h"
#include <fstream>

int old_main ( int argc, char* argv[] )
{
  if (argc < 4) {
    FACE_TRGL *myFaces;
    CSModelController* csmdlCon = new CSModelController();
    if (!csmdlCon->setCADFilename( argv[1]))
      cout << "Error while setting filename " << argv[1] << endl;

    int size = 0;
    myFaces = csmdlCon->getTriangulation(size);

    //Print to file
    ofstream output (argv[2]);
    cout << "SIZE: " << size << endl;

    for (int i = 0; i < size; i++) {
      output << i+1 <<endl;
      output << myFaces[i].iSizeVertices <<endl;
      output << endl;

      for (int k = 0; k < myFaces[i].iSizeVertices; k++) {
        output << myFaces[i].vertices[k].x << "\t"<< myFaces[i].vertices[k].y << "\t"<< myFaces[i].vertices[k].z <<endl;
      }
      output << endl;

      output << myFaces[i].iSizeTriangles <<endl;
      output << endl;

      for (int l = 0; l < myFaces[i].iSizeTriangles; l++) {
        output << myFaces[i].triangles[l].a << "\t" << myFaces[i].triangles[l].b  << "\t"<< myFaces[i].triangles[l].c<<endl;
      }
      output << endl;

      output << myFaces[i].iSizePolesU <<endl;
      output << myFaces[i].iSizePolesV <<endl;
      output << endl;

      for (int m = 0; m < (myFaces[i].iSizePolesU * myFaces[i].iSizePolesV); m++) {
        output << myFaces[i].poles[m].x << "\t"<< myFaces[i].poles[m].y << "\t"<< myFaces[i].poles[m].z <<endl;
      }
      output << endl << endl;
    }

    if (csmdlCon->storeCADFileAs("test.step"))
      cout << "successfull writen to test.step" << endl;
    else
      cout << "error while writing to test.step" << endl;

  } else
          cout << "No argument!";
  return 0;
}

