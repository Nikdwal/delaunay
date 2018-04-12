#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <math.h>
#include <stdlib.h>  
#include <array>
#include <chrono>
#include <fstream>
//#include <Windows.h> //windows header heeft sleep in milliseconds, usleep werkt niet op windows

#include <SFML/Graphics.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

typedef double								Real;
typedef CGAL::Simple_cartesian<Real>		Kernel;
typedef Kernel::Point_3						Point;
typedef CGAL::Polyhedron_3<Kernel>			PH;
typedef std::chrono::high_resolution_clock	Clock;
typedef Clock::duration						Duration;

using namespace std;

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

long int toNanoseconds(Duration dt){
	return std::chrono::duration_cast<std::chrono::nanoseconds>(dt).count();
}

int main()
{
    //srand is PSEUD-RNG, moet als argument een bepaald getal meekrijgen (zelfde getal zal altijd zelfde 'random' getallen genereren 
    //--> door argument time(NULL) mee te geven wordt verzekerd dat het telkens andere getallen zijn)
	srand (time(NULL));

//	std::cout << "Generating " << numberPoints << " random points" << std::endl;

	// TOTALE REKENTIJD
//	std::ofstream file;
//	file.open ("metingen.csv");
//	int numReruns = 7;
//	int step = 5000;
//	file << "points, bw, hilbert, xsort" << std::endl;
//	for(int numberPoints = step; numberPoints <= 30*step; numberPoints += step){
//		for(int k = 0; k < numReruns; k++){
//			Delaunay_CGAL triangulator; // om een of andere reden blijft bw hangen als je niet steeds een nieuwe triangulator maakt
//			std::vector<Point> points, hilbertCopy, xSortCopy;
//			for(int j = 0; j < numberPoints; j++) {
//				points.push_back(Point(RandomReal(0, 1), RandomReal(0, 1), 0)); //push_back voegt punt toe aan einde lijst
//			}
//			// zorg dat de triangulator geen structuur aanbrengt in de invoer van de volgende
//			hilbertCopy = points;
//			xSortCopy   = points;
//			long int BWTime      = toMicroseconds(triangulator.stdBowyerWatson(points));
//			long int hilbertTime = toMicroseconds(triangulator.hilbert(hilbertCopy));
//			long int xSortTime   = toMicroseconds(triangulator.xSort(xSortCopy));
//			file << numberPoints << ", " << BWTime << ", " << hilbertTime << ", " << xSortTime << std::endl;
//			std::cout << numberPoints << ", " << BWTime << ", " << hilbertTime << ", " << xSortTime << std::endl;
//
//			//usleep(4e6);
//		}
//	}
//	file.close();
//
//	// LENGTE VAN DE WANDELING
//	std::ofstream file;
//	file.open ("searchtimes.csv");
//	int numReruns = 7;
//	int step = 5000;
//	file << "points, bw, hilbert, xsort" << std::endl;
//	for(int numberPoints = step; numberPoints <= 30*step; numberPoints += step){
//		for(int k = 0; k < numReruns; k++){
//			Delaunay_CGAL triangulator; // om een of andere reden blijft bw hangen als je niet steeds een nieuwe triangulator maakt
//			std::vector<Point> points, hilbertCopy, xSortCopy;
//			for(int j = 0; j < numberPoints; j++) {
//				points.push_back(Point(RandomReal(0, 1), RandomReal(0, 1), 0)); //push_back voegt punt toe aan einde lijst
//			}
//			// zorg dat de triangulator geen structuur aanbrengt in de invoer van de volgende
//			hilbertCopy = points;
//			xSortCopy   = points;
//			triangulator.stdBowyerWatson(points);
//			int BWPath      = 0;//triangulator.getTotalPathLength() / numberPoints;
//			triangulator.hilbert(hilbertCopy);
//			int hilbertPath = triangulator.getTotalPathLength();// / numberPoints;
//			triangulator.hilbert2(xSortCopy); //TODO: terugzetten naar xsort
//			int xSortPath   = triangulator.getTotalPathLength();// / numberPoints;
//
//			file << numberPoints << ", " << BWPath << ", " << hilbertPath << ", " << xSortPath << std::endl;
//			std::cout << numberPoints << ", " << BWPath << ", " << hilbertPath << ", " << xSortPath << std::endl;
//
////			Sleep(4000);
//            //usleep(4e6);
//		}
//	}
//	file.close();
//
	// AANTAL VERWIJDERDE DRIEHOEKEN
	std::ofstream file;
	file.open("deletedTriangles.csv");
	int numReruns = 1;//7;
	int step = 5000;
	file << "points, bw, hilbert, xsort" << std::endl;
	for(int numberPoints = step; numberPoints <= 30*step; numberPoints += step){
		for(int k = 0; k < numReruns; k++){
			Delaunay_CGAL triangulator; // om een of andere reden blijft bw hangen als je niet steeds een nieuwe triangulator maakt
			std::vector<Point> points, hilbertCopy, xSortCopy;
			for(int j = 0; j < numberPoints; j++) {
				points.push_back(Point(RandomReal(0, 1), RandomReal(0, 1), 0)); //push_back voegt punt toe aan einde lijst
			}
			// zorg dat de triangulator geen structuur aanbrengt in de invoer van de volgende
			hilbertCopy = points;
			xSortCopy   = points;
//			triangulator.stdBowyerWatson(points);
			int BWPath      = triangulator.getNumDeletedTriangles();// / numberPoints;
//			triangulator.hilbert(hilbertCopy);
			int hilbertPath = triangulator.getNumDeletedTriangles();// / numberPoints;
			triangulator.hilbert2(xSortCopy); // TODO: xsort terugzetten
			int xSortPath   = triangulator.getNumDeletedTriangles();// / numberPoints;

			file << numberPoints << ", " << BWPath << ", " << hilbertPath << ", " << xSortPath << std::endl;
			std::cout << numberPoints << ", " << BWPath << ", " << hilbertPath << ", " << xSortPath << std::endl;

//			Sleep(4000);
            //usleep(4e6);
		}
	}
	file.close();

//	// METING KLEINSTE HOEK
//	std::ofstream file;
//	file.open ("cosSmallestAngle.csv");
//	int numReruns = 7;
//	int step = 5000;
//	file << "points, bw, hilbert, xsort" << std::endl;
//	for(int numberPoints = step; numberPoints <= 30*step; numberPoints += step){
//		for(int k = 0; k < numReruns; k++){
//			Delaunay_CGAL triangulator; // om een of andere reden blijft bw hangen als je niet steeds een nieuwe triangulator maakt
//			std::vector<Point> points, hilbertCopy, xSortCopy;
//			for(int j = 0; j < numberPoints; j++) {
//				points.push_back(Point(RandomReal(0, 1), RandomReal(0, 1), 0)); //push_back voegt punt toe aan einde lijst
//			}
//			// zorg dat de triangulator geen structuur aanbrengt in de invoer van de volgende
//			hilbertCopy = points;
//			xSortCopy   = points;
//			triangulator.stdBowyerWatson(points);
//			Real BWPath      = triangulator.getAvgCosSmallestAngle();
//			triangulator.hilbert(hilbertCopy);
//			Real hilbertPath = triangulator.getAvgCosSmallestAngle();
//			triangulator.xSort(xSortCopy);
//			Real xSortPath   = triangulator.getAvgCosSmallestAngle();
//
//			file << numberPoints << ", " << BWPath << ", " << hilbertPath << ", " << xSortPath << std::endl;
//			std::cout << numberPoints << ", " << BWPath << ", " << hilbertPath << ", " << xSortPath << std::endl;
//
////			Sleep(4000);
//            //usleep(4e6);
//		}
//	}
//	file.close();

//	int numberPoints = 50000;
//	Delaunay_CGAL triangulator; // om een of andere reden blijft bw hangen als je niet steeds een nieuwe triangulator maakt
//	std::vector<Point> points, hilbertCopy, xSortCopy;
//	for(int j = 0; j < numberPoints; j++) {
//		points.push_back(Point(RandomReal(0, 1), RandomReal(0, 1), 0)); //push_back voegt punt toe aan einde lijst
//	}
//	triangulator.xSort(points);
//	vector<Real> angle = triangulator.getCosMaxMin();
//	for(int i = 0; i < angle.size(); i++)
//		std::cout << acos(angle[i]) * 180 / 3.141592653589793238 << std::endl;



//	Delaunay_CGAL triangulator;
//	std::vector<Point> points;
//	for(int i = 0; i < 2000; i++){
//		points.push_back(Point(RandomReal(0, 800), RandomReal(0, 600), 0)); //push_back voegt punt toe aan einde lijst
//	}
//	triangulator.triangulate(points);
//	std::cout << "sucess\n";

//
//	auto int_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(triangulator.hilbert(points));
//
//    std::cout << "Triangulation took " << ((double) int_ns.count()) / 1e0 << "ns" << std::endl;

	return 0;
}
