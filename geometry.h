#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef CGAL::Simple_cartesian<double>		Kernel;
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

	float ab = (p[0].x() * p[0].x()) + (p[0].y() * p[0].y());
	float cd = (p[1].x() * p[1].x()) + (p[1].y() * p[1].y());
	float ef = (p[2].x() * p[2].x()) + (p[2].y() * p[2].y());

	float circum_x = (ab * (p[2].y() - p[1].y()) + cd * (p[0].y() - p[2].y()) + ef * (p[1].y() - p[0].y())) / (p[0].x() * (p[2].y() - p[1].y()) + p[1].x() * (p[0].y() - p[2].y()) + p[2].x() * (p[1].y() - p[0].y())) / 2.f;
	float circum_y = (ab * (p[2].x() - p[1].x()) + cd * (p[0].x() - p[2].x()) + ef * (p[1].x() - p[0].x())) / (p[0].y() * (p[2].x() - p[1].x()) + p[1].y() * (p[0].x() - p[2].x()) + p[2].y() * (p[1].x() - p[0].x())) / 2.f;
	float circum_radius_sq = ((p[0].x() - circum_x) * (p[0].x() - circum_x)) + ((p[0].y() - circum_y) * (p[0].y() - circum_y));

	float dist_sq = ((v.x() - circum_x) * (v.x() - circum_x)) + ((v.y() - circum_y) * (v.y() - circum_y));
	return dist_sq <= circum_radius_sq;
}


#endif /* GEOMETRY_H_ */
