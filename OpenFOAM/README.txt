Directory for OpenFOAM case generation based on motorBike tutorial

-= VM Config =-

Some notes on creating a virtual machine for running Vespa OpenFOAM cases

Based on Centos 6.3, configured with 10G of RAM (we will make this the min RAM needed as default). Since this is intended to be a vm dedicated to OpenFOAM everthing will be installed in the home dir

1) Install OOpenFOAM as per http://www.openfoam.org/download/source.php, set the users bashrc to automatically source $FOAM_INST_DIR/OpenFOAM-2.1.1/etc/bachrc

more documentation to come...

2) Install CMake...

3) Install VTK 5.8.0...

4) git this repository...

5) run test case...



