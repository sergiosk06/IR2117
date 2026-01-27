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

	return sum
}

int main() {
	int n;
	cout << "¿Cuantos numeros?"
	cin >> n;

	int suma = computar_suma(n);
	cout << "la suma es " << sum << endl;
	return 0;
}
   
