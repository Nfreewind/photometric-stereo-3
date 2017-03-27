#include "depth.hpp"
#include <cmath>

using namespace std;
using namespace cv;

typedef vector<int> range_t;
typedef range_t& range;

/*
 * create a vecotr filled with ordered number
 *
 * @param start		start number, included
 * @param end		end number, excluded
 * @param step		step size (optional)
 */
range_t make_range(const int start, const int end, const int step = 1) {
	// calculate element count
	int count = (float)abs(end - start) / abs(step) + .99;

	// generate serial
	range_t nums(count);
	if (start < end) {
		for (int i = 0, n = start; n < end; i++, n += step) {
			nums[i] = n;
		}
	}
	else {
		for (int i = 0, n = start; n > end; i++, n += step) {
			nums[i] = n;
		}
	}

	return nums;
}

/*
 * sub-func for `integral`
 * integral image accroding to gradient, iter along rows than cols
 */
void integral_rtc(Mat& depth, const Mat& grd_x, const Mat& grd_y, range rang_x, range rang_y) {
	auto last_i = rang_y[0];

	// iter over rows
	for (auto r = rang_y.begin(); r != rang_y.end(); r++) {
		auto last_j = rang_x[0];

		// set value for first column
		const int i = *r;
		depth.at<float>(i, last_j) = depth.at<float>(last_i, last_j) + grd_y.at<float>(i, last_j);
		last_i = i;

		// iter over cols
		for (auto c = rang_x.begin() + 1; c != rang_x.end(); c++) {
			const int j = *c;
			depth.at<float>(i, j) = depth.at<float>(i, last_j) + grd_x.at<float>(i, j);
			last_j = j;
		}
	}
}

/*
 * sub-func for `integral`
 * integral image accroding to gradient, iter along cols than rows
 */
void integral_ctr(Mat& depth, const Mat& grd_x, const Mat& grd_y, range rang_x, range rang_y) {
	auto last_j = rang_x[0];

	// iter over cols
	for (auto c = rang_x.begin(); c != rang_x.end(); c++) {
		auto last_i = rang_y[0];

		// set value for first row
		const int j = *c;
		depth.at<float>(last_i, j) = depth.at<float>(last_i, last_j) + grd_y.at<float>(last_i, j);
		last_j = j;

		// iter over rows
		for (auto r = rang_y.begin() + 1; r != rang_y.end(); r++) {
			const int i = *r;
			depth.at<float>(i, j) = depth.at<float>(last_i, j) + grd_x.at<float>(i, j);
			last_i = i;
		}
	}
}

/*
 * integral image accroding to gradient
 *
 * @param depth		depth map
 * @param grd_x		gradient map on x-direction
 * @param grd_y		gradient map on y-direction
 * @param rang_x	iterate range along x-axis
 * @param rang_x	iterate range along y-axis
 */
void integral(Mat& depth, const Mat& grd_x, const Mat& grd_y, range rang_x, range rang_y) {
	integral_rtc(depth, grd_x, grd_y, rang_x, rang_y);
	integral_ctr(depth, grd_x, grd_y, rang_x, rang_y);
}

/*
 * surface reconstruct from normal vectors
 */
void calculate_depth(InputArray _normal, OutputArray _depth) {
	// split channels
	vector<Mat> normals;
	split(_normal, normals);

	// diff
	Mat grd_x, grd_y;
	divide(normals[0], normals[2], grd_x); // df/dx
	divide(normals[1], normals[2], grd_y); // df/dy

	grd_x = -grd_x;
	grd_y = -grd_y;

	// integral for depths from center
	Mat depth(grd_y.rows, grd_y.cols, CV_32FC1);
	const auto ctr_h = grd_y.rows >> 1;
	const auto ctr_w = grd_y.cols >> 1;

	// init value
	depth.at<float>(ctr_h, ctr_w) = 0;

	auto range_east = make_range(ctr_w, grd_y.cols);
	auto range_west = make_range(ctr_w, -1, -1);

	auto range_south = make_range(ctr_h, grd_y.rows);
	auto range_north = make_range(ctr_h, -1, -1);

	integral(depth, grd_x, grd_y, range_east, range_north); // south-east
	integral(depth, grd_x, grd_y, range_east, range_south); // north-east
	integral(depth, grd_x, grd_y, range_west, range_north); // south-west
	integral(depth, grd_x, grd_y, range_west, range_south); // north-west

	depth.copyTo(_depth);
}