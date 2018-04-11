#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <stdlib.h>  
#include <array>
#include <chrono>
#include <fstream>
#include <Windows.h> //windows header heeft sleep in milliseconds, usleep werkt niet op windows

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
	

	//std::cout << "Generating " << numberPoints << " random points" << std::endl;

	//std::ofstream file;
	//file.open ("metingen.csv");
	//int numReruns = 7;
	//int step = 5000;
	//file << "points, bw, hilbert, xsort" << std::endl;
	//for(int numberPoints = step; numberPoints <= 30*step; numberPoints += step){
	//	for(int k = 0; k < numReruns; k++){
	//		Delaunay_CGAL triangulator; // om een of andere reden blijft bw hangen als je niet steeds een nieuwe triangulator maakt
	//		std::vector<Point> points, hilbertCopy, xSortCopy;
	//		for(int j = 0; j < numberPoints; j++) {
	//			points.push_back(Point(RandomReal(0, 1), RandomReal(0, 1), 0)); //push_back voegt punt toe aan einde lijst
	//		}
	//		// zorg dat de triangulator geen structuur aanbrengt in de invoer van de volgende
	//		hilbertCopy = points;
	//		xSortCopy   = points;
	//		long int BWTime      = toMicroseconds(triangulator.stdBowyerWatson(points));
	//		long int hilbertTime = toMicroseconds(triangulator.hilbert(hilbertCopy));
	//		long int xSortTime   = toMicroseconds(triangulator.xSort(xSortCopy));

	//		file << numberPoints << ", " << BWTime << ", " << hilbertTime << ", " << xSortTime << std::endl;
	//		std::cout << numberPoints << ", " << BWTime << ", " << hilbertTime << ", " << xSortTime << std::endl;

	//		Sleep(4000);
 //           //usleep(4e6);
	//	}
	//}
	//file.close();

	Delaunay_CGAL triangulator;
	std::vector<Point> points;
	for(int i = 0; i < 10; i++){
		points.push_back(Point(RandomReal(0, 800), RandomReal(0, 600), 0)); //push_back voegt punt toe aan einde lijst
	}
	triangulator.triangulate(points);
	std::cout << "sucess\n";


	//auto int_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(triangulator.hilbert(points));

    //std::cout << "Triangulation took " << ((double) int_ns.count()) / 1e0 << "ns" << std::endl;

	return 0;
}
