#include <iostream>
#include "segmentint.h"
#include <set>
#include <list>
#include <map>
using namespace std;

#define LEFT    0
#define RIGHT   1
#define INTERSECTION 2

int xyorder(Point *p1, Point *p2)
{
	if (p1->x[0] > p2->x[0]) return false; 
	if (p1->x[0] < p2->x[0]) return true;
	
	if (p1->x[1] > p2->x[1]) return false; 
	if (p1->x[1] < p2->x[1]) return true;
	
	return true;  //mismo punto
}

float isLeft(Point *P0, Point *P1, Point *P2)
{
	return (P1->x[0] - P0->x[0])*(P2->x[1] - P0->x[1]) - (P2->x[0] - P0->x[0])*(P1->x[1] -  P0->x[1]);
}

double mod (Point P) {
	return sqrt(pow(P.x[0]-P.x[0],2)+pow(P.x[1]*P.x[1],2));
}

//Rutinas de EventQueue
//===================================================================

bool EventQueue::add(Point *P,int Intnumb,list<Point*> Inters) 
{
	
	list<Point*>::iterator it = Inters.begin();
	double eps = 1e-3;
	for(it=Inters.begin();it!=Inters.end();it++) {
		Point *Pi = (*it);
		if (mod(*P - *Pi)<eps)
			return false; //ya esta en la lista
	}
	
	Event Nuevo;
	Nuevo.seg_index = Intnumb;
	Nuevo.eV = P;
	Nuevo.type = INTERSECTION;
	
	//Lo inserto total set se encarga de ubicarlo
	Eq.insert(Nuevo);
	
	return true;
}

void EventQueue::remove(Event E) 
{
	set<Event>::iterator it;
	for(it=Eq.begin();it!=Eq.end();it++) { 
		if (*it == E)
			break;
	}
	Eq.erase(it);
}

void EventQueue::showevents() 
{
	set<Event>::iterator it;
	for(it=Eq.begin();it!=Eq.end();it++) { 
		if ((*it).type==RIGHT) cout<<"right"; else cout<<"left";
		cout<<endl;
		cout<<(*it).eV->x[0]<<" "<<(*it).eV->x[1]<<" "<<endl;
	}
}

//constructor
EventQueue::EventQueue(DCEL &P)
{
	ix = 0;
	cant_aristas = P.he.size()/2;
	// Initialize event queue with edge segment endpoints
	for (size_t i=0; i < (2.0*cant_aristas); i+=2) {        // init data for edge i
		Event Nuevo1,Nuevo2;
		Nuevo1.eV = P.he[i]->tail;
		Nuevo2.eV = P.he[i]->twin->tail;
		
		Nuevo1.seg_index = (P.he[i]->edge)%cant_aristas;
		Nuevo2.seg_index = (P.he[i]->twin->edge)%cant_aristas;
		
		Nuevo1.Seg = P.he[i];
		Nuevo2.Seg = P.he[i]->twin;
		Nuevo1.Seg->seg_index = Nuevo1.seg_index;
		Nuevo2.Seg->seg_index = Nuevo2.seg_index;
		
		if (xyorder(Nuevo1.eV,Nuevo2.eV) > 0)  { // determine type
			Nuevo1.type = LEFT;
			Nuevo2.type = RIGHT;
			
			Nuevo1.Seg->lP = Nuevo1.eV;
			Nuevo1.Seg->rP = Nuevo2.eV;
			Nuevo2.Seg->lP = Nuevo1.eV;
			Nuevo2.Seg->rP = Nuevo2.eV;
		}
		else {
			Nuevo1.type = RIGHT;
			Nuevo2.type = LEFT;
			
			Nuevo1.Seg->lP = Nuevo2.eV;
			Nuevo1.Seg->rP = Nuevo1.eV;
			Nuevo2.Seg->lP = Nuevo2.eV;
			Nuevo2.Seg->rP = Nuevo1.eV;
		}
		
		Eq.insert(Nuevo1);
		Eq.insert(Nuevo2);
	}
}

int EventQueue::size() 
{
	return Eq.size();
}

Event EventQueue::next()
{	
	return *(Eq.begin());
}

//Rutinas de SweepLine
//===================================================================

void SweepLine::actualizarvals(double xnew) 
{
	set<SLseg*>::iterator it;
	for(it=Tree.begin();it!=Tree.end();it++)
		(*it)->val = (*it)->function(xnew);
}

void SweepLine::returnSegmentNumbers() 
{
	set<SLseg*>::iterator it;
	for(it=Tree.begin();it!=Tree.end();it++)
		cout<<(*it)->seg_index<<" "<<(*it)->lP->x[0]<<" "<<(*it)->lP->x[1]<<" "<<(*it)->rP->x[0]<<" "<<(*it)->rP->x[1]<<" "<<(*it)->val<<endl;
};

void SweepLine::swapSeg(int e1,int e2) {
	set_iterator it1 = find(e1);
	set_iterator it2 = find(e2);

	SLseg* Saux = new SLseg;
	Saux->seg_index = (*it2)->seg_index;
	Saux->lP = (*it2)->lP;
	Saux->rP = (*it2)->rP;
	
	(*it2)->lP = (*it1)->lP;
	(*it2)->rP = (*it1)->rP;
	(*it2)->seg_index = (*it1)->seg_index;
	
	(*it1)->lP = Saux->lP;
	(*it1)->rP = Saux->rP;
	(*it1)->seg_index = Saux->seg_index;
}

set_iterator SweepLine::add(Event &E)
{
	SLseg *s = E.Seg;
	
	//Trucheada máxima: 
	//Para que la dcel no me joda con los segmentos que se intersectan solo en
	//un vértice,los "acorto" cosa de que me guarde segmentos que no estan
	//unidos (Que trucazo no?)
	Point *result = new Point(*s->lP*0.99999 + *s->rP*0.00001);
	s->lP = result;
	Point *result2 = new Point(*s->lP*0.00001 + *s->rP*0.99999);
	s->rP = result2;
	s->val = s->function(s->lP->x[0]);
	
	//Agregar al arbol
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
		if ((*it)->seg_index == E.seg_index)
			return it;
	
	return Tree.end();
}

set_iterator SweepLine::find(int seg_index)
{
	if (seg_index==-1) return Tree.end();
	
	set<SLseg*>::iterator it = Tree.begin();
	for (it=Tree.begin();it!=Tree.end();it++)
		if ((*it)->seg_index == seg_index)
			return it;
	
	return Tree.end();
}

bool SweepLine::remove(SLseg *s)
{
	// remove the node from the balanced binary tree
	if (s->seg_index == -1) return false;
	
	set<SLseg*>::iterator nd,nx,np,beg,end;
	beg = Tree.begin();
	nd = Tree.find(s);
	
	if (nd == Tree.end())
		return false; // not there
	
	Tree.erase(nd); // remove it
	return true;
}

//=============================================================================

double Dot(const Point& a,const Point& b) { return (a.x[0]*b.x[0]) + (a.x[1]*b.x[1]); }
double PerpDot(const Point& a,const Point& b) { return (a.x[1]*b.x[0]) - (a.x[0]*b.x[1]); }

// Test de interseccion
Point* SweepLine::intersect(SLseg *s1, SLseg *s2)
{
	Point nulo(0,0);
	if (s1->seg_index == -1 || s2->seg_index == -1)
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
	
	Point a = *s1->rP - *s1->lP;
	Point b = *s2->rP - *s2->lP;
	Point c = *s2->rP - *s1->rP;
	double aa = PerpDot(a,c);
	double f = PerpDot(a,b);
	double out = 1.0 - (aa / f);
	Point intersection = ((*s2->rP - *s2->lP) * out) + *s2->lP;
	Point* salida = new Point(intersection.x[0],intersection.x[1]);
	
	return salida;
}

//=============================================================================
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
////===========================================================================

list<Point*> Return_Intersections(DCEL &dcel) {
	
	EventQueue Q1(dcel);      //Construyo la cola de eventos //Todos los endpoints ordenados x-creciente
	SweepLine S;              //Construyo la SweepLine
	
	//Variables varias
	Event E;
	Point nulo(0,0);
	set_iterator Seg,Seg2,SegA,SegB;
	bool Inserted=false;
	Point* I;
	list<Point*> Intersections;
	map<int,pair<int,int> > Map_Intersections;
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
			S.actualizarvals(E.eV->x[0]);
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
						p.first = SegA->seg_index;
						p.second = (*Seg)->seg_index;
						
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
						p.first = (*Seg)->seg_index;
						p.second = SegB->seg_index;
						
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
			S.returnSegmentNumbers();
			bool checkintersection = Seg!=S.begin();
			checkintersection &= (++Seg)!=S.end();
			
			Seg = S.find(E);
			S.remove(*Seg);
			
			cout<<endl;
			S.returnSegmentNumbers();
			system("clear");
			if (checkintersection) {
				
				I=S.intersect(SegA,SegB);
				
				if (I) {
					Inserted=Q1.add(I,Intnumb,Intersections);
					
					if (Inserted) {
						pair<int,int> p;
						p.first = SegA->seg_index;
						p.second = SegB->seg_index;
						
						Map_Intersections[Intnumb]=p;
						--Intnumb;
						Intersections.push_back(I);
					}
				}
			}
		} else { //INTERSECTION
			
			pair<int,int> p = Map_Intersections[E.seg_index];
			
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
				S.add(*Seg,E.eV->x[0]+0.02);
				S.remove(*Seg2);
				S.add(*Seg2,E.eV->x[0]+0.02);
				
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
							p.first = SegA->seg_index;
							p.second = (*Seg2)->seg_index;
							
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
							p.first = (*Seg)->seg_index;
							p.second = SegB->seg_index;
							
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
	
	return Intersections;
}
