#include <vtkSTLWriter.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkAppendPolyData.h>

// C/C++
using namespace std;

// A util to combine a number of STL files into one
// The first parameter is the name of the output STL
// The next parameters are a list of STL files that get added to the output
// Ref: http://www.vtk.org/Wiki/VTK/Examples/Cxx/Filtering/CombinePolydata

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        std::cout << "Required parameters: output.stl 1.stl 2.stl ..." << std::endl;
		return EXIT_FAILURE;
    }

    string outputfile = argv[1];
    vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();

    // assemble files into one starting at second argument
    for (int file = 2; file < argc; file++)
    {
        // read STL
        vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
        string inputfile = argv[file];
        cout << "Reading: " << inputfile.c_str() << endl;
        stlReader->SetFileName(inputfile.c_str());
        stlReader->Update();

        // Add this mesh to everything else
        appendFilter->AddInput(stlReader->GetOutput());
        appendFilter->Update();
    }

	// write STL
	vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
	stlWriter->SetFileName(outputfile.c_str());
    stlWriter->SetInputConnection(appendFilter->GetOutputPort());
	stlWriter->Update();
    cout << "Writing: " << outputfile.c_str() << endl;
	stlWriter->Write();

    return EXIT_SUCCESS;
}
