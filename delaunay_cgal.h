#ifndef H_DELAUNAY_CGAL
#define H_DELAUNAY_CGAL

#include "geometry.h"
#include "NearestSearch.h"

//#include <Windows.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/hilbert_sort.h>
#include <numeric>

typedef double								Real;
typedef CGAL::Simple_cartesian<Real>		Kernel;
typedef Kernel::Point_3						Point;
typedef Kernel::Point_2						Point2;
typedef CGAL::Polyhedron_3<Kernel>			Polyhedron;
typedef Polyhedron							PH;
typedef std::chrono::high_resolution_clock	Clock;

using namespace std;

class Delaunay_CGAL
{
	public:

        /*
        *   kijkt na of de Euler-formule klopt voor de triangulatie.
        */
		bool eulerCheck(){
			int V = triangulation.size_of_vertices();
			int E = triangulation.size_of_halfedges() / 2;
			int F = 1 + triangulation.size_of_facets();
			return V - E + F == 2;
		}

        /*
        *   Kijkt na of de variabele triangulation wel degelijk een triangulatie is door de Euler-formule te controleren
        *   en te controleren of elke facet in de triangulatie wel een driehoek is.
        */
		bool isTriangulation(){
			if (!eulerCheck())
				return false;

			int numT = triangulation.size_of_facets();
			int i = 0;
			for(PH::Facet_handle f = triangulation.facets_begin(); i < numT; f++){
				if(! f->is_triangle()) {
					return false;
				}
				i++;
			}
			return true;
		}

        /*
        *   Kijkt na of de triangulatie delaunay is door te kijken of de omschreven cirkel rond elke driehoek in de 
        *   triangulatie leeg is.
        */
		bool isDelaunay(){
			if (!isTriangulation())
				return false;
			int numT = triangulation.size_of_facets();
			int i = 0;
			for(PH::Facet_handle f = triangulation.facets_begin(); i < numT; f++){
				for(PH::Vertex_handle v = triangulation.vertices_begin(); v != triangulation.vertices_end(); v++){
					if (f->halfedge()->vertex() == v || f->halfedge()->next()->vertex() == v || f->halfedge()->prev()->vertex() == v)
						continue;
					if (circumCircleContains(f, v->point())){
						std::cout << "Het punt (" << v->point().x() << ", " << v->point().y() << ") ligt in de driehoek bepaald door\n";
						PH::Halfedge_handle e = f->halfedge();
						for(int j = 0; j < 3; j++){
							// std::cout << "(" << std::to_string(e->vertex()->point().x()) << ", " << std::to_string((e++)->vertex()->point().y()) << ")\n";
						}
						return false;
					}
				}
				i++;
			}
			return true;
		}

        /*
        *   Kijkt na of de triangulatie nog controlepunten bevat.
        */
		bool containsControlPoints(){
			for(PH::Vertex_handle v = triangulation.vertices_begin(); v != triangulation.vertices_end(); v++){
				if(v->point().z() != 0) return true;
			}
			return false;
		}

        /*
        *   Geeft de triangulatie weer als een string van vertices en edges.
        */
		void print(){
			for(PH::Edge_iterator e = triangulation.edges_begin(); e != triangulation.edges_end(); e++){
				std::cout << eStr(e);
			}
	    	std::cout << "=====================================\n";
		}
		
		int getTotalPathLength(){
			return totalPathLength;
		}

		int getNumDeletedTriangles(){
			return numDeletedTriangles;
		}

		// Het gemiddelde over alle vertices van {cosSmallestAngle(vertex), op het moment dat een vertex wordt toegevoed}.
		Real getAvgSmallestAngle(){
			Real sum = 0;
			for(int i = 0; i < cosineSmallestAngle.size(); i++)
				sum += acos(cosineSmallestAngle[i]);
			return sum / cosineSmallestAngle.size();
			//return std::accumulate(cosineSmallestAngle.begin(), cosineSmallestAngle.end(), 0) / triangulation.size_of_vertices();
		}

		// {cosSmallestAngle(vertex), op het moment dat een vertex wordt toegevoed}
		vector<Real> getCosSmallestAngle(){
			return cosineSmallestAngle;
		}

		// {cosMaxMin(vertex), op het moment dat een vertex wordt toegevoed}
		vector<Real> getCosMaxMin(){
			return cosineMaxMin;
		}

        /*
        *   Print edges af.
        */
		std::string eStr(PH::Halfedge_handle e){
			std::string s = "";
			s += "Start: (";
			s += std::to_string(e->opposite()->vertex()->point().x());
			s += ", ";
			s += std::to_string(e->opposite()->vertex()->point().y());
			s += ", ";
			s += std::to_string(e->opposite()->vertex()->point().z());
			s +=")\nEnd: (";
			s += std::to_string(e->vertex()->point().x());
			s += ", ";
			s += std::to_string(e->vertex()->point().y());
			s += ", ";
			s += std::to_string(e->vertex()->point().z());
			s += ")\n\n";
			return s;
		}

        /*
        *   Print driehoeken af als combinatie van edges.
        */
		std::string triStr(PH::Facet_handle t){
			std::string s = "";
			PH::Halfedge_handle e = t->halfedge();
			for(int i = 0; i < 3; i ++){
				s += eStr(e);
				e = e->next();
			}
		}

        /*
        *   Neemt vector van punten en geeft de delaunay triangulatie van deze punten terug.
        *   De manier waarop de triangulatie gevormd wordt staat op de eerste lijn code van deze methode.
        *       --> stdBowyerWatson = algoritme waarin de Bowyer-Watson procedure uitgevoerd wordt
        *       --> hilbert = triangulatie wordt gevonden door eerst de punten te sorteren langs een hilbertcurve
        *       --> xSort = punten worden gesorteerd volgens coordinaat langs x-as
        *       --> xSortRightMost = neemt meest rechtse punt langs de x-as en begint daarin triangulatie
        */
		PH triangulate(std::vector<Point> &points){
			stdBowyerWatson(points);
			return triangulation;
		}

        /*
        *   Neemt vector van punten en vormt de delaunay triangulatie van deze punten door middel van het 
        *   Bowyer-Watson algoritme.
        *   Geeft de tijd terug (in nanoseconden) dat het algoritme nodig had om de triangulatie te berekenen.
        */
		Clock::duration stdBowyerWatson(std::vector<Point> &points){
			preprocessingTime = Duration::zero();
			Clock::time_point tik = Clock::now();

			initialize(points);
			Point midCoordinates = Point(midx, midy, 0);
			PH::Halfedge_handle startEdge = triangulation.halfedges_begin();
			Point &startPoint = startEdge->vertex()->point();
			Real minDist = CGAL::squared_distance(startPoint, midCoordinates);

			for(auto p = begin(points); p != end(points); p++){
				PH::Vertex_handle addedVertex = addVertex(*p, startEdge);
				Real dist = CGAL::squared_distance(addedVertex->point(), midCoordinates);
				if(dist < minDist){
					minDist = dist;
					startEdge = addedVertex->halfedge();
				}
			}
			finish();

			Clock::time_point tak = Clock::now();
			return tak - tik;
		}

        /*
        *   Neemt vector van punten en gaat deze punten sorteren volgens een hilbertcurve en roept vervolgens iteratief
        *   de addVertex()-methode op om de triangulatie te berekenen.
        *   Geeft de tijd terug (in nanoseconden) dat het algoritme nodig had om de triangulatie te berekenen.
        */
		Clock::duration hilbert(std::vector<Point> &points){

			// Zuiver overhead omwille van conversie Point2<->Point3. Dit moet je niet timen, want dit
			// komt gewoon door het gebruik van de libraries
			std::vector<Point2> points2;
			for(auto p = begin(points); p != end(points); p++)
				points2.push_back(Point2(p->x(), p->y()));

			Clock::time_point tik = Clock::now();
			CGAL::hilbert_sort(points2.begin(), points2.end());
			Clock::time_point tak = Clock::now();

			preprocessingTime = tak - tik;

			//std::vector<Point> points3;
			for(int i = 0; i < points.size(); i++)
				points[i] = Point(points2[i].x(), points2[i].y(), 0);

			Clock::time_point tok = Clock::now();
			initialize(points);
			PH::Halfedge_handle startEdge = triangulation.halfedges_begin();
			for(auto p = begin(points); p != end(points); p++)
				startEdge = addVertex(*p, startEdge)->halfedge();

			finish();
			Clock::time_point tek = Clock::now();
			return (tak - tik) + (tek - tok);
		}

		Clock::duration hilbertControlPts(std::vector<Point> &points, Real percentageControlPoints){
			int cutoffPoint = percentageControlPoints * points.size();

			std::vector<Point2> points2;
			for(auto p = begin(points); p != end(points); p++)
				points2.push_back(Point2(p->x(), p->y()));

			Clock::time_point tik = Clock::now();
			CGAL::hilbert_sort(points2.begin(), points2.begin() + cutoffPoint);
			CGAL::hilbert_sort(points2.begin() + cutoffPoint, points2.end());
			Clock::time_point tak = Clock::now();

			preprocessingTime = tak - tik;

			// zorg dat het het einde van de eerste hilbertcurve overgaat in het begin van de tweede
			std::reverse(points2.begin(), points2.begin() + cutoffPoint);

			for(int i = 0; i < points.size(); i++)
				points[i] = Point(points2[i].x(), points2[i].y(), 0);

			Clock::time_point tok = Clock::now();
			initialize(points);
			PH::Halfedge_handle startEdge = triangulation.halfedges_begin();
			for(auto p = begin(points); p != end(points); p++)
				startEdge = addVertex(*p, startEdge)->halfedge();

			finish();
			Clock::time_point tek = Clock::now();
			return (tak - tik) + (tek - tok);
		}

        /*
        *   Neemt vector van punten en gaat deze punten sorteren volgens de x-as en roept vervolgens iteratief
        *   de addVertex()-methode op om de triangulatie te berekenen.
        *   Geeft de tijd terug (in nanoseconden) dat het algoritme nodig had om de triangulatie te berekenen.
        */
		Clock::duration xSort(std::vector<Point> &points){
			struct comparator {
			  bool operator() (Point &v, Point &w) { return (v.x() < w.x());}
			} compare;


			Clock::time_point tik = Clock::now();
			std::sort(points.begin(), points.end(), compare);
			Clock::time_point tak = Clock::now();
			preprocessingTime = tak - tik;

			initialize(points);
			PH::Halfedge_handle startEdge = triangulation.halfedges_begin();
			for(auto p = begin(points); p != end(points); p++)
				startEdge = addVertex(*p, startEdge)->halfedge();

			finish();

			Clock::time_point tok = Clock::now();
			return tok - tik;
		}

        /*
        *   Neemt vector van punten en gaat deze punten sorteren volgens de x-as en roept vervolgens iteratief
        *   de addVertex()-methode, beginnende van het punt dat het meest rechts ligt van de initi�le supertriangle,
        *   om de triangulatie te berekenen. 
        *   Geeft de tijd terug (in nanoseconden) dat het algoritme nodig had om de triangulatie te berekenen.
        */
		Clock::duration xSortRightmost(std::vector<Point> &points){
			struct comparator {
			  bool operator() (Point &v, Point &w) { return (v.x() < w.x());}
			} compare;

			Clock::time_point tik = Clock::now();

			std::sort(points.begin(), points.end(), compare);

			initialize(points);

			Real maxX = 0;
			PH::Vertex_handle maxV;
			for(int i = 0; i < 3; i++){
				if(superVertices[i]->point().x() > maxX){
					maxX = superVertices[i]->point().x();
					maxV = superVertices[i];
				}
			}

			PH::Halfedge_handle startEdge = triangulation.halfedges_begin();
			for(auto p = begin(points); p != end(points); p++)
				// neem steeds een boog verbonden met het meest rechtse punt van de superdriehoek
				addVertex(*p, maxV->halfedge());

			finish();

			Clock::time_point tak = Clock::now();
			return tak - tik;
		}

		/*  
        *   Neemt een vector van punten en initialiseert deze punten door de intiti�le supertriangle te berekenen
        *   en deze toe te voegen aan de supervertices vector
        */
        //TODO: WAAROM NIET GEWOON SUPERVERTICES VECTOR TERUGGEVEN IPV PRIVATE TE VERKLAREN?
        void initialize(std::vector<Point> &points){
			triangulation.clear();
			// zet enkele te meten statistieken op nul
			totalPathLength = 0;
			numDeletedTriangles = 0;
			cosineSmallestAngle.clear();
			cosineMaxMin.clear();
			searchTime = Duration::zero();
			updateTime = Duration::zero();
			finishTime = Duration::zero();

			Clock::time_point tik = Clock::now();

			// Determinate the super triangle
			Real minX = points[0].x();
			Real minY = points[0].y();
			Real maxX = minX;
			Real maxY = minY;

			for(std::size_t i = 0; i < points.size(); ++i)
			{
				if (points[i].x() < minX) minX = points[i].x();
				if (points[i].y() < minY) minY = points[i].y();
				if (points[i].x() > maxX) maxX = points[i].x();
				if (points[i].y() > maxY) maxY = points[i].y();
			}
			
			Real dx = maxX - minX;
			Real dy = maxY - minY;
			Real deltaMax = std::max(dx, dy);
			midx = (minX + maxX) / 2.f;
			midy = (minY + maxY) / 2.f;

			//construct 3 edges of supertriangle
			Point p1(midx - 20 * deltaMax, midy - deltaMax, 0);
			Point p2(midx, midy + 20 * deltaMax, 0);
			Point p3(midx + 20 * deltaMax, midy - deltaMax, 0);

			
			// Create a list of triangles, and add the supertriangle in it
			triangulation.make_triangle(p1, p2, p3);
			PH::Vertex_iterator v_iter = triangulation.vertices_begin();
			for(int i = 0; i < 3; i++)
				superVertices[i] = v_iter++;

			// We kunnen te maken hebben met een vlak waarvan de normaal ofwel de eenheidsvector in
			// de z-richting is, ofwel in de tegengestelde richting.
			// De halfbogen draaien ccw rond deze normaal, en het is handiger om zeker te
			// zijn dat die in de positieve z-richting is.
			PH::Halfedge_handle internalEdge = triangulation.facets_begin()->halfedge();
			if(rightOf(internalEdge, internalEdge->next()->vertex()->point()))
				triangulation.inside_out();

			preprocessingTime += Clock::now() - tik;

		}

        /*
        *   Voegt een vertex toe aan de triangulatie en berekent de nieuwe triangulatie.
        *   Geeft Vertex_handle terug, dit is gelijkend aan een pointer.
        */
        //TODO: MAAK APARTE METHODES VAN ELKE STAP DIE NODIG IS OM EEN VERTEX TOE TE VOEGEN AAN DE TRIANGULATIE!!
		PH::Vertex_handle addVertex(Point &p, PH::Halfedge_handle startEdge)
		{
			// blijkbaar is het trager als je van polygon en discoveredTriangles sets maakt.
			std::vector<PH::Halfedge_handle> polygon;
			std::vector<PH::Facet_handle> badTriangles;

			// vind de slechte driehoeken
			Clock::time_point tik = Clock::now();
			PH::Halfedge_handle gs_edge = adjEdge(p, startEdge, totalPathLength);
			Clock::time_point tak = Clock::now();
			searchTime += tak - tik;

			std::vector<PH::Facet_handle> discoveredTriangles;
			std::list<PH::Facet_handle> queue;
			queue.push_back(gs_edge->facet());
			while(!queue.empty()){
				PH::Facet_handle current = queue.front();
				queue.pop_front();
				badTriangles.push_back(current);
				discoveredTriangles.push_back(current);
				PH::Halfedge_handle e = current->halfedge();
				for(int i = 0; i < current->facet_degree(); i++){
					e = e->next();
					// plaats naburige driehoek op de queue als deze nog niet gevonden is
					if(e->opposite()->is_border())
						continue; // dit is geen driehoek
					PH::Facet_handle neighbor = e->opposite()->facet();
					bool alreadySeen = false;
					for(int j =0; j < discoveredTriangles.size(); j++){
						if(discoveredTriangles[j] == neighbor)
							alreadySeen = true;
					}
					if(!alreadySeen && circumCircleContains(neighbor, p)){
						queue.push_back(neighbor);
					}
				}
			}

			// update de statistieken
			numDeletedTriangles += badTriangles.size();

			// construeer polygon
			for(int i = 0; i < badTriangles.size(); i++){
				PH::Halfedge_handle e = badTriangles[i]->facet_begin();
				for(int j = 0; j < badTriangles[i]->facet_degree(); j++){
					polygon.push_back(e);
					e = e->next();
				}
			}

			// markeer edges die wel en niet weg moeten
			std::vector<PH::Halfedge_handle> goodEdges;
			std::vector<PH::Halfedge_handle> badEdges;
			bool isBad;
			for(int i = 0; i < polygon.size(); i++){
				PH::Halfedge_handle e1 = polygon[i];
				isBad = false;
				for(int j = 0; j < polygon.size(); j++){
					PH::Halfedge_handle e2 = polygon[j];
					if(e1->opposite() == e2)
					{
						// alle bogen die via twee verschillende driehoeken gevonden worden, moeten volledig weg (samen met hun tweeling)
						isBad = true;
						// TODO: Moet je wel een lijst met bad-edges bijhouden? Want je kunt ook gewoon alle halfedges uit
						// de "slechte" driehoeken verwijderen.
//						badEdges.push_back(e1);
					}
				}
				// alle bogen die maar via 1 driehoek gevonden worden, worden deel van een nieuwe driehoek die p zal bevatten
				if(!isBad)
					goodEdges.push_back(e1->opposite());
			}

			// Verwijder alle halfbogen van polygon uit de de triangulatie om zo een veelhoekig gat te maken.
			// Voor elke (half)boog die verwijderd wordt, moet je er eerst voor zorgen dat er geen punten verwijderd
			// worden door CGAL. Als je voor elke boog waarvan de tweeling een buitenrand is, een extra
			// vlak toevoegt, *denk ik* dat je veilig zit, maar het is goed mogelijk dat dit eigenlijk teveel werk is.
			std::vector<PH::Halfedge_handle> auxEdges;
			for(int i = 0; i < goodEdges.size(); i++){
				PH::Halfedge_handle e = goodEdges[i];
				if(! e->is_border())
					continue; // deze is niet in gevaar

				// deze boog is aangrenzend aan een andere boog op de rand. Normaal gezien ligt de vorige boog op de
				// rand, maar het zou kunnen (omwille van de volgorde van de bogen) dat je ook naar de volgende moet kijken
				PH::Halfedge_handle otherBorder = e->prev();
//				if(!otherBorder->is_border())
//					throw std::runtime_error("Fout in de implementatie.");
				PH::Halfedge_handle auxEdge = triangulation.add_vertex_and_facet_to_border(otherBorder, e);
//				Point &v = auxEdge->vertex()->point();
//				v = Point(v.x(), v.y(), 10);
				auxEdges.push_back(auxEdge);
			}

			// Verwijder nu effectief alle halfbogen die weg moeten
			for(int i = 0; i < badTriangles.size(); i++)
				triangulation.erase_facet(badTriangles[i]->halfedge());

			// Voeg de nieuwe driehoeken toe met een punt in p
			PH::Halfedge_handle he = goodEdges[0]->opposite();
			PH::Halfedge_handle newHE = triangulation.add_vertex_and_facet_to_border(he, he->next());
			PH::Vertex_handle addedVertex = newHE->vertex();
			addedVertex->point() = p; // newhe = van e2->eindpunt naar newpoint
			he = newHE->next()->opposite(); // e = ep
			// Nu zijn er al 2 van de 3 bogen toegevoegd. Nog 3 - 2 = 1 te gaan.
			for(int i = 0; i < goodEdges.size() - 2; i++){
				he = triangulation.add_facet_to_border(he->prev()->prev(), he);
				he = he->opposite();
			}
			// nu is er enkel nog een driehoekig gat dat moet opgevuld worden (zonder bogen toe te voegen)
			triangulation.fill_hole(he);

			// verwijder wat je met de hulpbogen had toegevoegd
			for(int i = 0; i < auxEdges.size(); i++)
				triangulation.erase_facet(auxEdges[i]);

			updateTime += Clock::now() - tak;

			// update de statistieken
			cosineSmallestAngle.push_back(cosSmallestAngle(addedVertex));
			cosineMaxMin.push_back(cosMaxMin(addedVertex));

			return addedVertex;
		}

        /*
        *   Verwijderd de supertriangle en alle driehoeken die met de hoekpunten hiervan verbonden zijn.
        *   Geeft de triangulatie terug.
        */
		PH &finish(){
			Clock::time_point tik = Clock::now();

			for(int i = 0; i < 3; i++){
				PH::Vertex_handle v = superVertices[i];
				// we weten dat v op de rand ligt
				int deg = v->degree();
				PH::Halfedge_around_vertex_circulator e = v->vertex_begin();
				while(!(e->is_border_edge() && prev(e)->is_border_edge())) e++;
				for(int i=0; i < deg - 1; i++)
					triangulation.erase_facet(e++);
			}

			finishTime = Clock::now() - tik;

//			std::cout << "=====EINDRESULTAAT=====================\n";
//			print();
//			if (isDelaunay())
//				std::cout << "Deze is delaunay.\n";
			return triangulation;
		}

		Clock::duration preprocessingTime;
		Clock::duration searchTime;
		Clock::duration updateTime;
		Clock::duration finishTime;

	private:
		Polyhedron triangulation;
		PH::Vertex_handle superVertices[3];
		Real midx, midy;
		int totalPathLength;
		int numDeletedTriangles;
		// {cosSmallestAngle(vertex), op het moment dat een vertex wordt toegevoed | voor elk toegevoegd punt}.
		vector<Real> cosineSmallestAngle;
		vector<Real> cosineMaxMin;

};

#endif
