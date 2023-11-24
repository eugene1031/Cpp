// GSM_SP_8Nt2Nr4_1222.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
// GSM_SP_8Nt_1201.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
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

#define  OUTPUTFILE  "STBCSM(M=8 N=2)_ML_2.5db.txt"
#define  OPENSTRING  "#Eb/No(dB) BitErrRate         BitErrSample  TotalSample \n" 
#define  DATASTRING  "%5.2f      %18.16f %7d %11u 1207\n",\
                     (snrdb),(double)biterrno/dsignal,biterrno,dsignal,deviate
#define  DB0		-10.0
#define  DBSTEP		2.0
#define  POINTNO	20
#define  ERRNOSTEP  2
#define  MAXERRNO	2000//2000//1000//500
#define  SAMPLE_NUM 100000//5000000//500000//200000//100000
#define  NORMALIZE  0.70710678118654752440084436210485

#define  M              8
#define  N              4
#define  frame_w        20//35
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define  bits_num       30//28
#define  antenna_number 14//0~13

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



//int interleaved_pattern[105][8];
//int interleaved_pattern2[15][8];

//int antenna_perm2[106][4];

//===================================================================================

//==========================================================================
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

MatrixXcd H(N, M), H_new(N, M), H_test(N, 2), noise(N, M), ABC(M, M), I_row(M, M);
MatrixXcd  Y1(N, 2), Y2(N, 2), Y3(N, 2), Y4(N, 2), y(N, M), NY(N, M);
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
	//天線排列
	//第12時間
	if (*reference_order == 1) {
		H_test.col(0) << (*H).col(0);
		H_test.col(1) << (*H).col(1);

	}
	//第34時間
	if (*reference_order == 2) {
		H_test.col(0) << (*H).col(2);
		H_test.col(1) << (*H).col(3);

	}
	//第56時間
	if (*reference_order == 3) {
		H_test.col(0) << (*H).col(4);
		H_test.col(1) << (*H).col(5);

	}
	//第78時間
	if (*reference_order == 4) {
		H_test.col(0) << (*H).col(6);
		H_test.col(1) << (*H).col(7);

	}
	return H_test;
}


void Y1Y2Y3Y4(int c, int a) {
	if (a == 0) {
		//new define
		Y1.col(0) << y.col(0);
		Y1.col(1) << y.col(1);
		Y2.col(0) << y.col(2);
		Y2.col(1) << y.col(3);
		Y3.col(0) << y.col(4);
		Y3.col(1) << y.col(5);
		Y4.col(0) << y.col(6);
		Y4.col(1) << y.col(7);
	}
}

MatrixXcd ret(int x) {
	MatrixXcd antenna1(2, M); antenna1 << 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0, 0, 0;
	MatrixXcd antenna2(2, M); antenna2 << 0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 0;
	MatrixXcd antenna3(2, M); antenna3 << 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0;
	MatrixXcd antenna4(2, M); antenna4 << 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 1;
	if (x == 1) return antenna1;
	else if (x == 2) return antenna2;
	else if (x == 3) return antenna3;
	else if (x == 4) return antenna4;
}
MatrixXcd generation(int* x, int* x1, int* x2, int* x3, MatrixXcd* I_s1s2, MatrixXcd* I_s3s4, MatrixXcd* I_s5s6, MatrixXcd* I_s7s8) {
	//轉至後位子會跑掉，所以先給一個假的

	MatrixXcd time12(M, 2), time34(M, 2), time56(M, 2), time78(M, 2), final_mat(M, M);

	(*I_s1s2)(0, 1) = -conj((*I_s1s2)(0, 1));
	(*I_s1s2)(1, 0) = -conj((*I_s1s2)(1, 0));
	(*I_s3s4)(0, 1) = -conj((*I_s3s4)(0, 1));
	(*I_s3s4)(1, 0) = -conj((*I_s3s4)(1, 0));
	(*I_s5s6)(0, 1) = -conj((*I_s5s6)(0, 1));
	(*I_s5s6)(1, 0) = -conj((*I_s5s6)(1, 0));
	(*I_s7s8)(0, 1) = -conj((*I_s7s8)(0, 1));
	(*I_s7s8)(1, 0) = -conj((*I_s7s8)(1, 0));

	time12 << ((*I_s1s2) * ret(*x)).transpose();
	time34 << ((*I_s3s4) * ret(*x1)).transpose();
	time56 << ((*I_s5s6) * ret(*x2)).transpose();
	time78 << ((*I_s7s8) * ret(*x3)).transpose();
	//	cout << time12 << endl;
	final_mat << time12, time34, time56, time78;
	return final_mat;
}

void receive()//void frame_length()
{
	//srand((unsigned)time(NULL));

	int input_bits[frame_w][bits_num], decode_bits[frame_w][bits_num], ab_save[frame_w][M], ab_save_temp[frame_w][M], m_save[frame_w], c_save[frame_w], final_save[frame_w]
		, interleaved, reference_order, m, r, g
		, sp, ant;
	double norm_save[16384] = { 0 };
	int debug_savem[16384] = { 0 };
	double norm_output = 0, norm_output1 = 0, norm_output2 = 0, norm_output3 = 0, norm_output4 = 0, min = 9999999
		, temp_min1 = 9999999, temp_min2 = 9999999, temp_min3 = 9999999, temp_min4 = 9999999;

	complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) }, Q[frame_w][M];

	MatrixXcd zero(2, 2), /*Y1(N, 2), Y2(N, 2),*/ I_s1s2(2, 2), I_s3s4(2, 2), I_s5s6(2, 2), I_s7s8(2, 2), I_diag(M, M), I_diag2(M, M), St1(M, M), /*y(N, M),*/ de_STBC(2, 2);

	MatrixXcd st2(M, M), temp(M, 6), save(M, 6);

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
			/*intput_bits[i][j] = (pnstate& M31)>>30;
			pntemp=(pnstate& M3)>>2;
			pntemp^=intput_bits[i][j];
			pnstate=(pnstate<<1)|pntemp;*/

			input_bits[i][j] = rand() % 2;

		}


		//11110010101111 



		 //天線bits是14 [0~13]再傳天線 [14~29] 才再傳QPSK
		//111111101000001001010101011100
		//15695 -> 11110101001111 -> reverse -> 11110010101111

		/*
		input_bits[i][0] = 1;
		input_bits[i][1] = 1;
		input_bits[i][2] = 1;
		input_bits[i][3] = 1;
		input_bits[i][4] = 1;
		input_bits[i][5] = 1;
		input_bits[i][6] = 1;
		input_bits[i][7] = 1;
		input_bits[i][8] = 1;
		input_bits[i][9] = 1;
		input_bits[i][10] = 1;
		input_bits[i][11] = 1;
		input_bits[i][12] = 1;
		input_bits[i][13] = 1;
		*/


		//383


		for (int j = 0; j < M; j++)
		{
			Q[i][j] = qpsk_map[(input_bits[i][j * 2 + 14] << 1)
				+ input_bits[i][j * 2 + (14 + 1)]];

			//Q[i][j] = j;
		}

		min = 999999;
		//	cout<<"----------------------"<<endl;
		I_s1s2 << Q[i][0], -conj(Q[i][1]), Q[i][1], conj(Q[i][0]);
		I_s1s2 << 1 / sqrt(2.0) * I_s1s2;
		I_s3s4 << Q[i][2], -conj(Q[i][3]), Q[i][3], conj(Q[i][2]);
		I_s3s4 << 1 / sqrt(2.0) * I_s3s4;
		I_s5s6 << Q[i][4], -conj(Q[i][5]), Q[i][5], conj(Q[i][4]);
		I_s5s6 << 1 / sqrt(2.0) * I_s5s6;
		I_s7s8 << Q[i][6], -conj(Q[i][7]), Q[i][7], conj(Q[i][6]);
		I_s7s8 << 1 / sqrt(2.0) * I_s7s8;
		//正規化了

		//	cout << I_s1s2 << endl;
		//天線位元排0~13


		/*
		//意義
		m = (input_bits[i][0]) + ((input_bits[i][1])*2)
			+ ((input_bits[i][2])*4) + ((input_bits[i][3])*8)
			+ ((input_bits[i][4])*16) + ((input_bits[i][5])*32)
			+ ((input_bits[i][6])*64) + ((input_bits[i][7])*128)
			+ ((input_bits[i][8])*256) + ((input_bits[i][9])*512)
			+ ((input_bits[i][10])*1024) + ((input_bits[i][11])*2048)
			+ ((input_bits[i][12])*4096) + ((input_bits[i][13])*(8192));
		*/


		m = (input_bits[i][0]) + ((input_bits[i][1]) << 1)
			+ ((input_bits[i][2]) << 2) + ((input_bits[i][3]) << 3)
			+ ((input_bits[i][4]) << 4) + ((input_bits[i][5]) << 5)
			+ ((input_bits[i][6]) << 6) + ((input_bits[i][7]) << 7)
			+ ((input_bits[i][8]) << 8) + ((input_bits[i][9]) << 9)
			+ ((input_bits[i][10]) << 10) + ((input_bits[i][11]) << 11)
			+ ((input_bits[i][12]) << 12) + ((input_bits[i][13]) << 13);


		//debug 印出每個的
		//cout << i << " =i ---------------------------------------------------------------------- " << endl;
		//cout << "m = " << m << "m/24 = " << m/24 << "m%24 = " << m%24 << endl;
		//cout << "send" << endl;
		/*
		for (int j = 0; j < bits_num; j++) {
			cout << input_bits[i][j];
		}
		puts("");
		cout << " ------------------------------------------------------------------------ " << endl;
		*/





		//傳送端 利用m轉成二進制去當成天線位元 前面14bits
		//後面剩下的bits拿來傳QPSK
		//if (m < 16384) { //><歐布
			//cout << "OK:)" << endl;
		sp = m / 24; //交錯
		ant = m % 24;//天線排列

		I_diag << generation(&antenna_perm[ant][0], &antenna_perm[ant][1], &antenna_perm[ant][2], &antenna_perm[ant][3], I_s1s2_ptr, I_s3s4_ptr, I_s5s6_ptr, I_s7s8_ptr);


		//new define
		//交錯樣式在接收端會解回來
		st2.row(0) << I_diag.row(sp840[sp][0] - 1);
		st2.row(1) << I_diag.row(sp840[sp][1] - 1);
		st2.row(2) << I_diag.row(sp840[sp][2] - 1);
		st2.row(3) << I_diag.row(sp840[sp][3] - 1);
		st2.row(4) << I_diag.row(sp840[sp][4] - 1);
		st2.row(5) << I_diag.row(sp840[sp][5] - 1);
		st2.row(6) << I_diag.row(sp840[sp][6] - 1);
		st2.row(7) << I_diag.row(sp840[sp][7] - 1);



		/*

		//檢查原始的傳送矩陣，以及做完交錯的矩陣
		cout << "symbol permutation : " << endl << sp << endl;

			for (int i = 0; i < 8; i++) {
				cout << sp840[sp][i] << "\t";
			}
			cout << endl;
		cout << "antenna perm" << endl << ant << endl;

			for (int i = 0; i < 4; i++) {
				cout << antenna_perm24[ant][i] << "\t";
			}
			cout << endl;
		cout << "I original" << endl << I_diag << endl;
		cout << "st2" << endl << st2 << endl;
		*/
		//}

		//傳送式子
        ABC.setIdentity();
        y = H * st2;
        //   +White_noise(N, M);

        min = 9999999;
		norm_output = 0;
		/*
		//印出檢查 因為在最後面的接收端 印出來的inputbits會與傳送端的不相同 但會完全與decodebits相同
		for (int j = 0; j < bits_num; j++) {
			cout << input_bits[i][j];
		}
		puts("");
		*/


		//接收端 解接收到的訊號回傳送訊號
		for (int mr = 0; mr < 16384; mr++) //0~16383 共16384種
		{
			temp_min1 = 999999;
			temp_min2 = 999999;
			temp_min3 = 999999;
			temp_min4 = 999999;


			interleaved = mr / 24;
			reference_order = mr % 24;
			Y1Y2Y3Y4(interleaved, 0);

			for (int a = 0; a < 4; a++)
			{
				for (int b = 0; b < 4; b++)
				{
					de_STBC << qpsk_map[a], -conj(qpsk_map[b]), qpsk_map[b], conj(qpsk_map[a]);
					de_STBC = 1 / sqrt(2.0) * de_STBC;

                    for (int kk = 0; kk < M; kk++) {
						    I_row.row(kk) << ABC.row(sp840[interleaved][kk] - 1);
					    }
					    H_new = H * I_row;
					if (mr < 16384) {
						//檢測式子 y-Hx 照理來說要等於0時存下來 是一個8*8的矩陣兩行兩行去解分成4個兩行去解
						norm_output1 = (Y1 - channel_estimation(&H_new, &antenna_perm[reference_order][0]) * de_STBC).norm();
						norm_output2 = (Y2 - channel_estimation(&H_new, &antenna_perm[reference_order][1]) * de_STBC).norm();
						norm_output3 = (Y3 - channel_estimation(&H_new, &antenna_perm[reference_order][2]) * de_STBC).norm();
						norm_output4 = (Y4 - channel_estimation(&H_new, &antenna_perm[reference_order][3]) * de_STBC).norm();
					}


					//因此使用for迴圈去找最小值一直到接近0或等於零食存入
					//第12時間最小值
					if (norm_output1 < temp_min1) {
						temp_min1 = norm_output1;
						ab_save_temp[i][0] = a;
						ab_save_temp[i][1] = b;
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
			//cout << "mr = " << mr;
			//cout << "     temp_mini2 = " << temp_min2 << endl;


			//相加之後再去找最小的才會準確!! 找到之後存起來

			if (norm_output < min)
			{
				//cout <<"    mr/15 = "<< mr / 15 ;
				//cout << "   mr%15 = " << mr % 15 ;
				min = norm_output;
				//cout << "   min = " << min << endl;
				m_save[i] = mr;
				//c_save[i] = c;
				ab_save[i][0] = ab_save_temp[i][0];
				ab_save[i][1] = ab_save_temp[i][1];
				ab_save[i][2] = ab_save_temp[i][2];
				ab_save[i][3] = ab_save_temp[i][3];
				ab_save[i][4] = ab_save_temp[i][4];
				ab_save[i][5] = ab_save_temp[i][5];
				ab_save[i][6] = ab_save_temp[i][6];
				ab_save[i][7] = ab_save_temp[i][7];
				//norm_save[m_save[i]] = min;//不可以寫在這邊!!!!因為會覆蓋到別人的值


			}


		}//mr的for
		//cout << "min = " << min << endl;
		norm_save[m_save[i]] = min; //因為有錯所以將Norm值存在對應的mr的矩陣內中 ex第mr格的norm_save矩陣匯存那個mr解出來的norm值


	}
	/*
	m = 15695;
	for (int i = 0; i < frame_w; i++)
	{
		if (m == 15695) {
			cout << "send2" << endl;
			for (int j = 0; j < bits_num; j++) {
				cout << input_bits[i][j];
			}
			puts("");

		}
	}
	*/





	//將收到的東西還原成傳送的樣子
	for (int i = 0; i < frame_w; i++)
	{

		//cout << "mr = " << m_save[i] << endl;
		//	cout << "m=" <<m_save[i]<<endl;
		//天線位元共有14個 (0~13)


		//msave裡的10進制轉成2進制重新放回decodebits中 最右邊是最高位元跟一般寫字相反 >>右移是除
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


		/*
		//印出輸入的矩陣 inputbits以及換算成 m 再印出 msave解道的 以及還原成 decodebits
		cout << endl;
		cout << " i ";
		for (int y = 0; y < 14; y++) {
			cout << input_bits[i][y] << "\t";
		}
		cout << "m=" << m << endl;
		cout << endl;
		cout << "msave=" << m_save[i]<< endl;
		cout << " d ";
		for (int y = 0; y < 14; y++) {
			cout << decode_bits[i][y] << "\t";
		}
		cout << endl;

		*/
		//m_save裡面存mr=0~16382
		//14是天線位元 0~13
		/*
		for (int k = 0; k < 14; k++) {
			decode_bits[i][k] = ((m_save[i] >> k) % 2);
		}

		*/



		//解後面的QPSK
		for (int k = 0; k < M; k++) {
			decode_bits[i][k * 2 + 14] = (ab_save[i][k] >> 1) % 2; //14是天線位元
			decode_bits[i][k * 2 + 15] = (ab_save[i][k]) % 2;		//15是天線位元+1
		}
		//解天線
		/*
		//放到Frame迴圈外面試試看
		for (int j = 0; j < bits_num; j++)
		{
			//cout << decode_bits[i][j] << " " ;
			//比較接收端還原出來的decodebits跟傳送端的input_bits
			if (decode_bits[i][j] != input_bits[i][j]) {
				biterrno++;

			}
		}
		dsignal += bits_num;
		*/
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
	//學長印出傳送接收的code
	/*
	//找到norm不等於零的再印出
	for (int i = 0; i < frame_w; i++)
	{

		//只多了這邊的判斷式
		if (norm_save[m_save[i]] != 0)
		{
			cout << "======================================== frame = " << i << endl;
			cout << " m = " << (input_bits[i][0]) + ((input_bits[i][1]) << 1)
				+ ((input_bits[i][2]) << 2) + ((input_bits[i][3]) << 3)
				+ ((input_bits[i][4]) << 4) + ((input_bits[i][5]) << 5)
				+ ((input_bits[i][6]) << 6) + ((input_bits[i][7]) << 7)
				+ ((input_bits[i][8]) << 8) + ((input_bits[i][9]) << 9)
				+ ((input_bits[i][10]) << 10) + ((input_bits[i][11]) << 11)
				+ ((input_bits[i][12]) << 12) + ((input_bits[i][13]) << 13) << endl;
			//cout << "*********************************************************" << endl;
			//cout << " m  = " << debug_savem[m_save[i]] << "  m/24 = " << debug_savem[m_save[i]] / 24 << "  m%24 = " << debug_savem[m_save[i]] % 24 << endl;
			cout << " mr = " << m_save[i] << "  mr/24 =  " << m_save[i] / 24 << "  mr%24 =  " << m_save[i] % 24 << " norm = " << norm_save[m_save[i]] << endl;

			for (int j = 0; j < bits_num; j++)
			{
				//			cout << decode_bits[i][j] << " " ;
				cout << input_bits[i][j];

			}
			puts("");
			for (int j = 0; j < bits_num; j++)
			{
				//			cout << decode_bits[i][j] << " " ;
				cout << decode_bits[i][j];

			}
			puts("");
			puts("");
		}

	}
	*/
	
	/*
	for (int a; a < 16383; a++) {
		if (norm_save[a] != 0)
		{
			cout << " m = " << debug_savem[a] << endl;
			cout << "  mr/24 =  " << a / 24 << "  mr%24 =  " << a % 24 << "  ->  " << a << " = " << norm_save[a] << endl;


		}

	}
	*/
	/*
		for (int i = 0; i < frame_w; i++)
	{
		if (norm_save[m_save[i]] != 0)
		{
			cout << " m  = " << debug_savem[m_save[i]] << "  m/24 = " << debug_savem[m_save[i]] / 24 << "  m%24 = "<< debug_savem[m_save[i]] % 24 << endl;
			cout << " mr = " << m_save[i] <<"  mr/24 =  " << m_save[i] / 24 << "  mr%24 =  " << m_save[i] % 24 << " norm = " << norm_save[m_save[i]] << endl;

			for (int j = 0; j < bits_num; j++) {
				cout << input_bits[i][j];
			}
			puts("");
			for (int j = 0; j < bits_num; j++) {
				cout << decode_bits[i][j];
			}
			puts("");
			puts("");
			}

	}

	*/



	/*
	for (int i = 0; i < frame_w; i++) {
		for (int j = 0; j < bits_num; j++) {
			cout << input_bits[i][j];
		}
		puts("");
		for (int j = 0; j < bits_num; j++) {
			cout << decode_bits[i][j];
		}
		puts("");
		puts("");
	}
	*/



	/*
	for (int i = 0; i < frame_w; i++)
	{

		for (int a; a < 16383; a++)
		{
			if (norm_save[a] != 0)
			{
				cout << " m = " << debug_savem[a] << endl;
				cout << "  mr/24 =  " << a / 24 << "  mr%24 =  " << a % 24 << "  ->  " << a << " = " << norm_save[a] << endl;

				for (int j = 0; j < bits_num; j++) {
					cout << "ip:" << input_bits[i][j];
				}
				puts("");
				for (int j = 0; j < bits_num; j++) {
					cout << "op:" << decode_bits[i][j];
				}
				puts("");
				puts("");
			}
		}
	}
	*/
	//cout << "symbol permutation : " << endl << sp << endl;
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

	//srand((unsigned)time(NULL));
	initial();
	//show();
	for (point = 0; point < POINTNO; point++)
	{
		pnstate = 0xaaaaaaaaL;
		snrdb = DB0 + point * DBSTEP;
		snr = (3.75) * pow(10.0, 0.1 * snrdb); //(bitsnum / timenum)
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
		system("pause");
		//cout << "跳出while迴圈 biterno比maxerrno多了 已找到足夠多的錯誤(maxerrno) " << endl;
		//record();
		//printf("%5.2f   %18.16f   %7d   %11u \n", (snrdb), (double)biterrno / dsignal, biterrno, dsignal, deviate);
	}
	system("pause");
	return 0;
}
