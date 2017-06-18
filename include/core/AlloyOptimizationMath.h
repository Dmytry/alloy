/*
 * Copyright(C) 2016, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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

#ifndef INCLUDE_CORE_ALLOYOPTIMIZATIONMATH_H_
#define INCLUDE_CORE_ALLOYOPTIMIZATIONMATH_H_
#include <cereal/types/list.hpp>
#include "AlloyVector.h"
#include "AlignedAllocator.h"
#include "cereal/types/vector.hpp"
#include "cereal/types/tuple.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/archives/portable_binary.hpp"
#include <vector>
#include <list>
#include <map>
namespace aly {
template<class T> struct Vec {
public:
	std::vector<T, aligned_allocator<T, 64>> data;
	Vec(size_t sz = 0, T value = T(0)) :
			data(sz, value) {
	}
	void set(const T& val) {
		data.assign(data.size(), val);
	}
	inline void set(const Vec<T>& val) {
		this->data = val.data;
	}
	inline void set(const std::vector<T, aligned_allocator<T, 64>>& val) {
		this->data = data;
	}
	template<class Archive>
	void save(Archive & archive) const {
		archive(CEREAL_NVP(data));
	}

	template<class Archive>
	void load(Archive & archive) {
		archive(CEREAL_NVP(data));
	}
	void resize(size_t sz) {
		data.resize(sz);
		data.shrink_to_fit();
	}
	void resize(size_t sz, const T& val) {
		data.resize(sz, val);
		data.shrink_to_fit();
	}
	void append(const T& val) {
		data.push_back(val);
	}
	void push_back(const T& val) {
		data.push_back(val);
	}
	T* ptr() {
		return data.data();
	}
	const T* ptr() const {
		return data.data();
	}
	void setZero() {
		for (T& val : data) {
			val = T(0);
		}
	}
	const T& operator[](const size_t i) const {
		if (i >= data.size())
			throw std::runtime_error(
					MakeString() << "Vector index out of bounds " << i << "/"
							<< data.size());
		return data[i];
	}
	T& operator[](const size_t i) {
		if (i >= data.size())
			throw std::runtime_error(
					MakeString() << "Vector index out of bounds " << i << "/"
							<< data.size());
		return data[i];
	}
	inline void clear() {
		data.clear();
		data.shrink_to_fit();
	}
	size_t size() const {
		return data.size();
	}
};

template<class T> void Transform(Vec<T>& im1, Vec<T>& im2,
		const std::function<void(T&, T&)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (size_t offset = 0; offset < sz; offset++) {
		func(im1[offset], im2[offset]);
	}
}
template<class T> void Transform(Vec<T>& im1,
		const std::function<void(T&)>& func) {
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1[offset]);
	}
}
template<class T> void Transform(Vec<T>& im1, const Vec<T>& im2,
		const std::function<void(T&, const T&)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1[offset], im2[offset]);
	}
}
template<class T> void Transform(Vec<T>& im1, const Vec<T>& im2,
		const Vec<T>& im3, const Vec<T>& im4,
		const std::function<void(T&, const T&, const T&, const T&)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1[offset], im2[offset], im3[offset], im4[offset]);
	}
}
template<class T> void Transform(Vec<T>& im1, const Vec<T>& im2,
		const Vec<T>& im3,
		const std::function<void(T&, const T&, const T&)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (int offset = 0; offset < (int) sz; offset++) {
		func(im1[offset], im2[offset], im3[offset]);
	}
}
template<class T> void Transform(Vec<T>& im1, Vec<T>& im2,
		const std::function<void(size_t offset, T& val1, T& val2)>& func) {
	if (im1.size() != im2.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << im1.size()
						<< "!=" << im2.size());
	size_t sz = im1.size();
#pragma omp parallel for
	for (size_t offset = 0; offset < sz; offset++) {
		func(offset, im1[offset], im2[offset]);
	}
}
template<class T, class L, class R, int C> std::basic_ostream<L, R> & operator <<(
		std::basic_ostream<L, R> & ss, const Vec<T> & A) {
	size_t index = 0;
	for (const T& val : A.data) {
		ss << std::setw(5) << index++ << ": " << val << std::endl;
	}
	return ss;
}
template<class T> Vec<T> operator+(const T& scalar, const Vec<T>& img) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = scalar + val2;};
	Transform(out, img, f);
	return out;
}
template<class T> void ScaleAdd(Vec<T>& out, const T& scalar,
		const Vec<T>& in) {
	out.resize(in.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 += scalar * val2;};
	Transform(out, in, f);
}
template<class T> void ScaleAdd(Vec<T>& out, const Vec<T>& in1, const T& scalar,
		const Vec<T>& in2) {
	out.resize(in1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2+scalar * val3;};
	Transform(out, in1, in2, f);
}
template<class T> void ScaleAdd(Vec<T>& out, const Vec<T>& in1,
		const T& scalar2, const Vec<T>& in2, const T& scalar3,
		const Vec<T>& in3) {
	out.resize(in1.size());
	std::function<void(T&, const T&, const T&, const T&)> f = [=](T& out,
			const T& val1,
			const T& val2,
			const T& val3) {
		out = val1+scalar2*val2+scalar3 * val3;};
	Transform(out, in1, in2, in3, f);
}
template<class T> void ScaleSubtract(Vec<T>& out, const T& scalar,
		const Vec<T>& in) {
	out.resize(in.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 -= scalar * val2;};
	Transform(out, in, f);
}
template<class T> void ScaleSubtract(Vec<T>& out, const Vec<T>& in1,
		const T& scalar, const Vec<T>& in2) {
	out.resize(in1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2 - scalar * val3;};
	Transform(out, in1, in2, f);
}
template<class T> void Subtract(Vec<T>& out, const Vec<T>& v1,
		const Vec<T>& v2) {
	out.resize(v1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2-val3;};
	Transform(out, v1, v2, f);
}
template<class T> void Add(Vec<T>& out, const Vec<T>& v1, const Vec<T>& v2) {
	out.resize(v1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2 + val3;};
	Transform(out, v1, v2, f);
}
template<class T> Vec<T> operator-(const T& scalar, const Vec<T>& img) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = scalar - val2;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator*(const T& scalar, const Vec<T>& img) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = scalar*val2;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator/(const T& scalar, const Vec<T>& img) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = scalar / val2;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator+(const Vec<T>& img, const T& scalar) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = val2 + scalar;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator-(const Vec<T>& img, const T& scalar) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = val2 - scalar;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator*(const Vec<T>& img, const T& scalar) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = val2*scalar;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator/(const Vec<T>& img, const T& scalar) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = val2 / scalar;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator-(const Vec<T>& img) {
	Vec<T> out(img.size());
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 = -val2;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator+=(Vec<T>& out, const Vec<T>& img) {
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 += val2;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator-=(Vec<T>& out, const Vec<T>& img) {
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 -= val2;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator*=(Vec<T>& out, const Vec<T>& img) {
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 *= val2;};
	Transform(out, img, f);
	return out;
}
template<class T> Vec<T> operator/=(Vec<T>& out, const Vec<T>& img) {
	std::function<void(T&, const T&)> f =
			[=](T& val1, const T& val2) {val1 /= val2;};
	Transform(out, img, f);
	return out;
}

template<class T> Vec<T> operator+=(Vec<T>& out, const T& scalar) {
	std::function<void(T&)> f = [=](T& val1) {val1 += scalar;};
	Transform(out, f);
	return out;
}
template<class T> Vec<T> operator-=(Vec<T>& out, const T& scalar) {
	std::function<void(T&)> f = [=](T& val1) {val1 -= scalar;};
	Transform(out, f);
	return out;
}
template<class T> Vec<T> operator*=(Vec<T>& out, const T& scalar) {
	std::function<void(T&)> f = [=](T& val1) {val1 *= scalar;};
	Transform(out, f);
	return out;
}
template<class T> Vec<T> operator/=(Vec<T>& out, const T& scalar) {
	std::function<void(T&)> f = [=](T& val1) {val1 /= scalar;};
	Transform(out, f);
	return out;
}

template<class T> Vec<T> operator+(const Vec<T>& img1, const Vec<T>& img2) {
	Vec<T> out(img1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2 + val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T> Vec<T> operator-(const Vec<T>& img1, const Vec<T>& img2) {
	Vec<T> out(img1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2 - val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T> Vec<T> operator*(const Vec<T>& img1, const Vec<T>& img2) {
	Vec<T> out(img1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2*val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T> Vec<T> operator/(const Vec<T>& img1, const Vec<T>& img2) {
	Vec<T> out(img1.size());
	std::function<void(T&, const T&, const T&)> f =
			[=](T& val1, const T& val2, const T& val3) {val1 = val2 / val3;};
	Transform(out, img1, img2, f);
	return out;
}
template<class T> struct DenseMat {
public:
	Vec<T> vector; //Treat whole tensor as flat vector. Useful!
	std::vector<T, aligned_allocator<T, 64>>& data;
	int rows, cols;
	typedef T ValueType;
	typedef typename std::vector<ValueType>::iterator iterator;
	typedef typename std::vector<ValueType>::const_iterator const_iterator;
	typedef typename std::vector<ValueType>::reverse_iterator reverse_iterator;
	size_t size() const {
		return rows * cols;
	}
	iterator begin(int i) const {
		return data[i].begin();
	}
	iterator end(int i) const {
		return data[i].end();
	}
	iterator begin(int i) {
		return data[i].begin();
	}
	iterator end(int i) {
		return data[i].end();
	}
	const_iterator cbegin(int i) const {
		return data[i].cbegin();
	}
	const_iterator cend(int i) const {
		return data[i].cend();
	}
	reverse_iterator rbegin(int i) {
		return data[i].rbegin();
	}
	reverse_iterator rend(int i) {
		return data[i].rend();
	}
	reverse_iterator rbegin(int i) const {
		return data[i].rbegin();
	}
	reverse_iterator rend(int i) const {
		return data[i].rend();
	}
	template<class Archive> void serialize(Archive & archive) {
		archive(CEREAL_NVP(rows), CEREAL_NVP(cols),
				cereal::make_nvp(MakeString() << "matrix", data));
	}
	T& operator[](size_t i) {
		return data[i];
	}
	const T& operator[](size_t i) const {
		return data[i];
	}
	DenseMat() :
			data(vector.data), rows(0), cols(0) {
	}
	DenseMat(int rows, int cols) :
			data(vector.data), rows(rows), cols(cols) {
		data.resize(rows * (size_t) cols);
	}
	void resize(int rows, int cols) {
		if (this->rows != rows || this->cols != cols) {
			data.resize(rows * (size_t) cols);
			this->rows = rows;
			this->cols = cols;
		}
	}
	void set(size_t i, size_t j, const T& value) {
		if (i >= (size_t) rows || j >= (size_t) cols || i < 0 || j < 0)
			throw std::runtime_error(
					MakeString() << "Index (" << i << "," << j
							<< ") exceeds matrix bounds [" << rows << ","
							<< cols << "]");
		data[i + cols * j] = value;
	}
	T get(size_t i, size_t j) const {
		if (i >= (size_t) rows || j >= (size_t) cols || i < 0 || j < 0)
			throw std::runtime_error(
					MakeString() << "Index (" << i << "," << j
							<< ") exceeds matrix bounds [" << rows << ","
							<< cols << "]");
		return data[i + cols * j];
	}
	T& operator()(size_t i, size_t j) {
		return data[i + cols * j];
	}
	const T& operator()(size_t i, size_t j) const {
		return data[i + cols * j];
	}
	inline DenseMat<T> transpose() const {
		DenseMat<T> M(cols, rows);
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				M(j, i) = data[i + cols * j];
			}
		}
		return M;
	}
	inline static DenseMat<T> identity(size_t M, size_t N) {
		DenseMat<T> A(M, N);
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				A(i, j) = T(T((i == j) ? 1 : 0));
			}
		}
		return A;
	}
	inline static DenseMat<T> zero(size_t M, size_t N) {
		DenseMat<T> A(M, N);
		for (int i = 0; i < M; i++) {
			for (int j = 0; j < N; j++) {
				A(i, j) = T(T(0));
			}
		}
		return A;
	}
	inline static DenseMat<T> diagonal(const Vec<T>& v) {
		DenseMat<T> A((int) v.size(), (int) v.size());
		for (int i = 0; i < A.rows; i++) {
			for (int j = 0; j < A.cols; j++) {
				A(i, j) = T(T((i == j) ? v[i] : 0));
			}
		}
		return A;
	}
};
template<class A, class B, class T, int C> std::basic_ostream<A, B> & operator <<(
		std::basic_ostream<A, B> & ss, const DenseMat<T>& M) {
	ss << "\n";
	if (C == 1) {
		for (int i = 0; i < M.rows; i++) {
			ss << "[";
			for (int j = 0; j < M.cols; j++) {
				ss << std::setprecision(10) << std::setw(16) << M(i, j)
						<< ((j < M.cols - 1) ? "," : "]\n");
			}
		}
	} else {
		for (int i = 0; i < M.rows; i++) {
			ss << "[";
			for (int j = 0; j < M.cols; j++) {
				ss << M(i, j) << ((j < M.cols - 1) ? "," : "]\n");
			}
		}
	}
	return ss;
}

template<class T> Vec<T> operator*(const DenseMat<T>& A, const Vec<T>& v) {
	Vec<T> out(A.rows);
	for (int i = 0; i < A.rows; i++) {
		T sum(0.0);
		for (int j = 0; j < A.cols; j++) {
			sum += A(i, j) * v[j];
		}
		out[i] = sum;
	}
	return out;
}
template<class T> DenseMat<T> operator*(const DenseMat<T>& A,
		const DenseMat<T>& B) {
	if (A.cols != B.rows)
		throw std::runtime_error(
				MakeString()
						<< "Cannot multiply matrices. Inner dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	DenseMat<T> out(A.rows, B.cols);
	for (int i = 0; i < out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			T sum(0.0);
			for (int k = 0; k < A.cols; k++) {
				sum += A(i, k) * B(k, j);
			}
			out(i, j) = sum;
		}
	}
	return out;
}
//Slight abuse of mathematics here. Vectors are always interpreted as column vectors as a convention,
//so this multiplcation is equivalent to multiplying "A" with a diagonal matrix constructed from "W".
//To multiply a matrix with a column vector to get a row vector, convert "W" to a dense matrix.
template<class T> DenseMat<T> operator*(const Vec<T>& W, const DenseMat<T>& A) {
	if (A.rows != (int) W.size())
		throw std::runtime_error(
				MakeString()
						<< "Cannot scale matrix by vector. Rows must match. "
						<< "[" << W.size() << "] * [" << A.rows << "," << A.cols
						<< "]");
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			out(i, j) = W[i] * A(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T>& operator*=(DenseMat<T>& A, const Vec<T>& W) {
	if (A.rows != W.size())
		throw std::runtime_error(
				MakeString()
						<< "Cannot scale matrix by vector. Rows must match. "
						<< "[" << W.size() << "] * [" << A.rows << "," << A.cols
						<< "]");
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A(i, j) *= W[i];
		}
	}
	return A;
}
template<class T> DenseMat<T> operator-(const DenseMat<T>& A) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < (int) out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			out(i, j) = -A(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T> operator-(const DenseMat<T>& A,
		const DenseMat<T>& B) {
	if (A.rows != B.rows || A.cols != B.cols) {
		throw std::runtime_error(
				"Cannot subtract matricies. Matrix dimensions must match.");
	}
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < (int) out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			out(i, j) = A(i, j) - B(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T> operator+(const DenseMat<T>& A,
		const DenseMat<T>& B) {
	if (A.rows != B.rows || A.cols != B.cols) {
		throw std::runtime_error(
				"Cannot add matricies. Matrix dimensions must match.");
	}
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < (int) out.rows; i++) {
		for (int j = 0; j < out.cols; j++) {
			out(i, j) = A(i, j) + B(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T> operator*(const DenseMat<T>& A, const T& v) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = A(i, j) * v;
		}
	}
	return out;
}
template<class T> DenseMat<T> operator/(const DenseMat<T>& A, const T& v) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = A(i, j) / v;
		}
	}
	return out;
}
template<class T> DenseMat<T> operator+(const DenseMat<T>& A, const T& v) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = A(i, j) + v;
		}
	}
	return out;
}
template<class T> DenseMat<T> operator-(const DenseMat<T>& A, const T& v) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = A(i, j) - v;
		}
	}
	return out;
}
template<class T> DenseMat<T> operator*(const T& v, const DenseMat<T>& A) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = v * A(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T> operator/(const T& v, const DenseMat<T>& A) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = v / A(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T> operator+(const T& v, const DenseMat<T>& A) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = v + A(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T> operator-(const T& v, const DenseMat<T>& A) {
	DenseMat<T> out(A.rows, A.cols);
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			out(i, j) = v - A(i, j);
		}
	}
	return out;
}
template<class T> DenseMat<T>& operator*=(DenseMat<T>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A(i, j) = A(i, j) * v;
		}
	}
	return A;
}
template<class T> DenseMat<T>& operator/=(DenseMat<T>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A(i, j) = A(i, j) / v;
		}
	}
	return A;
}
template<class T> DenseMat<T>& operator+=(DenseMat<T>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A(i, j) = A(i, j) + v;
		}
	}
	return A;
}
template<class T> DenseMat<T>& operator-=(DenseMat<T>& A, const T& v) {
	for (int i = 0; i < A.rows; i++) {
		for (int j = 0; j < A.cols; j++) {
			A(i, j) = A(i, j) - v;
		}
	}
	return A;
}

template<class T> void WriteDenseMatToFile(const std::string& file,
		const DenseMat<T>& matrix) {
	std::ofstream os(file);
	cereal::PortableBinaryOutputArchive ar(os);
	ar(cereal::make_nvp("dense_matrix", matrix));
}
template<class T> void ReadDenseMatFromFile(const std::string& file,
		DenseMat<T>& matrix) {
	std::ifstream os(file);
	cereal::PortableBinaryInputArchive ar(os);
	ar(cereal::make_nvp("dense_matrix", matrix));
}

template<class T> struct SparseMat {
private:
	std::vector<std::map<size_t, T>> storage;
public:
	size_t rows, cols;
	SparseMat() :
			rows(0), cols(0) {

	}
	template<class Archive> void serialize(Archive & archive) {
		archive(CEREAL_NVP(rows), CEREAL_NVP(cols),
				cereal::make_nvp(MakeString() << "matrix", storage));
	}
	std::map<size_t, T>& operator[](size_t i) {
		if (i >= rows || i < 0)
			throw std::runtime_error(
					MakeString() << "Index (" << i
							<< ",*) exceeds matrix bounds [" << rows << ","
							<< cols << "]");
		return storage[i];
	}
	const std::map<size_t, T>& operator[](size_t i) const {
		if (i >= rows || i < 0)
			throw std::runtime_error(
					MakeString() << "Index (" << i
							<< ",*) exceeds matrix bounds [" << rows << ","
							<< cols << "]");
		return storage[i];
	}
	SparseMat(size_t rows, size_t cols) :
			storage(rows), rows(rows), cols(cols) {
	}
	size_t size() const {
		size_t count = 0;
		for (const std::map<size_t, T>& vec : storage) {
			count += vec.size();
		}
		return count;
	}
	void resize(size_t rows, size_t cols) {
		this->rows = rows;
		this->cols = cols;
		storage.resize(rows);
	}
	void set(size_t i, size_t j, const T& value) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
			throw std::runtime_error(
					MakeString() << "Index (" << i << "," << j
							<< ") exceeds matrix bounds [" << rows << ","
							<< cols << "]");
		storage[i][j] = value;
	}
	T& operator()(size_t i, size_t j) {
		if (i >= rows || j >= cols || i < 0 || j < 0)
			throw std::runtime_error(
					MakeString() << "Index (" << i << "," << j
							<< ") exceeds matrix bounds [" << rows << ","
							<< cols << "]");
		return storage[i][j];
	}

	T get(size_t i, size_t j) const {
		if (i >= rows || j >= cols || i < 0 || j < 0)
			throw std::runtime_error(
					MakeString() << "Index (" << i << "," << j
							<< ") exceeds matrix bounds [" << rows << ","
							<< cols << "]");
		if (storage[i].find(j) == storage[i].end()) {
			return T(T(0));
		} else {
			return storage[i].at(j);
		}
	}
	T operator()(size_t i, size_t j) const {
		return get(i, j);
	}
	SparseMat<T> transpose() const {
		SparseMat<T> M(cols, rows);
		for (int i = 0; i < (int) storage.size(); i++) {
			for (const std::pair<size_t, T>& iv : storage[i]) {
				M.set(iv.first, i, iv.second);
			}
		}
		return M;
	}
	static SparseMat<T> identity(size_t M, size_t N) {
		SparseMat<T> A(M, N);
		int K = (int) aly::min(M, N);
#pragma omp parallel for
		for (int k = 0; k < K; k++) {
			A[k][k] = T(T(1));
		}
		return A;
	}
	static SparseMat<T> diagonal(const Vec<T>& v) {
		SparseMat<T> A(v.size(), v.size());
#pragma omp parallel for
		for (int k = 0; k < (int) v.size(); k++) {
			A[k][k] = v[k];
		}
		return A;
	}
};
template<class A, class B, class T, int C> std::basic_ostream<A, B> & operator <<(
		std::basic_ostream<A, B> & ss, const SparseMat<T>& M) {
	for (int i = 0; i < (int) M.rows; i++) {
		ss << "M[" << i << ",*]=";
		for (const std::pair<size_t, T>& pr : M[i]) {
			ss << "<" << pr.first << ":" << pr.second << "> ";
		}
		ss << std::endl;
	}
	return ss;
}

template<class T> SparseMat<T>& operator*=(SparseMat<T>& A, const T& v) {
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		for (std::pair<size_t, T>& pr : A[i]) {
			A[i][pr.first] = pr.second * v;
		}
	}
	return A;
}
template<class T> SparseMat<T>& operator/=(const SparseMat<T>& A, const T& v) {
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		for (std::pair<size_t, T>& pr : A[i]) {
			A[i][pr.first] = pr.second / v;
		}
	}
	return A;
}
template<class T> SparseMat<T>& operator+=(SparseMat<T>& A, const T& v) {
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		for (std::pair<size_t, T>& pr : A[i]) {
			A[i][pr.first] = pr.second + v;
		}
	}
	return A;
}
template<class T> SparseMat<T>& operator-=(SparseMat<T>& A, const T& v) {
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		for (std::pair<size_t, T>& pr : A[i]) {
			A[i][pr.first] = pr.second - v;
		}
	}
	return A;
}

template<class T> SparseMat<T>& operator/=(SparseMat<T>& A, const T& v) {
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		for (std::pair<size_t, T>& pr : A[i]) {
			A[i][pr.first] = pr.second / v;
		}
	}
	return A;
}

template<class T> SparseMat<T> operator*(const SparseMat<T>& A,
		const SparseMat<T>& B) {
	if (A.cols != B.rows)
		throw std::runtime_error(
				MakeString()
						<< "Cannot multiply matrices. Inner dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	SparseMat<T> out(A.rows, B.cols);
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) { //a[i,*]
		for (std::pair<size_t, T> pr1 : A[i]) { //a[i,k]
			int k = (int) pr1.first;
			for (std::pair<size_t, T> pr2 : B[k]) { //b[k,j]
				int j = (int) pr2.first;
				out(i, j) += pr1.second * pr2.second;
			}
		}
	}
	return out;
}

template<class T> SparseMat<T> operator*(const T& v, const SparseMat<T>& A) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = v * pr.second;
		}
	}
	return out;
}
template<class T> SparseMat<T> operator/(const T& v, const SparseMat<T>& A) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = v / pr.second;
		}
	}
	return out;
}
template<class T> SparseMat<T> operator+(const T& v, const SparseMat<T>& A) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = T(v) + pr.second;
		}
	}
	return out;
}
template<class T> SparseMat<T> operator-(const T& v, const SparseMat<T>& A) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = T(v) - pr.second;
		}
	}
	return out;
}

template<class T> SparseMat<T> operator-(const SparseMat<T>& A, const T& v) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = pr.second - v;
		}
	}
	return out;
}
template<class T> SparseMat<T> operator+(const SparseMat<T>& A, const T& v) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = pr.second + v;
		}
	}
	return out;
}
template<class T> SparseMat<T> operator*(const SparseMat<T>& A, const T& v) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = pr.second * v;
		}
	}
	return out;
}

template<class T> SparseMat<T> operator/(const SparseMat<T>& A, const T& v) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = pr.second / v;
		}
	}
	return out;
}

template<class T> SparseMat<T> operator-(const SparseMat<T>& A) {
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : A[i]) {
			out[i][pr.first] = -pr.second;
		}
	}
	return out;
}
template<class T> SparseMat<T> operator+(const SparseMat<T>& A,
		const SparseMat<T>& B) {
	if (A.rows != B.rows || A.cols != B.cols)
		throw std::runtime_error(
				MakeString() << "Cannot add matrices. Dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : B[i]) {
			out[i][pr.first] += pr.second;
		}
	}
	return out;
}
template<class T> SparseMat<T> operator-(const SparseMat<T>& A,
		const SparseMat<T>& B) {
	if (A.rows != B.rows || A.cols != B.cols)
		throw std::runtime_error(
				MakeString()
						<< "Cannot subtract matrices. Dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
	SparseMat<T> out = A;
#pragma omp parallel for
	for (int i = 0; i < (int) out.rows; i++) {
		for (std::pair<size_t, T> pr : B[i]) {
			out[i][pr.first] -= pr.second;
		}
	}
	return out;
}
template<class T> SparseMat<T>& operator+=(SparseMat<T>& A,
		const SparseMat<T>& B) {
	if (A.rows != B.rows || A.cols != B.cols)
		throw std::runtime_error(
				MakeString() << "Cannot add matrices. Dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		for (std::pair<size_t, T> pr : B[i]) {
			A[i][pr.first] += pr.second;
		}
	}
	return A;
}
template<class T> SparseMat<T>& operator-=(SparseMat<T>& A,
		const SparseMat<T>& B) {
	if (A.rows != B.rows || A.cols != B.cols)
		throw std::runtime_error(
				MakeString()
						<< "Cannot subtract matrices. Dimensions do not match. "
						<< "[" << A.rows << "," << A.cols << "] * [" << B.rows
						<< "," << B.cols << "]");
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		for (std::pair<size_t, T> pr : B[i]) {
			A[i][pr.first] -= pr.second;
		}
	}
	return A;
}
template<class T> void Multiply(Vec<T>& out, const SparseMat<T>& A,
		const Vec<T>& v) {
	out.resize(A.rows);
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		double sum = (0.0);
		for (const std::pair<size_t, T>& pr : A[i]) {
			sum += double(v[pr.first]) * (double) pr.second;
		}
		out[i] = T(sum);
	}
}
template<class T> void AddMultiply(Vec<T>& out, const Vec<T>& b,
		const SparseMat<T>& A, const Vec<T>& v) {
	out.resize(A.rows);
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		double sum = 0.0;
		for (const std::pair<size_t, T>& pr : A[i]) {
			sum += double(v[pr.first]) * (double) pr.second;
		}
		out[i] = b[i] + T(sum);
	}
}
template<class T> void SubtractMultiply(Vec<T>& out, const Vec<T>& b,
		const SparseMat<T>& A, const Vec<T>& v) {
	out.resize(A.rows);
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		double sum = 0.0;
		for (const std::pair<size_t, T>& pr : A[i]) {
			sum += double(v[pr.first]) * (double) pr.second;
		}
		out[i] = b[i] - T(sum);
	}
}
template<class T> Vec<T> operator*(const SparseMat<T>& A, const Vec<T>& v) {
	Vec<T> out(A.rows);
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		double sum = 0.0;
		for (const std::pair<size_t, T>& pr : A[i]) {
			sum += double(v[pr.first]) * double(pr.second);
		}
		out[i] = T(sum);
	}
	return out;
}
template<class T> void MultiplyVec(Vec<T>& out, const SparseMat<T>& A,
		const Vec<T>& v) {
	out.resize(A.rows);
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		double sum = 0.0;
		for (const std::pair<size_t, T>& pr : A[i]) {
			sum += double(v[pr.first]) * double(pr.second);
		}
		out[i] = T(sum);
	}
}

template<class T> void AddMultiplyVec(Vec<T>& out, const Vec<T>& b,
		const SparseMat<T>& A, const Vec<T>& v) {
	out.resize(A.rows);
#pragma omp parallel for
	for (int i = 0; i < A.rows; i++) {
		double sum = 0.0;
		for (const std::pair<size_t, T>& pr : A[i]) {
			sum += double(v[pr.first]) * double(pr.second);
		}
		out[i] = b[i] + T(sum);
	}
}
template<class T> void SubtractMultiplyVec(Vec<T>& out, const Vec<T>& b,
		const SparseMat<T>& A, const Vec<T>& v) {
	out.resize(A.rows);
#pragma omp parallel for
	for (int i = 0; i < (int) A.rows; i++) {
		double sum = 0.0;
		for (const std::pair<size_t, T>& pr : A[i]) {
			sum += double(v[pr.first]) * double(pr.second);
		}
		out[i] = b[i] - T(sum);
	}
}
template<class T> void WriteSparseMatToFile(const std::string& file,
		const SparseMat<T>& matrix) {
	std::ofstream os(file);
	cereal::PortableBinaryOutputArchive ar(os);
	ar(cereal::make_nvp("sparse_matrix", matrix));
}
template<class T> void ReadSparseMatFromFile(const std::string& file,
		SparseMat<T>& matrix) {
	std::ifstream os(file);
	cereal::PortableBinaryInputArchive ar(os);
	ar(cereal::make_nvp("sparse_matrix", matrix));
}
template<class T> double lengthSqr(const Vec<T>& a) {
	size_t sz = a.size();
	double cans = 0;
#pragma omp parallel for reduction(+:cans)
	for (int i = 0; i < (int) sz; i++) {
		double val = a[i];
		cans += val * val;
	}
	return cans;
}
template<class T> double length(const Vec<T>& a) {
	return std::sqrt(lengthSqr(a));
}
template<class T> double dot(const Vec<T>& a, const Vec<T>& b) {
	double ans = 0.0;
	if (a.size() != b.size())
		throw std::runtime_error(
				MakeString() << "Vector dimensions do not match. " << a.size()
						<< "!=" << b.size());
	size_t sz = a.size();
#pragma omp parallel for reduction(+:ans)
	for (int i = 0; i < (int) sz; i++) {
		ans += double(a[i]) * double(b[i]);
	}
	return ans;
}
template<class T> T lengthL1(const Vec<T>& a) {
	T ans(0);
	size_t sz = a.size();
#pragma omp parallel for reduction(+:ans)
	for (int i = 0; i < (int) sz; i++) {
		ans += std::abs(a[i]);
	}
	return ans;
}
template<class T> T lengthInf(const Vec<T>& a) {
	T ans(0);
	size_t sz = a.size();
	for (int i = 0; i < (int) sz; i++) {
		ans = std::max(ans, std::abs(a[i]));
	}
	return ans;
}
typedef DenseMat<double> DenseMatrixDouble;
typedef SparseMat<double> SparseMatrixDouble;
typedef Vec<double> VecDouble;
typedef Vec<double> Vec1d;
typedef Vec<double2> Vec2d;
typedef Vec<double3> Vec3d;
typedef Vec<double3> Vec4d;

typedef DenseMat<float> DenseMatrixFloat;
typedef SparseMat<float> SparseMatrixFloat;
typedef Vec<float> VecFloat;
typedef Vec<float> Vec1f;
typedef Vec<float2> Vec2f;
typedef Vec<float3> Vec3f;
typedef Vec<float3> Vec4f;

}

#endif /* INCLUDE_CORE_ALLOYOPTIMIZATIONMATH_H_ */
