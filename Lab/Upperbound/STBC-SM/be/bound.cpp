#include "control_variable.h"
#include "recording.h"
#include "modulate.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <bitset>

using namespace std;

const int DB0        = -10;                // Initial SNR(Eb/No(dB))
const int POINTNO    = 9;                // How many SNR(Eb/No(dB)) are measured
const int DBSTEP     = 2;                // Step of SNR(Eb/No(dB))

double snr, es1, es2;

double intregation();
double f(double);

QPSK signal;
STBC_SM index;
MATRIX mi(signal, index);

int main()
{
	initial_upperbound();
    TX_MATRIX A = mi.matrix[0];

	double R = (double)BIT / 2 / 2;  // time slot, 2 is number of antenna

	for (int point = 0; point < POINTNO; point++) {

	    snrdb = DB0 + point * DBSTEP;               // SNR in dB scale
	    snr = R * pow(10.0, 0.1 * snrdb);  // SNR in linear scale 
	    sigma = 0;

	    for (int i = 1; i < (int)pow(2, BIT); i++)
	    {
	 		// Hamming weight
	 		bitset<BIT> Tx_Bits(i);
	 		int b = Tx_Bits.count();

	 		// EigenValues
	 		TX_MATRIX B = mi.matrix[i];
	 		//ComplexEigenSolver<Matrix<COMPLEX, 2, 2>> es((A - B) * (A - B).adjoint());
            ComplexEigenSolver<MatrixXcd> es((A - B) * (A - B).adjoint());

	 		es1 = es.eigenvalues()[0].real();
	 		es2 = es.eigenvalues()[1].real();

	 		// intregation
	 		double intrega = intregation();

	 		sigma += (double)(b / (BIT * M_PI)) * intrega;
	    }

	    record_upperbound();
    	cout << setw(5) << snrdb;
        cout << setw(24) << fixed << setprecision(16) << sigma << endl;
	}
    system("pause");
	return 0;
}

double intregation()
{
	double h = (double)(M_PI / 2) / 1000; // 區間寬度
	double sum = 0.0;

	for (int i = 1; i < 1000; i++) {
		double x = i * h;
		sum += f(x);
	}

	sum += 0.5 * (f(0) + f(M_PI / 2));

	return sum * h;
}

double f(double x)
{
	double s1 = 1.0 + (snr * es1 * 0.25) / (sin(x) * sin(x));
	double s2 = 1.0 + (snr * es2 * 0.25) / (sin(x) * sin(x));
	double f1 = pow((1 / s1), RX);
	double f2 = pow((1 / s2), RX);

	return f1 * f2;
}