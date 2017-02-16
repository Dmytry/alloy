/*
 * Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef INCLUDE_ALLOYIMAGEPROCESSING_H_
#define INCLUDE_ALLOYIMAGEPROCESSING_H_
#include "AlloyImage.h"
namespace aly {
bool SANITY_CHECK_IMAGE_PROCESSING();
template<class T, size_t M, size_t N> void GaussianKernel(T (&kernel)[M][N],
		T sigmaX = T(0.607902736 * (M - 1) * 0.5),
		T sigmaY = T(0.607902736 * (N - 1) * 0.5)) {
	T sum = 0;
#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < (int)M; i++) {
		for (int j = 0; j < (int)N; j++) {
			T x = T(i - 0.5 * (M - 1));
			T y = T(j - 0.5 * (N - 1));
			double xn = x / sigmaX;
			double yn = y / sigmaY;
			T w = T(std::exp(-0.5 * (xn * xn + yn * yn)));
			sum += w;
			kernel[i][j] = w;
		}
	}
	sum = T(1) / sum;
#pragma omp parallel for
	for (int i = 0; i < (int)M; i++) {
		for (int j = 0; j < (int)N; j++) {
			kernel[i][j] *= sum;
		}
	}
}

template<class T, size_t M, size_t N> void GaussianKernelDerivative(
		T (&gX)[M][N], T (&gY)[M][N], T sigmaX = T(0.607902736 * (M - 1) * 0.5),
		T sigmaY = T(0.607902736 * (N - 1) * 0.5)) {
	T sum = 0;
#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < (int)M; i++) {
		for (int j = 0; j < (int)N; j++) {
			T x = T(i - 0.5 * (M - 1));
			T y = T(j - 0.5 * (N - 1));
			double xn = x / sigmaX;
			double yn = y / sigmaY;
			T w = T(std::exp(-0.5 * (xn * xn + yn * yn)));
			sum += w;
			gX[i][j] = w * xn / sigmaX;
			gY[i][j] = w * yn / sigmaY;

		}
	}
	sum = T(1) / sum;
#pragma omp parallel for
	for (int i = 0; i < (int)M; i++) {
		for (int j = 0; j < (int)N; j++) {
			gX[i][j] *= sum;
			gY[i][j] *= sum;
		}
	}
}
template<class T, size_t M, size_t N> void GaussianKernelLaplacian(
		T (&kernel)[M][N], T sigmaX = T(0.607902736 * (M - 1) * 0.5), T sigmaY =
				T(0.607902736 * (N - 1) * 0.5)) {
	T sum = 0;
	T sum2 = 0;
#pragma omp parallel for
	for (int i = 0; i < (int)M; i++) {
		for (int j = 0; j < (int)N; j++) {
			T x = T(i - 0.5 * (M - 1));
			T y = T(j - 0.5 * (N - 1));
			double xn = x / sigmaX;
			double yn = y / sigmaY;
			T w = T(std::exp(-0.5 * (xn * xn + yn * yn)));
			sum += w;
			T ww = w
					* (xn * xn / (sigmaX * sigmaX) + yn * yn / (sigmaY * sigmaY)
						   - 1 / (sigmaX * sigmaX)       - 1 / (sigmaY * sigmaY));
			sum2 += ww;
			kernel[i][j] = ww;
		}
	}
	sum = T(1) / sum;
	sum2 /= T(M * N);
	for (int i = 0; i < (int)M; i++) {
		for (int j = 0; j < (int)N; j++) {
			kernel[i][j] = (kernel[i][j] - sum2) * sum;
		}
	}
}
template<class T, size_t M, size_t N> struct GaussianOperators {
	T filter[M][N];
	T filterGradX[M][N];
	T filterGradY[M][N];
	T filterLaplacian[M][N];
	GaussianOperators(T sigmaX = T(0.607902736 * (M - 1) * 0.5),
			T sigmaY = T(0.607902736 * (N - 1) * 0.5)) {
		T sum = 0;
		T sum2 = 0;
		for (int i = 0; i < (int)M; i++) {
			for (int j = 0; j < (int)N; j++) {
				T x = T(i - 0.5 * (M - 1));
				T y = T(j - 0.5 * (N - 1));
				double xn = x / sigmaX;
				double yn = y / sigmaY;
				T w = T(std::exp(-0.5 * (xn * xn + yn * yn)));
				filter[i][j] = w;
				filterGradX[i][j] = w * xn / sigmaX;
				filterGradY[i][j] = w * yn / sigmaY;
				sum += w;
				T ww = w
					* (xn * xn / (sigmaX * sigmaX) + yn * yn / (sigmaY * sigmaY)
						- 1 / (sigmaX * sigmaX) - 1 / (sigmaY * sigmaY));
				sum2 += ww;
				filterLaplacian[i][j] = ww;
			}
		}
		sum = T(1) / sum;
		sum2 /= T(M * N);
		for (int i = 0; i < (int)M; i++) {
			for (int j = 0; j < (int)N; j++) {
				filterLaplacian[i][j] = (filterLaplacian[i][j] - sum2) * sum;
				filter[i][j] *=sum;
				filterGradX[i][j] *= sum;
				filterGradY[i][j] *= sum;
			}
		}
	}
};
template<size_t M, size_t N, class T, int C, ImageType I> void Gradient(
		const Image<T, C, I>& image, Image<T, C, I>& gX, Image<T, C, I>& gY, double sigmaX = (0.607902736 * (M - 1) * 0.5),
	double sigmaY = (0.607902736 * (N - 1) * 0.5)) {

	double filterX[M][N], filterY[M][N];
	GaussianKernelDerivative(filterX, filterY,sigmaX,sigmaY);

	gX.resize(image.width, image.height);
	gY.resize(image.width, image.height);
#pragma omp parallel for
	for (int j = 0; j < image.height; j++) {
		for (int i = 0; i < image.width; i++) {
			vec<double, C> vsumX(0.0);
			vec<double, C> vsumY(0.0);
			for (int ii = 0; ii < (int)M; ii++) {
				for (int jj = 0; jj < (int)N; jj++) {
					vec<T, C> val = image(i + ii - (int)M / 2, j + jj - (int)N / 2);
					vsumX += filterX[ii][jj] * vec<double, C>(val);
					vsumY += filterY[ii][jj] * vec<double, C>(val);
				}
			}
			gX(i, j) = vec<T, C>(vsumX);
			gY(i, j) = vec<T, C>(vsumY);
		}
	}
}
template<size_t M, size_t N, class T, int C, ImageType I> void Laplacian(
		const Image<T, C, I>& image, Image<T, C, I>& L, double sigmaX = (0.607902736 * (M - 1) * 0.5),
	double sigmaY = (0.607902736 * (N - 1) * 0.5)) {
	float filter[M][N];
	GaussianKernelLaplacian(filter,(float)sigmaX,(float)sigmaY);
	L.resize(image.width, image.height);
#pragma omp parallel for
	for (int j = 0; j < image.height; j++) {
		for (int i = 0; i < image.width; i++) {
			vec<float, C> vsum(0.0);
			for (int ii = 0; ii < (int)M; ii++) {
				for (int jj = 0; jj < (int)N; jj++) {
					vec<T, C> val = image(i + ii - (int)M / 2, j + jj - (int)N / 2);
					vsum += filter[ii][jj] * vec<float, C>(val);
				}
			}
			L(i, j) = vec<T, C>(vsum);
		}
	}
}
template<size_t M, size_t N, class T, int C, ImageType I> void Smooth(
		const Image<T, C, I>& image, Image<T, C, I>& B, double sigmaX = (0.607902736 * (M - 1) * 0.5),
	double sigmaY = (0.607902736 * (N - 1) * 0.5)) {
	float filter[M][N];
	GaussianKernel(filter,(float)sigmaX,(float)sigmaY);
	B.resize(image.width, image.height);
#pragma omp parallel for
	for (int j = 0; j < image.height; j++) {
		for (int i = 0; i < image.width; i++) {
			vec<float, C> vsum(0.0);
			for (int ii = 0; ii < (int)M; ii++) {
				for (int jj = 0; jj < (int)N; jj++) {
					vec<T, C> val = image(i + ii - (int)M / 2, j + jj - (int)N / 2);
					vsum += filter[ii][jj] * vec<float, C>(val);
				}
			}
			B(i, j) = vec<T, C>(vsum);
		}
	}
}
template<class T, int C, ImageType I> void Smooth(const Image<T, C, I>& image, Image<T, C, I>& B,double sigmaX,double sigmaY) {
	double sigma = std::max(sigmaX, sigmaY);
	if (sigma < 1.5f) {
		Smooth<3, 3>(image, B,sigmaX, sigmaY);
	}
	else if (sigma < 2.5f) {
		Smooth<5, 5>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 3.5f) {
		Smooth<7, 7>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 5.5f) {
		Smooth<11, 11>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 6.5f) {
		Smooth<13, 13>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 7.5f) {
		Smooth<15, 15>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 8.5f) {
		Smooth<17, 17>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 9.5f) {
		Smooth<19, 19>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 10.5f) {
		Smooth<21, 21>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 11.5f) {
		Smooth<23, 23>(image, B, sigmaX, sigmaY);
	}
	else if (sigma < 12.5f) {
		Smooth<25, 25>(image, B, sigmaX, sigmaY);
	}
}
template<class T, int C, ImageType I> void Smooth3x3(
		const Image<T, C, I>& image, Image<T, C, I>& B) {
	Smooth<3, 3>(image, B);
}
template<class T, int C, ImageType I> void Smooth5x5(
		const Image<T, C, I>& image, Image<T, C, I>& B) {
	Smooth<5, 5>(image, B);
}
template<class T, int C, ImageType I> void Smooth7x7(
		const Image<T, C, I>& image, Image<T, C, I>& B) {
	Smooth<7, 7>(image, B);
}
template<class T, int C, ImageType I> void Smooth11x11(
		const Image<T, C, I>& image, Image<T, C, I>& B) {
	Smooth<11, 11>(image, B);
}

template<class T, int C, ImageType I> void Laplacian3x3(
		const Image<T, C, I>& image, Image<T, C, I>& L) {
	Laplacian<3, 3>(image, L);
}
template<class T, int C, ImageType I> void Laplacian5x5(
		const Image<T, C, I>& image, Image<T, C, I>& L) {
	Laplacian<5, 5>(image, L);
}
template<class T, int C, ImageType I> void Laplacian7x7(
		const Image<T, C, I>& image, Image<T, C, I>& L) {
	Laplacian<7, 7>(image, L);
}
template<class T, int C, ImageType I> void Laplacian11x11(
		const Image<T, C, I>& image, Image<T, C, I>& L) {
	Laplacian<11, 11>(image, L);
}

template<class T, int C, ImageType I> void Gradient3x3(
		const Image<T, C, I>& image, Image<T, C, I>& gX, Image<T, C, I>& gY) {
	Gradient<3, 3>(image, gX, gY);
}
template<class T, int C, ImageType I> void Gradient5x5(
		const Image<T, C, I>& image, Image<T, C, I>& gX, Image<T, C, I>& gY) {
	Gradient<5, 5>(image, gX, gY);
}
template<class T, int C, ImageType I> void Gradient7x7(
		const Image<T, C, I>& image, Image<T, C, I>& gX, Image<T, C, I>& gY) {
	Gradient<7, 7>(image, gX, gY);
}
template<class T, int C, ImageType I> void Gradient11x11(
		const Image<T, C, I>& image, Image<T, C, I>& gX, Image<T, C, I>& gY) {
	Gradient<11, 11>(image, gX, gY);
}

}

#endif /* INCLUDE_CORE_IMAGEPROCESSING_H_ */
