#ifndef __PLY_H
#define __PLY_H

#include <opencv2/core/core.hpp>

bool to_ply(std::string filename, cv::InputArray depth, cv::InputArray color);

#endif // !__PLY_H
