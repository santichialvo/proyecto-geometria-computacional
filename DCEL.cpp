#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "malla.h"

using namespace std;

struct half_edge;

struct vertex 
{
	double x,y;
	half_edge *incident_edge;
	vector<half_edge*> rep;
	
	void add_edge_to_list(half_edge *_he) {
		rep.push_back(_he);
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
	vertex *tail;     /* twin->next->tail == tail && prev->twin->tail == tail */
	face *incident_face;
};


vertex *center_t;

bool less_he(half_edge *a_he, half_edge *b_he)
{
	vertex *a = a_he->twin->tail;
	vertex *b = b_he->twin->tail;
	
	if (a->x - center_t->x >= 0 && b->x - center_t->x < 0)
		return true;
	if (a->x - center_t->x < 0 && b->x - center_t->x >= 0)
		return false;
	if (a->x - center_t->x == 0 && b->x - center_t->x == 0) {
		if (a->y - center_t->y >= 0 || b->y - center_t->y >= 0)
			return a->y > b->y;
		return b->y > a->y;
	}
	
	// compute the cross product of vectors (center_t -> a) x (center_t -> b)
	int det = (a->x - center_t->x) * (b->y - center_t->y) - (b->x - center_t->x) * (a->y - center_t->y);
	if (det < 0)
		return true;
	if (det > 0)
		return false;
	
	// points a and b are on the same line from the center_t
	// check which point is closer to the center_t
	int d1 = (a->x - center_t->x) * (a->x - center_t->x) + (a->y - center_t->y) * (a->y - center_t->y);
	int d2 = (b->x - center_t->x) * (b->x - center_t->x) + (b->y - center_t->y) * (b->y - center_t->y);
	return d1 > d2;
}

struct DCEL 
{
	vector<vertex*> v;
	vector<face*> f;
	vector<half_edge*> he;
	
	DCEL() {}
	DCEL(malla &m) {
		int cant_vertex = m.n.len/*-1*/; //1 menos por el editor de quads
		int cant_edges = m.e.len/*-1*/;
		
		for(int i=0;i<cant_vertex;i++) {
			vertex *vn = new vertex;
			vn->x=m.n[i].x[0];
			vn->y=m.n[i].x[1];
			v.push_back(vn);
		}
		
		for(int i=0;i<cant_edges;i++) {
			half_edge *he1 = new half_edge;
			half_edge *he2 = new half_edge;
			he1->tail = v[m.e[i].n[0]];
			he2->tail = v[m.e[i].n[1]];
			he1->twin = he2;
			he2->twin = he1;
			he1->index = i;
			he2->index = i+cant_edges; //Correspondencia entre twins 0->n_edges,1->n_edges+1,etc
			
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
			vector<half_edge*> &rep = v[i]->rep;
			for(int j=0;j<rep.size()-1;j++) {
				rep[j]->twin->next = rep[j+1];
				rep[j+1]->prev = rep[j]->twin;
			}
			rep[rep.size()-1]->twin->next = rep[0];
			rep[0]->prev = rep[rep.size()-1]->twin;
		}
		
		//"armar" las caras, buscando los loops para todos los half_edge
		int i=0,j;
		bool hay_para_procesar = true;
		half_edge *e = he[0]; //agarro la primera para empezar
		while (hay_para_procesar) {
			
			face *fn = new face;
			fn->index = i++;
			e->incident_face = fn;
			fn->rep.push_back(e);
			half_edge *enext = e->next;
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
	void show_half_edges() {
		vector<half_edge*>::iterator it;
		for (it=he.begin();it!=he.end();it++) {
			cout<<(*it)->index<<" "<<(*it)->tail->x<<" "<<(*it)->tail->y<<endl;
		}
	}
	
	void show_vertex_incident_edge() {
		vector<vertex*>::iterator it;
		for (it=v.begin();it!=v.end();it++) {
			cout<<(*it)->x<<" "<<(*it)->y<<" "<<(*it)->incident_edge->index<<endl;
		}
	}
	
	void show_vertex_rep() {
		vector<vertex*>::iterator it;
		for (it=v.begin();it!=v.end();it++) {
			cout<<(*it)->x<<" "<<(*it)->y<<" ";
			vector<half_edge*>::iterator it2;
			for (it2=(*it)->rep.begin();it2!=(*it)->rep.end();it2++) {
				cout<<(*it2)->index<<" ";
			}
			cout<<endl;
		}
	}
	
	void show_loop_edge(int edge_comienzo, bool clockwise=true) {
		half_edge *e = he[edge_comienzo];
		half_edge *enext = e->next;
		cout<<e->index<<" ";
		while (e != enext) {
			cout<<enext->index<<" ";
			enext = enext->next;
		}
	}
	
	void show_loop_face(int face_comienzo) {
		vector<half_edge*>::iterator it;
		for (it=f[face_comienzo]->rep.begin();it!=f[face_comienzo]->rep.end();it++) {
			cout<<(*it)->index<<" "<<(*it)->tail->x<<" "<<(*it)->tail->y<<endl;
		}
	}
};

int main() 
{
	malla m;
	m.read(_DIR "dcel2.dat");
	
	DCEL dcel(m);
	
	dcel.show_loop_face(2);
}
