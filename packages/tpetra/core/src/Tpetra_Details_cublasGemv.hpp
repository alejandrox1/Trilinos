/*
//@HEADER
// ************************************************************************
//
//          Kokkos: Node API and Parallel Node Kernels
//              Copyright (2008) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef TPETRA_DETAILS_CUBLASGEMV_HPP
#define TPETRA_DETAILS_CUBLASGEMV_HPP

/// \file Tpetra_Details_cublasGemv.hpp
/// \brief Implementation detail of Tpetra::MultiVector
///
/// \warning This file, and its contents, are an implementation detail
///   of Tpetra::MultiVector.  Either may disappear or change at any
///   time.
///
/// The point of this file is to wrap cuBLAS calls, so that
/// application code is not exposed to cublas.h or cublas_v2.h.  This
/// fixes the following issue relating to conflicts at build time
/// between the old and new cuBLAS API:
///
/// https://github.com/trilinos/Trilinos/issues/1194
///
/// It also generally improves encapsulation.

#include "TpetraCore_config.h"
#include "Tpetra_Details_Blas.hpp"
#include "Tpetra_Details_libGemv.hpp"

namespace Tpetra {
namespace Details {
namespace Blas {
namespace Cublas {

/// \brief For this set of template parameters, can and should we
///   implement Gemv (see below) using cuBLAS?
template<class ViewType1,
         class ViewType2,
         class ViewType3,
         class CoefficientType,
         class IndexType>
struct GemvCanUseCublas {
#ifdef KOKKOS_HAVE_CUDA
  static constexpr bool value =
    ::Tpetra::Details::Blas::Lib::GemvCanUseLib<ViewType1, ViewType2, ViewType3,
                          CoefficientType, IndexType>::value &&
    std::is_same<typename ViewType1::execution_space, ::Kokkos::Cuda>::value &&
    std::is_same<typename ViewType2::execution_space, ::Kokkos::Cuda>::value &&
    std::is_same<typename ViewType3::execution_space, ::Kokkos::Cuda>::value;
#else // NOT KOKKOS_HAVE_CUDA
  static constexpr bool value = false;
#endif // KOKKOS_HAVE_CUDA
};

namespace Impl {

/// \brief Wrapped version of cublasCgemv (v1 API).
///
/// See the cuBLAS documentation for details.
void
cgemv (const char char_trans,
       const int m,
       const int n,
       const ::Kokkos::complex<float>& alpha,
       const ::Kokkos::complex<float> A[],
       const int lda,
       const ::Kokkos::complex<float> x[],
       const int incx,
       const ::Kokkos::complex<float>& beta,
       ::Kokkos::complex<float> y[],
       const int incy);

/// \brief Wrapped version of cublasDgemv (v1 API).
///
/// See the cuBLAS documentation for details.
void
dgemv (const char char_trans,
       const int m,
       const int n,
       const double alpha,
       const double A[],
       const int lda,
       const double x[],
       const int incx,
       const double beta,
       double y[],
       const int incy);

/// \brief Wrapped version of cublasSgemv (v1 API).
///
/// See the cuBLAS documentation for details.
void
sgemv (const char char_trans,
       const int m,
       const int n,
       const float alpha,
       const float A[],
       const int lda,
       const float x[],
       const int incx,
       const float beta,
       float y[],
       const int incy);

/// \brief Wrapped version of cublasZgemv (v1 API).
///
/// See the cuBLAS documentation for details.
void
zgemv (const char char_trans,
       const int m,
       const int n,
       const ::Kokkos::complex<double>& alpha,
       const ::Kokkos::complex<double> A[],
       const int lda,
       const ::Kokkos::complex<double> x[],
       const int incx,
       const ::Kokkos::complex<double>& beta,
       ::Kokkos::complex<double> y[],
       const int incy);

/// \brief Wrapper for the above wrappers, templated on scalar type
///   (the type of each entry in the matrices).
template<class ScalarType> struct Gemv {};

template<>
struct Gemv< ::Kokkos::complex<float> > {
  typedef ::Kokkos::complex<float> scalar_type;

  static void
  gemv (const char trans,
        const int m,
        const int n,
        const scalar_type& alpha,
        const scalar_type A[],
        const int lda,
        const scalar_type x[],
        const int incx,
        const scalar_type& beta,
        scalar_type y[],
        const int incy)
  {
    return cgemv (trans, m, n, alpha, A, lda, x, incx, beta, y, incy);
  }
};

template<>
struct Gemv<double> {
  typedef double scalar_type;

  static void
  gemv (const char trans,
        const int m,
        const int n,
        const scalar_type& alpha,
        const scalar_type A[],
        const int lda,
        const scalar_type x[],
        const int incx,
        const scalar_type& beta,
        scalar_type y[],
        const int incy)
  {
    return dgemv (trans, m, n, alpha, A, lda, x, incx, beta, y, incy);
  }
};

template<>
struct Gemv<float> {
  typedef float scalar_type;

  static void
  gemv (const char trans,
        const int m,
        const int n,
        const scalar_type& alpha,
        const scalar_type A[],
        const int lda,
        const scalar_type x[],
        const int incx,
        const scalar_type& beta,
        scalar_type y[],
        const int incy)
  {
    return sgemv (trans, m, n, alpha, A, lda, x, incx, beta, y, incy);
  }
};

template<>
struct Gemv< ::Kokkos::complex<double> > {
  typedef ::Kokkos::complex<double> scalar_type;

  static void
  gemv (const char trans,
        const int m,
        const int n,
        const scalar_type& alpha,
        const scalar_type A[],
        const int lda,
        const scalar_type x[],
        const int incx,
        const scalar_type& beta,
        scalar_type y[],
        const int incy)
  {
    return zgemv (trans, m, n, alpha, A, lda, x, incx, beta, y, incy);
  }
};

} // namespace Impl

template<class ViewType1,
         class ViewType2,
         class ViewType3,
         class CoefficientType>
static void
gemv (const char trans,
      const CoefficientType& alpha,
      const ViewType1& A,
      const ViewType2& x,
      const CoefficientType& beta,
      const ViewType3& y)
{
  static_assert (std::is_same<typename ViewType1::array_layout, Kokkos::LayoutLeft>::value ||
                 std::is_same<typename ViewType1::array_layout, Kokkos::LayoutRight>::value ||
                 std::is_same<typename ViewType1::array_layout, Kokkos::LayoutStride>::value,
                 "A's layout must be either LayoutLeft, LayoutRight, or LayoutStride.");
  static_assert (std::is_same<typename ViewType2::array_layout, Kokkos::LayoutLeft>::value ||
                 std::is_same<typename ViewType2::array_layout, Kokkos::LayoutRight>::value ||
                 std::is_same<typename ViewType2::array_layout, Kokkos::LayoutStride>::value,
                 "x's layout must be either LayoutLeft, LayoutRight, or LayoutStride.");
  static_assert (std::is_same<typename ViewType3::array_layout, Kokkos::LayoutLeft>::value ||
                 std::is_same<typename ViewType3::array_layout, Kokkos::LayoutRight>::value ||
                 std::is_same<typename ViewType3::array_layout, Kokkos::LayoutStride>::value,
                 "y's layout must be either LayoutLeft, LayoutRight, or LayoutStride.");

  typedef CoefficientType scalar_type;
  typedef Impl::Gemv<scalar_type> impl_type;
  typedef int index_type;

  const index_type lda = getStride2DView<ViewType1, index_type> (A);
  const index_type incx = getStride1DView<ViewType2, index_type> (x);
  const index_type incy = getStride1DView<ViewType2, index_type> (y);
  const index_type m = static_cast<index_type> (A.extent (0));
  const index_type n = static_cast<index_type> (A.extent (1));
  impl_type::gemv (trans, m, n,
                   alpha, A.data (), lda,
                   x.data (), incx,
                   beta, y.data (), incy);
}

} // namespace Cublas
} // namespace Blas
} // namespace Details
} // namespace Tpetra

#endif // TPETRA_DETAILS_CUBLASGEMV_HPP
