#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef double								Real;
typedef CGAL::Simple_cartesian<Real>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			PH;




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
	if(dist_sq <= circum_radius_sq) std::cout << sqrtf(circum_radius_sq) - sqrtf(dist_sq) << "\n";

	return dist_sq <= circum_radius_sq;
}

//geeft true als punt p rechts van edge e ligt
bool rightOf(PH::Halfedge_const_handle e, const PH::Point &p) {
	const Point &v = e->opposite()->vertex()->point();
	const Point &w = e->vertex()->point();
    Real det = (p.x() - v.x())*(w.y() - v.y()) - (p.y() - v.y())*(w.x() - v.x());
    return det > 0;
}

PH::Halfedge_handle origNext(PH::Halfedge_handle e){
	PH::Halfedge_around_vertex_circulator iter = e->opposite()->vertex_begin();
	return prev(iter)->opposite();
}

PH::Halfedge_handle destPrev(PH::Halfedge_handle e){
	PH::Halfedge_around_vertex_circulator iter = e->vertex_begin();
	return next(iter);
}



#endif /* GEOMETRY_H_ */
