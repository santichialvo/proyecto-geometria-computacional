#ifndef SEGMENTINT_H
#define SEGMENTINT_H
#include "malla.h"
#include "DCEL.h"
#include <set>
#include <list>

using namespace std;

struct Polygon
{
	int n;
	Point *V;
	Polygon() {};
	Polygon(malla &m) {
		n = m.e.len;
		V = new Point[m.e.len*2];
		for (int i=0,j=0; i<m.e.len; i++, j+=2) {
			V[j].x[0]=m.n[m.e[i][0]].x[0];
			V[j].x[1]=m.n[m.e[i][0]].x[1];
			V[j+1].x[0]=m.n[m.e[i][1]].x[0];
			V[j+1].x[1]=m.n[m.e[i][1]].x[1];
		}
	}
};

//=============================================================================

int xyorder(Point *p1, Point *p2);

// Clase EventQueue 
struct Event
{
	int seg_index;  // el edge va de V[i] a V[i+1]
	int type;       // LEFT o RIGHT
	Point *eV;      // event vertex
	SLseg *Seg;
	
	bool operator==(const Event &E2) const {
		return (seg_index==E2.seg_index && type==E2.type);
	}
	
	bool operator<(const Event &E2) const {
		return xyorder(eV,E2.eV);
	}
};

//=============================================================================

class EventQueue 
{
	int cant_aristas;
	int ix;        //index of next event on queue
	set<Event> Eq; //array of all events
public:
	EventQueue(DCEL &P);     //constructor
	
	Event next(); // next event on queue
	int size();
	bool add(Point *P,int Intnumb,list<Point*> Inters);
	void remove(Event E);
	void showevents();
};

//=============================================================================

#define set_iterator set<SLseg*>::iterator

struct set_comparador
{
	bool operator() (const SLseg* s1, const SLseg* s2) const  { 
		return (s1->val > s2->val);
	}
};

class SweepLine {
	int     nv;                        // number of vertices in polygon
	Polygon Pn;                        // initial Polygon
	set<SLseg*,set_comparador> Tree;   // utilizo set
public:
	SweepLine(Polygon P)               // constructor
	{ nv = P.n*2; Pn = P; }
	
	set_iterator add(Event &E);
	set_iterator add(SLseg*,double);
	set_iterator find(Event);
	set_iterator find(int);
	void swapSeg(int,int);
	Point* intersect(SLseg*,SLseg*);
	bool remove(SLseg*);
	void actualizarvals(double xnew);
	set_iterator begin() { return Tree.begin(); }
	set_iterator end() { return Tree.end(); }
	void returnSegmentNumbers();
};

list<Point*> Return_Intersections(malla &m);

//=============================================================================



#endif
