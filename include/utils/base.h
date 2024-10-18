#pragma once

#include <qimage.h>

#include <opencv2/core/mat.hpp>

namespace arm_face_id {
namespace utils {
QImage mat_to_qimage(const cv::Mat &);
}
}  // namespace arm_face_id