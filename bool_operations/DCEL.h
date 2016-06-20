#ifndef DCEL_H
#define DCEL_H
#include <vector>
#include "malla.h"

using namespace std;

struct SLseg;

struct Point
{
	double x[2];
	Point() {};
	Point(double _x, double _y) 
	{
		x[0]=_x; x[1]=_y; 
	}
	//Esto es para DCEL
	//=========================================================================
	SLseg *incident_edge;
	vector<SLseg*> rep;	
	void add_edge_to_list(SLseg *_he);
	//Esto es para segmentint
	//=========================================================================
	bool operator<(const Point &P2) const {
		if (x[0]<P2.x[0]) return true;
		if (x[1]<P2.x[1]) return true;
		return false;
	}
	bool operator>(const Point &P2) const {
		if (x[0]>P2.x[0]) return true;
		else if (x[1]>P2.x[1]) return true;
		return false;
	}
	bool operator==(const Point &P2) const {
		return (x[0]==P2.x[0] && x[1]==P2.x[1]);
	}
	bool operator!=(const Point &P2) const {
		return (x[0]!=P2.x[0] || x[1]!=P2.x[1]);
	}
	Point operator+(const Point &P2) const {
		Point a(x[0]+P2.x[0],x[1]+P2.x[1]);
		return a;
	}
	Point operator-(const Point &P2) const {
		Point a(x[0]-P2.x[0],x[1]-P2.x[1]);
		return a;
	}
	Point operator*(double alpha) const {
		Point a(x[0]*alpha,x[1]*alpha);
		return a;
	}
};

struct face
{
	int index;
	vector<SLseg*> rep;
};

struct SLseg
{
	//Esto es para DCEL
	//=========================================================================
	int edge;           //Indice del edge/segmento
	SLseg *prev;
	SLseg *next;
	SLseg *twin;
	Point *tail;
	face *incident_face;
	
	//Esto es para segmentint
	//=========================================================================
	int seg_index;      //Indice del edge/segmento
	Point *lP;          // leftmost vertex point
	Point *rP;          // rightmost vertex point
	mutable double val; // valor de la función en la posicion de la sweepline
	
	bool operator!=(const SLseg &S2) const 
	{
		return (edge != S2.edge);
	}
	
	bool operator==(const SLseg &S2) const 
	{
		return (edge == S2.edge);
	}
	
	double function(double x) 
	{
		return ((rP->x[1]-lP->x[1])/(rP->x[0]-lP->x[0]))*(x-lP->x[0]) + lP->x[1];
	}
};

struct DCEL
{
	vector<Point*> v;
	vector<face*> f;
	vector<SLseg*> he;
	
	DCEL() {}
	DCEL(malla &m);
	void show_half_edges();
	void show_vertex_incident_edge();
	void show_vertex_rep();
	void show_loop_edge(int edge_comienzo, bool clockwise=true);
	void show_loop_face(int face_comienzo);
};

#endif
