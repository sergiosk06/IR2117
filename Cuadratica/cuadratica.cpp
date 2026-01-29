#include <iostream>
using namespace std;
#include <cmath>

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
	       	double x1 = (-b + sqrt(d))/(2*a);
		double x2 = (-b - sqrt(d))/(2*a);
		cout << "Soluciones: " << x1 << ", " << x2 << endl;
		
	}
	else if (d < 0){
		cout << "No hay ninguna solucion";
	} else{
		double x = -b/(2*a);
		cout << "Hay una unica solucion x = " << x << endl;
	}
	return 0;
}

