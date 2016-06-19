#ifndef DCEL_H
#define DCEL_H
#include <vector>

using namespace std;

struct half_edge;

struct vertex
{
	double x,y;
	half_edge *incident_edge;
	vector<half_edge*> rep;
	
	void add_edge_to_list(half_edge *_he);
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

struct DCEL
{
	vector<vertex*> v;
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
