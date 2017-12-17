#ifndef H_DELAUNAY
#define H_DELAUNAY

#include "vector2.h"
#include "edge.h"
#include "triangle.h"
#include "Vertex.h"

#include <vector>
#include <algorithm>

template <class T>
class Delaunay
{
	public:
		using TriangleType = Triangle<T>;
		using EdgeType = Edge<T>;
		using InputType = Vector2<T>;
		using VertexType = Vertex<T>;
		
		const std::vector<TriangleType>& triangulate(std::vector<InputType> &vertices)
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
			VertexType p1(midx - 20 * deltaMax, midy - deltaMax);
			VertexType p2(midx, midy + 20 * deltaMax);
			VertexType p3(midx + 20 * deltaMax, midy - deltaMax);

			//std::cout << "Super triangle " << std::endl << Triangle(p1, p2, p3) << std::endl;
			
			// Create a list of triangles, and add the supertriangle in it
			_triangles.push_back(TriangleType(p1, p2, p3));

			for(auto p = begin(vertices); p != end(vertices); p++)
			{
				//std::cout << "Traitement du point " << *p << std::endl;
				//std::cout << "_triangles contains " << _triangles.size() << " elements" << std::endl;	

				std::vector<EdgeType> polygon;
				VertexType v(*p);

				// TODO: maak dit efficienter met een wandeling
				for(auto t = begin(_triangles); t != end(_triangles); t++)
				{
					//std::cout << "Processing " << std::endl << *t << std::endl;

					if(t->circumCircleContains(v))
					{
						//std::cout << "Pushing bad triangle " << *t << std::endl;
						t->isBad = true;
						polygon.push_back(t->e1);	
						polygon.push_back(t->e2);	
						polygon.push_back(t->e3);	
					}
					else
					{
						//std::cout << " does not contains " << *p << " in his circum center" << std::endl;
					}
				}

				_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [](TriangleType &t){
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

				polygon.erase(std::remove_if(begin(polygon), end(polygon), [](EdgeType &e){
					return e.isBad;
				}), end(polygon));

				for(auto e = begin(polygon); e != end(polygon); e++)
					// TODO: voeg nieuwe punten toe aan de adjacency lists van v, p1 en p2
					_triangles.push_back(TriangleType(e->p1, e->p2, v));
			
			}

			_triangles.erase(std::remove_if(begin(_triangles), end(_triangles), [p1, p2, p3](TriangleType &t){
				// TODO verwijder vertices uit de adjacency list van elk vormend punt
				return t.containsVertex(p1) || t.containsVertex(p2) || t.containsVertex(p3);
			}), end(_triangles));

			for(auto t = begin(_triangles); t != end(_triangles); t++)
			{
				_edges.push_back(t->e1);
				_edges.push_back(t->e2);
				_edges.push_back(t->e3);
			} 

			return _triangles;
		}
		
		const std::vector<TriangleType>& getTriangles() const { return _triangles; };
		const std::vector<EdgeType>& getEdges() const { return _edges; };
		const std::vector<InputType>& getVertices() const { return _vertices; };

	private:
		std::vector<TriangleType> _triangles;
		std::vector<EdgeType> _edges;
		std::vector<InputType> _vertices;
};

#endif
