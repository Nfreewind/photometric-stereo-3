#include <iostream>
#include <fstream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "getopt.h"
#include "ply.h"

using namespace std;
using namespace cv;

/*
 * main
 *
 * arguments:
 *  [OPTION] -o [NAME]	output filename
 *  [OPTION] -s			enable show result
 *  [REQUIRE]	[NAME]	input directory
 */
int main(const int argc, char* const argv[]) {
	// parameters
	string folder_input;
	string filename_output = "result.ply";
	bool should_show_result = false;

	// parsing arguments
	int c;
	while ((c = getopt(argc, argv, "d:o:s")) != -1) {
		switch (c) {
		case 'o':
			filename_output = optarg;
			break;
		case 's':
			should_show_result = true;
			break;
		}
	}

	if (optind >= argc) {
		cerr << "input soucre not specific" << endl;
		return 1;
	}
	else {
		folder_input = argv[optind];
	}

	// read data
	ifstream file_light(folder_input + "\\LightSource.txt");
	string line;

	vector<pair<Vec3i, Mat>> data;
	while (getline(file_light, line)) {
		// parsing light source file
		int i;
		Vec3i pt;
		sscanf_s(line.c_str(), "pic%d: (%d,%d,%d)", &i, &pt[0], &pt[1], &pt[2]);

		// read image
		string fn("pic" + to_string(i) + ".bmp");
		auto mat = cv::imread(fn, cv::IMREAD_GRAYSCALE);
		data.push_back(make_pair(pt, mat));
	}


	system("pause");
	return 0;
}