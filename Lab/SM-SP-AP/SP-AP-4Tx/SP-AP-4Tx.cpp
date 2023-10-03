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

int interleaved_pattern[6][4] = { 1,2,3,4,     //  1324 是不合法的
									1,3,4,2,
									1,4,2,3,
									2,1,4,3,
	                                2,4,3,1,
	                                2,3,1,4
};

//PERM傳送矩陣
int antenna_perm[2][2] = { 1,2,
							2,1 };

MatrixXcd H(N, M), H_test(N, M / 2), noise(N, M);
MatrixXcd  Y1(N, 2), Y2(N, 2), y(N, M), new_y(N, M); //new define new_Y

void fading_channel(int row, int col)
{
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

MatrixXcd channel_estimation(MatrixXcd* H, int* reference_order, int* de_row) {
	//(1,1)
	/*[ H12 | H34 ]*[ STBC12 | STBC34 ] = Y= [ H12 * STBC12 | H12 * STBC34 ]  (1*2)的矩陣乘上(2*2) 等於 (1*2)的矩陣
						 0		  0
	*/
    MatrixXcd H_new(N, M);
    if (*de_row == 0)
    {
        for (int i = 0; i < M; i++){
            H_new.col(i) << (*H).col(interleaved_pattern[0][i] - 1);
        }
    }
    if (*de_row == 1)
    {
        for (int i = 0; i < M; i++){
            H_new.col(i) << (*H).col(interleaved_pattern[2][i] - 1);
        }
    }
    if (*de_row == 2){
        for (int i = 0; i < M; i++){
            H_new.col(i) << (*H).col(interleaved_pattern[1][i] - 1);
        }
    }
	//H12
	if (*reference_order == 1) {
		H_test.col(0) << (H_new).col(0);
		H_test.col(1) << (H_new).col(1);

	}
	//H34
	if (*reference_order == 2) {
		H_test.col(0) << (H_new).col(2);
		H_test.col(1) << (H_new).col(3);

	}
	//H23
	if (*reference_order == 3) {
		H_test.col(0) << (H_new).col(1);
		H_test.col(1) << (H_new).col(2);
	}
	//H14
	if (*reference_order == 4) {
		H_test.col(0) << (H_new).col(0);
		H_test.col(1) << (H_new).col(3);
	}
	return H_test;
}

void Y1Y2(int c, int a) {
	if (a == 1) {
		Y1.col(0) << y.col(interleaved_pattern[c][0] - 1);
		Y1.col(1) << y.col(interleaved_pattern[c][1] - 1);
		Y2.col(0) << y.col(interleaved_pattern[c][2] - 1);
		Y2.col(1) << y.col(interleaved_pattern[c][3] - 1);
	}
}

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
}

//轉至後位子會跑掉，所以先給一個假的
MatrixXcd generation(int* x, int* x1, MatrixXcd* I_s1s2, MatrixXcd* I_s3s4) {
	MatrixXcd time12(4, 2), time34(4, 2), final_mat(M, M);
	//fake is1s2
	(*I_s1s2)(0, 1) = -conj((*I_s1s2)(0, 1));
	(*I_s1s2)(1, 0) = -conj((*I_s1s2)(1, 0));
	(*I_s3s4)(0, 1) = -conj((*I_s3s4)(0, 1));
	(*I_s3s4)(1, 0) = -conj((*I_s3s4)(1, 0));
	time12 << ((*I_s1s2) * ret(*x)).transpose();
	time34 << ((*I_s3s4) * ret(*x1)).transpose();

	//cout << (*I_s1s2) << endl<<endl;
	final_mat << time12, time34;
	//cout << final_mat << endl << endl;

	return final_mat;
}

void receive()//void frame_length()
{

	int set_of_x_save[frame_w], sp_save[frame_w];
	int input_bits[frame_w][bits_num], decode_bits[frame_w][bits_num], ab_save[frame_w][4], ab_save_temp[frame_w][4], m_save[frame_w], interleaved, reference_order, m, r, g;

	double norm_output = 0, norm_output1 = 0, norm_output2 = 0, norm_output3 = 0, norm_output4 = 0, min = 9999999, temp_min1 = 9999999, temp_min2 = 9999999;

	complex<double> qpsk_map[qpsk_num] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) }, Q[frame_w][4];

	MatrixXcd zero(2, 2), /*Y1(N, 2), Y2(N, 2),*/ I_s1s2(2, 2), I_s3s4(2, 2), I_diag(M, M), I_diag_new(M, M), I_diag_new1(M, M), St1(M, M), /*y(N, M),*/ de_STBC(2, 2);
	int de_r, de_q;

	MatrixXcd* I_s1s2_ptr = &I_s1s2;
	MatrixXcd* I_s3s4_ptr = &I_s3s4;
	MatrixXcd* H_ptr = &H;

	zero << 0, 0,
		0, 0;
	//===========================encoder_start===============================//


	fading_channel(N, M);
	//cout <<"H = "<< H << endl;
	for (int i = 0; i < frame_w; i++)
	{
		for (int j = 0; j < bits_num; j++)
		{
			input_bits[i][j] = rand() % 2;
		}

		for (int j = 0; j < qpsk_num; j++)
		{
			Q[i][j]
				= qpsk_map[(input_bits[i][j * 2 + antenna_bits_num] << 1)
				+ input_bits[i][j * 2 + (antenna_bits_num + 1)]];
		}
		//二進制轉成十進制

		min = 999999;
		//	cout<<"----------------------"<<endl;
		//S1S2是一個AMOUTIBLOCK 由X1X2組成 X1X2就是代表兩個QPSK
		//兩個STBC BLOCK(alamouti block)可以組成一個trasmission block
		//有四種傳送區塊  代表有四個 兩個兩個為單位 [H1 H2 |H3 H4]
		I_s1s2 << Q[i][0], -conj(Q[i][1]), Q[i][1], conj(Q[i][0]);
		I_s1s2 << 1 / sqrt(2.0) * I_s1s2;
		I_s3s4 << Q[i][2], -conj(Q[i][3]), Q[i][3], conj(Q[i][2]);
		I_s3s4 << 1 / sqrt(2.0) * I_s3s4;
		//前面的種類選擇
		//傳送端
		int set_of_x;
		set_of_x = input_bits[i][0];
		I_diag << generation(&antenna_perm[set_of_x][0], &antenna_perm[set_of_x][1], I_s1s2_ptr, I_s3s4_ptr);

		int sp;//symbol permutaion choice 
		sp = (input_bits[i][1])
			+ (input_bits[i][2] << 1)
            + (input_bits[i][3] << 2)
            + (input_bits[i][4] << 3);

		// cout << sp << endl;//0-15
        // cin.get();

        int col = sp % 6;
        int row = sp / 6;
        // 以下三個if是做三種interleaved

        //new define
		for (int kk = 0; kk < 4; kk++) {
			I_diag_new.col(kk) << I_diag.col(interleaved_pattern[col][kk] - 1);
		}
        for (int kk = 0; kk < 4; kk++) {
			I_diag_new1.row(kk) << I_diag_new.row(interleaved_pattern[row][kk] - 1);
		}

		//傳送端結束

        y = H * I_diag_new1 + White_noise(N, M);

        //y = H*St1;
		//cout << "White_noise = " << White_noise(N, M) << endl;

		//沒有加interleaved的16種 (之前的 還沒有要加interleaved)
		//receive here
		for (int de_set_of_x = 0; de_set_of_x < 2; de_set_of_x++)
		{
			for (int de_sp = 0; de_sp < 16; de_sp++) //symbol permutaion
			{
                int de_col = de_sp % 6;
                int de_row = de_sp / 6;
                // new define //try 3 interleaved pattern
                new_y.col(interleaved_pattern[de_col][0] - 1) << y.col(0); //y.col(0)
				new_y.col(interleaved_pattern[de_col][1] - 1) << y.col(1); //y.col(1)
				new_y.col(interleaved_pattern[de_col][2] - 1) << y.col(2);
				new_y.col(interleaved_pattern[de_col][3] - 1) << y.col(3);

				Y1.col(0) << new_y.col(0);
				Y1.col(1) << new_y.col(1);
				Y2.col(0) << new_y.col(2);
				Y2.col(1) << new_y.col(3);


				temp_min1 = 999999;
				temp_min2 = 999999;

				for (int a = 0; a < 4; a++)
				{
					for (int b = 0; b < 4; b++)
					{
						if (de_set_of_x < 2)
						{
							de_STBC << qpsk_map[a], -conj(qpsk_map[b]), qpsk_map[b], conj(qpsk_map[a]);
							de_STBC = 1 / sqrt(2.0) * de_STBC;
							norm_output1 = (Y1 - channel_estimation(H_ptr, &antenna_perm[de_set_of_x][0], &de_row) * de_STBC).norm();
							norm_output2 = (Y2 - channel_estimation(H_ptr, &antenna_perm[de_set_of_x][1], &de_row) * de_STBC).norm();
						}

						//代表左邊最佳星座點 
						if (norm_output1 < temp_min1) {
							temp_min1 = norm_output1;
							ab_save_temp[i][0] = a;
							ab_save_temp[i][1] = b;
						}
						//代表右邊最佳星座點
						if (norm_output2 < temp_min2) {
							temp_min2 = norm_output2;
							ab_save_temp[i][2] = a;
							ab_save_temp[i][3] = b;
						}
					}
				}
				norm_output = temp_min1 + temp_min2;

				if (norm_output < min)
				{
					min = norm_output;
					set_of_x_save[i] = de_set_of_x;
					sp_save[i] = de_sp;
                    // m_save[i] = mr;
                    ab_save[i][0] = ab_save_temp[i][0];
					ab_save[i][1] = ab_save_temp[i][1];
					ab_save[i][2] = ab_save_temp[i][2];
					ab_save[i][3] = ab_save_temp[i][3];
				}
			}
		}
		min = 9999999;
		norm_output = 0;
	}


	for (int i = 0; i < frame_w; i++)
	{
		//解天線
		decode_bits[i][0] = set_of_x_save[i];
		decode_bits[i][1] = sp_save[i] % 2;
		decode_bits[i][2] = ((sp_save[i] >> 1) % 2);
		decode_bits[i][3] = ((sp_save[i] >> 2) % 2);
		decode_bits[i][4] = ((sp_save[i] >> 3) % 2);


		//解QPSK
		for (int k = 0; k < qpsk_num; k++)
		{
			decode_bits[i][k * 2 + antenna_bits_num] = (ab_save[i][k] >> 1) % 2;
			decode_bits[i][k * 2 + (antenna_bits_num + 1)] = (ab_save[i][k]) % 2;
		}

		//對應天線位元
		for (int j = 0; j < antenna_bits_num; j++)
		{
			//			cout << decode_bits[i][j] << " " ;
			if (decode_bits[i][j] != input_bits[i][j]) {
				biterrno_ant++;
				biterrno++;
			}
		}
		//QPSK 4個QPSK
		for (int j = antenna_bits_num; j < bits_num; j++) //12=bits_num
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
	// for (int i = 0; i < frame_w; i++) {
	// 	for (int j = 0; j < bits_num; j++) {
	// 		cout << input_bits[i][j];
	// 	}
	// 	puts("");
	// 	for (int j = 0; j < bits_num; j++) {
	// 		cout << decode_bits[i][j];
	// 	}
	// 	puts("");
	// 	puts("");
	// 	}
    //     cin.get(); 
}

int main()
{

	printf("STBCSM_SPAP_4Tx\n");
	srand((unsigned)time(NULL));
	//initial();

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
			}
		}
		//system("pause");
		//record();
		// db 錯誤率 錯的sample數 總sample數
		printf("%5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);

	}
	system("pause");
	return 0;
}

