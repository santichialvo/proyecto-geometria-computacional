#include "segmentint.h"
#include "DCEL.h"
#include "malla.h"
#include <iostream>

using namespace std;

int main() {
	malla m1,m2,m12;
	m1.read(_DIR "dcel1.dat"); //Leo la primer malla de entrada
	m2.read(_DIR "dcel2.dat"); //Leo la segunda malla de entrada
	m12.read(_DIR "dcel12.dat"); //Leo la union de las mallas de entrada
	
	list<Point*> Intersections = Return_Intersections(m12);
	
	list<Point*>::iterator it;
	for(it=Intersections.begin();it!=Intersections.end();it++)
		cout<<(*it)->x[0]<<" "<<(*it)->x[1]<<endl;
}
