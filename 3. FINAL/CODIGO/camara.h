#include "definitions.h"
#ifndef ENTREGA2_CAMARA_H
#define ENTREGA2_CAMARA_H

#endif
void mirarObjeto(camara* cam,object3d* obj);
void calculaMatrizCamara(camara *camara);
camara* inicializarCamara(point3 e, point3 at, vector3 up);
void trasladarCamaraVuelo(camara *camara, GLdouble x, GLdouble y, GLdouble z);
void trasladarCamaraAnalisis(camara *camara, GLdouble x, GLdouble y, GLdouble z);
void rotacionCamaraVuelo(camara *camara, GLdouble x, GLdouble y, GLdouble z);
void rotacionCamaraAnalisis(camara *camara, GLdouble x, GLdouble y, GLdouble z);
point3 multiplicarMatrizPorPunto(GLdouble m[16], point3 p);
