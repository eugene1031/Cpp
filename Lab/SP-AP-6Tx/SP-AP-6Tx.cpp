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

#define  OUTPUTFILE  "addrateGSM(M=6 N=4)_ML_2.5db.txt"
#define  OPENSTRING  "#Eb/No(dB) BitErrRate         BitErrSample  TotalSample ======== DATE:1130 (6T4R)\n" 
#define  DATASTRING  "%5.2f      %18.16f %7d %11u \n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate
#define  DB0		0.0
#define  DBSTEP		5
#define  POINTNO	20
#define  ERRNOSTEP  2
#define  MAXERRNO	1900//2900//2000//1000//500
#define  SAMPLE_NUM 30000//500000//200000//100000
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              6
#define  N              1
#define  frame_w        25
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define  bits_num       24 //原本19
#define  antenna_number 12  //原本6
#define	 data_rate  3.33333333333 //原本 19/6
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
//==========================================================================
int antenna_perm1[2][3] = { 1,1,1,
							2,2,2
};

int antenna_perm2[6][3] = { 1,2,3,	2,1,3,	3,1,2,
							 1,3,2, 2,3,1,	3,2,1
};

int antenna_perm3[12][3] = { 1,1,2,	1,1,3,	1,2,1,	1,3,1,
							 2,1,2, 2,2,1,	2,2,3,	2,3,2,
							 3,1,3,	3,2,3,	3,3,1,	3,3,2
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

MatrixXcd H(N, T), H_test(N, 2), noise(N, T);
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

MatrixXcd channel_estimation(MatrixXcd* H, int* reference_order) {

	if (*reference_order == 1) {
		H_test.col(0) << (*H).col(0);
		H_test.col(1) << (*H).col(1);

	}
	if (*reference_order == 2) {
		H_test.col(0) << (*H).col(2);
		H_test.col(1) << (*H).col(3);

	}
	if (*reference_order == 3) {
		H_test.col(0) << (*H).col(4);
		H_test.col(1) << (*H).col(5);

	}

	return H_test;
}


void Y1Y2Y3(int c, int a) {
    int col = c / 6 % 60;
    int row = c / 6 / 60;
    if (a == 1)
    {
        //new define
        //NY.row(interleaved_pattern1[row][0] - 1) << y.row(0);
		//NY.row(interleaved_pattern1[row][1] - 1) << y.row(1);
		//NY.row(interleaved_pattern1[row][2] - 1) << y.row(2);
		//NY.row(interleaved_pattern1[row][3] - 1) << y.row(3);
		//NY.row(interleaved_pattern1[row][4] - 1) << y.row(4);
		//NY.row(interleaved_pattern1[row][5] - 1) << y.row(5);


		NY.col(interleaved_pattern1[col][0] - 1) << y.col(0);
		NY.col(interleaved_pattern1[col][1] - 1) << y.col(1);
		NY.col(interleaved_pattern1[col][2] - 1) << y.col(2);
		NY.col(interleaved_pattern1[col][3] - 1) << y.col(3);
		NY.col(interleaved_pattern1[col][4] - 1) << y.col(4);
		NY.col(interleaved_pattern1[col][5] - 1) << y.col(5);


		Y1.col(0) << NY.col(0);
		Y1.col(1) << NY.col(1);
		Y2.col(0) << NY.col(2);
		Y2.col(1) << NY.col(3);
		Y3.col(0) << NY.col(4);
		Y3.col(1) << NY.col(5);
    }

    else if (a == 2) {

		Y1.col(0) << y.col(0);
		Y1.col(1) << y.col(1);
		Y2.col(0) << y.col(interleaved_pattern2[c][0] + 1);
		Y2.col(1) << y.col(interleaved_pattern2[c][1] + 1);
		Y3.col(0) << y.col(interleaved_pattern2[c][2] + 1);
		Y3.col(1) << y.col(interleaved_pattern2[c][3] + 1);
	}

	else if (a == 3) {

		Y1.col(0) << y.col(0);
		Y1.col(1) << y.col(1);
		Y2.col(0) << y.col(2);
		Y2.col(1) << y.col(3);
		Y3.col(0) << y.col(4);
		Y3.col(1) << y.col(5);

	}
}

MatrixXcd ret(int x) {
	MatrixXcd antenna1(2, M); antenna1 << 1, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0;
	MatrixXcd antenna2(2, M); antenna2 << 0, 0, 1, 0, 0, 0,
		0, 0, 0, 1, 0, 0;
	MatrixXcd antenna3(2, M); antenna3 << 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 1;

	if (x == 1) return antenna1;
	else if (x == 2) return antenna2;
	else if (x == 3) return antenna3;

}
MatrixXcd generation(int* x, int* x1, int* x2, MatrixXcd* I_s1s2, MatrixXcd* I_s3s4, MatrixXcd* I_s5s6) {  //轉至後位子會跑掉，所以先給一個假的

	MatrixXcd time12(M, 2), time34(M, 2), time56(M, 2), time78(M, 2), final_mat(M, M);

	(*I_s1s2)(0, 1) = -conj((*I_s1s2)(0, 1));
	(*I_s1s2)(1, 0) = -conj((*I_s1s2)(1, 0));
	(*I_s3s4)(0, 1) = -conj((*I_s3s4)(0, 1));
	(*I_s3s4)(1, 0) = -conj((*I_s3s4)(1, 0));
	(*I_s5s6)(0, 1) = -conj((*I_s5s6)(0, 1));
	(*I_s5s6)(1, 0) = -conj((*I_s5s6)(1, 0));


	time12 << ((*I_s1s2) * ret(*x)).transpose();
	time34 << ((*I_s3s4) * ret(*x1)).transpose();
	time56 << ((*I_s5s6) * ret(*x2)).transpose();

	//	cout << time12 << endl;
	final_mat << time12, time34, time56;
	return final_mat;
}

void receive()//void frame_length()
{
	//srand((unsigned)time(NULL));
	int input_bits[frame_w][bits_num], decode_bits[frame_w][bits_num], ab_save[frame_w][M], ab_save_temp[frame_w][M], m_save[frame_w], c_save[frame_w], final_save[frame_w], interleaved, reference_order, m, r, g;

	double norm_output = 0, norm_output1 = 0, norm_output2 = 0, norm_output3 = 0, norm_output4 = 0, min = 9999999, temp_min1 = 9999999, temp_min2 = 9999999, temp_min3 = 9999999, temp_min4 = 9999999;

	complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) }, Q[frame_w][M];

	MatrixXcd zero(2, 2), /*Y1(N, 2), Y2(N, 2),*/ I_s1s2(2, 2), I_s3s4(2, 2), I_s5s6(2, 2), I_s7s8(2, 2), I_diag(M, M), I_diag2(M, M), St1(M, M), /*y(N, M),*/ de_STBC(2, 2);

	MatrixXcd st4(M, T), st2(M, T), temp(M, 6), save(M, 6);

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
		//rember to change 
		//antenna bits 8 9
		for (int j = 0; j < M; j++)
		{
			Q[i][j] = qpsk_map[(input_bits[i][j * 2 + antenna_number] << 1) + input_bits[i][j * 2 + (antenna_number + 1)]];
		}

		min = 999999;
		//	cout<<"----------------------"<<endl;
		I_s1s2 << Q[i][0], -conj(Q[i][1]), Q[i][1], conj(Q[i][0]);
		I_s1s2 << 1 / sqrt(2.0) * I_s1s2;
		I_s3s4 << Q[i][2], -conj(Q[i][3]), Q[i][3], conj(Q[i][2]);
		I_s3s4 << 1 / sqrt(2.0) * I_s3s4;
		I_s5s6 << Q[i][4], -conj(Q[i][5]), Q[i][5], conj(Q[i][4]);
		I_s5s6 << 1 / sqrt(2.0) * I_s5s6;

		m = (input_bits[i][0])
			+ ((input_bits[i][1]) << 1)
			+ ((input_bits[i][2]) << 2)
			+ ((input_bits[i][3]) << 3)
			+ ((input_bits[i][4]) << 4)
			+ ((input_bits[i][5]) << 5)
			+ ((input_bits[i][6]) << 6)
			+ ((input_bits[i][7]) << 7)
            + ((input_bits[i][8]) << 8)
            + ((input_bits[i][9]) << 9)
            + ((input_bits[i][10]) << 10)
            + ((input_bits[i][11]) << 11);
		//cout <<"Tx m = "<< m << endl;
		if (m < 4096) {
			r = m % 6;
			g = m / 6;
            int col = g % 60;
            int row = g / 60;

            I_diag << generation(&antenna_perm2[r][0], &antenna_perm2[r][1], &antenna_perm2[r][2], I_s1s2_ptr, I_s3s4_ptr, I_s5s6_ptr);

            //new define
			st4.col(0) << I_diag.col(interleaved_pattern1[col][0] - 1);
			st4.col(1) << I_diag.col(interleaved_pattern1[col][1] - 1);
			st4.col(2) << I_diag.col(interleaved_pattern1[col][2] - 1);
			st4.col(3) << I_diag.col(interleaved_pattern1[col][3] - 1);
			st4.col(4) << I_diag.col(interleaved_pattern1[col][4] - 1);
			st4.col(5) << I_diag.col(interleaved_pattern1[col][5] - 1);
        for (int kk = 0; kk < 6; kk++) {
				st2.row(kk) << st4.row(interleaved_pattern1[row][kk] - 1);
			}
		}

		y = H * st2;// +White_noise(N, T);

		for (int mr = 0; mr < 4096; mr++)
		{

			temp_min1 = 999999;
			temp_min2 = 999999;
			temp_min3 = 999999;
			temp_min4 = 999999;
			for (int a = 0; a < 4; a++)
			{
				for (int b = 0; b < 4; b++)
				{
					de_STBC << qpsk_map[a], -conj(qpsk_map[b]), qpsk_map[b], conj(qpsk_map[a]);
					de_STBC = 1 / sqrt(2.0) * de_STBC; //除以根號二
					if (mr < 4096) {
						interleaved = mr;
						reference_order = mr % 6;
						Y1Y2Y3(interleaved, 1);
						norm_output1 = (Y1 - channel_estimation(H_ptr, &antenna_perm2[reference_order][0]) * de_STBC).norm();
						norm_output2 = (Y2 - channel_estimation(H_ptr, &antenna_perm2[reference_order][1]) * de_STBC).norm();
						norm_output3 = (Y3 - channel_estimation(H_ptr, &antenna_perm2[reference_order][2]) * de_STBC).norm();

					}

					if (norm_output1 < temp_min1) {
						temp_min1 = norm_output1;
						ab_save_temp[i][0] = a;
						ab_save_temp[i][1] = b;
					}
					if (norm_output2 < temp_min2) {
						temp_min2 = norm_output2;
						ab_save_temp[i][2] = a;
						ab_save_temp[i][3] = b;
					}
					if (norm_output3 < temp_min3) {
						temp_min3 = norm_output3;
						ab_save_temp[i][4] = a;
						ab_save_temp[i][5] = b;
					}

				}
			}
			norm_output = temp_min1 + temp_min2 + temp_min3;
			//	cout << "mr = " << mr;
			//	cout << "     norm_output = " << norm_output << endl; ;
			if (norm_output < min)
			{
				min = norm_output;
				m_save[i] = mr;

				ab_save[i][0] = ab_save_temp[i][0];
				ab_save[i][1] = ab_save_temp[i][1];
				ab_save[i][2] = ab_save_temp[i][2];
				ab_save[i][3] = ab_save_temp[i][3];
				ab_save[i][4] = ab_save_temp[i][4];
				ab_save[i][5] = ab_save_temp[i][5];

			}
		}
		min = 9999999;
		norm_output = 0;

	}

	for (int i = 0; i < frame_w; i++)
	{
		for (int k = 0; k < antenna_number; k++) {
			decode_bits[i][k] = ((m_save[i] >> k) % 2);
		}

		for (int k = 0; k < 6; k++) {
			decode_bits[i][k * 2 + 8] = (ab_save[i][k] >> 1) % 2;
			decode_bits[i][k * 2 + 9] = (ab_save[i][k]) % 2;
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
	/*	for (int i = 0; i < frame_w; i++) {
			for (int j = 0; j < bits_num; j++) {
				cout << input_bits[i][j];
			}
			puts("");
			for (int j = 0; j < bits_num; j++) {
				cout << decode_bits[i][j];
			}
			puts("");
			puts("");
		}*/
}

int main()
{

	printf("%dT%dR DB0:%f stbcsmsp",M,N,DB0);
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


