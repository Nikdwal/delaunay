#ifndef H_DELAUNAY_CGAL
#define H_DELAUNAY_CGAL

#include "geometry.h"
#include "NearestSearch.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef CGAL::Simple_cartesian<double>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			Polyhedron;
typedef Polyhedron							PH;

class Delaunay_CGAL
{
	public:

		bool eulerCheck(){
			int V = triangulation.size_of_vertices();
			int E = triangulation.size_of_halfedges() / 2;
			int F = 1 + triangulation.size_of_facets();
			if (V - E + F == 2)
				return true;
			std::cout << "Euler faalt.";
			return false;
		}

		bool isTriangulation(){
			if (!eulerCheck())
				return false;

			int numT = triangulation.size_of_facets();
			int i = 0;
			for(PH::Facet_handle f = triangulation.facets_begin(); i < numT; f++){
				if(! f->is_triangle()) {
					std::cout << "Geen triangulatie";
					return false;
				}
				i++;
			}
			return true;
		}

		bool isDelaunay(){
			if (!isTriangulation())
				return false;
			int numT = triangulation.size_of_facets();
			int i = 0;
			for(PH::Facet_handle f = triangulation.facets_begin(); i < numT; f++){
				for(PH::Vertex_handle v = triangulation.vertices_begin(); v != triangulation.vertices_end(); v++){
					if (f->halfedge()->vertex() == v || f->halfedge()->next()->vertex() == v || f->halfedge()->prev()->vertex() == v)
						continue;
					if (circumCircleContains(f, v->point())){
						std::cout << "Het punt (" << v->point().x() << ", " << v->point().y() << ") ligt in de driehoek bepaald door\n";
						PH::Halfedge_handle e = f->halfedge();
						for(int j = 0; j < 3; j++){
							std::cout << "(" << std::to_string(e->vertex()->point().x()) << ", " << std::to_string((e++)->vertex()->point().y()) << ")\n";
						}
						return false;
					}
				}
				i++;
			}
			return true;
		}

		bool containsControlPoints(){
			for(PH::Vertex_handle v = triangulation.vertices_begin(); v != triangulation.vertices_end(); v++){
				if(v->point().z() != 0) return true;
			}
			return false;
		}

		void print(){
			for(PH::Edge_iterator e = triangulation.edges_begin(); e != triangulation.edges_end(); e++){
				std::cout << eStr(e);
			}
	    	std::cout << "=====================================\n";
		}
		
		std::string eStr(PH::Halfedge_handle e){
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

		const Polyhedron& triangulate(std::vector<Point> &vertices)
		{
			triangulation.clear();

			// Determinate the super triangle
            float minX = vertices[0].x();
			float minY = vertices[0].y();
			float maxX = minX;
			float maxY = minY;

			for(std::size_t i = 0; i < vertices.size(); ++i) 
			{
				if (vertices[i].x() < minX) minX = vertices[i].x();
		    	if (vertices[i].y() < minY) minY = vertices[i].y();
		    	if (vertices[i].x() > maxX) maxX = vertices[i].x();
		    	if (vertices[i].y() > maxY) maxY = vertices[i].y();
			}
			
			float dx = maxX - minX;
			float dy = maxY - minY;
			float deltaMax = std::max(dx, dy);
			float midx = (minX + maxX) / 2.f;
			float midy = (minY + maxY) / 2.f;

            //construct 3 edges of supertriangle
			Point p1(midx - 20 * deltaMax, midy - deltaMax, 0);
			Point p2(midx, midy + 20 * deltaMax, 0);
			Point p3(midx + 20 * deltaMax, midy - deltaMax, 0);

			
			// Create a list of triangles, and add the supertriangle in it
			triangulation.make_triangle(p1, p2, p3);
			PH::Vertex_handle superVertices[3];
			PH::Vertex_iterator v_iter = triangulation.vertices_begin();
			for(int i = 0; i < 3; i++)
				superVertices[i] = v_iter++;

			// We kunnen te maken hebben met een vlak waarvan de normaal ofwel de eenheidsvector in
			// de z-richting is, ofwel in de tegengestelde richting.
			// De halfbogen draaien ccw rond deze normaal, en het is handiger om zeker te
			// zijn dat die in de positieve z-richting is.
			PH::Halfedge_handle internalEdge = triangulation.facets_begin()->halfedge();
			if(rightOf(internalEdge, internalEdge->next()->vertex()->point()))
				triangulation.inside_out();

			int numPoints=0;
			for(auto p = begin(vertices); p != end(vertices); p++)
			{

				// debug
				std::cout << ++numPoints << "\n";
				if(numPoints % 400 == 0 && containsControlPoints())
					throw std::runtime_error("Niet alle controlepunten zijn zorgvuldig verwijderd.");
				if(numPoints % 400 == 0 && ! isDelaunay()){
					throw std::runtime_error("Triangulatie is niet Delaunay.");
				}

				std::vector<PH::Halfedge_handle> polygon;
				std::vector<PH::Facet_handle> badTriangles;

				// debug
				PH::Halfedge_handle gs_edge = adjEdge(*p, triangulation);
				if(!circumCircleContains(gs_edge->facet(), *p))
					throw std::runtime_error("gs walk is fout");

				// TODO: maak dit efficienter met een wandeling + tree search
				for(PH::Facet_handle t = triangulation.facets_begin(); t != triangulation.facets_end(); t++)
				{
                
                    if(circumCircleContains(t, *p))
					{
                        //if circumcircle of triangle t contains v then t is a bad triangle (i.e. a triangle that should be adjusted/removed.
						badTriangles.push_back(t);
						PH::Halfedge_handle he = t->halfedge();
						for(int i = 0; i < 3; i++){
							polygon.push_back(he);
							he = he->next();
						}
					}
					else
					{
						//std::cout << " does not contain " << *p << " in his circum center" << std::endl;
					}
				}

				// markeer edges die wel en niet weg moeten
				std::vector<PH::Halfedge_handle> goodEdges;
				std::vector<PH::Halfedge_handle> badEdges;
				bool isBad;
				for(int i = 0; i < polygon.size(); i++){
					PH::Halfedge_handle e1 = polygon[i];
					isBad = false;
					for(int j = 0; j < polygon.size(); j++){
						PH::Halfedge_handle e2 = polygon[j];
						if(e1->opposite() == e2)
						{
							// alle bogen die via twee verschillende driehoeken gevonden worden, moeten volledig weg (samen met hun tweeling)
							isBad = true;
							// TODO: Moet je wel een lijst met bad-edges bijhouden? Want je kunt ook gewoon alle halfedges uit
							// de "slechte" driehoeken verwijderen.
							badEdges.push_back(e1);
						}
					}
					// alle bogen die maar via 1 driehoek gevonden worden, worden deel van een nieuwe driehoek die p zal bevatten
					if(!isBad)
						goodEdges.push_back(e1->opposite());
				}

				// Verwijder alle halfbogen van polygon uit de de triangulatie om zo een veelhoekig gat te maken.
				// Voor elke (half)boog die verwijderd wordt, moet je er eerst voor zorgen dat er geen punten verwijderd
				// worden door CGAL. Als je voor elke boog waarvan de tweeling een buitenrand is, een extra
				// vlak toevoegt, *denk ik* dat je veilig zit, maar het is goed mogelijk dat dit eigenlijk teveel werk is.
				std::vector<PH::Halfedge_handle> auxEdges;
				for(int i = 0; i < goodEdges.size(); i++){
					PH::Halfedge_handle e = goodEdges[i];
					if(! e->is_border())
						continue; // deze is niet in gevaar

					// deze boog is aangrenzend aan een andere boog op de rand. Normaal gezien ligt de vorige boog op de
					// rand, maar het zou kunnen (omwille van de volgorde van de bogen) dat je ook naar de volgende moet kijken
					PH::Halfedge_handle otherBorder = e->prev();
					if(!otherBorder->is_border())
						throw std::runtime_error("Fout in de implementatie.");
					PH::Halfedge_handle auxEdge = triangulation.add_vertex_and_facet_to_border(otherBorder, e);
					Point &v = auxEdge->vertex()->point();
					v = Point(v.x(), v.y(), 10);
					auxEdges.push_back(auxEdge);
				}
				// Verwijder nu effectief alle halfbogen die weg moeten
				for(int i = 0; i < badTriangles.size(); i++)
					triangulation.erase_facet(badTriangles[i]->halfedge());

				//print();


//				// Voeg de nieuwe driehoeken toe met een punt in p
				PH::Halfedge_handle he = goodEdges[0]->opposite(); // zou op de rand van het gat moeten liggen m.a.w. he->is_border()
//				PH::Halfedge_handle edgeToP = triangulation.add_vertex_and_facet_to_border(he->prev(), he)->opposite();
//				// dit verandert de waarde waar he naar wijst. he is nu een inwendige halfboog die vertrekt vanuit hetzelfde punt
//				// en landt in p (om een of andere reden)
//				edgeToP->vertex()->point() = *p; // he is nu deel van een driehoek
//				for(int i = 0; i < goodEdges.size() - 2; i++)
//					edgeToP = triangulation.add_vertex_and_facet_to_border(edgeToP->prev(), edgeToP->next())->opposite();

				PH::Halfedge_handle newHE = triangulation.add_vertex_and_facet_to_border(he, he->next()); // e->next() (=e2) is verbonden met punt ergens gekozen door cgal
				newHE->vertex()->point() = *p; // newhe = van e2->eindpunt naar newpoint
				he = newHE->next()->opposite(); // e = ep
				// Nu zijn er al 2 van de 3 bogen toegevoegd. Nog 3 - 2 = 1 te gaan.
				for(int i = 0; i < goodEdges.size() - 2; i++){
					he = triangulation.add_facet_to_border(he->prev()->prev(), he);
					he = he->opposite();
				}
				// nu is er enkel nog een driehoekig gat dat moet opgevuld worden (zonder bogen toe te voegen)
				triangulation.fill_hole(he);

				// verwijder wat je met de hulpbogen had toegevoegd
				for(int i = 0; i < auxEdges.size(); i++)
					triangulation.erase_facet(auxEdges[i]);

				// print();
			
			}

			// verwijder de super-triangle en alle driehoeken die met de hoekpunten hiervan verbonden zijn
			for(int i = 0; i < 3; i++){
				PH::Vertex_handle v = superVertices[i];
				// we weten dat v op de rand ligt
				int deg = v->degree();
				PH::Halfedge_around_vertex_circulator e = v->vertex_begin();
				while(!(e->is_border_edge() && prev(e)->is_border_edge())) e++;
				for(int i=0; i < deg - 1; i++)
					triangulation.erase_facet(e++);
			}

			return triangulation;
		}

	private:
		Polyhedron triangulation;
};

#endif
