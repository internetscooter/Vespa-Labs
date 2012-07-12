#include <vtkPolyData.h>
#include <vtkSTLWriter.h>
#include <vtkSTLReader.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

// A simple util based on VTK to move STL objects in space
// move an object up by 0.26 metres in the y axis.
// this is hard coded at the moment but should be easy to pass translations
// as arguements.

int main(int argc, char *argv[])
{
	if(argc != 3)
    {
		std::cout << "Required parameters: input.stl output.stl" << std::endl;
		return EXIT_FAILURE;
    }

	std::string inputfile = argv[1];
	std::string outputfile = argv[2];

	// read STL
	std::cout << "Reading: " << inputfile << std::endl;
	vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
	stlReader->SetFileName(inputfile.c_str());
	stlReader->Update();

	// tranform up by 0.26 metres
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Translate(0,0,0.26);	

	// apply transform to STL
	vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	transformFilter->SetInputConnection(stlReader->GetOutputPort());
	transformFilter->SetTransform(transform);

	// write STL
	vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
	stlWriter->SetFileName(outputfile.c_str());
	stlWriter->SetInputConnection(transformFilter->GetOutputPort());
	stlWriter->Update();
	stlWriter->Write();

    return EXIT_SUCCESS;
}
