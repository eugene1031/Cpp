#pragma warning(disable : 4996)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <complex> 
#include <vector>
#include <cstdlib>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#define  OUTPUTFILE  "SA-SM4Tx.txt"
#define  OUTPUTFILED "SA-SM4Txdebug.txt"

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
double   snrdb, snr, deviate, dmin, dmin1;

MatrixXcd X1(2, 2);
MatrixXcd X2(2, 2);
MatrixXcd X3(2, 2);
MatrixXcd Xp1(2, 2);
MatrixXcd Xp2(2, 2);
MatrixXcd Xp3(2, 2);

complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) };
//complex<double> qpsk_map[4] = { complex<double>(0.7071,0.7071),complex<double>(-0.7071,0.7071),complex<double>(-0.7071,-0.7071),complex<double>(0.7071,-0.7071) };

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
MatrixXcd I_diag_new(M, M);

int gt1 = 0;
int gt2 = 0;
int g1 = 0;
int g2 = 0;

int m = 0;

MatrixXcd H(N, M), H_test(N, M / 2), noise(N, M);

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

int find3And4Zero(int x, int y, double a, double b, double c, double d) {
	int count = 0;
	if (fabs(a) < 1e-6)
		count = count + 1;
	if (fabs(b) < 1e-6)
		count = count + 1;
	if (fabs(c) < 1e-6)
		count = count + 1;
	if (fabs(d) < 1e-6)
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

void TxRx()
{
	//輸出.txt要加的!!
	fp = fopen(filename, "a");
	fpDebug = fopen(fpDebugFilename, "a");

	MatrixXcd zero(2, 2);
	zero << 0, 0,
		0, 0;

	MatrixXcd Xt0(M, M);
	MatrixXcd Xt1(M, M);

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

			I_s1s2 << s[0][0], s[0][1], -conj(s[0][1]), conj(s[0][0]);

			Q_s1s2 << s[1][0], s[1][1], -conj(s[1][1]), conj(s[1][0]);

			m_a = ((a / 32) % 2) * 2 + ((a / 16) % 2);
            
            if(m_a == 0){
                I_diag << I_s1s2, zero;
                //I_diag << (1 / sqrt(2.0)) * I_diag;
            }
            else if (m_a == 1) {
						I_diag <<
							s[0][0], 0,
							0, conj(s[0][0]),
							s[0][1], 0,
							0, conj(s[0][1]);
						//I_diag << (1 / sqrt(2.0)) * I_diag;
					}
			else if (m_a == 2) {
						I_diag <<
							s[0][0], 0,
							0, conj(s[0][0]),
							0, s[0][1],
							-conj(s[0][1]), 0;
						 //I_diag << (1 / sqrt(2.0)) * I_diag;
					}
			else if (m_a == 3) {
						I_diag <<
							0, s[0][0],
							-conj(s[0][0]), 0,
							s[0][1], 0,
							0, conj(s[0][1]);
						//I_diag << (1 / sqrt(2.0)) * I_diag;
					}

			m_b = ((b / 32) % 2) * 2 + ((b / 16) % 2);

            if(m_b == 0){
                Q_diag << Q_s1s2, zero;
                //Q_diag << (1 / sqrt(2.0)) * Q_diag;
            }
            else if (m_b == 1) {
						Q_diag <<
							s[1][0], 0,
							0, conj(s[1][0]),
							s[1][1], 0,
							0, conj(s[1][1]);
						//Q_diag << (1 / sqrt(2.0)) * Q_diag;
						//cout << "xr_2=" << H * xr << endl;
					}
			else if (m_b == 2) {
						Q_diag <<
							s[1][0], 0,
							0, conj(s[1][0]),
							0, s[1][1],
							-conj(s[1][1]), 0;
						// Q_diag << (1 / sqrt(2.0)) * Q_diag;
						//cout << "xr_2=" << H * xr << endl;
					}
			else if (m_b == 3) {
						Q_diag <<
							0, s[1][0],
							-conj(s[1][0]), 0,
							s[1][1], 0,
							0, conj(s[1][1]);
						//Q_diag << (1 / sqrt(2.0)) * Q_diag;
						//cout << "xr_2=" << H * xr << endl;
					}
					// cout << "I:" << endl 
					// 	 << I_diag << endl;
					// cout << "Q:" << endl
					// 	 << Q_diag << endl;
					// break;
					// 論文裡面的公式
					r_matrix << (I_diag - Q_diag);
					Dij_matrix << (r_matrix) * (r_matrix.adjoint());
					//cout << Dij_matrix << endl;
					/*
					cout << "算出來的特徵值: ";
					for (int k = 0; k < M; k++)
						cout << " " << ((Dij_matrix.eigenvalues()).real())(k);;
					puts("");
					*/

					// Debug find zero numbers bigger than 3
					if (find3And4Zero(a, b, ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3)) > 2)
					{
						fprintf(fpDebug, "%d    %.5f%10.5f%10.5f%10.5f  (%d,%d)\n", hammingDistance(a, b), ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), a, b);
			}
			if(a < b){// && s[0][0] == s[1][0] && s[0][1] == s[1][1]){
				// ofstream excel;
				// excel.open("SA-SM-Bound.csv");
				//if((((Dij_matrix.eigenvalues()).real())(2) * ((Dij_matrix.eigenvalues()).real())(3)) < 12){
				fprintf(fp, "%d    %.5f%10.5f%10.5f%10.5f%10.5f\n", hammingDistance(a, b), abs(((Dij_matrix.eigenvalues()).real())(0)), abs(((Dij_matrix.eigenvalues()).real())(1)), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), (((Dij_matrix.eigenvalues()).real())(2) * ((Dij_matrix.eigenvalues()).real())(3)));//, a, b);
				//}
				// if(excel.is_open())
				// {excel << ((Dij_matrix.eigenvalues()).real())(2) << ',' << ((Dij_matrix.eigenvalues()).real())(3)<< ',' << (((Dij_matrix.eigenvalues()).real())(2) * ((Dij_matrix.eigenvalues()).real())(3))<< ',' << a<< ',' << b<< ',';
				// excel << endl;
				}
			}
			/*
			if (((Dij_matrix.eigenvalues()).real())(0) == 0 && ((Dij_matrix.eigenvalues()).real())(1) == 0) {
				//cout << "diversity 為2的情況下的dmin，";
				dmin1 = abs(((Dij_matrix.eigenvalues()))(2) * ((Dij_matrix.eigenvalues()))(3));
				if(dmin1 < 1.0 && dmin1 != 1.0){
					cout << "dmin1: " << dmin1 << endl;
				}
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
			*/
			



			}//b!=a


		}//b for

		// cout << "dmin is 1 :" << temp_dmin[0] << endl;
		// cout << "dmin is 2 :" << temp_dmin[1] << endl;
		// cout << "dmin is 3 :" << temp_dmin[2] << endl;
		// cout << "dmin is 4 :" << temp_dmin[3] << endl<<endl;
		

		// statistics_dmin[a][0] = temp_dmin[0];
		// statistics_dmin[a][1] = temp_dmin[1];
		// statistics_dmin[a][2] = temp_dmin[2];
		// statistics_dmin[a][3] = temp_dmin[3];


		// temp_dmin[0] = 0;
		// temp_dmin[1] = 0;
		// temp_dmin[2] = 0;
		// temp_dmin[3] = 0;
		//printf("diversity is 4 : %d\n", four);
		//printf("diversity is 2 : %d\n", two);
	}
// 	double dmin[M];
// 	for (int jj = 0; jj < codeword - 1; jj++) {

// 		dmin[0] += statistics_dmin[jj][0];
// 		dmin[1] += statistics_dmin[jj][1];
// 		dmin[2] += statistics_dmin[jj][2];
// 		dmin[3] += statistics_dmin[jj][3];
// 		fclose(fp);
// 	}
// 	cout << "dmin is 1 : " << (dmin[0] * 2) / codeword << endl;
// 	cout << "dmin is 2 : " << (dmin[1] * 2) / (codeword) << endl;
// 	cout << "dmin is 3 : " << (dmin[2] * 2) / (codeword) << endl;
// 	cout << "dmin is 4 : " << (dmin[3] * 2) / (codeword) << endl;

// 	//cout << "dmin is 1 : " << (dmin[0] )  << endl;
// 	//cout << "dmin is 2 : " << (dmin[1] )  << endl;
// 	//cout << "dmin is 3 : " << (dmin[2] )  << endl;
// 	//cout << "dmin is 4 : " << (dmin[3] )  << endl;

// }
int main()
{
	TxRx();
	//record();

	system("pause");
	return 0;
}