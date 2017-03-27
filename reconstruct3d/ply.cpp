#include "ply.hpp"
#include <iostream>
#include <fstream>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

/*
 * write Z data to ply
 */
bool to_ply(string filename, InputArray _depth, InputArray _color)
{
	assert(_depth.rows() == _color.rows());
	assert(_depth.cols() == _color.cols());
	assert(_depth.type() == CV_32FC1);
	assert(_color.type() == CV_32FC1);

	// open file
	ofstream stream(filename);
	if (!stream.is_open()) {
		cerr << "cannot open file" << endl;
		return false;
	}

	// retrieve depth map
	Mat depth = _depth.getMat();
	if (depth.empty()) {
		cerr << "empty matrix" << endl;
		return false;
	}

	// retrieve color map & normzlize
	Mat color;
	normalize(_color, color, 0, 255, NORM_MINMAX);

	// write ply header
	stream << "ply" << endl;
	stream << "format ascii 1.0" << endl;
	stream << "element vertex " << countNonZero(color) << endl;
	stream << "property float x" << endl;
	stream << "property float y" << endl;
	stream << "property float z" << endl;
	stream << "property uchar red" << endl;
	stream << "property uchar green" << endl;
	stream << "property uchar blue" << endl;
	stream << "end_header" << endl;
	
	// write data
	for (int i = 0; i < depth.rows; i++) {
		for (int j = 0; j < depth.cols; j++) {
			if (color.at<float>(i, j) > 0) {
				int c = color.at<float>(i, j);
				stream << i << " " << j << " " << depth.at<float>(i, j) << " " << c << " " << c << " " << c << endl;
			}
		}
	}

	stream.close();
	return true;
}