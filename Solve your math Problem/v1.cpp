#include <iostream>
using namespace std;

//Ecuacion lineal

int main(){

	float a = 0;
	float b = 0;

	cout << "Escribe un valor para 'a' en una eccuaicion lineal 'ax+b=0' ";
	cin >> a;
	cout << "Ahora escribe un valor para 'b' en una eccuaicion lineal 'ax+b=0' ";
	cin >> b;
	cout << "El valor de 'a' es " << a <<" y el valor de 'b' es " << b << " quedando la equacion como "<< a <<"x+"<< b <<"=0"<<endl;
	
	if (a != 0) {
		float x = -b / a;
		cout << "Hay una unica solucion para 'x' y es = " << x << endl;
	}
	else if (b != 0){
		cout << "No hay solucion para x" << endl;
	} else {
		cout << "x tiene infinitas soluciones" << endl;
	}
	return 0;	
}
