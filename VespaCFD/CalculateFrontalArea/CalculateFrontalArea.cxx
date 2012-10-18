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

// just to make things nicer later and save some documentation reading ;)
struct boundingBox {
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    double zmin;
    double zmax;
};

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
    double resolution = 1; // step value while scanning - will need to do some more calculations later if changed
    //char direction = "X";  // direction of scan

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
    boundingBox boxBounds;
    polydata->GetBounds(bounds);
    boxBounds.xmin = bounds[0];
    boxBounds.xmax = bounds[1];
    boxBounds.ymin = bounds[2];
    boxBounds.ymax = bounds[3];
    boxBounds.zmin = bounds[4];
    boxBounds.zmax = bounds[5];

    //    Debug info if needed:
    std::cout  << "xmin: " << boxBounds.xmin << " "
               << "xmax: " << boxBounds.xmax << std::endl
               << "ymin: " << boxBounds.ymin << " "
               << "ymax: " << boxBounds.ymax << std::endl
               << "zmin: " << boxBounds.zmin << " "
               << "zmax: " << boxBounds.zmax << std::endl;

    // build OBB Tree locator
    vtkSmartPointer<vtkOBBTree> tree = vtkSmartPointer<vtkOBBTree>::New();
    tree->SetDataSet(polydata);
    tree->BuildLocator();

    // calculate area
    double area = 0;
    for (double zdir = boxBounds.zmin; zdir < boxBounds.zmax; zdir = zdir + resolution)
    {
        for (double ydir = boxBounds.ymin; ydir <= boxBounds.ymax; ydir = ydir + resolution)
        {
            // Line to intersect with
            double p1[3] = {boxBounds.xmin, ydir, zdir};
            double p2[3] = {boxBounds.xmax, ydir, zdir};
            //Find intersection points
            vtkSmartPointer<vtkPoints> intersectPoints = vtkSmartPointer<vtkPoints>::New();
            tree->IntersectWithLine(p1, p2, intersectPoints, NULL);
            // this bit will draw a rough approximation of the shape
            if (intersectPoints->GetNumberOfPoints() > 0)
            {
                cout << "#";
                area++;
            }
            else
            {
                cout << " ";
            }
        }
        cout << std::endl;
    }
    // output how many hits, which for a resolution of 1 should equal something like 1mm^2
    cout << "area: " << area << std::endl;

    return EXIT_SUCCESS;
}
