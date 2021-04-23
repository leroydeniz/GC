#include "camara.h"
#include "definitions.h"
#include "math.h"
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

extern camara *_selected_camera;
extern object3d *_selected_object;

/***********************************/
/*** MULTIPLICA MATRIZ POR PUNTO ***/
/***********************************/
point3 multiplicarMatrizPorPunto(GLdouble m[16], point3 p) {
    point3 r;
    r.x = m[0] * p.x + m[4] * p.y + m[8] * p.z + m[12];
    r.y = m[1] * p.x + m[5] * p.y + m[9] * p.z + m[13];
    r.z = m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14];

    return r;
}

/************************************/
/*** MULTIPLICA MATRIZ POR VECTOR ***/
/************************************/
vector3 multiplicarMatrizPorVector(GLdouble m[16], vector3 p) {
    vector3 r;
    r.x = m[0] * p.x + m[4] * p.y + m[8] * p.z;
    r.y = m[1] * p.x + m[5] * p.y + m[9] * p.z;
    r.z = m[2] * p.x + m[6] * p.y + m[10] * p.z;

    return r;
}

/***************************************************/
/*** CALCULA MATRIZ DE CAMARA QUE MIRA AL OBJETO ***/
/***************************************************/
void mirarObjeto(camara *cam, object3d *obj) {
    cam->at.x = obj->pMptr->M[12];
    cam->at.y = obj->pMptr->M[13];
    cam->at.z = obj->pMptr->M[14];
    calculaMatrizCamara(cam);

}

/********************************/
/*** CALCULA MATRIZ DE CAMARA ***/
/********************************/
void calculaMatrizCamara(camara *camara) {
    glLoadIdentity();
    gluLookAt(camara->e.x, camara->e.y, camara->e.z, camara->at.x, camara->at.y, camara->at.z, camara->up.x,
              camara->up.y, camara->up.z);
    glGetDoublev(GL_MODELVIEW_MATRIX, camara->matrizCamara);
}

/*****************************/
/*** INICIALIZACION CAMARA ***/
/*****************************/
camara *inicializarCamara(point3 e, point3 at, vector3 up) {
    camara *cam = (camara *) malloc(sizeof(camara));
    cam->e = e;
    cam->at = at;
    cam->up = up;
    calculaMatrizCamara(cam);
    return cam;
}

/**************************************/
/*** TRASLADAR CÁMARA EN MODO VUELO ***/
/**************************************/
void trasladarCamaraVuelo(camara *camara, GLdouble x, GLdouble y, GLdouble z) {
    vector3 x_cam = {camara->matrizCamara[0], camara->matrizCamara[4], camara->matrizCamara[8]};
    vector3 y_cam = {camara->matrizCamara[1], camara->matrizCamara[5], camara->matrizCamara[9]};
    vector3 z_cam = {camara->matrizCamara[2], camara->matrizCamara[6], camara->matrizCamara[10]};

    camara->e.x += x * x_cam.x + y * y_cam.x + z * z_cam.x;
    camara->e.y += x * x_cam.y + y * y_cam.y + z * z_cam.y;
    camara->e.z += x * x_cam.z + y * y_cam.z + z * z_cam.z;

    camara->at.x += x * x_cam.x + y * y_cam.x + z * z_cam.x;
    camara->at.y += x * x_cam.y + y * y_cam.y + z * z_cam.y;
    camara->at.z += x * x_cam.z + y * y_cam.z + z * z_cam.z;
    calculaMatrizCamara(camara);
}

/*****************************************/
/*** TRASLADAR CÁMARA EN MODO ANÁLISIS ***/
/*****************************************/
void trasladarCamaraAnalisis(camara *camara, GLdouble x, GLdouble y, GLdouble z) {
    vector3 x_cam = {camara->matrizCamara[0], camara->matrizCamara[4], camara->matrizCamara[8]};
    vector3 y_cam = {camara->matrizCamara[1], camara->matrizCamara[5], camara->matrizCamara[9]};
    vector3 z_cam = {camara->matrizCamara[2], camara->matrizCamara[6], camara->matrizCamara[10]};

    camara->e.x += x * x_cam.x + y * y_cam.x + z * z_cam.x;
    camara->e.y += x * x_cam.y + y * y_cam.y + z * z_cam.y;
    camara->e.z += x * x_cam.z + y * y_cam.z + z * z_cam.z;
    calculaMatrizCamara(camara);

}

/****************************************/
/*** ROTACION DE CAMARA EN MODO VUELO ***/
/****************************************/
void rotacionCamaraVuelo(camara *camara, GLdouble x, GLdouble y, GLdouble z) {
    point3 newAt;

    vector3 x_cam = {camara->matrizCamara[0], camara->matrizCamara[4], camara->matrizCamara[8]};
    vector3 y_cam = {camara->matrizCamara[1], camara->matrizCamara[5], camara->matrizCamara[9]};
    vector3 z_cam = {camara->matrizCamara[2], camara->matrizCamara[6], camara->matrizCamara[10]};


    GLdouble matrizRotacion[16];
    glLoadIdentity();

    if (x != 0) {
        glRotated(2*x,x_cam.x,x_cam.y,x_cam.z);

    } else if (y != 0) {
        glRotated(2*y,y_cam.x,y_cam.y,y_cam.z);

    } else if (z != 0) {
        glRotated(2*z,z_cam.x,z_cam.y,z_cam.z);

    }
    glGetDoublev(GL_MODELVIEW_MATRIX, matrizRotacion);

    newAt.x = camara->at.x - camara->e.x;
    newAt.y = camara->at.y - camara->e.y;
    newAt.z = camara->at.z - camara->e.z;
    newAt=multiplicarMatrizPorPunto(matrizRotacion,newAt);

    newAt.x += camara->e.x;
    newAt.y += camara->e.y;
    newAt.z += camara->e.z;
    camara->at = newAt;
    camara->up=multiplicarMatrizPorVector(matrizRotacion,camara->up);
    calculaMatrizCamara(camara);
}

/*******************************************/
/*** ROTACION DE CAMARA EN MODO ANALISIS ***/
/*******************************************/
void rotacionCamaraAnalisis(camara *camara, GLdouble x, GLdouble y, GLdouble z) {
    point3 newPos;

    vector3 x_cam = {camara->matrizCamara[0], camara->matrizCamara[4], camara->matrizCamara[8]};
    vector3 y_cam = {camara->matrizCamara[1], camara->matrizCamara[5], camara->matrizCamara[9]};
    vector3 z_cam = {camara->matrizCamara[2], camara->matrizCamara[6], camara->matrizCamara[10]};


    GLdouble matrizRotacion[16];
    glLoadIdentity();

    if (x != 0) {
        glRotated(2*x,x_cam.x,x_cam.y,x_cam.z);

    } else if (y != 0) {
        glRotated(2*y,y_cam.x,y_cam.y,y_cam.z);

    } else if (z != 0) {
        glRotated(2*z,z_cam.x,z_cam.y,z_cam.z);

    }
    glGetDoublev(GL_MODELVIEW_MATRIX, matrizRotacion);

    newPos.x = camara->e.x - camara->at.x;
    newPos.y = camara->e.y - camara->at.y;
    newPos.z = camara->e.z - camara->at.z;
    newPos=multiplicarMatrizPorPunto(matrizRotacion,newPos);

    newPos.x += camara->at.x;
    newPos.y += camara->at.y;
    newPos.z += camara->at.z;
    camara->e = newPos;
    camara->up=multiplicarMatrizPorVector(matrizRotacion,camara->up);
    calculaMatrizCamara(camara);
}