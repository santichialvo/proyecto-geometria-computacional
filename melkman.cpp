#include "malla.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <deque>
using namespace std;
class malla;

struct Punto {
	double x,y;
};

bool operator != (Punto a, Punto b)  { 
	return (a.x!=b.x || a.y!=b.y); 
}

// >0 P2 a la izquierda de P0-P1
// =0 P2 en la linea
// <0 P2 a la derecha de P0-P1
double isLeft(Punto &P0, Punto &P1, Punto &P2)
{
	return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
}

deque<Punto> melkman_v1(malla &m)
{
	deque<Punto> D;
	int nodlen = m.n.len;
	vector<Punto> nodosmalla; nodosmalla.resize(m.n.len);
	for(int i=0;i<m.n.len;i++) { nodosmalla[i].x = m.n[i].x[0]; nodosmalla[i].y = m.n[i].x[1]; }
	
	//Ver orientacion de los 3 primeros puntos
	D.push_back(nodosmalla[2]);
	if (isLeft(nodosmalla[0], nodosmalla[1], nodosmalla[2]) > 0) {
		D.push_back(nodosmalla[0]); // 2,0,1,2
		D.push_back(nodosmalla[1]); 
	}
	else {
		D.push_back(nodosmalla[1]); // 2,1,0,2
		D.push_back(nodosmalla[0]);
	}
	D.push_back(nodosmalla[2]);
	
	for (int i=3;i<nodlen;i++) {   // resto de los vertices
		if ((isLeft(D[0], D[1], nodosmalla[i]) > 0) &&
			(isLeft(D[D.size()-2], D[D.size()-1], nodosmalla[i]) > 0) )
			continue;         // interior, continuar
		
		while (isLeft(D[0], D[1], nodosmalla[i]) <= 0)
			D.pop_front();
		
		while (isLeft(D[D.size()-2], D[D.size()-1], nodosmalla[i]) <= 0)
			D.pop_back();
		
		D.push_front(nodosmalla[i]);
		D.push_back(nodosmalla[i]);
	}
	
	return D;
}

struct myclass {
	bool operator() (punto a,punto b) { return (a[0]<b[0]);}
} comparador;

//int main() {
//	malla m;
//	m.read(_DIR "tapascilindrorotadox.dat");
//	
//	vector<punto> Ps;
//	for(int i=0;i<m.n.len;i++)
//		Ps.push_back(m.n[i]);
//	
//	sort(Ps.begin(),Ps.end(),comparador);
//	
//	for(int i=0;i<m.n.len;i++)
//		m.n[i]=Ps[i];
//	
//	deque<Punto> D = melkman_v1(m);
//	
//	malla result;
//	for(int i=0;i<D.size()-1;i++) { 
//		nodo n;
//		n.x[0] = D[i].x;
//		n.x[1] = D[i].y;
//		result.n+=n;
//	}
//	_revienta(D[0]!=D[D.size()-1]);
//	
//	punto centro;
//	for (int i=0;i<result.n.len;i++)
//		centro+=result.n[i];
//	centro/=result.n.len;
//	
//	result.n+=centro;
//	
//	double max=0;
//	for (int i=0;i<result.n.len;i++)
//		if (centro.distancia(result.n[i])>max)
//			max=centro.distancia(result.n[i]);
////	for(int i=1;i<result.n.len;i++) { 
////		if (result.n[0].distancia(result.n[i])>max) {
////			max=result.n[0].distancia(result.n[i]);
////			cout<<i<<endl;
////		}
////	}
////	cout<<result.n[2].distancia(result.n[30])/2<<endl;
//	
//	cout<<max;
//	
//	result.save_dat(_DIR "result.dat");
//}
