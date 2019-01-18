/*
    Allan Spektor
    Homework 2 Program 4
*/

#include <cstdio>
#include <iostream>
#include <string>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

int main(int argc, char **argv) 
{

	if (argc != 4) {
		printf("Usage: %s file1 file2 file3\n", argv[0]);
		return 0;
	}
	
	const string input_file(argv[1]);
	const int threshold = stoi(argv[2]);
	const string output_file(argv[3]);

	Image image;

	if (!ReadImage(input_file, &image)) {
		cout << "Can't open file " << input_file << endl;
		return 0;
	}

	//iterates through image and sets pixel to 0 or 255 based on threshold
	for (unsigned int i = 0; i < image.num_rows(); i++)
	{
		for (unsigned int j = 0; j < image.num_columns(); j++) 
		{
			if (image.GetPixel(i, j) < threshold) image.SetPixel(i, j, 0);
			else image.SetPixel(i, j, 255);
		}
	}

	if (!WriteImage(output_file, image)) {
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}

	cout << "done" << endl;
}
