#pragma once

#include "ATen/TensorImpl.h"
#include <sstream>

namespace at {

static inline int64_t maybe_wrap_dim(int64_t dim, int64_t dim_post_expr, bool wrap_scalar=true) {
  if (dim_post_expr <= 0) {
    if (!wrap_scalar) {
      std::ostringstream oss;
      oss << "dimension specified as " << dim << " but tensor has no dimensions";
      throw std::runtime_error(oss.str());
    }
    dim_post_expr = 1; // this will make range [-1, 0]
  }

  int64_t min = -dim_post_expr;
  int64_t max = dim_post_expr - 1;
  if (dim < min || dim > max) {
    std::ostringstream oss;
    oss << "dimension out of range (expected to be in range of [" << min
        << ", " << max << "], but got " << dim << ")",
    throw std::runtime_error(oss.str());
  }
  if (dim < 0) dim += dim_post_expr;
  return dim;
}

static inline int64_t maybe_wrap_dim(int64_t dim, TensorImpl *tensor) {
  return maybe_wrap_dim(dim, tensor->dim());
}

static inline int64_t maybe_wrap_dim(int64_t dim, TensorList tensors) {
  if (tensors.size() == 0) {
    // can't wrap empty TensorList; rely on underlying implementation to throw error if necessary.
    return dim;
  }
  return maybe_wrap_dim(dim, tensors[0].dim());
}

static inline int64_t maybe_wrap_dim(int64_t dim, const std::vector<std::vector<int64_t>> & tensor_sizes) {
  if (tensor_sizes.size() == 0) {
    // can't wrap empty list; rely on underlying implementation to throw error if necessary
    return dim;
  }
  return maybe_wrap_dim(dim, tensor_sizes[0].size());
}

// previously, size [0] tensors were the only possible empty tensors; thus, it wasn't possible
// to cat empty tensors unless all the other tensors were 1-dimensional, so we allowed these tensors
// to be "skipped" (both for wrap dimension behavior and dimension size checking).
// We maintain this behavior for backwards compatibility, but only for this specific size
// (i.e. other empty sizes are not skipped).
static inline int64_t legacy_cat_wrap_dim(int64_t dim, const std::vector<std::vector<int64_t>>& tensor_sizes) {
  for (auto& sizes : tensor_sizes) {
    if (sizes == std::vector<int64_t>({0})) {
      continue;
    }
    return maybe_wrap_dim(dim, sizes.size());
  }
  return dim;
}

static inline int64_t legacy_cat_wrap_dim(int64_t dim, TensorList tensors) {
  for (auto& tensor : tensors) {
    if (tensor.dim() == 1 && tensor.sizes()[0] == 0) {
      continue;
    }
    return maybe_wrap_dim(dim, tensor.dim());
  }
  return dim;
}

}
