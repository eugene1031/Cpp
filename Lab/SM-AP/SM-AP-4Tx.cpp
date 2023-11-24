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

#define  OUTPUTFILE  "SMAP4Tx.txt"
#define  OPENSTRING  "STBCSM_AP  ,\
						\n 作法：0930將參數調成2500跟BE-GSM比較 ,\
						\n#Eb/No(dB) BitErrRate         BitErrSample  TotalSample \n" 
#define  DATASTRING  "%5.2f      %18.16f %7d %11u \n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate
#define  DB0		4.0 //要看diversity要用一收看
#define  DBSTEP		2.0
#define  POINTNO	20
#define  ERRNOSTEP  2
#define  MAXERRNO	2000//2000//1000//500
#define  SAMPLE_NUM 100000//500000//200000//100000
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              4
#define  N              4 //R
#define  frame_w        20
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define  bits_num       11 /*5+2+2+2+2->3+2+2+2+2*/ 
#define  qpsk_num		4
#define  antenna_bits_num 3
#define  time_num 4
#define data_rate 2.75

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

/*

*/
int interleaved_pattern[4][4] = { 1,2,3,4,     //  1324 是不合法的
									1,3,4,2,
									1,4,2,3,
									2,1,4,3,
	//2,4,3,1,
	//2,3,1,4
};




/*
int interleaved_pattern[6][4] = { 1,2,3,4,     //  1324 是不合法的
									1,3,4,2,
									1,4,2,3,
									2,1,4,3,
									3,1,2,4,
									4,1,3,2
};
*/



//PERM傳送矩陣
int antenna_perm[2][2] = { 1,2,
							2,1 };
int antenna_perm2[2][2] = { 1,1,
							2,2 };

/*
int antenna[4][2] = {1,2,
					2,1,
					1,1,
					2,2 };
*/
/*
* //不佳interleaved的四傳
* int antenna[16][2] = { 1,2,
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
*/


//因為要加INTERLEAVE  D所以要切成兩個矩陣 一個是可以加INTERLEAVED一個是不能的
//can't add interleaved
int antenna_A[4][2] = { 1,1,
					2,2,
					3,3,
					4,4
};
//can add interleaved
int antenna_B[12][2] = { 1,2,
					2,1,

					3,4,//都加角度4
					4,3,


					3,1,//加左邊角度
					3,2,
					4,1,
					4,2,
					1,3,//加右邊角度
					1,4,
					2,3,
					2,4

};



MatrixXcd H(N, M), H_new(N, M), H_test(N, M / 2), noise(N, M), ABC(4, 4), I_row(4, 4);

MatrixXcd  Y1(N, 2), Y2(N, 2), y(N, M), new_y(N, M); //new define new_Y
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


//??
void Y1Y2(int c, int a) {
	if (a == 1) {
		Y1.col(0) << y.col(interleaved_pattern[c][0] - 1);
		Y1.col(1) << y.col(interleaved_pattern[c][1] - 1);
		Y2.col(0) << y.col(interleaved_pattern[c][2] - 1);
		Y2.col(1) << y.col(interleaved_pattern[c][3] - 1);
	}

	if (a == 2) {
		Y1.col(0) << y.col(interleaved_pattern[0][0] - 1);
		Y1.col(1) << y.col(interleaved_pattern[0][1] - 1);
		Y2.col(0) << y.col(interleaved_pattern[0][2] - 1);
		Y2.col(1) << y.col(interleaved_pattern[0][3] - 1);
	}
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
	//cout << "func" << endl;
	//cout << final_mat << endl<<endl;
	return final_mat;
}
/*void ptr_test_func(MatrixXcd* p, MatrixXcd d) {
	cout <<"d = "<< d << endl << endl;

	cout << "ptr" << endl;
	cout <<"*(p) = " <<(*p)(0,0) << endl<<endl;
	//cout <<"*(p+1) = "<< * (p + 1) << endl<<endl;

}*/

void receive()//void frame_length()
{

	int set_of_x_save[frame_w], sp_save[frame_w];
	int input_bits[frame_w][bits_num], decode_bits[frame_w][bits_num], ab_save[frame_w][4], ab_save_temp[frame_w][4], m_save[frame_w], interleaved, reference_order, m, r, g;

	double norm_output = 0, norm_output1 = 0, norm_output2 = 0, norm_output3 = 0, norm_output4 = 0, min = 9999999, temp_min1 = 9999999, temp_min2 = 9999999;

	complex<double> qpsk_map[qpsk_num] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) }, Q[frame_w][4];

	MatrixXcd zero(2, 2), /*Y1(N, 2), Y2(N, 2),*/ I_s1s2(2, 2), I_s3s4(2, 2), I_diag(M, M), I_diag_new(M, M), St1(M, M), /*y(N, M),*/ de_STBC(2, 2);
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
			/*intput_bits[i][j] = (pnstate& M31)>>30;
			pntemp=(pnstate& M3)>>2;
			pntemp^=intput_bits[i][j];
			pnstate=(pnstate<<1)|pntemp;*/
			input_bits[i][j] = rand() % 2;

		}
		//ASK 這邊的Q有點看不懂他要幹嘛 假設i是0 j也從0開始等於是QPSK
		/*前面留下6個位元存放拿來映射到天線選擇的位元*/
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
		/*
		 m = (input_bits[i][0])
			+ (input_bits[i][1] << 1)
			+ (input_bits[i][2] << 2)
			+ (input_bits[i][3] << 3)
			+ (input_bits[i][4] << 4)
			+ (input_bits[i][5] << 5);
		*/
		/* m寫法
		m = (input_bits[i][1] << 1)
			+ (input_bits[i][2] << 2);
		*/

		/*m當作interleaving 天線選擇*/

		//4個2進制換成十進制 0~15 0824改5個二進制(家INTERLEAVED)
		//&antenna_A[] 不能加interleaved的

		//I_diag << generation(&antenna_A[m][0], &antenna_A[m][1], I_s1s2_ptr, I_s3s4_ptr);
		//imp
		//可加interleaved的B矩陣

		/*m寫法
		int q; //可加interleaved的選擇映射(B)
		int r; //交錯符號元素(interleaved)(C)
		q = m / 6;
		r = m % 6;
		I_diag << generation(&antenna_perm[q][0], &antenna_perm[q][1], I_s1s2_ptr, I_s3s4_ptr);
		*/


		int set_of_x;
		set_of_x = input_bits[i][0];
		I_diag << generation(&antenna_perm[set_of_x][0], &antenna_perm[set_of_x][1], I_s1s2_ptr, I_s3s4_ptr);

		int sp;//symbol permutaion choice 
		sp = (input_bits[i][1])
			+ (input_bits[i][2] << 1);
		//cout << input_bits[i][1] << input_bits[i][2] << endl; //00 01 10 11

		//cout << sp << endl;//0-3
		//以下三個if是做三種interleaved

		//1234 r=0 //1342 r=1//1423 r=2 //kk是 0 1 2 3 
		//new define
		for (int kk = 0; kk < 4; kk++) {
			I_diag_new.row(kk) << I_diag.row(interleaved_pattern[sp][kk] - 1);
		}

		/*正確
		* 	I_diag_new.col(0) << I_diag.col(interleaved_pattern[r][0] - 1);
			I_diag_new.col(1) << I_diag.col(interleaved_pattern[r][1] - 1);
			I_diag_new.col(2) << I_diag.col(interleaved_pattern[r][2] - 1);
			I_diag_new.col(3) << I_diag.col(interleaved_pattern[r][3] - 1);
		*/


		//傳送端結束 

		//
		/*	for (int l = 0; l < M; l++) {
				for (int ll = 0; ll < N; ll++) {
					accu += H(l, ll);

				}
			}
			cout << "hcounter = " << hcounter << endl;
			hcounter += 1;
			cout << accu << endl;
			*/
        ABC << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
        y = H * I_diag_new + White_noise(N, M);
        //y = H*St1;
		//cout << "White_noise = " << White_noise(N, M) << endl;

		//沒有加interleaved的16種 (之前的 還沒有要加interleaved)
		//receive here
		for (int de_set_of_x = 0; de_set_of_x < 2; de_set_of_x++)
		{
			for (int de_sp = 0; de_sp < 4; de_sp++) //symbol permutaion
			{

				/*
				de_r = mr % 6;
				de_q = mr / 6;
				*/

				Y1.col(0) << y.col(0);
				Y1.col(1) << y.col(1);
				Y2.col(0) << y.col(2);
				Y2.col(1) << y.col(3);


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
                            for (int kk = 0; kk < 4; kk++) {
						        I_row.row(kk) << ABC.row(interleaved_pattern[de_sp][kk] - 1);
					        }
					        H_new = H * I_row;
							norm_output1 = (Y1 - channel_estimation(&H_new, &antenna_perm[de_set_of_x][0]) * de_STBC).norm();
							norm_output2 = (Y2 - channel_estimation(&H_new, &antenna_perm[de_set_of_x][1]) * de_STBC).norm();
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
					//m_save[i] = mr;
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
		//	cout << "m=" <<m_save[i]<<endl;

		decode_bits[i][0] = set_of_x_save[i];
		decode_bits[i][1] = sp_save[i] % 2;
		decode_bits[i][2] = ((sp_save[i] >> 1) % 2);
		/*
		decode_bits[i][0] = ((m_save[i]) % 2);			// 23 % 2 = 1 (最右)
		decode_bits[i][1] = ((m_save[i] >> 1) % 2);		// 23 / 2 = 11，11 % 2 = 1
		decode_bits[i][2] = ((m_save[i] >> 2) % 2);		// 11 / 2 = 5，5 % 2 = 1
		decode_bits[i][3] = ((m_save[i] >> 3) % 2);		// 5 / 2 = 2，2 % 2 = 0
		decode_bits[i][4] = ((m_save[i] >> 4) % 2);		// 2 / 2 = 1，1 % 2 = 1 (最左)
		decode_bits[i][5] = ((m_save[i] >> 5) % 2);
		 (23)10 = (10111)2
		*/
		//改寫以上
		/*
		for (int n = 0; n < antenna_bits_num; n++)
		{
			decode_bits[i][n] = ((m_save[i] >> n) % 2);
		}
		*/

		//解QPSK

		for (int k = 0; k < qpsk_num; k++)
		{
			decode_bits[i][k * 2 + antenna_bits_num] = (ab_save[i][k] >> 1) % 2;
			decode_bits[i][k * 2 + (antenna_bits_num + 1)] = (ab_save[i][k]) % 2;
		}

		//對應天線位元
		//here

		for (int j = 0; j < antenna_bits_num; j++)
		{
			//			cout << decode_bits[i][j] << " " ;
			if (decode_bits[i][j] != input_bits[i][j]) {
				biterrno_ant++;
				biterrno++;
			}
		}
		//QPSK 4個QPSKb
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
	/*for (int i = 0; i < frame_w; i++) {
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

	//printf("4T4RSTBCSM_SP_no_angle_ex1_0926\n 4種symbol permutaion 不加雜訊錯誤率為0 \n 1013 run again");
	//srand((unsigned)time(NULL));
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
		printf("%5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
		//printf("ant       %5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno_ant / dsignal, biterrno_ant, dsignal, deviate);
		//printf("sym       %5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno_sym / dsignal, biterrno_sym, dsignal, deviate);
		//puts("");
	}
	system("pause");
	return 0;
}

