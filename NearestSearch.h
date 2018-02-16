#ifndef NEARESTSEARCH_H_
#define NEARESTSEARCH_H_

#include <vector>

#include "Vertex.h"
#include "edge.h"
#include "triangle.h"

Edge getClosestEdge(const Vertex &p, const std::vector<Triangle> triangleList) {
    Triangle t = triangleList.at(0);   // TODO: middenste punt nemen als startpunt of eindpunt hilbert
    Edge e = t.getEdge();

    bool flag = true;
    while (flag) {
        if (p == e.getOrigin() || p == e.getDestination()) {
            return e;
        }
        else if (e.leftOf(p)) {
            e = e.getSym();
        }
        else if (!(t.getOrgNext(e)).leftOf(p)) {
            e = t.getOrgNext(e);
        }
        else if (!(t.getDestPrev(e)).leftOf(p)) {
            e = t.getDestPrev(e);
        }
        else {
            return e;
        }
    }
}

//TODO: Zie return by value of return by pointer
Vertex walkNN(const Vertex &p, const std::vector<Triangle> &triangleList) {

    Edge closestEdge = getClosestEdge(p, triangleList);

    Vertex org = closestEdge.getOrigin();
    Vertex dest = closestEdge.getDestination();

    if (org.distsq(p) < dest.distsq(p)) {
        return org;
    }
    else {
        return dest;
    }
}

//Triangle getContainingTriangle(Vertex v, Triangle t) {
//
//}

#endif