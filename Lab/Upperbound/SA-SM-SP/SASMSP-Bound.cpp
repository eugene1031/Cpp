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

#define  OUTPUTFILE  "SA-SM-SP4Tx.txt"
#define  OUTPUTFILED "SA-SM-SP4Txdebug.txt"

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
#define  codeword		1024 //=y 代表看x個bits 2^x = y  //bitsnum=12


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

MatrixXcd I_s1s2(2, 2);
MatrixXcd I_s3s4(2, 2);

MatrixXcd Q_s1s2(2, 2);
MatrixXcd Q_s3s4(2, 2);

MatrixXcd r_matrix(M, M), Dij_matrix(M, M);
MatrixXcd Q_diag(M, 2), Q_diag1(M, 2), Q_final(M, M),new_Q_diag(M, M);
MatrixXcd I_diag(M, 2), I_diag1(M, 2), I_final(M, M), new_I_diag(M, M);
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
									//1,4,2,3,
									2,1,4,3,
	//2,4,3,1,
	//2,3,1,4
};
int antenna_perm[2][2] = { 1,2,
							2,1 };
//要用的天線

int two_time_ant_per[4][1] = { 1,
							2,
							3,
							4 };

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
	for (int a = 0; a < 256; a++) { // a < codeword - 1

		for (int b = 256; b < 512; b++) {
			cout << " a = " << a << " b = " << b << endl;

			//cout << "hamming Distance = " << hammingDistance(a, b) << endl;
			if (a < b) {
			//10進制轉2進制，再兩個一組轉10進制
            s[0][0] = qpsk_map[((a / 128) % 2) * 2 + ((a / 64) % 2)];
			s[0][1] = qpsk_map[((a / 32) % 2) * 2 + ((a / 16) % 2)];
			s[0][2] = qpsk_map[((a / 8) % 2) * 2 + ((a / 4) % 2)];
			s[0][3] = qpsk_map[((a / 2) % 2) * 2 + (a % 2)];

			s[1][0] = qpsk_map[((b / 128) % 2) * 2 + ((b / 64) % 2)];
			s[1][1] = qpsk_map[((b / 32) % 2) * 2 + ((b / 16) % 2)];
			s[1][2] = qpsk_map[((b / 8) % 2) * 2 + ((b / 4) % 2)];
			s[1][3] = qpsk_map[((b / 2) % 2) * 2 + (b % 2)];

			I_s1s2 << s[0][0], s[0][1], -conj(s[0][1]), conj(s[0][0]);

			Q_s1s2 << s[1][0], s[1][1], -conj(s[1][1]), conj(s[1][0]);

			m_a = ((a / 512) % 2) * 2 + ((a / 256) % 2);
            
                int ant1 = 3; //0, 1, 2, 3
                int ant2 = 2;  // 0, 1, 2, 3

                ///First block
                switch (ant1)
                {
                case 0:
                I_diag << I_s1s2, zero;
                    break;
                case 1:
                    /// codeword1
                    I_diag << s[0][0], 0,
                        0, conj(s[0][0]),
                        s[0][1], 0,
                        0, conj(s[0][1]);
                    break;
                case 2:
                /// codeword2
               I_diag <<
							s[0][0], 0,
							0, conj(s[0][0]),
							0, s[0][1],
							-conj(s[0][1]), 0;
                break;
                case 3:
                /// codeword3
               I_diag <<
							0, s[0][0],
							-conj(s[0][0]), 0,
							s[0][1], 0,
							0, conj(s[0][1]);
                break;
                }

                ///Second block
                switch (ant2)
                {
                case 0:
                I_diag1 << I_s1s2, zero;
                break;
                case 1:
                /// codeword1
                I_diag1 <<
                			s[0][2], 0,
                			0, conj(s[0][2]),
                			s[0][3], 0,
                			0, conj(s[0][3]);
                break;
                case 2:
                /// codeword2
               I_diag1 <<
							s[0][2], 0,
							0, conj(s[0][2]),
							0, s[0][3],
							-conj(s[0][3]), 0;
                break;
                case 3:
                /// codeword3
               I_diag1 <<
							0, s[0][2],
							-conj(s[0][2]), 0,
							s[0][3], 0,
							0, conj(s[0][3]);
                break;
                }

                I_final << I_diag, I_diag1;

            // if(m_a < 3){
            //     for (int kk = 0; kk < 4; kk++) {
			// 	new_I_diag.col(kk) << I_final.col(interleaved_pattern[m_a][kk] - 1);
			//     }
            // }
            
			m_b = ((b / 512) % 2) * 2 + ((b / 256) % 2);

                ///First block
                switch (ant1)
                {
                case 0:
                Q_diag << Q_s1s2, zero;
                break;
                case 1:
                /// codeword1
                Q_diag <<
                			s[1][0], 0,
                			0, conj(s[1][0]),
                			s[1][1], 0,
                			0, conj(s[1][1]);
                break;
                case 2:
                /// codeword2
               Q_diag <<
							s[1][0], 0,
							0, conj(s[1][0]),
							0, s[1][1],
							-conj(s[1][1]), 0;
                break;
                case 3:
                /// codeword3
               Q_diag <<
							0, s[1][0],
							-conj(s[1][0]), 0,
							s[1][1], 0,
							0, conj(s[1][1]);
                break;
                }

                ///Second block
                switch (ant2)
                {
                case 0:
                Q_diag1 << Q_s1s2, zero;
                break;
                case 1:
                /// codeword1
                Q_diag1 <<
                			s[1][2], 0,
                			0, conj(s[1][2]),
                			s[1][3], 0,
                			0, conj(s[1][3]);
                break;
                case 2:
                /// codeword2
               Q_diag1 <<
							s[1][2], 0,
							0, conj(s[1][2]),
							0, s[1][3],
							-conj(s[1][3]), 0;
                break;
                case 3:
                /// codeword3
               Q_diag1 <<
							0, s[1][2],
							-conj(s[1][2]), 0,
							s[1][3], 0,
							0, conj(s[1][3]);
                break;
                }

                Q_final << Q_diag, Q_diag1;

                ///interleaving
                for (int ii = 0; ii < 4; ii++) {
				new_Q_diag.col(ii) << Q_final.col(interleaved_pattern[2][ii] - 1);
			    }

			//論文裡面的公式
			r_matrix << (I_final - new_Q_diag);
			Dij_matrix << (r_matrix) * (r_matrix.adjoint());

			/*
			cout << "算出來的特徵值: ";
			for (int k = 0; k < M; k++)
				cout << " " << ((Dij_matrix.eigenvalues()).real())(k);;
			puts("");
			*/
            //cout << find3And4Zero(a, b, ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3)) << endl;
            //break;
            // Debug find zero numbers bigger than 3
            if (find3And4Zero(a, b, ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3)) < 2)
			{
				fprintf(fpDebug, "%d    %.5f%10.5f%10.5f%10.5f%10.5f  (%d,%d)\n", hammingDistance(a, b), abs(((Dij_matrix.eigenvalues()).real())(0)), abs(((Dij_matrix.eigenvalues()).real())(1)), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), (((Dij_matrix.eigenvalues()).real())(1) * ((Dij_matrix.eigenvalues()).real())(2) * ((Dij_matrix.eigenvalues()).real())(3)), a, b);
			}
            if (find3And4Zero(a, b, ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3)) == 2)
            {
			fprintf(fp, "%d    %.5f%10.5f%10.5f%10.5f%10.5f%10d%10d\n", hammingDistance(a, b), abs(((Dij_matrix.eigenvalues()).real())(0)), abs(((Dij_matrix.eigenvalues()).real())(1)), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), (((Dij_matrix.eigenvalues()).real())(2) * ((Dij_matrix.eigenvalues()).real())(3)), a, b);
            }
		}//b!=a
    }//b for

	}
	double dmin[M];
	for (int jj = 0; jj < codeword - 1; jj++) {

		dmin[0] += statistics_dmin[jj][0];
		dmin[1] += statistics_dmin[jj][1];
		dmin[2] += statistics_dmin[jj][2];
		dmin[3] += statistics_dmin[jj][3];
		fclose(fp);
	}

}
int main()
{
	TxRx();
	//record();

	system("pause");
	return 0;
}