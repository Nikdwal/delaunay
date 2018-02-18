#ifndef NEARESTSEARCH_H_
#define NEARESTSEARCH_H_

#include <vector>

#include "geometry.h"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef CGAL::Simple_cartesian<double>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			PH;

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

PH::Halfedge_handle adjEdge(const Point &p, PH &triangulation){
	// TODO: dit kun je nog wijzigen, bv. naar het middelste punt
	return adjEdge(p, triangulation.halfedges_begin());
}

// TODO: ik denk dat je na het zoeken van deze boog gewoon de driehoek kan teruggeven
// in plaats van een van de vertices van deze gevonden boog. Dit is immers het punt waarop
// GS en BW van elkaar beginnen te verschillen.

//
//Edge getClosestEdge(const Vertex &p, const std::vector<Triangle> triangleList) {
//    Triangle t = triangleList.at(0);   // TODO: middenste punt nemen als startpunt of eindpunt hilbert
//    Edge e = t.getEdge();
//
//    bool flag = true;
//    while (flag) {
//        if (p == e.getOrigin() || p == e.getDestination()) {
//            return e;
//        }
//        else if (e.rightOf(p)) {
//            e = e.getSym();
//        }
//        else if (!(t.getOrgNext(e)).rightOf(p)) {
//            e = t.getOrgNext(e);
//        }
//        else if (!(t.getDestPrev(e)).rightOf(p)) {
//            e = t.getDestPrev(e);
//        }
//        else {
//            return e;
//        }
//    }
//}
//
////TODO: Zie return by value of return by pointer
//Vertex walkNN(const Vertex &p, const std::vector<Triangle> &triangleList) {
//
//    Edge closestEdge = getClosestEdge(p, triangleList);
//
//    Vertex org = closestEdge.getOrigin();
//    Vertex dest = closestEdge.getDestination();
//
//    if (org.distsq(p) < dest.distsq(p)) {
//        return org;
//    }
//    else {
//        return dest;
//    }
//}

#endif
