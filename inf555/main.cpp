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
#include <GLUT/glut.h>
#include "CannyFilter.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

vector<string> files;
string currentFile = "";
int numberOfViews = 70;
int currentView = numberOfViews+1;

void processImage(float* depth) {
    
    // À adapter
    // Transformer depth en matrice
    // Puis lui appliquer Canny
    Mat img = imread("../fruits.JPG");
    CannyFilter canny = CannyFilter(10, 30);
    Mat edges = canny.detectEdges(img);
    imshow("Edges", edges); waitKey(0);
    
    // Puis découper en morceaux etc...
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
        
        // Ligne 2 : umber of vertices, number of faces, number of edges
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
    
    glEnd();
    glFlush();
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // On met l'objet dans la position correspondant à la vue souhaitée
    glRotated(currentView+45, 0, 1, 0);
    
    glTranslatef(-b[0], -b[1], -b[2]);
    
    // On calcule la profondeur
    float* depth = new float[800*800];
    glReadPixels(0, 0, 800, 800, GL_DEPTH_COMPONENT, GL_FLOAT, depth);
    
    // On applique la suite
    processImage(depth);
    
    //    for (int l = 0; l < 800; l++) {
    //        for (int k = 0; k < 800; k++) {
    //            cout << depth[l*800+k] << " ";
    //        }
    //        cout << endl;
    //    }
    
    delete[] depth;
    for (int i = 0; i < nbVertices; i++) {
        delete[] TempVertices[i];
    }
    delete[] TempVertices;
    delete[] b;
    File.close();
}

void draw() {
    if (currentView < numberOfViews) {
        currentView++;
    } else {
        if (files.empty()) {
            return;
        }
        currentView = 1;
        currentFile = files.back();
        files.pop_back();
        cout << "Fichier " << currentFile << endl;
    }
    
    continueFile();
    
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    
    cout << "Preprocessing images\n";
    if (argc < 2) {
        cout << "Usage: preprocessing folder\n";
        return EXIT_FAILURE;
    }
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("GLUT Program");
    
    DIR *dir;
    struct dirent *ent;
    regex off("^.+\\.off$");
    if ((dir = opendir (argv[1])) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (regex_match(ent->d_name, off)) {
                string fc(argv[1]);
                fc = fc + "/";
                fc = fc + ent->d_name;
                files.push_back(fc);
            }
        }
        closedir (dir);
    } else {
        cout << "Impossible d'ouvrir le dossier";
        return EXIT_FAILURE;
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(1);
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glColor3f(1.0, 1.0, 0.0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -3, 3);
    
    glutDisplayFunc(draw);
    
    glutMainLoop();
    
    return 0;
}