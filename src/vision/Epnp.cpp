/******************************************************************************
 * Author:   Laurent Kneip                                                    *
 * Contact:  kneip.laurent@gmail.com                                          *
 * License:  Copyright (c) 2013 Laurent Kneip, ANU. All rights reserved.      *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions         *
 * are met:                                                                   *
 * * Redistributions of source code must retain the above copyright           *
 *   notice, this list of conditions and the following disclaimer.            *
 * * Redistributions in binary form must reproduce the above copyright        *
 *   notice, this list of conditions and the following disclaimer in the      *
 *   documentation and/or other materials provided with the distribution.     *
 * * Neither the name of ANU nor the names of its contributors may be         *
 *   used to endorse or promote products derived from this software without   *
 *   specific prior written permission.                                       *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"*
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE *
 * ARE DISCLAIMED. IN NO EVENT SHALL ANU OR THE CONTRIBUTORS BE LIABLE        *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT         *
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY  *
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF     *
 * SUCH DAMAGE.                                                               *
 ******************************************************************************/

// Note: this code has been downloaded from the homepage of the "Computer 
// Vision Laboratory" at EPFL Lausanne, and was originally developped by the 
// authors of [4]. I only adapted it to Eigen.
#include <iostream>
using namespace std;
#include "vision/Epnp.h"
namespace aly {
aly::float4x4 SolvePnP(const aly::Vector3f& pts, const aly::Vector2f& pxs,
		const CameraProjector& view) {
	Epnp PnP;
	PnP.set_internal_parameters(view.K(0, 2), view.K(1, 2), view.K(0, 0),
			view.K(1, 1));
	PnP.set_maximum_number_of_correspondences(pts.size());
	PnP.reset_correspondences();
	for (int i = 0; i < pts.size(); i++) {
		float3 pt = pts[i];
		float2 pix = pxs[i];
		PnP.add_correspondence(pt.x, pt.y, pt.z, pix.x, pix.y, 1.0f);
	}
	double R_est[3][3];
	double t_est[3] = { 0, 0, 0 };
	double err2 = PnP.compute_pose(R_est, t_est);
	double rot_err, transl_err;
	float4x4 M = float4x4::identity();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			M(i, j) = R_est[i][j];
		}
		M(i, 3) = t_est[i];
	}
	return M;
}
Epnp::Epnp(void) {
	cws_determinant = 0;
	maximum_number_of_correspondences = 0;
	number_of_correspondences = 0;
	pws = 0;
	us = 0;
	alphas = 0;
	pcs = 0;
	signs = 0; //added
	this->uc = 0.0;
	this->vc = 0.0;
	this->fu = 1.0;
	this->fv = 1.0;
}

Epnp::~Epnp() {
	delete[] pws;
	delete[] us;
	delete[] alphas;
	delete[] pcs;
	delete[] signs; //added
}
void Epnp::set_internal_parameters(double u, double v, double fu, double fv) {
	this->uc = u;
	this->vc = v;
	this->fu = fu;
	this->fv = fv;
}
void Epnp::set_maximum_number_of_correspondences(int n) {
	if (maximum_number_of_correspondences < n) {
		if (pws != 0)
			delete[] pws;
		if (us != 0)
			delete[] us;
		if (alphas != 0)
			delete[] alphas;
		if (pcs != 0)
			delete[] pcs;
		if (signs != 0)
			delete[] signs; //added

		maximum_number_of_correspondences = n;
		pws = new double[3 * maximum_number_of_correspondences];
		us = new double[2 * maximum_number_of_correspondences];
		alphas = new double[4 * maximum_number_of_correspondences];
		pcs = new double[3 * maximum_number_of_correspondences];
		signs = new int[maximum_number_of_correspondences];
	}
}

void Epnp::reset_correspondences(void) {
	number_of_correspondences = 0;
}

void Epnp::add_correspondence(double X, double Y, double Z, double x, double y,
		double z) //changed this interface
		{
	pws[3 * number_of_correspondences] = X;
	pws[3 * number_of_correspondences + 1] = Y;
	pws[3 * number_of_correspondences + 2] = Z;

	us[2 * number_of_correspondences] = x / z;
	us[2 * number_of_correspondences + 1] = y / z;

	//added the following
	if (z > 0.0)
		signs[number_of_correspondences] = 1;
	else
		signs[number_of_correspondences] = -1;

	number_of_correspondences++;
}

void Epnp::choose_control_points(void) {
	// Take C0 as the reference points centroid:
	cws[0][0] = cws[0][1] = cws[0][2] = 0;
	for (int i = 0; i < number_of_correspondences; i++)
		for (int j = 0; j < 3; j++)
			cws[0][j] += pws[3 * i + j];
	for (int j = 0; j < 3; j++)
		cws[0][j] /= number_of_correspondences;
	// Take C1, C2, and C3 from PCA on the reference points:
	DenseMat<double> PW0(number_of_correspondences, 3);

	for (int i = 0; i < number_of_correspondences; i++)
		for (int j = 0; j < 3; j++)
			PW0(i, j) = pws[3 * i + j] - cws[0][j];

	DenseMat<double> PW0tPW0 = PW0.transpose() * PW0;
	DenseMat<double> U;
	DenseMat<double> D;
	DenseMat<double> Vt;
	SVD(PW0tPW0, U, D, Vt);
	double det = D(0, 0) * D(1, 1) * D(2, 2);
	if (det != 0) {
		for (int i = 1; i < 4; i++) {
			double k = sqrt(D(i - 1, i - 1) / number_of_correspondences);
			for (int j = 0; j < 3; j++) {
				cws[i][j] = cws[0][j] + k * U(j, (i - 1));
			}
		}
	} else {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 3; j++) {
				cws[i][j] = pws[3 * i + j]-pws[j] + cws[0][j];
			}
		}
	}

}

void Epnp::compute_barycentric_coordinates(void) {
	double3x3 CC;
	for (int i = 0; i < 3; i++) {
		for (int j = 1; j < 4; j++) {
			CC(i, j - 1) = cws[j][i] - cws[0][i];
		}
	}
	double3x3 U,D,Vt;
	SVD(CC,U,D,Vt);
	for(int i=0;i<3;i++){
		if(std::abs(D(i,i))>1E-15f){
			D(i,i)=1.0/D(i,i);
		}
	}
	double3x3 CC_inv = transpose(Vt)*D*transpose(U);
	for (int i = 0; i < number_of_correspondences; i++) {
		double * pi = pws + 3 * i;
		double * a = alphas + 4 * i;
		for (int j = 0; j < 3; j++)
			a[1 + j] = CC_inv(j, 0) * (pi[0] - cws[0][0])
					+ CC_inv(j, 1) * (pi[1] - cws[0][1])
					+ CC_inv(j, 2) * (pi[2] - cws[0][2]);
		a[0] = 1.0f - a[1] - a[2] - a[3];
	}
}

void Epnp::fill_M(DenseMat<double> & M, const int row, const double * as,
		const double u, const double v) {
	for (int i = 0; i < 4; i++) {
		M(row, 3 * i) = as[i] * fu;
		M(row, 3 * i + 1) = 0.0;
		M(row, 3 * i + 2) = as[i] * (uc - u);

		M(row + 1, 3 * i) = 0.0;
		M(row + 1, 3 * i + 1) = as[i] * fv;
		M(row + 1, 3 * i + 2) = as[i] * (vc - v);
	}
}

void Epnp::compute_ccs(const double * betas, const DenseMat<double> & ut) {
	for (int i = 0; i < 4; i++)
		ccs[i][0] = ccs[i][1] = ccs[i][2] = 0.0f;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 3; k++)
				ccs[j][k] += betas[i] * ut(11 - i, 3 * j + k);
	}
}

void Epnp::compute_pcs(void) {
	for (int i = 0; i < number_of_correspondences; i++) {
		double * a = alphas + 4 * i;
		double * pc = pcs + 3 * i;

		for (int j = 0; j < 3; j++)
			pc[j] = a[0] * ccs[0][j] + a[1] * ccs[1][j] + a[2] * ccs[2][j]
					+ a[3] * ccs[3][j];
	}
}

double Epnp::compute_pose(double R[3][3], double t[3]) {
	choose_control_points();
	compute_barycentric_coordinates();
	DenseMat<double> M(2 * number_of_correspondences, 12);
	for (int i = 0; i < number_of_correspondences; i++){
		fill_M(M, 2 * i, alphas + 4 * i, us[2 * i], us[2 * i + 1]);
	}
	DenseMat<double> MtM = M.transpose() * M;
	DenseMat<double> U, D, Vt;
	SVD(MtM, U, D, Vt);
	DenseMat<double> Ut = U.transpose();
	DenseMat<double> L_6x10(6, 10);
	Vec<double> Rho(6);
	compute_L_6x10(Ut, L_6x10);
	compute_rho(Rho);
	double Betas[4][4], rep_errors[4];
	double Rs[4][3][3], ts[4][3];
	find_betas_approx_1(L_6x10, Rho, Betas[1]);
	gauss_newton(L_6x10, Rho, Betas[1]);
	rep_errors[1] = compute_R_and_t(Ut, Betas[1], Rs[1], ts[1]);
	find_betas_approx_2(L_6x10, Rho, Betas[2]);
	gauss_newton(L_6x10, Rho, Betas[2]);
	rep_errors[2] = compute_R_and_t(Ut, Betas[2], Rs[2], ts[2]);
	find_betas_approx_3(L_6x10, Rho, Betas[3]);
	gauss_newton(L_6x10, Rho, Betas[3]);
	rep_errors[3] = compute_R_and_t(Ut, Betas[3], Rs[3], ts[3]);
	int N = 1;
	if (rep_errors[2] < rep_errors[1])
		N = 2;
	if (rep_errors[3] < rep_errors[N])
		N = 3;
	copy_R_and_t(Rs[N], ts[N], R, t);
	return rep_errors[N];
}
void Epnp::copy_R_and_t(const double R_src[3][3], const double t_src[3],
		double R_dst[3][3], double t_dst[3]) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			R_dst[i][j] = R_src[i][j];
		t_dst[i] = t_src[i];
	}
}

double Epnp::dist2(const double * p1, const double * p2) {
	return (p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1])
			+ (p1[2] - p2[2]) * (p1[2] - p2[2]);
}

double Epnp::dot(const double * v1, const double * v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

double Epnp::reprojection_error(const double R[3][3], const double t[3]) {
	double sum2 = 0.0;

	for (int i = 0; i < number_of_correspondences; i++) {
		double * pw = pws + 3 * i;
		double Xc = dot(R[0], pw) + t[0];
		double Yc = dot(R[1], pw) + t[1];
		double inv_Zc = 1.0 / (dot(R[2], pw) + t[2]);
		double ue = uc + fu * Xc * inv_Zc;
		double ve = vc + fv * Yc * inv_Zc;
		double u = us[2 * i], v = us[2 * i + 1];
		sum2 += std::sqrt((u - ue) * (u - ue) + (v - ve) * (v - ve));
	}

	return sum2 / number_of_correspondences;
}

void Epnp::estimate_R_and_t(double R[3][3], double t[3]) {
	double pc0[3], pw0[3];

	pc0[0] = pc0[1] = pc0[2] = 0.0;
	pw0[0] = pw0[1] = pw0[2] = 0.0;

	for (int i = 0; i < number_of_correspondences; i++) {
		const double * pc = pcs + 3 * i;
		const double * pw = pws + 3 * i;

		for (int j = 0; j < 3; j++) {
			pc0[j] += pc[j];
			pw0[j] += pw[j];
		}
	}
	for (int j = 0; j < 3; j++) {
		pc0[j] /= number_of_correspondences;
		pw0[j] /= number_of_correspondences;
	}

	DenseMat<double> Abt(3, 3);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
			Abt(i, j) = 0.0;
	}

	for (int i = 0; i < number_of_correspondences; i++) {
		double * pc = pcs + 3 * i;
		double * pw = pws + 3 * i;

		for (int j = 0; j < 3; j++) {
			Abt(j, 0) += (pc[j] - pc0[j]) * (pw[0] - pw0[0]);
			Abt(j, 1) += (pc[j] - pc0[j]) * (pw[1] - pw0[1]);
			Abt(j, 2) += (pc[j] - pc0[j]) * (pw[2] - pw0[2]);
		}
	}

	DenseMat<double> Abt_u, D, Vt;
	SVD(Abt, Abt_u, D, Vt);
	DenseMat<double> Abt_v = Vt.transpose();
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			R[i][j] = aly::dot(Abt_u.getRow(i), Abt_v.getRow(j));
		}
	const double det = R[0][0] * R[1][1] * R[2][2] + R[0][1] * R[1][2] * R[2][0]
			+ R[0][2] * R[1][0] * R[2][1] - R[0][2] * R[1][1] * R[2][0]
			- R[0][1] * R[1][0] * R[2][2] - R[0][0] * R[1][2] * R[2][1];

	//change 1: negative determinant problem is solved by changing Abt_v, not R

	if (det < 0) {
		//R[2][0] = -R[2][0];
		//R[2][1] = -R[2][1];
		//R[2][2] = -R[2][2];
		DenseMat<double> Abt_v_prime = Abt_v;
		Abt_v_prime.getColumn(2) = -Abt_v.getColumn(2);
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				R[i][j] = aly::dot(Abt_u.getRow(i), Abt_v_prime.getRow(j));
	}

	t[0] = pc0[0] - dot(R[0], pw0);
	t[1] = pc0[1] - dot(R[1], pw0);
	t[2] = pc0[2] - dot(R[2], pw0);
}

void Epnp::print_pose(const double R[3][3], const double t[3]) {
	cout << R[0][0] << " " << R[0][1] << " " << R[0][2] << " " << t[0] << endl;
	cout << R[1][0] << " " << R[1][1] << " " << R[1][2] << " " << t[1] << endl;
	cout << R[2][0] << " " << R[2][1] << " " << R[2][2] << " " << t[2] << endl;
}

void Epnp::solve_for_sign(void) {
	//change2: the following method is not independent of the optical system
	/*if (pcs[2] < 0.0) {
	 for(int i = 0; i < 4; i++)
	 for(int j = 0; j < 3; j++)
	 ccs[i][j] = -ccs[i][j];

	 for(int i = 0; i < number_of_correspondences; i++) {
	 pcs[3 * i    ] = -pcs[3 * i];
	 pcs[3 * i + 1] = -pcs[3 * i + 1];
	 pcs[3 * i + 2] = -pcs[3 * i + 2];
	 }
	 }*/

	//change to this (using original depths)
	if ((pcs[2] < 0.0 && signs[0] > 0) || (pcs[2] > 0.0 && signs[0] < 0)) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 3; j++)
				ccs[i][j] = -ccs[i][j];

		for (int i = 0; i < number_of_correspondences; i++) {
			pcs[3 * i] = -pcs[3 * i];
			pcs[3 * i + 1] = -pcs[3 * i + 1];
			pcs[3 * i + 2] = -pcs[3 * i + 2];
		}
	}
}

double Epnp::compute_R_and_t(const DenseMat<double> & Ut, const double * betas,
		double R[3][3], double t[3]) {
	compute_ccs(betas, Ut);
	compute_pcs();

	solve_for_sign();

	estimate_R_and_t(R, t);

	return reprojection_error(R, t);
}

// betas10        = [B11 B12 B22 B13 B23 B33 B14 B24 B34 B44]
// betas_approx_1 = [B11 B12     B13         B14]

void Epnp::find_betas_approx_1(const DenseMat<double> & L_6x10,
		const Vec<double> & Rho, double * betas) {
	DenseMat<double> L_6x4(6, 4);
	for (int i = 0; i < 6; i++) {
		L_6x4(i, 0) = L_6x10(i, 0);
		L_6x4(i, 1) = L_6x10(i, 1);
		L_6x4(i, 2) = L_6x10(i, 3);
		L_6x4(i, 3) = L_6x10(i, 6);
	}
	Vec<double> b4 = Solve(L_6x4, Rho, MatrixFactorization::SVD);
	if (b4[0] < 0) {
		betas[0] = std::sqrt(-b4[0]);
		betas[1] = -b4[1] / betas[0];
		betas[2] = -b4[2] / betas[0];
		betas[3] = -b4[3] / betas[0];
	} else {
		betas[0] = std::sqrt(b4[0]);
		betas[1] = b4[1] / betas[0];
		betas[2] = b4[2] / betas[0];
		betas[3] = b4[3] / betas[0];
	}
}

// betas10        = [B11 B12 B22 B13 B23 B33 B14 B24 B34 B44]
// betas_approx_2 = [B11 B12 B22                            ]

void Epnp::find_betas_approx_2(const DenseMat<double> & L_6x10,
		const Vec<double> & Rho, double * betas) {
	DenseMat<double> L_6x3(6, 3);
	for (int i = 0; i < 6; i++) {
		L_6x3(i, 0) = L_6x10(i, 0);
		L_6x3(i, 1) = L_6x10(i, 1);
		L_6x3(i, 2) = L_6x10(i, 2);
	}
	Vec<double> b3 = Solve(L_6x3, Rho, MatrixFactorization::SVD);
	if (b3[0] < 0) {
		betas[0] = sqrt(-b3[0]);
		betas[1] = (b3[2] < 0) ? sqrt(-b3[2]) : 0.0;
	} else {
		betas[0] = sqrt(b3[0]);
		betas[1] = (b3[2] > 0) ? sqrt(b3[2]) : 0.0;
	}

	if (b3[1] < 0)
		betas[0] = -betas[0];

	betas[2] = 0.0;
	betas[3] = 0.0;
}

// betas10        = [B11 B12 B22 B13 B23 B33 B14 B24 B34 B44]
// betas_approx_3 = [B11 B12 B22 B13 B23                    ]

void Epnp::find_betas_approx_3(const DenseMat<double> & L_6x10,
		const Vec<double> & Rho, double * betas) {
	DenseMat<double> L_6x5(6, 5);

	for (int i = 0; i < 6; i++) {
		L_6x5(i, 0) = L_6x10(i, 0);
		L_6x5(i, 1) = L_6x10(i, 1);
		L_6x5(i, 2) = L_6x10(i, 2);
		L_6x5(i, 3) = L_6x10(i, 3);
		L_6x5(i, 4) = L_6x10(i, 4);
	}

	Vec<double> b5 = Solve(L_6x5, Rho, MatrixFactorization::SVD);

	if (b5[0] < 0) {
		betas[0] = sqrt(-b5[0]);
		betas[1] = (b5[2] < 0) ? sqrt(-b5[2]) : 0.0;
	} else {
		betas[0] = sqrt(b5[0]);
		betas[1] = (b5[2] > 0) ? sqrt(b5[2]) : 0.0;
	}
	if (b5[1] < 0)
		betas[0] = -betas[0];
	betas[2] = b5[3] / betas[0];
	betas[3] = 0.0;
}

void Epnp::compute_L_6x10(const DenseMat<double> & Ut,
		DenseMat<double> & L_6x10) {
	double3 dv[4][6];
	for (int i = 0; i < 4; i++) {
		int a = 0, b = 1;
		for (int j = 0; j < 6; j++) {
			dv[i][j][0] = Ut(11 - i, 3 * a) - Ut(11 - i, 3 * b);
			dv[i][j][1] = Ut(11 - i, 3 * a + 1) - Ut(11 - i, 3 * b + 1);
			dv[i][j][2] = Ut(11 - i, 3 * a + 2) - Ut(11 - i, 3 * b + 2);
			b++;
			if (b > 3) {
				a++;
				b = a + 1;
			}
		}
	}

	for (int i = 0; i < 6; i++) {
		L_6x10(i, 0) = aly::dot(dv[0][i], dv[0][i]);
		L_6x10(i, 1) = 2.0 * aly::dot(dv[0][i], dv[1][i]);
		L_6x10(i, 2) = aly::dot(dv[1][i], dv[1][i]);
		L_6x10(i, 3) = 2.0 * aly::dot(dv[0][i], dv[2][i]);
		L_6x10(i, 4) = 2.0 * aly::dot(dv[1][i], dv[2][i]);
		L_6x10(i, 5) = aly::dot(dv[2][i], dv[2][i]);
		L_6x10(i, 6) = 2.0 * aly::dot(dv[0][i], dv[3][i]);
		L_6x10(i, 7) = 2.0 * aly::dot(dv[1][i], dv[3][i]);
		L_6x10(i, 8) = 2.0 * aly::dot(dv[2][i], dv[3][i]);
		L_6x10(i, 9) = aly::dot(dv[3][i], dv[3][i]);
	}
}

void Epnp::compute_rho(Vec<double> & Rho) {
	Rho[0] = dist2(cws[0], cws[1]);
	Rho[1] = dist2(cws[0], cws[2]);
	Rho[2] = dist2(cws[0], cws[3]);
	Rho[3] = dist2(cws[1], cws[2]);
	Rho[4] = dist2(cws[1], cws[3]);
	Rho[5] = dist2(cws[2], cws[3]);
}

void Epnp::compute_A_and_b_gauss_newton(const DenseMat<double> & L_6x10,
		const Vec<double> & Rho, double betas[4], DenseMat<double> & A,
		Vec<double> & b) {
	for (int i = 0; i < 6; i++) {
		A(i, 0) = 2 * L_6x10(i, 0) * betas[0] + L_6x10(i, 1) * betas[1]
				+ L_6x10(i, 3) * betas[2] + L_6x10(i, 6) * betas[3];
		A(i, 1) = L_6x10(i, 1) * betas[0] + 2 * L_6x10(i, 2) * betas[1]
				+ L_6x10(i, 4) * betas[2] + L_6x10(i, 7) * betas[3];
		A(i, 2) = L_6x10(i, 3) * betas[0] + L_6x10(i, 4) * betas[1]
				+ 2 * L_6x10(i, 5) * betas[2] + L_6x10(i, 8) * betas[3];
		A(i, 3) = L_6x10(i, 6) * betas[0] + L_6x10(i, 7) * betas[1]
				+ L_6x10(i, 8) * betas[2] + 2 * L_6x10(i, 9) * betas[3];
		b[i] = Rho[i]
				- (L_6x10(i, 0) * betas[0] * betas[0]
						+ L_6x10(i, 1) * betas[0] * betas[1]
						+ L_6x10(i, 2) * betas[1] * betas[1]
						+ L_6x10(i, 3) * betas[0] * betas[2]
						+ L_6x10(i, 4) * betas[1] * betas[2]
						+ L_6x10(i, 5) * betas[2] * betas[2]
						+ L_6x10(i, 6) * betas[0] * betas[3]
						+ L_6x10(i, 7) * betas[1] * betas[3]
						+ L_6x10(i, 8) * betas[2] * betas[3]
						+ L_6x10(i, 9) * betas[3] * betas[3]);
	}
}
void Epnp::gauss_newton(const DenseMat<double> & L_6x10,
		const Vec<double> & Rho, double betas[4]) {
	const int iterations_number = 5;
	DenseMat<double> A(6, 4);
	Vec<double> B(6);
	Vec<double> X;
	for (int k = 0; k < iterations_number; k++) {
		compute_A_and_b_gauss_newton(L_6x10, Rho, betas, A, B);
		X = Solve(A, B, MatrixFactorization::QR);
		for (int i = 0; i < 4; i++)
			betas[i] += X[i];
	}
}

void Epnp::relative_error(double & rot_err, double & transl_err,
		const double Rtrue[3][3], const double ttrue[3],
		const double Rest[3][3], const double test[3]) {
	double qtrue[4], qest[4];

	mat_to_quat(Rtrue, qtrue);
	mat_to_quat(Rest, qest);

	double rot_err1 = sqrt(
			(qtrue[0] - qest[0]) * (qtrue[0] - qest[0])
					+ (qtrue[1] - qest[1]) * (qtrue[1] - qest[1])
					+ (qtrue[2] - qest[2]) * (qtrue[2] - qest[2])
					+ (qtrue[3] - qest[3]) * (qtrue[3] - qest[3]))
			/ sqrt(
					qtrue[0] * qtrue[0] + qtrue[1] * qtrue[1]
							+ qtrue[2] * qtrue[2] + qtrue[3] * qtrue[3]);

	double rot_err2 = sqrt(
			(qtrue[0] + qest[0]) * (qtrue[0] + qest[0])
					+ (qtrue[1] + qest[1]) * (qtrue[1] + qest[1])
					+ (qtrue[2] + qest[2]) * (qtrue[2] + qest[2])
					+ (qtrue[3] + qest[3]) * (qtrue[3] + qest[3]))
			/ sqrt(
					qtrue[0] * qtrue[0] + qtrue[1] * qtrue[1]
							+ qtrue[2] * qtrue[2] + qtrue[3] * qtrue[3]);

	rot_err = min(rot_err1, rot_err2);

	transl_err = sqrt(
			(ttrue[0] - test[0]) * (ttrue[0] - test[0])
					+ (ttrue[1] - test[1]) * (ttrue[1] - test[1])
					+ (ttrue[2] - test[2]) * (ttrue[2] - test[2]))
			/ sqrt(
					ttrue[0] * ttrue[0] + ttrue[1] * ttrue[1]
							+ ttrue[2] * ttrue[2]);
}

void Epnp::mat_to_quat(const double R[3][3], double q[4]) {
	double tr = R[0][0] + R[1][1] + R[2][2];
	double n4;

	if (tr > 0.0f) {
		q[0] = R[1][2] - R[2][1];
		q[1] = R[2][0] - R[0][2];
		q[2] = R[0][1] - R[1][0];
		q[3] = tr + 1.0f;
		n4 = q[3];
	} else if ((R[0][0] > R[1][1]) && (R[0][0] > R[2][2])) {
		q[0] = 1.0f + R[0][0] - R[1][1] - R[2][2];
		q[1] = R[1][0] + R[0][1];
		q[2] = R[2][0] + R[0][2];
		q[3] = R[1][2] - R[2][1];
		n4 = q[0];
	} else if (R[1][1] > R[2][2]) {
		q[0] = R[1][0] + R[0][1];
		q[1] = 1.0f + R[1][1] - R[0][0] - R[2][2];
		q[2] = R[2][1] + R[1][2];
		q[3] = R[2][0] - R[0][2];
		n4 = q[1];
	} else {
		q[0] = R[2][0] + R[0][2];
		q[1] = R[2][1] + R[1][2];
		q[2] = 1.0f + R[2][2] - R[0][0] - R[1][1];
		q[3] = R[0][1] - R[1][0];
		n4 = q[2];
	}
	double scale = 0.5f / double(sqrt(n4));

	q[0] *= scale;
	q[1] *= scale;
	q[2] *= scale;
	q[3] *= scale;
}
}
