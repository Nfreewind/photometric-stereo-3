#include "ply.h"
#include <iostream>
#include <fstream>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

#define AT(p, i)	((unsigned int)((*p)[i]))	// access value inside Vec* type, and convert to numbeic type to output

using namespace std;
using namespace cv;

/*
 * write Z data to ply
 */
bool to_ply(string filename, InputArray source)
{
	// open file
	ofstream stream(filename);
	if (!stream.is_open()) {
		cerr << "cannot open file" << endl;
		return false;
	}

	// retrieve image
	Mat mat = source.getMat();
	if (mat.empty()) {
		cerr << "empty matrix" << endl;
		return false;
	}

	// write ply header
	stream << "ply" << endl;
	stream << "format ascii 1.0" << endl;
	stream << "element vertex " << (mat.rows * mat.cols) << endl;
	stream << "property float x" << endl;
	stream << "property float y" << endl;
	stream << "property float z" << endl;
	stream << "property uchar red" << endl;
	stream << "property uchar green" << endl;
	stream << "property uchar blue" << endl;
	stream << "end_header" << endl;

	// type casting
	Mat gray;
	Mat data;
	mat.convertTo(gray, CV_8UC1);
	mat.convertTo(data, CV_32FC1);

	// apply color map
	Mat colored;
	applyColorMap(mat, colored, COLORMAP_COOL);

	// write data
	float *pZ = (float*)data.datastart;		// pointer access is much faster than Mat::at<>()
	Vec3b *pC = (Vec3b*)colored.datastart;
	for (int i = 0; i < mat.rows; i++) {
		for (int j = 0; j < mat.cols; j++, pZ++, pC++) {
			stream << i << " " << j << " " << (*pZ) << " " << AT(pC, 0) << " " << AT(pC, 1) << " " << AT(pC, 2) << endl;
		}
	}

	stream.close();
	return true;
}