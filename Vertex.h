#ifndef VERTEX_H_
#define VERTEX_H_

#include <list>

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