#ifndef H_VECTOR2
#define H_VECTOR2

#include <iostream>
#include <cmath>

template <typename T>
class Vector2
{
	public:
        T x;
        T y;

		//
		// Constructors
		//

		Vector2()
		{
			x = 0;
			y = 0;
		}

		Vector2(T xp, T yp) 
		{
			x = xp;
			y = yp;
		}

		Vector2(const Vector2 &v)
		{
			x = v.x;
			y = v.y;
		}

		void set(const Vector2 &v)
		{
			x = v.x;
			y = v.y;
		}

		//
		// Operations
		//	
        //distance from this vector to the given vector
		T dist2(const Vector2 &v)
		{
			T dx = x - v.x;
			T dy = y - v.y;
			return dx * dx + dy * dy;	
		}

		float dist(const Vector2 &v)
		{
			return sqrtf(dist2(v));
		}
};

template<typename T>
std::ostream &operator << (std::ostream &str, Vector2<T> const &point) 
{
	return str << "Point x: " << point.x << " y: " << point.y;
}

template<typename T>
bool operator == (Vector2<T> v1, Vector2<T> v2)
{
	return (v1.x == v2.x) && (v1.y == v2.y);
}
	
#endif