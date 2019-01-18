/*
    Allan Spektor
    Homework 2 Program 4
*/

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <set>
#include <queue>
#include <unordered_map>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

void generateObjectLabelMap(int** objectMap, Image & image);
void labelImageFromObjectLabelMap(int** objectMap, Image & image);
void searchAndLabel(int **objectMap, Image & image, pair<int, int> coordinates, int label);

int main(int argc, char **argv) 
{
	if (argc != 3) {
		printf("Usage: %s file1 file2\n", argv[0]);
		return 0;
	}

	const string input_file(argv[1]);
	const string output_file(argv[2]);

	Image image;

	if (!ReadImage(input_file, &image)) {
		cout << "Can't open file " << input_file << endl;
		system("pause");
		return 0;
	}

	//create a temporary 2d matrix
	int **objectMap = new int* [image.num_rows()];
	for (int i = 0; i < image.num_rows(); i++)
	{
		objectMap[i] = new int[image.num_columns()];
		memset(objectMap[i], -1, image.num_columns() * 4); //set it all to black
	}

	//fills a table with the correct labels, will be used to label the image
	generateObjectLabelMap(objectMap, image);
	//uses the objectMap, as a reference to label the image
	labelImageFromObjectLabelMap(objectMap, image);

	if (!WriteImage(output_file, image)) {
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}
	cout << "done" << endl;
}

void generateObjectLabelMap(int** objectMap, Image & image)
{
	int objectLabel = 0;
	//label + create equivalence table
	for (unsigned int i = 0; i < image.num_rows(); i++)
	{
		for (unsigned int j = 0; j < image.num_columns(); j++)
		{
			if (image.GetPixel(i, j) > 0 && objectMap[i][j] == -1) //if you find a unlabeled white tile
			{
				searchAndLabel(objectMap, image, pair<int, int>(i, j), objectLabel++);
			}
		}
	}
}

void searchAndLabel(int **objectMap, Image & image, pair<int, int> coordinates, int label)
{
	queue<pair<int, int>> coordQueue;
	coordQueue.push(coordinates);

	while (!coordQueue.empty())
	{
		//first => i, second => j
		pair<int,int> point = coordQueue.front();
		const int & i = point.first;
		const int & j = point.second;
		coordQueue.pop();

		if (image.GetPixel(i, j) > 0 && objectMap[i][j] == -1)
		{
			objectMap[i][j] = label;

			if (i > 0) //if i > 0 check up
			{
				coordQueue.push(pair<int, int>(i - 1, j));
			}
			if (i < image.num_rows()) //if i > 0 check down
			{
				coordQueue.push(pair<int, int>(i + 1, j));
			}
			if (j > 0) //check left
			{
				coordQueue.push(pair<int, int>(i, j - 1));
			}
			if (j < image.num_columns()) //check right
			{
				coordQueue.push(pair<int, int>(i, j + 1));
			}
		}
	}
}


void labelImageFromObjectLabelMap(int** objectMap, Image & image)
{
	unordered_map<int, int> grayValueTable;
	int valueCounter = 3;
	for (unsigned int i = 0; i < image.num_rows(); i++)
	{
		for (unsigned int j = 0; j < image.num_columns(); j++)
		{
			if (objectMap[i][j] != -1)
			{
				if (grayValueTable.count(objectMap[i][j]) < 1)
					grayValueTable.insert(pair<int, int>(objectMap[i][j], valueCounter++));
					
				image.SetPixel(i, j, grayValueTable.at(objectMap[i][j]));
			}
		}
	}

	image.SetNumberGrayLevels(valueCounter);
}
