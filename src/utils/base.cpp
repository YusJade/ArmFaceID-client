#include "utils/base.h"

#include <qimage.h>

#include <QPainter>

#include <opencv2/imgproc.hpp>

using namespace arm_face_id::utils;

QImage arm_face_id::utils::mat_to_qimage(const cv::Mat &mat) {
  if (mat.empty()) {
    QImage img;
    QPainter painter(&img);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, 1000, 1000);
    return img;
  }

  switch (mat.type()) {
    // 8-bit, 1 channel
    case CV_8UC1: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                   QImage::Format_Grayscale8);
      return image.copy();
    }
    // 8-bit, 3 channels
    case CV_8UC3: {
      cv::Mat mat_tmp;
      cv::cvtColor(mat, mat_tmp, cv::COLOR_BGR2RGB);
      QImage image(mat_tmp.data, mat_tmp.cols, mat_tmp.rows,
                   static_cast<int>(mat_tmp.step), QImage::Format_RGB888);
      return image.copy();
    }
    // 8-bit, 4 channels
    case CV_8UC4: {
      QImage image(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step),
                   QImage::Format_RGBA8888);
      return image.copy();
    }
    default: {
      // 未知格式
      return QImage();
    }
  }
}