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

using namespace std;

template <typename T>
// TODO kan deze niet gewoon erven van vector2?
class Vertex {
public:

	using PointType = Vector2<T>;

	Vertex(T _x, T _y){
		x = _x;
		y = _y;
		//neighbors = new list();
	}

	Vertex(const PointType &p){
		x = p.x;
		y = p.y;
		//neighbors = new list();
	}

	T distsq(const Vertex<T> &v){
		T dx = x - v.x;
		T dy = y - v.y;
		return dx * dx + dy * dy;
	}

	T distsq(const PointType &p){
		T dx = x - p.x;
		T dy = y - p.y;
		return dx * dx + dy * dy;
	}

	// standaard GS/BW walk naar de dichtste buur van p in de triangulatie
	Vertex *walkNN(const PointType p){
		Vertex *current = this;
		Vertex *closest;
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
