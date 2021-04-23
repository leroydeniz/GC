#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "display.h"
#include "camara.h"

int estado;
int referencia;
int modo;
int modoCamara;
int proyeccion;
int shading = SMOOTH;
int luzSeleccionada;

extern object3d *_first_object;
extern object3d *_selected_object;
extern camara *_selected_camera;

extern GLdouble _ortho_x_min, _ortho_x_max;
extern GLdouble _ortho_y_min, _ortho_y_max;
extern GLdouble _ortho_z_min, _ortho_z_max;

extern GLdouble _frust_x_min, _frust_x_max;
extern GLdouble _frust_y_min, _frust_y_max;
extern GLdouble _frust_z_min, _frust_z_max;

extern int luces_activadas;
extern int tipo_luz[8];

camara *auxCamara = 0;
extern GLfloat angle;
extern point3 posicion_luces[8];
extern luz luces[8];
int i;

extern GLint GL_LIGHT[8];
GLfloat newPos[4];


elem_Matriz *mPtr;
elem_Matriz *redoPtr = 0;
elem_Matriz *auxPtr = 0;



/************************/
/*** FUNCIÓN DE AYUDA ***/
/************************/

void print_help() {
	printf("*** Practica GC - Leroy Deniz ***\n\n");

	printf("*** FUNCIONES PRINCIPALES ***\n");
	printf("<?>\t\t Mostrar la ayuda \n");
	printf("<ESC>\t\t Salir del programa \n");
	printf("<F>\t\t Cargar un objeto\n");
	printf("<TAB>\t\t Cambiar objeto seleccionado\n");
	printf("<DEL>\t\t Eliminar objeto\n\n");
	printf("<CTRL + ->\t Alejar\n");
	printf("<CTRL + +>\t Acercar\n");
	printf("<CTRL + Z z>\t Deshacer\n");
	printf("<CTRL + X x>\t Rehacer\n\n");
	printf("<UP>\t\t Trasladar+Y | Escalar+Y | Rotar+X \n");
	printf("<DOWN>\t\t Trasladar-Y | Escalar-Y | Rotar-X \n");
	printf("<RIGHT>\t\t Trasladar+X | Escalar+X | Rotar+Y \n");
	printf("<LEFT>\t\t Trasladar-X | Escalar-X | Rotar-Y \n");
	printf("<AVPAG>\t\t Trasladar+Z | Escalar+Z | Rotar+Z \n");
	printf("<REPAG>\t\t Trasladar-Z | Escalar-Z | Rotar-Z \n");
	printf("\n\n");

	printf("*** SELECCION DE MODOS ***\n");
	printf("<CTRL + O o>\t\t Modo objeto \n");
	printf("<CTRL + K k>\t\t Modo camara \n");
	printf("<CTRL + A a>\t\t Modo luz \n");
	printf("\n\n");

	printf("*** FUNCIONES TRANSFORMACION ***\n");
	printf("<CTRL + M m>\t\t Modo traslacion \n");
	printf("<CTRL + B b>\t\t Modo rotacion \n");
	printf("<CTRL + T t>\t\t Modo escalado \n");
	printf("<CTRL + G g>\t\t Sistema de referencia global \n");
	printf("<CTRL + L l>\t\t Sistema de referencia local \n");
	printf("\n\n");

	printf("*** FUNCIONES DE CAMARA ***\n");
	printf("<c>\t\t Cambia la camara \n");
	printf("<C>\t\t Visualizacion desde el objeto \n");
	printf("<n>\t\t Añadir nueva camara \n");
	printf("<G g>\t\t Camara en modo analisis \n");
	printf("<L l>\t\t Camara en modo vuelo\n");
	printf("<T t>\t\t Cambio de volumen de vision \n");
	printf("<B b>\t\t Rotacion de la camara \n");
	printf("<M m>\t\t Traslacion de la camara \n");
	printf("<P p>\t\t Cambio de proyeccion: perspectiva / paralela \n");
	printf("\n\n");

	printf("*** FUNCIONES ILUMINACION ***\n");
	printf("<F1>\t\t Activar / desactivar SOL \n");
	printf("<F2>\t\t Activar / desactivar BOMBILLA \n");
	printf("<F3>\t\t Activar / desactivar FOCO DEL OBJETO \n");
	printf("<1 - 8>\t\t Seleccionar fuente de luz correspondiente \n");
	printf("<F9>\t\t Activar / desactivar iluminación \n");
	printf("<F12>\t\t Cambiar tipo de iluminación \n");
	printf("<+>\t\t Incrementar angulo de apertura de foco\n");
	printf("<->\t\t Decrementar angulo de apertura de foco\n");
	printf("\n\n");
}


/********************************************/
/*** FUNCIÓN DE CÁLCULO DE MATRIZ INVERSA ***/
/********************************************/

void inversa(GLdouble m[16], GLdouble *inv) {
    // Gldouble inv[16];
    inv[0] = m[0];
    inv[1] = m[4];
    inv[2] = m[8];
    inv[3] = (GLdouble) 0;
    inv[4] = m[1];
    inv[5] = m[5];
    inv[6] = m[9];
    inv[7] = (GLdouble) 0;
    inv[8] = m[2];
    inv[9] = m[6];
    inv[10] = m[10];
    inv[11] = (GLdouble) 0;
    inv[12] = -m[12];
    inv[13] = -m[13];
    inv[14] = -m[14];
    inv[15] = (GLdouble) 1;


}


void print_matrix(GLdouble matriz[16]) {
    int i;
    for (i = 0; i < 16; i += 4)
        printf("%0.2f %0.2f %0.2f %0.2f \n", matriz[i], matriz[i + 1], matriz[i + 2], matriz[i + 3]);
    printf("\n");
}


/******************************************************/
/*** FUNCIÓN DE MODIFICACIÓN LOCAL (POR LA DERECHA) ***/
/******************************************************/

void multiplicaMatrizCamara(GLdouble matriz[]) {
    glMultMatrixd(matriz);
}

void multiplicaMatriz(GLdouble matriz[]) {
    mPtr = (elem_Matriz *) malloc(sizeof(elem_Matriz));
    glLoadMatrixd(_selected_object->pMptr->M);
    glMultMatrixd(matriz);
    glGetDoublev(GL_MODELVIEW_MATRIX, mPtr->M);
    mPtr->siguiente = _selected_object->pMptr;
    _selected_object->pMptr = mPtr;
}


/*********************************************************/
/*** FUNCIÓN DE MODIFICACIÓN GLOBAL (POR LA IZQUIERDA) ***/
/*********************************************************/

void multiplicaMatrizGlobal(GLdouble matriz[]) {
    mPtr = (elem_Matriz *) malloc(sizeof(elem_Matriz));
    glLoadIdentity();
    glMultMatrixd(matriz);
    glMultMatrixd(_selected_object->pMptr->M);
    glGetDoublev(GL_MODELVIEW_MATRIX, mPtr->M);
    mPtr->siguiente = _selected_object->pMptr;
    _selected_object->pMptr = mPtr;
}


/********************************************/
/*** FUNCIÓN DE CAMBIO DE BOMBILLA A FOCO ***/
/********************************************/

void cambiaFuenteLuz(int luzSeleccionada) {
    if (luzSeleccionada >= 3) {
        if (luces[luzSeleccionada].tipo==BOMBILLA){
            luces[luzSeleccionada].tipo=FOCO;
            luces[luzSeleccionada].ampFoco=45;
            luces[luzSeleccionada].dir[0]=0;
            luces[luzSeleccionada].dir[1]=-1;
            luces[luzSeleccionada].dir[2]=0;
        }
        else if(luces[luzSeleccionada].tipo==FOCO){
            luces[luzSeleccionada].tipo=BOMBILLA;
            luces[luzSeleccionada].ampFoco=180;
        }
    }
}


void trasladarLuz(int luzSeleccionada, GLfloat x, GLfloat y, GLfloat z) {
    point3 pos={x,y,z};
    point3 newPos;

    GLdouble matrizTraslacion[16];
    glLoadIdentity();
    glTranslatef(x, y, z);
    glGetDoublev(GL_MODELVIEW_MATRIX, matrizTraslacion);

    newPos=multiplicarMatrizPorPunto(matrizTraslacion,pos);
    luces[luzSeleccionada].pos[0]=newPos.x;
    luces[luzSeleccionada].pos[1]=newPos.y;
    luces[luzSeleccionada].pos[2]=newPos.z;


}

void rotacionLuz(int luzSeleccionada, GLfloat x, GLfloat y, GLfloat z) {
    point3 pos={x,y,z};
    point3 newPos;

    GLdouble matrizRotacion[16];
    glLoadIdentity();
    glRotatef(5.0, x, y, z);
    glGetDoublev(GL_MODELVIEW_MATRIX, matrizRotacion);

    newPos=multiplicarMatrizPorPunto(matrizRotacion,pos);
    luces[luzSeleccionada].pos[0]=newPos.x;
    luces[luzSeleccionada].pos[1]=newPos.y;
    luces[luzSeleccionada].pos[2]=newPos.z;


}


/************************************/
/*** FUNCIÓN DE TRASLACIÓN DE LUZ ***/
/************************************/

void ejecutaTraslacionLuz(int key) {
    if (key == GLUT_KEY_UP) {
        trasladarLuz(luzSeleccionada, 1, 0, 0);
    } else if (key == GLUT_KEY_DOWN) {
        trasladarLuz(luzSeleccionada, -1, 0, 0);
    } else if (key == GLUT_KEY_LEFT) {
        trasladarLuz(luzSeleccionada, 0, -1, 0);
    } else if (key == GLUT_KEY_RIGHT) {
        trasladarLuz(luzSeleccionada, 0, 1, 0);
    } else if (key == GLUT_KEY_PAGE_UP) {
        trasladarLuz(luzSeleccionada, 0, 0, 1);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        trasladarLuz(luzSeleccionada, 0, 0, -1);

    }
}


/**********************************/
/*** FUNCIÓN DE ROTACIÓN DE LUZ ***/
/**********************************/

void ejecutaRotacionLuz(int key) {
    if (key == GLUT_KEY_UP) {
        rotacionLuz(luzSeleccionada, 0, 1, 0);
    } else if (key == GLUT_KEY_DOWN) {
        rotacionLuz(luzSeleccionada, 0, -1, 0);
    } else if (key == GLUT_KEY_LEFT) {
        rotacionLuz(luzSeleccionada, -1, 0, 0);
    } else if (key == GLUT_KEY_RIGHT) {
        rotacionLuz(luzSeleccionada, 1, 0, 0);
    } else if (key == GLUT_KEY_PAGE_UP) {
        rotacionLuz(luzSeleccionada, 0, 0, 1);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        rotacionLuz(luzSeleccionada, 0, 0, -1);

    }
}


/************************************************************/
/*** FUNCIÓN DE MODIFICACIÓN DE VOLUMEN SEGUN PERSPECTIVA ***/
/************************************************************/

void ejecutaModificacionVolumen(int key) {
    if (proyeccion == PARALELA) {
        if (key == GLUT_KEY_UP) {
            _ortho_y_max += 0.1;
        } else if (key == GLUT_KEY_DOWN) {
            _ortho_y_max -= 0.1;
        } else if (key == GLUT_KEY_LEFT) {
            _ortho_x_max -= 0.1;
        } else if (key == GLUT_KEY_RIGHT) {
            _ortho_x_max += 0.1;
        } else if (key == GLUT_KEY_PAGE_UP) {
            _ortho_z_max -= 0.1;
            _ortho_z_min -= 0.1;
        } else if (key == GLUT_KEY_PAGE_DOWN) {
            _ortho_z_max += 0.1;
            _ortho_z_min += 0.1;

        }
    } else if (proyeccion == PERSPECTIVA) {
        if (key == GLUT_KEY_UP) {
            _frust_y_max += 0.005;
        } else if (key == GLUT_KEY_DOWN) {
            _frust_y_max -= 0.005;
        } else if (key == GLUT_KEY_LEFT) {
            _frust_x_max -= 0.005;
        } else if (key == GLUT_KEY_RIGHT) {
            _frust_x_max += 0.005;
        } else if (key == GLUT_KEY_PAGE_UP) {
            _frust_z_max -= 0.005;
            _frust_z_min -= 0.005;
        } else if (key == GLUT_KEY_PAGE_DOWN) {
            _frust_z_max += 0.005;
            _frust_z_min += 0.005;

        }
    }

}


/***********************************************/
/*** FUNCIÓN DE TRASLACIÓN CAMARA MODO VUELO ***/
/***********************************************/

void ejecutaTraslacionCamaraVuelo(int key) {
    if (key == GLUT_KEY_UP) {
        trasladarCamaraVuelo(_selected_camera, 0, 0.1, 0);
    } else if (key == GLUT_KEY_DOWN) {
        trasladarCamaraVuelo(_selected_camera, 0, -0.1, 0);
    } else if (key == GLUT_KEY_LEFT) {
        trasladarCamaraVuelo(_selected_camera, -0.1, 0, 0);
    } else if (key == GLUT_KEY_RIGHT) {
        trasladarCamaraVuelo(_selected_camera, 0.1, 0, 0);
    } else if (key == GLUT_KEY_PAGE_UP) {
        trasladarCamaraVuelo(_selected_camera, 0, 0, 0.1);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        trasladarCamaraVuelo(_selected_camera, 0, 0, -0.1);

    }
}


/**************************************************/
/*** FUNCIÓN DE TRASLACIÓN CAMARA MODO ANALISIS ***/
/**************************************************/

void ejecutaTraslacionCamaraAnalisis(int key) {
    if (key == GLUT_KEY_UP) {
        trasladarCamaraAnalisis(_selected_camera, 0, 0.1, 0);
    } else if (key == GLUT_KEY_DOWN) {
        trasladarCamaraAnalisis(_selected_camera, 0, -0.1, 0);
    } else if (key == GLUT_KEY_LEFT) {
        trasladarCamaraAnalisis(_selected_camera, -0.1, 0, 0);
    } else if (key == GLUT_KEY_RIGHT) {
        trasladarCamaraAnalisis(_selected_camera, 0.1, 0, 0);
    } else if (key == GLUT_KEY_PAGE_UP) {
        trasladarCamaraAnalisis(_selected_camera, 0, 0, 0.1);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        trasladarCamaraAnalisis(_selected_camera, 0, 0, -0.1);

    }
}


/****************************************/
/*** FUNCIÓN DE ROTACIÓN CAMARA VUELO ***/
/****************************************/

void ejecutaRotacionCamaraVuelo(int key) {
    if (key == GLUT_KEY_UP) {
        rotacionCamaraVuelo(_selected_camera, 1, 0, 0);
    } else if (key == GLUT_KEY_DOWN) {
        rotacionCamaraVuelo(_selected_camera, -1, 0, 0);
    } else if (key == GLUT_KEY_LEFT) {
        rotacionCamaraVuelo(_selected_camera, 0, -1, 0);
    } else if (key == GLUT_KEY_RIGHT) {
        rotacionCamaraVuelo(_selected_camera, 0, 1, 0);
    } else if (key == GLUT_KEY_PAGE_UP) {
        rotacionCamaraVuelo(_selected_camera, 0, 0, 1);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        rotacionCamaraVuelo(_selected_camera, 0, 0, -1);

    }
}

/*******************************************/
/*** FUNCIÓN DE ROTACIÓN CAMARA ANALISIS ***/
/*******************************************/

void ejecutaRotacionCamaraAnalisis(int key) {
    if (key == GLUT_KEY_UP) {
        rotacionCamaraAnalisis(_selected_camera, 1, 0, 0);
    } else if (key == GLUT_KEY_DOWN) {
        rotacionCamaraAnalisis(_selected_camera, -1, 0, 0);
    } else if (key == GLUT_KEY_LEFT) {
        rotacionCamaraAnalisis(_selected_camera, 0, -1, 0);
    } else if (key == GLUT_KEY_RIGHT) {
        rotacionCamaraAnalisis(_selected_camera, 0, 1, 0);
    } else if (key == GLUT_KEY_PAGE_UP) {
        rotacionCamaraAnalisis(_selected_camera, 0, 0, 1);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        rotacionCamaraAnalisis(_selected_camera, 0, 0, -1);
    }
}

/*************************************/
/*** FUNCIÓN DE ESCALADO HOMOGENEO ***/
/*************************************/

void escaladoHomogeneo(int signo) {
    GLdouble escala_Plus[16] = {1.05, 0, 0, 0, 0, 1.05, 0, 0, 0, 0, 1.05, 0, 0, 0, 0, 1};
    GLdouble escala_Minus[16] = {0.95, 0, 0, 0, 0, 0.95, 0, 0, 0, 0, 0.95, 0, 0, 0, 0, 1};


    if (signo == 0) {
        multiplicaMatriz(escala_Plus);
    } else
        multiplicaMatriz(escala_Minus);

}


/*********************************/
/*** FUNCIÓN DE ROTACIÓN LOCAL ***/
/*********************************/

void ejecutaRotacion(int key, int x, int y) {
    GLdouble rota_respecto_x_plus[16] = {1, 0, 0, 0, 0, cos(0.05), sin(0.05), 0, 0, -sin(0.05), cos(0.05), 0, 0, 0, 0, 1};
    GLdouble rota_respecto_x_minus[16] = {1, 0, 0, 0, 0, cos(-0.05), sin(-0.05), 0, 0, -sin(-0.05), cos(-0.05), 0, 0, 0, 0, 1};

    GLdouble rota_respecto_y_plus[16] = {cos(0.05), 0, -sin(0.05), 0, 0, 1, 0, 0, sin(0.05), 0, cos(0.05), 0, 0, 0, 0, 1};
    GLdouble rota_respecto_y_minus[16] = {cos(-0.05), 0, -sin(-0.05), 0, 0, 1, 0, 0, sin(-0.05), 0, cos(-0.05), 0, 0, 0, 0, 1};

    GLdouble rota_respecto_z_plus[16] = {cos(0.05), sin(0.05), 0, 0, -sin(0.05), cos(0.05), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    GLdouble rota_respecto_z_minus[16] = {cos(-0.05), sin(-0.05), 0, 0, -sin(-0.05), cos(-0.05), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    if (key == GLUT_KEY_UP) {
        multiplicaMatriz(rota_respecto_x_plus);
    } else if (key == GLUT_KEY_LEFT) {
        multiplicaMatriz(rota_respecto_y_minus);
    } else if (key == GLUT_KEY_DOWN) {
        multiplicaMatriz(rota_respecto_x_minus);
    } else if (key == GLUT_KEY_RIGHT) {
        multiplicaMatriz(rota_respecto_y_plus);
    } else if (key == GLUT_KEY_PAGE_UP) {
        multiplicaMatriz(rota_respecto_z_minus);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        multiplicaMatriz(rota_respecto_z_plus);
    }
}

/*********************************/
/*** FUNCIÓN DE ESCALADO LOCAL ***/
/*********************************/

void ejecutaEscalado(int key, int x, int y) {
    GLdouble escala_x_Plus[16] = {1.05, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    GLdouble escala_x_Minus[16] = {0.95, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    GLdouble escala_y_Plus[16] = {1, 0, 0, 0, 0, 1.05, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    GLdouble escala_y_Minus[16] = {1, 0, 0, 0, 0, 0.95, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    GLdouble escala_z_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1.05, 0, 0, 0, 0, 1};
    GLdouble escala_z_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.95, 0, 0, 0, 0, 1};

    if (key == GLUT_KEY_UP) {
        multiplicaMatriz(escala_y_Plus);
    } else if (key == GLUT_KEY_LEFT) {
        multiplicaMatriz(escala_x_Minus);
    } else if (key == GLUT_KEY_DOWN) {
        multiplicaMatriz(escala_y_Minus);
    } else if (key == GLUT_KEY_RIGHT) {
        multiplicaMatriz(escala_x_Plus);
    } else if (key == GLUT_KEY_PAGE_UP) {
        multiplicaMatriz(escala_z_Minus);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        multiplicaMatriz(escala_z_Plus);
    }
}


/***********************************/
/*** FUNCIÓN DE TRASLACIÓN LOCAL ***/
/***********************************/

void ejecutaTraslacion(int key, int x, int y) {
    GLdouble tras_x_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0.2, 0, 0, 1};
    GLdouble tras_x_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -0.2, 0, 0, 1};

    GLdouble tras_y_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.2, 0, 1};
    GLdouble tras_y_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, -0.2, 0, 1};

    GLdouble tras_z_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0.2, 1};
    GLdouble tras_z_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -0.2, 1};

    if (key == GLUT_KEY_UP) {
        multiplicaMatriz(tras_y_Plus);
    } else if (key == GLUT_KEY_LEFT) {
        multiplicaMatriz(tras_x_Minus);
    } else if (key == GLUT_KEY_DOWN) {
        multiplicaMatriz(tras_y_Minus);
    } else if (key == GLUT_KEY_RIGHT) {
        multiplicaMatriz(tras_x_Plus);
    } else if (key == GLUT_KEY_PAGE_UP) {
        multiplicaMatriz(tras_z_Minus);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        multiplicaMatriz(tras_z_Plus);
    }
}


/**********************************/
/*** FUNCIÓN DE ROTACIÓN GLOBAL ***/
/**********************************/

void ejecutaRotacionGlobal(int key, int x, int y) {
    GLdouble rota_respecto_x_plus[16] = {1, 0, 0, 0, 0, cos(0.1), sin(0.1), 0, 0, -sin(0.1), cos(0.1), 0, 0, 0, 0, 1};
    GLdouble rota_respecto_x_minus[16] = {1, 0, 0, 0, 0, cos(-0.1), sin(-0.1), 0, 0, -sin(-0.1), cos(-0.1), 0, 0, 0, 0, 1};

    GLdouble rota_respecto_y_plus[16] = {cos(0.1), 0, -sin(0.1), 0, 0, 1, 0, 0, sin(0.1), 0, cos(0.1), 0, 0, 0, 0, 1};
    GLdouble rota_respecto_y_minus[16] = {cos(-0.1), 0, -sin(-0.1), 0, 0, 1, 0, 0, sin(-0.1), 0, cos(-0.1), 0, 0, 0, 0, 1};

    GLdouble rota_respecto_z_plus[16] = {cos(0.1), sin(0.1), 0, 0, -sin(0.1), cos(0.1), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    GLdouble rota_respecto_z_minus[16] = {cos(-0.1), sin(-0.1), 0, 0, -sin(-0.1), cos(-0.1), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    if (key == GLUT_KEY_UP) {
        multiplicaMatrizGlobal(rota_respecto_x_plus);
    } else if (key == GLUT_KEY_LEFT) {
        multiplicaMatrizGlobal(rota_respecto_y_minus);
    } else if (key == GLUT_KEY_DOWN) {
        multiplicaMatrizGlobal(rota_respecto_x_minus);
    } else if (key == GLUT_KEY_RIGHT) {
        multiplicaMatrizGlobal(rota_respecto_y_plus);
    } else if (key == GLUT_KEY_PAGE_UP) {
        multiplicaMatrizGlobal(rota_respecto_z_minus);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        multiplicaMatrizGlobal(rota_respecto_z_plus);
    }
}


/************************************/
/*** FUNCIÓN DE TRASLACIÓN GLOBAL ***/
/************************************/

void ejecutaTraslacionGlobal(int key, int x, int y) {
    GLdouble tras_x_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0.1, 0, 0, 1};
    GLdouble tras_x_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -0.1, 0, 0, 1};

    GLdouble tras_y_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.1, 0, 1};
    GLdouble tras_y_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, -0.1, 0, 1};

    GLdouble tras_z_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0.1, 1};
    GLdouble tras_z_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -0.1, 1};

    if (key == GLUT_KEY_UP) {
        multiplicaMatrizGlobal(tras_y_Plus);
    } else if (key == GLUT_KEY_LEFT) {
        multiplicaMatrizGlobal(tras_x_Minus);
    } else if (key == GLUT_KEY_DOWN) {
        multiplicaMatrizGlobal(tras_y_Minus);
    } else if (key == GLUT_KEY_RIGHT) {
        multiplicaMatrizGlobal(tras_x_Plus);
    } else if (key == GLUT_KEY_PAGE_UP) {
        multiplicaMatrizGlobal(tras_z_Minus);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        multiplicaMatrizGlobal(tras_z_Plus);
    }
}


/**********************************/
/*** FUNCIÓN DE ESCALADO GLOBAL ***/
/**********************************/

void ejecutaEscaladoGlobal(int key, int x, int y) {
    GLdouble escala_x_Plus[16] = {1.05, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    GLdouble escala_x_Minus[16] = {0.95, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    GLdouble escala_y_Plus[16] = {1, 0, 0, 0, 0, 1.05, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    GLdouble escala_y_Minus[16] = {1, 0, 0, 0, 0, 0.95, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    GLdouble escala_z_Plus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1.05, 0, 0, 0, 0, 1};
    GLdouble escala_z_Minus[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.95, 0, 0, 0, 0, 1};

    if (key == GLUT_KEY_UP) {
        multiplicaMatrizGlobal(escala_y_Plus);
    } else if (key == GLUT_KEY_LEFT) {
        multiplicaMatrizGlobal(escala_x_Minus);

    } else if (key == GLUT_KEY_DOWN) {
        multiplicaMatrizGlobal(escala_y_Minus);

    } else if (key == GLUT_KEY_RIGHT) {
        multiplicaMatrizGlobal(escala_x_Plus);

    } else if (key == GLUT_KEY_PAGE_UP) {
        multiplicaMatrizGlobal(escala_z_Minus);

    } else if (key == GLUT_KEY_PAGE_DOWN) {
        multiplicaMatrizGlobal(escala_z_Plus);
    }
}

vector3 productoVectorial(vector3 u, vector3 v) {
    vector3 res;
    res.x = u.y * v.z - v.y * u.z;
    res.y = v.x * u.z - u.x * v.z;
    res.z = u.x * v.y - v.x * u.y;
    return res;
}

void rotacionCamara(int key, int x, int y) {
    GLdouble rota_respecto_x_plus[16] = {1, 0, 0, 0, 0, cos(0.05), sin(0.05), 0, 0, -sin(0.05), cos(0.05), 0, 0, 0, 0, 1};
    GLdouble rota_respecto_x_minus[16] = {1, 0, 0, 0, 0, cos(-0.05), sin(-0.05), 0, 0, -sin(-0.05), cos(-0.05), 0, 0, 0,
                                          0, 1};

    GLdouble rota_respecto_y_plus[16] = {cos(0.05), 0, -sin(0.05), 0, 0, 1, 0, 0, sin(0.05), 0, cos(0.05), 0, 0, 0, 0, 1};
    GLdouble rota_respecto_y_minus[16] = {cos(-0.05), 0, -sin(-0.05), 0, 0, 1, 0, 0, sin(-0.05), 0, cos(-0.05), 0, 0, 0, 0, 1};

    GLdouble rota_respecto_z_plus[16] = {cos(0.05), sin(0.05), 0, 0, -sin(0.05), cos(0.05), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
    GLdouble rota_respecto_z_minus[16] = {cos(-0.05), sin(-0.05), 0, 0, -sin(-0.05), cos(-0.05), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    if (key == GLUT_KEY_UP) {
        multiplicaMatrizCamara(rota_respecto_x_plus);
    } else if (key == GLUT_KEY_LEFT) {
        multiplicaMatrizCamara(rota_respecto_y_minus);
    } else if (key == GLUT_KEY_DOWN) {
        multiplicaMatrizCamara(rota_respecto_x_minus);
    } else if (key == GLUT_KEY_RIGHT) {
        multiplicaMatrizCamara(rota_respecto_y_plus);
    } else if (key == GLUT_KEY_PAGE_UP) {
        multiplicaMatrizCamara(rota_respecto_z_minus);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        multiplicaMatrizCamara(rota_respecto_z_plus);
    }

}


/*****************************************/
/*** FUNCIÓN DE ACCIÓN DE FLECHAS Y FX ***/
/*****************************************/

void k_arrow(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_F9:
            if (luces_activadas == 1) {
                printf("\n*** ACTIVADO : LUCES ***\n");
                glEnable(GL_LIGHTING);
                luces_activadas = 0;
            } else {
                printf("\n*** DESACTIVADO : LUCES ***\n");
                glDisable(GL_LIGHTING);
                luces_activadas = 1;
            }
            break;
        case GLUT_KEY_F1:
            if (luces[0].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ  - SOL ***\n");
                glEnable(GL_LIGHT0);
                luces[0].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ  - SOL ***\n");
                glDisable(GL_LIGHT0);
                luces[0].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F2:
            if (luces[1].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ 1 - BOMBILLA ***\n");
                glEnable(GL_LIGHT1);
                luces[1].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ 1 - BOMBILLA ***\n");
                glDisable(GL_LIGHT1);
                luces[1].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F3:
            if (luces[2].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ 2 - FOCO OBJETO ***\n");
                glEnable(GL_LIGHT2);
                luces[2].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ 2 - FOCO OBJETO ***\n");
                glDisable(GL_LIGHT2);
                luces[2].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F4:
            if (luces[3].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ 3 ***\n");
                glEnable(GL_LIGHT3);
                luces[3].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ 3 ***\n");
                glDisable(GL_LIGHT3);
                luces[3].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F5:
            if (luces[4].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ 4 ***\n");
                glEnable(GL_LIGHT4);
                luces[4].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ 4 ***\n");
                glDisable(GL_LIGHT4);
                luces[4].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F6:
            if (luces[5].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ 5 ***\n");
                glEnable(GL_LIGHT5);
                luces[5].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ 5 ***\n");
                glDisable(GL_LIGHT5);
                luces[5].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F7:
            if (luces[6].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ 6 ***\n");
                glEnable(GL_LIGHT6);
                luces[6].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ 6 ***\n");
                glDisable(GL_LIGHT6);
                luces[6].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F8:
            if (luces[7].estado==APAGADA) {
                printf("\n*** ACTIVADA : LUZ 7 ***\n");
                glEnable(GL_LIGHT7);
                luces[7].estado=ENCENDIDA;
            } else {
                printf("\n*** DESACTIVADA : LUZ 7 ***\n");
                glDisable(GL_LIGHT7);
                luces[7].estado=APAGADA;
            }
            break;
        case GLUT_KEY_F12:
            if (shading == SMOOTH) {
                shading = FLAT;
                glShadeModel(GL_FLAT);
            } else {
                shading = SMOOTH;
                glShadeModel(GL_SMOOTH);
            }
            break;

    }
    if (modo == OBJETO) {
	if(_selected_object!=0) {
		if (referencia == LOCAL) {
		    if (estado == ROTACION)
		        ejecutaRotacion(key, x, y);
		    else if (estado == ESCALADO)
		        ejecutaEscalado(key, x, y);
		    else if (estado == TRASLACION)
		        ejecutaTraslacion(key, x, y);
		} else {
		    if (estado == ROTACION)
		        ejecutaRotacionGlobal(key, x, y);
		    else if (estado == ESCALADO)
		        ejecutaEscaladoGlobal(key, x, y);
		    else if (estado == TRASLACION)
		        ejecutaTraslacionGlobal(key, x, y);
		}
		}
	    } else if (modo == CAMARA) {
		if (estado == ROTACION) {
		    printf("\n*** CAMARA : ROTANDO CAMARA ***\n");
		    if (modoCamara == VUELO)
		        ejecutaRotacionCamaraVuelo(key);
		    else if (modoCamara == ANALISIS) {
		        ejecutaRotacionCamaraAnalisis(key);
		    }
		} else if (estado == TRASLACION) {
		    printf("\n*** TRASLACION : MOVIENDO CAMARA ***\n");
		    if (modoCamara == VUELO)
		        ejecutaTraslacionCamaraVuelo(key);
		    else if (modoCamara == ANALISIS)
		        ejecutaTraslacionCamaraAnalisis(key);
		} else if (estado == VOLUMEN) {
		    printf("\n*** VOLUMEN : MODIFICANDO VOLUMEN ***\n");
		    ejecutaModificacionVolumen(key);
		}
	    } else if (modo == LUZ) {
		if (luzSeleccionada != SOL) {
		    if (estado == TRASLACION) {
		        ejecutaTraslacionLuz(key);
		    }
		    else if(estado==ROTACION){
		        ejecutaRotacionLuz(key);
		    }
		}
	
    }
    glutPostRedisplay();
}


/*************************************/
/*** FUNCIÓN DE DESHACER Y REHACER ***/
/*************************************/

void redo(void) {
    auxPtr = _selected_object->pMptr->siguiente;
    _selected_object->pMptr->siguiente = _selected_object->containerNodePtr;
    _selected_object->containerNodePtr = _selected_object->pMptr;
    _selected_object->pMptr = auxPtr;
}

/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {

    char *fname = malloc(sizeof(char) * 128); /* Note that scanf adds a null character at the end of the vector*/
    int read = 0;
    object3d *auxiliar_object = 0;
    GLdouble wd, he, midx, midy;


    switch (key) {
        case 'k':
        case 'K':
            printf("\n*** ACTIVADO : MODO CAMARA ***\n");
            modo = CAMARA;
            break;
        case 'o':
        case 'O':
            printf("\n*** ACTIVADO : MODO OBJETOS ***\n");
            modo = OBJETO;
            break;
        case 'b':
        case 'B':
            printf("\n*** ACTIVADO : MODO ROTACION ***\n");
            estado = ROTACION;
            break;
        case 't':
        case 'T':
            if (modo == OBJETO) {
                printf("\n*** ACTIVADO : MODO ESCALADO ***\n");
                estado = ESCALADO;
            } else if (modo == CAMARA) {
                printf("\n*** ACTIVADO : MODO VOLUMEN ***\n");
                estado = VOLUMEN;

            }
            break;
        case 'p':
        case 'P':
            if (proyeccion == PARALELA) {
                printf("\n*** ACTIVADO : PROYECCION PERSPECTIVA ***\n");
                proyeccion = PERSPECTIVA;
            } else if (proyeccion == PERSPECTIVA) {
                printf("\n*** ACTIVADO : PROYECCION PARALELA ***\n");
                proyeccion = PARALELA;
            }
            break;
        case 'm':
        case 'M':
            printf("\n*** ACTIVADO : MODO TRASLACION ***\n");
            estado = TRASLACION;
            break;
        case 'g':
        case 'G':
            if (modo == OBJETO) {
                printf("\n*** ACTIVADO : REFERENCIA GLOBAL ***\n");
                referencia = GLOBAL;
            } else if (modo == CAMARA) {
                printf("\n*** ACTIVADO : MODO ANALISIS ***\n");
                modoCamara = ANALISIS;
                mirarObjeto(_selected_camera, _selected_object);
            }
            break;
        case 'l':
        case 'L':
            if (modo == OBJETO) {
                printf("\n*** ACTIVADO : REFERENCIA LOCAL ***\n");
                referencia = LOCAL;
            } else if (modo == CAMARA) {
                printf("\n*** ACTIVADO : MODO VUELO ***\n");
                modoCamara = VUELO;
            }
            break;
        case '.':
            if (modo == OBJETO) {
                print_matrix(_selected_object->pMptr->M);
            } else
                print_matrix(_selected_camera->matrizCamara);
            break;
        case 48:
            printf("\n*** ACTIVADO : CAMBIAR FUENTE DE LUZ ***\n");
            cambiaFuenteLuz(luzSeleccionada);
            break;
        case 49: /* <1> */
            printf("\n*** ACTIVADA : LUZ 0 ***\n");
            luzSeleccionada = 0;
            break;
        case 50: /* <2> */
            printf("\n*** ACTIVADA : LUZ 1 ***\n");
            luzSeleccionada = 1;
            break;
        case 51: /* <3> */
            printf("\n*** ACTIVADA : LUZ 2 ***\n");
            luzSeleccionada = 2;
            break;
        case 52: /* <4> */
            printf("\n*** ACTIVADA : LUZ 3 ***\n");
            luzSeleccionada = 3;
            break;
        case 53: /* <5> */
            printf("\n*** ACTIVADA : LUZ 4 ***\n");
            luzSeleccionada = 4;
            break;
        case 54: /* <6> */
            printf("\n*** ACTIVADA : LUZ 5 ***\n");
            luzSeleccionada = 5;
            break;
        case 55: /* <7> */
            printf("\n*** ACTIVADA : LUZ 6 ***\n");
            luzSeleccionada = 6;
            break;
        case 56: /* <8> */
            printf("\n*** ACTIVADA : LUZ 7 ***\n");
            luzSeleccionada = 7;
            break;
        case 'a':
        case 'A':
            modo = LUZ;
            printf("\n*** ACTIVADO : MODO LUZ ***\n");
            break;
        case 'f':
        case 'F':
            /*Ask for file*/
            printf("%s", KG_MSSG_SELECT_FILE);
            scanf("%s", fname);
            /*Allocate memory for the structure and read the file*/
            auxiliar_object = (object3d *) malloc(sizeof(object3d));
            read = read_wavefront(fname, auxiliar_object);
            switch (read) {
                /*Errors in the reading*/
                case 1:
                    printf("%s: %s\n", fname, KG_MSSG_FILENOTFOUND);
                    break;
                case 2:
                    printf("%s: %s\n", fname, KG_MSSG_INVALIDFILE);
                    break;
                case 3:
                    printf("%s: %s\n", fname, KG_MSSG_EMPTYFILE);
                    break;
                case 0:
                    /*Insert the new object in the list*/
                    auxiliar_object->siguiente = _first_object;
                    _first_object = auxiliar_object;
                    _selected_object = _first_object;
                    auxiliar_object->pMptr = (elem_Matriz *) malloc(sizeof(elem_Matriz));
                    auxiliar_object->containerNodePtr = (elem_Matriz *) malloc(sizeof(elem_Matriz));
                    printf("%s\n", KG_MSSG_FILEREAD);
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    glGetDoublev(GL_MODELVIEW_MATRIX, auxiliar_object->pMptr->M);
                    auxiliar_object->pMptr->siguiente = 0;
                    break;
            }

            break;
        case 'c':
        case 'C':
            inversa(_selected_object->pMptr->M, _selected_camera->matrizCamara);
            printf("\n*** ACTIVADA : CAMARA DE OBJETO ***\n");
            break;
        case 9: /* <TAB> */
            if (_selected_object != 0) {
                if (_selected_object->siguiente != 0)
                    _selected_object = _selected_object->siguiente;
                else
                    _selected_object = _first_object;
                /*The selection is circular, thus if we move out of the list we go back to the first element*/
            } else _selected_object = _first_object;
            break;
        case 127: /* <SUPR> */
            	/*Erasing an object depends on whether it is the first one or not*/
		if(_first_object != 0 ) {
		    if (_selected_object == _first_object) {
		        /*To remove the first object we just set the first as the current's siguiente*/
		        _first_object = _first_object->siguiente;
		        /*Once updated the pointer to the first object it is save to free the memory*/
		        free(_selected_object);
		        /*Finally, set the selected to the new first one*/
		        _selected_object = _first_object;
		    } else {
		        /*In this case we need to get the previous element to the one we want to erase*/
		        auxiliar_object = _first_object;
		        while (auxiliar_object->siguiente != _selected_object)
		            auxiliar_object = auxiliar_object->siguiente;
		        /*Now we bypass the element to erase*/
		        auxiliar_object->siguiente = _selected_object->siguiente;
		        /*free the memory*/
		        free(_selected_object);
		        /*and update the selection*/
		        _selected_object = auxiliar_object;
		    }
		}
		break;

        case '-':
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
                /*Increase the projection plane; compute the new dimensions*/
                wd = (_ortho_x_max - _ortho_x_min) / KG_STEP_ZOOM;
                he = (_ortho_y_max - _ortho_y_min) / KG_STEP_ZOOM;
                /*In order to avoid moving the center of the plane, we get its coordinates*/
                midx = (_ortho_x_max + _ortho_x_min) / 2;
                midy = (_ortho_y_max + _ortho_y_min) / 2;
                /*The the new limits are set, keeping the center of the plane*/
                _ortho_x_max = midx + wd / 2;
                _ortho_x_min = midx - wd / 2;
                _ortho_y_max = midy + he / 2;
                _ortho_y_min = midy - he / 2;
            } else if (modo == OBJETO && estado == ESCALADO) {
                escaladoHomogeneo(1);
            } else if (luces[luzSeleccionada].tipo==FOCO) {
                if(luces[luzSeleccionada].ampFoco<=0){
                    printf("\n*** AVISO : MODIFICACION NO PERMITIDA, AMPLITUD = 0 ***\n");
                }else
                    printf("DISMINUYENDO ANGULO DE APERTURA %f\n", luces[luzSeleccionada].ampFoco--);
            }
            break;


        case '+':
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
                /*Increase the projection plane; compute the new dimensions*/
                wd = (_ortho_x_max - _ortho_x_min) * KG_STEP_ZOOM;
                he = (_ortho_y_max - _ortho_y_min) * KG_STEP_ZOOM;
                /*In order to avoid moving the center of the plane, we get its coordinates*/
                midx = (_ortho_x_max + _ortho_x_min) * 2;
                midy = (_ortho_y_max + _ortho_y_min) * 2;
                /*The the new limits are set, keeping the center of the plane*/
                _ortho_x_max = midx + wd / 2;
                _ortho_x_min = midx - wd / 2;
                _ortho_y_max = midy + he / 2;
                _ortho_y_min = midy - he / 2;
            } else if (modo == OBJETO && estado == ESCALADO) {
                escaladoHomogeneo(0);

            } else if (luces[luzSeleccionada].tipo==FOCO) {
                if(luces[luzSeleccionada].ampFoco>=90){
                    printf("\n*** AVISO : MODIFICACION NO PERMITIDA, AMPLITUD = 90 ***\n");
                }else
                    printf("AUMENTANDO ANGULO DE APERTURA %f\n", luces[luzSeleccionada].ampFoco++);
            }
            break;
        case 26: /* <CTRL + Z>- undo */
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
                if (_selected_object) {
                    if (_selected_object->pMptr->siguiente != 0) {
                        redo();
                    } else
                        printf("\n*** AVISO : NADA MAS PARA DESHACER ***\n");
                }
            }
            break;
        case 24:/* <CTRL + X> - redo*/
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
                if (_selected_object->containerNodePtr->siguiente != 0) {
                    auxPtr = _selected_object->containerNodePtr->siguiente;
                    _selected_object->containerNodePtr->siguiente = _selected_object->pMptr;
                    _selected_object->pMptr = _selected_object->containerNodePtr;
                    _selected_object->containerNodePtr = auxPtr;
                } else {
                    printf("\n*** AVISO : NADA MAS PARA REHACER ***\n");
                }
            }

            break;

        case '?':
            print_help();
            break;

        case 27: /* <ESC> */
            exit(0);
            break;
        default:
            /*In the default case we just print the code of the key. This is usefull to define new cases*/
            printf("%d %c\n", key, key);
    }
    /*In case we have do any modification affecting the displaying of the object, we redraw them*/
    glutPostRedisplay();
}

