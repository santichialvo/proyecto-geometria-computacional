#include <iostream>
#include <set>
#include "malla.h"
#include <cstddef>
#include <list>
#include <map>
#include <cstdlib>
using namespace std;

struct Point {
	double x[2];
	Point() {};
	Point(double _x, double _y) { 
//		x = new double[2];
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
		n = m.n.len/2;
		V = new Point[m.n.len];
		for(int i=0;i<m.n.len;i++) {
			V[i].x[0]=m.n[i].x[0];
			V[i].x[1]=m.n[i].x[1];
		}
	}
};

//#define FALSE   0
//#define TRUE    1
#define LEFT    0
#define RIGHT   1
#define INTERSECTION 2

//extern void  qsort(void*, unsigned, unsigned, int(*)(const void*,const void*));

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
};

//int E_compare(Event v1, Event v2) //Comparar dos eventos
//{
//	return xyorder(v1.eV, v2.eV);
//}

class EventQueue {
	int ne;        //total number of events in array
	int ix;        //index of next event on queue
	set<Event> Eq; //array of all events
	set<Event>::iterator it_q;
public:
	EventQueue(Polygon P);     // constructor
//	~EventQueue(void)           // destructor
//	{ delete Eq; delete Edata; }
	
	Event next(); // next event on queue
	
	bool add(Point P,int Intnumb,list<Point> Inters) {
		
		list<Point>::iterator it = Inters.begin();
		double eps = 1e-3;
		for(it=Inters.begin();it!=Inters.end();it++) {
			Point Pi = (*it);
			if (mod(P-Pi)<eps)
				return false; //ya esta en la lista
		}
		
		Event Nuevo;
		Nuevo.edge = Intnumb;
		Nuevo.eV = P;
		Nuevo.type = INTERSECTION;
		
		//Lo inserto total set se encarga de ubicarlo
		Eq.insert(Nuevo);
		
		if ((*it_q).eV.x[0] > Nuevo.eV.x[0])
			--it_q;
		
		return true;
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
	ne = 2*P.n; // 2 vertex events for each edge
//	Edata = (Event*)new Event[ne];
//	Eq = (Event**)new Event*[ne];
//	for (int i=0; i < ne; i++)           // init Eq array pointers
//		Eq[i] = &Edata[i];
	
	// Initialize event queue with edge segment endpoints
	for (int i=0,j=0; i < P.n; i++, j+=2) {        // init data for edge i
		Event Nuevo1,Nuevo2;
		Nuevo1.edge=Nuevo2.edge=i;
		Nuevo1.eV = P.V[j];
		Nuevo2.eV = P.V[j+1];
//		Eq[2*i]->edge = i;
//		Eq[2*i+1]->edge = i;
//		Eq[2*i]->eV   = &(P.V[j]);
//		Eq[2*i+1]->eV = &(P.V[j+1]);
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
	// Sort Eq[] by increasing x and y
//	qsort(Eq, ne, sizeof(Event*), E_compare);
	it_q = Eq.begin();
}

Event EventQueue::next()
{
	Event e;
	e.edge = -1;
	if (it_q == Eq.end())
		return e;
	
	e = (*it_q);
	it_q++;	
	
	return e;
}

//===================================================================
// SweepLine Class
struct SLseg;

// SweepLine segment data struct
struct SLseg {
	int   edge;          // polygon edge i is V[i] to V[i+1]
	Point lP;            // leftmost vertex point
	Point rP;            // rightmost vertex point
	mutable int above;         // segment above this one
	mutable int below;         // segment below this one
	
	bool operator<(const SLseg &S2) const {
//		if (lP.x[0]<S2.lP.x[0]) 
//			return true;
		/*else */if (/*(lP.x[0]==S2.lP.x[0])&&*/(lP.x[1]<S2.lP.x[1])) 
			return true;
		return false;
	}
	
	bool operator!=(const SLseg &S2) const {
		return (edge != S2.edge);
	}
	
	bool operator==(const SLseg &S2) const {
		return (edge == S2.edge);
	}
};

//struct cmp
//{
//	bool operator()(const _SL_segment &a, const _SL_segment &b) const {
//		return (a.edge != b.edge);
//	}
//};

// the Sweep Line itself
class SweepLine {
	int     nv;        // number of vertices in polygon
	Polygon Pn;        // initial Polygon
	set<SLseg> Tree;   // uso set a ver que onda
public:
	SweepLine(Polygon P) // constructor
	{ nv = P.n*2; Pn = P; }
//	~SweepLine(void)                 // destructor
//	{ Tree.freetree();}
	
	SLseg add(Event);
	
	SLseg find(Event);
	
	SLseg find(int);
	
	void swapSeg(int,int);
	
	Point intersect(SLseg,SLseg);
	
	void remove(SLseg);
	
	void returnSegmentNumbers() {
		set<SLseg>::iterator it;
		for(it=Tree.begin();it!=Tree.end();it++)
			cout<<it->edge<<" "<<it->lP.x[0]<<" "<<it->lP.x[1]<<" "<<it->rP.x[0]<<" "<<it->rP.x[1]<<endl;
	};
	void returnabovebelow() {
		set<SLseg>::iterator it;
		for(it=Tree.begin();it!=Tree.end();it++){
			cout<<it->edge<<endl;
			if (it->below != -1) 
				cout<<"Abajo: "<<it->below<<endl;
			if (it->above != -1) 
				cout<<"Arriba: "<<it->above<<endl;
			cout<<endl;
		}
	}
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

void SweepLine::swapSeg(int e2,int e3) {
	SLseg S2 = find(e2);
	SLseg S3 = find(e3);
	SLseg S1 = find(S2.above);
	SLseg S4 = find(S3.below);
	int s1=S2.above,s2=e2,s3=e3,s4=S3.below;
	set<SLseg>::iterator it1,it2,it3,it4,it;
	for (it=Tree.begin();it!=Tree.end();it++) {
		if ((*it)==S1) {
			it1=it;
			(*it1).below=s3;
		}
		if ((*it)==S2) {
			it2=it;
			(*it2).below=s4;
			(*it2).above=s3;
		}
		if ((*it)==S3) {
			it3=it;
			(*it3).below=s2;
			(*it3).above=s1;
		}
		if ((*it)==S4) {
			it4=it;
			(*it4).above=s2;
		}
	}
}

SLseg SweepLine::add(Event E)
{
	SLseg s;
	s.edge  = E.edge;
	
	// if it is being added, then it must be a LEFT edge event
	// but need to determine which endpoint is the left one 
	Point v1 = Pn.V[2*(s.edge)]; 
	Point v2 = Pn.V[2*(s.edge)+1]; 
	if (xyorder(v1,v2) > 0) { // determine which is leftmost
		s.lP = v1; 
		s.rP = v2; 
	} 
	else { 
		s.rP = v1; 
		s.lP = v2;
	}
	s.above = -1;
	s.below = -1;
	
	returnSegmentNumbers();
	system("clear");
	
	// add a node to the balanced binary tree
	pair<set<SLseg>::iterator,bool> nd;
	nd = Tree.insert(s);
	
	returnSegmentNumbers();
	returnabovebelow();
//	system("clear");
	if (nd.second) { //booleano que es true si se inserto con exito (no estaba repetido)
		
		set<SLseg>::iterator it1,it2;
		set<SLseg>::iterator beg = Tree.begin();
		set<SLseg>::iterator end = Tree.end();
		it1=it2=nd.first;

		++it2;
		if (it1 != beg) {
			--it1;
			SLseg *nx = new SLseg(*it1);
			(nd.first)->above = nx->edge;
			it1->below = s.edge;
		}
		if (it2 != end) {
			SLseg *np = new SLseg(*it2);
			(nd.first)->below = np->edge;
			it2->above = s.edge;
		}
	}
	
	returnSegmentNumbers();
	system("clear");
	
	return *nd.first;
}

SLseg SweepLine::find(Event E)
{
	set<SLseg>::iterator it = Tree.begin();
	
	for (it=Tree.begin();it!=Tree.end();it++)
		if ((*it).edge == E.edge)
			break;
	
	SLseg nd = *it;
	if (it == Tree.end())
		return *it;
	
	return nd;
}

SLseg SweepLine::find(int edge)
{
	SLseg nd;
	nd.edge=-1;
	if (edge==-1) return nd;
	
	set<SLseg>::iterator it = Tree.begin();
	for (it=Tree.begin();it!=Tree.end();it++)
		if ((*it).edge == edge)
			break;
	
	nd = *it;
	
	if (it == Tree.end())
		return *it;
	
	return nd;
}

void SweepLine::remove(SLseg s)
{
	// remove the node from the balanced binary tree
	if (s.edge == -1) return;
	
	set<SLseg>::iterator nd,nx,np,beg,end;
	beg = Tree.begin();
	end = Tree.end();
	SLseg ss = s;
	returnabovebelow();
	returnSegmentNumbers();
	system("clear");
	nd = Tree.find(ss);
	nx = nd; np = nd;
	if (nd == end)
		return; // not there
	
	// get the above and below segments pointing to each other
//	nx++;
	if (nd->below != -1) {
		SLseg S = find(nd->below);
		set<SLseg>::iterator it;
		for (it=Tree.begin();it!=Tree.end();it++)
			if ((*it)==S)
				break;
		(*it).above = nd->above;
	}
	returnabovebelow();
	returnSegmentNumbers();
	system("clear");
//	np--;
	if (nd->above != -1) {
		SLseg S = find(nd->above);
		set<SLseg>::iterator it;
		for (it=Tree.begin();it!=Tree.end();it++)
			if ((*it)==S)
				break;
		(*it).below = nd->below;
	}
	
	Tree.erase(nd); //now  can safely remove it
	returnabovebelow();
	returnSegmentNumbers();
	system("clear");
}

double Dot(const Point& a,const Point& b) { return (a.x[0]*b.x[0]) + (a.x[1]*b.x[1]); }
double PerpDot(const Point& a,const Point& b) { return (a.x[1]*b.x[0]) - (a.x[0]*b.x[1]); }

// test intersect of 2 segments and return: 0=none, 1=intersect
Point SweepLine::intersect(SLseg s1, SLseg s2)
{
	Point nulo(0,0);
	if (s1.edge == -1 || s2.edge == -1)
		return nulo;       // no intersect if either segment doesn't exist
	
	// check for consecutive edges in polygon
//	int e1 = s1.edge;
//	int e2 = s2.edge;
	
	// test for existence of an intersect point
	float lsign, rsign;
	lsign = isLeft(s1.lP, s1.rP, s2.lP);    // s2 left point sign
	rsign = isLeft(s1.lP, s1.rP, s2.rP);    // s2 right point sign
	
	if (lsign * rsign >= 0) // s2 endpoints have same sign  relative to s1
		return nulo;       // => on same side => no intersect is possible
	
	lsign = isLeft(s2.lP, s2.rP, s1.lP);    //  s1 left point sign
	rsign = isLeft(s2.lP, s2.rP, s1.rP);    //  s1 right point sign
	
	if (lsign * rsign >= 0) // s1 endpoints have same sign  relative to s2
		return nulo;       // => on same side => no intersect is possible
	
	Point a = s1.rP - s1.lP;
	Point b = s2.rP - s2.lP;
	Point c = s2.rP - s1.rP;
	double aa = PerpDot(a,c);
	double f = PerpDot(a,b);
	double out = 1.0 - (aa / f);
	Point intersection = ((s2.rP - s2.lP) * out) + s2.lP;
	Point salida(intersection.x[0],intersection.x[1]);
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

list<Point> Return_Intersections() {
	malla m;
	m.read(_DIR "poli1.dat"); //Leo la malla de entrada
	
	Polygon P(m);          //Construyo el poligono simple
	EventQueue Q1(P);      //Construyo la cola de eventos //Todos los endpoints ordenados x-creciente
	SweepLine S(P);        //Construyo la SweepLine
	
	Event E;
	Point nulo(0,0);
	SLseg Seg,Seg2,SegA,SegB;
	bool Inserted=false;
//	int EdgeA,EdgeB;
//	int SSeg,SSeg2;
	Point I;
	list<Point> Intersections;
	map<int,pair<int,int>> Map_Intersections;
	int Intnumb=-2;
	Q1.showevents();
	system("clear");
	while (true) {
		
		E = Q1.next();
		if (E.edge==-1) break;
		Q1.showevents();
		system("clear");
		
		if (E.type==LEFT) {
			
			Seg = S.add(E);     //Agrego el segmento a la SweepLine
//			SSeg=S.find(E);
			S.returnabovebelow();
			S.returnSegmentNumbers();
			system("clear");
			
			SegA = S.find(Seg.above);
			I=S.intersect(Seg,SegA);
			
			if (I!=nulo) {
				Inserted=Q1.add(I,Intnumb,Intersections);
				
				if (Inserted) {
					pair<int,int> p; 
					p.first=Seg.edge; 
					p.second=SegA.edge;
					
					Map_Intersections[Intnumb]=p;
					--Intnumb;
					Intersections.push_back(I);
				}
//				cout<<I->x<<" "<<I->y<<endl;
			}
			
			SegB = S.find(Seg.below);
			I=S.intersect(Seg,SegB);
			
			if (I!=nulo) {
				Inserted=Q1.add(I,Intnumb,Intersections);
				
				if (Inserted) {
					pair<int,int> p; 
					p.first=Seg.edge; 
					p.second=SegB.edge;
					
					Map_Intersections[Intnumb]=p;
					--Intnumb;
					Intersections.push_back(I);
				}
//				cout<<I->x<<" "<<I->y<<endl;
			}
		}
		
		else if (E.type==RIGHT) {
			
			Seg = S.find(E);
			SegA=S.find(Seg.above);
			SegB=S.find(Seg.below);
			
			S.returnabovebelow();
			S.returnSegmentNumbers();
			system("clear");
			
			S.remove(Seg);
			
			S.returnabovebelow();
			S.returnSegmentNumbers();
			system("clear");
			
			I=S.intersect(SegA,SegB);
			
			if (I!=nulo) {
				Inserted=Q1.add(I,Intnumb,Intersections);
				
				if (Inserted) {
					pair<int,int> p; 
					p.first = SegA.edge; 
					p.second = SegB.edge;
					
					Map_Intersections[Intnumb]=p;
					--Intnumb;
					//				Q1.showevents();
					Intersections.push_back(I);
				}
//				cout<<I->x<<" "<<I->y<<endl;
			}
//			cout<<I->x<<" "<<I->y<<endl;
		} else { //INTERSECTION
//			Intersections.push_back(E->eV);
//			S.returnabovebelow();
//			S.returnSegmentNumbers();
//			cout<<endl;
			
			pair<int,int> p = Map_Intersections[E.edge];
			
			S.returnabovebelow();
			S.returnSegmentNumbers();
			system("clear");
			
			S.swapSeg(p.second,p.first);
			
			S.returnabovebelow();
			S.returnSegmentNumbers();
			system("clear");
			
			Seg = S.find(p.second);
			Seg2 = S.find(p.first);
			SegA = S.find(Seg2.above);
			SegB = S.find(Seg.below);
			
			I=S.intersect(Seg2,SegA);
			
			if (I!=nulo) {
				
				Inserted=Q1.add(I,Intnumb,Intersections);
				
				if (Inserted) {
					pair<int,int> p;
					p.first = Seg2.edge; 
					p.second = SegA.edge;
					
					Map_Intersections[Intnumb]=p;
					--Intnumb;
					Intersections.push_back(I);
				}
//				cout<<I->x<<" "<<I->y<<endl;
			}
			
			I=S.intersect(Seg,SegB);
			
			if (I!=nulo) {
				
				Inserted=Q1.add(I,Intnumb,Intersections);
				
				if (Inserted) {
					pair<int,int> p; 
					p.first = Seg.edge; 
					p.second = SegB.edge;
					
					Map_Intersections[Intnumb]=p;
					--Intnumb;
					
					Intersections.push_back(I);
				}
//				cout<<I->x<<" "<<I->y<<endl;
			}
		}
	}
	
	list<Point>::iterator it;
	for(it=Intersections.begin();it!=Intersections.end();it++)
		cout<<(*it).x[0]<<" "<<(*it).x[1]<<endl;
	
	return Intersections;
}
