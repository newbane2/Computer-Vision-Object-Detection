/*
    Allan Spektor
    Homework 2 Program 4
*/

#include <cstdio>
#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <set>
#include <queue>
#include <fstream>
#include <unordered_map>
#include "image.h"

using namespace std;
using namespace ComputerVisionProjects;

struct Entry
{
    int inertia, area;
    double E, theta, rho, roundness;
    pair<int,int> center, referencePoint;
    tuple<int,int,int> abcValues;
};

void calculateE(Entry & entry);
void drawDot(const Entry & e, Image & image);
pair<int, int> generatePoint(const Entry & entry);
bool loadImageDB(unordered_map<int, Entry> & db, string path);
void calculateSecondMomentWRTCenter(Entry & e, Image & image);
bool isInImageDB(const Entry & e, unordered_map<int, Entry> & db);
void computeEverything(unordered_map<int, Entry> & db, Image & image);
void dumpObjectModelsIntoFile(string path, unordered_map<int, Entry> & db);

int main(int argc, char **argv) 
{
	if (argc != 4) {
		printf("Usage: %s file1 file2\n", argv[0]);
		return 0;
	}

	const string input_file(argv[1]);
	const string db_file(argv[2]);
	const string output_file(argv[3]);

	Image image;

	if (!ReadImage(input_file, &image)) {
		cout << "Can't open file " << input_file << endl;
		return 0;
	}

    unordered_map<int, Entry> db, loadedDB;
   
    if(!loadImageDB(loadedDB, db_file))
    {
        cout << "failed to load the DB" << endl;
        abort();
    }

    computeEverything(db, image);

    //after computing E and Theta
    for(pair<int,Entry> p : db)
    {
        auto secondPoint = generatePoint(p.second);
        int cFirst = p.second.center.first;
        int cSecond = p.second.center.second;
        if(!isInImageDB(p.second, loadedDB)) continue; //if the object not in the db continue, else draw line / dot
        DrawLine(secondPoint.first, secondPoint.second, cFirst, cSecond, image.num_gray_levels(), &image);
        drawDot(p.second, image);
    }

	if (!WriteImage(output_file, image)) {
		cout << "Can't write to file " << output_file << endl;
		return 0;
	}

    cout << "done" << endl;
}

//goes through the image and calculates the area, center, and then the Theta and E
void computeEverything(unordered_map<int, Entry> & db, Image & image)
{
    for(int i = 0; i < image.num_rows(); i++)
    {
        for(int j = 0; j < image.num_columns(); j++)
        {
            int gv = image.GetPixel(i, j);
            if(gv != 0)
            {
                if(db.count(gv) < 1) //if no label in map add it
                {
                    db.insert(pair<int,Entry>(gv,Entry()));
                    db.at(gv).referencePoint = pair<int,int>(i,j); //record a reference point for easy object access later
                }
                db.at(gv).area++; //"count the tiles"
                db.at(gv).center.first += i;
                db.at(gv).center.second += j;
            }
        }
    }

    //calculate everything at once
    for(auto& p : db)
    {
        //get final center, note: center can be outside of the actual object, which is why ref point is needed
        int & cFirst = p.second.center.first;
        int & cSecond = p.second.center.second;
        cFirst = cFirst / p.second.area;
        cSecond = cSecond / p.second.area;
        calculateSecondMomentWRTCenter(p.second, image);
        calculateE(p.second);
    }    
}

//calcs abc directly instead of doing a'b'c' -> abc, uses a breadth first search + a starting reference point
void calculateSecondMomentWRTCenter(Entry & e, Image & image)
{
    set<pair<int,int>> visited;
    queue<pair<int,int>> coordQueue;
    int & cFirst = e.center.first;
    int & cSecond = e.center.second;
    coordQueue.push(pair<int,int>(e.referencePoint.first, e.referencePoint.second));
    while(!coordQueue.empty())
    {
        //first => i, second => j
        pair<int,int> point = coordQueue.front();
        const int & i = point.first;
        const int & j = point.second;
        coordQueue.pop();

        if(image.GetPixel(i,j) > 0 && visited.count(point) < 1) //if non 0 and not yet visited
        {
            visited.insert(point);

            //calculates a b and c
            get<0>(e.abcValues) += pow(i-cFirst, 2);
            get<1>(e.abcValues) += 2 * ((i-cFirst) * (j - cSecond));
            get<2>(e.abcValues) += pow(j-cSecond, 2);

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

void calculateE(Entry & entry)
{
    const double PI = atan(1)*4;
    int a = get<0>(entry.abcValues), b = get<1>(entry.abcValues), c = get<2>(entry.abcValues);
    //double theta = asin(b / sqrt(pow(b,2) + pow(a-c,2))) / 2; entry.theta = theta; //in radians
    double theta = atan2(b,a-c) / 2; entry.theta = theta;
    entry.rho = entry.center.second * cos(theta) - entry.center.first * sin(theta); //get rho
    entry.E = a * pow(sin(theta),2) - b * sin(theta) * cos(theta) + c * pow(cos(theta),2); //atan(b / (a-c)) / 2 //a * pow(sin(theta),2) - b * sin(theta) * cos(theta) + c * pow(cos(theta),2);
    double Emax = a * pow(sin(theta + (PI / 2)),2) - b * sin(theta + (PI/2)) * cos(theta + (PI/2)) + c * pow(cos(theta + (PI / 2)),2);
    entry.roundness = entry.E / Emax;
}

pair<int, int> generatePoint(const Entry & entry)
{
    pair<int,int> ret;
    int segmentLength = 50;

    //point generation using the calculated rho;
    // ret.first = entry.center.first - segmentLength;
    // ret.second = (ret.first * sin(entry.theta) + entry.rho) / cos(entry.theta);

    //point generation using only the theta; same result, but looks cleaner, less rounding errors?
    ret.first = entry.center.first - segmentLength * cos(entry.theta);
    ret.second = entry.center.second - segmentLength * sin(entry.theta);
    
    return ret;
}

//dumps the database to a output file
void dumpObjectModelsIntoFile(string path, unordered_map<int, Entry> & db)
{
    ofstream outFile(path.c_str());
    if(!outFile.fail())
    {
        for(auto i : db)
        {
            outFile << i.first << " "
                    << i.second.center.first << " "
                    << i.second.center.second << " "
                    << i.second.E << " "
                    << i.second.theta << " "
                    << i.second.roundness << endl;
        }
        outFile.close();
    }
    else
    {
        cout << "Error saving object model unable to create file at : " << path << endl; 
    }
}

//draws a 3x3 dot at the center
void drawDot(const Entry & e, Image & image)
{
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            image.SetPixel(e.center.first + i, e.center.second + j, image.num_gray_levels());
            image.SetPixel(e.center.first + i, e.center.second - j, image.num_gray_levels());
            image.SetPixel(e.center.first - i, e.center.second + j, image.num_gray_levels());
            image.SetPixel(e.center.first - i, e.center.second - j, image.num_gray_levels());
        }
    }
}

//loads the image
bool loadImageDB(unordered_map<int, Entry> & db, string path)
{
    ifstream ifile(path.c_str());
    if(ifile.fail()) return false;

    int label; //label, center(i,j), E, roundness
    while(ifile >> label)
    {
        db.insert(pair<int,Entry>(label,Entry()));
        ifile >> db.at(label).center.first >> db.at(label).center.second >>
        db.at(label).E >> db.at(label).theta >> db.at(label).roundness;
    }
    return true;
}

//uses % difference of the inertia and roundness to determine identity
bool isInImageDB(const Entry & e, unordered_map<int, Entry> & db)
{
    for(auto i : db)
    {
        const double & iOri = e.E, iDB = i.second.E, rOri = e.roundness, rDB = i.second.roundness;
        double perDiffInertia = abs(iOri - iDB) / ((iOri + iDB) / 2) * 100;
        double perDiffRoundness = abs(rOri - rDB) / ((rOri + rDB) / 2) * 100;
		if (perDiffInertia < 1 && perDiffRoundness < 1) return true;
        //cout << "label : " << i.first << " diffInertia : " << perDiffInertia << " , diffRoundness : " << perDiffRoundness << endl;
    }
    return false;
}
