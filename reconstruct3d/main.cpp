#include <iostream>
#include <fstream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "getopt.h"
#include "ply.h"
#include "depth.hpp"

// macro: handling return situation
#define ERROR		(1)
#define SUCCESS		(0)

#ifdef _DEBUG
#define RETURN(x)	{system("pause"); return x;}
#else
#define RETURN(x)	{return x;}
#endif // _DEBUG

// namespace utilize
using namespace std;
using namespace cv;

// prototypes
void plot(string name, InputArray& matrix);
void calcualte_normal(vector<pair<Vec3f, Mat>>& data, OutputArray normal, OutputArray albedo);

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
	while ((c = getopt(argc, argv, "o:s")) != -1) {
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
		RETURN(ERROR);
	}
	else {
		folder_input = argv[optind];
	}

	// read data
	vector<pair<Vec3f, Mat>> data;

	// read light source file
	ifstream file_light(folder_input + "\\LightSource.txt");
	if (!file_light.is_open()) {
		cerr << "light source file not found" << endl;
		RETURN(ERROR);
	}


	string line;
	while (getline(file_light, line)) {
		// parsing light source file
		int i, pt_x, pt_y, pt_z;
		sscanf_s(line.c_str(), "pic%d: (%d,%d,%d)", &i, &pt_x, &pt_y, &pt_z);

		// read image
		Mat mat;

		string fn(folder_input + "\\pic" + to_string(i) + ".bmp");
		imread(fn, IMREAD_GRAYSCALE).convertTo(mat, CV_32FC1);

		// check image loaded
		if (mat.empty()) {
			cerr << "image not found: " << fn << endl;
			RETURN(ERROR);
		}

		// save
		data.push_back(make_pair(Vec3f(pt_x, pt_y, pt_z), mat));
	}

	// calculate normal
	Mat normal, albedo;
	calcualte_normal(data, normal, albedo);

	if (should_show_result) {
		plot("albedo", albedo);
	}

	// calculate depth
	Mat depth;
	calculate_depth(normal, depth);

	// refine dpeth map
	for (int i = 0; i < depth.rows; i++) {
		for (int j = 0; j < depth.cols; j++) {
			if (albedo.at<float>(i, j) == 0) {
				depth.at<float>(i, j) = 0;
			}
		}
	}

	// display depth map
	if (should_show_result) {
		plot("depth", depth);
	}

	// save to ply
	to_ply(filename_output, depth, albedo);

	// return
	if (should_show_result) {
		clog << "process finished. press any key to continue." << endl;
		waitKey();
	}

	RETURN(SUCCESS);
}

/*
 * assisting function for imshow
 * this func normalized the image before display
 */
void plot(string name, InputArray& matrix) {
	Mat A = matrix.getMat();

	double min_val, max_val;
	minMaxLoc(A, &min_val, &max_val);
	imshow(name, (A - min_val) / (max_val - min_val));
}

/*
 *	calcuate normal map and albedo
 */
void calcualte_normal(vector<pair<Vec3f, Mat>>& data, OutputArray _normal, OutputArray _albedo) {
	const auto n = data.size();
	const auto size = data[0].second.size();

	// build light source vector S
	Mat S(n, 3, CV_32FC1);
	for (int i = 0; i < n; i++) {
		S.row(i) = data[i].first;
	}

	Mat S_sol;
	invert(S, S_sol, DECOMP_SVD);

	// build normal map
	_normal.create(size, CV_32FC3);
	_normal.setTo(0);

	_albedo.create(size, CV_32FC1);
	_albedo.setTo(0);

	auto normal = _normal.getMat();
	auto albedo = _albedo.getMat();

	const auto eps = numeric_limits<double>::epsilon();
	for (int i = 0, r_idx = 0; i < size.height; i++, r_idx += size.width) {
		for (int j = 0; j < size.width; j++) {
			Mat I(n, 1, CV_32FC1);
			for (int k = 0; k < n; k++) {
				I.at<float>(k) = data[k].second.at<float>(r_idx + j);
			}

			Mat b = S_sol *I;
			auto albedo_val = norm(b, NORM_L2);

			if (albedo_val > eps) { // floating point inaccuracy
				normal.at<Vec3f>(i, j) = Mat(b / albedo_val);	// N = b / |b|
				albedo.at<float>(r_idx + j) = albedo_val;		// A = |b|
			}
		}
	}
}

