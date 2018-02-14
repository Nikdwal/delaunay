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
#include "triangle.h"

using namespace std;

// TODO kan deze niet gewoon erven van vector2?
class Vertex {
public:

    Vertex(float _x, float _y) {
        x = _x;
        y = _y;
        //neighbors = new list();
    }

    Vertex(const Vertex &p) {
        x = p.x;
        y = p.y;
        //neighbors = new list();
    }

    float distsq(const Vertex &v) {
        float dx = x - v.x;
        float dy = y - v.y;
        return dx * dx + dy * dy;
    }

    //Vertex walkNN(const Vertex &p, const std::vector<Triangle> &triangleList) {

    //    Edge closestEdge = getClosestEdge(p, triangleList);
    //    
    //    Vertex &org = closestEdge.getOrigin();
    //    Vertex &dest = closestEdge.getDestination();

    //    if (org.distsq(p) < dest.distsq(p)) {
    //        return org;
    //    }
    //    else {
    //        return dest;
    //    }
    //}
    //

    ////@RETURN geeft ofwel de edge terug waarop p ligt , ofwel de edge die de driehoek waarin p ligt aan zijn linkerkant heeft liggen
    //Edge getClosestEdge(const Vertex &p, const std::vector<Triangle> triangleList) {
    //    Triangle t = begin(triangleList);   // TODO: middenste punt nemen als startpunt of eindpunt hilbert
    //    Edge *e = t.getEdge();

    //    Edge temp;

    //    bool flag = true;
    //    while (flag) {
    //        if (p == e->getOrigin() || p == e->getDestination()) {
    //            flag = false;
    //            return *e;
    //        }
    //        else if (rightOf(p, *e)) {
    //            temp = e->getSym();
    //            e = &temp;
    //        }
    //        else if (!rightOf(p, e->getOrgNext(t))) {
    //            temp = e->getOrgNext(t);
    //            e = &temp;
    //        }
    //        else if (!rightOf(p, e->getDestPrev(t))) {
    //            temp = e->getDestPrev(t);
    //            e = &temp;
    //        }
    //        else {
    //            flag = false;
    //            return *e;
    //        }
    //    }
    //}

    ////geeft true als punt p rechts van edge e ligt
    //bool rightOf(const Vertex p, const Edgetype &e) {
    //    Vertex origin = e.getOrigin();
    //    Vertex destination = e.getDestination();

    //    float det = p.x*(origin.y - destination.y) - origin.x*(p.y - destination.y) + destination.x*(p.y - origin.y);

    //    return det > 0;
    //}

    //Vertex getNearest(const Vertex p, const Triangle t) {
    //    float d1 = p.distsq(t.p1);
    //    float d2 = p.distsq(t.p2);
    //    float d3 = p.distsq(t.p3);
    //    float least = std::min({ d1, d2, d3 });
    //    if (least == d1) {
    //        return t.p1;
    //    }
    //    else if (least == d2) {
    //        return t.p2;
    //    }
    //    else {
    //        return t.p3
    //    }
    //}


	//// standaard GS/BW walk naar de dichtste buur van p in de triangulatie
	//Vertex walkNNstd(const Vertex p){
 //      
 //       float dmin, d;
	//	while(closest == NULL){
	//		closest = NULL;
	//		for (Vertex *neighbor : current->neighbors){
	//			d = neighbor->distsq(p);
	//			if (d < dmin){
	//				dmin = d;
	//				closest = neighbor;
	//			}
	//		}
	//		current = closest;
	//	}
	//	return current;
	//}

	float x, y;
	list<Vertex> neighbors;
};

std::ostream &operator << (std::ostream &str, Vertex const &point)
{
	return str << "Vertex x: " << point.x << " y: " << point.y;
}

// TODO: waarom zijn dit geen wijzers?
bool operator == (Vertex v1, Vertex v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y) && (v1.neighbors == v2.neighbors);
}


#endif /* VERTEX_H_ */