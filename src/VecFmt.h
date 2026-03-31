#pragma once
#include <ostream>

template <class Vec> struct VecFmt {
  Vec vec;
  VecFmt(Vec vec) : vec(vec) {}

  friend std::ostream &operator<<(std::ostream &out, const VecFmt &fmt) {
    out << "(";
    for (int i = 0; i < fmt.vec.length(); i++) {
      out << fmt.vec[i];
      if (i + 1 != fmt.vec.length()) {
        out << ", ";
      }
    }
    out << ")";
    return out;
  }
};

template <class Mat> struct MatFmt {
  Mat mat;
  MatFmt(Mat mat) : mat(mat) {}

  friend std::ostream &operator<<(std::ostream &out, const MatFmt &fmt) {
    for (int i = 0; i < fmt.mat[0].length(); i++) {
      out << "\n[ ";
      for (int j = 0; j < fmt.mat.length(); j++) {
        out << fmt.mat[j][i] << " ";
      }
      out << "]";
    }
    return out;
  }
};
