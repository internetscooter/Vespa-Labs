#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkSphereSource.h>
#include <vtkPointSource.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkSTLReader.h>
#include <vtkSTLWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkWeightedTransformFilter.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetSurfaceFilter.h>

// C/C++
#include <iostream>
#include <sstream>
#include <stdio.h>
using namespace std;

// just to make things nicer later and save some documentation reading ;)
struct boundingBox {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
};

/*

  This shrinkwraps a mesh so that it is water tight

  Here's how it works:
  1) We load an STL and work out the dimensions of the mesh
  2) We place sphere around the mesh with a particular resolution
  3) We use a smooth filter to "wrap" the mesh with the sphere
  e.g. On Windows
  "WrapMesh.exe D:\scooter\vespalabs\openfoam\motorBike\constant\triSurface\motorBike.stl" 5000

  */

int main(int argc, char *argv[])
{
    // check and get options provided
    if ( argc != 3 )
    {
        cout << "Required parameters:" << endl;
        cout << "  Filename (Full path to stl file e.g. something.stl)" << endl;
        cout << "  Mesh resolution of shrinkwrap e.g. 500)" << endl;

        return EXIT_FAILURE;
    }
    std::string inputfile = argv[1];
    double meshres = atof (argv[2]);

    // Read STL
    std::cout << "Reading: " << inputfile << std::endl;
    vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
    stlReader->SetFileName(inputfile.c_str());
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata = stlReader->GetOutput();
    stlReader->Update();

    // The bounding box will tell us where the object is and how big it is...
    std::cout << "Calculating bounding box: " << inputfile << std::endl;
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(stlReader->GetOutputPort());
    double centre[3];
    double bounds[6];
    boundingBox boxBounds;
    mapper->GetBounds(bounds);
    mapper->GetCenter(centre);
    boxBounds.xmin = bounds[0];
    boxBounds.xmax = bounds[1];
    boxBounds.ymin = bounds[2];
    boxBounds.ymax = bounds[3];
    boxBounds.zmin = bounds[4];
    boxBounds.zmax = bounds[5];

  // TODO: possibly remesh with finer detail with subdivide filter?

  // Create sphere around bounds (TODO: possibly transformed to a better shape?)
    std::cout << "Creating Sphere: Mesh Resolution = " << meshres << std::endl;
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius( boxBounds.xmax + boxBounds.ymax + boxBounds.zmax); // could be better but will work
    sphereSource->SetPhiResolution(meshres);
    sphereSource->SetThetaResolution(meshres);
    sphereSource->SetCenter(centre);
    sphereSource->Update();

  // Shrinkwrap
  std::cout << "ShrinkWrapping: " << inputfile << std::endl;
  vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  smoothFilter->SetInputConnection(0, sphereSource->GetOutputPort());
  smoothFilter->SetNumberOfIterations(1);
//  smoothFilter->SetFeatureAngle(90);
//  smoothFilter->SetFeatureEdgeSmoothing(1);
  smoothFilter->SetInputData( 1, polydata);
  smoothFilter->Update();
 
  // spit out the result as an stl
  vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName("output.vtp");
  writer->SetInputConnection(smoothFilter->GetOutputPort());
  writer->Write();
 
  // write STL
  vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
  stlWriter->SetFileName("shrinkwrapped.stl");
  stlWriter->SetInputConnection(smoothFilter->GetOutputPort());
  stlWriter->Update();
  cout << "Writing: " << "shrinkwrapped.stl" << endl;
  stlWriter->Write();

  return EXIT_SUCCESS;
}
