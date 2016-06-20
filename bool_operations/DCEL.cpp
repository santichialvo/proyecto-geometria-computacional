#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "malla.h"
#include "DCEL.h"

using namespace std;

void Point::add_edge_to_list(SLseg *_he) 
{
	rep.push_back(_he);
}

Point *center_t;

bool less_he(SLseg *a_he, SLseg *b_he)
{
	Point *a = a_he->twin->tail;
	Point *b = b_he->twin->tail;
	
	if (a->x[0] - center_t->x[0] >= 0 && b->x[0] - center_t->x[0] < 0)
		return true;
	if (a->x[0] - center_t->x[0] < 0 && b->x[0] - center_t->x[0] >= 0)
		return false;
	if (a->x[0] - center_t->x[0] == 0 && b->x[0] - center_t->x[0] == 0) {
		if (a->x[1] - center_t->x[1] >= 0 || b->x[1] - center_t->x[1] >= 0)
			return a->x[1] > b->x[1];
		return b->x[1] > a->x[1];
	}
	
	// compute the cross product of vectors (center_t -> a) x (center_t -> b)
	int det = (a->x[0] - center_t->x[0]) * (b->x[1] - center_t->x[1]) - (b->x[0] - center_t->x[0]) * (a->x[1] - center_t->x[1]);
	if (det < 0)
		return true;
	if (det > 0)
		return false;
	
	// points a and b are on the same line from the center_t
	// check which point is closer to the center_t
	int d1 = (a->x[0] - center_t->x[0]) * (a->x[0] - center_t->x[0]) + (a->x[1] - center_t->x[1]) * (a->x[1] - center_t->x[1]);
	int d2 = (b->x[0] - center_t->x[0]) * (b->x[0] - center_t->x[0]) + (b->x[1] - center_t->x[1]) * (b->x[1] - center_t->x[1]);
	return d1 > d2;
}

DCEL::DCEL(malla &m) 
{
	int cant_vertex = m.n.len/*-1*/; //1 menos por el editor de quads
	int cant_edges = m.e.len/*-1*/;
	
	for(int i=0;i<cant_vertex;i++) {
		Point *vn = new Point;
		vn->x[0]=m.n[i].x[0];
		vn->x[1]=m.n[i].x[1];
		v.push_back(vn);
	}
	
	for(int i=0;i<cant_edges;i++) {
		SLseg *he1 = new SLseg;
		SLseg *he2 = new SLseg;
		he1->tail = v[m.e[i].n[0]];
		he2->tail = v[m.e[i].n[1]];
		he1->twin = he2;
		he2->twin = he1;
		he1->edge = i;
		he2->edge = i+cant_edges; //Correspondencia entre twins 0->n_edges,1->n_edges+1,etc
		
		he.push_back(he1);
		he.push_back(he2);
		
		v[m.e[i].n[0]]->add_edge_to_list(he1);
		v[m.e[i].n[1]]->add_edge_to_list(he2);
		
		v[m.e[i].n[0]]->incident_edge = he1;
		v[m.e[i].n[1]]->incident_edge = he2;
	}
	
	//ordenar los edges clockwise
	for(int i=0;i<cant_vertex;i++) {
		center_t = v[i];
		sort(v[i]->rep.begin(),v[i]->rep.end(),less_he);
	}
	
	//setear los next y prevs
	for(int i=0;i<cant_vertex;i++) { 
		vector<SLseg*> &rep = v[i]->rep;
		for(size_t j=0;j<rep.size()-1;j++) {
			rep[j]->twin->next = rep[j+1];
			rep[j+1]->prev = rep[j]->twin;
		}
		rep[rep.size()-1]->twin->next = rep[0];
		rep[0]->prev = rep[rep.size()-1]->twin;
	}
	
	//"armar" las caras, buscando los loops para todos los half_edge
	size_t i=0,j;
	bool hay_para_procesar = true;
	SLseg *e = he[0]; //agarro la primera para empezar
	while (hay_para_procesar) {
		
		face *fn = new face;
		fn->index = i++;
		e->incident_face = fn;
		fn->rep.push_back(e);
		SLseg *enext = e->next;
		while (e != enext) {
			enext->incident_face = fn;
			fn->rep.push_back(enext);
			enext = enext->next;
		}
		f.push_back(fn);
		
		for(j=0;j<he.size();j++) { if (he[j]->incident_face==NULL) break; }
		if (j==he.size()) { 
			hay_para_procesar=false; 
		} else  {
			e = he[j];
		}
	}
	
}
void DCEL::show_half_edges()
{
	vector<SLseg*>::iterator it;
	for (it=he.begin();it!=he.end();it++) {
		cout<<(*it)->edge<<" "<<(*it)->tail->x[0]<<" "<<(*it)->tail->x[1]<<endl;
	}
}

void DCEL::show_vertex_incident_edge()
{
	vector<Point*>::iterator it;
	for (it=v.begin();it!=v.end();it++) {
		cout<<(*it)->x[0]<<" "<<(*it)->x[1]<<" "<<(*it)->incident_edge->edge<<endl;
	}
}

void DCEL::show_vertex_rep()
{
	vector<Point*>::iterator it;
	for (it=v.begin();it!=v.end();it++) {
		cout<<(*it)->x[0]<<" "<<(*it)->x[1]<<" ";
		vector<SLseg*>::iterator it2;
		for (it2=(*it)->rep.begin();it2!=(*it)->rep.end();it2++) {
			cout<<(*it2)->edge<<" ";
		}
		cout<<endl;
	}
}

void DCEL::show_loop_edge(int edge_comienzo, bool clockwise)
{
	SLseg *e = he[edge_comienzo];
	SLseg *enext = e->next;
	cout<<e->edge<<" ";
	while (e != enext) {
		cout<<enext->edge<<" ";
		enext = enext->next;
	}
}

void DCEL::show_loop_face(int face_comienzo)
{
	vector<SLseg*>::iterator it;
	for (it=f[face_comienzo]->rep.begin();it!=f[face_comienzo]->rep.end();it++) {
		cout<<(*it)->edge<<" "<<(*it)->tail->x[0]<<" "<<(*it)->tail->x[1]<<endl;
	}
}

//void DCEL::ad
