/*
 * Vertex.h
 *
 *  Created on: 15 Dec 2017
 *      Author: nick
 */

#ifndef VERTEX_H_
#define VERTEX_H_

#include <list>
#include "vector2.h"
//#include "edge.h"
//#include "triangle.h"

using namespace std;

template <typename T>
// TODO kan deze niet gewoon erven van vector2?
class Vertex {
public:

    using PointType = Vector2<T>;
    //using TriangleType = Triangle<T>;
    //using EdgeType = Edge<T>;

    Vertex(T _x, T _y) {
        x = _x;
        y = _y;
        //neighbors = new list();
    }

    Vertex(const PointType &p) {
        x = p.x;
        y = p.y;
        //neighbors = new list();
    }

    T distsq(const Vertex<T> &v) {
        T dx = x - v.x;
        T dy = y - v.y;
        return dx * dx + dy * dy;
    }

    T distsq(const PointType &p) {
        T dx = x - p.x;
        T dy = y - p.y;
        return dx * dx + dy * dy;
    }

    //@RETURN   geeft ofwel de edge terug waarop p ligt , ofwel de edge die de driehoek waarin p ligt aan zijn linkerkant heeft liggen
    /*EdgeType getClosestEdge(const Vertex<T> p, const std::vector<TriangleType> triangleList) {
        TriangleType t = begin(triangleList);
        EdgeType e = t.getEdge();

        bool flag = true;
        while (flag) {
            if (p == e.getOrigin() || p == e.getDestination()) {
                flag = false;
                return e;
            }
            else if (RightOf(p, e)) {
                e = e.getSym();
            }
            else if (!RightOf(p, e.getOrgNext(t))) {
                e = e.getOrgNext(t);
            }
            else if (!RightOf(p, e.getDestPrev(t))) {
                e = e.getDestPrev(t);
            }
            else {
                flag = false;
                return e;
            }
        }
    }*/

    /*Vertex getNearest(const Vertex<T> p, const TriangleType t) {
        T d1 = p.distsq(t.p1);
        T d2 = p.distsq(t.p2);
        T d3 = p.distsq(t.p3);
        T least = std::min({ d1, d2, d3 });
        if (least == d1) {
            return t.p1;
        }
        else if (least == d2) {
            return t.p2;
        }
        else {
            return t.p3
        }
    }*/

   /* Vertex walkNN(const Vertex<T> p, const std::vector<TriangleType> triangleList) {

        EdgeType closestEdge = getClosestEdge(p, triangleList);
        TriangleType t = closestEdge.getLeftTriangle();             //TODO: implementeer getLeftTriangle()
        Vertex nearest = getNearest(p, t);

        return nearest;
    }*/

	// standaard GS/BW walk naar de dichtste buur van p in de triangulatie
	Vertex walkNN(const Vertex<T> p){
       
        T dmin, d;
		while(closest == NULL){
			closest = NULL;
			for (Vertex *neighbor : current->neighbors){
				d = neighbor->distsq(p);
				if (d < dmin){
					dmin = d;
					closest = neighbor;
				}
			}
			current = closest;
		}
		return current;
	}

	T x, y;
	list<Vertex<T>> neighbors;
};

template<typename T>
std::ostream &operator << (std::ostream &str, Vertex<T> const &point)
{
	return str << "Vertex x: " << point.x << " y: " << point.y;
}

template<typename T>
// TODO: waarom zijn dit geen wijzers?
bool operator == (Vertex<T> v1, Vertex<T> v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.neighbors == v2.neighbors);
}


#endif /* VERTEX_H_ */
