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

#define  OUTPUTFILE  "stbcsmspap8Tx.txt"
#define  OUTPUTFILEA  "bbb.txt"
#define  OUTPUTFILED "stbcsmspap8txdebug.txt"

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

#define  M              8
#define  N              4
#define  frame_w        1
#define  sqrt2          0.70710678118654752440084436210485
#define  pi			    3.14159265359
#define	 bits_num		21
#define  inter	        2
#define  codeword	    2097152 // 2^21

using namespace std;
using namespace Eigen;


#define  M3  0x00000004L
#define  M31 0x40000000L
#define  eulen(r1,i1,r2,i2) ((r1)-(r2))*((r1)-(r2))+((i1)-(i2))*((i1)-(i2))

FILE* fp;
FILE* fpa;
FILE* fpDebug;
static char  filename[80] = OUTPUTFILE;
static char  filenamea[80] = OUTPUTFILEA;
static char fpDebugFilename[80] = OUTPUTFILED;

static int	 v[2] = { -1, 1 };
int	 point, biterrno, errlevel, u, t, de_info, dsignal, samp;
long    pnstate, pntemp;
double   snrdb, snr, deviate, dmin;
int antenna_perm[24][4] = { 1,2,3,4,	1,2,4,3,	1,3,2,4,	1,3,4,2,	1,4,2,3,	1,4,3,2,
								2,1,3,4,	2,1,4,3,	2,3,1,4,	2,3,4,1,	2,4,1,3,	2,4,3,1,
								3,1,2,4,	3,1,4,2,	3,2,1,4,	3,2,4,1,	3,4,1,2,	3,4,2,1,
								4,1,2,3,	4,1,3,2,	4,2,1,3,	4,2,3,1,	4,3,1,2,	4,3,2,1,
};
int interleaved_pattern[105][8] = {
	{1, 2, 3, 4, 5, 6, 7, 8}, {1, 2, 3, 4, 5, 8, 6, 7}, {1, 2, 3, 4, 5, 7, 8, 6}, {1, 2, 3, 5, 4, 7, 6, 8}, {1, 2, 3, 5, 4, 8, 7, 6}, {1, 2, 3, 5, 4, 6, 8, 7}, {1, 2, 3, 6, 4, 5, 7, 8}, {1, 2, 3, 6, 4, 8, 5, 7}, {1, 2, 3, 6, 4, 7, 8, 5}, {1, 2, 3, 7, 4, 6, 5, 8}, {1, 2, 3, 7, 4, 8, 6, 5}, {1, 2, 3, 7, 4, 5, 8, 6}, {1, 2, 3, 8, 4, 5, 6, 7}, {1, 2, 3, 8, 4, 7, 5, 6}, {1, 2, 3, 8, 4, 6, 7, 5}, // 15

	{1, 3, 2, 4, 5, 7, 6, 8},
	{1, 3, 2, 4, 5, 8, 7, 6},
	{1, 3, 2, 4, 5, 6, 8, 7},
	{1, 3, 2, 5, 4, 6, 7, 8},
	{1, 3, 2, 5, 4, 8, 6, 7},
	{1, 3, 2, 5, 4, 7, 8, 6},
	{1, 3, 2, 6, 4, 7, 5, 8},
	{1, 3, 2, 6, 4, 8, 7, 5},
	{1, 3, 2, 6, 4, 5, 8, 7},
	{1, 3, 2, 7, 4, 5, 6, 8},
	{1, 3, 2, 7, 4, 8, 5, 6},
	{1, 3, 2, 7, 4, 6, 8, 5},
	{1, 3, 2, 8, 4, 6, 5, 7},
	{1, 3, 2, 8, 4, 7, 6, 5},
	{1, 3, 2, 8, 4, 5, 7, 6}, // 30

	{1, 4, 2, 3, 5, 6, 7, 8},
	{1, 4, 2, 3, 5, 8, 6, 7},
	{1, 4, 2, 3, 5, 7, 8, 6},
	{1, 4, 2, 5, 3, 7, 6, 8},
	{1, 4, 2, 5, 3, 8, 7, 6},
	{1, 4, 2, 5, 3, 6, 8, 7},
	{1, 4, 2, 6, 3, 5, 7, 8},
	{1, 4, 2, 6, 3, 8, 5, 7},
	{1, 4, 2, 6, 3, 7, 8, 5},
	{1, 4, 2, 7, 3, 6, 5, 8},
	{1, 4, 2, 7, 3, 8, 6, 5},
	{1, 4, 2, 7, 3, 5, 8, 6},
	{1, 4, 2, 8, 3, 5, 6, 7},
	{1, 4, 2, 8, 3, 7, 5, 6},
	{1, 4, 2, 8, 3, 6, 7, 5}, // 45

	{1, 5, 2, 3, 4, 7, 6, 8},
	{1, 5, 2, 3, 4, 8, 7, 6},
	{1, 5, 2, 3, 4, 6, 8, 7}, // 48
	{1, 5, 2, 4, 3, 6, 7, 8},
	{1, 5, 2, 4, 3, 8, 6, 7},
	{1, 5, 2, 4, 3, 7, 8, 6}, // 51
	{1, 5, 2, 6, 3, 7, 4, 8},
	{1, 5, 2, 6, 3, 8, 7, 4},
	{1, 5, 2, 6, 3, 4, 8, 7}, // 54    53:17283465->15263874
	{1, 5, 2, 7, 3, 4, 6, 8},
	{1, 5, 2, 7, 3, 8, 4, 6},
	{1, 5, 2, 7, 3, 6, 8, 4}, //      57:18234567->15273684
	{1, 5, 2, 8, 3, 6, 4, 7},
	{1, 5, 2, 8, 3, 7, 6, 4},
	{1, 5, 2, 8, 3, 4, 7, 6}, // 60    59:18234675->15283764

	{1, 6, 2, 3, 4, 5, 7, 8},
	{1, 6, 2, 3, 4, 8, 5, 7},
	{1, 6, 2, 3, 4, 7, 8, 5}, // 63
	{1, 6, 2, 4, 3, 7, 5, 8},
	{1, 6, 2, 4, 3, 8, 7, 5},
	{1, 6, 2, 4, 3, 5, 8, 7}, // 66
	{1, 6, 2, 5, 3, 4, 7, 8},
	{1, 6, 2, 5, 3, 8, 4, 7},
	{1, 6, 2, 5, 3, 7, 8, 4}, // 69    69:18234756->16253784
	{1, 6, 2, 7, 3, 5, 4, 8},
	{1, 6, 2, 7, 3, 8, 5, 4},
	{1, 6, 2, 7, 3, 4, 8, 5}, // 72    71:18243576->16273854
	{1, 6, 2, 8, 3, 4, 5, 7},
	{1, 6, 2, 8, 3, 7, 4, 5},
	{1, 6, 2, 8, 3, 5, 7, 4}, // 75

	{1, 7, 2, 3, 4, 6, 5, 8},
	{1, 7, 2, 3, 4, 8, 6, 5},
	{1, 7, 2, 3, 4, 5, 8, 6}, // 78
	{1, 7, 2, 4, 3, 5, 6, 8},
	{1, 7, 2, 4, 3, 8, 5, 6},
	{1, 7, 2, 4, 3, 6, 8, 5}, // 81
	{1, 7, 2, 5, 3, 6, 4, 8},
	{1, 7, 2, 5, 3, 8, 6, 4},
	{1, 7, 2, 5, 3, 4, 8, 6}, // 84    83:18243765->17253864
	{1, 7, 2, 6, 3, 4, 5, 8},
	{1, 7, 2, 6, 3, 8, 4, 5},
	{1, 7, 2, 6, 3, 5, 8, 4}, // 87    86:18253467->17263845,   87:18273564->17263584
	{1, 7, 2, 8, 3, 4, 6, 5},
	{1, 7, 2, 8, 3, 5, 4, 6},
	{1, 7, 2, 8, 3, 6, 5, 4}, // 90    88:17283465->13856472

	{1, 8, 2, 3, 4, 5, 6, 7},
	{1, 8, 2, 3, 4, 6, 7, 5},
	{1, 8, 2, 3, 4, 7, 5, 6}, // 91:18234567->13758462, 92:18234675->13657428,	93:18234756->13586274
	{1, 8, 2, 4, 3, 6, 5, 7},
	{1, 8, 2, 4, 3, 5, 7, 6},
	{1, 8, 2, 4, 3, 7, 6, 5}, // 95:18243576->13578264, 96:18243765->13567284
	{1, 8, 2, 5, 3, 4, 6, 7},
	{1, 8, 2, 5, 3, 6, 7, 4},
	{1, 8, 2, 5, 3, 7, 4, 6}, // 97:18253467->13546782
	{1, 8, 2, 6, 4, 3, 5, 7},
	{1, 8, 2, 6, 4, 5, 7, 3},
	{1, 8, 2, 6, 4, 7, 3, 5},
	{1, 8, 2, 7, 3, 4, 5, 6},
	{1, 8, 2, 7, 3, 5, 6, 4},
	{1, 8, 2, 7, 3, 6, 4, 5} // 104:18273564->13547268
};
int sp105[105][8] = {
	1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 7, 8, 6, 1, 2, 3, 4, 5, 8, 6, 7, // 1row 3
	1, 2, 3, 5, 4, 6, 8, 7, 1, 2, 3, 5, 4, 8, 7, 6, 1, 2, 3, 5, 4, 7, 6, 8, // 2row
	1, 2, 3, 5, 6, 4, 7, 8, 1, 2, 3, 5, 6, 7, 8, 4, 1, 2, 3, 5, 6, 8, 4, 7, //
	1, 2, 3, 5, 7, 4, 8, 6, 1, 2, 3, 5, 7, 8, 6, 4, 1, 2, 3, 5, 7, 6, 4, 8, //
	1, 2, 3, 5, 8, 4, 6, 7, 1, 2, 3, 5, 8, 6, 7, 4, 1, 2, 3, 5, 8, 7, 4, 6, // 5row  15

	1, 3, 2, 4, 5, 7, 6, 8, 1, 3, 2, 4, 5, 6, 8, 7, 1, 3, 2, 4, 5, 8, 7, 6, // 6row
	1, 3, 2, 5, 4, 6, 7, 8, 1, 3, 2, 5, 4, 7, 8, 6, 1, 3, 2, 5, 4, 8, 6, 7, //
	1, 3, 2, 5, 6, 4, 8, 7, 1, 3, 2, 5, 6, 8, 7, 4, 1, 3, 2, 5, 6, 7, 4, 8, //
	1, 3, 2, 5, 7, 4, 6, 8, 1, 3, 2, 5, 7, 6, 8, 4, 1, 3, 2, 5, 7, 8, 4, 6, //
	1, 3, 2, 5, 8, 4, 7, 6, 1, 3, 2, 5, 8, 7, 6, 4, 1, 3, 2, 5, 8, 6, 4, 7, // 10row

	1, 3, 4, 2, 5, 6, 7, 8, 1, 3, 4, 2, 5, 7, 8, 6, 1, 3, 4, 2, 5, 8, 6, 7, // 11row
	1, 3, 5, 2, 4, 6, 8, 7, 1, 3, 5, 2, 4, 8, 7, 6, 1, 3, 5, 2, 4, 7, 6, 8, //
	1, 3, 5, 2, 6, 4, 7, 8, 1, 3, 5, 2, 6, 7, 8, 4, 1, 3, 5, 2, 6, 8, 4, 7, //
	1, 3, 5, 2, 7, 4, 8, 6, 1, 3, 5, 2, 7, 8, 6, 4, 1, 3, 5, 2, 7, 6, 4, 8, //
	1, 3, 5, 2, 8, 4, 6, 7, 1, 3, 5, 2, 8, 6, 7, 4, 1, 3, 5, 2, 8, 7, 4, 6, // 15row

	1, 3, 4, 5, 2, 6, 8, 7, 1, 3, 4, 5, 2, 8, 7, 6, 1, 3, 4, 5, 2, 7, 6, 8, // 16row
	1, 3, 5, 4, 2, 6, 7, 8, 1, 3, 5, 4, 2, 7, 8, 6, 1, 3, 5, 4, 2, 8, 6, 7, //
	1, 3, 5, 6, 2, 4, 8, 7, 1, 3, 5, 6, 2, 8, 7, 4, 1, 3, 5, 6, 2, 7, 4, 8, //
	1, 3, 5, 7, 2, 4, 6, 8, 1, 3, 5, 7, 2, 6, 8, 4, 1, 3, 5, 7, 2, 8, 4, 6, //
	1, 3, 5, 8, 2, 4, 7, 6, 1, 3, 5, 8, 2, 7, 6, 4, 1, 3, 5, 8, 2, 6, 4, 7, // 20row

	1, 3, 4, 5, 6, 2, 7, 8, 1, 3, 4, 5, 6, 7, 8, 2, 1, 3, 4, 5, 6, 8, 2, 7, // 21row
	1, 3, 4, 5, 7, 2, 8, 6, 1, 3, 4, 5, 7, 8, 6, 2, 1, 3, 4, 5, 7, 6, 2, 8, //
	1, 3, 4, 5, 8, 2, 6, 7, 1, 3, 4, 5, 8, 6, 7, 2, 1, 3, 4, 5, 8, 7, 2, 6, //
	1, 3, 5, 4, 6, 2, 8, 7, 1, 3, 5, 4, 6, 8, 7, 2, 1, 3, 5, 4, 6, 7, 2, 8, //
	1, 3, 5, 4, 7, 2, 6, 8, 1, 3, 5, 4, 7, 6, 8, 2, 1, 3, 5, 4, 7, 8, 2, 6, // 25row

	1, 3, 5, 4, 8, 2, 7, 6, 1, 3, 5, 4, 8, 7, 6, 2, 1, 3, 5, 4, 8, 6, 2, 7, // 26row
	1, 3, 5, 6, 4, 2, 7, 8, 1, 3, 5, 6, 4, 7, 8, 2, 1, 3, 5, 6, 4, 8, 2, 7, //
	1, 3, 5, 6, 7, 2, 8, 4, 1, 3, 5, 6, 7, 8, 4, 2, 1, 3, 5, 6, 7, 4, 2, 8, //
	1, 3, 5, 6, 8, 2, 4, 7, 1, 3, 5, 6, 8, 4, 7, 2, 1, 3, 5, 6, 8, 7, 2, 4, //
	1, 3, 5, 7, 4, 2, 8, 6, 1, 3, 5, 7, 4, 8, 6, 2, 1, 3, 5, 7, 4, 6, 2, 8, // 30row

	1, 3, 5, 7, 6, 2, 4, 8, 1, 3, 5, 7, 6, 4, 8, 2, 1, 3, 5, 7, 6, 8, 2, 4, // 31row
	1, 3, 5, 7, 8, 2, 6, 4, 1, 3, 5, 7, 8, 6, 4, 2, 1, 3, 5, 7, 8, 4, 2, 6, //
	1, 3, 5, 8, 4, 2, 6, 7, 1, 3, 5, 8, 4, 6, 7, 2, 1, 3, 5, 8, 4, 7, 2, 6, //
	1, 3, 5, 8, 6, 2, 7, 4, 1, 3, 5, 8, 6, 7, 4, 2, 1, 3, 5, 8, 6, 4, 2, 7, //
	1, 3, 5, 8, 7, 2, 4, 6, 1, 3, 5, 8, 7, 4, 6, 2, 1, 3, 5, 8, 7, 6, 2, 4, // 35row 105種
};
int sp840[840][8] = { 0 };

void creatsp840()
{
	// 將sp105個資料放8個給sp840
	for (int x = 0; x < 8; x++)
	{
		for (int row = 0; row < 105; row++)
		{
			for (int col = 0; col < 8; col++)
			{
				sp840[105 * x + row][col] = sp105[row][col];
			}
		}
	}
}
void show()
{
	int r = 840;
	int c = 8;
	// 印出symbol permutaion矩陣
	for (int row = 0; row < r; row++)
	{
		for (int i = 0; i < c; i++)
		{
			cout << sp840[row][i] << "\t";
		}
		cout << endl;
	}
}

void change(int a, int b, int c, int d, int th)
{
	// 如果th=1 for (int i = 105*1; i < 105 * 2 - 1; i++)
	// th=2  for (int i = 105*2; i < 105 * 3 - 1; i++)
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
	// 如果th=1 for (int i = 105*1; i < 105 * 2 - 1; i++)
	// th=2  for (int i = 105*2; i < 105 * 3 - 1; i++)
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


MatrixXcd X1(2, 2);
MatrixXcd X2(2, 2);
MatrixXcd X3(2, 2);
MatrixXcd Xp1(2, 2);
MatrixXcd Xp2(2, 2);
MatrixXcd Xp3(2, 2);


complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(0,-1),complex<double>(-1,0) };

MatrixXcd I_s1s2(2, 2);
MatrixXcd I_s3s4(2, 2);
MatrixXcd I_s5s6(2, 2);
MatrixXcd I_s7s8(2, 2);
MatrixXcd Q_s1s2(2, 2);
MatrixXcd Q_s3s4(2, 2);
MatrixXcd Q_s5s6(2, 2);
MatrixXcd Q_s7s8(2, 2);

MatrixXcd r_matrix(M, M), Dij_matrix(M, M);
MatrixXcd Q_diag(M, M), Q_diag1(M, M);
MatrixXcd I_diag(M, M), I_diag1(M, M);
//DiagonalMatrix<complex<double>,M> I_diag;
MatrixXcd St0(2 * M, M);
MatrixXcd St1(2 * M, M);
MatrixXcd S0(M, M);
MatrixXcd S1(M, M);
MatrixXcd Xt(M, M);
MatrixXcd Xt_1(M, M);
MatrixXcd Xtp(M, M);
MatrixXcd Xt_1p(M, M);
MatrixXcd X_I(M, M), X_Q(M, M);
MatrixXcd A(M, M);
//MatrixXcd G(M,M);
VectorXcd SingularValues(M), eigenvalue(M);

complex<double> s[2][M];

int gt1 = 0;
int gt2 = 0;
int g1 = 0;
int g2 = 0;

int m = 0;
int r1 = 0;
int r2 = 0;
int q11 = 0;
int q12 = 0;
int q13 = 0;
int q14 = 0;
int q21 = 0;
int q22 = 0;
int q23 = 0;
int q24 = 0;

int aaa = 0;

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
int find3And4Zero(int x, int y, double a, double b, double c, double d, double e, double f, double g, double h) {
	int count = 0;
	if (fabs(a) < 1e-6)
		count = count + 1;
	if (fabs(b) < 1e-6)
		count = count + 1;
	if (fabs(c) < 1e-6)
		count = count + 1;
	if (fabs(d) < 1e-6)
		count = count + 1;
	if (fabs(e) < 1e-6)
		count = count + 1;
	if (fabs(f) < 1e-6)
		count = count + 1;
	if (fabs(g) < 1e-6)
		count = count + 1;
	if (fabs(h) < 1e-6)
		count = count + 1;
	return count;
}

MatrixXcd ret(int x)
{
	MatrixXcd antenna1(2, M);
	antenna1 << 1, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0, 0, 0;
	MatrixXcd antenna2(2, M);
	antenna2 << 0, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 0;
	MatrixXcd antenna3(2, M);
	antenna3 << 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0;
	MatrixXcd antenna4(2, M);
	antenna4 << 0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 1;
	if (x == 1)
		return antenna1;
	else if (x == 2)
		return antenna2;
	else if (x == 3)
		return antenna3;
	else if (x == 4)
		return antenna4;
}


MatrixXcd generation(int* x, int* x1, int* x2, int* x3, MatrixXcd* I_s1s2, MatrixXcd* I_s3s4, MatrixXcd* I_s5s6, MatrixXcd* I_s7s8)
{
	// 轉至後位子會跑掉，所以先給一個假的

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

MatrixXcd* I_s1s2_ptr = &I_s1s2;
MatrixXcd* I_s3s4_ptr = &I_s3s4;
MatrixXcd* I_s5s6_ptr = &I_s5s6;
MatrixXcd* I_s7s8_ptr = &I_s7s8;
MatrixXcd* Q_s1s2_ptr = &Q_s1s2;
MatrixXcd* Q_s3s4_ptr = &Q_s3s4;
MatrixXcd* Q_s5s6_ptr = &Q_s5s6;
MatrixXcd* Q_s7s8_ptr = &Q_s7s8;

void TxRx()
{
	//輸出.txt要加的!!
	fp = fopen(filename, "a");
	fpa = fopen(filenamea, "a");
	fpDebug = fopen(fpDebugFilename, "a");

	MatrixXcd zero(2, 2);
	zero << 0, 0,
		0, 0;

	MatrixXcd Xt0(M, M);
	MatrixXcd Xt1(M, M);
	int m_a, r_a, col_a, row_a;
	int m_b, r_b, col_b, row_b;
	int two = 0, four = 0;
	for (int a = 0; a < 1; a++) {  //for (int a = 0; a < codeword - 1; a++) {

		for (int b = 1; b < codeword; b++) { //for (int b = a + 1; b < codeword; b++) { //for (int b = 1; b < codeword; b++) {

			//if (b != a) {

			s[0][0] = qpsk_map[0];
			s[0][1] = qpsk_map[0];

			s[0][2] = qpsk_map[0];
			s[0][3] = qpsk_map[0];

			s[0][4] = qpsk_map[0];
			s[0][5] = qpsk_map[0];

			s[0][6] = qpsk_map[0];
			s[0][7] = qpsk_map[0];

			//2048=2^11
			s[1][0] = qpsk_map[0];
			s[1][1] = qpsk_map[0];
			  
			s[1][2] = qpsk_map[0];
			s[1][3] = qpsk_map[0];
			  
			s[1][4] = qpsk_map[0];
			s[1][5] = qpsk_map[0];
			  
			s[1][6] = qpsk_map[0];
			s[1][7] = qpsk_map[0];


			I_s1s2 << s[0][0], -conj(s[0][1]), s[0][1], conj(s[0][0]);
			I_s3s4 << s[0][2], -conj(s[0][3]), s[0][3], conj(s[0][2]);
			I_s5s6 << s[0][4], -conj(s[0][5]), s[0][5], conj(s[0][4]);
			I_s7s8 << s[0][6], -conj(s[0][7]), s[0][7], conj(s[0][6]);

			/*I_s1s2 << 1 / sqrt(2.0) * I_s1s2;
			I_s3s4 << 1 / sqrt(2.0) * I_s3s4;
			I_s5s6 << 1 / sqrt(2.0) * I_s5s6;
			*/
			//cout <<I_s1s2<<endl;
			//cout << I_s3s4 << endl << endl;

			Q_s1s2 << s[1][0], -conj(s[1][1]), s[1][1], conj(s[1][0]);
			Q_s3s4 << s[1][2], -conj(s[1][3]), s[1][3], conj(s[1][2]);
			Q_s5s6 << s[1][4], -conj(s[1][5]), s[1][5], conj(s[1][4]);
			Q_s7s8 << s[1][6], -conj(s[1][7]), s[1][7], conj(s[1][6]);

			/*Q_s1s2 << 1 / sqrt(2.0) * Q_s1s2;
			Q_s3s4 << 1 / sqrt(2.0) * Q_s3s4;
			Q_s5s6 << 1 / sqrt(2.0) * Q_s5s6;*/
			//cout << Q_s1s2 << endl;
			//cout << Q_s3s4 << endl;

			//cout << "constant      : " << ((a / 256) % 2) << ((a / 128) % 2) << ((a / 64) % 2) << ((a / 32) % 2) << ((a / 16) % 2) << ((a / 8) % 2) << ((a / 4) % 2) << ((a / 2) % 2) << ((a) % 2) << endl;
			//cout << "512_codeword  : " << ((b / 256) % 2) << ((b / 128) % 2) << ((b / 64) % 2) << ((b / 32) % 2) << ((b / 16) % 2) << ((b / 8) % 2) << ((b / 4) % 2) << ((b / 2) % 2) << ((b) % 2) << endl;

			//cout << "是第幾個codeword : " << b << endl;		

			m_a = 0;


			r_a = m_a % 24;  //天線排列又稱reference order
			///interleaved
			col_a = m_a / 24 % 840;
			row_a = m_a / 24 / 840;
			I_diag << generation(&antenna_perm[r_a][0], &antenna_perm[r_a][1], &antenna_perm[r_a][2], &antenna_perm[r_a][3], I_s1s2_ptr, I_s3s4_ptr, I_s5s6_ptr, I_s7s8_ptr);

			//注意不能用這樣子的 會錯m_b = 128 * ((b / ((2^19))) % 2) +64 * ((b / ((2 ^ 18))) % 2)
			//1111100000000000000000000 = 32505856
			//100000000000000000000 = 1048576

			//original mb
			/*m_b = 1048576 * ((b / 524288) % 2)
				+ 524288 * ((b / 262144) % 2)
				+ 262144 * ((b / 131072) % 2)
				+ 131072 * ((b / 65536) % 2)
				+ 65536 * ((b / 32768) % 2)
				+ 32768 * ((b / 16384) % 2)
				+ 16384 * ((b / 8192) % 2)
				+ 8192 * ((b / 65536) % 2)
				+ 4096 * ((b / 4096) % 2)
				+ 2048 * ((b / 2048) % 2)
				+ 1024 * ((b / 1024) % 2)
				+ 512 * ((b / 512) % 2)
				+ 256 * ((b / 256) % 2)
				+ 128 * ((b / 128) % 2)
				+ 64 * ((b / 64) % 2)
				+ 32 * ((b / 32) % 2)
				+ 16 * ((b / 16) % 2)
				+ 8 * ((b / 8) % 2)
				+ 4 * ((b / 4) % 2)
				+ 2 * ((b / 2) % 2)
				+ ((b / 1) % 2);*/
			//cout << "mb" << m_b << endl;

			r_b = b % 24;  //天線排列又稱reference order
			//interleaved
			col_b = b / 24 % 840;
			row_b = b / 24 / 840;

			Q_diag << generation(&antenna_perm[r_b][0], &antenna_perm[r_b][1], &antenna_perm[r_b][2], &antenna_perm[r_b][3], Q_s1s2_ptr, Q_s3s4_ptr, Q_s5s6_ptr, Q_s7s8_ptr);

			//new define
			for (int kk = 0; kk < 8; kk++) {
				I_diag1.col(kk) << I_diag.col(sp840[col_a][kk] - 1);
			}
			for (int kk = 0; kk < 8; kk++) {
				X_I.row(kk) << I_diag1.row(sp105[row_a][kk] - 1);
			}
			
			//new define
			for (int kk = 0; kk < 8; kk++) {
				Q_diag1.col(kk) << Q_diag.col(sp840[col_b][kk] - 1);
			}
			for (int kk = 0; kk < 8; kk++) {
				X_Q.row(kk) << Q_diag1.row(sp105[row_b][kk] - 1);
			}
			/*cout << Q_diag << endl;
			system("pause");
			cout << X_Q << endl;
			system("pause");*/
			r_matrix << (X_I - X_Q);
			Dij_matrix << (r_matrix) * (r_matrix.adjoint());

			//eigenvalue << Dij_matrix.eigenvalues();
			//cout << "eigenvalue"  << endl;
			//cout << eigenvalue << endl<<endl;

			//real_eigenvalue << eigenvalue.real();

			//cout << (eigenvalue.real())(0) << endl << endl;
			//eigenvalue << ((Dij_matrix.eigenvalues()).real())(0);

			/*cout << "算出來的特徵值: ";
			for (int k = 0; k < M; k++)
				cout << " " << ((Dij_matrix.eigenvalues()).real())(k);;
			puts("");
			*/
			//hammingDistance(a, b);
			//if(b < 1048576)
			if (find3And4Zero(a, b, ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), ((Dij_matrix.eigenvalues()).real())(4), ((Dij_matrix.eigenvalues()).real())(5), ((Dij_matrix.eigenvalues()).real())(6), ((Dij_matrix.eigenvalues()).real())(7)) == 6)
			{
				fprintf(fp, "%d    %.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10d%10d\n", hammingDistance(a, b), abs(((Dij_matrix.eigenvalues()).real())(0)), abs(((Dij_matrix.eigenvalues()).real())(1)), abs(((Dij_matrix.eigenvalues()).real())(2)), abs(((Dij_matrix.eigenvalues()).real())(3)), abs(((Dij_matrix.eigenvalues()).real())(4)), abs(((Dij_matrix.eigenvalues()).real())(5)), ((Dij_matrix.eigenvalues()).real())(6), ((Dij_matrix.eigenvalues()).real())(7), (((Dij_matrix.eigenvalues()).real())(6) * ((Dij_matrix.eigenvalues()).real())(7)), a, b);
			}
			//fprintf(fp, "%d    %.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10d%10d\n", hammingDistance(a, b), ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), ((Dij_matrix.eigenvalues()).real())(4), ((Dij_matrix.eigenvalues()).real())(5), a, b);
			//else
			//	fprintf(fpa, "%d    %.5f%10.5f%10.5f%10.5f%10.5f%10.5f%10d%10d\n", hammingDistance(a, b), ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), ((Dij_matrix.eigenvalues()).real())(4), ((Dij_matrix.eigenvalues()).real())(5), a, b);
			cout << " a = " << a << " b = " << b << endl;
			//fprintf(fp, "%d\n", hammingDistance(a, b));

			//Debug find zero numbers = 5 or 6
			/*if (find3And4Zero(a, b, ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), ((Dij_matrix.eigenvalues()).real())(4), ((Dij_matrix.eigenvalues()).real())(5)) > 4)
			{
				cout << "wrong" << endl;
				fprintf(fpDebug, "%d    %.5f%10.5f%10.5f%10.5f%10.5f%10.5f  (%d,%d)\n", hammingDistance(a, b), ((Dij_matrix.eigenvalues()).real())(0), ((Dij_matrix.eigenvalues()).real())(1), ((Dij_matrix.eigenvalues()).real())(2), ((Dij_matrix.eigenvalues()).real())(3), ((Dij_matrix.eigenvalues()).real())(4), ((Dij_matrix.eigenvalues()).real())(5), a, b);
			}*/

			if (((Dij_matrix.eigenvalues()).real())(0) == 0 && ((Dij_matrix.eigenvalues()).real())(1) == 0 && ((Dij_matrix.eigenvalues()).real())(2) == 0 && ((Dij_matrix.eigenvalues()).real())(3) == 0) {
				//cout << "diversity 為2的情況下的dmin，";
				dmin = pow((((Dij_matrix.eigenvalues()).real())(4) * ((Dij_matrix.eigenvalues()).real())(5)), 0.5);
				//printf("%.17f\n", dmin);
				if (dmin <= 1) {
					temp_dmin[0] += 1;
				}
				if (dmin > 1.9 && dmin <= 2) {
					temp_dmin[1] += 1;
				}
				if (dmin > 2.9 && dmin <= 3) {
					temp_dmin[2] += 1;
				}
				if (dmin > 3.9 && dmin <= 4) {
					temp_dmin[3] += 1;
				}
				//cout << "看這裡==>" << dmin << endl << endl;
				two++;
			}
			else {
				four++;
			}
			//	}//b!=a
		}//b for


		statistics_dmin[a][0] = temp_dmin[0];
		statistics_dmin[a][1] = temp_dmin[1];
		statistics_dmin[a][2] = temp_dmin[2];
		statistics_dmin[a][3] = temp_dmin[3];


		temp_dmin[0] = 0;
		temp_dmin[1] = 0;
		temp_dmin[2] = 0;
		temp_dmin[3] = 0;
		//printf("diversity is 4 : %d\n", four);
		//printf("diversity is 2 : %d\n", two);
	}
	double dmin[4];
	/*
	for (int jj = 0; jj < codeword; jj++) {

		dmin[0] += statistics_dmin[jj][0];
		dmin[1] += statistics_dmin[jj][1];
		dmin[2] += statistics_dmin[jj][2];
		dmin[3] += statistics_dmin[jj][3];
	}
	cout << "dmin is 1 : " << (dmin[0] * 2) / codeword << endl;
	cout << "dmin is 2 : " << (dmin[1] * 2) / (codeword) << endl;
	cout << "dmin is 3 : " << (dmin[2] * 2) / (codeword) << endl;
	cout << "dmin is 4 : " << (dmin[3] * 2) / (codeword) << endl;
	*/
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
	//cout << "codeword=" << codeword << endl;
	//cout << "interleaved=" << inter << endl;
	TxRx();
	//record();
	system("pause");
	return 0;
}