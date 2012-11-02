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

  This calculates the frontal area of a mesh based on a camera position provided by the user. There is a lot
  of code that could be improved but for my purposes it does the trick so I will leave as is.

  Here's how it works:
  1) We load an STL and work out the dimensions of the mesh to place it nicely in the scene
  2) We place a parallel projection camera at some offset from the centre of the mesh. The offset number just needs
     to be big so that you are looking at the parallel projection from a distance and direction you wish to
     calculate the frontal area.
  3) The VTK camera scale is done in real world sizes and is above+below the camera centre point. Setting a
     size of 1 (metre) makes a window which is 2 (metres) high. Note: Units are "whatever" i.e. could be 1 mm/m/nm
  4) We then take a screenshot at some resolution magnification, the bigger the resolution the more accurate, however
     the slower the calculation (we just count pixels one by one - anything that is black is a hit).
  5) Once we have the pixel count we can calculate area using the camera scale and the number of overall

  e.g. On Windows
  "CalculateFrontalArea.exe D:\scooter\vespalabs\openfoam\motorBike\constant\triSurface\motorBike.stl" 48 1000 0 0

  */

int main(int argc, char *argv[])
{
    // check and get options provided
    if ( argc != 6 )
    {
        cout << "Required parameters:" << endl;
        cout << "  Filename (Full path to stl file e.g. something.stl)" << endl;
        cout << "  Magnification (Image resolution size multipler e.g. 2)" << endl;
        cout << "  x (x camera offset position from centre)" << endl;
        cout << "  y (y camera offset position from centre)" << endl;
        cout << "  z (z camera offset position from centre)" << endl;

        return EXIT_FAILURE;
    }
    std::string inputfile = argv[1];
    double magnification = atof (argv[2]);
    double cameraxOffset = atof (argv[3]);
    double camerayOffset = atof (argv[4]);
    double camerazOffset = atof (argv[5]);

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

    // work out good camera height
    // Warning!!!: We are assuming something squarish - other shapes may need code changes to suit wide aspects
    double height = boxBounds.xmax - boxBounds.xmin;
    cout << "Bounding box height: " << height << endl;

    //    Debug info if needed:
    cout  << "xmin: " << boxBounds.xmin << " "
          << "xmax: " << boxBounds.xmax << endl
          << "ymin: " << boxBounds.ymin << " "
          << "ymax: " << boxBounds.ymax << endl
          << "zmin: " << boxBounds.zmin << " "
          << "zmax: " << boxBounds.zmax << endl;

    cout << "centre position x:" << centre[0] <<  " " << " y:" << centre[1] << " z:" << centre[2] << endl;

    // Visualise
    vtkSmartPointer<vtkPolyDataMapper> polydataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    polydataMapper->SetInput(polydata);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(polydataMapper);

    // Camera
    vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New();
    camera->SetFocalPoint(centre);
    camera->SetPosition(centre[0] + cameraxOffset,centre[1] + camerayOffset,centre[2] + camerazOffset);
    camera->SetParallelProjection(1);
    camera->SetParallelScale(height/1.8); // make camera size close to but a bit bigger than object size
    camera->SetClippingRange(camera->GetDistance()+boxBounds.zmax, 0.2);
    camera->SetViewUp(0,0,1);

    // Render
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1); // Background color white
    renderer->SetAutomaticLightCreation(0);// Turn off the lights so the object is black
    renderer->SetActiveCamera(camera);

    // Render Window
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindow->SetAlphaBitPlanes(1); //enable usage of alpha channel
    // disable display - comment this out to see what VTK sees
    renderWindow->SetOffScreenRendering(1);
    renderWindow->Render();

    // Screenshot
    vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    windowToImageFilter->SetInput(renderWindow);
    windowToImageFilter->SetMagnification(magnification); //set the resolution of the output image (3 times the current resolution of vtk render window)
    windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
    windowToImageFilter->Update();

    // Create an image data
    vtkSmartPointer<vtkImageData> imageData = vtkSmartPointer<vtkImageData>::New();
    imageData = windowToImageFilter->GetOutput();
    int* dims = imageData->GetDimensions();

    // start counting pixels...
    double area = 0;
    // Retrieve the entries from the image data and print them to the screen (if you remove cout comments)
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

    // work out the size of a pixel
    long double res = (camera->GetParallelScale() * 2 * camera->GetParallelScale() * 2) / (dims[0]*dims[0]);
    cout.precision(15);
    cout << "image dims :" << dims[0] << " " << dims[1] << " " << dims[2] << endl;
    cout << "height : " << height << endl;
    cout << "res    : " << res << endl;
    cout << "count  : " << area << endl;
    cout << "area   : " << area * res  << endl;

    // save an image so we can check we got the right angle
    vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
    writer->SetFileName("screenshot2.png");
    writer->SetInputConnection(windowToImageFilter->GetOutputPort());
    writer->Write();

//    renderWindow->Render();
//    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
