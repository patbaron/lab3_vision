#include "StdAfx.h"
#include "Traitement.h"
#include <math.h>


int CTraitement::seuilAutomatique(Mat mat,int* positionSommet1, int* positionSommet2)
{
	int seuil = 0;
	int sommet1 = 0;
	int sommet2 = 0;

	Mat tonsGris = calculerTonsGris(mat);
	int bob[256] = {};
	for (int i=0; i< 256;i++)
	{
		bob[i] = tonsGris.at<int>(Point(0,i));
	}
	int quantiteMax = 0;
	for(int x = 0; x < tonsGris.rows; x++)
	{
		if (tonsGris.at<int>(Point(0,x)) > quantiteMax)
		{		
			quantiteMax = tonsGris.at<int>(Point(0,x));
			sommet1 = x;
			if(positionSommet1 != NULL)
			{
				*positionSommet1 = x;
			}
		}			
	}

	 quantiteMax = 0;
	for(int x = 0; x < tonsGris.rows; x++)
	{
		if ((tonsGris.at<int>(Point(0,x)) * pow((float)(x - sommet1), (float)2)) > quantiteMax)
		{
			quantiteMax =tonsGris.at<int>(Point(0,x)) * pow((float)(x - sommet1), (float)2);
			sommet2 = x;
			if(positionSommet2 != NULL)
			{
				*positionSommet2 = x;
			}
		}
	}

	seuil = sommet2 + ((sommet1 - sommet2)/2);

	return seuil;
}

int CTraitement::seuilAutomatiqueLigne(Mat mat, Point point)
{
	if(mat.channels() > 1)
	{
		mat = ConvertisTonGris(mat);
	}
	int seuil = 0;
	Mat tonsGris = Mat(256, 1, CV_32S);
	tonsGris.setTo(0);
	int sommet1 = 0;
	int sommet2 = 0;
	uchar valeurDeGris;	
	
	for(int x = 0; x < mat.cols; x++)
	{
		valeurDeGris = mat.at<uchar>(Point(x,point.y));			
		tonsGris.at<int>(Point(0,valeurDeGris))++;
	}
	

	for(int x = 0; x < tonsGris.rows; x++)
	{
		if (tonsGris.at<int>(Point(0,x)) > sommet1)
			sommet1 = x;
	}

	for(int x = 0; x < tonsGris.rows; x++)
	{
		if ((tonsGris.at<int>(Point(0,x)) * pow((float)(x - sommet1), (float)2)) > sommet2)
			sommet2 = x;
	}

	seuil = sommet2 + ((sommet1 - sommet2)/2);

	return seuil;
}

Mat CTraitement::ConvertisTonGris(Mat image)
{
	if (image.channels() == 1)
	{
		return image;
	}
	cv::Mat imageGrise;
	cv::cvtColor(image, imageGrise, CV_BGR2GRAY);
	return imageGrise;
}

bool CTraitement::inspectionFils(Mat image, Point point)
{
	if(image.channels() > 1)
	{
		image = ConvertisTonGris(image);
	}
	int nbrFils = 0;
	bool debutFil = false;
	int seuil = 140;

	for (int x =0 ; x < image.cols; x++)
	{
		if (!debutFil && image.at<uchar>(Point(x, point.y)) < seuil)
		{
			debutFil = true;
		}
		else if (debutFil && image.at<uchar>(Point(x, point.y)) > seuil)
		{
			nbrFils++;
			debutFil = false;
		}
	}
	return nbrFils == 4;
}

bool CTraitement::detectionCercle(Mat image)
{
	vector<Vec3f> cercles;
	image = ConvertisTonGris(image);
	GaussianBlur(image, image, Size(9,9), 2, 2);
	HoughCircles(image, cercles, CV_HOUGH_GRADIENT, 2, image.rows/4, 200, 100);

	if(cercles.size() > 0)
	{
		return true;
	}
	return false;
}

Mat CTraitement::binariser(Mat image, int seuil)
{
	image = ConvertisTonGris(image);
	
	for (int i=0; i < image.rows*image.cols; i++)
	{
		if (image.data[i] > seuil)
			image.data[i] = 255;
		else
			image.data[i] = 0;
	}
	return image;
}

Mat CTraitement::etirerHistogramme(Mat image)
{
	Mat histogrammeTonsGris = calculerTonsGris(image);
	int positionSommet1, positionSommet2;
	seuilAutomatique(image, &positionSommet1, &positionSommet2);

	
	double plage = abs(positionSommet2 - positionSommet1);

	int sommetGauche, sommetDroite;
	if (positionSommet1 > positionSommet2)
	{
		sommetDroite = positionSommet1;
		sommetGauche = positionSommet2;
	}
	else
	{
		sommetGauche = positionSommet1;
		sommetDroite = positionSommet2;
	}
	Mat histogrammeEtire = Mat(histogrammeTonsGris.rows, histogrammeTonsGris.cols,histogrammeTonsGris.type());

	for(int i= 0; i< 256;i++)
	{
		histogrammeEtire.at<int>(Point(0,i)) = histogrammeTonsGris.at<int>(Point(0,(int)(sommetGauche+(i/255.0)*plage)));
	}

	

	return histogrammeEtire;
}

Mat CTraitement::calculerTonsGris(Mat image)
{
	image = ConvertisTonGris(image);

	//calculer
	Mat tonsGris = Mat(256, 1, CV_32S);
	tonsGris.setTo(0);	
	uchar valeurDeGris;	
	for(int y = 0; y < image.rows; y++)
	{
		for(int x = 0; x < image.cols; x++)
		{
			valeurDeGris = image.at<uchar>(Point(x,y));			
			tonsGris.at<int>(Point(0,valeurDeGris))++;
		}
	}
	
	return tonsGris;
}

void CTraitement::DessineDiagramme(Mat histogrammeTonsGris)
{
	Mat Diagramme(480,640,CV_8U);
	Diagramme.setTo(Scalar(255));
	int xAxis=70,yAxis=460;
	Point pointPrecedant, pointActuel;

	line(Diagramme, Point(xAxis, 20), Point(xAxis, 460), Scalar(0), 2,1);
	line(Diagramme, Point(70, yAxis), Point(620, yAxis), Scalar(0), 2,1);

	line(Diagramme, Point(66, 374), Point(74, 374), Scalar(0), 2,1);
	line(Diagramme, Point(66, 287), Point(74, 287), Scalar(0), 2,1);
	line(Diagramme, Point(66, 201), Point(74, 201), Scalar(0), 2,1);
	line(Diagramme, Point(66, 115), Point(74, 115), Scalar(0), 2,1);
	line(Diagramme, Point(66, 29), Point (74, 29), Scalar(0), 2,1);

	int maxVal=0;
	for(int i=0; i<histogrammeTonsGris.rows;i++)
	{
		if (histogrammeTonsGris.at<int>(Point(0,i)) > maxVal)
			maxVal = histogrammeTonsGris.at<int>(Point(0,i));
	}
	
	int step = (int)maxVal/5;
	char valeur[10];

	for(int i = 0 ; i <= 5; i++)
	{
		memset(valeur,0,10);
		sprintf(valeur, "%i", i*step);
		putText(Diagramme, valeur, Point(5, 465-(i*86)), CV_FONT_HERSHEY_COMPLEX, 0.5,  Scalar(0,0,200));
	}
		
	double valeurHauteurPixel = maxVal / 431.0;
	
	
	int yPixel, xPixel;
	double pixQte;

	for(int i=0; i<histogrammeTonsGris.rows;i++)
	{
		pixQte = histogrammeTonsGris.at<int>(Point(0,i));
		
		yPixel = yAxis - (int)(pixQte/valeurHauteurPixel);
		xPixel = xAxis + (((double)i/255.0))*550.0;

		pointActuel = Point(xPixel, yPixel);
		if (i>1)
		{
			line(Diagramme, pointPrecedant, pointActuel, Scalar(0), 1,1);
		}
		pointPrecedant = pointActuel;
	}

	imshow("Diagramme",Diagramme);
}

void RegardePixelACote(Mat image, Point point, uchar* valMin, uchar* valMax)
{
	if (image.at<uchar>(point) > *valMax)
		*valMax = image.at<uchar>(point);
	if (image.at<uchar>(point) < *valMin)
		*valMin = image.at<uchar>(point);
}

uchar CalculeMediane(Mat image, int x, int y, int grosseur)
{
	uchar valMin=255;
	uchar valMax=0;
	RegardePixelACote(image,Point(x-1,y-1),&valMin,&valMax);	
	RegardePixelACote(image,Point(x,y-1),&valMin,&valMax);
	RegardePixelACote(image,Point(x+1,y-1),&valMin,&valMax);
	RegardePixelACote(image,Point(x-1,y),&valMin,&valMax);
	RegardePixelACote(image,Point(x,y),&valMin,&valMax);
	RegardePixelACote(image,Point(x+1,y),&valMin,&valMax);
	RegardePixelACote(image,Point(x-1,y+1),&valMin,&valMax);
	RegardePixelACote(image,Point(x,y+1),&valMin,&valMax);
	RegardePixelACote(image,Point(x+1,y+1),&valMin,&valMax);
	if (grosseur == 5)
	{
		RegardePixelACote(image,Point(x-2,y-2),&valMin,&valMax);
		RegardePixelACote(image,Point(x-1,y-2),&valMin,&valMax);
		RegardePixelACote(image,Point(x,y-2),&valMin,&valMax);
		RegardePixelACote(image,Point(x+1,y-2),&valMin,&valMax);
		RegardePixelACote(image,Point(x+2,y-2),&valMin,&valMax);
		RegardePixelACote(image,Point(x-2,y-1),&valMin,&valMax);
		RegardePixelACote(image,Point(x+2,y-1),&valMin,&valMax);
		RegardePixelACote(image,Point(x-2,y),&valMin,&valMax);
		RegardePixelACote(image,Point(x+2,y),&valMin,&valMax);
		RegardePixelACote(image,Point(x-2,y+1),&valMin,&valMax);
		RegardePixelACote(image,Point(x+2,y+1),&valMin,&valMax);
		RegardePixelACote(image,Point(x-2,y+2),&valMin,&valMax);
		RegardePixelACote(image,Point(x-1,y+2),&valMin,&valMax);
		RegardePixelACote(image,Point(x,y+2),&valMin,&valMax);
		RegardePixelACote(image,Point(x+1,y+2),&valMin,&valMax);
		RegardePixelACote(image,Point(x+2,y+2),&valMin,&valMax);
	}
	return (uchar)((valMax-valMin)/2.0)+valMin;
}

Mat CTraitement::filtrePasseHaut(Mat image)
{
	Mat imageFPH = ConvertisTonGris(image);
	Laplacian(image, imageFPH, CV_16S,3, 1, 0, BORDER_DEFAULT );
	convertScaleAbs( imageFPH, imageFPH );
	return imageFPH;
}


uchar CalculeMoyenne(Mat image, int x, int y,int grosseur)
{
	int valeur=0;
	valeur += image.at<uchar>(Point(x-1,y-1));
	valeur += image.at<uchar>(Point(x,y-1));
	valeur += image.at<uchar>(Point(x+1,y-1));
	valeur += image.at<uchar>(Point(x-1,y));
	valeur += image.at<uchar>(Point(x,y));
	valeur += image.at<uchar>(Point(x+1,y));
	valeur += image.at<uchar>(Point(x-1,y+1));
	valeur += image.at<uchar>(Point(x,y+1));
	valeur += image.at<uchar>(Point(x+1,y+1));
	if (grosseur == 5)
	{
		valeur += image.at<uchar>(Point(x-2,y-2));
		valeur += image.at<uchar>(Point(x-1,y-2));
		valeur += image.at<uchar>(Point(x,y-2));
		valeur += image.at<uchar>(Point(x+1,y-2));
		valeur += image.at<uchar>(Point(x+2,y-2));
		valeur += image.at<uchar>(Point(x-2,y-1));
		valeur += image.at<uchar>(Point(x+2,y-1));
		valeur += image.at<uchar>(Point(x-2,y));
		valeur += image.at<uchar>(Point(x+2,y));
		valeur += image.at<uchar>(Point(x-2,y+1));
		valeur += image.at<uchar>(Point(x+2,y+1));
		valeur += image.at<uchar>(Point(x-2,y+2));
		valeur += image.at<uchar>(Point(x-1,y+2));
		valeur += image.at<uchar>(Point(x,y+2));
		valeur += image.at<uchar>(Point(x+1,y+2));
		valeur += image.at<uchar>(Point(x+2,y+2));
	}
	return (uchar)(valeur/(grosseur*grosseur));
}

Mat CTraitement::lissage(Mat image, bool typeMedian, int grosseur)
{
	list<Point> pointsMatrice;
	image = ConvertisTonGris(image);
	double valeur = 0;
	int moitieEntiere = grosseur /2;	
	Mat resultante = Mat(image.rows,image.cols, image.type());
	resultante.setTo(0);
	if( grosseur != 3 && grosseur!= 5)
		return resultante;
	Point p;
	uchar mediane,moyenne;
	if (typeMedian)
	{
		for (int x = moitieEntiere; x < image.cols-moitieEntiere;x++)
		{
			for (int y=moitieEntiere; y<image.rows-moitieEntiere;y++)
			{	
				mediane = CalculeMediane(image,x,y,grosseur);				
				resultante.at<uchar>(Point(x,y)) = mediane;
			}
		}
	}
	else
	{
		for (int x = moitieEntiere; x < image.cols-moitieEntiere;x++)
		{
			for (int y=moitieEntiere; y < (image.rows-moitieEntiere); y++)
			{	
				moyenne = CalculeMoyenne(image,x,y,grosseur);
				resultante.at<uchar>(Point(x,y)) = moyenne;
			}
		}
	}
	return resultante;
}

