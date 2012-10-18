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
//    cout << "Cells: " << polydata->GetNumberOfCells() << endl;
//    cout << "Points: " << polydata->GetNumberOfPoints() << endl;
//    cout << "Polys: " << polydata->GetNumberOfPolys() << endl;
//    cout << "Verts: " << polydata->GetNumberOfVerts() << endl;
//	polydata->Print(cout);

    double bounds[6];
    polydata->GetBounds(bounds);

    std::cout  << "xmin: " << bounds[0] << " "
               << "xmax: " << bounds[1] << std::endl
               << "ymin: " << bounds[2] << " "
               << "ymax: " << bounds[3] << std::endl
               << "zmin: " << bounds[4] << " "
               << "zmax: " << bounds[5] << std::endl;

    // select cells of interest
    // ref: http://www.kitware.com/media/html/SelectionsInVTK.html
    vtkSmartPointer<vtkSelectionNode> cellSelectionNode = vtkSmartPointer<vtkSelectionNode>::New();
    cellSelectionNode->SetFieldType(vtkSelectionNode::CELL);
    cellSelectionNode->SetContentType(vtkSelectionNode::INDICES);

    vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
    ids->SetNumberOfComponents(1);
    vtkSmartPointer<vtkOBBTree> tree = vtkSmartPointer<vtkOBBTree>::New();
    tree->SetDataSet(polydata);
    tree->BuildLocator();

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
    for (int nCellID = 0; nCellID < polydata->GetNumberOfCells(); nCellID++)
    {
        vtkCell* cell;
        cell = polydata->GetCell(nCellID);

        double tolerance = 0.001;
        // Outputs
        double t; // Parametric coordinate of intersection (0 (corresponding to p1) to 1 (corresponding to p2))
        double x[3]; // The coordinate of the intersection
        double pcoords[3];
        int subId;
        double p1[3] = {0, 0, 0.0};
        double p2[3] = {0, 0, 500.0};
        vtkIdType iD = cell->IntersectWithLine(p1, p2, tolerance, t, x, pcoords, subId);
        //vtkIdType iD = polygon->IntersectWithLine(p1, p2, tolerance, t, x, pcoords, subId);
        if (iD)
        {
            std::cout << "intersected? " << iD << std::endl;
            std::cout << "intersection: " << x[0] << " " << x[1] << " " << x[2] << std::endl;
            ids->InsertNextValue(nCellID);
        }

//        vtkTriangle* tcell;
//        double* normal = new double[3];
//        tcell->ComputeNormal(cell->GetPoints()->GetPoint(0),cell->GetPoints()->GetPoint(1),cell->GetPoints()->GetPoint(2),normal);
//        //tcell->Print(cout);
//        cell->Print(cout);
//        cout << "faces:" << cell->GetNumberOfFaces();
//        cout << normal[0] << " " << normal[1] << " " << normal[2] << " " << endl;

    }
	cellSelectionNode->SetSelectionList(ids);
	vtkSmartPointer<vtkSelection> cellSelection = vtkSmartPointer<vtkSelection>::New();
    cellSelection->AddNode(cellSelectionNode);
    // cout << "Selected Cells: " << cellSelection->GetNode(0)->GetSelectionList()->GetNumberOfTuples() << endl;
    // cellSelection->Print(cout);

	// Extract Selection
	// doesn't work vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
	vtkSmartPointer<vtkExtractSelectedPolyDataIds> extractSelection = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
	extractSelection->SetInput(0, polydata);		// The dataset is given on its first input port
	extractSelection->SetInput(1, cellSelection);	// The subset is described by the contents of the vtkSelection on its second input port
	extractSelection->Update();
    //extractSelection->Print(cout);

	vtkSmartPointer<vtkPolyData> selectedPolydata = vtkSmartPointer<vtkPolyData>::New();
	selectedPolydata->ShallowCopy(extractSelection->GetOutput());
    //selectedPolydata->Print(cout);

    // test line
//     Create two points, P1 and P2
    double p1[3] = {0.0, 0, 0.0};
    double p2[3] = {0.0, 0, 250.0};
    vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
    lineSource->SetPoint1(p1);
    lineSource->SetPoint2(p2);
    lineSource->Update();
    vtkSmartPointer<vtkPolyDataMapper> linemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    linemapper->SetInputConnection(lineSource->GetOutputPort());
    vtkSmartPointer<vtkActor> lineactor = vtkSmartPointer<vtkActor>::New();
    lineactor->SetMapper(linemapper);
    lineactor->GetProperty()->SetLineWidth(1);

    // Visualise
    vtkSmartPointer<vtkPolyDataMapper> selectedPolydataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	selectedPolydataMapper->SetInput(selectedPolydata);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    //actor->SetMapper(mapper);
    actor->SetMapper(selectedPolydataMapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);
    renderer->AddActor(lineactor);
    renderer->SetBackground(.3, .6, .3); // Background color green

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
