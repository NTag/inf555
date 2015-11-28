//
//  main.cpp
//  inf555
//
//  Created by Basile Bruneau on 04/11/2015.
//  Copyright © 2015 Basile Camille. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <array>

#include <GLUT/glut.h>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include "CannyFilter.hpp"
#include "Galif.hpp"
#include "DistributedViews.hpp"
#include "Point3.hpp"
#include "Vocabulary.hpp"
#include "HistogramHelper.hpp"

using namespace std;
using namespace cv;

/* ***** Variables ***** */

static const int FEAT_SIZE = 256;

char* path;
vector<string> files;
string currentFile = "";
int numberOfViews = 50;
int currentView = numberOfViews+1;
DistributedViews sphere(numberOfViews);
Point3* directions;
CannyFilter canny = CannyFilter(10, 30);
GALIF* galif = new GALIF(0.13, 5, 4, 8);
double proba = 1./numberOfViews;
vector<array<float, FEAT_SIZE>> features_set;
Vocabulary* vocab = new Vocabulary();
HistogramHelper* helper;


/* ***** Utils functions ***** */

/**
 * Usage : 
 * string ty =  type2str( img.type() );
 * printf("Matrix: %s %dx%d \n", ty.c_str(), img.cols, img.rows );
 */
string type2str(int type) {
    string r;
    
    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);
    
    switch (depth) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
    }
    
    r += "C";
    r += (chans+'0');
    
    return r;
}




void loadFilesFromFolder(char* path) {
    DIR *dir;
    struct dirent *ent;
    regex off("^.+\\.off$");
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (regex_match(ent->d_name, off)) {
                string fc(path);
                fc = fc + "/";
                fc = fc + ent->d_name;
                files.push_back(fc);
            }
        }
        closedir(dir);
    } else {
        cout << "Impossible d'ouvrir le dossier";
    }
}




/* ***** OFF files preprocessing ***** */

void processImage(float* depth) {
    // Transform depth into a cv::Mat
    Mat img(800, 800, CV_32F, depth);
//    imshow("Depth", img);
    
    // Apply Canny filter to extract lines
    Mat edges = canny.detectEdges(img);
    cout << "Canny : terminé" << endl;
//    imshow("Edges", edges);
//    waitKey(0);
    
    // Compute features
    cout << "Extraction de features : démarrage" << endl;
    edges.convertTo(edges, CV_32F);
    vector<array<float, FEAT_SIZE>> features = galif->features(edges, proba);
    if (vocab->kMeansDone) { // Create histograms
        int idx = helper->addPreHistogram(currentFile);
        for (vector<array<float, FEAT_SIZE>>::iterator it = features.begin(); it != features.end(); ++it) {
            helper->addFeatureForPreHistogram(idx, *it);
        }
    } else { // Prepare for vocabulary creation
        for (int i = 0; i < features.size(); i++) {
            features_set.push_back(features[i]);
        }
    }
    cout << "Extraction de features : terminé" << endl;
    
    
}




void continueFile() {
    cout << "Vue " << currentView << endl;
    
    ifstream File(currentFile);
    string Line;
    string Name;
    int i = 0;
    GLfloat** TempVertices;
    
    int nbVertices = 0;
    int nbFaces = 0;
    int nbEdges = 0;
    
    float* b = new float[3];
    b[0] = 0;
    b[1] = 0;
    b[2] = 0;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    while(getline(File, Line)) {
        if(Line == "" || Line[0] == '#')// Skip everything and continue with the next line
            continue;
        i++;
        
        // Ligne 2 : number of vertices, number of faces, number of edges
        if (i == 1) {
            
        } else if (i == 2) {
            sscanf(Line.c_str(), "%i %i %i", &nbVertices, &nbFaces, &nbEdges);
            TempVertices = new GLfloat*[nbVertices];
        } else if (i - 2 <= nbVertices) {
            float* Vertex = new float[3];
            sscanf(Line.c_str(), "%f %f %f", &Vertex[0], &Vertex[1], &Vertex[2]);
            b[0] += Vertex[0];
            b[1] += Vertex[1];
            b[2] += Vertex[2];
            
            TempVertices[i-3] = Vertex;
        } else if (i - 2 - nbVertices <= nbFaces) {
            int currentVertices[3];
            sscanf(Line.c_str(), "3 %i %i %i", &currentVertices[0], &currentVertices[1], &currentVertices[2]);
            GLfloat c[3] = {0.3, 0.5,0.1};
            glColor3fv(c);
            glVertex3fv(TempVertices[currentVertices[0]]);
            glVertex3fv(TempVertices[currentVertices[1]]);
            glVertex3fv(TempVertices[currentVertices[2]]);
        }
    }
    
    b[0] = b[0]/nbVertices;
    b[1] = b[1]/nbVertices;
    b[2] = b[2]/nbVertices;
    
    // cout << "b : " << b[0] << " " << b[1] << " " << b[2] << endl;
    
    glEnd();
    glFlush();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // On met l'objet dans la position correspondant à la vue souhaitée
    // glRotated(currentView+45, 0, 1, 0);
    float fa = 1.1;
    gluLookAt(fa*directions[currentView].x, fa*directions[currentView].y, fa*directions[currentView].z, b[0], b[1], b[2], -directions[currentView].x, -directions[currentView].y, -directions[currentView].z);
    // cout << directions[currentView].x << " " << directions[currentView].y << " " << directions[currentView].z << endl;
    
    
    //glTranslatef(-b[0], -b[1], -b[2]);
    
    // On calcule la profondeur
    float* depth = new float[800*800];
    glReadPixels(0, 0, 800, 800, GL_DEPTH_COMPONENT, GL_FLOAT, depth);
    
//    for (int l = 0; l < 800; l++) {
//        for (int k = 0; k < 800; k++) {
//            cout << depth[l*800+k] << " ";
//        }
//        cout << endl;
//    }
    
    // On applique la suite
    processImage(depth);
    

    
    delete[] depth;
    for (int i = 0; i < nbVertices; i++) {
        delete[] TempVertices[i];
    }
    delete[] TempVertices;
    delete[] b;
    File.close();
    cout << endl;
}




void draw() {
    if (currentView < numberOfViews) {
        currentView++;
    } else {
        if (files.empty()) {
            if (!vocab->kMeansDone) {
                cout << "Création du vocabulaire visuel : démarrage" << endl;
                vocab = new Vocabulary(50, features_set);
                vocab->kMeans();
                cout << "MSE : " << vocab->MSE << endl;
                cout << "Création du vocabulaire visuel : terminé" << endl << endl;
                
                vector<array<float, FEAT_SIZE>> words;
                for (int i = 0; i < vocab->size; i++) {
                    words.push_back(vocab->centroids[i]);
                }
                helper = new HistogramHelper(galif->k * galif->n * galif->n, words);
                
                // Compute all features
                proba = 1.;
                loadFilesFromFolder(path);
                currentView = numberOfViews+1;
            } else {
                helper->computeHistograms();
                helper->saveHistograms("/Users/ntag/Documents/X/2015-2016/INF555/inf555/histograms");
                return;
            }
        }
        currentView = 1;
        currentFile = files.back();
        files.pop_back();
        cout << "Fichier " << currentFile << endl;
        
        // On génère les directions selon lesquelles projeter
        // Et on les stocke
        directions = sphere.getDirections();
    }
    
    continueFile();
    // sleep(1);
    
    glutPostRedisplay();
}

void query(string filename) {
    Mat img = imread(filename);
    cvtColor(img, img, COLOR_BGR2GRAY);
    
    Mat edges = canny.detectEdges(img);
    cout << "Canny : terminé" << endl;
    
    edges.convertTo(edges, CV_32F);
    vector<array<float, FEAT_SIZE>> features = galif->features(edges, 1);
    
    vector<string> models = helper->findClosestModels(features, 2);
    for (int i = 0; i < models.size(); i++) {
        cout << models[i] << endl;
    }
}


int main(int argc, char** argv) {
//    Mat gf = galif->get_filter(1, 1600, 1600);
//    string tygf = type2str(gf.type());
//    printf("Matrix: %s %dx%d \n", tygf.c_str(), gf.cols, gf.rows );
//    
//    
//    Mat gf_space;
//    idft(gf, gf_space);
//
////    int step = gf.rows/2;
////    Mat q0(gf, Rect(0, 0, step, step));
////    Mat q1(gf, Rect(step, 0, step, step));
////    Mat q2(gf, Rect(0, step, step, step));
////    Mat q3(gf, Rect(step, step, step, step));
////    Mat tmp;
////    q0.copyTo(tmp);
////    q3.copyTo(q0);
////    tmp.copyTo(q3);
////    q1.copyTo(tmp);
////    q2.copyTo(q1);
////    tmp.copyTo(q2);
//    
//    Mat planes[] = {Mat::zeros(gf.size(), CV_32F), Mat::zeros(gf.size(), CV_32F)};
//    split(gf, planes);
//    magnitude(planes[0], planes[1], planes[0]);
//    planes[0] += 1;
//    log(planes[0], planes[0]);
//    planes[0] *= 20.0;
//    normalize(planes[0], planes[0], 0, 1, CV_MINMAX);
//    imshow("IMG", planes[0]);
//    waitKey();
//    
//    
//    string tygf_s = type2str(gf_space.type());
//    printf("Matrix: %s %dx%d \n", tygf_s.c_str(), gf_space.cols, gf_space.rows );
//    int stepp = gf_space.rows/2;
//    Mat p0(gf_space, Rect(0, 0, stepp, stepp));
//    Mat p1(gf_space, Rect(stepp, 0, stepp, stepp));
//    Mat p2(gf_space, Rect(0, stepp, stepp, stepp));
//    Mat p3(gf_space, Rect(stepp, stepp, stepp, stepp));
//    Mat tmpp;
//    p0.copyTo(tmpp);
//    p3.copyTo(p0);
//    tmpp.copyTo(p3);
//    p1.copyTo(tmpp);
//    p2.copyTo(p1);
//    tmpp.copyTo(p2);
//    
//    split(gf_space, planes);
//    magnitude(planes[0], planes[1], planes[0]);
//    planes[0] += 1;
//    log(planes[0], planes[0]);
//    planes[0] *= 20.0;
//    normalize(planes[0], planes[0], 0, 1, CV_MINMAX);
////    for (int i = 0; i < planes[0].total(); i++) {
////        assert(0 <= planes[0].at<float>(i) && planes[0].at<float>(i) <= 1);
////    }
//    
//    imshow("IMG", planes[0]);waitKey();
//    //
//    //    Mat dft_img = Mat(getOptimalDFTSize(gf.rows), getOptimalDFTSize(gf.cols), CV_32FC2);
//    //    idft(gf, dft_img);
//    //    imshow("IMG", dft_img);waitKey();
//    //
//    //    double minFreq, maxFreq;
//    //    Point minPt, maxPt;
//    //    minMaxLoc(dft_img, &minFreq, &maxFreq, &minPt, &maxPt);
//    //    cout << maxPt << endl;
////    img.convertTo(img, CV_32F);
////    galif->compute_filters(img);
////    Mat f = galif->filter(0, img);
////    imshow("Filter", f);waitKey();
//    return 0;
    
    /* OFF preprocessing */
    cout << "INF555\n";
    if (argc < 3) {
        cout << "Usage: inf555 preprocess folder\n";
        cout << "Usage: inf555 query histograms image\n";
        return EXIT_FAILURE;
    }
    
    string action = argv[1];
    if (action == "query") {
        cout << "Query on database\n";
        helper = new HistogramHelper(argv[2]);
        
        query(argv[3]);
        
        return 0;
    }
    
    cout << "Preprocessing\n";
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("GLUT Program");
    
    path = argv[2];
    loadFilesFromFolder(path);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glColor3f(1.0, 1.0, 0.0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -2.2, 2.2);
    
    glutDisplayFunc(draw);
    
    glutMainLoop();
    
    return 0;
}