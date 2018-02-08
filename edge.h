#ifndef H_EDGE
#define H_EDGE

#include "Vertex.h"

template <class T>
class Edge
{
	public:
		using VertexType = Vertex<T>;
        //using TriangleType = Triangle<T>;
		
		Edge(const VertexType &p1, const VertexType &p2) : p1(p1), p2(p2), isBad(false) {};
		Edge(const Edge &e) : p1(e.p1), p2(e.p2), isBad(false) {};

        //Vertex getOrigin() {
        //    return p1;
        //}

        //Vertex getDestination() {
        //    return p2;
        //}

        //geeft edge in tegengestelde richting terug
        //Edge getSym() {
        //    return Edge(p2, p1);
        //}

        //Edge getOrgNext(TriangleType t){
        //  return Edge(t.getFirst(), t.getThird());  
        //}

        //Edge getDestPrev(TriangleType t) {
        //  return Edge(t.getThird(), t.getSecond());
        //}

		VertexType p1;
		VertexType p2;

		bool isBad;
};

template <class T>
inline std::ostream &operator << (std::ostream &str, Edge<T> const &e)
{
	return str << "Edge " << e.p1 << ", " << e.p2;
}

template <class T>
inline bool operator == (const Edge<T> & e1, const Edge<T> & e2)
{
	return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
			(e1.p1 == e2.p2 && e1.p2 == e2.p1);
}

#endif