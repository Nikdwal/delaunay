#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <stdlib.h>  
#include <array>
#include <chrono>

#include <SFML/Graphics.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef CGAL::Simple_cartesian<double>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			PH;

#include "delaunay_cgal.h"



/*
* !! X-AS STAAT VERTICAAL EN Y-AS STAAT HORIZONTAAL !!
*/

/*
* Genereert random getal tussen meegegeven waarden a en b 
* (Gaat er van uit dat a < b!)
*/
float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

int main()
{


    //srand is PSEUD-RNG, moet als argument een bepaald getal meekrijgen (zelfde getal zal altijd zelfde 'random' getallen genereren 
    //--> door argument time(NULL) mee te geven wordt verzekerd dat het telkens andere getallen zijn)
	srand (time(NULL));
	
    float numberPoints = 3000;
//    float numberPoints = roundf(RandomFloat(4, 40));

//	std::cout << "Generating " << numberPoints << " random points" << std::endl;

    /*
    * Genereer de punten
    */
	std::vector<Point> points; //std::vector is een dynamische lijst
	
    for(int i = 0; i < numberPoints; i++) {
		points.push_back(Point(RandomFloat(0, 800), RandomFloat(0, 600), 0)); //push_back voegt punt toe aan einde lijst
	}
    
    points.push_back(Point(100.0, 200.0, 0));
    points.push_back(Point(200.0, 400.0, 0));
    points.push_back(Point(300.0, 100.0, 0));
    points.push_back(Point(400.0, 500.0, 0));
    points.push_back(Point(500.0, 300.0, 0));

	Delaunay_CGAL triangulator;

    auto t1 = std::chrono::high_resolution_clock::now();
	PH tri = triangulator.triangulate(points);
    auto t2 = std::chrono::high_resolution_clock::now();

	std::cout << tri.size_of_facets() << " triangles generated\n";
	//std::vector<Edge> edges = triangulator.getEdges();
	
	std::cout << " ========= ";
	
	std::cout << "\nPoints : " << points.size() << std::endl;
	for(auto &p : points)
		std::cout << p << std::endl;
	
	if(points.size() != tri.size_of_vertices())
		throw std::runtime_error("Triangulated the wrong number of points.");

//	std::cout << "\nTriangles : " << tri.size_of_facets() << std::endl;
//	for(auto &t : triangles)
//		std::cout << t << std::endl;
//
//	std::cout << "\nEdges : " << edges.size() << std::endl;
//	for(auto &e : edges)
//		std::cout << e << std::endl;

    auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    std::cout << "Triangulation took " << int_ms.count() << "ms" << std::endl;

//    triangulator.print();

    /*
	// SFML window
    	sf::RenderWindow window(sf::VideoMode(800, 600), "Delaunay triangulation");

	// Transform each points of each vector as a rectangle
	std::vector<sf::RectangleShape*> squares;

	for(auto p = begin(points); p != end(points); p++) {
		sf::RectangleShape *c1 = new sf::RectangleShape(sf::Vector2f(4, 4));
		c1->setPosition(p->x(), p->y());
		squares.push_back(c1);
	}
	
	// Make the lines
	std::vector<std::array<sf::Vertex, 2> > lines;
	for(auto e = tri.halfedges_begin(); e != tri.halfedges_end(); e++) {
		lines.push_back({{
			sf::Vertex(sf::Vector2f(e->vertex()->point().x() + 2, e->vertex()->point().y() + 2)),
			sf::Vertex(sf::Vector2f(e->opposite()->vertex()->point().x() + 2, e->opposite()->vertex()->point().y()))
		}});
	}
 
	while (window.isOpen())
	{
	        sf::Event event;
	        while (window.pollEvent(event))
	        {
	            if (event.type == sf::Event::Closed)
	                window.close();
	        }
	
	        window.clear();
	
		// Draw the squares
		for(auto s = begin(squares); s != end(squares); s++) {
			window.draw(**s);
		}
	
		// Draw the lines
		for(auto l = begin(lines); l != end(lines); l++) {
			window.draw((*l).data(), 2, sf::Lines);
		}
	       	
		window.display();
	}
	*/
	


	return 0;
}
