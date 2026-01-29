#include <iostream>
using namespace std;

int main() {
	double a, b, c;
	int soluciones = 0;

	cout << "Coeficiente a: ";
	cin >> a;
	cout << "Coeficiente b: ";
	cin >> b;
	cout << "Coeficiente c: ";
	cin >> c;

	cout << "a = " << a << ", b = " << b << ", c = " << c << ", soluciones = "<< soluciones << endl;
	
	double d = b*b - 4*a*c;
	cout << "Discriminante = " << d << endl;

	if (d > 0) {
	soluciones += 2;
	cout << soluciones << endl;
	}
	return 0;
}

