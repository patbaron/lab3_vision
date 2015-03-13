#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <list>
#include <vector>


using namespace std;
using namespace cv;

class CTraitement
{
public:
	static int seuilAutomatique(Mat mat,int* positionSommet1, int* positionSommet2);
	static bool inspectionFils(Mat image, Point point);
	static bool detectionCercle(Mat image);
	static Mat binariser(Mat image, int seuil);
	static Mat etirerHistogramme(Mat);
	static Mat lissage(Mat image, bool typeMedian, int grosseur);
	static void passeHaut();
	static void HistogrammeCouleur();
	static Mat ConvertisTonGris(Mat image);
	static int seuilAutomatiqueLigne(Mat mat, Point point);
	static void DessineDiagramme(Mat);
	static Mat calculerTonsGris(Mat image);
	static Mat filtrePasseHaut(Mat image);
};

