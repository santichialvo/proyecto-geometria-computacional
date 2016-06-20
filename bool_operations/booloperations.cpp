#include "segmentint.h"
#include "DCEL.h"
#include "malla.h"
#include <iostream>
#include <cstdlib>
#include "window.h"

using namespace std;

void bool_operations(malla &m, DCEL &dcel) 
{
//	list<Point*> Intersections = Return_Intersections(dcel);
//	list<Point*>::iterator it;
//	for(it=Intersections.begin();it!=Intersections.end();it++)
//		cout<<(*it)->x[0]<<" "<<(*it)->x[1]<<endl;
}

void show_loop_face(malla &m, DCEL &dcel, int face_comienzo) 
{
  dcel.show_loop_face(2);
  cout<<endl;
  
  vector<SLseg*>::iterator it;
  vector<int> conservar;
  vector<int> no_conservar;
  for (it=dcel.f[face_comienzo]->rep.begin();it!=dcel.f[face_comienzo]->rep.end();it++) 
  {
    punto a((*it)->tail->x[0],(*it)->tail->x[1]);
    for(int i=0;i<m.n.len;i++) 
    {
      if ((a-m.n[i]).mod2()<1e-5) 
      {
        conservar.push_back(i);
        break;
      }
    }
  }
  for(int i=0;i<m.n.len;i++) 
  {
    int j=0;
    for(j=0;j<conservar.size();j++) 
    { 
      if (i==conservar[j])
        break;
    }
    if (j==conservar.size())
      no_conservar.push_back(i);
  }
  vector<int> elem_no_conservar;
  for(int i=0;i<no_conservar.size();i++) 
  {
    for(int j=0;j<m.n[no_conservar[i]].e.len;j++) 
    {
      elem_no_conservar.push_back(m.n[no_conservar[i]].e[j]);
    }
  }
  sort(elem_no_conservar.begin(),elem_no_conservar.end());
  elem_no_conservar.erase(unique(elem_no_conservar.begin(),elem_no_conservar.end()),elem_no_conservar.end());
  reverse(elem_no_conservar.begin(),elem_no_conservar.end());
  
  for(int i=0;i<elem_no_conservar.size();i++) 
  { 
    m.swap_e(elem_no_conservar[i],m.e.len-1);
    m.e.len--;
  }
}


void GLBox::experimento()
{
  DCEL dcel(*(this->m));
  bool_operations(*(this->m),dcel);
  show_loop_face(*(this->m),dcel,2);
  cambia_malla();
}
