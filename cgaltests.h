#include <iostream>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

#include <list>

using namespace std;

typedef CGAL::Simple_cartesian<double>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			Polyhedron;
typedef Polyhedron							PH;


void printPoint(const Point &p) {
	cout << "(" << p.x() << ", " << p.y() << ")\n";
}

void printEdge(PH::Halfedge_handle e){
	cout << "Start: ";
	printPoint(e->prev()->vertex()->point());
	cout << "End: ";
	printPoint(e->vertex()->point());
	cout << "\n";
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
    // cout << "Punten na wijziging: \n";
    PH::Halfedge_around_facet_const_circulator he = triangle->facet_begin();
    for (int i = 0; i < 3; i++) {
    	//printPoint(he->vertex()->point());
    	++he;
    }
    v->point() = p1;

    // zoek e1 en e2
    e = triangle->facet_begin();
    PH::Halfedge_handle h,g;
    for(int i=0; i < 3; i++){
    	if (e->opposite()->vertex()->point() == p2) h = e->opposite();
    	if (e->opposite()->vertex()->point() == p1) g = e->opposite();
    	e = e->next();
    }
//    printEdge(h);
//    printEdge(g);

    // voeg nieuw punt links toe
    PH::Halfedge_handle newHE = P.add_vertex_and_facet_to_border(h, g);
    newHE->vertex()->point() = Point(-5,5,0);

    //cout << P.size_of_halfedges() << "\n";
    // hier is er het juiste aantal bogen

    Point newPoint(5,10,0);
    // markeer binnenkant van e1, e2, e3

    // maak controlepunten
    list<PH::Halfedge_handle> controlEdges;
    PH::Halfedge_handle edge_with_ctl_pt;
    for (int i = 0; i < 3; i++) {
    	if (e->opposite()->is_border()){
    		// deze gaat controlepunten krijgen

    		PH::Halfedge_handle newHE = P.add_vertex_and_facet_to_border(e->opposite()->prev(), e->opposite());
    		controlEdges.push_back(newHE);
    		edge_with_ctl_pt = e->opposite();

    		printEdge(e);

    	}
		e = e->next();
	}
    int sz = controlEdges.size();


    // verwijder oorspronkelijke driehoek
    P.erase_facet(e);
    // nu is er het juiste aantal vlakken (=1 + 2 controlevlakken)
    cout << P.size_of_facets() << "\n";

    e = edge_with_ctl_pt->opposite(); // zou de rand van het gecreeerde gat moeten zijn

    newHE = P.add_vertex_and_facet_to_border(e, e->next()); // e->next() (=e2) is verbonden met punt ergens gekozen door cgal
    newHE->vertex()->point() = newPoint; // newhe = van e2->eindpunt naar newpoint
    e = newHE->next()->opposite(); // e = ep
    // Nu zijn er al 2 van de 3 bogen toegevoegd. Nog 3 - 2 = 1 te gaan.
    for(int i = 0; i < 3 - 2; i++){
    	e = P.add_facet_to_border(e->prev()->prev(), e);
    	e = e->opposite();
    }

    // verwijder de controlepunten
    while(!controlEdges.empty()){
    	P.erase_facet(controlEdges.front());
    	controlEdges.pop_front();
    }

    for(PH::Halfedge_handle e = P.halfedges_begin(); e != P.halfedges_end(); ++e){
    	cout << "Start: ";
    	printPoint(e->prev()->vertex()->point());
    	cout << "End: ";
    	printPoint(e->vertex()->point());
    	cout << "\n";
    }
    cout << P.size_of_halfedges();

}




