#include <iostream>
using namespace std;

int main() {
	int a, b;

	do {
	
		cout << "Introduce (a) siendo a positivo: ";
		cin >> a;
		
	} while (a < 1 );
	
	do {
	
		cout << "Introduce (b) siendo b positivo y >= que a: ";
		cin >> b;
		
	} while (b < 1 and b <= a);
	
		
	
	int sum = a + b;
	cout << "La suma es " << sum << endl;
	
	return 0;

}
