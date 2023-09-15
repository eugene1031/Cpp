// SA-SM_sp_diversity.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
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
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include "Eigen/Eigen" 
#include <Eigen/SVD>

using namespace std;
using std::complex;
using namespace Eigen;
void LU(void){
    
}
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
void creatsp840() {//將sp105個資料放8個給sp840
	for (int x = 0; x < 8; x++) {
		for (int row = 0; row < 105; row++)
		{
			for (int col = 0; col < 8; col++)
			{
				sp840[105 * x + row][col] = sp105[row][col];

			}
		}
	}
};

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

int main() {
	complex<double> qpsk_map[4] = { complex<double>(1,0),complex<double>(0,1),complex<double>(-1,0),complex<double>(0,-1) };
	creatsp840();




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
1, 4, 5, 2, 3, 6,  1, 4, 5, 3, 6, 2,  1, 4, 5, 6, 2, 3
	};

	MatrixXcd de_STBC(6, 6), de_STBC_new(6, 6), de_STBC_new1(6, 6), de_STBC_new2(6, 6), de_STBC1(6, 6), de_STBC2(6, 6), de_STBC3(6, 6), de_STBC4(6, 6), de_STBC5(6, 6);
	MatrixXcd x1(4, 2), x2(4, 2), x3(4, 2), x4(4, 2);
	vector<MatrixXcd> matrices;
	vector<int> matrices0a, matrices0b, matrices1a, matrices1b;
	for (int g = 0; g < 3600; g++)
	{
		for (int x = 0; x < 1; x++) //14745600 * 6 = 88473600
		{
			for (int a = 0; a < 1; a++) //3686400 * 4 = 14745600
			{
				for (int b = 0; b < 1; b++) //921600 * 4 = 3686400
				{
					for (int c = 0; c < 1; c++) //230400 * 4 = 921600
					{
						for (int d = 0; d < 1; d++) //57600 * 4 = 230400
						{
							for (int e = 0; e < 1; e++) //14400 * 4 = 57600
							{
								for (int f = 0; f < 1; f++) //3600 * 4 = 14400
								{
									if (x == 0) {
										de_STBC <<
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0,
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0,
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]);
									}
									if (x == 1) {
										de_STBC <<
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0,
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0,
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]);
									}
									if (x == 2) {
										de_STBC <<
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0,
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]),
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0;
									}
									if (x == 3) {
										de_STBC <<
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0,
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]),
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0;
									}
									if (x == 4) {
										de_STBC <<
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]),
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0,
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0;
									}
									if (x == 5) {
										de_STBC <<
											0, 0, 0, 0, qpsk_map[e], -conj(qpsk_map[f]),
											0, 0, 0, 0, qpsk_map[f], conj(qpsk_map[e]),
											0, 0, qpsk_map[c], -conj(qpsk_map[d]), 0, 0,
											0, 0, qpsk_map[d], conj(qpsk_map[c]), 0, 0,
											qpsk_map[a], -conj(qpsk_map[b]), 0, 0, 0, 0,
											qpsk_map[b], conj(qpsk_map[a]), 0, 0, 0, 0;
									}

									de_STBC_new.col(0) << de_STBC.col(interleaved_pattern1[g % 60][0] - 1);
									de_STBC_new.col(1) << de_STBC.col(interleaved_pattern1[g % 60][1] - 1);
									de_STBC_new.col(2) << de_STBC.col(interleaved_pattern1[g % 60][2] - 1);
									de_STBC_new.col(3) << de_STBC.col(interleaved_pattern1[g % 60][3] - 1);
									de_STBC_new.col(4) << de_STBC.col(interleaved_pattern1[g % 60][4] - 1);
									de_STBC_new.col(5) << de_STBC.col(interleaved_pattern1[g % 60][5] - 1);

									de_STBC_new1 << de_STBC_new;

									de_STBC_new2.row(0) << de_STBC_new1.row(interleaved_pattern1[g / 60][0] - 1);
									de_STBC_new2.row(1) << de_STBC_new1.row(interleaved_pattern1[g / 60][1] - 1);
									de_STBC_new2.row(2) << de_STBC_new1.row(interleaved_pattern1[g / 60][2] - 1);
									de_STBC_new2.row(3) << de_STBC_new1.row(interleaved_pattern1[g / 60][3] - 1);
									de_STBC_new2.row(4) << de_STBC_new1.row(interleaved_pattern1[g / 60][4] - 1);
									de_STBC_new2.row(5) << de_STBC_new1.row(interleaved_pattern1[g / 60][5] - 1);
									matrices.push_back(de_STBC_new2);
								}
							}
						}
					}
				}
			}
		}
	}
	//int count = matrices.size();
	//cout << "向量中包含 " << count << " 個矩陣。" << endl;
	//cout << "matrices[60]:" << endl << matrices[60] << endl << endl;
	//cout << "matrices[1474560]:" << endl << matrices[1474560] << endl << endl;
	//cout << "matrices[1474561]:" << endl << matrices[1474561] << endl;
	//cout << sizeof(matrices);
	//cout << "matrices[0]:" << endl << matrices[0] << endl;
	//cout << "matrices[1]:" << endl << matrices[1] << endl;
	//cout << "matrices[2]:" << endl << matrices[2] << endl;
	//cout << "matrices[61]:" << endl << matrices[61] << endl;
	//cout << "matrices[766]:" << endl << matrices[766] << endl;
	//cout << "matrices[767]:" << endl << matrices[767] << endl;
	//cout << "matrices[768]:" << endl << matrices[768] << endl;
	int count0 = 0;
	int count1 = 0;

	cout << matrices.size();
	for (int i = 0; i < matrices.size(); i++) {
		for (int j = i + 1; j < matrices.size(); j++) {
			if (i < j) {
				cout << "i=" << i << "j=" << j << endl;
				MatrixXcd diff = matrices[i] - matrices[j];
				int rank_diff = diff.fullPivLu().rank();  // 計算差的rank
				if (rank_diff == 0) {
					//cout << "Matrices " << i / 512 << " and " << j / 512 << " have rank difference = 0." << endl;
					cout << "第 " << i % 3600 << " and " << j % 3600 << " have rank difference = 0." << endl;
					cout << matrices[i] << endl << endl << "         -   " << endl << endl << matrices[j] << endl << "\n\n";
					matrices0a.push_back(i / 384);
					matrices0b.push_back(j / 384);
					count0++;
					if (j % 384 != 0) {
						int num = 384 - (j % 384);
						j += num;
					}

				}
				if (rank_diff == 1) {
					//cout << "Matrices " << i / 512 << " and " << j / 512 << " have rank difference = 1." << endl;
					cout << "第 " << i % 3600 << " and " << j % 3600 << " have rank difference = 1." << endl;
					cout << matrices[i] << endl << endl << "         -   " << endl << endl << matrices[j] << endl << "\n\n";
					matrices1a.push_back(i / 384);
					matrices1b.push_back(j / 384);
					count1++;
					if (j % 384 != 0) {
						int num = 384 - (j % 384);
						j += num;
					}
				}
			}
		}
	}
	cout << "有" << count0 << "組rank=0" << endl;
	cout << "有" << count1 << "組rank=1" << endl;
	for (int a = 0; a < matrices0a.size(); a++) {
		cout << matrices0a[a] << "-" << matrices0b[a] << endl << "rank = 0 " << endl;
	}
	for (int b = 0; b < matrices1a.size(); b++) {
		cout << matrices1a[b] << "-" << matrices1b[b] << endl << "rank = 1 " << endl;
	}
	system("pause");
}
