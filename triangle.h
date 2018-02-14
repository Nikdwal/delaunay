#ifndef H_TRIANGLE
#define H_TRIANGLE

#include "Vertex.h"
#include "edge.h"

#include <assert.h>
#include <math.h>

class Triangle
{
	public:
			
		Triangle(const Vertex &_p1, const Vertex &_p2, const Vertex &_p3)
		:	p1(_p1), p2(_p2), p3(_p3),
			e1(_p1, _p2), e2(_p2, _p3), e3(_p3, _p1), isBad(false)
		{}
	
		bool containsVertex(const Vertex &v)
		{
			return p1 == v || p2 == v || p3 == v; 
		}
		
        Edge *getEdge() {
            return &e1;
        }

		// TODO: werk gewoon met de gekwadrateerde straal in plaats van de straal zelf
		bool circumCircleContains(const Vertex &v)
		{
			float ab = (p1.x * p1.x) + (p1.y * p1.y);
			float cd = (p2.x * p2.x) + (p2.y * p2.y);
			float ef = (p3.x * p3.x) + (p3.y * p3.y);

			float circum_x = (ab * (p3.y - p2.y) + cd * (p1.y - p3.y) + ef * (p2.y - p1.y)) / (p1.x * (p3.y - p2.y) + p2.x * (p1.y - p3.y) + p3.x * (p2.y - p1.y)) / 2.f;
			float circum_y = (ab * (p3.x - p2.x) + cd * (p1.x - p3.x) + ef * (p2.x - p1.x)) / (p1.y * (p3.x - p2.x) + p2.y * (p1.x - p3.x) + p3.y * (p2.x - p1.x)) / 2.f;
			float circum_radius = sqrtf(((p1.x - circum_x) * (p1.x - circum_x)) + ((p1.y - circum_y) * (p1.y - circum_y)));

			float dist = sqrtf(((v.x - circum_x) * (v.x - circum_x)) + ((v.y - circum_y) * (v.y - circum_y)));
			return dist <= circum_radius;
		}
	
		// standaard BW gegevensstructuren
		Vertex p1;
		Vertex p2;
		Vertex p3;
		Triangle* neighboringTriangles[3];

		Edge e1;				
		Edge e2;
		Edge e3;
		bool isBad;
};

inline std::ostream &operator << (std::ostream &str, const Triangle &t)
{
	return str << "Triangle:" << std::endl << "\t" << t.p1 << std::endl << "\t" << t.p2 << std::endl << "\t" << t.p3 << std::endl << "\t" << t.e1 << std::endl << "\t" << t.e2 << std::endl << "\t" << t.e3 << std::endl;
		
}

inline bool operator == (const Triangle &t1, const Triangle &t2)
{
	return	(t1.p1 == t2.p1 || t1.p1 == t2.p2 || t1.p1 == t2.p3) &&
			(t1.p2 == t2.p1 || t1.p2 == t2.p2 || t1.p2 == t2.p3) && 
			(t1.p3 == t2.p1 || t1.p3 == t2.p2 || t1.p3 == t2.p3);
}


#endif
