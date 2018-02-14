#ifndef H_EDGE
#define H_EDGE

#include "Vertex.h"

class Triangle;
class Edge
{
	public:

		Edge(Vertex &p1, Vertex &p2) 
            : p1(p1), p2(p2), isBad(false) {};
		Edge(const Edge &e) : p1(e.p1), p2(e.p2), isBad(false) {};

        Vertex& getOrigin() {
            return p1;
        }

        Vertex& getDestination() {
            return p2;
        }

        //geeft edge in tegengestelde richting terug
        Edge getSym() {
            return Edge(p2, p1);
        }

        //Edge getOrgNext(Triangle t){
        //  //return Edge(t.getFirst(), t.getThird()); 
        //    return NULL;
        //}

        //Edge getDestPrev(Triangle t) {
        //  //return Edge(t.getThird(), t.getSecond());
        //    return NULL;
        //}

        Vertex p1;
        Vertex p2;
		bool isBad;
};

inline std::ostream &operator << (std::ostream &str, Edge const &e)
{
	return str << "Edge " << e.p1 << ", " << e.p2;
}

inline bool operator == (const Edge & e1, const Edge & e2)
{
	return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
			(e1.p1 == e2.p2 && e1.p2 == e2.p1);
}

#endif