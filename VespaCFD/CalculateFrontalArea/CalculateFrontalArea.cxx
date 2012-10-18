#include <vtkPolyData.h>
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

// C++
#include <iostream>
using namespace std;

// soon to be updated - just a git test for the moment
int main(int argc, char *argv[])
{
	// check and get the stl input file provided
    if ( argc != 2 )
    {
        cout << "Required parameters: Filename" << endl;
        return EXIT_FAILURE;
    }
    std::string inputfile = argv[1];

    // read STL and print out some info
    std::cout << "Reading: " << inputfile << std::endl;
    vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
    stlReader->SetFileName(inputfile.c_str());
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New(); 
	polydata = stlReader->GetOutput();
    polydata->Update();
    cout << "Cells: " << polydata->GetNumberOfCells() << endl;
    cout << "Points: " << polydata->GetNumberOfPoints() << endl;
    cout << "Polys: " << polydata->GetNumberOfPolys() << endl;
    cout << "Verts: " << polydata->GetNumberOfVerts() << endl;
	polydata->Print(cout);

	// select cells of interest
	// ref: http://www.kitware.com/media/html/SelectionsInVTK.html
    vtkSmartPointer<vtkSelectionNode> cellSelectionNode = vtkSmartPointer<vtkSelectionNode>::New();
	cellSelectionNode->SetFieldType(vtkSelectionNode::CELL);
	cellSelectionNode->SetContentType(vtkSelectionNode::INDICES);

    vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
    ids->SetNumberOfComponents(1);

	int nCellsNum = polydata->GetNumberOfCells();
	for (int nCellID = 0; nCellID < 1 /*nCellsNum/2*/; nCellID++) 
	{ 
		vtkCell* cell; 
		cell = polydata->GetCell(nCellID);
		
		ids->InsertNextValue(nCellID);
	}
	cellSelectionNode->SetSelectionList(ids);
	vtkSmartPointer<vtkSelection> cellSelection = vtkSmartPointer<vtkSelection>::New();
    cellSelection->AddNode(cellSelectionNode);
	cout << "Selected Cells: " << cellSelection->GetNode(0)->GetSelectionList()->GetNumberOfTuples() << endl;
	cellSelection->Print(cout);

	// Extract Selection
	// doesn't work vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
	vtkSmartPointer<vtkExtractSelectedPolyDataIds> extractSelection = vtkSmartPointer<vtkExtractSelectedPolyDataIds>::New();
	extractSelection->SetInput(0, polydata);		// The dataset is given on its first input port
	extractSelection->SetInput(1, cellSelection);	// The subset is described by the contents of the vtkSelection on its second input port
	extractSelection->Update();
	extractSelection->Print(cout);

	vtkSmartPointer<vtkPolyData> selectedPolydata = vtkSmartPointer<vtkPolyData>::New();
	selectedPolydata->ShallowCopy(extractSelection->GetOutput());
	selectedPolydata->Print(cout);

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
    renderer->SetBackground(.3, .6, .3); // Background color green

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
