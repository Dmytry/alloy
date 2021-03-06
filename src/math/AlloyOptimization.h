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

#ifndef INCLUDE_CORE_ALLOYOPTIMIZATION_H_
#define INCLUDE_CORE_ALLOYOPTIMIZATION_H_

#include "math/AlloyOptimizationMath.h"
#include "math/AlloyLBFGS.h"
#include "ui/AlloyEnum.h"

namespace aly {
template<class T> struct SparseProblem {
	virtual size_t getInputSize() const=0;
	virtual size_t getOutputSize() const=0;
	virtual Vec<T> constraint() =0;
	virtual Vec<T> evaluate(const Vec<T>& input) =0;
	virtual void differentiate(const size_t& index, const Vec<T>& input,
			std::vector<std::pair<size_t, T>>& derivative) = 0;
	virtual void reset() {
	}

	void differentiate(const Vec<T>& input, SparseMat<T>& J) {
		J.resize(getOutputSize(), getInputSize());
#pragma omp parallel for
		for (int r = 0; r < (int) J.rows; r++) {
			std::vector<std::pair<size_t, T>> row;
			differentiate(r, input, row);
			for (auto pr : row) {
				J.set(r, pr.first, pr.second);
			}
		}
	}
	Vec<T> residual(const Vec<T>& input) {
		return constraint() - evaluate(input);
	}
	T errorL1(const Vec<T>& input) {
		return lengthL1(constraint() - evaluate(input));
	}
	T errorL2(const Vec<T>& input) {
		return length(constraint() - evaluate(input));
	}
	T errorSqr(const Vec<T>& input) {
		return lengthSqr(constraint() - evaluate(input));
	}
	T errorInf(const Vec<T>& input) {
		return lengthInf(constraint() - evaluate(input));
	}
	virtual ~SparseProblem() {

	}
};

/******************************************************************************
 * XLISP-STAT 2.1 Copyright (c) 1990, by Luke Tierney
 * XLISP version 2.1, Copyright (c) 1989, by David Betz.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Luke Tierney and David Betz not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Luke Tierney and David Betz
 * make no representations about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * LUKE TIERNEY AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL LUKE TIERNEY NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * XLISP-STAT AUTHOR:
 *               Luke Tierney
 *               School of Statistics
 *               University of Minnesota
 *               Minneapolis, MN 55455
 *               (612) 625-7843
 *
 *       Email Address:
 *               internet: luke@umnstat.stat.umn.edu
 *
 * XLISP AUTHOR:
 *              David Betz
 *              P.O. Box 144
 *              Peterborough, NH 03458
 *              (603) 924-4145
 ******************************************************************************
 * XLISP-STAT 2.1 was ported to the Amiga by
 *              J.K. Lindsey
 *              Faculty of Economic, Business and Social Sciences,
 *              University of Liege,
 *              Sart Tilman B31,
 *              4000 Liege,
 *              Belgium
 *              32-41-56.29.64
 *
 * The above permission and disclaimer also applies to all of the specifically
 * Amiga portions of this software, with the restriction that the Amiga
 * version not be used for any military-related applications.
 ******************************************************************************

 */

template<class T> void SVD(const DenseMat<T>& M, DenseMat<T>& U, DenseMat<T>& D,
		DenseMat<T>& Vt, double zeroTolerance = 0) {
	const int m = M.rows;
	const int n = M.cols;
	DenseMat<double> u(m, m);
	DenseMat<double> v(n, n);
	std::vector<double> w(n);
	std::vector<double> rv1(n);
	int flag, i, its, j, jj, k, l, nm = 0;
	double c, f, h, s, x, y, z;
	double anorm, g, scale;
	U.resize(m, m);
	Vt.resize(n, n);
	D.resize(m, n);
	anorm = 0.0, g = 0.0, scale = 0.0;
	if (m < n) {
		throw std::runtime_error(
				"SVD error, rows must be greater than or equal to cols.");
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			u[i][j] = (double) M[i][j];
		}
	}
	for (i = 0; i < n; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m) {
			for (k = i; k < m; k++)
				scale += std::abs((double) u[k][i]);
			if (scale > zeroTolerance) {
				for (k = i; k < m; k++) {
					u[k][i] = ((double) u[k][i] / scale);
					s += ((double) u[k][i] * (double) u[k][i]);
				}
				f = (double) u[i][i];
				g = -sign(std::sqrt(s), f);
				h = f * g - s;
				u[i][i] = (f - g);
				if (i != n - 1) {
					for (j = l; j < n; j++) {
						for (s = 0.0, k = i; k < m; k++)
							s += ((double) u[k][i] * (double) u[k][j]);
						f = s / h;
						for (k = i; k < m; k++)
							u[k][j] += (f * (double) u[k][i]);
					}
				}
				for (k = i; k < m; k++)
					u[k][i] = ((double) u[k][i] * scale);
			}
		}
		w[i] = (scale * g);
		g = s = scale = 0.0;
		if (i < m && i != n - 1) {
			for (k = l; k < n; k++)
				scale += std::abs((double) u[i][k]);
			if (scale > zeroTolerance) {
				for (k = l; k < n; k++) {
					u[i][k] = ((double) u[i][k] / scale);
					s += ((double) u[i][k] * (double) u[i][k]);
				}
				f = (double) u[i][l];
				g = -sign(std::sqrt(s), f);
				h = f * g - s;
				u[i][l] = (f - g);
				for (k = l; k < n; k++)
					rv1[k] = (double) u[i][k] / h;
				if (i != m - 1) {
					for (j = l; j < m; j++) {
						for (s = 0.0, k = l; k < n; k++)
							s += ((double) u[j][k] * (double) u[i][k]);
						for (k = l; k < n; k++)
							u[j][k] += (s * rv1[k]);
					}
				}
				for (k = l; k < n; k++)
					u[i][k] = ((double) u[i][k] * scale);
			}
		}
		anorm = aly::max(anorm, (std::abs((double) w[i]) + std::abs(rv1[i])));
	}
	for (i = n - 1; i >= 0; i--) {
		if (i < n - 1) {
			if (std::abs(g) > zeroTolerance) {
				for (j = l; j < n; j++)
					v[j][i] = (((double) u[i][j] / (double) u[i][l]) / g);
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < n; k++)
						s += ((double) u[i][k] * (double) v[k][j]);
					for (k = l; k < n; k++)
						v[k][j] += (s * (double) v[k][i]);
				}
			}
			for (j = l; j < n; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = n - 1; i >= 0; i--) {
		l = i + 1;
		g = (double) w[i];
		if (i < n - 1)
			for (j = l; j < n; j++)
				u[i][j] = 0.0;
		if (std::abs(g) > zeroTolerance) {
			g = 1.0 / g;
			if (i != n - 1) {
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < m; k++)
						s += ((double) u[k][i] * (double) u[k][j]);
					f = (s / (double) u[i][i]) * g;
					for (k = i; k < m; k++)
						u[k][j] += (f * (double) u[k][i]);
				}
			}
			for (j = i; j < m; j++)
				u[j][i] = ((double) u[j][i] * g);
		} else {
			for (j = i; j < m; j++)
				u[j][i] = 0.0;
		}
		++u[i][i];
	}
	for (k = n - 1; k >= 0; k--) {
		for (its = 0; its < 30; its++) {
			flag = 1;
			for (l = k; l >= 0; l--) {
				nm = l - 1;
				if (std::abs(rv1[l]) + anorm == anorm) {
					flag = 0;
					break;
				}
				if (nm >= 0 && std::abs((double) w[nm]) + anorm == anorm) {
					break;
				}
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					if (std::abs(f) + anorm != anorm) {
						g = (double) w[i];
						h = pythag(f, g);
						w[i] = h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 0; j < m; j++) {
							y = (double) u[j][nm];
							z = (double) u[j][i];
							u[j][nm] = (y * c + z * s);
							u[j][i] = (z * c - y * s);
						}
					}
				}
			}
			z = (double) w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = (-z);
					for (j = 0; j < n; j++)
						v[j][k] = (-v[j][k]);
				}
				int iii, jjj;
				for (iii = k; (iii < n - 1) && (w[iii] < w[iii + 1]); iii++) {
					std::swap(w[iii], w[iii + 1]);
					for (jjj = 0; jjj < m; jjj++)
						std::swap(u[jjj][iii], u[jjj][iii + 1]);
					for (jjj = 0; jjj < n; jjj++)
						std::swap(v[jjj][iii], v[jjj][iii + 1]);
				}
				break;
			}
			if (its >= 30) {
				throw std::runtime_error("SVD did not converge.");
			}
			x = (double) w[l];
			nm = k - 1;
			y = (double) w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = pythag(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + sign(g, f))) - h)) / x;
			c = s = 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = (double) w[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 0; jj < n; jj++) {
					x = (double) v[jj][j];
					z = (double) v[jj][i];
					v[jj][j] = (x * c + z * s);
					v[jj][i] = (z * c - x * s);
				}
				z = pythag(f, h);
				w[j] = z;
				if (z) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 0; jj < m; jj++) {
					y = (double) u[jj][j];
					z = (double) u[jj][i];
					u[jj][j] = (y * c + z * s);
					u[jj][i] = (z * c - y * s);
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = x;
		}
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) {
				D[i][j] = (T) w[j];
			} else {
				D[i][j] = T(0);
			}
		}
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++) {
			U[i][j] = (T) u[i][j];
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			Vt[i][j] = (T) v[j][i];
		}
	}
}
template<class T> DenseMat<T> inverse(const DenseMat<T>& M,
		double zeroTolerance = 0.0) {
	if (M.rows != M.cols) {
		throw std::runtime_error(
				MakeString()
						<< "Could not invert matrix.  Rows and columns must agree: ["
						<< M.rows << ", " << M.cols << "]");
	}
	DenseMat<T> U, D, Vt;
	SVD(M, U, D, Vt);
	int K = aly::min(D.rows, D.cols);
	for (int k = 0; k < K; k++) {
		double d = D[k][k];
		if (std::abs(d) > zeroTolerance) {
			d = 1.0 / d;
		}
		D[k][k] = T(d);
	}
	return (U * D * Vt).transpose();
}
template<class T> Vec<T> SolveSVD(const DenseMat<T>& A, const Vec<T>& b) {
	if (A.rows != (int) b.size()) {
		throw std::runtime_error(
				MakeString()
						<< "Matrix row dimensions and vector length must agree. A=["
						<< A.rows << "," << A.cols << "] b=[" << b.size()
						<< "]");
	}
	if (A.rows != A.cols) {
		DenseMat<T> At = A.transpose();
		DenseMat<T> AtA = At * A;
		Vec<T> Atb = At * b;
		return inverse(AtA) * Atb;
	} else {
		return inverse(A) * b;
	}
}
//Back port of NIST's Java Implementation of LINPACK called JAMA. Code is licensed for free use in the public domain. http://math.nist.gov/javanumerics/jama/
/** LU Decomposition.
 <P>
 For an m-by-n matrix A with m >= n, the LU decomposition is an m-by-n
 unit lower triangular matrix L, an n-by-n upper triangular matrix U,
 and a permutation vector piv of length m so that A(piv,:) = L*U.
 If m < n, then L is m-by-m and U is m-by-n.
 <P>
 The LU decompostion with pivoting always exists, even if the matrix is
 singular, so the constructor will never fail.  The primary use of the
 LU decomposition is in the solution of square systems of simultaneous
 linear equations.  This will fail if isNonsingular() returns false.
 */
template<class T> bool LU(const DenseMat<T>& A, DenseMat<T>& L, DenseMat<T>& U,
		std::vector<int>& piv, const double zeroTolerance = 0.0) {
	const int m = A.rows;
	const int n = A.cols;
	std::vector<std::vector<double>> LU(m, std::vector<double>(n, 0.0));
	piv.resize(m);
	std::vector<double> LUcolj(m, 0.0);
	int pivsign;
	double* LUrowi;
	L.resize(m, n);
	U.resize(n, n);
	bool nonSingular = true;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			LU[i][j] = (double) A[i][j];
		}
	}
	for (int i = 0; i < m; i++) {
		piv[i] = i;
	}
	pivsign = 1;
	for (int j = 0; j < n; j++) {
		for (int i = 0; i < m; i++) {
			LUcolj[i] = LU[i][j];
		}
		for (int i = 0; i < m; i++) {
			LUrowi = &LU[i][0];
			int kmax = aly::min(i, j);
			double s = 0.0;
			for (int k = 0; k < kmax; k++) {
				s += LUrowi[k] * LUcolj[k];
			}
			LUrowi[j] = LUcolj[i] -= s;
		}
		int p = j;
		for (int i = j + 1; i < m; i++) {
			if (std::abs(LUcolj[i]) > std::abs(LUcolj[p])) {
				p = i;
			}
		}
		if (p != j) {
			for (int k = 0; k < n; k++) {
				std::swap(LU[p][k], LU[j][k]);
			}
			std::swap(piv[p], piv[j]);
			pivsign = -pivsign;
		}
		if (j < m && std::abs(LU[j][j]) > zeroTolerance) {
			for (int i = j + 1; i < m; i++) {
				LU[i][j] /= LU[j][j];
			}
		}
	}
	for (int j = 0; j < n; j++) {
		if (std::abs(LU[j][j]) <= zeroTolerance) {
			nonSingular = false;
			break;
		}
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (i > j) {
				L[i][j] = (T) LU[i][j];
			} else if (i == j) {
				L[i][j] = T(1.0);
			} else {
				L[i][j] = T(0.0);
			}
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i <= j) {
				U[i][j] = T(LU[i][j]);
			} else {
				U[i][j] = T(0.0);
			}
		}
	}
	return nonSingular;
}

template<class T> Vec<T> SolveLU(const DenseMat<T>& A, const Vec<T>& b) {

	if (A.rows != (int) b.size()) {
		throw std::runtime_error(
				MakeString()
						<< "Matrix row dimensions and vector length must agree. A=["
						<< A.rows << "," << A.cols << "] b=[" << b.size()
						<< "]");
	}
	if (A.rows != A.cols) {
		DenseMat<T> At = A.transpose();
		DenseMat<T> AtA = At * A;
		Vec<T> Atb = At * b;
		int n = AtA.cols;
		Vec<T> x(A.cols);
		Vec<T> y(A.cols);
		DenseMat<T> L, U;
		std::vector<int> piv;
		bool nonSingular = LU(AtA, L, U, piv);
		if (!nonSingular) {
			throw std::runtime_error("Matrix is singular.");
		}
		// Forward solve Ly = b
		for (int i = 0; i < n; i++) {
			y[i] = Atb[piv[i]];
			for (int j = 0; j < i; j++) {
				y[i] -= L[i][j] * y[j];
			}
			y[i] /= L[i][i];
		}
		// Backward solve Ux = y
		for (int i = n - 1; i >= 0; i--) {
			x[i] = y[i];
			for (int j = i + 1; j < n; j++) {
				x[i] -= U[i][j] * x[j];
			}
			x[i] /= U[i][i];
		}
		return x;
	} else {
		int n = A.cols;

		Vec<T> x(A.cols);
		Vec<T> y(A.cols);
		DenseMat<T> L, U;
		std::vector<int> piv;
		bool nonSingular = LU(A, L, U, piv);
		if (!nonSingular) {
			throw std::runtime_error("Matrix is singular.");
		}
		// Forward solve Ly = b
		for (int i = 0; i < n; i++) {
			y[i] = b[piv[i]];
			for (int j = 0; j < i; j++) {
				y[i] -= L[i][j] * y[j];
			}
			y[i] /= L[i][i];
		}
		// Backward solve Ux = y
		for (int i = n - 1; i >= 0; i--) {
			x[i] = y[i];
			for (int j = i + 1; j < n; j++) {
				x[i] -= U[i][j] * x[j];
			}
			x[i] /= U[i][i];
		}
		return x;
	}
}

/** QR Decomposition.
 <P>
 For an m-by-n matrix A with m >= n, the QR decomposition is an m-by-n
 orthogonal matrix Q and an n-by-n upper triangular matrix R so that
 A = Q*R.
 <P>
 The QR decompostion always exists, even if the matrix does not have
 full rank, so the constructor will never fail.  The primary use of the
 QR decomposition is in the least squares solution of nonsquare systems
 of simultaneous linear equations.  This will fail if isFullRank()
 returns false.
 */
template<class T> bool QR(const DenseMat<T>& A, DenseMat<T>& Q,
		DenseMat<T>& R) {
	const int m = A.rows;
	const int n = A.cols;
	DenseMat<double> qr(m, n);
	std::vector<double> Rdiag(m);
	R.resize(n, n);
	Q.resize(m, n);
	bool nonSingular = true;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			qr[i][j] = (double) A[i][j];
		}
	}
	for (int k = 0; k < n; k++) {
		double nrm = 0;
		for (int i = k; i < m; i++) {
			nrm = pythag(nrm, qr[i][k]);
		}
		if (nrm != 0.0) {
			if (qr[k][k] < 0) {
				nrm = -nrm;
			}
			for (int i = k; i < m; i++) {
				qr[i][k] /= nrm;
			}
			qr[k][k] += 1.0;
			for (int j = k + 1; j < n; j++) {
				double s = 0.0;
				for (int i = k; i < m; i++) {
					s += qr[i][k] * qr[i][j];
				}
				s = -s / qr[k][k];
				for (int i = k; i < m; i++) {
					qr[i][j] += s * qr[i][k];
				}
			}
		}
		Rdiag[k] = -nrm;
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i < j) {
				R[i][j] = T(qr[i][j]);
			} else if (i == j) {
				R[i][j] = T(Rdiag[i]);
			} else {
				R[i][j] = T(0.0);
			}
		}
	}
	if(n>1){
		for (int j = 0; j < n; j++) {
			if (Rdiag[j] == 0) {
				nonSingular = false;
				break;
			}
		}
	}
	for (int k = n - 1; k >= 0; k--) {
		for (int i = 0; i < m; i++) {
			Q[i][k] = T(0.0);
		}
		Q[k][k] = T(1.0);
		for (int j = k; j < n; j++) {
			if (qr[k][k] != 0) {
				double s = 0.0;
				for (int i = k; i < m; i++) {
					s += qr[i][k] * Q[i][j];
				}
				s = -s / qr[k][k];
				for (int i = k; i < m; i++) {
					Q[i][j] += T(s * qr[i][k]);
				}
			}
		}
	}
	return nonSingular;
}

template<class T> Vec<T> SolveQR(const DenseMat<T>& A, const Vec<T>& b) {

	if (A.rows != (int) b.size()) {
		throw std::runtime_error(
				MakeString()
						<< "Matrix row dimensions and vector length must agree. A=["
						<< A.rows << "," << A.cols << "] b=[" << b.size()
						<< "]");
	}
	if (A.rows != A.cols) {
		DenseMat<T> At = A.transpose();
		DenseMat<T> AtA = At * A;
		Vec<T> Atb = At * b;
		int n = AtA.cols;
		Vec<T> x(A.cols);
		DenseMat<T> Q, R;
		bool nonSingular = QR(AtA, Q, R);
		if (!nonSingular) {
			throw std::runtime_error("Matrix is singular.");
		}
		// Compute Y = transpose(Q)*B
		x = Q.transpose() * Atb;
		// Solve R*X = Y;
		for (int k = n - 1; k >= 0; k--) {
			x[k] /= R[k][k];
			for (int i = 0; i < k; i++) {
				x[i] -= x[k] * R[i][k];
			}
		}
		return x;
	} else {
		int n = A.cols;
		Vec<T> x(A.cols);
		DenseMat<T> Q, R;
		bool nonSingular = QR(A, Q, R);
		if (!nonSingular) {
			throw std::runtime_error("Matrix is singular.");
		}
		// Compute Y = transpose(Q)*B
		x = Q.transpose() * b;
		// Solve R*X = Y;
		for (int k = n - 1; k >= 0; k--) {
			x[k] /= R[k][k];
			for (int i = 0; i < k; i++) {
				x[i] -= x[k] * R[i][k];
			}
		}
		return x;
	}
}
template<class C, class R> std::basic_ostream<C, R> & operator <<(
		std::basic_ostream<C, R> & ss, const MatrixFactorization& type) {
	switch (type) {
	case MatrixFactorization::SVD:
		return ss << "SVD";
	case MatrixFactorization::QR:
		return ss << "QR";
	case MatrixFactorization::LU:
		return ss << "LU";
	}
	return ss;
}
Vec<float> Solve(const DenseMat<float>& A, const Vec<float>& b,
		const MatrixFactorization& factor = MatrixFactorization::SVD);

Vec<float> SolveNonNegative(const DenseMat<float>& A, const Vec<float>& b,
		const MatrixFactorization& factor = MatrixFactorization::SVD);

Vec<double> Solve(const DenseMat<double>& A, const Vec<double>& b,
		const MatrixFactorization& factor = MatrixFactorization::SVD);

Vec<double> SolveNonNegative(const DenseMat<double>& A, const Vec<double>& b,
		const MatrixFactorization& factor = MatrixFactorization::SVD);

void SolveCG(const Vec<float>& b, const SparseMat<float>& A, Vec<float>& x,
		int iters = 100, double tolerance = 1E-6,
		const std::function<bool(int, double)>& iterationMonitor = nullptr);
void SolveBICGStab(const Vec<float>& b, const SparseMat<float>& A,
		Vec<float>& x, int iters = 100, double tolerance = 1E-6,
		const std::function<bool(int, double)>& iterationMonitor = nullptr);
void SolveLevenbergMarquardt(SparseProblem<float>& problem, Vec<float>& p,
		int maxIterations = 100, double errorTolerance = 1E-9,
		const std::function<bool(int, double)>& monitor = nullptr);
void SolveDogLeg(SparseProblem<float>& problem, Vec<float>& p,
		int maxIterations = 100, double errorTolerance = 1E-9, float trust =
				1E3f,
		const std::function<bool(int, double)>& monitor = nullptr);

void SolveCG(const Vec<double>& b, const SparseMat<double>& A, Vec<double>& x,
		int iters = 100, double tolerance = 1E-6,
		const std::function<bool(int, double)>& iterationMonitor = nullptr);
void SolveBICGStab(const Vec<double>& b, const SparseMat<double>& A,
		Vec<double>& x, int iters = 100, double tolerance = 1E-6,
		const std::function<bool(int, double)>& iterationMonitor = nullptr);
void SolveLevenbergMarquardt(SparseProblem<double>& problem, Vec<double>& p,
		int maxIterations = 100, double errorTolerance = 1E-9,
		const std::function<bool(int, double)>& monitor = nullptr);
void SolveDogLeg(SparseProblem<double>& problem, Vec<double>& p,
		int maxIterations = 100, double errorTolerance = 1E-9, double trust =
				1E3f,
		const std::function<bool(int, double)>& monitor = nullptr);

}
#endif /* INCLUDE_CORE_ALLOYOPTIMIZATION_H_ */
