#include <iostream>
using namespace std;

// funcion para computar

int computar_suma(int n) {

	int sum = 0;
	int x;

	for (int i = 0; i < n; ++i) {
		cout << "Escribe un numero: ";
		cin >> x;
		sum += x;

	}

	return sum;
}

int main() {
	int n;
	cout << "¿Cuantos numeros?";
	cin >> n;
	
	//se comprueba que sea true que n >= 1
	if (n >= 1) {
		int suma = computar_suma(n);
		cout << "la suma es " << suma << endl;
	}
	else {
		cout << "el numero de sumas tiene que ser mayor que 1. Escribe un numero nuevo: " << endl;
		cin >> n;
	}	
	
	return 0;
}
   
