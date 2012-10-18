#include <vtkPolyData.h>
#include <vtkTriangle.h>
#include <vtkLineSource.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkIdTypeArray.h>
#include <vtkSelectionNode.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSelection.h>
#include <vtkExtractSelection.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkCellArray.h>
#include <vtkCell.h>
#include <vtkInformation.h>
#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkOBBTree.h>


// C++
#include <iostream>
using namespace std;

// this projects a series of lines through a mesh and looks for intersections to determine frontal area
// lines are projected based on the bounding box and the resolution is determined by the steps
// For my application I just need to look at frontal area along the X direction. The follow code could easily be
// adapted to take a direction as a command line option and produce the area from that .
// So this currently "scans" yMin->yMax,zMin->ZMax, with a line from Xmin to Xmax
// ref: http://www.cmake.org/Wiki/VTK/Examples/Cxx/DataStructures/OBBTree_IntersectWithLine

int main(int argc, char *argv[])
{
	// check and get the stl input file provided
    if ( argc != 2 )
    {
        cout << "Required parameters: Filename" << endl;
        return EXIT_FAILURE;
    }
    std::string inputfile = argv[1];

    // later this should be a command line option - hard coded for now
    double resolution = 2; // step value while scanning

    // read STL and print out some info
    std::cout << "Reading: " << inputfile << std::endl;
    vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
    stlReader->SetFileName(inputfile.c_str());
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New(); 
	polydata = stlReader->GetOutput();
    polydata->Update();
    //    Debug info if needed:
    cout << "Cells: " << polydata->GetNumberOfCells() << endl;
    cout << "Points: " << polydata->GetNumberOfPoints() << endl;
    cout << "Polys: " << polydata->GetNumberOfPolys() << endl;
    cout << "Verts: " << polydata->GetNumberOfVerts() << endl;
    polydata->Print(cout);

    // makes sense to only scan in an area the object exists, the bounding box will tell us this
    double bounds[6];
    polydata->GetBounds(bounds);

    //    Debug info if needed:
    std::cout  << "xmin: " << bounds[0] << " "
               << "xmax: " << bounds[1] << std::endl
               << "ymin: " << bounds[2] << " "
               << "ymax: " << bounds[3] << std::endl
               << "zmin: " << bounds[4] << " "
               << "zmax: " << bounds[5] << std::endl;

    // build OBB Tree locator
    vtkSmartPointer<vtkOBBTree> tree = vtkSmartPointer<vtkOBBTree>::New();
    tree->SetDataSet(polydata);
    tree->BuildLocator();

    // calculate area
    double area = 0;
    for (double ydir = bounds[2]; ydir < bounds[3]; ydir = ydir + resolution)
    {
        for (double xdir = bounds[0]; xdir < bounds[1]; xdir = xdir + resolution)
        {


            // Line to intersect with
            double p1[3] = {xdir, ydir, bounds[4]};
            double p2[3] = {xdir, ydir, bounds[5]};
            //Find intersection points
            vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
            tree->IntersectWithLine(p1, p2, intersectPoints, NULL);

    //        std::cout << "NumPoints: " << intersectPoints->GetNumberOfPoints()
    //                  << std::endl;
//            for(int i = 0; i < intersectPoints->GetNumberOfPoints(); i++ )
//            {
//                double intersection[3];
//                intersectPoints->GetPoint(i, intersection);
//                std::cout << "Intersection " << i << ": "
//                          << intersection[0] << ", "
//                          << intersection[1] << ", "
//                          << intersection[2] << std::endl;
//            }
            if (intersectPoints->GetNumberOfPoints() > 0)
            {
                cout << ".";
                area++;
            }
            else
            {
                cout << " ";
            }
        }
        cout << std::endl;
    }
    cout << "area: " << area << std::endl;

    return EXIT_SUCCESS;
}
