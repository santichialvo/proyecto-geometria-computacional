#ifndef SEGMENTINT_H
#define SEGMENTINT_H
#include "malla.h"
#include "DCEL.h"
#include <set>
#include <list>

using namespace std;

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
	set<SLseg*,set_comparador> Tree;   // utilizo set
public:
	SweepLine() {}               // constructor
	
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
