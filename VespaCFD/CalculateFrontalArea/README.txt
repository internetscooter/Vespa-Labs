CalculateFrontalArea: Takes an STL file and calculates the frontal area from a specified direction.

Build

From this dir...

mkdir BUILD
cd BUILD
cmake ../
make

Usage

e.g. CalculateFrontalArea.exe D:\scooter\vespalabs\openfoam\motorBike\constant\triSurface\motorBike.stl" 48 1000 0 0"

Required parameters:
Filename (Full path to stl file e.g. something.stl):  motorBike.stl
Magnification (Image resolution size multipler e.g. 2): 48
x (x camera offset position from centre): 1000
y (y camera offset position from centre): 0
z (z camera offset position from centre): 0 
Also it will generate a screen shot used for the calculation

Example stl files included to test out
