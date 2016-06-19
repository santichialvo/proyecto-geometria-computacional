#ifndef DCEL_H
#define DCEL_H
#include <vector>
#include "malla.h"

using namespace std;

struct half_edge;

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
	half_edge *incident_edge;
	vector<half_edge*> rep;	
	void add_edge_to_list(half_edge *_he);
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
	vector<half_edge*> rep;
};

struct half_edge
{
	int index;
	half_edge *prev;  /* prev->next == this */
	half_edge *next;  /* next->prev == this */
	half_edge *twin;  /* twin->twin == this */
	Point *tail;     /* twin->next->tail == tail && prev->twin->tail == tail */
	face *incident_face;
};

struct DCEL
{
	vector<Point*> v;
	vector<face*> f;
	vector<half_edge*> he;
	
	DCEL() {}
	DCEL(malla &m);
	void show_half_edges();
	void show_vertex_incident_edge();
	void show_vertex_rep();
	void show_loop_edge(int edge_comienzo, bool clockwise=true);
	void show_loop_face(int face_comienzo);
};

#endif
