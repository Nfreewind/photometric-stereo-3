#include <iostream>
#include <fstream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "getopt.h"
#include "ply.h"

// macro: handling return situation
#define ERROR		(1)
#define SUCCESS		(0)

#ifdef _DEBUG
#define RETURN(x)	{system("pause"); return x;}
#else
#define RETURN(x)	{return x;}
#endif // _DEBUG

// macro: accelerate accessing cv::mat
#define AT_F(A, i)	(((float*)A.data)[i])

// namespace utilize
using namespace std;
using namespace cv;

// prototypes
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
		double min, max;
		minMaxLoc(albedo, &min, &max);
		imshow("albedo", albedo / max);
	}




	if (should_show_result) {
		clog << "process finished. press any key to continue." << endl;
		waitKey();
	}

	RETURN(SUCCESS);
}


/*
 *	calcuate normal map and albedo
 */
void calcualte_normal(vector<pair<Vec3f, Mat>>& data, OutputArray normal, OutputArray albedo) {
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
	normal.create(size, CV_32FC3);
	normal.setTo(0);

	albedo.create(size, CV_32FC1);
	albedo.setTo(0);

	auto N = normal.getMat();
	auto A = albedo.getMat();
	for (int i = 0, r_idx = 0; i < size.height; i++, r_idx += size.width) {
		for (int j = 0; j < size.width; j++) {
			Mat I(n, 1, CV_32FC1);
			for (int k = 0; k < n; k++) {
				AT_F(I, k) = AT_F(data[k].second, r_idx + j);
			}

			Mat b = S_sol *I;
			auto albedo = norm(b, NORM_L2);
			if (albedo > 1E-4) { // eps
				N.at<Vec3f>(i, j) = Mat(b / albedo);
				AT_F(A, r_idx + j) = albedo;
			}
		}
	}
}
