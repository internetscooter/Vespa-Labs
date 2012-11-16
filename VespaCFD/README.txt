Some helper utils for doing stuff with CFD, specifically STL mesh manipulation.


AssembleVespa: Takes a bunch of bits in STL file format, joins them together and outputs a single STL file.

CalculateFrontalArea: Takes an STL file and calculates the frontal area from a specified direction.

TranslateSTL: Takes an STL and shifts its position.

GenerateTearDrop: Generates an aerodynamic teardrop shape given some parameters (work in progress)

 
This uses VTK to do a bunch of stuff. Note that there is a bug in VTK 5.10 with STL handlers. This
is known to work with VTK 5.8.0.

Note that the following option seems to be required to build with Qt OK
//Build with shared libraries.
BUILD_SHARED_LIBS:BOOL=ON