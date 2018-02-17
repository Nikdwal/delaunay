#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

using namespace std;

typedef CGAL::Simple_cartesian<double>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			Polyhedron;
typedef Polyhedron							PH;


void printPoint(const Point &p) {
	cout << "(" << p.x() << ", " << p.y() << ")\n";
}

void cgaltests() {
	// kijk of je een driehoek kan maken
    Polyhedron P;
    Point p1(0,0,0);
    Point p2(0,20,0);
    Point p3(10,10,0);
    PH::Halfedge_handle e = P.make_triangle(p1, p2, p3);
    PH::Facet_handle triangle = e->facet();

    // kijk of je een punt kunt verplaatsen
    PH::Vertex_handle v = e->vertex();
    v->point() = Point(-1, -1, 0);
    cout << "Punten na wijziging: \n";
    PH::Halfedge_around_facet_const_circulator he = triangle->facet_begin();
    for (int i = 0; i < 3; i++) {
    	printPoint(he->vertex()->point());
    	++he;
    }

    // kijk of je vlakken kunt verwijderen
    P.erase_facet(e);
    cout << P.size_of_vertices();


}




