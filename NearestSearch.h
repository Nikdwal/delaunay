#ifndef NEARESTSEARCH_H_
#define NEARESTSEARCH_H_

#include <vector>

#include "geometry.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef double								Real;
typedef CGAL::Simple_cartesian<Real>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			PH;

/*
*   Neemt een halfedge e en geeft deze weer als een string van de vorm:
*   "Start: (x1, y1, z1) \n End(x2, y2, z2)"
*/
std::string es(PH::Halfedge_handle e){
	std::string s = "";
	s += "Start: (";
	s += std::to_string(e->opposite()->vertex()->point().x());
	s += ", ";
	s += std::to_string(e->opposite()->vertex()->point().y());
	s += ", ";
	s += std::to_string(e->opposite()->vertex()->point().z());
	s +=")\nEnd: (";
	s += std::to_string(e->vertex()->point().x());
	s += ", ";
	s += std::to_string(e->vertex()->point().y());
	s += ", ";
	s += std::to_string(e->vertex()->point().z());
	s += ")\n\n";
	return s;
}

/*
*   Neemt een punt p en geeft dit weer als een string van de vorm:
*   "(x, y, z)"
*/
std::string ps(Point &p){
	std::string s = "";
	s += "(";
	s += std::to_string(p.x());
	s += ", ";
	s += std::to_string(p.y());
	s += ", ";
	s += std::to_string(p.z());
	s +=")";
	return s;
}

// algoritme van Guibas-Stolfi voor lokalisatie
/*
*   Neemt een punt p en een halfedge startEdge en berekent halfedge e door te wandelen over 
*   de reeds bepaalde delaunay triangulatie.
*   Geeft de halfedge terug waarop p ligt of die p links van zich heeft liggen.
*/
PH::Halfedge_handle adjEdge(const Point &p, PH::Halfedge_handle startEdge){
	PH::Halfedge_handle e = startEdge;
	while(true){
		if(p == e->vertex()->point() || p == e->opposite()->vertex()->point())
			return e;
		else if(rightOf(e,p))
			e = e->opposite();
		else if(!rightOf(origNext(e), p))
			e = origNext(e);
		else if(!rightOf(destPrev(e), p))
			e = destPrev(e);
		else
			return e;
	}
}

/*
*   Neemt een punt p en een triangulatie triangulation en berekent halfedge waarop p ligt
*   of de halfedge die p links van zich heeft liggen door de methode adjEdge(Point p, Halfedge e)
*   op te roepen.
*/
PH::Halfedge_handle adjEdge(const Point &p, PH &triangulation) {
    // TODO: dit kun je nog wijzigen, bv. naar het middelste punt
    return adjEdge(p, triangulation.halfedges_begin());
}
#endif
