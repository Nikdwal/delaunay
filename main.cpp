#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <stdlib.h>  
#include <array>
#include <chrono>
#include <fstream>

#include <SFML/Graphics.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef double								Real;
typedef CGAL::Simple_cartesian<Real>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			PH;
typedef std::chrono::high_resolution_clock	Clock;
typedef Clock::duration						Duration;

#include "delaunay_cgal.h"



/*
* !! X-AS STAAT VERTICAAL EN Y-AS STAAT HORIZONTAAL !!
*/

/*
* Genereert random getal tussen meegegeven waarden a en b 
* (Gaat er van uit dat a < b!)
*/
Real RandomReal(Real a, Real b) {
    Real random = (Real) rand() / RAND_MAX;
    Real diff = b - a;
    Real r = random * diff;
    return a + r;
}

long int toMicroseconds(Duration dt){
	return std::chrono::duration_cast<std::chrono::microseconds>(dt).count();
}

int main()
{


    //srand is PSEUD-RNG, moet als argument een bepaald getal meekrijgen (zelfde getal zal altijd zelfde 'random' getallen genereren 
    //--> door argument time(NULL) mee te geven wordt verzekerd dat het telkens andere getallen zijn)
	srand (time(NULL));
	

//	std::cout << "Generating " << numberPoints << " random points" << std::endl;

	std::ofstream file;
	file.open ("metingen.csv");
	int numReruns = 5;
	int step = 5000;
	file << "points, bw, hilbert, xsort" << std::endl;
	for(int numberPoints = step; numberPoints <= 3*step; numberPoints += step){
		for(int k = 0; k < numReruns; k++){
			Delaunay_CGAL triangulator; // om een of andere reden blijft bw hangen als je niet steeds een nieuwe triangulator maakt
			std::vector<Point> points, hilbertCopy, xSortCopy;
			for(int j = 0; j < numberPoints; j++) {
				points.push_back(Point(RandomReal(0, 800), RandomReal(0, 600), 0)); //push_back voegt punt toe aan einde lijst
			}
			// zorg dat de triangulator geen structuur aanbrengt in de invoer van de volgende
			hilbertCopy = points;
			xSortCopy   = points;
			long int BWTime      = toMicroseconds(triangulator.stdBowyerWatson(points));
			long int hilbertTime = toMicroseconds(triangulator.hilbert(hilbertCopy));
			long int xSortTime   = toMicroseconds(triangulator.xSort(xSortCopy));

			file << numberPoints << ", " << BWTime << ", " << hilbertTime << ", " << xSortTime << std::endl;
		}
	}
	file.close();

//	std::vector<Point> points;
//	for(int i = 0; i < 20000; i++){
//		points.push_back(Point(RandomReal(0, 800), RandomReal(0, 600), 0)); //push_back voegt punt toe aan einde lijst
//	}
//	std::cout << toMicroseconds(triangulator.stdBowyerWatson(points)) << std::endl;



//    points.push_back(Point(100.0, 200.0, 0));
//    points.push_back(Point(200.0, 400.0, 0));
//    points.push_back(Point(300.0, 100.0, 0));
//    points.push_back(Point(400.0, 500.0, 0));
//    points.push_back(Point(500.0, 300.0, 0));

//
//
//	auto int_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(triangulator.hilbert(points));
//
//    std::cout << "Triangulation took " << ((double) int_ns.count()) / 1e0 << "ns" << std::endl;

	


	return 0;
}
