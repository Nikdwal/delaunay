/*
*   De geometry.h header file bevat methodes die bewerkingen uitvoeren op verschillende combinaties 
*   van vertices, edges en triangles.
*/
#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

typedef double								Real;
typedef CGAL::Simple_cartesian<Real>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			PH;

using namespace CGAL;

/*
*   Neemt een triangle en een punt v en kijkt na of de omschreven cirkel rond triangle het punt v bevat.
*   Als dit zo is geeft deze methode true terug, anders false.
*/
bool circumCircleContains(PH::Facet_const_handle triangle, const Point &v) {
	// TODO: kon dit ook niet met een determinant? Zie liu/snoeyink
	PH::Halfedge_const_handle e = triangle->halfedge();
	Point p[3];
	for(int i = 0; i < 3; i++){
		p[i] = e->vertex()->point();
		e = e->next();
	}

	Real ab = (p[0].x() * p[0].x()) + (p[0].y() * p[0].y());
	Real cd = (p[1].x() * p[1].x()) + (p[1].y() * p[1].y());
	Real ef = (p[2].x() * p[2].x()) + (p[2].y() * p[2].y());

	Real circum_x = (ab * (p[2].y() - p[1].y()) + cd * (p[0].y() - p[2].y()) + ef * (p[1].y() - p[0].y())) / (p[0].x() * (p[2].y() - p[1].y()) + p[1].x() * (p[0].y() - p[2].y()) + p[2].x() * (p[1].y() - p[0].y())) / 2.f;
	Real circum_y = (ab * (p[2].x() - p[1].x()) + cd * (p[0].x() - p[2].x()) + ef * (p[1].x() - p[0].x())) / (p[0].y() * (p[2].x() - p[1].x()) + p[1].y() * (p[0].x() - p[2].x()) + p[2].y() * (p[1].x() - p[0].x())) / 2.f;
	Real circum_radius_sq = ((p[0].x() - circum_x) * (p[0].x() - circum_x)) + ((p[0].y() - circum_y) * (p[0].y() - circum_y));

	Real dist_sq = ((v.x() - circum_x) * (v.x() - circum_x)) + ((v.y() - circum_y) * (v.y() - circum_y));

	// debug
    //if(dist_sq <= circum_radius_sq) std::cout << sqrtf(circum_radius_sq) - sqrtf(dist_sq) << "\n";

	return dist_sq <= circum_radius_sq;
}

/*
*   Neemt een punt p en een halfedge e en kijkt na of p rechts van e ligt door gebruik te maken van de determinant.
*   Als p rechts van e ligt dan geeft deze methode true terug, anders false.
*/
//geeft true als punt p rechts van edge e ligt
bool rightOf(PH::Halfedge_const_handle e, const PH::Point &p) {
	const Point &v = e->opposite()->vertex()->point();
	const Point &w = e->vertex()->point();
    Real det = (p.x() - v.x())*(w.y() - v.y()) - (p.y() - v.y())*(w.x() - v.x());
    return det > 0;
}

/*
*   Neemt een halfedge, berekent de edge die counterclockwise ligt vanaf de origin van die halfedge.
*   Hiervoor wordt een circulator gebruikt die voorzien wordt door de CGAL library.
*   Geeft de gevonden halfedge terug.
*/
PH::Halfedge_handle origNext(PH::Halfedge_handle e){
	PH::Halfedge_around_vertex_circulator iter = e->opposite()->vertex_begin();
	return prev(iter)->opposite();
}

/*
*   Neemt een halfedge, berekent de edge die clockwise ligt vanaf de destination van die halfedge.
*   Hiervoor wordt een circulator gebruikt die voorzien wordt door de CGAL library.
*   Geeft de gevonden halfedge terug.
*/
PH::Halfedge_handle destPrev(PH::Halfedge_handle e){
	PH::Halfedge_around_vertex_circulator iter = e->vertex_begin();
	return next(iter);
}

/**
 * Cosinus van de kleinste hoek van een driehoek
 */
Real cosSmallestAngle(PH::Facet_handle triangle){
	PH::Halfedge_handle e = triangle->halfedge();
	Point A = e->vertex()->point();
	Point B = e->next()->vertex()->point();
	Point C = e->prev()->vertex()->point();
	Real dists[3] = {squared_distance(A,B), squared_distance(B,C), squared_distance(A, C)};

	// de kleinste hoek staat tegenover de kortste zijde
	int argmin = 0;
	for(int i = 1; i < 3; i++)
		if (dists[i] < dists[argmin]) argmin = i;

	Real asq = dists[argmin];
	Real bsq = dists[(argmin+1)%3];
	Real csq = dists[(argmin+2)%3];
	// cosinusregel
	return (bsq + csq - asq) / (2 * sqrt(bsq * csq));
}

/**
 * De cosinus van de kleinste hoek, gemeten over alle driehoeken die een hoekpunt hebben in het gegeven punt
 */
Real cosSmallestAngle(PH::Vertex_handle vertex){
	// We weten dat de kleinste hoek ten hoogste 60 graden is, dus de cosinus ligt tussen 0.5 en 1.
	// De kleinste hoek heeft de grootste cosinus.
	Real cos = 0;
	Real cos2;
	PH::Halfedge_around_vertex_circulator circulator = vertex->vertex_begin();
	int i = 0;
	for(PH::Halfedge_handle edge = circulator; i++ < vertex->degree(); edge++){
		cos2 = cosSmallestAngle(edge->facet());
		if(cos2 > cos)
			cos = cos2;
	}
	return cos;
}

/**
 * De cosinus van het maximum over alle aangrenzende driehoeken van de kleinste hoek van die driehoek.
 * Deze hoek zegt of er in een toegevoegd punt ook "goede" Delaunay-driehoeken zijn.
 */
Real cosMaxMin(PH::Vertex_handle vertex){
	// We weten dat de kleinste hoek ten hoogste 60 graden is, dus de cosinus ligt tussen 0.5 en 1.
	// De grootste hoek heeft de kleinste cosinus.
	Real cos = 1;
	Real cos2;
	PH::Halfedge_around_vertex_circulator circulator = vertex->vertex_begin();
	int i = 0;
	for(PH::Halfedge_handle edge = circulator; i++ < vertex->degree(); edge++){
		cos2 = cosSmallestAngle(edge->facet());
		if(cos2 < cos)
			cos = cos2;
	}
	return cos;
}

Real length(PH::Halfedge_handle e){
	return sqrt(CGAL::squared_distance(e->vertex()->point(), e->opposite()->vertex()->point()));
}


#endif /* GEOMETRY_H_ */
