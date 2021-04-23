#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <GL/gl.h>

/** DEFINITIONS **/

#define KG_WINDOW_TITLE                     "Practica GC - Leroy Deniz"
#define KG_WINDOW_WIDTH                     1200
#define KG_WINDOW_HEIGHT                    900
#define KG_WINDOW_X                         50
#define KG_WINDOW_Y                         50

#define KG_MSSG_SELECT_FILE                 "Ruta al objeto: "
#define KG_MSSG_FILENOTFOUND                "El objeto no existe."
#define KG_MSSG_INVALIDFILE                 "Hubo un problema con el archivo."
#define KG_MSSG_EMPTYFILE                   "Fichero vacío."
#define KG_MSSG_FILEREAD                    "Lectura de archivo completada."

#define KG_STEP_MOVE                        5.0f
#define KG_STEP_ROTATE                      10.0f
#define KG_STEP_ZOOM                        0.75
#define KG_STEP_CAMERA_ANGLE                5.0f

#define KG_ORTHO_X_MIN_INIT                -5
#define KG_ORTHO_X_MAX_INIT                 5
#define KG_ORTHO_Y_MIN_INIT                -5
#define KG_ORTHO_Y_MAX_INIT                 5
#define KG_ORTHO_Z_MIN_INIT                -100
#define KG_ORTHO_Z_MAX_INIT                 10000

#define KG_COL_BACK_R                       0.30f
#define KG_COL_BACK_G                       0.30f
#define KG_COL_BACK_B                       0.30f
#define KG_COL_BACK_A                       1.00f

#define KG_COL_SELECTED_R                   1.00f
#define KG_COL_SELECTED_G                   0.75f
#define KG_COL_SELECTED_B                   0.00f

#define KG_COL_NONSELECTED_R                1.00f
#define KG_COL_NONSELECTED_G                1.00f
#define KG_COL_NONSELECTED_B                1.00f

#define KG_COL_X_AXIS_R                     1.0f
#define KG_COL_X_AXIS_G                     0.0f
#define KG_COL_X_AXIS_B                     0.0f

#define KG_COL_Y_AXIS_R                     0.0f
#define KG_COL_Y_AXIS_G                     1.0f
#define KG_COL_Y_AXIS_B                     0.0f

#define KG_COL_Z_AXIS_R                     0.0f
#define KG_COL_Z_AXIS_G                     0.0f
#define KG_COL_Z_AXIS_B                     1.0f

#define KG_MAX_DOUBLE                       8000

#define ROTACION 0
#define ESCALADO 1
#define TRASLACION 2
#define VOLUMEN 3

#define GLOBAL 0
#define LOCAL 1

#define OBJETO 0
#define CAMARA 1
#define LUZ 2

#define VUELO 0
#define ANALISIS 1

#define PARALELA 0
#define PERSPECTIVA 1

#define SMOOTH 0
#define FLAT 1

#define SOL 0
#define BOMB1 1
#define FOCO1 2
#define LUZ3 3
#define LUZ4 4
#define LUZ5 5
#define LUZ6 6
#define LUZ7 7


#define SOL 0
#define BOMBILLA 1
#define FOCO 2

#define ENCENDIDA 1
#define APAGADA 0

/** ESTRUCTURAS **/

/****************************
 * Structure to store the   *
 * coordinates of 3D points *
 ****************************/
typedef struct {
    GLdouble x, y, z;
} point3;

/*****************************
 * Structure to store the    *
 * coordinates of 3D vectors *
 *****************************/
typedef struct {
    GLdouble x, y, z;
} vector3;

/****************************
 * Structure to store the   *
 * colors in RGB mode       *
 ****************************/
typedef struct {
    GLfloat r, g, b;
} color3;

/****************************
 * Structure to store       *
 * objects' vertices        *
 ****************************/
typedef struct {
    vector3 normal;
    point3 coord;                       /* coordinates,x, y, z */
    GLint num_faces;                    /* number of faces that share this vertex */
} vertex;


/****************************
 * Structure to store       *
 * objects' faces or        *
 * polygons                 *
 ****************************/
typedef struct {
    vector3 normal;
    GLint num_vertices;                 /* number of vertices in the face */
    GLint *vertex_table;                /* table with the index of each vertex */
} face;


/****************************
 * Estructura de matriz     *
 ****************************/
typedef struct elem_Matriz{
    GLdouble M[16];                /*matriz de la tranformacion*/
    struct elem_Matriz *siguiente; /*Puntero al siguiente nodo*/
}elem_Matriz;


/****************************
 * Estructura de materiales *
 ****************************/
typedef struct material{
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
}material;


/****************************
 * Estructura de objetos    *
 ****************************/
struct object3d{
    GLint num_vertices;                     /* number of vertices in the object*/
    vertex *vertex_table;                   /* tabla de vértices */
    GLint num_faces;                        /* número de caras del objeto */
    face *face_table;                       /* tabla de caras */
    point3 min;                             /* coordinates' lower bounds */
    point3 max;                             /* coordinates' bigger bounds */
    material mt;			    /* definición del material sobre el objeto */
    elem_Matriz *pMptr;          	    /* puntero a la matriz de transformaciones*/
    elem_Matriz *containerNodePtr;          /* puntero al primer nodo containerNode*/
    struct object3d *siguiente;             /* puntero al siguiente objeto */
};


/****************************
 * Estructura de cámaras    *
 ****************************/
typedef struct camara{
    point3 e;
    point3 at;
    vector3 up;
    GLdouble matrizCamara[16];
}camara;


/****************************
 * Estructura de luces      *
 ****************************/
typedef struct luz{
    int tipo;
    int estado;
    GLfloat pos[4];
    GLfloat dir[3];
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat ampFoco;
}luz;

typedef struct object3d object3d;

#endif // DEFINITIONS_H
