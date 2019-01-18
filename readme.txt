The goal is to develop a vision system that recognizes two–dimensional objects in pgm images. 
The two objects we are interested in are shown in the image two objects.pgm. Given an image such as
many objects 1.pgm, use the vision system to determine if the two objects are in
the image and if so compute their positions and orientations.

Usage Information:

To compile just use make all

To run the program you need to do
./program1.exe inputfile threshold(as int) outputfile1
./program2.exe outputfile1 outputfile2
./program3.exe outputfile2 databasepath outputfile3
./program4.exe _outputfile2 databasepath outputfile3 //_outputfile2 is a different labeled object

Images used:

two_objects.pgm
many_objects.pgm
many_objects_1.pgm
many_objects_2.pgm
