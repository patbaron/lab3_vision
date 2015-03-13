#include "stdafx.h"
#include "device_launch_parameters.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "Traitement.h"

using namespace std;
using namespace cv;
#define E_KEY 101
#define P_KEY 112
#define F_KEY 102
#define H_KEY 104
#define J_KEY 106
#define L_KEY 108
#define B_KEY 98
#define TAB_KEY 9
#define ENTER_KEY 13
#define ESCAPE_KEY 27
#define G_KEY 103
#define S_KEY 115
#define C_KEY 99
#define NOM_FENETRE "Image"
#define NOM_SAUVEGARDE  "snapshot.png"

Mat imageCapturee = Mat(480,640,CV_8UC3);
bool modeCamera = true;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	uchar couleurGris;
	char msg[100];
	if(event == EVENT_RBUTTONDOWN)
	{
		if (x >= 0 && x < imageCapturee.cols && y >= 0 && y < imageCapturee.rows)
		{
			if(imageCapturee.channels() > 1)
			{
				imageCapturee = CTraitement::ConvertisTonGris(imageCapturee);
			}
			couleurGris = imageCapturee.at<uchar>(Point(x, y));
			sprintf(msg,"%i\n",couleurGris);			
			printf(msg);
		}
	}
	else if  (event == EVENT_LBUTTONDOWN && !modeCamera)
	{
		
		if(CTraitement::inspectionFils(imageCapturee, Point(x,y)))
			putText(imageCapturee, "ACCEPTE", Point(5,25), CV_FONT_HERSHEY_COMPLEX, 0.5,  Scalar(0,0,200));	
		
		else
			putText(imageCapturee, "REFUSE", Point(5,25), CV_FONT_HERSHEY_COMPLEX, 0.5,  Scalar(0,0,200));

		
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		
	}
}

bool fichierExiste(string fileName)
{
	ifstream infile(fileName.c_str());
	return infile.good();
}

Mat ChargerFichier(int * succes)
{
	*succes = 0;
	Mat mat;
	string chemin = "";
	cout << "Veuillez entrer le nom du fichier que vous desirez charger. (nom.extension)" << endl;
	getline(cin, chemin);
	if (fichierExiste(chemin))
	{
		mat = imread(chemin, CV_LOAD_IMAGE_COLOR);
		cout << "Le fichier a ete charge." << endl;
		*succes = 1;
	}
	else
	{
		cout << "Le fichier na pas ete trouve." << endl;		
	}
	return mat;
}

int main(int argc, char** argv)
{
	
	int key = 0;
	Mat imageCamera = Mat(480,640,CV_8UC3);
	imageCamera.setTo(0);
	imageCapturee.setTo(0);
	int fichierCharge = 0;
	int seuilAuto = 0;

	namedWindow(NOM_FENETRE, 1);
	setMouseCallback(NOM_FENETRE, CallBackFunc, NULL);

	VideoCapture cap(0);
	if(!cap.isOpened()) 
		return -1;

	while(key != ESCAPE_KEY)
	{
		cap >> imageCamera;
		key = waitKey(30);

		if (key == TAB_KEY)
			modeCamera = !modeCamera;
		if (key == F_KEY)
		{
			imageCapturee = ChargerFichier(&fichierCharge);
			if(fichierCharge == 1)
				modeCamera = false;
		}
			
		if(modeCamera)
		{
			if(key==ENTER_KEY)
			{
				vector<int> compression_params;
				compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
				compression_params.push_back(9);
				imwrite(NOM_SAUVEGARDE, imageCamera, compression_params);
				imageCamera.copyTo(imageCapturee);
				modeCamera = false;
			}
			imshow(NOM_FENETRE, imageCamera);
		}
		else
		{
			if(key == G_KEY)
			{
				imageCapturee = CTraitement::ConvertisTonGris(imageCapturee);
			}
			if(key == S_KEY)
			{
				seuilAuto = CTraitement::seuilAutomatique(imageCapturee,NULL,NULL);
			}
			if(key == C_KEY)
			{
				bool reponse = CTraitement::detectionCercle(imageCapturee);
				if(reponse)
					putText(imageCapturee, "CERCLE", Point(5,25), CV_FONT_HERSHEY_COMPLEX, 0.5,  Scalar(0,0,200));	
				else
					putText(imageCapturee, "RECTANGLE", Point(5,25), CV_FONT_HERSHEY_COMPLEX, 0.5,  Scalar(0,0,200));
			}
			if (key == B_KEY)
			{
				imageCapturee = CTraitement::binariser(imageCapturee, 127);
			}
			if (key == E_KEY)
			{
				Mat vecTonGris = CTraitement::etirerHistogramme(imageCapturee);				
				CTraitement::DessineDiagramme(vecTonGris);
			}
			if (key == H_KEY) //histogramme de base
			{
				Mat vecTonGris = CTraitement::calculerTonsGris(imageCapturee);				
				CTraitement::DessineDiagramme(vecTonGris);
			}
			if (key == L_KEY)
			{				
				imageCapturee = CTraitement::lissage(imageCapturee,true,5);
			}
			if (key == P_KEY)
			{
				imageCapturee = CTraitement::filtrePasseHaut(imageCapturee);
			}
			if (key == J_KEY)
			{
				imageCapturee = imread("lolz.png", CV_LOAD_IMAGE_COLOR);
			}
			imshow(NOM_FENETRE, imageCapturee);
		}
	}

	return 0;
}



