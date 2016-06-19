#include <iostream>
#include <set>
#include "malla.h"
#include "DCEL.h"
#include <cstddef>
#include <list>
#include <map>
#include <cstdlib>
using namespace std;

struct Point {
	double x[2];
	Point() {};
	Point(double _x, double _y) {
		x[0]=_x; x[1]=_y; 
	}
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

struct Polygon {
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

#define LEFT    0
#define RIGHT   1
#define INTERSECTION 2

int xyorder(Point p1, Point p2)
{
	if (p1.x[0] > p2.x[0]) return false; 
	if (p1.x[0] < p2.x[0]) return true;
	
	if (p1.x[1] > p2.x[1]) return false; 
	if (p1.x[1] < p2.x[1]) return true;
	
	return true;  //mismo punto
}

float isLeft(Point P0, Point P1, Point P2)
{
	return (P1.x[0] - P0.x[0])*(P2.x[1] - P0.x[1]) - (P2.x[0] - P0.x[0])*(P1.x[1] -  P0.x[1]);
}

double mod (Point P) {
	return sqrt(pow(P.x[0]-P.x[0],2)+pow(P.x[1]*P.x[1],2));
}

//===================================================================

// EventQueue Clase
struct Event {
	int edge;  // el edge va de V[i] a V[i+1]
	int type;  // LEFT o RIGHT
	Point eV;  // event vertex
	
	bool operator<(const Event &E2) const {
		return xyorder(eV,E2.eV);
	}
	bool operator==(const Event &E2) const {
		return (edge==E2.edge && type==E2.type);
	}
};

class EventQueue {
	int ne;        //total number of events in array
	int ix;        //index of next event on queue
	set<Event> Eq; //array of all events
//	set<Event>::iterator it_q;
public:
	EventQueue(Polygon P);     // constructor
	
	Event next(); // next event on queue
	int size();
	
	bool add(Point *P,int Intnumb,list<Point*> Inters) {
		
		list<Point*>::iterator it = Inters.begin();
		double eps = 1e-3;
		for(it=Inters.begin();it!=Inters.end();it++) {
			Point *Pi = (*it);
			if (mod(*P - *Pi)<eps)
				return false; //ya esta en la lista
		}
		
		Event Nuevo;
		Nuevo.edge = Intnumb;
		Nuevo.eV = *P;
		Nuevo.type = INTERSECTION;
		
		//Lo inserto total set se encarga de ubicarlo
		Eq.insert(Nuevo);
		
//		++it_q;
//		if ((*it_q).eV.x[0] > Nuevo.eV.x[0])
//			--it_q;
		
		return true;
	}
	
	void remove(Event E) {
		set<Event>::iterator it;
		for(it=Eq.begin();it!=Eq.end();it++) { 
			if (*it == E)
				break;
		}
		Eq.erase(it);
	}
	
	void showevents() {
		set<Event>::iterator it;
		for(it=Eq.begin();it!=Eq.end();it++) { 
			if ((*it).type==RIGHT) cout<<"right"; else cout<<"left";
			cout<<endl;
			cout<<(*it).eV.x[0]<<" "<<(*it).eV.x[1]<<" "<<endl;
		}
	}
};

// EventQueue Routines
EventQueue::EventQueue(Polygon P)
{
	ix = 0;
	ne = P.n; // 2 vertex events for each edge
	
	// Initialize event queue with edge segment endpoints
	for (int i=0,j=0; i < P.n; i++, j+=2) {        // init data for edge i
		Event Nuevo1,Nuevo2;
		Nuevo1.edge=Nuevo2.edge=i;
		Nuevo1.eV = P.V[j];
		Nuevo2.eV = P.V[j+1];
		
		if (xyorder(P.V[j],P.V[j+1]) > 0)  { // determine type
			Nuevo1.type = LEFT;
			Nuevo2.type = RIGHT;
		}
		else {
			Nuevo1.type = RIGHT;
			Nuevo2.type = LEFT;
		}
		
		Eq.insert(Nuevo1);
		Eq.insert(Nuevo2);
	}
	
//	it_q = Eq.begin();
}

int EventQueue::size() {
	return Eq.size();
}

Event EventQueue::next()
{	
	return *(Eq.begin());
}

//===================================================================
// SweepLine Class
struct SLseg;

// SweepLine segment data struct
struct SLseg {
	int   edge;        // polygon edge i is V[i] to V[i+1]
	Point lP;          // leftmost vertex point
	Point rP;          // rightmost vertex point
	mutable double val;       // valor de la función en la posicion de la sweepline
	
//	bool operator < (const SLseg *S2) const {
//		return (val < S2->val);
//	}
	
	bool operator!=(const SLseg &S2) const {
		return (edge != S2.edge);
	}
	
	bool operator==(const SLseg &S2) const {
		return (edge == S2.edge);
	}
	
	double function(double x) {
		return ((rP.x[1]-lP.x[1])/(rP.x[0]-lP.x[0]))*(x-lP.x[0]) + lP.x[1];
	}
};

struct set_comparador {
	bool operator() (const SLseg* s1, const SLseg* s2) const  { 
		return (s1->val > s2->val);
	}
};

#define set_iterator set<SLseg*>::iterator 

// the Sweep Line itself
class SweepLine {
	int     nv;        // number of vertices in polygon
	Polygon Pn;        // initial Polygon
	set<SLseg*,set_comparador> Tree;   // uso set
public:
	SweepLine(Polygon P) // constructor
	{ nv = P.n*2; Pn = P; }
	
	set_iterator add(Event);
	
	set_iterator add(SLseg*,double);
	
	set_iterator find(Event);
	
	set_iterator find(int);
	
	void swapSeg(int,int);
	
	Point* intersect(SLseg*,SLseg*);
	
	bool remove(SLseg*);
	
	void actualizarvals(double xnew) {
		set<SLseg*>::iterator it;
		for(it=Tree.begin();it!=Tree.end();it++)
			(*it)->val = (*it)->function(xnew);
	}
	
	set_iterator begin() { return Tree.begin(); }
	set_iterator end() { return Tree.end(); }
	
	void returnSegmentNumbers() {
		set<SLseg*>::iterator it;
		for(it=Tree.begin();it!=Tree.end();it++)
			cout<<(*it)->edge<<" "<<(*it)->lP.x[0]<<" "<<(*it)->lP.x[1]<<" "<<(*it)->rP.x[0]<<" "<<(*it)->rP.x[1]<<" "<<(*it)->val<<endl;
	};
	
//	void returnabovebelow() {
//		set<SLseg*>::iterator it;
//		for(it=Tree.begin();it!=Tree.end();it++){
//			cout<<(*it)->edge<<endl;
//			if (it->below != -1) 
//				cout<<"Abajo: "<<it->below<<endl;
//			if (it->above != -1) 
//				cout<<"Arriba: "<<it->above<<endl;
//			cout<<endl;
//		}
//	}
//	void checkintersectionforcebrute() {
//		set<_SL_segment>::iterator it1,it2;
//		it1 = Tree.begin();
//		while (it1!=Tree.end()) {
//			it2 = it1;
//			it2++;
//			for(it2;it2!=Tree.end();it2++){
//				SLseg s1 = *it1;
//				SLseg s2 = *it2;
//				if (intersect(&s1,&s2))
//					cout<<it1->edge<<" "<<it2->edge<<" intersectan"<<endl;
//			}
//			it1++;
//		}
//	}
};

void SweepLine::swapSeg(int e1,int e2) {
	set_iterator it1 = find(e1);
	set_iterator it2 = find(e2);

	SLseg* Saux = new SLseg;
	Saux->edge = (*it2)->edge;
	Saux->lP = (*it2)->lP;
	Saux->rP = (*it2)->rP;
	
	(*it2)->lP = (*it1)->lP;
	(*it2)->rP = (*it1)->rP;
	(*it2)->edge = (*it1)->edge;
	
	(*it1)->lP = Saux->lP;
	(*it1)->rP = Saux->rP;
	(*it1)->edge = Saux->edge;
}

set_iterator SweepLine::add(Event E)
{
	SLseg *s = new SLseg;
	s->edge  = E.edge;
	
	// if it is being added, then it must be a LEFT edge event
	// but need to determine which endpoint is the left one 
	Point v1 = Pn.V[2*(s->edge)]; 
	Point v2 = Pn.V[2*(s->edge)+1]; 
	if (xyorder(v1,v2) > 0) { // determine which is leftmost
		s->lP = v1; 
		s->rP = v2; 
	} 
	else { 
		s->rP = v1; 
		s->lP = v2;
	}
	
	//Trucheada máxima: 
	//Para que la dcel no me joda con los segmentos que se intersectan solo en
	//un vértice,los "acorto" cosa de que me guarde segmentos que no estan
	//unidos (Que trucazo no?)
	s->lP = s->lP*0.99999 + s->rP*0.00001;
	s->rP = s->lP*0.00001 + s->rP*0.99999;
	s->val = s->function(s->lP.x[0]);
	
	
	// add a node to the balanced binary tree
	pair<set<SLseg*>::iterator,bool> nd;
	nd = Tree.insert(s);
	
	if (nd.second) { //booleano que es true si se inserto con exito (no estaba repetido)
		return nd.first;
	} else {
		return Tree.end();
	}
}

set_iterator SweepLine::add(SLseg* s, double xnew)
{	
	s->val = s->function(xnew);
	// add a node to the balanced binary tree
	pair<set<SLseg*>::iterator,bool> nd;
	nd = Tree.insert(s);
	
	if (nd.second) { //booleano que es true si se inserto con exito (no estaba repetido)
		return nd.first;
	} else {
		return Tree.end();
	}
}

set_iterator SweepLine::find(Event E)
{
	set<SLseg*>::iterator it = Tree.begin();
	
	for (it=Tree.begin();it!=Tree.end();it++)
		if ((*it)->edge == E.edge)
			return it;
	
	return Tree.end();
}

set_iterator SweepLine::find(int edge)
{
	if (edge==-1) return Tree.end();
	
	set<SLseg*>::iterator it = Tree.begin();
	for (it=Tree.begin();it!=Tree.end();it++)
		if ((*it)->edge == edge)
			return it;
	
	return Tree.end();
}

bool SweepLine::remove(SLseg *s)
{
	// remove the node from the balanced binary tree
	if (s->edge == -1) return false;
	
	set<SLseg*>::iterator nd,nx,np,beg,end;
	beg = Tree.begin();
	nd = Tree.find(s);
	
	if (nd == Tree.end())
		return false; // not there
	
	Tree.erase(nd); // remove it
	return true;
}

double Dot(const Point& a,const Point& b) { return (a.x[0]*b.x[0]) + (a.x[1]*b.x[1]); }
double PerpDot(const Point& a,const Point& b) { return (a.x[1]*b.x[0]) - (a.x[0]*b.x[1]); }

// test intersect of 2 segments and return: 0=none, 1=intersect
Point* SweepLine::intersect(SLseg *s1, SLseg *s2)
{
	Point nulo(0,0);
	if (s1->edge == -1 || s2->edge == -1)
		return NULL;       // no intersect if either segment doesn't exist
	
	// test for existence of an intersect point
	float lsign, rsign;
	lsign = isLeft(s1->lP, s1->rP, s2->lP);    // s2 left point sign
	rsign = isLeft(s1->lP, s1->rP, s2->rP);    // s2 right point sign
	
	if (lsign * rsign >= 0) // s2 endpoints have same sign  relative to s1
		return NULL;       // => on same side => no intersect is possible
	
	lsign = isLeft(s2->lP, s2->rP, s1->lP);    //  s1 left point sign
	rsign = isLeft(s2->lP, s2->rP, s1->rP);    //  s1 right point sign
	
	if (lsign * rsign >= 0) // s1 endpoints have same sign  relative to s2
		return NULL;       // => on same side => no intersect is possible
	
	Point a = s1->rP - s1->lP;
	Point b = s2->rP - s2->lP;
	Point c = s2->rP - s1->rP;
	double aa = PerpDot(a,c);
	double f = PerpDot(a,b);
	double out = 1.0 - (aa / f);
	Point intersection = ((s2->rP - s2->lP) * out) + s2->lP;
	Point* salida = new Point(intersection.x[0],intersection.x[1]);
	
	return salida;
}
//===================================================================
//
//// simple_Polygon(): test if a Polygon is simple or not
////     Input:  Pn = a polygon with n vertices V[]
////     Return: FALSE(0) = is NOT simple
////             TRUE(1)  = IS simple
//int
//	simple_Polygon( Polygon Pn )
//{
//	EventQueue  Eq(Pn);
//	SweepLine   SL(Pn);
//	Event*      e;                  // the current event
//	SLseg*      s;                  // the current SL segment
//	
//	// This loop processes all events in the sorted queue
//	// Events are only left or right vertices since
//	// No new events will be added (an intersect => Done)
//	while (e = Eq.next()) {         // while there are events
//		if (e->type == LEFT) {      // process a left vertex
//			s = SL.add(e);          // add it to the sweep line
//			if (SL.intersect(  s, s->above)) 
//				return FALSE;      // Pn is NOT simple
//			if (SL.intersect(  s, s->below)) 
//				return FALSE;      // Pn is NOT simple
//		}
//		else {                      // processs a right vertex
//			s = SL.find(e);
//			if (SL.intersect(  s->above, s->below)) 
//				return FALSE;      // Pn is NOT simple
//			SL.remove(s);           // remove it from the sweep line
//		}
//	}
//	return TRUE;      // Pn IS simple
//}
////===================================================================

//list<Point*> Return_Intersections() {
int main() {
	malla m;
	m.read(_DIR "dcel1.dat"); //Leo la malla de entrada
	
	DCEL dcel(m);
	dcel.show_loop_face(2);
	cout<<endl;
	
	Polygon P(m);          //Construyo el poligono simple
	EventQueue Q1(P);      //Construyo la cola de eventos //Todos los endpoints ordenados x-creciente
	SweepLine S(P);        //Construyo la SweepLine
	
	//Variables varias
	Event E;
	Point nulo(0,0);
	set_iterator Seg,Seg2,SegA,SegB;
	bool Inserted=false;
	Point* I;
	list<Point*> Intersections;
	map<int,pair<int,int>> Map_Intersections;
	int Intnumb=-2;
//	Q1.showevents();
//	system("clear");
	
	while (Q1.size()!=0) {
		
		E = Q1.next();
//		if (E.edge==-1) break;
//		Q1.showevents();
//		system("clear");
		
		if (E.type==LEFT) {
			
//			S.returnSegmentNumbers();
			S.actualizarvals(E.eV.x[0]);
			Seg = S.add(E);     //Agrego el segmento a la SweepLine

//			cout<<endl;
//			S.returnSegmentNumbers();
//			system("clear");
			
			if (*Seg && Seg != S.begin()) {
				SLseg *SegA = *(--Seg);
				Seg++; //incremento porque decremente antes, para que quede en la posicion orginal
				I = S.intersect(*Seg,SegA);
				
				if (I) {
					Inserted = Q1.add(I,Intnumb,Intersections);
					
					if (Inserted) {
						pair<int,int> p;
						p.first = SegA->edge;
						p.second = (*Seg)->edge;
						
						Map_Intersections[Intnumb]=p;
						--Intnumb;
						Intersections.push_back(I);
					}
				}
			}
			
			Seg++;
			if (Seg != S.end()) {
				SLseg *SegB = *Seg;
				Seg--; //decremento porque incremente antes, para que quede en la posicion orginal
				I = S.intersect(*Seg,SegB);
				
				if (I) {
					Inserted=Q1.add(I,Intnumb,Intersections);
					
					if (Inserted) {
						pair<int,int> p;
						p.first = (*Seg)->edge;
						p.second = SegB->edge;
						
						Map_Intersections[Intnumb]=p;
						--Intnumb;
						Intersections.push_back(I);
					}
				}
			}
		}
		
		else if (E.type==RIGHT) {
			
			Seg = S.find(E);
			SLseg *SegA = *(--Seg);
			++Seg;
			SLseg *SegB = *(++Seg);
			--Seg;
			
			Seg = S.find(E);
//			S.returnSegmentNumbers();
			bool checkintersection = Seg!=S.begin();
			checkintersection &= (++Seg)!=S.end();
			
			Seg = S.find(E);
			S.remove(*Seg);
			
//			cout<<endl;
//			S.returnSegmentNumbers();
//			system("clear");
			if (checkintersection) {
				
				I=S.intersect(SegA,SegB);
				
				if (I) {
					Inserted=Q1.add(I,Intnumb,Intersections);
					
					if (Inserted) {
						pair<int,int> p;
						p.first = SegA->edge;
						p.second = SegB->edge;
						
						Map_Intersections[Intnumb]=p;
						--Intnumb;
						Intersections.push_back(I);
					}
				}
			}
		} else { //INTERSECTION
			
			pair<int,int> p = Map_Intersections[E.edge];
			
//			S.returnSegmentNumbers();
			Seg = S.find(p.first);
			Seg2 = S.find(p.second);
			
			if (Seg!=S.end() && Seg2!=S.end()) {
//				S.swapSeg(p.first,p.second);
				
//				S.returnSegmentNumbers();
//				cout<<endl;
				
				Seg = S.find(p.first);
				Seg2 = S.find(p.second);
				S.remove(*Seg);
				S.add(*Seg,E.eV.x[0]+0.02);
				S.remove(*Seg2);
				S.add(*Seg2,E.eV.x[0]+0.02);
				
//				S.returnSegmentNumbers();
//				cout<<endl;
//				system("clear");
				
				
				SLseg *SegA = *(--Seg2);
				SLseg *SegB = *(++Seg);
				Seg2 = S.find(p.second);
				Seg = S.find(p.first);
				
				if (Seg2 != S.begin()) {
					I = S.intersect(*Seg2,SegA);
					
					if (I) {
						
						Inserted = Q1.add(I,Intnumb,Intersections);
						
						if (Inserted) {
							pair<int,int> p;
							p.first = SegA->edge;
							p.second = (*Seg2)->edge;
							
							Map_Intersections[Intnumb]=p;
							--Intnumb;
							Intersections.push_back(I);
						}
					}
				}
				
				++Seg;
				if (Seg != S.end()) {
					--Seg;
					I = S.intersect(*Seg,SegB);
					
					if (I) {
						
						Inserted = Q1.add(I,Intnumb,Intersections);
						
						if (Inserted) {
							pair<int,int> p;
							//1 y 2 se insertan al reves
							p.first = (*Seg)->edge;
							p.second = SegB->edge;
							
							Map_Intersections[Intnumb]=p;
							--Intnumb;
							
							Intersections.push_back(I);
						}
					}
				}
			}
		}
		
		Q1.remove(E);
	}
	
	list<Point*>::iterator it;
	for(it=Intersections.begin();it!=Intersections.end();it++)
		cout<<(*it)->x[0]<<" "<<(*it)->x[1]<<endl;
	
//	return Intersections;
}
