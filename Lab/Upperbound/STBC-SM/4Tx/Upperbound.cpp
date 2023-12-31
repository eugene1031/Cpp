#pragma warning(disable : 4996)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <complex> 
#include <vector>
#include <cstdlib>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#define  OUTPUTFILE  "anglenosp4tx0302all.txt"
#define  OUTPUTFILED "anglenosp4tx0302alldebug.txt"

#define  OPENSTRING  "#Eb/No(dB) BitErrRate         BitErrSample  TotalSample \n" 
#define  DATASTRING  "%5.2f      %18.16f %7d %11u \n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate
#define  DB0		0.0
#define  DBSTEP		5.0
#define  POINTNO	7
#define  ERRNOSTEP  2
#define  MAXERRNO	5000//2000//1000//500
#define  SAMPLE_NUM 3000000//500000//200000//100000
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              4
#define  N              4
#define  frame_w        1
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359

#define  inter	        2
#define  codeword		64 //=y 代表看x個bits 2^x = y  //bitsnum=12


using namespace std;
using namespace Eigen;


#define  M3  0x00000004L
#define  M31 0x40000000L
#define  eulen(r1,i1,r2,i2) ((r1)-(r2))*((r1)-(r2))+((i1)-(i2))*((i1)-(i2))

FILE* fp;
FILE* fpDebug;
static char  filename[80] = OUTPUTFILE;
static char fpDebugFilename[80] = OUTPUTFILED;


static int	 v[2] = { -1, 1 };
int	 point, biterrno, errlevel, u, t, de_info, dsignal, samp;
long    pnstate, pntemp;
double   snrdb, snr, deviate, dmin;

MatrixXcd X1(2, 2);
MatrixXcd X2(2, 2);
MatrixXcd X3(2, 2);
MatrixXcd Xp1(2, 2);
MatrixXcd Xp2(2, 2);
MatrixXcd Xp3(2, 2);

complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) };

MatrixXcd I_s1s2(2, 2);
MatrixXcd I_s3s4(2, 2);

MatrixXcd Q_s1s2(2, 2);
MatrixXcd Q_s3s4(2, 2);

MatrixXcd r_matrix(M, 2), Dij_matrix(M, M);
MatrixXcd Q_diag(M, 2), new_Q_diag(M, M);
MatrixXcd I_diag(M, 2), new_I_diag(M, M);
MatrixXcd St0(2 * M, M);
MatrixXcd St1(2 * M, M);
MatrixXcd S0(M, M);
MatrixXcd S1(M, M);
MatrixXcd Xt(M, M);
MatrixXcd Xt_1(M, M);
MatrixXcd Xtp(M, M);
MatrixXcd Xt_1p(M, M);
MatrixXcd A(M, M);
VectorXcd SingularValues(M), eigenvalue(M);


complex<double> s[2][M];

MatrixXcd* I_s1s2_ptr = &I_s1s2;
MatrixXcd* I_s3s4_ptr = &I_s3s4;
MatrixXcd* Q_s1s2_ptr = &Q_s1s2;
MatrixXcd* Q_s3s4_ptr = &Q_s3s4;
MatrixXcd I_diag_new(M, M);

int gt1 = 0;
int gt2 = 0;
int g1 = 0;
int g2 = 0;

int m = 0;

int q11 = 0;
int q12 = 0;
int q13 = 0;
int q14 = 0;
int q21 = 0;
int q22 = 0;
int q23 = 0;
int q24 = 0;

int aaa = 0;
int interleaved_pattern[4][4] = { 1,2,3,4,     //  1324 是不合法的
									1,3,4,2,
									1,4,2,3,
									2,1,4,3,
	//2,4,3,1,
	//2,3,1,4
};
int antenna_perm[2][2] = { 1,2,
							2,1 };
//要用的天線
int antenna[16][2] = { 1,2,
					2,1,
					1,1,
					2,2,
					3,4,//都加角度4
					4,3,
					3,3,
					4,4,
					3,1,//加左邊角度
					3,2,
					4,1,
					4,2,
					1,3,//加右邊角度
					1,4,
					2,3,
					2,4
};
int two_time_ant_per[4][1] = { 1,
							2,
							3,
							4 };

MatrixXcd H(N, M), H_test(N, M / 2), noise(N, M);
MatrixXcd channel_estimation(MatrixXcd* H, int* reference_order) {
	//(1,1)
	/*[ H12 | H34 ]*[ STBC12 | STBC34 ] = Y= [ H12 * STBC12 | H12 * STBC34 ]  (1*2)的矩陣乘上(2*2) 等於 (1*2)的矩陣
						 0		  0
	*/
	//H12
	if (*reference_order == 1) {
		H_test.col(0) << (*H).col(0);
		H_test.col(1) << (*H).col(1);

	}
	//H34
	if (*reference_order == 2) {
		H_test.col(0) << (*H).col(2);
		H_test.col(1) << (*H).col(3);

	}
	//H23
	if (*reference_order == 3) {
		H_test.col(0) << (*H).col(1);
		H_test.col(1) << (*H).col(2);
	}
	//H14
	if (*reference_order == 4) {
		H_test.col(0) << (*H).col(0);
		H_test.col(1) << (*H).col(3);
	}
	return H_test;
}



int m_a, m_b;
int r_a, r_b;
int q_a, q_b;
//ori statistics_dmin[511][M]
double sqrtsingular[M] = {}, statistics_dmin[codeword - 1][M] = { 0 }, temp_dmin[M];
double gain[M] = { 1,1,1,1 };
double gaintotal = 1;
double totaltest = 0;
double rank1 = 0;
double rank2_count = 0;
double gain_save[20] = {};
double number[20] = {};
double min1 = 9999999.0;

/*void record()
{
	fp = fopen(filename, "a");
	fprintf(fp, DATASTRING);
	fclose(fp);
}*/

MatrixXcd Gmatrix(int g)
{
	MatrixXcd G1(M, M);
	G1 << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;


	MatrixXcd G2(M, M);
	G2 << 1, 0, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
		0, 1, 0, 0;



	if (g == 0) {
		return G1;
	}
	if (g == 1) {
		return G2;
	}

}
int find3And4Zero(int x, int y, double a, double b, double c, double d) {
	int count = 0;
	if (a == 0.0)
		count = count + 1;
	if (b == 0.0)
		count = count + 1;
	if (c == 0.0)
		count = count + 1;
	if (d == 0.0)
		count = count + 1;
	return count;
}
int hammingDistance(int x, int y) {
	int ans = 0;
	int m = max(x, y);
	while (m) {
		int c1 = x & 1;
		int c2 = y & 1;
		if (c1 != c2)
			ans += 1;
		m = m >> 1;
		x = x >> 1;
		y = y >> 1;
	}
	return ans;
}

complex<double> angle = complex<double>(cos(pi / 5.150214), sin(pi / 5.150214)); //sita = 0.61rad
MatrixXcd ret(int x) {


	MatrixXcd antenna1(2, 4); antenna1 << 1, 0, 0, 0,
		0, 1, 0, 0;

	MatrixXcd antenna2(2, 4); antenna2 << 0, 0, 1, 0,
		0, 0, 0, 1;

	MatrixXcd antenna3(2, 4); antenna3 << 0, 1, 0, 0,
		0, 0, 1, 0;

	MatrixXcd antenna4(2, 4); antenna4 << 1, 0, 0, 0,
		0, 0, 0, 1;

	//if (x == 1) return antenna1;
	//if (x == 2) return antenna2;
	if (x == 1)
	{
		return antenna1;
	}
	if (x == 2)
	{
		return antenna2;
	}
	if (x == 3)
	{
		//angle
		return antenna3 * angle;
	}
	if (x == 4)
	{
		//angle
		return antenna4 * angle;
	}

	//return x == 1 ? antenna1 : antenna2;
}
MatrixXcd generationTime12(int* x, MatrixXcd* I_s1s2) {  //轉至後位子會跑掉，所以先給一個假的
	MatrixXcd time12(4, 2), time34(4, 2), final_mat(4, 2);
	//fake is1s2
	(*I_s1s2)(0, 1) = -conj((*I_s1s2)(0, 1));
	(*I_s1s2)(1, 0) = -conj((*I_s1s2)(1, 0));
	
	time12 << ((*I_s1s2) * ret(*x)).transpose();

	//cout << (*I_s1s2) << endl<<endl;
	//final_mat << time12, time34;
	//final_mat << (1 / sqrt(2.0)) * time12, (1 / sqrt(2.0))* time34;
	final_mat << (1 / sqrt(2.0)) * time12;

	//cout << final_mat << endl << endl;
	//cout << "func" << endl;
	//cout << final_mat << endl<<endl;
	return final_mat;
}



void TxRx()
{
	//輸出.txt要加的!!
	fp = fopen(filename, "a");
	fpDebug = fopen(fpDebugFilename, "a");

	MatrixXcd zero(2, 2);
	zero << 0, 0,
		0, 0;

	MatrixXcd I0(M, M);
	I0 << 1, -1, 0, 0,
		1, 1, 0, 0,
		0, 0, 1, -1,
		0, 0, 1, 1;
	I0 = (1 / sqrt(2.0)) * I0;

	MatrixXcd I1(M, M);
	I1 << 1, -1, 0, 0,
		1, 1, 0, 0,
		0, 0, 1, -1,
		0, 0, 1, 1;
	I1 = (1 / sqrt(2.0)) * I1;

	MatrixXcd Xt0(M, M);
	MatrixXcd Xt1(M, M);
	for (int kk = 0 && kk != 3; kk < 10; kk++) {
		if (kk != 3) {

		}
	}

	int two = 0, four = 0;
	for (int a = 0; a < codeword; a++) { // a < codeword - 1

		for (int b = 0; b < codeword; b++) {
			cout << " a = " << a << " b = " << b << endl;

			//cout << "hamming Distance = " << hammingDistance(a, b) << endl;
				if (b != a) {
			//10進制轉2進制，再兩個一組轉10進制
			
			//time12
			s[0][0] = qpsk_map[((a / 8) % 2) * 2 + ((a / 4) % 2)];
			s[0][1] = qpsk_map[((a / 2) % 2) * 2 + (a % 2)];

			s[1][0] = qpsk_map[((b / 8) % 2) * 2 + ((b / 4) % 2)];
			s[1][1] = qpsk_map[((b / 2) % 2) * 2 + (b % 2)];

			I_s1s2 << s[0][0], -conj(s[0][1]), s[0][1], conj(s[0][0]);

			Q_s1s2 << s[1][0], -conj(s[1][1]), s[1][1], conj(s[1][0]);

			m_a = ((a / 32) % 2) * 2 + ((a / 16) % 2);
			I_diag << generationTime12(&two_time_ant_per[m_a][0], I_s1s2_ptr);

			m_b = ((b / 32) % 2) * 2 + ((b / 16) % 2);
			Q_diag << generationTime12(&two_time_ant_per[m_b][0], Q_s1s2_ptr);

			//論文裡面的公式
			r_matrix << (I_diag - Q_diag);
			Dij_matrix << (r_matrix) * (r_matrix.adjoint());

			//cout << r_matrix << endl << endl;
			//cout << conj_matrix << endl << endl;
			//cout << trans_matrix << endl << endl;
			//cout << "I_diag : " << endl;
			//cout <<  I_diag << endl << endl;

			//cout << "Q_diag : " << endl;
			//cout << Q_diag << endl << endl;

			//cout << "(Xi - Xj)" << endl;
			//cout << r_matrix << endl<<endl;

			//cout << "(Xi - Xj)_trans/conj" << endl;
			//cout << r_matrix.adjoint() << endl<<endl;

			//cout << "Dij_matrix : "<< endl;
			//cout << Dij_matrix << endl<<endl;
			


			//eigenvalue << Dij_matrix.eigenvalues();
			//cout << "eigenvalue"  << endl;
			//cout << eigenvalue << endl<<endl;

			//real_eigenvalue << eigenvalue.real();

			//cout << (eigenvalue.real())(0) << endl << endl;
			//eigenvalue << ((Dij_matrix.eigenvalues()).real())(0);

			/*
			cout << "算出來的特徵值: ";
			for (int k = 0; k < M; k++)
				cout << " " << ((Dij_matrix.eigenvalues()).real())(k);;
			puts("");
			*/
			/*

			*/
			//Debug find zero numbers bigger than 3
			if (find3And4Zero(a, b, ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3)) > 2)
			{
				fprintf(fpDebug, "%d    %.5f%10.5f%10.5f%10.5f  (%d,%d)\n", hammingDistance(a, b), ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), a, b);
			}



			if(a < b){
			fprintf(fp, "%d    %.5f%10.5f%10.5f%10.5f%10.5f%10d%10d\n", hammingDistance(a, b), ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), (((Dij_matrix.eigenvalues()).real())(2) * ((Dij_matrix.eigenvalues()).real())(3)), a, b);
			}



			
			if (((Dij_matrix.eigenvalues()).real())(0) == 0 && ((Dij_matrix.eigenvalues()).real())(1) == 0) {
				//cout << "diversity 為2的情況下的dmin，";
				dmin = pow((((Dij_matrix.eigenvalues()).real())(2) * ((Dij_matrix.eigenvalues()).real())(3)), 0.5);
				//printf("%.17f\n", dmin);
				if (dmin < 1) {
					temp_dmin[0] += 1;
				}
				if (dmin > 1.9 && dmin < 2) {
					temp_dmin[1] += 1;
				}
				if (dmin > 2.9 && dmin < 3) {
					temp_dmin[2] += 1;
				}
				if (dmin > 3.9 && dmin < 4) {
					temp_dmin[3] += 1;
				}

				//cout << "看這裡==>" << dmin << endl << endl;
				two++;
			}
			else {
				//puts(" ");
				four++;
			}
			
			



			}//b!=a


		}//b for

		cout << "dmin is 1 :" << temp_dmin[0] << endl;
		cout << "dmin is 2 :" << temp_dmin[1] << endl;
		cout << "dmin is 3 :" << temp_dmin[2] << endl;
		cout << "dmin is 4 :" << temp_dmin[3] << endl<<endl;
		

		statistics_dmin[a][0] = temp_dmin[0];
		statistics_dmin[a][1] = temp_dmin[1];
		statistics_dmin[a][2] = temp_dmin[2];
		statistics_dmin[a][3] = temp_dmin[3];


		temp_dmin[0] = 0;
		temp_dmin[1] = 0;
		temp_dmin[2] = 0;
		temp_dmin[3] = 0;
		//printf("diversity is 4 : %d\n", four);
		//printf("diversity is 2 : %d\n", two);

	}
	double dmin[M];
	for (int jj = 0; jj < codeword - 1; jj++) {

		dmin[0] += statistics_dmin[jj][0];
		dmin[1] += statistics_dmin[jj][1];
		dmin[2] += statistics_dmin[jj][2];
		dmin[3] += statistics_dmin[jj][3];
		fclose(fp);
	}
	cout << "dmin is 1 : " << (dmin[0] * 2) / codeword << endl;
	cout << "dmin is 2 : " << (dmin[1] * 2) / (codeword) << endl;
	cout << "dmin is 3 : " << (dmin[2] * 2) / (codeword) << endl;
	cout << "dmin is 4 : " << (dmin[3] * 2) / (codeword) << endl;

	//cout << "dmin is 1 : " << (dmin[0] )  << endl;
	//cout << "dmin is 2 : " << (dmin[1] )  << endl;
	//cout << "dmin is 3 : " << (dmin[2] )  << endl;
	//cout << "dmin is 4 : " << (dmin[3] )  << endl;

}
int main()
{
	TxRx();
	//record();

	system("pause");
	return 0;
}