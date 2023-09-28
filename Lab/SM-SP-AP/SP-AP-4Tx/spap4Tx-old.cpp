// STBCSM_SPAP_4Tx.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

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


#define  OUTPUTFILE  "addrateGSM(M=4 N=2)_ML_2.5db.txt"
#define  OPENSTRING  "STBCSM_SPAP_4Tx  ,\
						\n#Eb/No(dB) BitErrRate         BitErrSample  TotalSample \n" 
#define  DATASTRING  "%5.2f      %18.16f %7d %11u \n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate
#define  DB0		-0.0 //要看diversity要用一收看
#define  DBSTEP		5.0
#define  POINTNO	20
#define  ERRNOSTEP  2
#define  MAXERRNO	500//2000//1000//500
#define  SAMPLE_NUM 100000//500000//200000//100000
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              4
#define  N              1 //R
#define  frame_w        1
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define  bits_num       13 /*1+5+2+2+2+2*/ //後4=QPSK(四傳)
#define  qpsk_num		4
#define  antenna_bits_num 5
#define  time_num 4
#define  data_rate 3.25

using namespace std;
using namespace Eigen;


#define  M3  0x00000004L
#define  M31 0x40000000L
#define  eulen(r1,i1,r2,i2) ((r1)-(r2))*((r1)-(r2))+((i1)-(i2))*((i1)-(i2))

FILE* fp;
static char  filename[80] = OUTPUTFILE;
static int	 v[2] = { -1, 1 };
int	 point, biterrno, biterrno_ant, biterrno_sym, errlevel, u, t, de_info, dsignal, samp;
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

int hcounter = 1, ytcounter = 0;
complex<double> accu, avg, yaccu, yavg;

//PERM傳送矩陣
int antenna_perm[2][2] = { 1,2,
							2,1 };



MatrixXcd H(N, M), H_test(N, M / 2), noise(N, M);

MatrixXcd  Y1(N, 2), Y2(N, 2), y(N, M), new_y1(N, M), new_y(N, M); //new define new_Y
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

	int set_of_x_save[frame_w], sp_save[frame_w];
	int input_bits[frame_w][bits_num], decode_bits[frame_w][bits_num], ab_save[frame_w][4], ab_save_temp[frame_w][4], m_save[frame_w], interleaved, reference_order, m, r, g;

	double norm_output = 0, norm_output1 = 0, norm_output2 = 0, norm_output3 = 0, norm_output4 = 0, min = 9999999, temp_min1 = 9999999, temp_min2 = 9999999;

	complex<double> qpsk_map[qpsk_num] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) }, Q[frame_w][4];

	MatrixXcd zero(2, 2), /*Y1(N, 2), Y2(N, 2),*/  I_diag0(M, M), I_diag(M, M), I_diag_new(M, M), I_diag_new1(M, M), St1(M, M), /*y(N, M),*/ de_STBC(4, 4), de_STBC_new1(4, 4), de_STBC_new(4, 4);
	MatrixXcd col_1342(4, 4), col_1423(4, 4), col_2143(4, 4), col_2431(4, 4), col_2314(4, 4);
	MatrixXcd row_1342(4, 4), row_1423(4, 4), row_2143(4, 4), row_2431(4, 4), row_2314(4, 4);
	col_1342 <<
		1, 0, 0, 0,
		0, 0, 0, 1,
		0, 1, 0, 0,
		0, 0, 1, 0;
	col_1423 <<
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
		0, 1, 0, 0;
	col_2143 <<
		0, 1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 1,
		0, 0, 1, 0;
	col_2431 <<
		0, 0, 0, 1,
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0;
	col_2314 <<
		0, 0, 1, 0,
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 0, 1;
	row_1342 <<
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
		0, 1, 0, 0;
	row_1423 <<
		1, 0, 0, 0,
		0, 0, 0, 1,
		0, 1, 0, 0,
		0, 0, 1, 0;
	row_2143 <<
		0, 1, 0, 0,
		1, 0, 0, 0,
		0, 0, 0, 1,
		0, 0, 1, 0;
	row_2431 <<
		0, 1, 0, 0,
		0, 0, 0, 1,
		0, 0, 1, 0,
		1, 0, 0, 0;
	row_2314 <<
		0, 1, 0, 0,
		0, 0, 1, 0,
		1, 0, 0, 0,
		0, 0, 0, 1;

	int de_r, de_q;

	MatrixXcd* H_ptr = &H;

	//===========================encoder_start===============================//


	fading_channel(N, M);
	//cout <<"H = "<< H << endl;
	for (int i = 0; i < frame_w; i++)
	{
		//cout << "====================================" << i << endl;
		for (int j = 0; j < bits_num; j++)
		{
			input_bits[i][j] = rand() % 2;
		}


		//假設i是0 j也從0開始等於是QPSK
		/*前面留下6個位元存放拿來映射到天線選擇的位元*/
		for (int j = 0; j < qpsk_num; j++)
		{
			Q[i][j] = qpsk_map[(input_bits[i][j * 2 + antenna_bits_num] << 1) + input_bits[i][j * 2 + (antenna_bits_num + 1)]];
		}
		//二進制轉成十進制

		min = 999999;


		int set_of_x;
		set_of_x = input_bits[i][0];

		if (set_of_x == 0) {
			I_diag0 <<
				Q[i][0], -conj(Q[i][1]), 0, 0,
				Q[i][1], conj(Q[i][0]), 0, 0,
				0, 0, Q[i][2], -conj(Q[i][3]),
				0, 0, Q[i][3], conj(Q[i][2]);
			I_diag << 1 / sqrt(2.0) * I_diag0;
		}
		if (set_of_x == 1) {
			I_diag0 <<
				0, 0, Q[i][2], -conj(Q[i][3]),
				0, 0, Q[i][3], conj(Q[i][2]),
				Q[i][0], -conj(Q[i][1]), 0, 0,
				Q[i][1], conj(Q[i][0]), 0, 0;
			I_diag << 1 / sqrt(2.0) * I_diag0;
		}
		int sp;//symbol permutaion choice 
		sp = (input_bits[i][4])
			+ (input_bits[i][3] << 1)
			+ (input_bits[i][2] << 2)
			+ (input_bits[i][1] << 3);

		//cout << "encode:"<<endl << input_bits[i][1] << input_bits[i][2] << input_bits[i][3] << input_bits[i][4] << input_bits[i][5] << endl<<endl;

		//cout << sp << endl; //0-15

		////////////////////////16種interleaved//////////////////////

		if (sp == 0) {
			I_diag_new << I_diag;
		}

		else if (sp == 1) {
			I_diag_new << I_diag * col_1342;
		}
		else if (sp == 2) {
			I_diag_new << I_diag * col_1423;
		}
		else if (sp == 3) {
			I_diag_new << I_diag * col_2143;
		}
		else if (sp == 4) {
			I_diag_new << I_diag * col_2431;
		}
		else if (sp == 5) {
			I_diag_new << I_diag * col_2314;
		}
		else if (sp == 6) {
			I_diag_new << row_1342 * I_diag;
		}
		else if (sp == 7) {
			I_diag_new << row_1342 * I_diag * col_1342;
		}
		else if (sp == 8) {
			I_diag_new << row_1342 * I_diag * col_1423;
		}
		else if (sp == 9) {
			I_diag_new << row_1342 * I_diag * col_2143;
		}
		else if (sp == 10) {
			I_diag_new << row_1342 * I_diag * col_2431;
		}
		else if (sp == 11) {
			I_diag_new << row_1342 * I_diag * col_2314;
		}
		else if (sp == 12) {
			I_diag_new << row_1423 * I_diag;
		}
		else if (sp == 13) {
			I_diag_new << row_1423 * I_diag * col_1342;
		}
		else if (sp == 14) {
			I_diag_new << row_1423 * I_diag * col_1423;
		}
		else if (sp == 15) {
			I_diag_new << row_1423 * I_diag * col_2143;
		}
		/*else if (sp == 16) {
			I_diag_new << row_1423 * I_diag * col_2431;
		}
		else if (sp == 17) {
			I_diag_new << row_1423 * I_diag * col_2314;
		}
		else if (sp == 18) {
			I_diag_new << row_2143 * I_diag;
		}
		else if (sp == 19) {
			I_diag_new << row_2143 * I_diag * col_1342;
		}
		else if (sp == 20) {
			I_diag_new << row_2143 * I_diag * col_1423;
		}
		else if (sp == 21) {
			I_diag_new << row_2143 * I_diag * col_2143;
		}
		else if (sp == 22) {
			I_diag_new << row_2143 * I_diag * col_2431;
		}
		else if (sp == 23) {
			I_diag_new << row_2143 * I_diag * col_2314;
		}
		else if (sp == 24) {
			I_diag_new << row_2431 * I_diag;
		}
		else if (sp == 25) {
			I_diag_new << row_2431 * I_diag * col_1342;
		}
		else if (sp == 26) {
			I_diag_new << row_2431 * I_diag * col_1423;
		}
		else if (sp == 27) {
			I_diag_new << row_2431 * I_diag * col_2143;
		}
		else if (sp == 28) {
			I_diag_new << row_2431 * I_diag * col_2431;
		}
		else if (sp == 29) {
			I_diag_new << row_2431 * I_diag * col_2314;
		}
		else if (sp == 30) {
			I_diag_new << row_2314 * I_diag;
		}
		else if (sp == 31) {
			I_diag_new << row_2314 * I_diag * col_1342;
		}*/
		//cout << "interleaved前:" << endl << I_diag << endl;
		//cout << "interleaved中:" << endl << I_diag_new1 << endl;
		//cout << "interleaved後:" << endl << I_diag_new << endl << endl;

		//system("pause");

		/////////////////////////////////////////////////////////傳送端結束//////////////////////////////////////////////////


		y = H * I_diag_new + White_noise(N, M);

		//cout << "y=" << y << endl;

		//receive here

		temp_min1 = 999999;
		temp_min2 = 999999;



		/////////////////de_STBC interleaved//////////////////////
		for (int de_set_of_x = 0; de_set_of_x < 2; de_set_of_x++)
		{
			for (int de_sp = 0; de_sp < 16; de_sp++)
			{
				for (int a = 0; a < 4; a++)
				{
					for (int b = 0; b < 4; b++)
					{
						for (int c = 0; c < 4; c++)
						{
							for (int d = 0; d < 4; d++)
							{
								if (de_set_of_x == 0) {
									de_STBC <<
										qpsk_map[a], -conj(qpsk_map[b]), 0, 0,
										qpsk_map[b], conj(qpsk_map[a]), 0, 0,
										0, 0, qpsk_map[c], -conj(qpsk_map[d]),
										0, 0, qpsk_map[d], conj(qpsk_map[c]);

									de_STBC = 1 / sqrt(2.0) * de_STBC;
								}

								else if (de_set_of_x == 1) {
									de_STBC <<
										0, 0, qpsk_map[c], -conj(qpsk_map[d]),
										0, 0, qpsk_map[d], conj(qpsk_map[c]),
										qpsk_map[a], -conj(qpsk_map[b]), 0, 0,
										qpsk_map[b], conj(qpsk_map[a]), 0, 0;

									de_STBC = 1 / sqrt(2.0) * de_STBC;
								}

								//cout << "de_sp=" << de_sp << endl;

								if (de_sp == 0) {
									de_STBC_new << de_STBC;
								}

								else if (de_sp == 1) {
									de_STBC_new << de_STBC * col_1342;
								}
								else if (de_sp == 2) {
									de_STBC_new << de_STBC * col_1423;
								}
								else if (de_sp == 3) {
									de_STBC_new << de_STBC * col_2143;
								}
								else if (de_sp == 4) {
									de_STBC_new << de_STBC * col_2431;
								}
								else if (de_sp == 5) {
									de_STBC_new << de_STBC * col_2314;
								}
								else if (de_sp == 6) {
									de_STBC_new << row_1342 * de_STBC;
								}
								else if (de_sp == 7) {
									de_STBC_new << row_1342 * de_STBC * col_1342;
								}
								else if (de_sp == 8) {
									de_STBC_new << row_1342 * de_STBC * col_1423;
								}
								else if (de_sp == 9) {
									de_STBC_new << row_1342 * de_STBC * col_2143;
								}
								else if (de_sp == 10) {
									de_STBC_new << row_1342 * de_STBC * col_2431;
								}
								else if (de_sp == 11) {
									de_STBC_new << row_1342 * de_STBC * col_2314;
								}
								else if (de_sp == 12) {
									de_STBC_new << row_1423 * de_STBC;
								}
								else if (de_sp == 13) {
									de_STBC_new << row_1423 * de_STBC * col_1342;
								}
								else if (de_sp == 14) {
									de_STBC_new << row_1423 * de_STBC * col_1423;
								}
								else if (de_sp == 15) {
									de_STBC_new << row_1423 * de_STBC * col_2143;
								}
								/*else if (de_sp == 16) {
									de_STBC_new << row_1423 * de_STBC * col_2431;
								}
								else if (de_sp == 17) {
									de_STBC_new << row_1423 * de_STBC * col_2314;
								}
								else if (de_sp == 18) {
									de_STBC_new << row_2143 * de_STBC;
								}
								else if (de_sp == 19) {
									de_STBC_new << row_2143 * de_STBC * col_1342;
								}
								else if (de_sp == 20) {
									de_STBC_new << row_2143 * de_STBC * col_1423;
								}
								else if (de_sp == 21) {
									de_STBC_new << row_2143 * de_STBC * col_2143;
								}
								else if (de_sp == 22) {
									de_STBC_new << row_2143 * de_STBC * col_2431;
								}
								else if (de_sp == 23) {
									de_STBC_new << row_2143 * de_STBC * col_2314;
								}
								else if (de_sp == 24) {
									de_STBC_new << row_2431 * de_STBC;
								}
								else if (de_sp == 25) {
									de_STBC_new << row_2431 * de_STBC * col_1342;
								}
								else if (de_sp == 26) {
									de_STBC_new << row_2431 * de_STBC * col_1423;
								}
								else if (de_sp == 27) {
									de_STBC_new << row_2431 * de_STBC * col_2143;
								}
								else if (de_sp == 28) {
									de_STBC_new << row_2431 * de_STBC * col_2431;
								}
								else if (de_sp == 29) {
									de_STBC_new << row_2431 * de_STBC * col_2314;
								}
								else if (de_sp == 30) {
									de_STBC_new << row_2314 * de_STBC;
								}
								else if (de_sp == 31) {
									de_STBC_new << row_2314 * de_STBC * col_1342;
								}*/
								//cout << "interleaved前:" << endl << de_STBC << endl;
								//cout << "interleaved中:" << endl << de_STBC_new1 << endl;
								//cout << "interleaved後:" << endl << de_STBC_new << endl << endl;

							//cout << "interleaved前:" << endl << de_STBC << endl;
								//cout << "interleaved中:" << endl << de_STBC_new1 << endl;
								//cout << "interleaved後:" << endl <<  de_STBC_new << endl << endl;

								norm_output1 = pow((y - H * de_STBC_new).norm(), 2);
								//cout << "de_sp:" << de_sp << endl;
								//cout << "y:" << endl << y << endl;
								//cout << "x" << endl <<  de_STBC_new << endl;

								if (norm_output1 < temp_min1)
								{
									//cout << temp_min1 << endl;
									//cout << "de_sp:" << de_sp << endl;
									//cout << "y:" << endl << y << endl;
									//cout << "x" << endl <<  de_STBC_new << endl;

									temp_min1 = norm_output1;
									ab_save_temp[i][0] = a;
									ab_save_temp[i][1] = b;
									ab_save_temp[i][2] = c;
									ab_save_temp[i][3] = d;
								}


							}
						}

					}
				}

				norm_output = temp_min1;
				if (norm_output < min) {
					min = norm_output;
					set_of_x_save[i] = de_set_of_x;//reference order
					sp_save[i] = de_sp;//interleaved
					ab_save[i][0] = ab_save_temp[i][0];
					ab_save[i][1] = ab_save_temp[i][1];
					ab_save[i][2] = ab_save_temp[i][2];
					ab_save[i][3] = ab_save_temp[i][3];
				}
			}
		}
		//cout << "sp_save=" << sp_save[i] << endl;
		//cout << "min=" << min << endl;
		min = 9999999;
		norm_output = 0;

	}


	for (int i = 0; i < frame_w; i++)
	{
		//解天線
		//	cout << "m=" <<m_save[i]<<endl;

		decode_bits[i][0] = set_of_x_save[i];
		decode_bits[i][4] = (sp_save[i] % 2);         //15 % 2 = 1
		decode_bits[i][3] = ((sp_save[i] >> 1) % 2);  // 15 / 2 = 7，7 % 2 = 1
		decode_bits[i][2] = ((sp_save[i] >> 2) % 2);  // 7 / 2 = 3，3 % 2 = 1
		decode_bits[i][1] = ((sp_save[i] >> 3) % 2);  // 3 / 2 = 1，1 % 2 = 1 (最左)
		//cout << "decode:"<<endl << decode_bits[i][1] << decode_bits[i][2] << decode_bits[i][3] << decode_bits[i][4] << decode_bits[i][5] << endl;


		//解QPSK

		decode_bits[i][5] = (ab_save[i][0] >> 1) % 2;
		decode_bits[i][6] = (ab_save[i][0]) % 2;
		decode_bits[i][7] = (ab_save[i][1] >> 1) % 2;
		decode_bits[i][8] = (ab_save[i][1]) % 2;
		decode_bits[i][9] = (ab_save[i][2] >> 1) % 2;
		decode_bits[i][10] = (ab_save[i][2]) % 2;
		decode_bits[i][11] = (ab_save[i][3] >> 1) % 2;
		decode_bits[i][12] = (ab_save[i][3]) % 2;


		//對應天線位元
		//here

		for (int j = 0; j < antenna_bits_num; j++)
		{
			//cout << decode_bits[i][j] << " " ;
			if (decode_bits[i][j] != input_bits[i][j]) {
				biterrno_ant++;
				biterrno++;
			}
		}
		//QPSK 4個QPSKb
		for (int j = antenna_bits_num; j < bits_num; j++) //14=bits_num
		{
			//			cout << decode_bits[i][j] << " " ;
			if (decode_bits[i][j] != input_bits[i][j]) {
				biterrno_sym++;
				//cout << biterrno << endl; //錯幾個bit都記錄下來
				biterrno++;
			}
		}
		dsignal += bits_num;
	}

	///////檢查
	//for (int i = 0; i < frame_w; i++) {
	//	for (int j = 0; j < 14; j++) {
	//		cout << input_bits[i][j];
	//	}
	//	puts("");
	//	for (int j = 0; j < 14; j++) {
	//		cout << decode_bits[i][j];
	//	}
	//	puts("");
	//	puts("");
	//	}
}

int main()
{

	printf("STBCSM_SPAP_4Tx\n");
	srand((unsigned)time(NULL));
	initial();

	for (point = 0; point < POINTNO; point++)
	{
		pnstate = 0xaaaaaaaaL;
		snrdb = DB0 + point * DBSTEP;
		snr = (data_rate)*pow(10.0, 0.1 * snrdb);
		deviate = sqrt(0.5 / snr);
		biterrno = 0;
		biterrno_sym = 0;
		biterrno_ant = 0;
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
				/*
				printf("ant       %5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno_ant / dsignal, biterrno_ant, dsignal, deviate);
				printf("sym       %5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno_sym / dsignal, biterrno_sym, dsignal, deviate);
				puts("");
				*/
			}
		}
		//system("pause");
		//record();
		//記得改Y=HN+NOISE
		// db 錯誤率 錯的sample數 總sample數
		//printf("%5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
		/*
		printf("ant       %5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno_ant / dsignal, biterrno_ant, dsignal, deviate);
		printf("sym       %5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno_sym / dsignal, biterrno_sym, dsignal, deviate);
		puts("");
		*/
	}
	system("pause");
	return 0;
}

