////#include "segmentint.h"
//#include "malla.h"
//#include <iostream>
//#include <vector>
//#include <map>
//#include <algorithm>
//
//using namespace std;
//
//struct Vertex;
//struct Face;
//
//struct Point {
//	double x[2];
//	Point() {};
//	Point(double _x, double _y) { 
//		//		x = new double[2];
//		x[0]=_x; x[1]=_y; 
//	}
//	bool operator<(const Point &P2) const {
//		if (x[0]<P2.x[0]) return true;
//		if (x[1]<P2.x[1]) return true;
//		return false;
//	}
//	bool operator>(const Point &P2) const {
//		if (x[0]>P2.x[0]) return true;
//		else if (x[1]>P2.x[1]) return true;
//		return false;
//	}
//	bool operator==(const Point &P2) const {
//		return (x[0]==P2.x[0] && x[1]==P2.x[1]);
//	}
//	bool operator!=(const Point &P2) const {
//		return (x[0]!=P2.x[0] || x[1]!=P2.x[1]);
//	}
//	Point operator+(const Point &P2) const {
//		Point a(x[0]+P2.x[0],x[1]+P2.x[1]);
//		return a;
//	}
//	Point operator-(const Point &P2) const {
//		Point a(x[0]-P2.x[0],x[1]-P2.x[1]);
//		return a;
//	}
//	Point operator*(double alpha) const {
//		Point a(x[0]*alpha,x[1]*alpha);
//		return a;
//	}
//};
//
//struct Edge {
//	int indx;
//	Vertex *v1,*v2;
//	Face *f1,*f2;
//	Edge *e1,*e2;
//};
//
////TODO: Agregar logica de faces
//struct Face {
//	Edge e;
//};
//
////Face outside,inside;
//
//struct Vertex {
//	int indx;
//	Point p; //Punto
//	int e;  //edge al cual pertenece (el mas chico)
//	
//	inline bool operator < (const Vertex& v1) const
//	{
//		return (indx < v1.indx);
//	}
//	inline bool operator == (const Vertex& v1) const
//	{
//		return (indx == v1.indx);
//	}
//};
//
//struct DCEL {
//	
//	int n;
//	vector<Vertex> V;
//	vector<Edge> E;
//	
//	map<int,vector<Edge>> H; //Header de vertex-to-edge
//	
//public:
//	DCEL() {};
//	DCEL(malla &m) {
//		int cant_edges = m.e.len;
//		int cant_vertex = m.n.len;
//		for(int i=0;i<cant_edges;i++) {
//			Point p1(m.n[m.e[i].n[0]].x[0],m.n[m.e[i].n[0]].x[1]);
//			Point p2(m.n[m.e[i].n[1]].x[0],m.n[m.e[i].n[1]].x[1]);
//			Vertex *v1,*v2;
//			v1 = new Vertex;
//			v2 = new Vertex;
//			v1->p = p1;
//			v2->p = p2;
//			v1->e = i; 
//			v2->e = i;
//			v1->indx=m.e[i].n[0];
//			v2->indx=m.e[i].n[1];
//			V.push_back(*v1);
//			V.push_back(*v2);
//			
//			Edge e;
//			e.v1 = v1;
//			e.v2 = v2;
//			e.indx = i;
//			E.push_back(e);
//			
//			vector<Edge> emap = H[v1->indx];
//			emap.push_back(e);
//			H[v1->indx] = emap;
//			emap = H[v2->indx];
//			emap.push_back(e);
//			H[v2->indx] = emap;
//		}
////		sort(V.begin(),V.end());
////		V.erase(unique(V.begin(),V.end()),V.end());
//	}
//	
//	void show_edges() {
//		vector<Edge>::iterator it;
//		for(it=E.begin();it!=E.end();it++) {
//			cout<<(*it).indx<<" "<<(*it).v1->p.x[0]<<" "<<(*it).v1->p.x[1]<<" "<<(*it).v2->p.x[0]<<" "<<(*it).v2->p.x[1]<<endl;
//		}
//	}
//	void show_vertex() {
//		vector<Vertex>::iterator it;
//		for(it=V.begin();it!=V.end();it++) {
//			cout<<(*it).indx<<" "<<(*it).p.x[0]<<" "<<(*it).p.x[1]<<endl;
//		}
//	}
//	void show_map_H() {
//		map<int,vector<Edge>>::iterator it;
//		for(it=H.begin();it!=H.end();it++) {
//			cout<<it->first<<" ";
//			vector<Edge>::iterator it2;
//			for(it2=it->second.begin();it2!=it->second.end();it2++) {
//				cout<<(*it2).indx<<" ";
//			}
//			cout<<endl;
//		}
//	}
//	
//	void construir_vertex_to_edge();
//};
//
//void DCEL::construir_vertex_to_edge() {
//	
//	
//}
//
////int main() {
////	malla m;
////	m.read(_DIR "poli1.dat"); //Leo la malla de entrada
////	
////	DCEL P(m);
//////	P.show_vertex();
//////	cout<<endl;
////	P.show_edges();
////	cout<<endl;
////	P.show_map_H();
////	
////}
