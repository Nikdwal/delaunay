#ifndef H_DELAUNAY_CGAL
#define H_DELAUNAY_CGAL

#include "geometry.h"

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
			PH::Vertex_iterator v_iter;
			for(int i = 0; i < 3; i++)
				superVertices[i] = v_iter++;

			for(auto p = begin(vertices); p != end(vertices); p++)
			{

				std::vector<PH::Halfedge_handle> polygon;
				std::vector<PH::Facet_handle> badTriangles;


				// TODO: maak dit efficienter met een wandeling + tree search
				for(PH::Facet_handle t = triangulation.facets_begin(); t != triangulation.facets_begin(); t++)
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

				// TODO: verwijder "slechte" driehoeken

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
						// alle bogen die maar via 1 driehoek gevonden worden, worden deel van een nieuwe driehoek die p zal bevatten
						if(!isBad)
							goodEdges.push_back(e1->opposite());
					}
				}

				// Verwijder alle halfbogen van polygon uit de de triangulatie om zo een veelhoekig gat te maken.
				// Voor elke (half)boog die verwijderd wordt, moet je er eerst voor zorgen dat er geen punten verwijderd
				// worden door CGAL. Als je voor elke boog waarvan de tweeling een buitenrand is, een extra
				// vlak toevoegt, *denk ik* dat je veilig zit, maar het is goed mogelijk dat dit eigenlijk teveel werk is.
				std::vector<PH::Halfedge_handle> auxEdges;
				for(int i = 0; i < polygon.size(); i++){
					PH::Halfedge_handle e = auxEdges[i]->opposite();
					if(! e->is_border())
						continue; // deze is niet in gevaar

					// deze boog is aangrenzend aan een andere boog op de rand. Normaal gezien ligt de vorige boog op de
					// rand, maar het zou kunnen (omwille van de volgorde van de bogen) dat je ook naar de volgende moet kijken
					PH::Halfedge_handle otherBorder = e->prev();
					if(!otherBorder->is_border())
						throw std::runtime_error("Fout in de implementatie.");
					auxEdges.push_back(triangulation.add_vertex_and_facet_to_border(otherBorder, e));
				}
				// Verwijder nu effectief alle halfbogen die weg moeten
				for(int i = 0; i < badTriangles.size(); i++)
					triangulation.erase_facet(badTriangles[i]->halfedge());


				// Voeg de nieuwe driehoeken toe met een punt in p
				PH::Halfedge_handle he = goodEdges[0]->opposite(); // zou op de rand van het gat moeten liggen m.a.w. he->is_border()
				PH::Halfedge_handle edgeFromP = triangulation.add_vertex_and_facet_to_border(he->prev(), he)->opposite();
				edgeFromP->vertex()->point() = *p; // he is nu deel van een driehoek
				for(int i = 0; i < goodEdges.size() - 2; i++)
					edgeFromP = triangulation.add_vertex_and_facet_to_border(edgeFromP->prev(), edgeFromP->next())->opposite();

				// verwijder wat je met de hulpbogen had toegevoegd
				for(int i = 0; i < auxEdges.size(); i++)
					triangulation.erase_facet(auxEdges[i]);
			
			}

			// verwijder de super-triangle en alle driehoeken die met de hoekpunten hiervan verbonden zijn
			for(int i = 0; i < 3; i++){
				PH::Vertex_handle v = superVertices[i];
				PH::Halfedge_around_vertex_circulator incident_edge = v->vertex_begin();
				int deg = v->vertex_degree();
				for(int j = 0; j < deg; j++){
					triangulation.erase_facet(incident_edge);
					incident_edge++;
				}
			}

			return triangulation;
		}

	private:
		Polyhedron triangulation;
};

#endif
