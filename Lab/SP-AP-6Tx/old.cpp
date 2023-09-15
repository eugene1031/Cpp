// STBCSM_SPAP_6Tx_0610.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。

#define _CRT_SECURE_NO_WARNINGS
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
//noerror no white noise
#define  OUTPUTFILE  "addrateGSM(M=6 N=4)_ML_2.5db.txt"
#define  OPENSTRING  "#Eb/No(dB) BitErrRate         BitErrSample  TotalSample ======== DATE:1130 (6T4R)\n" 
#define  DATASTRING  "%5.2f      %18.16f %7d %11u \n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate
#define  DB0		30.0
#define  DBSTEP		5
#define  POINTNO	20
#define  ERRNOSTEP  2
#define  MAXERRNO	1900//2900//2000//1000//500
#define  SAMPLE_NUM 30000//500000//200000//100000
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              6
#define  N              1
#define  frame_w        20
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define  bits_num       24 // 12+12
#define  antenna_number 12 
#define	 data_rate		4 //25 / 6
#define  T				6

using namespace std;
using namespace Eigen;


#define  M3  0x00000004L
#define  M31 0x40000000L
#define  eulen(r1,i1,r2,i2) ((r1)-(r2))*((r1)-(r2))+((i1)-(i2))*((i1)-(i2))

FILE* fp;
static char  filename[80] = OUTPUTFILE;
static int	 v[2] = { -1, 1 };
int	 point, biterrno, errlevel, u, t, de_info, dsignal, samp;
long    pnstate, pntemp;
double   snrdb, snr, deviate;

double   drand48()
{
	double  w;
	/*RAND_MAX = 0xffff;*/
	w = (double)rand() / RAND_MAX;

	return  w;
}

void initial()
{
	if ((fp = fopen(filename, "a")) == NULL)
	{
		printf("\nOpen file error!\n");
		exit(1);
	}
	else
	{
		fprintf(fp, OPENSTRING);
		fclose(fp);
		printf("\nProgram Start...\n\n");
	}
}

double normal()
{
	static int iset = 0;
	static double gset;
	double fac, r, v1, v2;
	if (iset == 0)
	{
		do
		{
			v1 = 2.0 * drand48() - 1.0;
			v2 = 2.0 * drand48() - 1.0;
			r = v1 * v1 + v2 * v2;
		} while (r >= 1.0 || r == 0.0);
		fac = sqrt(-2.0 * log(r) / r);
		gset = v1 * fac;
		iset = 1;
		return(v2 * fac);
	}
	else
	{
		iset = 0;
		return gset;
	}
}

void record()
{
	fp = fopen(filename, "a");
	fprintf(fp, DATASTRING);
	fclose(fp);
}

int antenna_perm2[6][3] = { 1,2,3,	2,1,3,	3,1,2,
							 1,3,2, 2,3,1,	3,2,1
};

int interleaved_pattern1[60][6] = {

1, 2, 3, 4, 5, 6,  1, 2, 3, 5, 6, 4,  1, 2, 3, 6, 4, 5,
1, 3, 2, 4, 6, 5,  1, 3, 2, 5, 4, 6,  1, 3, 2, 6, 5, 4,
1, 3, 4, 2, 5, 6,  1, 3, 4, 5, 6, 2,  1, 3, 4, 6, 2, 5,
1, 3, 5, 2, 6, 4,  1, 3, 5, 4, 2, 6,  1, 3, 5, 6, 4, 2,
1, 3, 6, 2, 4, 5,  1, 3, 6, 4, 5, 2,  1, 3, 6, 5, 2, 4,
2, 1, 4, 3, 5, 6,  2, 1, 4, 5, 6, 3,  2, 1, 4, 6, 3, 5,
2, 4, 1, 3, 6, 5,  2, 4, 1, 6, 5, 3,  2, 4, 1, 5, 3, 6,
2, 4, 3, 1, 5, 6,  2, 4, 3, 5, 6, 1,  2, 4, 3, 6, 1, 5,
2, 4, 5, 1, 6, 3,  2, 4, 5, 6, 3, 1,  2, 4, 5, 3, 1, 6,
2, 4, 6, 1, 3, 5,  2, 4, 6, 3, 5, 1,  2, 4, 6, 5, 1, 3,
2, 1, 3, 4, 6, 5,  2, 1, 3, 6, 5, 4,  2, 1, 3, 5, 4, 6,
2, 3, 1, 4, 5, 6,  2, 3, 1, 5, 6, 4,  2, 3, 1, 6, 4, 5,
2, 3, 4, 1, 6, 5,  2, 3, 4, 6, 5, 1,  2, 3, 4, 5, 1, 6,
2, 3, 6, 1, 5, 4,  2, 3, 6, 5, 4, 1,  2, 3, 6, 4, 1, 5,
2, 3, 5, 1, 4, 6,  2, 3, 5, 4, 6, 1,  2, 3, 5, 6, 1, 4,
1, 2, 4, 3, 6, 5,  1, 2, 4, 6, 5, 3,  1, 2, 4, 5, 3, 6,
1, 4, 2, 3, 5, 6,  1, 4, 2, 5, 6, 3,  1, 4, 2, 6, 3, 5,
1, 4, 3, 2, 6, 5,  1, 4, 3, 6, 5, 2,  1, 4, 3, 5, 2, 6,
1, 4, 6, 2, 5, 3,  1, 4, 6, 5, 3, 2,  1, 4, 6, 3, 2, 5,
1, 4, 5, 2, 3, 6,  1, 4, 5, 3, 6, 2,  1, 4, 5, 6, 2, 3 };





int interleaved_pattern2[3][4] = {
									1,2,3,4,
									1,4,2,3,
									1,3,4,2
};


//==========================================================================================================================

MatrixXcd H(N, M), H_test(N, 2), noise(N, T);
MatrixXcd  Y1(N, 2), Y2(N, 2), Y3(N, 2), Y4(N, 2), y(N, T), NY(N, M);
MatrixXcd temp(N, 6), save(N, 6), Y1_temp(N, 2), Y2_temp(N, 2), Y3_temp(N, 2), Y4_temp(N, 2);
void fading_channel(int row, int col) {
	for (int a = 0; a < row; a++) {
		for (int b = 0; b < col; b++) {
			H(a, b) = (complex<double>(normal(), normal())) / sqrt(2.0);
		}
	}

}
MatrixXcd White_noise(int row, int col) {
	for (int a = 0; a < row; a++) {
		for (int b = 0; b < col; b++) {
			noise(a, b) = complex<double>(normal(), normal()) * deviate;
		}
	}
	return noise;
}


void receive()//void frame_length()
{
	//srand((unsigned)time(NULL));





	int input_bits[frame_w][bits_num], decode_bits[frame_w][bits_num], ab_save[frame_w][M], ab_save_temp[frame_w][M], m_save[frame_w], c_save[frame_w], final_save[frame_w], interleaved, reference_order, m, r, g;

	double norm_output = 0, norm_output1 = 0, norm_output2 = 0, norm_output3 = 0, norm_output4 = 0, min = 9999999, temp_min1 = 9999999, temp_min2 = 9999999, temp_min3 = 9999999, temp_min4 = 9999999;

	complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) }, Q[frame_w][M];

	MatrixXcd zero(2, 2), /*Y1(N, 2), Y2(N, 2),*/ I_s1s2(2, 2), I_s3s4(2, 2), I_s5s6(2, 2), I_s7s8(2, 2), I_diag(M, M), I_diag2(M, M), St1(M, M), /*y(N, M),*/ de_STBC(6, 6), de_STBC1(6, 6), de_STBC2(6, 6), de_STBC3(6, 6), de_STBC4(6, 6), de_STBC5(6, 6), de_STBC6(6, 6), de_STBCnew(6, 6), de_STBCnew1(6, 6), de_STBCnew2(6, 6);

	MatrixXcd st2(M, M), st3(M, T), st4(M, T), temp(M, 6), save(M, 6);

	MatrixXcd* I_s1s2_ptr = &I_s1s2;
	MatrixXcd* I_s3s4_ptr = &I_s3s4;
	MatrixXcd* I_s5s6_ptr = &I_s5s6;
	MatrixXcd* I_s7s8_ptr = &I_s7s8;
	MatrixXcd* H_ptr = &H;
	zero << 0, 0,
		0, 0;
	//===========================encoder_start===============================


	fading_channel(N, M);

	for (int i = 0; i < frame_w; i++)
	{
		for (int j = 0; j < bits_num; j++)
		{
			input_bits[i][j] = rand() % 2;
		}

		//antenna bits 8 9
		for (int j = 0; j < M; j++)
		{
			Q[i][j] = qpsk_map[(input_bits[i][j * 2 + antenna_number] << 1) + input_bits[i][j * 2 + (antenna_number + 1)]];
		}
		min = 999999;

		I_s1s2 << Q[i][0], -conj(Q[i][1]), Q[i][1], conj(Q[i][0]);
		I_s1s2 << 1 / sqrt(2.0) * I_s1s2;
		I_s3s4 << Q[i][2], -conj(Q[i][3]), Q[i][3], conj(Q[i][2]);
		I_s3s4 << 1 / sqrt(2.0) * I_s3s4;
		I_s5s6 << Q[i][4], -conj(Q[i][5]), Q[i][5], conj(Q[i][4]);
		I_s5s6 << 1 / sqrt(2.0) * I_s5s6;

		//	cout << I_s1s2 << endl;
		//remember to change the antenna bits

		m = (input_bits[i][11])       //m = 0 ~ 2^12 = 0 ~ 4096
			+ ((input_bits[i][10]) << 1)
			+ ((input_bits[i][9]) << 2)
			+ ((input_bits[i][8]) << 3)
			+ ((input_bits[i][7]) << 4)
			+ ((input_bits[i][6]) << 5)
			+ ((input_bits[i][5]) << 6)
			+ ((input_bits[i][4]) << 7)
			+ ((input_bits[i][3]) << 8)
			+ ((input_bits[i][2]) << 9)
			+ ((input_bits[i][1]) << 10)
			+ ((input_bits[i][0]) << 11);


		//m = 0;
		//cout << "Tx m = " << m << endl;
		//11111111->255 共256種

		r = m % 6;
		g = m / 6;

		if (r == 0) {
			I_diag <<
				I_s1s2, zero, zero,
				zero, I_s3s4, zero,
				zero, zero, I_s5s6;
		}
		else if (r == 1) {
			I_diag <<
				zero, I_s1s2, zero,
				I_s3s4, zero, zero,
				zero, zero, I_s5s6;
		}
		else if (r == 2) {
			I_diag <<
				zero, zero, I_s1s2,
				I_s3s4, zero, zero,
				zero, I_s5s6, zero;
		}
		else if (r == 3) {
			I_diag <<
				I_s1s2, zero, zero,
				zero, zero, I_s3s4,
				zero, I_s5s6, zero;
		}
		else if (r == 4) {
			I_diag <<
				zero, I_s1s2, zero,
				zero, zero, I_s3s4,
				I_s5s6, zero, zero;
		}
		else if (r == 5) {
			I_diag <<
				zero, zero, I_s1s2,
				zero, I_s3s4, zero,
				I_s5s6, zero, zero;
		}
		//new define 
		if (g < 60) {
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g][kk] - 1);
			}
			st4 << st2;
		}
		else if (60 * 1 - 1 < g && g < 60 * 2) {
			int g1 = g - 60 * 1;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g1][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[1][kk] - 1);
			}
		}
		else if (60 * 2 - 1 < g && g < 60 * 3) {
			int g2 = g - 60 * 2;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g2][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[2][kk] - 1);
			}
		}
		else if (60 * 3 - 1 < g && g < 60 * 4) {
			int g3 = g - 60 * 3;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g3][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[3][kk] - 1);
			}
		}
		else if (60 * 4 - 1 < g && g < 60 * 5) {
			int g4 = g - 60 * 4;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g4][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[4][kk] - 1);
			}
		}
		else if (60 * 5 - 1 < g && g < 60 * 6) {
			int g5 = g - 60 * 5;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g5][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[5][kk] - 1);
			}
		}
		else if (60 * 6 - 1 < g && g < 60 * 7) {
			int g6 = g - 60 * 6;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g6][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[6][kk] - 1);
			}
		}
		else if (60 * 7 - 1 < g && g < 60 * 8) {
			int g7 = g - 60 * 7;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g7][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[7][kk] - 1);
			}
		}
		else if (60 * 8 - 1 < g && g < 60 * 9) {
			int g8 = g - 60 * 8;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g8][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[8][kk] - 1);
			}
		}
		else if (60 * 9 - 1 < g && g < 60 * 10) {
			int g9 = g - 60 * 9;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g9][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[9][kk] - 1);
			}
		}
		else if (60 * 10 - 1 < g && g < 60 * 11) {
			int g10 = g - 60 * 10;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g10][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[10][kk] - 1);
			}
		}
		else if (60 * 11 - 1 < g && g < 60 * 12) {
			int g11 = g - 60 * 11;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g11][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[11][kk] - 1);
			}
		}
		else if (60 * 12 - 1 < g && g < 60 * 13) {
			int g12 = g - 60 * 12;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g12][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[12][kk] - 1);
			}
		}
		else if (60 * 13 - 1 < g && g < 60 * 14) {
			int g13 = g - 60 * 13;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g13][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[13][kk] - 1);
			}
		}
		else if (60 * 14 - 1 < g && g < 60 * 15) {
			int g14 = g - 60 * 14;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g14][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[14][kk] - 1);
			}
		}
		else if (60 * 15 - 1 < g && g < 60 * 16) {
			int g15 = g - 60 * 15;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g15][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[15][kk] - 1);
			}
		}
		else if (60 * 16 - 1 < g && g < 60 * 17) {
			int g16 = g - 60 * 16;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g16][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[16][kk] - 1);
			}
		}
		else if (60 * 17 - 1 < g && g < 60 * 18) {
			int g17 = g - 60 * 17;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g17][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[17][kk] - 1);
			}
		}
		else if (60 * 18 - 1 < g && g < 60 * 19) {
			int g18 = g - 60 * 18;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g18][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[18][kk] - 1);
			}
		}
		else if (60 * 19 - 1 < g && g < 60 * 20) {
			int g19 = g - 60 * 19;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g19][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[19][kk] - 1);
			}
		}
		else if (60 * 20 - 1 < g && g < 60 * 21) {
			int g20 = g - 60 * 20;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g20][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[20][kk] - 1);
			}
		}
		else if (60 * 21 - 1 < g && g < 60 * 22) {
			int g21 = g - 60 * 21;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g21][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[21][kk] - 1);
			}
		}
		else if (60 * 22 - 1 < g && g < 60 * 23) {
			int g22 = g - 60 * 22;
			for (int kk = 0; kk < 6; kk++) {
				st2.col(kk) << I_diag.col(interleaved_pattern1[g22][kk] - 1);
			}
			for (int kk = 0; kk < 6; kk++) {
				st4.row(kk) << st2.row(interleaved_pattern1[22][kk] - 1);
			}
		}




		y = H * st4 + White_noise(N, T);
		//cout << "st2 = " << st2 << endl << endl;
		//cout << "st4 = " << st4 << endl << endl;
		//cout << "y = " << y << endl << endl;
		temp_min1 = 999999;



		for (int mr = 0; mr < 4096; mr++)
		{
			reference_order = mr % 6;
			int inter = mr / 6;

			for (int a = 0; a < 4; a++)
			{
				for (int b = 0; b < 4; b++)
				{
					for (int c = 0; c < 4; c++)
					{
						for (int d = 0; d < 4; d++)
						{
							for (int e = 0; e < 4; e++)
							{
								for (int f = 0; f < 4; f++)
								{
									if (reference_order == 0) {
										de_STBC <<
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0,
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0,
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]);
										de_STBC = 1 / sqrt(2.0) * de_STBC; //除以根號二
									}

									else if (reference_order == 1) {
										de_STBC <<
											0, 0, qpsk_map[a], -conj(qpsk_map[b]), 0, 0,
											0, 0, qpsk_map[b], conj(qpsk_map[a]), 0, 0,
											qpsk_map[c], -conj(qpsk_map[d]), 0, 0, 0, 0,
											qpsk_map[d], conj(qpsk_map[c]), 0, 0, 0, 0,
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]);
										de_STBC = 1 / sqrt(2.0) * de_STBC; //除以根號二
									}
									else if (reference_order == 2) {
										de_STBC <<
											0, 0, 0, 0, qpsk_map[a], -conj(qpsk_map[b]),
											0, 0, 0, 0, qpsk_map[b], conj(qpsk_map[a]),
											qpsk_map[c], -conj(qpsk_map[d]), 0, 0, 0, 0,
											qpsk_map[d], conj(qpsk_map[c]), 0, 0, 0, 0,
											0, 0, qpsk_map[e], -conj(qpsk_map[f]), 0, 0,
											0, 0, qpsk_map[f], conj(qpsk_map[e]), 0, 0;
										de_STBC = 1 / sqrt(2.0) * de_STBC; //除以根號二
									}
									else if (reference_order == 3) {
										de_STBC <<
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0,
											0, 0, 0, 0, qpsk_map[c], -conj(qpsk_map[d]),
											0, 0, 0, 0, qpsk_map[d], conj(qpsk_map[c]),
											0, 0, qpsk_map[e], -conj(qpsk_map[f]), 0, 0,
											0, 0, qpsk_map[f], conj(qpsk_map[e]), 0, 0;
										de_STBC = 1 / sqrt(2.0) * de_STBC; //除以根號二
									}
									else if (reference_order == 4) {
										de_STBC <<
											0, 0, qpsk_map[a], -conj(qpsk_map[b]), 0, 0,
											0, 0, qpsk_map[b], conj(qpsk_map[a]), 0, 0,
											0, 0, 0, 0, qpsk_map[c], -conj(qpsk_map[d]),
											0, 0, 0, 0, qpsk_map[d], conj(qpsk_map[c]),
											qpsk_map[e], -conj(qpsk_map[f]), 0, 0, 0, 0,
											qpsk_map[f], conj(qpsk_map[e]), 0, 0, 0, 0;
										de_STBC = 1 / sqrt(2.0) * de_STBC; //除以根號二
									}
									else if (reference_order == 5) {
										de_STBC <<
											0, 0, 0, 0, qpsk_map[a], -conj(qpsk_map[b]),
											0, 0, 0, 0, qpsk_map[b], conj(qpsk_map[a]),
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0,
											qpsk_map[e], -conj(qpsk_map[f]), 0, 0, 0, 0,
											qpsk_map[f], conj(qpsk_map[e]), 0, 0, 0, 0;
										de_STBC = 1 / sqrt(2.0) * de_STBC; //除以根號二
									}

									if (inter < 60) {

										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter][kk] - 1);
										}
										de_STBCnew1 << de_STBCnew;
									}
									else if (60 - 1 < inter && inter < 60 * 2) {
										int inter1 = inter - 60;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter1][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[1][kk] - 1);
										}
									}
									else if (60 * 2 - 1 < inter && inter < 60 * 3) {
										int inter2 = inter - 60 * 2;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter2][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[2][kk] - 1);
										}
									}
									else if (60 * 3 - 1 < inter && inter < 60 * 4) {
										int inter3 = inter - 60 * 3;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter3][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[3][kk] - 1);
										}
									}
									else if (60 * 4 - 1 < inter && inter < 60 * 5) {
										int inter4 = inter - 60 * 4;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter4][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[4][kk] - 1);
										}
									}
									else if (60 * 5 - 1 < inter && inter < 60 * 6) {
										int inter5 = inter - 60 * 5;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter5][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[5][kk] - 1);
										}
									}
									else if (60 * 6 - 1 < inter && inter < 60 * 7) {
										int inter6 = inter - 60 * 6;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter6][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[6][kk] - 1);
										}
									}
									else if (60 * 7 - 1 < inter && inter < 60 * 8) {
										int inter7 = inter - 60 * 7;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter7][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[7][kk] - 1);
										}
									}
									else if (60 * 8 - 1 < inter && inter < 60 * 9) {
										int inter8 = inter - 60 * 8;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter8][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[8][kk] - 1);
										}
									}
									else if (60 * 9 - 1 < inter && inter < 60 * 10) {
										int inter9 = inter - 60 * 9;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter9][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[9][kk] - 1);
										}
									}
									else if (60 * 10 - 1 < inter && inter < 60 * 11) {
										int inter10 = inter - 60 * 10;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter10][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[10][kk] - 1);
										}
									}
									else if (60 * 11 - 1 < inter && inter < 60 * 12) {
										int inter11 = inter - 60 * 11;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter11][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[11][kk] - 1);
										}
									}
									else if (60 * 12 - 1 < inter && inter < 60 * 13) {
										int inter12 = inter - 60 * 12;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter12][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[12][kk] - 1);
										}
									}
									else if (60 * 13 - 1 < inter && inter < 60 * 14) {
										int inter13 = inter - 60 * 13;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter13][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[13][kk] - 1);
										}
									}
									else if (60 * 14 - 1 < inter && inter < 60 * 15) {
										int inter14 = inter - 60 * 14;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter14][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[14][kk] - 1);
										}
									}
									else if (60 * 15 - 1 < inter && inter < 60 * 16) {
										int inter15 = inter - 60 * 15;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter15][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[15][kk] - 1);
										}
									}
									else if (60 * 16 - 1 < inter && inter < 60 * 17) {
										int inter16 = inter - 60 * 16;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter16][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[16][kk] - 1);
										}
									}
									else if (60 * 17 - 1 < inter && inter < 60 * 18) {
										int inter17 = inter - 60 * 17;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter17][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[17][kk] - 1);
										}
									}
									else if (60 * 18 - 1 < inter && inter < 60 * 19) {
										int inter18 = inter - 60 * 18;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter18][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[18][kk] - 1);
										}
									}
									else if (60 * 19 - 1 < inter && inter < 60 * 20) {
										int inter19 = inter - 60 * 19;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter19][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[19][kk] - 1);
										}
									}
									else if (60 * 20 - 1 < inter && inter < 60 * 21) {
										int inter20 = inter - 60 * 20;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter20][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[20][kk] - 1);
										}
									}
									else if (60 * 21 - 1 < inter && inter < 60 * 22) {
										int inter21 = inter - 60 * 21;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter21][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[21][kk] - 1);
										}
									}
									else if (60 * 22 - 1 < inter && inter < 60 * 23) {
										int inter22 = inter - 60 * 22;
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew.col(kk) << de_STBC.col(interleaved_pattern1[inter22][kk] - 1);
										}
										for (int kk = 0; kk < 6; kk++) {
											de_STBCnew1.row(kk) << de_STBCnew.row(interleaved_pattern1[22][kk] - 1);
										}
									}



									norm_output1 = (y - H * de_STBCnew1).norm();




									if (norm_output1 < temp_min1) {
										temp_min1 = norm_output1;
										ab_save_temp[i][0] = a;
										ab_save_temp[i][1] = b;
										ab_save_temp[i][2] = c;
										ab_save_temp[i][3] = d;
										ab_save_temp[i][4] = e;
										ab_save_temp[i][5] = f;
										//cout << "x = " << H * de_STBCnew1 << endl << endl;
										//cout << "de_STBCnew1 = " <<  de_STBCnew1 << endl << endl;
									}
								}
							}
						}
					}
				}
			}
			norm_output = temp_min1;
			//cout << "mr = " << mr << endl;
			//	cout << "norm_output = " << norm_output << endl; ;
			if (norm_output < min)
			{
				min = norm_output;
				//cout << "   min = " << min << endl;
				m_save[i] = mr;
				ab_save[i][0] = ab_save_temp[i][0];
				ab_save[i][1] = ab_save_temp[i][1];
				ab_save[i][2] = ab_save_temp[i][2];
				ab_save[i][3] = ab_save_temp[i][3];
				ab_save[i][4] = ab_save_temp[i][4];
				ab_save[i][5] = ab_save_temp[i][5];
			}
		}
		//
		//cout << "m_save=" << m_save[i] << endl;
		//cout << "min=" << min << endl;
		min = 9999999;
		norm_output = 0;
	}

	for (int i = 0; i < frame_w; i++)
	{
		decode_bits[i][11] = (m_save[i] % 2);
		decode_bits[i][10] = ((m_save[i] >> 1) % 2);
		decode_bits[i][9] = ((m_save[i] >> 2) % 2);
		decode_bits[i][8] = ((m_save[i] >> 3) % 2);
		decode_bits[i][7] = ((m_save[i] >> 4) % 2);
		decode_bits[i][6] = ((m_save[i] >> 5) % 2);
		decode_bits[i][5] = ((m_save[i] >> 6) % 2);
		decode_bits[i][4] = ((m_save[i] >> 7) % 2);
		decode_bits[i][3] = ((m_save[i] >> 8) % 2);
		decode_bits[i][2] = ((m_save[i] >> 9) % 2);
		decode_bits[i][1] = ((m_save[i] >> 10) % 2);
		decode_bits[i][0] = ((m_save[i] >> 11) % 2);

		for (int k = 0; k < 6; k++) {
			decode_bits[i][k * 2 + 12] = (ab_save[i][k] >> 1) % 2;
			decode_bits[i][k * 2 + 13] = (ab_save[i][k]) % 2;
		}

		for (int j = 0; j < bits_num; j++)
		{
			//			cout << decode_bits[i][j] << " " ;
			if (decode_bits[i][j] != input_bits[i][j]) {
				biterrno++;

			}
		}
		dsignal += bits_num;

	}

	///////檢查
	//for (int i = 0; i < frame_w; i++) {
	//	for (int j = 0; j < bits_num; j++) {
	//		cout << input_bits[i][j];
	//	}
	//	puts("");
	//	for (int j = 0; j < bits_num; j++) {
	//		cout << decode_bits[i][j];
	//	}
	//	puts("");
	//	puts("");
	//}
}

int main()
{

	printf("%dT%dR DB0:%f spap6Tx", M, N, DB0);
	srand((unsigned)time(NULL));
	initial();
	for (point = 0; point < POINTNO; point++)
	{
		pnstate = 0xaaaaaaaaL;
		snrdb = DB0 + point * DBSTEP;
		snr = (data_rate)*pow(10.0, 0.1 * snrdb);
		deviate = sqrt(0.5 / snr);
		biterrno = 0;
		errlevel = ERRNOSTEP;
		dsignal = 0;

		while (biterrno < MAXERRNO)
		{
			if (biterrno > errlevel)
			{
				errlevel += ERRNOSTEP;
				//printf("%5.2f   %18.16f   %7d   %11u\n ",(snrdb),(double) biterrno/dsignal,biterrno,dsignal,deviate);
			}
			for (samp = 0; samp < SAMPLE_NUM; samp++) {
				receive();
				printf("%5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
			}
		}
		//record();
		//printf("%5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
		//system("pause");
	}
	system("pause");
	return 0;
}


