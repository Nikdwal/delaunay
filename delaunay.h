#ifndef H_DELAUNAY
#define H_DELAUNAY

#include "vector2.h"
#include "edge.h"
#include "triangle.h"
#include "Vertex.h"
#include "NearestSearch.h"

#include <vector>
#include <algorithm>

class Delaunay
{
	public:
		
        /* Triangulate geeft een vector(lijst) terug met references
        * naar driehoeken.
        */
		const std::vector<Triangle>& triangulate(std::vector<Vertex> &vertices)
		{
			// Store the vertices localy
			//_vertices = vertices;

			// Determinate the super triangle
            float minX = vertices[0].x;
			float minY = vertices[0].y;
			float maxX = minX;
			float maxY = minY;

			for(std::size_t i = 0; i < vertices.size(); ++i) 
			{
				if (vertices[i].x < minX) minX = vertices[i].x;
		    	if (vertices[i].y < minY) minY = vertices[i].y;
		    	if (vertices[i].x > maxX) maxX = vertices[i].x;
		    	if (vertices[i].y > maxY) maxY = vertices[i].y;
			}
			
			float dx = maxX - minX;
			float dy = maxY - minY;
			float deltaMax = std::max(dx, dy);
			float midx = (minX + maxX) / 2.f;
			float midy = (minY + maxY) / 2.f;

            //construct 3 edges of supertriangle
			Vertex p1(midx - 20 * deltaMax, midy - deltaMax);
			Vertex p2(midx, midy + 20 * deltaMax);
			Vertex p3(midx + 20 * deltaMax, midy - deltaMax);

			//std::cout << "Super triangle " << std::endl << Triangle(p1, p2, p3) << std::endl;
			
			// Create a list of triangles, and add the supertriangle in it
			_triangles.push_back(Triangle(p1, p2, p3));

            //!! p is een reference!!
			for(auto p = begin(vertices); p != end(vertices); p++)
			{

				std::vector<Edge> polygon;

                //*p returns the value that is pointed to by pointer p because vertices = &vertices is a list of references to points!
                //p points to a vertex that has an x and a y coordinate
				Vertex v(*p);
                
                Vertex nearest = walkNN(v, _triangles);

                //TODO: maak dit efficienter met een wandeling + tree search
				for(auto t = begin(_triangles); t != end(_triangles); t++)
				{
                
                    if(t->circumCircleContains(v))
					{
                        //if circumcircle of triangle t contains v then t is a bad triangle (i.e. a triangle that should be adjusted/removed.
						t->isBad = true;
						polygon.push_back(t->e12);	
						polygon.push_back(t->e23);	
						polygon.push_back(t->e31);	
					}
					else
					{
						//std::cout << " does not contain " << *p << " in his circum center" << std::endl;
					}
				}

				_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [](Triangle &t){
					// TODO verwijder vertices uit de adjacency list van elk vormend punt
					return t.isBad;
				}), end(_triangles));

				for(auto e1 = begin(polygon); e1 != end(polygon); e1++)
				{
					for(auto e2 = begin(polygon); e2 != end(polygon); e2++)
					{
						if(e1 == e2)
							continue;
						
						if(*e1 == *e2)
						{
							e1->isBad = true;
							e2->isBad = true;	
						}
					}
				}

				polygon.erase(std::remove_if(begin(polygon), end(polygon), [](Edge &e){
					return e.isBad;
				}), end(polygon));

				for(auto e = begin(polygon); e != end(polygon); e++)
					// TODO: voeg nieuwe punten toe aan de adjacency lists van v, p1 en p2
					_triangles.push_back(Triangle(e->p1, e->p2, v));
			
			}

			_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](Triangle &t){
				// TODO verwijder vertices uit de adjacency list van elk vormend punt
				return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
			}), end(_triangles));

			for(auto t = begin(_triangles); t != end(_triangles); t++)
			{
				_edges.push_back(t->e12);
				_edges.push_back(t->e23);
				_edges.push_back(t->e31);
			} 

			return _triangles;
		}
		
		const std::vector<Triangle>& getTriangles() const { return _triangles; };
		const std::vector<Edge>& getEdges() const { return _edges; };
		const std::vector<Vertex>& getVertices() const { return _vertices; };

	private:
		std::vector<Triangle> _triangles;
		std::vector<Edge> _edges;
		std::vector<Vertex> _vertices;
};

#endif