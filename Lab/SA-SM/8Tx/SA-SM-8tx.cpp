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


#define  OUTPUTFILE  "SM(M=4 N=4)_Es.txt"
#define  OPENSTRING  "#Eb/No(dB) BitErrRate         BitErrSample  TotalSample \n" 
#define  DATASTRING  "%5.2f      %18.16f %10f %15f \n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate



#define  DB0		-10.0
#define  DBSTEP		2.0 //每5DB一格
#define  POINTNO	20  //共10格
#define  ERRNOSTEP  100  
#define  MAXERRNO	3500    //錯幾個
#define  SAMPLE_NUM 50000 //sample個數
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              8
#define  N              4
#define  frame_w        20  //6
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define  qpsk_num		4
#define  antenna_bits_num 4
#define  bits_num       8
#define  data_rate      4


using namespace std;
using namespace Eigen;


#define  M3  0x00000004L
#define  M31 0x40000000L
#define  eulen(r1,i1,r2,i2) ((r1)-(r2))*((r1)-(r2))+((i1)-(i2))*((i1)-(i2))

FILE* fp;
static char  filename[80] = OUTPUTFILE;
static int	 v[2] = { -1, 1 };
int	 point, errlevel, u, t, de_info, samp;
long    pnstate, pntemp;
double   snrdb, snr, deviate, biterrno, dsignal;

double   drand48()
{
	double  w;
	/*RAND_MAX = 0xffff;*/
	w = (double)rand() / RAND_MAX;

	return  w;
}

/*void initial()
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
		printf(OPENSTRING);
	}
}*/
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
/*void record()
{
	fp = fopen(filename, "a");
	fprintf(fp, DATASTRING);
	fclose(fp);
}*/
MatrixXcd H(N, M), noise(N, 2), H_test(4, 2), H_test1(4, 2);
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

void TxRx()
{

	int input_bits[frame_w][bits_num] = { 0 };

	MatrixXcd X(2, M), X_final(M, 2), X_time12(M, 2), X_time34(2, M);
	//MatrixXcd H(N, M);
	MatrixXcd y(2, N), y_time12(N, 2), y_time34(2, N);
	//MatrixXcd noise(N, 2);
	MatrixXcd STBC_s1s2(2, 2), STBC_s3s4(2, 2), zero_ele(2, 2), de_STBC(2, 2), zero(2, 1);

	MatrixXcd x(4, 2), xr(M, 2), xr1(2, 4);

	complex<double> qpsk00(1, 0), qpsk01(0, 1), qpsk10(-1, 0), qpsk11(0, -1);
	complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) };
	complex<double> Q1[frame_w], Q2[frame_w], Q[frame_w][2];
	//complex<double> angle = complex<double>(cos(pi / 5.150214), sin(pi / 5.150214)); //sita = 0.61rad

	double norm_output = 0, norm_output2 = 0, norm2 = 0, norm3 = 0, min = 9999999, min2 = 9999999, min3 = 9999999, min_2 = 9999999;


	int an_save[frame_w];

	int u_save[frame_w] = { -1 }, k_save[frame_w] = { -1 }, uk_save[frame_w][4] = { -1 };

	int decode_bits[frame_w][bits_num] = { 0 };

	zero_ele << 0, 0,
		0, 0;



	//------------------------------------------------------------------輸入bit

	fading_channel(N, M); //block fading  千萬不要放回圈內
	for (int i = 0; i < frame_w; i++)
	{
		for (int j = 0; j < bits_num; j++)
		{
			input_bits[i][j] = rand() % 2;
		}
		for (int j = 0; j < 2; j++)
		{
			Q[i][j] = qpsk_map[(input_bits[i][j * 2 + antenna_bits_num] << 1) + input_bits[i][j * 2 + (antenna_bits_num + 1)]];
		}

		//cout << "12_ant = " << input_bits[i][0] << endl;
		//cout << "34_ant = " << input_bits[i][1] << endl;
		STBC_s1s2 << Q[i][0], Q[i][1], -conj(Q[i][1]), conj(Q[i][0]);
		STBC_s1s2 << (1 / sqrt(2.0)) * STBC_s1s2;
		//cout << STBC_s1s2 << endl;
		//cout << STBC_s3s4 << endl;
		int m = 0;
        for (int j = 0; j < antenna_bits_num; j++)
        {
            m += (input_bits[i][j] << j);
        }

		if (m == 0)//time12 第1，2根
		{
			X_time12 << STBC_s1s2, zero_ele, zero_ele, zero_ele;
		}
		else if (m == 1)//time 12 第3，4根
		{
			X_time12 <<
				Q[i][0], 0,
				0, conj(Q[i][0]),
				Q[i][1], 0,
				0, conj(Q[i][1]),
                zero_ele, zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
		else if (m == 2)//time 12 第3，4根
		{
			X_time12 <<
				Q[i][0], 0,
				0, conj(Q[i][0]),
				0, Q[i][1],
				-conj(Q[i][1]), 0,
                zero_ele, zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 3)//time 12 第3，4根
		{
			X_time12 <<
				Q[i][0], 0,
				0, conj(Q[i][0]),
                zero_ele, 
				Q[i][1], 0,
				0, conj(Q[i][1]),
                zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 4)//time 12 第3，4根
		{
			X_time12 <<
				Q[i][0], 0,
				0, conj(Q[i][0]),
                zero_ele,
				0, Q[i][1],
				-conj(Q[i][1]), 0,
                zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 5)//time 12 第3，4根
		{
            X_time12 << 
                Q[i][0], 0,
                0, conj(Q[i][0]),
                zero_ele, 
                zero_ele,
                Q[i][1], 0,
                0, conj(Q[i][1]);
            X_time12 << (1 / sqrt(2.0)) * X_time12;
        }
        else if (m == 6)//time 12 第3，4根
		{
            X_time12 << 
                Q[i][0], 0,
                0, conj(Q[i][0]),
                zero_ele,
                zero_ele,
                0, Q[i][1],
                -conj(Q[i][1]), 0;
            X_time12 << (1 / sqrt(2.0)) * X_time12;
        }
		else if (m == 7)//time 12 第3，4根
		{
			X_time12 <<
				0, Q[i][0],
				-conj(Q[i][0]), 0,
				Q[i][1], 0,
				0, conj(Q[i][1]),
                zero_ele, zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 8)//time 12 第3，4根
		{
			X_time12 <<
				0, Q[i][0],
				-conj(Q[i][0]), 0,
				0, Q[i][1],
				-conj(Q[i][1]), 0,
                zero_ele, zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 9)//time 12 第3，4根
		{
			X_time12 <<
				0, Q[i][0],
				-conj(Q[i][0]), 0,
                zero_ele,
				Q[i][1], 0,
				0, conj(Q[i][1]),
                zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 10)//time 12 第3，4根
		{
			X_time12 <<
				0, Q[i][0],
				-conj(Q[i][0]), 0,
                zero_ele,
				0, Q[i][1], 
				-conj(Q[i][1]), 0,
                zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 11)//time 12 第3，4根
		{
            X_time12 << 
                0, Q[i][0],
                -conj(Q[i][0]), 0,
                zero_ele,
                zero_ele,
                Q[i][1], 0,
                0, conj(Q[i][1]);
            X_time12 << (1 / sqrt(2.0)) * X_time12;
        }
        else if (m == 12)//time 12 第3，4根
		{
            X_time12 << 
                0, Q[i][0],
                -conj(Q[i][0]), 0,
                zero_ele,
                zero_ele,
                0, Q[i][1],
                -conj(Q[i][1]), 0;
            X_time12 << (1 / sqrt(2.0)) * X_time12;
        }
        else if (m == 13)//time 12 第3，4根
		{
			X_time12 <<
                zero_ele,
				STBC_s1s2,
                zero_ele, 
                zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 14)//time 12 第3，4根
		{
			X_time12 <<
                zero_ele,
				Q[i][0], 0,
				0, conj(Q[i][0]),
				Q[i][1], 0,
				0, conj(Q[i][1]),
                zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
        else if (m == 15)//time 12 第3，4根
		{
			X_time12 <<
                zero_ele,
				Q[i][0], 0,
				0, conj(Q[i][0]),
				0, Q[i][1],
				-conj(Q[i][1]), 0,
                zero_ele;
			X_time12 << (1 / sqrt(2.0)) * X_time12;
		}
		//cout << "m=" << m << endl;
		//cout << "I_diag" << endl << X_time12.transpose() << endl;
		//X_final << X_time12.transpose();
		//cout << "X_final" << endl << X_final << endl;
        y_time12 = H * X_time12;
     //   +White_noise(N, 2);

        //receive

		for (int an = 0; an < 16; an++) {
			for (int u = 0; u < 4; u++) {
				for (int k = 0; k < 4; k++) {
					de_STBC << qpsk_map[u], qpsk_map[k], -conj(qpsk_map[k]), conj(qpsk_map[u]);
					de_STBC << (1 / sqrt(2.0)) * de_STBC;
					
                    if (an == 0)//time12 第1，2根
		{
			xr << de_STBC, zero_ele, zero_ele, zero_ele;
		}
		else if (an == 1)//time 12 第3，4根
		{
			xr <<
				qpsk_map[u], 0,
				0, conj(qpsk_map[u]),
				qpsk_map[k], 0,
				0, conj(qpsk_map[k]),
                zero_ele, zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
		else if (an == 2)//time 12 第3，4根
		{
			xr <<
				qpsk_map[u], 0,
				0, conj(qpsk_map[u]),
				0, qpsk_map[k],
				-conj(qpsk_map[k]), 0,
                zero_ele, zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 3)//time 12 第3，4根
		{
			xr <<
				qpsk_map[u], 0,
				0, conj(qpsk_map[u]),
                zero_ele, 
				qpsk_map[k], 0,
				0, conj(qpsk_map[k]),
                zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 4)//time 12 第3，4根
		{
			xr <<
				qpsk_map[u], 0,
				0, conj(qpsk_map[u]),
                zero_ele,
				0, qpsk_map[k],
				-conj(qpsk_map[k]), 0,
                zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 5)//time 12 第3，4根
		{
            xr << 
                qpsk_map[u], 0,
                0, conj(qpsk_map[u]),
                zero_ele, 
                zero_ele,
                qpsk_map[k], 0,
                0, conj(qpsk_map[k]);
            xr << (1 / sqrt(2.0)) * xr;
        }
        else if (an == 6)//time 12 第3，4根
		{
            xr << 
                qpsk_map[u], 0,
                0, conj(qpsk_map[u]),
                zero_ele,
                zero_ele,
                0, qpsk_map[k],
                -conj(qpsk_map[k]), 0;
            xr << (1 / sqrt(2.0)) * xr;
        }
		else if (an == 7)//time 12 第3，4根
		{
			xr <<
				0, qpsk_map[u],
				-conj(qpsk_map[u]), 0,
				qpsk_map[k], 0,
				0, conj(qpsk_map[k]),
                zero_ele, zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 8)//time 12 第3，4根
		{
			xr <<
				0, qpsk_map[u],
				-conj(qpsk_map[u]), 0,
				0, qpsk_map[k],
				-conj(qpsk_map[k]), 0,
                zero_ele, zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 9)//time 12 第3，4根
		{
			xr <<
				0, qpsk_map[u],
				-conj(qpsk_map[u]), 0,
                zero_ele,
				qpsk_map[k], 0,
				0, conj(qpsk_map[k]),
                zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 10)//time 12 第3，4根
		{
			xr <<
				0, qpsk_map[u],
				-conj(qpsk_map[u]), 0,
                zero_ele,
				0, qpsk_map[k], 
				-conj(qpsk_map[k]), 0,
                zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 11)//time 12 第3，4根
		{
            xr << 
                0, qpsk_map[u],
                -conj(qpsk_map[u]), 0,
                zero_ele,
                zero_ele,
                qpsk_map[k], 0,
                0, conj(qpsk_map[k]);
            xr << (1 / sqrt(2.0)) * xr;
        }
        else if (an == 12)//time 12 第3，4根
		{
            xr << 
                0, qpsk_map[u],
                -conj(qpsk_map[u]), 0,
                zero_ele,
                zero_ele,
                0, qpsk_map[k],
                -conj(qpsk_map[k]), 0;
            xr << (1 / sqrt(2.0)) * xr;
        }
        else if (an == 13)//time 12 第3，4根
		{
			xr <<
                zero_ele,
				de_STBC,
                zero_ele, 
                zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 14)//time 12 第3，4根
		{
			xr <<
                zero_ele,
				qpsk_map[u], 0,
				0, conj(qpsk_map[u]),
				qpsk_map[k], 0,
				0, conj(qpsk_map[k]),
                zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
        else if (an == 15)//time 12 第3，4根
		{
			xr <<
                zero_ele,
				qpsk_map[u], 0,
				0, conj(qpsk_map[u]),
				0, qpsk_map[k],
				-conj(qpsk_map[k]), 0,
                zero_ele;
			xr << (1 / sqrt(2.0)) * xr;
		}
					//x << xr.transpose();
					norm_output = (y_time12 - H * xr).norm();
					if (norm_output < min)
					{
						min = norm_output;
						an_save[i] = an;
						uk_save[i][0] = u;
						uk_save[i][1] = k;
					}
				}
			}
		}


		norm_output = 0;
		min = 9999999;

	}


	for (int i = 0; i < frame_w; i++) {
        
        for (int k = 0; k < antenna_bits_num; k++) {
			decode_bits[i][k] = ((an_save[i] >> k) % 2);
		}

		for (int k = 0; k < 2; k++) {
			decode_bits[i][k * 2 + antenna_bits_num] = (uk_save[i][k] >> 1) % 2;
			decode_bits[i][k * 2 + (antenna_bits_num + 1)] = (uk_save[i][k]) % 2;
		}

		for (int j = 0; j < bits_num; j++)
		{
			//			cout << decode_bits[i][j] << " " ;
			if (decode_bits[i][j] != input_bits[i][j])
				biterrno++;
		}
		dsignal += bits_num;

	}
	//cout <<"dsignal="<<dsignal<< endl;
	//cout <<"biterrno="<<biterrno<< endl;
	/*for (int i = 0; i < frame_w; i++)
	{
		for (int j = 0; j < bits_num; j++)
		{
						cout << decode_bits[i][j] << " " ;
			cout << input_bits[i][j] ;

		}
		puts("");
		for (int j = 0; j < bits_num; j++)
		{
						cout << decode_bits[i][j] << " " ;
			cout << decode_bits[i][j];

		}
		puts("");
		puts("");
	}*/
}
int main()
{

	//srand((unsigned)time(NULL));
	//initial();
	for (point = 0; point < POINTNO; point++)
	{
		pnstate = 0xaaaaaaaaL;
		snrdb = DB0 + point * DBSTEP;
		snr = (data_rate) * pow(10.0, 0.1 * snrdb);
		deviate = sqrt(0.5 / snr);
		biterrno = 0;
		errlevel = ERRNOSTEP;
		dsignal = 0;

		//cout<<point;
		//cout<<"biterrno "<<biterrno <<endl;
		while (biterrno < MAXERRNO)
		{

			if (biterrno > errlevel)
			{
				errlevel += ERRNOSTEP;

			}

			for (samp = 0; samp < SAMPLE_NUM; samp++) {

				TxRx();
				//cout<<"samp= "<<samp <<endl;
				//cout<<"biterrno ="<<biterrno <<endl;
				//printf("%5.2f   %18.16f   %10f   %15f \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
			}
		}
		//cout<<"errlevel "<<errlevel <<endl;
		//record();
		printf("%5.2f   %18.16f   %10f   %15f \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
	}


	system("pause");
	return 0;
}
