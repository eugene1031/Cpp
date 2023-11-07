#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <complex> 
#include <vector>
#include <cstdlib>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/SVD>

#define  OUTPUTFILE  "SASM-SPAP4Tx.txt"
#define  OPENSTRING  "#Eb/No(dB) BitErrRate         BitErrSample  TotalSample \n" 
#define  DATASTRING  "%5.2f      %18.16f %7d %11u 1207\n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate
#define  DB0		2.0
#define  DBSTEP		2.0
#define  POINTNO	20
#define  ERRNOSTEP  2
#define  MAXERRNO	1500//2000//1000//500
#define  SAMPLE_NUM 100000//5000000//500000//200000//100000
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              4
#define  N              4
#define  frame_w        20//35
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define  bits_num       31//28
#define  antenna_number 15//0~13
double data_rate = bits_num / M;

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

//G5有24種天線排列

int antenna_perm[24][4] = { 1,2,3,4,	1,2,4,3,	1,3,2,4,	1,3,4,2,	1,4,2,3,	1,4,3,2,
								2,1,3,4,	2,1,4,3,	2,3,1,4,	2,3,4,1,	2,4,1,3,	2,4,3,1,
								3,1,2,4,	3,1,4,2,	3,2,1,4,	3,2,4,1,	3,4,1,2,	3,4,2,1,
								4,1,2,3,	4,1,3,2,	4,2,1,3,	4,2,3,1,	4,3,1,2,	4,3,2,1, };


int antenna_perm24[24][4] = { 1,2,3,4, 1,2,4,3, 1,3,2,4, 1,3,4,2, 1,4,2,3, 1,4,3,2,
								2,1,3,4, 2,1,4,3, 2,3,1,4, 2,3,4,1, 2,4,1,3, 2,4,3,1,
								3,1,2,4, 3,1,4,2, 3,2,1,4, 3,2,4,1, 3,4,1,2, 3,4,2,1,
								4,1,2,3, 4,1,3,2, 4,2,1,3, 4,2,3,1, 4,3,1,2, 4,3,2,1

};

int antenna_perm2[144][4] = { 1,1,2,3, 1,1,3,2, 1,2,1,3, 1,3,1,2, 1,2,3,1, 1,3,2,1, 2,3,1,1, 3,2,1,1 ,2,1,3,1, 3,1,2,1, 2,1,1,3, 3,1,1,2,//12
							   1,1,2,4,	1,1,4,2, 1,2,1,4, 1,4,1,2, 1,2,4,1, 1,4,2,1, 2,4,1,1, 4,2,1,1, 2,1,4,1, 4,1,2,1, 2,1,1,4, 4,1,1,2,//24
							   1,1,3,4, 1,1,4,3, 1,3,1,4, 1,4,1,3, 1,3,4,1, 1,4,3,1, 3,4,1,1, 4,3,1,1, 3,1,4,1, 4,1,3,1, 3,1,1,4, 4,1,1,3,//36

							   2,2,1,3, 2,2,3,1, 2,1,2,3, 2,3,2,1, 2,1,3,2, 2,3,1,2, 1,3,2,2, 3,1,2,2, 1,2,3,2, 3,2,1,2, 1,2,2,3, 3,2,2,1,//48
							   2,2,1,4, 2,2,4,1, 2,1,2,4, 2,4,2,1, 2,1,4,2, 2,4,1,2, 1,4,2,2, 4,1,2,2, 1,2,4,2, 4,2,1,2, 1,2,2,4, 4,2,2,1,//60
							   2,2,3,4, 2,2,4,3, 2,3,2,4, 2,4,2,3, 2,3,4,2, 2,4,3,2, 3,4,2,2, 4,3,2,2, 3,2,4,2, 4,2,3,2, 3,2,2,4, 4,2,2,3,//72

							   3,3,1,2, 3,3,2,1, 3,1,3,2, 3,2,3,1, 3,1,2,3, 3,2,1,3, 1,2,3,3, 2,1,3,3, 1,3,2,3, 2,3,1,3, 1,3,3,2, 2,3,3,1,//84
							   3,3,1,4, 3,3,4,1, 3,1,3,4, 3,4,3,1, 3,1,4,3, 3,4,1,3, 1,4,3,3, 4,1,3,3, 1,3,4,3, 4,3,1,3, 1,3,3,4, 4,3,3,1,//96
							   3,3,2,4, 3,3,4,2, 3,2,3,4, 3,4,3,2, 3,2,4,3, 3,4,2,3, 2,4,3,3, 4,2,3,3, 2,3,4,3, 4,3,2,3, 2,3,3,4, 4,3,3,2,//108

							   4,4,1,2, 4,4,2,1, 4,1,4,2, 4,2,4,1, 4,1,2,4, 4,2,1,4, 1,2,4,4, 2,1,4,4, 1,4,2,4, 2,4,1,4, 1,4,4,2, 2,4,4,1,//120
							   4,4,1,3, 4,4,3,1, 4,1,4,3, 4,1,3,4, 4,3,4,1, 4,3,1,4, 1,3,4,4, 3,1,4,4, 1,4,3,4, 3,4,1,4, 1,4,4,3, 3,4,4,1,//132
							   4,4,2,3, 4,4,3,2, 4,2,4,3, 4,3,4,2, 4,2,3,4, 4,3,2,4, 2,3,4,4, 3,2,4,4, 2,4,3,4, 3,4,2,4, 2,4,4,3, 3,4,4,2 //144

};

int new_antenna_perm2[144][4] = { 1,1,2,3, 1,1,3,2, 1,2,1,3, 1,3,1,2, 1,2,3,1, 1,3,2,1,//6
								  1,1,2,4, 1,1,4,2, 1,2,1,4, 1,4,1,2, 1,2,4,1, 1,4,2,1,//12
								  1,1,3,4, 1,1,4,3, 1,3,1,4, 1,4,1,3, 1,3,4,1, 1,4,3,1,//18
								  2,2,1,3, 2,2,3,1, 2,1,2,3, 2,3,2,1, 2,1,3,2, 2,3,1,2,//24
								  2,2,1,4, 2,2,4,1, 2,1,2,4, 2,4,2,1, 2,1,4,2, 2,4,1,2,
								  2,2,3,4, 2,2,4,3, 2,3,2,4, 2,4,2,3, 2,3,4,2, 2,4,3,2,
								  3,3,1,2, 3,3,2,1, 3,1,3,2, 3,2,3,1, 3,1,2,3, 3,2,1,3,
								  3,3,1,4, 3,3,4,1, 3,1,3,4, 3,4,3,1, 3,1,4,3, 3,4,1,3,
								  3,3,2,4, 3,3,4,2, 3,2,3,4, 3,4,3,2, 3,2,4,3, 3,4,2,3,
								  4,4,1,2, 4,4,2,1, 4,1,4,2, 4,2,4,1, 4,1,2,4, 4,2,1,4,
								  4,4,1,3, 4,4,3,1, 4,1,4,3, 4,1,3,4, 4,3,4,1, 4,3,1,4,
								  4,4,2,3, 4,4,3,2, 4,2,4,3, 4,3,4,2, 4,2,3,4, 4,3,2,4,               //72個   index 0~71 ifm<72
	//================================================================================================//
									2,3,1,1, 3,2,1,1 ,2,1,3,1, 3,1,2,1,                               //76
									2,4,1,1, 4,2,1,1, 2,1,4,1, 4,1,2,1,                               //80
									3,4,1,1, 4,3,1,1, 3,1,4,1, 4,1,3,1,                               //84

									1,3,2,2, 3,1,2,2, 1,2,3,2, 3,2,1,2,                               //88
									1,4,2,2, 4,1,2,2, 1,2,4,2, 4,2,1,2,
									3,4,2,2, 4,3,2,2, 3,2,4,2, 4,2,3,2,

									1,2,3,3, 2,1,3,3, 1,3,2,3, 2,3,1,3,
									1,4,3,3, 4,1,3,3, 1,3,4,3, 4,3,1,3,
									2,4,3,3, 4,2,3,3, 2,3,4,3, 4,3,2,3,

									1,2,4,4, 2,1,4,4, 1,4,2,4, 2,4,1,4,
									1,3,4,4, 3,1,4,4, 1,4,3,4, 3,4,1,4,
									2,3,4,4, 3,2,4,4, 2,4,3,4, 3,4,2,4,								  //48個  index 72~119 ifm>=72 && m<120
	//================================================================================================//
									2,1,1,3, 3,1,1,2,
									2,1,1,4, 4,1,1,2,
									3,1,1,4, 4,1,1,3,

									1,2,2,3, 3,2,2,1,
									1,2,2,4, 4,2,2,1,
									3,2,2,4, 4,2,2,3,

									1,3,3,2, 2,3,3,1,
									1,3,3,4, 4,3,3,1,
									2,3,3,4, 4,3,3,2,

									1,4,4,2, 2,4,4,1,
									1,4,4,3, 3,4,4,1,
									2,4,4,3, 3,4,4,2                                                  //24個  index 120~143  if m>=120
};

int row_interleaved_pattern[6][4] = { 1,2,3,4,     //  1324 是不合法的
									1,3,4,2,
									1,4,2,3,
									2,1,4,3,
	                                2,4,3,1,
	                                2,3,1,4
};

int interleaved_pattern2[15][6] = { 1,2,3,4,5,6,  1,2,3,6,4,5,  1,2,3,5,6,4,
								   1,3,4,2,5,6,  1,3,4,5,6,2,  1,3,4,6,2,5,
								   1,4,2,3,5,6,  1,4,2,6,3,5,  1,4,2,5,6,3,
								   1,5,2,4,3,6,  1,5,2,6,4,3,  1,5,2,3,6,4,
								   1,6,2,4,5,3,  1,6,2,3,4,5,  1,6,2,5,3,4 };



int interleaved_pattern[105][8] = {
				{1,2,3,4,5,6,7,8},{1,2,3,4,5,8,6,7},{1,2,3,4,5,7,8,6},
				{1,2,3,5,4,7,6,8},{1,2,3,5,4,8,7,6},{1,2,3,5,4,6,8,7},
				{1,2,3,6,4,5,7,8},{1,2,3,6,4,8,5,7},{1,2,3,6,4,7,8,5},
				{1,2,3,7,4,6,5,8},{1,2,3,7,4,8,6,5},{1,2,3,7,4,5,8,6},
				{1,2,3,8,4,5,6,7},{1,2,3,8,4,7,5,6},{1,2,3,8,4,6,7,5},//15

				{1,3,2,4,5,7,6,8},{1,3,2,4,5,8,7,6},{1,3,2,4,5,6,8,7},
				{1,3,2,5,4,6,7,8},{1,3,2,5,4,8,6,7},{1,3,2,5,4,7,8,6},
				{1,3,2,6,4,7,5,8},{1,3,2,6,4,8,7,5},{1,3,2,6,4,5,8,7},
				{1,3,2,7,4,5,6,8},{1,3,2,7,4,8,5,6},{1,3,2,7,4,6,8,5},
				{1,3,2,8,4,6,5,7},{1,3,2,8,4,7,6,5},{1,3,2,8,4,5,7,6},//30

				{1,4,2,3,5,6,7,8},{1,4,2,3,5,8,6,7},{1,4,2,3,5,7,8,6},
				{1,4,2,5,3,7,6,8},{1,4,2,5,3,8,7,6},{1,4,2,5,3,6,8,7},
				{1,4,2,6,3,5,7,8},{1,4,2,6,3,8,5,7},{1,4,2,6,3,7,8,5},
				{1,4,2,7,3,6,5,8},{1,4,2,7,3,8,6,5},{1,4,2,7,3,5,8,6},
				{1,4,2,8,3,5,6,7},{1,4,2,8,3,7,5,6},{1,4,2,8,3,6,7,5},//45

				{1,5,2,3,4,7,6,8},{1,5,2,3,4,8,7,6},{1,5,2,3,4,6,8,7},//48
				{1,5,2,4,3,6,7,8},{1,5,2,4,3,8,6,7},{1,5,2,4,3,7,8,6},//51
				{1,5,2,6,3,7,4,8},{1,5,2,6,3,8,7,4},{1,5,2,6,3,4,8,7},//54    53:17283465->15263874
				{1,5,2,7,3,4,6,8},{1,5,2,7,3,8,4,6},{1,5,2,7,3,6,8,4},//      57:18234567->15273684
				{1,5,2,8,3,6,4,7},{1,5,2,8,3,7,6,4},{1,5,2,8,3,4,7,6},//60    59:18234675->15283764

				{1,6,2,3,4,5,7,8},{1,6,2,3,4,8,5,7},{1,6,2,3,4,7,8,5},//63
				{1,6,2,4,3,7,5,8},{1,6,2,4,3,8,7,5},{1,6,2,4,3,5,8,7},//66
				{1,6,2,5,3,4,7,8},{1,6,2,5,3,8,4,7},{1,6,2,5,3,7,8,4},//69    69:18234756->16253784
				{1,6,2,7,3,5,4,8},{1,6,2,7,3,8,5,4},{1,6,2,7,3,4,8,5},//72    71:18243576->16273854
				{1,6,2,8,3,4,5,7},{1,6,2,8,3,7,4,5},{1,6,2,8,3,5,7,4},//75

				{1,7,2,3,4,6,5,8},{1,7,2,3,4,8,6,5},{1,7,2,3,4,5,8,6},//78
				{1,7,2,4,3,5,6,8},{1,7,2,4,3,8,5,6},{1,7,2,4,3,6,8,5},//81
				{1,7,2,5,3,6,4,8},{1,7,2,5,3,8,6,4},{1,7,2,5,3,4,8,6},//84    83:18243765->17253864
				{1,7,2,6,3,4,5,8},{1,7,2,6,3,8,4,5},{1,7,2,6,3,5,8,4},//87    86:18253467->17263845,   87:18273564->17263584
				{1,7,2,8,3,4,6,5},{1,7,2,8,3,5,4,6},{1,7,2,8,3,6,5,4},//90    88:17283465->13856472

				{1,8,2,3,4,5,6,7},{1,8,2,3,4,6,7,5},{1,8,2,3,4,7,5,6},       // 91:18234567->13758462, 92:18234675->13657428,	93:18234756->13586274
				{1,8,2,4,3,6,5,7},{1,8,2,4,3,5,7,6},{1,8,2,4,3,7,6,5},       // 95:18243576->13578264, 96:18243765->13567284
				{1,8,2,5,3,4,6,7},{1,8,2,5,3,6,7,4},{1,8,2,5,3,7,4,6},       // 97:18253467->13546782
				{1,8,2,6,4,3,5,7},{1,8,2,6,4,5,7,3},{1,8,2,6,4,7,3,5},
				{1,8,2,7,3,4,5,6},{1,8,2,7,3,5,6,4},{1,8,2,7,3,6,4,5}        //104:18273564->13547268
};


int sp105[105][8] = {
1, 2, 3, 4, 5, 6, 7, 8,   1, 2, 3, 4, 5, 7, 8, 6,   1, 2, 3, 4, 5, 8, 6, 7, //1row 3
1, 2, 3, 5, 4, 6, 8, 7,   1, 2, 3, 5, 4, 8, 7, 6,   1, 2, 3, 5, 4, 7, 6, 8, //2row
1, 2, 3, 5, 6, 4, 7, 8,   1, 2, 3, 5, 6, 7, 8, 4,   1, 2, 3, 5, 6, 8, 4, 7, //
1, 2, 3, 5, 7, 4, 8, 6,   1, 2, 3, 5, 7, 8, 6, 4,   1, 2, 3, 5, 7, 6, 4, 8, //
1, 2, 3, 5, 8, 4, 6, 7,   1, 2, 3, 5, 8, 6, 7, 4,   1, 2, 3, 5, 8, 7, 4, 6, //5row  15

1, 3, 2, 4, 5, 7, 6, 8,   1, 3, 2, 4, 5, 6, 8, 7,   1, 3, 2, 4, 5, 8, 7, 6, //6row
1, 3, 2, 5, 4, 6, 7, 8,   1, 3, 2, 5, 4, 7, 8, 6,   1, 3, 2, 5, 4, 8, 6, 7, //
1, 3, 2, 5, 6, 4, 8, 7,   1, 3, 2, 5, 6, 8, 7, 4,   1, 3, 2, 5, 6, 7, 4, 8, //
1, 3, 2, 5, 7, 4, 6, 8,   1, 3, 2, 5, 7, 6, 8, 4,   1, 3, 2, 5, 7, 8, 4, 6, //
1, 3, 2, 5, 8, 4, 7, 6,   1, 3, 2, 5, 8, 7, 6, 4,   1, 3, 2, 5, 8, 6, 4, 7, //10row

1, 3, 4, 2, 5, 6, 7, 8,   1, 3, 4, 2, 5, 7, 8, 6,   1, 3, 4, 2, 5, 8, 6, 7, //11row
1, 3, 5, 2, 4, 6, 8, 7,   1, 3, 5, 2, 4, 8, 7, 6,   1, 3, 5, 2, 4, 7, 6, 8, //
1, 3, 5, 2, 6, 4, 7, 8,   1, 3, 5, 2, 6, 7, 8, 4,   1, 3, 5, 2, 6, 8, 4, 7, //
1, 3, 5, 2, 7, 4, 8, 6,   1, 3, 5, 2, 7, 8, 6, 4,   1, 3, 5, 2, 7, 6, 4, 8, //
1, 3, 5, 2, 8, 4, 6, 7,   1, 3, 5, 2, 8, 6, 7, 4,   1, 3, 5, 2, 8, 7, 4, 6, //15row

1, 3, 4, 5, 2, 6, 8, 7,   1, 3, 4, 5, 2, 8, 7, 6,   1, 3, 4, 5, 2, 7, 6, 8, //16row
1, 3, 5, 4, 2, 6, 7, 8,   1, 3, 5, 4, 2, 7, 8, 6,   1, 3, 5, 4, 2, 8, 6, 7, //
1, 3, 5, 6, 2, 4, 8, 7,   1, 3, 5, 6, 2, 8, 7, 4,   1, 3, 5, 6, 2, 7, 4, 8, //
1, 3, 5, 7, 2, 4, 6, 8,   1, 3, 5, 7, 2, 6, 8, 4,   1, 3, 5, 7, 2, 8, 4, 6, //
1, 3, 5, 8, 2, 4, 7, 6,   1, 3, 5, 8, 2, 7, 6, 4,   1, 3, 5, 8, 2, 6, 4, 7, //20row

1, 3, 4, 5, 6, 2, 7, 8,   1, 3, 4, 5, 6, 7, 8, 2,   1, 3, 4, 5, 6, 8, 2, 7, //21row
1, 3, 4, 5, 7, 2, 8, 6,   1, 3, 4, 5, 7, 8, 6, 2,   1, 3, 4, 5, 7, 6, 2, 8, //
1, 3, 4, 5, 8, 2, 6, 7,   1, 3, 4, 5, 8, 6, 7, 2,   1, 3, 4, 5, 8, 7, 2, 6, //
1, 3, 5, 4, 6, 2, 8, 7,   1, 3, 5, 4, 6, 8, 7, 2,   1, 3, 5, 4, 6, 7, 2, 8, //
1, 3, 5, 4, 7, 2, 6, 8,   1, 3, 5, 4, 7, 6, 8, 2,   1, 3, 5, 4, 7, 8, 2, 6, //25row

1, 3, 5, 4, 8, 2, 7, 6,   1, 3, 5, 4, 8, 7, 6, 2,   1, 3, 5, 4, 8, 6, 2, 7, //26row
1, 3, 5, 6, 4, 2, 7, 8,   1, 3, 5, 6, 4, 7, 8, 2,   1, 3, 5, 6, 4, 8, 2, 7,  //
1, 3, 5, 6, 7, 2, 8, 4,   1, 3, 5, 6, 7, 8, 4, 2,   1, 3, 5, 6, 7, 4, 2, 8, //
1, 3, 5, 6, 8, 2, 4, 7,   1, 3, 5, 6, 8, 4, 7, 2,   1, 3, 5, 6, 8, 7, 2, 4, //
1, 3, 5, 7, 4, 2, 8, 6,   1, 3, 5, 7, 4, 8, 6, 2,   1, 3, 5, 7, 4, 6, 2, 8, //30row

1, 3, 5, 7, 6, 2, 4, 8,   1, 3, 5, 7, 6, 4, 8, 2,   1, 3, 5, 7, 6, 8, 2, 4, //31row
1, 3, 5, 7, 8, 2, 6, 4,   1, 3, 5, 7, 8, 6, 4, 2,   1, 3, 5, 7, 8, 4, 2, 6, //
1, 3, 5, 8, 4, 2, 6, 7,   1, 3, 5, 8, 4, 6, 7, 2,   1, 3, 5, 8, 4, 7, 2, 6, //
1, 3, 5, 8, 6, 2, 7, 4,   1, 3, 5, 8, 6, 7, 4, 2,   1, 3, 5, 8, 6, 4, 2, 7, //
1, 3, 5, 8, 7, 2, 4, 6,   1, 3, 5, 8, 7, 4, 6, 2,   1, 3, 5, 8, 7, 6, 2, 4, //35row 105種
};


int sp840[840][8] = { 0 };




template<class T>
void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}


void creatsp840() {
	//將sp105個資料放8個給sp840
	for (int x = 0; x < 8; x++) {
		for (int row = 0; row < 105; row++)
		{
			for (int col = 0; col < 8; col++)
			{
				sp840[105 * x + row][col] = sp105[row][col];

			}
		}
	}
}
//th代表第幾組105
//ab交換 cd交換 
//印出[r][c]的矩陣 
void show()
{
	int r = 840;
	int c = 8;
	//印出symbol permutaion矩陣
	for (int row = 0; row < r; row++) {
		for (int i = 0; i < c; i++) {
			cout << sp840[row][i] << "\t";
		}
		cout << endl;
	}
}

//sp840[839][7] = 999;//測試 sp840的最後一個資料
//  [105] [column]~[105 * 2 - 1][col]     th=1
//	[105 * 2][column]~[105 * 3 - 1][col]
//	[105 * 3][column]~[105 * 4 - 1][col]
//	[105 * 4][column]~[105 * 5 - 1][col]
//	[105 * 5][column]~[105 * 6 - 1][col]
//	[105 * 6][column]~[105 * 7 - 1][col]
//	[105 * 7][column]~[105 * 8 - 1][col]
//  0000                      th=0
//  1100  12交換 34交換	th=1
//  1010  12交換 56交換	th=2
//  1001  12交換 78交換	th=3
//  0110  34交換 56交換	th=4
//	0101  34交換 78交換	th=5
//	0011  56交換 78交換	th=6
//	1111  12交換 34交換 56交換 78交換 th=7

void change(int a, int b, int c, int d, int th)
{
	//如果th=1 for (int i = 105*1; i < 105 * 2 - 1; i++)
	//th=2  for (int i = 105*2; i < 105 * 3 - 1; i++)
	for (int i = 105 * th; i < 105 * (th + 1); i++)
	{
		for (int z = 0; z < 8; z++)
		{
			if (sp840[i][z] == a)
				sp840[i][z] = b;
			else if (sp840[i][z] == b)
				sp840[i][z] = a;
			else if (sp840[i][z] == c)
				sp840[i][z] = d;
			else if (sp840[i][z] == d)
				sp840[i][z] = c;
		}
	}
}
void change4(int a, int b, int c, int d, int e, int f, int g, int h, int th)
{
	//如果th=1 for (int i = 105*1; i < 105 * 2 - 1; i++)
	//th=2  for (int i = 105*2; i < 105 * 3 - 1; i++)
	for (int i = 105 * th; i < 105 * (th + 1); i++)
	{
		for (int z = 0; z < 8; z++)
		{
			if (sp840[i][z] == a)
				sp840[i][z] = b;
			else if (sp840[i][z] == b)
				sp840[i][z] = a;
			else if (sp840[i][z] == c)
				sp840[i][z] = d;
			else if (sp840[i][z] == d)
				sp840[i][z] = c;
			else if (sp840[i][z] == e)
				sp840[i][z] = f;
			else if (sp840[i][z] == f)
				sp840[i][z] = e;
			else if (sp840[i][z] == g)
				sp840[i][z] = h;
			else if (sp840[i][z] == h)
				sp840[i][z] = g;
		}
	}
}
//==========================================================================================================================

MatrixXcd H(N, 4), H_new(N, 4), H_test(N, 2), noise(N, 8), Y_me(N, 8), ABC(4, 4), I_row(4, 4);
MatrixXcd  Y1(N, 2), Y2(N, 2), Y3(N, 2), Y4(N, 2), y(N, 8), NY(N, 8);
MatrixXcd  Y_1(N, 2), Y_2(N, 2), Y_3(N, 2), Y_4(N, 2);
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

void Y1Y2Y3Y4(int c, int a) {
	if (a == 0) {
		//new define
		NY.col(sp840[c][0] - 1) << y.col(0);
		NY.col(sp840[c][1] - 1) << y.col(1);
		NY.col(sp840[c][2] - 1) << y.col(2);
		NY.col(sp840[c][3] - 1) << y.col(3);
		NY.col(sp840[c][4] - 1) << y.col(4);
		NY.col(sp840[c][5] - 1) << y.col(5);
		NY.col(sp840[c][6] - 1) << y.col(6);
		NY.col(sp840[c][7] - 1) << y.col(7);

		Y1.col(0) << NY.col(0);
		Y1.col(1) << NY.col(1);
		Y2.col(0) << NY.col(2);
		Y2.col(1) << NY.col(3);
		Y3.col(0) << NY.col(4);
		Y3.col(1) << NY.col(5);
		Y4.col(0) << NY.col(6);
		Y4.col(1) << NY.col(7);
	}


}

void receive()//void frame_length()
{
	//srand((unsigned)time(NULL));

	int input_bits[frame_w][bits_num], decode_bits[frame_w][bits_num], ab_save[frame_w][M * 2], ab_save_temp[frame_w][M * 2], m_save[frame_w], c_save[frame_w], final_save[frame_w]
		, interleaved, reference_order, m, r, g
		, sp, ant;
	double norm_save[16384] = { 0 };
	int debug_savem[16384] = { 0 };
	double norm_output = 0, norm_output1 = 0, norm_output2 = 0, norm_output3 = 0, norm_output4 = 0, min = 9999999
		, temp_min1 = 9999999, temp_min2 = 9999999, temp_min3 = 9999999, temp_min4 = 9999999;

	complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) }, Q[frame_w][M * 2];

	MatrixXcd zero(2, 2), /*Y1(N, 2), Y2(N, 2),*/ I_s1s2(2, 2), I_s3s4(2, 2), I_s5s6(2, 2), I_s7s8(2, 2), Ia(4, 2), Ib(4, 2), Ic(4, 2), Id(4, 2), Ya(4, 2), Yb(4, 2), Yc(4, 2), Yd(4, 2), I_me(4, 8), I_me_new(4, 8), Y_me(4, 8), X_me(4, 8), X_me1(4, 2), X_me2(4, 2), X_me3(4, 2), X_me4(4, 2), X_m1(4, 2), X_m2(4, 2), X_m3(4, 2), X_m4(4, 2), I_diag(M, M), I_diag2(M, M), St1(M, M), /*y(N, M),*/ de_STBC(2, 2);

	MatrixXcd st2(4, 8), temp(2 * M, 6), save(2 * M, 6), diff(4, 8);

	zero << 0, 0,
		0, 0;
	//MatrixXcd* H_ptr = &H;
	//===========================encoder_start===============================

	fading_channel(N, 4);

	for (int i = 0; i < frame_w; i++)
	{
		for (int j = 0; j < antenna_number; j++)
		{
			input_bits[i][j] = rand() % 2;
		}
		for (int j = antenna_number; j < bits_num; j++)
		{
			input_bits[i][j] = rand() % 2;
		}
		//input_bits[i][0] = 0;
		//input_bits[i][1] = 1;
		//input_bits[i][2] = 0;
		//input_bits[i][3] = 0;
		//input_bits[i][4] = 0;
		//input_bits[i][5] = 0;
		//input_bits[i][6] = 0;
		//input_bits[i][7] = 0;
		//input_bits[i][8] = 0;
		//input_bits[i][9] = 0;
		//input_bits[i][10] = 0;
		//input_bits[i][11] = 0;
		//input_bits[i][12] = 0;
		//input_bits[i][13] = 0;
		//input_bits[i][14] = 1;
		//input_bits[i][15] = 0;
		//input_bits[i][16] = 0;
		//input_bits[i][17] = 1;
		//input_bits[i][18] = 0;
		//input_bits[i][19] = 0;
		//input_bits[i][20] = 1;
		//input_bits[i][21] = 0;
		//input_bits[i][22] = 1;
		//input_bits[i][23] = 0;
		//input_bits[i][24] = 1;
		//input_bits[i][25] = 0;
		//input_bits[i][26] = 0;
		//input_bits[i][27] = 1;
		//input_bits[i][28] = 1;
		//input_bits[i][29] = 0;

		for (int j = 0; j < 8; j++)
		{
			Q[i][j] = qpsk_map[(input_bits[i][j * 2 + antenna_number] << 1)
				+ input_bits[i][j * 2 + (antenna_number + 1)]];

		}

		min = 999999;

		m = (input_bits[i][0]) + ((input_bits[i][1]) << 1)
			+ ((input_bits[i][2]) << 2) + ((input_bits[i][3]) << 3)
			+ ((input_bits[i][4]) << 4) + ((input_bits[i][5]) << 5)
			+ ((input_bits[i][6]) << 6) + ((input_bits[i][7]) << 7)
			+ ((input_bits[i][8]) << 8) + ((input_bits[i][9]) << 9)
			+ ((input_bits[i][10]) << 10) + ((input_bits[i][11]) << 11)
			+ ((input_bits[i][12]) << 12) + ((input_bits[i][13]) << 13)
			+ ((input_bits[i][14]) << 14);

		sp = m / 24; //交錯
		int col = m / 24 % 840;	//840種col
		int row = m / 24 / 840;	//6種row

		ant = m % 24; // 天線排列
		Ia << Q[i][0], Q[i][1],
			-conj(Q[i][1]), conj(Q[i][0]),
			0, 0,
			0, 0;
		Ia = 1 / sqrt(2.0) * Ia;
		Ib <<
			Q[i][2], 0,
			0, conj(Q[i][2]),
			Q[i][3], 0,
			0, conj(Q[i][3]);
		Ib = 1 / sqrt(2.0) * Ib;
		Ic <<
			Q[i][4], 0,
			0, conj(Q[i][4]),
			0, Q[i][5],
			-conj(Q[i][5]), 0;
		Ic = 1 / sqrt(2.0) * Ic;
		Id <<
			0, Q[i][6],
			-conj(Q[i][6]), 0,
			Q[i][7], 0,
			0, conj(Q[i][7]);
		Id = 1 / sqrt(2.0) * Id;

		if (ant == 0) {
			I_me << Ia, Ib, Ic, Id;
		}
		else if (ant == 1) {
			I_me <<
				Ia, Ib, Id, Ic;
		}
		else if (ant == 2) {
			I_me <<
				Ia, Ic, Ib, Id;
		}
		else if (ant == 3) {
			I_me <<
				Ia, Ic, Id, Ib;
		}
		else if (ant == 4) {
			I_me <<
				Ia, Id, Ib, Ic;
		}
		else if (ant == 5) {
			I_me <<
				Ia, Id, Ic, Ib;
		}
		else if (ant == 6) {
			I_me <<
				Ib, Ia, Ic, Id;
		}
		else if (ant == 7) {
			I_me <<
				Ib, Ia, Id, Ic;
		}
		else if (ant == 8) {
			I_me <<
				Ib, Ic, Ia, Id;
		}
		else if (ant == 9) {
			I_me <<
				Ib, Ic, Id, Ia;
		}
		else if (ant == 10) {
			I_me <<
				Ib, Id, Ia, Ic;
		}
		else if (ant == 11) {
			I_me <<
				Ib, Id, Ic, Ia;
		}
		else if (ant == 12) {
			I_me <<
				Ic, Ia, Ib, Id;
		}
		else if (ant == 13) {
			I_me <<
				Ic, Ia, Id, Ib;
		}
		else if (ant == 14) {
			I_me <<
				Ic, Ib, Ia, Id;
		}
		else if (ant == 15) {
			I_me <<
				Ic, Ib, Id, Ia;
		}
		else if (ant == 16) {
			I_me <<
				Ic, Id, Ia, Ib;
		}
		else if (ant == 17) {
			I_me <<
				Ic, Id, Ib, Ia;
		}
		else if (ant == 18) {
			I_me <<
				Id, Ia, Ib, Ic;
		}
		else if (ant == 19) {
			I_me <<
				Id, Ia, Ic, Ib;
		}
		else if (ant == 20) {
			I_me <<
				Id, Ib, Ia, Ic;
		}
		else if (ant == 21) {
			I_me <<
				Id, Ib, Ic, Ia;
		}
		else if (ant == 22) {
			I_me <<
				Id, Ic, Ia, Ib;
		}
		else if (ant == 23) {
			I_me <<
				Id, Ic, Ib, Ia;
		}

		//new define
		I_me_new.col(0) << I_me.col(sp840[col][0] - 1);
		I_me_new.col(1) << I_me.col(sp840[col][1] - 1);
		I_me_new.col(2) << I_me.col(sp840[col][2] - 1);
		I_me_new.col(3) << I_me.col(sp840[col][3] - 1);
		I_me_new.col(4) << I_me.col(sp840[col][4] - 1);
		I_me_new.col(5) << I_me.col(sp840[col][5] - 1);
		I_me_new.col(6) << I_me.col(sp840[col][6] - 1);
		I_me_new.col(7) << I_me.col(sp840[col][7] - 1);

		for (int kk = 0; kk < 4; kk++) {
			st2.row(kk) << I_me_new.row(row_interleaved_pattern[row][kk] - 1);
		}

		ABC << 1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1;
		/////////////傳送式子/////////////
		y = H * st2;// +White_noise(N, 8);

		//cout << "m:" << endl << m << endl;
		//cout << "sp:" << endl << sp << endl;
		//cout << "ant:" << endl << ant << endl;
		//cout << "I_me:" << endl << I_me << endl;
		//cout << "st2:" << endl << st2 << endl;
		min = 9999999;
		norm_output = 0;
		/*
		//印出檢查 因為在最後面的接收端 印出來的inputbits會與傳送端的不相同 但會完全與decodebits相同
		for (int j = 0; j < bits_num; j++) {
			cout << input_bits[i][j];
		}
		puts("");
		*/
		Eigen::MatrixXcd Ya_all[4][4];
		Eigen::MatrixXcd Yb_all[4][4];
		Eigen::MatrixXcd Yc_all[4][4];
		Eigen::MatrixXcd Yd_all[4][4];

		for (int a = 0; a < 4; a++) {
			for (int b = 0; b < 4; b++) {


				Ya << qpsk_map[a], qpsk_map[b],
					-conj(qpsk_map[b]), conj(qpsk_map[a]),
					0, 0,
					0, 0;
				Ya = 1 / sqrt(2.0) * Ya;
				Ya_all[a][b] = Ya;

				Yb <<
					qpsk_map[a], 0,
					0, conj(qpsk_map[a]),
					qpsk_map[b], 0,
					0, conj(qpsk_map[b]);
				Yb = 1 / sqrt(2.0) * Yb;
				Yb_all[a][b] = Yb;

				Yc <<
					qpsk_map[a], 0,
					0, conj(qpsk_map[a]),
					0, qpsk_map[b],
					-conj(qpsk_map[b]), 0;
				Yc = 1 / sqrt(2.0) * Yc;
				Yc_all[a][b] = Yc;

				Yd <<
					0, qpsk_map[a],
					-conj(qpsk_map[a]), 0,
					qpsk_map[b], 0,
					0, conj(qpsk_map[b]);
				Yd = 1 / sqrt(2.0) * Yd;
				Yd_all[a][b] = Yd;
			}
		}

		//接收端 解接收到的訊號回傳送訊號
		for (int mr = 0; mr < 32768; mr++) //0~16383 共16384種
		{
			temp_min1 = 999999;
			temp_min2 = 999999;
			temp_min3 = 999999;
			temp_min4 = 999999;

			int de_col = mr / 24 % 840;
			int de_row = mr / 24 / 840;
			reference_order = mr % 24;
			Y1Y2Y3Y4(de_col, 0);

			for (int a = 0; a < 4; a++)
			{
				for (int b = 0; b < 4; b++)
				{
					Ya = Ya_all[a][b];
					Yb = Yb_all[a][b];
					Yc = Yc_all[a][b];
					Yd = Yd_all[a][b];
					Y_me << Ya, Yb, Yc, Yd;
					if (reference_order == 0) {
						Y_1.col(0) << Y1.col(0);
						Y_1.col(1) << Y1.col(1);
						Y_2.col(0) << Y2.col(0);
						Y_2.col(1) << Y2.col(1);
						Y_3.col(0) << Y3.col(0);
						Y_3.col(1) << Y3.col(1);
						Y_4.col(0) << Y4.col(0);
						Y_4.col(1) << Y4.col(1);
					}
					if (reference_order == 1) {
						Y_1.col(0) << Y1.col(0);
						Y_1.col(1) << Y1.col(1);
						Y_2.col(0) << Y2.col(0);
						Y_2.col(1) << Y2.col(1);
						Y_4.col(0) << Y3.col(0);
						Y_4.col(1) << Y3.col(1);
						Y_3.col(0) << Y4.col(0);
						Y_3.col(1) << Y4.col(1);
					}
					if (reference_order == 2) {
						Y_1.col(0) << Y1.col(0);
						Y_1.col(1) << Y1.col(1);
						Y_3.col(0) << Y2.col(0);
						Y_3.col(1) << Y2.col(1);
						Y_2.col(0) << Y3.col(0);
						Y_2.col(1) << Y3.col(1);
						Y_4.col(0) << Y4.col(0);
						Y_4.col(1) << Y4.col(1);
					}
					if (reference_order == 3) {
						Y_1.col(0) << Y1.col(0);
						Y_1.col(1) << Y1.col(1);
						Y_3.col(0) << Y2.col(0);
						Y_3.col(1) << Y2.col(1);
						Y_4.col(0) << Y3.col(0);
						Y_4.col(1) << Y3.col(1);
						Y_2.col(0) << Y4.col(0);
						Y_2.col(1) << Y4.col(1);
					}
					if (reference_order == 4) {
						Y_1.col(0) << Y1.col(0);
						Y_1.col(1) << Y1.col(1);
						Y_4.col(0) << Y2.col(0);
						Y_4.col(1) << Y2.col(1);
						Y_2.col(0) << Y3.col(0);
						Y_2.col(1) << Y3.col(1);
						Y_3.col(0) << Y4.col(0);
						Y_3.col(1) << Y4.col(1);
					}
					if (reference_order == 5) {
						Y_1.col(0) << Y1.col(0);
						Y_1.col(1) << Y1.col(1);
						Y_4.col(0) << Y2.col(0);
						Y_4.col(1) << Y2.col(1);
						Y_3.col(0) << Y3.col(0);
						Y_3.col(1) << Y3.col(1);
						Y_2.col(0) << Y4.col(0);
						Y_2.col(1) << Y4.col(1);
					}
					if (reference_order == 6) {
						Y_2.col(0) << Y1.col(0);
						Y_2.col(1) << Y1.col(1);
						Y_1.col(0) << Y2.col(0);
						Y_1.col(1) << Y2.col(1);
						Y_3.col(0) << Y3.col(0);
						Y_3.col(1) << Y3.col(1);
						Y_4.col(0) << Y4.col(0);
						Y_4.col(1) << Y4.col(1);
					}
					if (reference_order == 7) {
						Y_2.col(0) << Y1.col(0);
						Y_2.col(1) << Y1.col(1);
						Y_1.col(0) << Y2.col(0);
						Y_1.col(1) << Y2.col(1);
						Y_4.col(0) << Y3.col(0);
						Y_4.col(1) << Y3.col(1);
						Y_3.col(0) << Y4.col(0);
						Y_3.col(1) << Y4.col(1);
					}
					if (reference_order == 8) {
						Y_2.col(0) << Y1.col(0);
						Y_2.col(1) << Y1.col(1);
						Y_3.col(0) << Y2.col(0);
						Y_3.col(1) << Y2.col(1);
						Y_1.col(0) << Y3.col(0);
						Y_1.col(1) << Y3.col(1);
						Y_4.col(0) << Y4.col(0);
						Y_4.col(1) << Y4.col(1);
					}
					if (reference_order == 9) {
						Y_2.col(0) << Y1.col(0);
						Y_2.col(1) << Y1.col(1);
						Y_3.col(0) << Y2.col(0);
						Y_3.col(1) << Y2.col(1);
						Y_4.col(0) << Y3.col(0);
						Y_4.col(1) << Y3.col(1);
						Y_1.col(0) << Y4.col(0);
						Y_1.col(1) << Y4.col(1);
					}
					if (reference_order == 10) {
						Y_2.col(0) << Y1.col(0);
						Y_2.col(1) << Y1.col(1);
						Y_4.col(0) << Y2.col(0);
						Y_4.col(1) << Y2.col(1);
						Y_1.col(0) << Y3.col(0);
						Y_1.col(1) << Y3.col(1);
						Y_3.col(0) << Y4.col(0);
						Y_3.col(1) << Y4.col(1);
					}
					if (reference_order == 11) {
						Y_2.col(0) << Y1.col(0);
						Y_2.col(1) << Y1.col(1);
						Y_4.col(0) << Y2.col(0);
						Y_4.col(1) << Y2.col(1);
						Y_3.col(0) << Y3.col(0);
						Y_3.col(1) << Y3.col(1);
						Y_1.col(0) << Y4.col(0);
						Y_1.col(1) << Y4.col(1);
					}
					if (reference_order == 12) {
						Y_3.col(0) << Y1.col(0);
						Y_3.col(1) << Y1.col(1);
						Y_1.col(0) << Y2.col(0);
						Y_1.col(1) << Y2.col(1);
						Y_2.col(0) << Y3.col(0);
						Y_2.col(1) << Y3.col(1);
						Y_4.col(0) << Y4.col(0);
						Y_4.col(1) << Y4.col(1);
					}
					if (reference_order == 13) {
						Y_3.col(0) << Y1.col(0);
						Y_3.col(1) << Y1.col(1);
						Y_1.col(0) << Y2.col(0);
						Y_1.col(1) << Y2.col(1);
						Y_4.col(0) << Y3.col(0);
						Y_4.col(1) << Y3.col(1);
						Y_2.col(0) << Y4.col(0);
						Y_2.col(1) << Y4.col(1);
					}
					if (reference_order == 14) {
						Y_3.col(0) << Y1.col(0);
						Y_3.col(1) << Y1.col(1);
						Y_2.col(0) << Y2.col(0);
						Y_2.col(1) << Y2.col(1);
						Y_1.col(0) << Y3.col(0);
						Y_1.col(1) << Y3.col(1);
						Y_4.col(0) << Y4.col(0);
						Y_4.col(1) << Y4.col(1);
					}
					if (reference_order == 15) {
						Y_3.col(0) << Y1.col(0);
						Y_3.col(1) << Y1.col(1);
						Y_2.col(0) << Y2.col(0);
						Y_2.col(1) << Y2.col(1);
						Y_4.col(0) << Y3.col(0);
						Y_4.col(1) << Y3.col(1);
						Y_1.col(0) << Y4.col(0);
						Y_1.col(1) << Y4.col(1);
					}
					if (reference_order == 16) {
						Y_3.col(0) << Y1.col(0);
						Y_3.col(1) << Y1.col(1);
						Y_4.col(0) << Y2.col(0);
						Y_4.col(1) << Y2.col(1);
						Y_1.col(0) << Y3.col(0);
						Y_1.col(1) << Y3.col(1);
						Y_2.col(0) << Y4.col(0);
						Y_2.col(1) << Y4.col(1);
					}
					if (reference_order == 17) {
						Y_3.col(0) << Y1.col(0);
						Y_3.col(1) << Y1.col(1);
						Y_4.col(0) << Y2.col(0);
						Y_4.col(1) << Y2.col(1);
						Y_2.col(0) << Y3.col(0);
						Y_2.col(1) << Y3.col(1);
						Y_1.col(0) << Y4.col(0);
						Y_1.col(1) << Y4.col(1);
					}
					if (reference_order == 18) {
						Y_4.col(0) << Y1.col(0);
						Y_4.col(1) << Y1.col(1);
						Y_1.col(0) << Y2.col(0);
						Y_1.col(1) << Y2.col(1);
						Y_2.col(0) << Y3.col(0);
						Y_2.col(1) << Y3.col(1);
						Y_3.col(0) << Y4.col(0);
						Y_3.col(1) << Y4.col(1);
					}
					if (reference_order == 19) {
						Y_4.col(0) << Y1.col(0);
						Y_4.col(1) << Y1.col(1);
						Y_1.col(0) << Y2.col(0);
						Y_1.col(1) << Y2.col(1);
						Y_3.col(0) << Y3.col(0);
						Y_3.col(1) << Y3.col(1);
						Y_2.col(0) << Y4.col(0);
						Y_2.col(1) << Y4.col(1);
					}
					if (reference_order == 20) {
						Y_4.col(0) << Y1.col(0);
						Y_4.col(1) << Y1.col(1);
						Y_2.col(0) << Y2.col(0);
						Y_2.col(1) << Y2.col(1);
						Y_1.col(0) << Y3.col(0);
						Y_1.col(1) << Y3.col(1);
						Y_3.col(0) << Y4.col(0);
						Y_3.col(1) << Y4.col(1);
					}
					if (reference_order == 21) {
						Y_4.col(0) << Y1.col(0);
						Y_4.col(1) << Y1.col(1);
						Y_2.col(0) << Y2.col(0);
						Y_2.col(1) << Y2.col(1);
						Y_3.col(0) << Y3.col(0);
						Y_3.col(1) << Y3.col(1);
						Y_1.col(0) << Y4.col(0);
						Y_1.col(1) << Y4.col(1);
					}
					if (reference_order == 22) {
						Y_4.col(0) << Y1.col(0);
						Y_4.col(1) << Y1.col(1);
						Y_3.col(0) << Y2.col(0);
						Y_3.col(1) << Y2.col(1);
						Y_1.col(0) << Y3.col(0);
						Y_1.col(1) << Y3.col(1);
						Y_2.col(0) << Y4.col(0);
						Y_2.col(1) << Y4.col(1);
					}
					if (reference_order == 23) {
						Y_4.col(0) << Y1.col(0);
						Y_4.col(1) << Y1.col(1);
						Y_3.col(0) << Y2.col(0);
						Y_3.col(1) << Y2.col(1);
						Y_2.col(0) << Y3.col(0);
						Y_2.col(1) << Y3.col(1);
						Y_1.col(0) << Y4.col(0);
						Y_1.col(1) << Y4.col(1);
					}

					X_me1.col(0) << Y_me.col(0);
					X_me1.col(1) << Y_me.col(1);
					X_me2.col(0) << Y_me.col(2);
					X_me2.col(1) << Y_me.col(3);
					X_me3.col(0) << Y_me.col(4);
					X_me3.col(1) << Y_me.col(5);
					X_me4.col(0) << Y_me.col(6);
					X_me4.col(1) << Y_me.col(7);

					for (int kk = 0; kk < 4; kk++) {
						I_row.row(kk) << ABC.row(row_interleaved_pattern[de_row][kk] - 1);
					}
					H_new = H * I_row;
					norm_output1 = (Y_1 - H_new * X_me1).squaredNorm();
					norm_output2 = (Y_2 - H_new * X_me2).squaredNorm();
					norm_output3 = (Y_3 - H_new * X_me3).squaredNorm();
					norm_output4 = (Y_4 - H_new * X_me4).squaredNorm();



					//因此使用for迴圈去找最小值一直到接近0或等於零食存入
					//第12時間最小值
					if (norm_output1 < temp_min1) {
						temp_min1 = norm_output1;
						ab_save_temp[i][0] = a;
						ab_save_temp[i][1] = b;
						//cout << "mr:" << mr << endl;
						//cout << "a:" << a << endl;
						//cout << "b:" << b << endl;
						//cout << "norm1:" << temp_min1 << endl;
					}
					//第34時間最小值
					if (norm_output2 < temp_min2) {
						temp_min2 = norm_output2;
						ab_save_temp[i][2] = a;
						ab_save_temp[i][3] = b;
						//cout << temp_min2 << endl;
					}
					//第56時間最小值
					if (norm_output3 < temp_min3) {
						temp_min3 = norm_output3;
						ab_save_temp[i][4] = a;
						ab_save_temp[i][5] = b;
						//cout << "   mr = " << mr << endl;
						//cout << "temp3=" << temp_min3 << endl;
						//cout << "norm_output3=" << norm_output3 << endl;
					}
					//第78時間最小值
					if (norm_output4 < temp_min4) {
						temp_min4 = norm_output4;
						ab_save_temp[i][6] = a;
						ab_save_temp[i][7] = b;
					}
				}
			}

			norm_output = temp_min1 + temp_min2 + temp_min3 + temp_min4;

			if (norm_output < min)
			{

				//cout << "y=" << endl << y << endl;
				//cout << "Y2=" << endl << Y2 << endl;
				//cout << "34=" << endl << ABC * X_me2 << endl;
				//cout << "Y2 - ABC * X_me2=" << endl << Y2 - ABC * X_me2 << endl;

				//cout << "   mr = " << mr << endl;
				min = norm_output;
				//cout << "   min = " << min << endl;

				m_save[i] = mr;

				ab_save[i][0] = ab_save_temp[i][0];
				ab_save[i][1] = ab_save_temp[i][1];
				ab_save[i][2] = ab_save_temp[i][2];
				ab_save[i][3] = ab_save_temp[i][3];
				ab_save[i][4] = ab_save_temp[i][4];
				ab_save[i][5] = ab_save_temp[i][5];
				ab_save[i][6] = ab_save_temp[i][6];
				ab_save[i][7] = ab_save_temp[i][7];

			}
		}
		//norm_save[m_save[i]] = min; //因為有錯所以將Norm值存在對應的mr的矩陣內中 ex第mr格的norm_save矩陣匯存那個mr解出來的norm值
	}

	for (int i = 0; i < frame_w; i++)
	{
		decode_bits[i][0] = ((m_save[i]) % 2);
		decode_bits[i][1] = ((m_save[i] >> 1) % 2);
		decode_bits[i][2] = ((m_save[i] >> 2) % 2);
		decode_bits[i][3] = ((m_save[i] >> 3) % 2);
		decode_bits[i][4] = ((m_save[i] >> 4) % 2);
		decode_bits[i][5] = ((m_save[i] >> 5) % 2);
		decode_bits[i][6] = ((m_save[i] >> 6) % 2);
		decode_bits[i][7] = ((m_save[i] >> 7) % 2);
		decode_bits[i][8] = ((m_save[i] >> 8) % 2);
		decode_bits[i][9] = ((m_save[i] >> 9) % 2);
		decode_bits[i][10] = ((m_save[i] >> 10) % 2);
		decode_bits[i][11] = ((m_save[i] >> 11) % 2);
		decode_bits[i][12] = ((m_save[i] >> 12) % 2);
		decode_bits[i][13] = ((m_save[i] >> 13) % 2);
		decode_bits[i][14] = ((m_save[i] >> 14) % 2);
		//decode_bits[i][15] = ((m_save[i] >> 15) % 2);


		//解後面的QPSK
		for (int k = 0; k < 8; k++) {
			decode_bits[i][k * 2 + 15] = (ab_save[i][k] >> 1) % 2;  //16是天線位元
			decode_bits[i][k * 2 + 16] = (ab_save[i][k]) % 2;		//17是天線位元+1
		}

	}
	//算解錯的bits 記錄下來
	for (int i = 0; i < frame_w; i++)
	{
		for (int j = 0; j < bits_num; j++)
		{
			//			cout << decode_bits[i][j] << " " ;
			if (decode_bits[i][j] != input_bits[i][j])
				biterrno++;
		}
		dsignal += bits_num;

	}


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

	creatsp840();

	change(1, 2, 3, 4, 1);
	change(1, 2, 5, 6, 2);
	change(1, 2, 7, 8, 3);
	change(3, 4, 5, 6, 4);
	change(3, 4, 7, 8, 5);
	change(5, 6, 7, 8, 6);
	change4(1, 2, 3, 4, 5, 6, 7, 8, 7);
	printf("%dT%dR DB0:%f ", M, N, DB0);

	srand((unsigned)time(NULL));
	initial();
	//show();
	for (point = 0; point < POINTNO; point++)
	{
		pnstate = 0xaaaaaaaaL;
		snrdb = DB0 + point * DBSTEP;
		snr = (31 / 8) * pow(10.0, 0.1 * snrdb); //(bitsnum / timenum)
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
		//cout << "跳出while迴圈 biterno比maxerrno多了 已找到足夠多的錯誤(maxerrno) " << endl;
		//record();
		//printf("%5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
	}
	system("pause");
	return 0;
}

