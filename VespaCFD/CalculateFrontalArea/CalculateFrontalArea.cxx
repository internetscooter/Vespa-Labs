// VTK
#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>
#include <vtkCamera.h>
#include <vtkImageData.h>
#include <vtkImageMagnitude.h>

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

int main(int argc, char *argv[])
{
	// check and get the stl input file provided
    if ( argc != 2 )
    {
        cout << "Required parameters: Filename" << endl;
        return EXIT_FAILURE;
    }
    std::string inputfile = argv[1];

    // Read STL
    std::cout << "Reading: " << inputfile << std::endl;
    vtkSmartPointer<vtkSTLReader> stlReader = vtkSmartPointer<vtkSTLReader>::New();
    stlReader->SetFileName(inputfile.c_str());
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New(); 
	polydata = stlReader->GetOutput();
    polydata->Update();

    // makes sense to only scan in an area the object exists, the bounding box will tell us this
    double centre[3];
    double bounds[6];
    boundingBox boxBounds;
    polydata->GetBounds(bounds);
    polydata->GetCenter(centre);
    boxBounds.xmin = bounds[0];
    boxBounds.xmax = bounds[1];
    boxBounds.ymin = bounds[2];
    boxBounds.ymax = bounds[3];
    boxBounds.zmin = bounds[4];
    boxBounds.zmax = bounds[5];

    //    Debug info if needed:
    cout  << "xmin: " << boxBounds.xmin << " "
          << "xmax: " << boxBounds.xmax << endl
          << "ymin: " << boxBounds.ymin << " "
          << "ymax: " << boxBounds.ymax << endl
          << "zmin: " << boxBounds.zmin << " "
          << "zmax: " << boxBounds.zmax << endl;

    cout << "position x:" << centre[0] <<  " "
         << " y:" << centre[1] << " z:" << centre[2] << endl;

    // Visualise
    vtkSmartPointer<vtkPolyDataMapper> polydataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    polydataMapper->SetInput(polydata);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(polydataMapper);

//    // Description:
//    // Set/Get the scaling used for a parallel projection, i.e. the height
//    // of the viewport in world-coordinate distances. The default is 1.
//    // Note that the "scale" parameter works as an "inverse scale" ---
//    // larger numbers produce smaller images.
//    // This method has no effect in perspective projection mode.
//    void SetParallelScale(double scale);
//    vtkGetMacro(ParallelScale,double);

//    How camera is configured from:
//    D:\vtk-5.8.0\Rendering\vtkCamera.cxx
//    if ( this->ParallelProjection)
//      {
//      // set up a rectangular parallelipiped

//      double width = this->ParallelScale * aspect;
//      double height = this->ParallelScale;

//      double xmin = ( this->WindowCenter[0] - 1.0 ) * width;
//      double xmax = ( this->WindowCenter[0] + 1.0 ) * width;
//      double ymin = ( this->WindowCenter[1] - 1.0 ) * height;
//      double ymax = ( this->WindowCenter[1] + 1.0 ) * height;

//      this->ProjectionTransform->Ortho( xmin, xmax, ymin, ymax,
//                                        this->ClippingRange[0],
//                                        this->ClippingRange[1] );
//      }


    // Camera
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetFocalPoint(centre);
    camera->SetPosition(centre[0],centre[1]+1000.33432,centre[2]);
    camera->SetParallelProjection(1);
    camera->SetParallelScale(0.05);
    //camera->SetThickness(0.2);
    camera->SetClippingRange(camera->GetDistance()+boxBounds.zmax, 0.2);
    camera->SetViewUp(0,0,1);
    //camera->SetParallelScale(0.5*boxBounds.ymax-boxBounds.ymin);
    //camera->SetDistance(1000);
    // need to calculate centre of thing and how far back we need to get
    camera->Print(cout);
    // Render
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1); // Background color white
    renderer->SetAutomaticLightCreation(0);// Turn off the lights so the object is black
    renderer->SetActiveCamera(camera);

    //renderer->ResetCamera(bounds);
    //camera->Print(cout);

    // Render Window
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->SetAlphaBitPlanes(1); //enable usage of alpha channel
    renderWindow->Render();

    // Screenshot
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->SetMagnification(10); //set the resolution of the output image (3 times the current resolution of vtk render window)
    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->Update();

    // Create an image data
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData = windowToImageFilter->GetOutput();
    int* dims = imageData->GetDimensions();

//    std::cout << "Dims: " << " x: " << dims[0] << " y: " << dims[1] << " z: " << dims[2] << std::endl;
//    std::cout << "Number of points: " << imageData->GetNumberOfPoints() << std::endl;
//    std::cout << "Number of cells: " << imageData->GetNumberOfCells() << std::endl;
//    imageData->Print(cout);

    cout << imageData->GetScalarTypeAsString();

    double area = 0;

    // Retrieve the entries from the image data and print them to the screen
    for (int z = 0; z < dims[2]; z++)
      {
      for (int y = 0; y < dims[1]; y++)
        {
        for (int x = 0; x < dims[0]; x++)
          {
           float pixel = imageData->GetScalarComponentAsFloat(x,y,z,0);
           if (pixel == 255)
           {
               // cout << " ";
           }
           else
           {
               // cout << "#";
               area++;
           }
          // std::cout << pixel;// << " ";
          }
        //std::cout << std::endl;
        }
      //std::cout << std::endl;
      }

    cout << "area: " << area << endl;

    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName("screenshot2.png");
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();

    renderWindow->Render();
    //renderer->ResetCamera(bounds);
    //renderer->ResetCamera();
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
