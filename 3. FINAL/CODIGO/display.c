#include "definitions.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>

/** EXTERNAL VARIABLES **/

extern GLdouble _window_ratio;
extern GLdouble _ortho_x_min, _ortho_x_max;
extern GLdouble _ortho_y_min, _ortho_y_max;
extern GLdouble _ortho_z_min, _ortho_z_max;

extern GLdouble _frust_x_min,_frust_x_max;
extern GLdouble _frust_y_min ,_frust_y_max;
extern GLdouble _frust_z_min,_frust_z_max;

extern object3d *_first_object;
extern object3d *_selected_object;
extern camara *_selected_camera;
extern luz luces[8];
extern int shading;
extern GLint GL_LIGHT[8];

extern int proyeccion;

void load_object_camera(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLdouble v[9]={1,0,0,0,0,0,0,1,0};
    gluLookAt(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]);

}

void calculaLuces(){
    int i;
    glLightfv(GL_LIGHT[0], GL_POSITION, luces[0].pos);
    for (i=1;i<8;i++) {
        glLightfv(GL_LIGHT[i], GL_POSITION, luces[i].pos);
        glLightfv(GL_LIGHT[i], GL_SPOT_DIRECTION, luces[i].dir);
        glLightf(GL_LIGHT[i],GL_SPOT_CUTOFF,luces[i].ampFoco);

        if(i==1){
            i=3;
        }
    }

}

void cargaFocoObjeto(){
    glLoadMatrixd(_selected_object->pMptr->M);
    luces[2].pos[0]=0.0;
    luces[2].pos[1]=0.0;
    luces[2].pos[2]=0.0;

    luces[2].dir[0]=0.0;
    luces[2].dir[1]=0.0;
    luces[2].dir[2]=1.0;

    glLightfv(GL_LIGHT[2], GL_POSITION, luces[2].pos);
    glLightfv(GL_LIGHT[2], GL_SPOT_DIRECTION, luces[2].dir);
}

/**
 * @brief Function to draw the axes
 */
void draw_axes() {
    /*Draw X axis*/
    glColor3f(KG_COL_X_AXIS_R, KG_COL_X_AXIS_G, KG_COL_X_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(KG_MAX_DOUBLE, 0, 0);
    glVertex3d(-1 * KG_MAX_DOUBLE, 0, 0);
    glEnd();
    /*Draw Y axis*/
    glColor3f(KG_COL_Y_AXIS_R, KG_COL_Y_AXIS_G, KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0, KG_MAX_DOUBLE, 0);
    glVertex3d(0, -1 * KG_MAX_DOUBLE, 0);
    glEnd();
    /*Draw Z axis*/
    glColor3f(KG_COL_Z_AXIS_R, KG_COL_Z_AXIS_G, KG_COL_Z_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0, 0, KG_MAX_DOUBLE);
    glVertex3d(0, 0, -1 * KG_MAX_DOUBLE);
    glEnd();
}
/**
 * @brief Calcula la inversa de la matriz m
 * @param m Matriz a invertir
 * @param inv inversa de la matriz
 */

/**
 * @brief Callback reshape function. We just store the new proportions of the window
 * @param width New width of the window
 * @param height New height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    /*  Take care, the width and height are integer numbers, but the ratio is a GLdouble so, in order to avoid
     *  rounding the ratio to integer values we need to cast width and height before computing the ratio */
    _window_ratio = (GLdouble) width / (GLdouble) height;
}



/**
 * @brief Callback display function
 */

void display(void) {
    GLdouble inv[16];
    GLint v_index, v, f;
    object3d *aux_obj = _first_object;
    /* Clear the screen */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*When the window is wider than our original projection plane we extend the plane in the X axis*/
    if (proyeccion == PARALELA) {
        if ((_ortho_x_max - _ortho_x_min) / (_ortho_y_max - _ortho_y_min) < _window_ratio) {
            //* New width *//*
                    GLdouble wd = (_ortho_y_max - _ortho_y_min) * _window_ratio;
            //* Midpoint in the X axis *//*
                    GLdouble midpt = (_ortho_x_min + _ortho_x_max) / 2;
            //*Definition of the projection*//*
                    glOrtho(midpt - (wd / 2), midpt + (wd / 2), _ortho_y_min, _ortho_y_max, _ortho_z_min, _ortho_z_max);
        } else {
            //* In the opposite situation we extend the Y axis *//*
                    //* New height *//*
            GLdouble he = (_ortho_x_max - _ortho_x_min) / _window_ratio;
            //* Midpoint in the Y axis *//*
            GLdouble midpt = (_ortho_y_min + _ortho_y_max) / 2;
            //*Definition of the projection*//*
            glOrtho(_ortho_x_min, _ortho_x_max, midpt - (he / 2), midpt + (he / 2), _ortho_z_min, _ortho_z_max);
        }
    }
    else if(proyeccion==PERSPECTIVA){
        glFrustum(_frust_x_min,_frust_x_max,_frust_y_min,_frust_y_max,_frust_z_min,_frust_z_max);
    }
    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*First, we draw the axes*/
    draw_axes();


    glLoadMatrixd(_selected_camera->matrizCamara);
    calculaLuces();
    glLoadIdentity();

    /*Now each of the objects in the list*/

    while (aux_obj != 0) {

	aux_obj->mt.ambient[0] = 0.135;
	aux_obj->mt.ambient[1] = 0.2225;
	aux_obj->mt.ambient[2] = 0.1575;
	aux_obj->mt.ambient[3] = 1;

	aux_obj->mt.diffuse[0] = 0.54;
	aux_obj->mt.diffuse[1] = 0.89;
	aux_obj->mt.diffuse[2] = 0.63;
	aux_obj->mt.diffuse[3] = 1;

	aux_obj->mt.specular[0] = 0.316228;
	aux_obj->mt.specular[1] = 0.316228;
	aux_obj->mt.specular[2] = 0.316228;
	aux_obj->mt.specular[3] = 1;

	aux_obj->mt.shininess = 0.2;


	glMaterialfv(GL_FRONT, GL_AMBIENT, aux_obj->mt.ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, aux_obj->mt.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, aux_obj->mt.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, aux_obj->mt.shininess);

        /* Select the color, depending on whether the current object is the selected one or not */
        if (aux_obj == _selected_object) {
            glColor3f(KG_COL_SELECTED_R, KG_COL_SELECTED_G, KG_COL_SELECTED_B);
        } else {
            glColor3f(KG_COL_NONSELECTED_R, KG_COL_NONSELECTED_G, KG_COL_NONSELECTED_B);
        }
        /*Carga la matriz del objeto seleccionado, en el estado acutal en el modelview*/
        cargaFocoObjeto();
        glLoadMatrixd(_selected_camera->matrizCamara);
        glMultMatrixd(aux_obj->pMptr->M);

        /* Draw the object; for each face create a new polygon with the corresponding vertices */
        for (f = 0; f < aux_obj->num_faces; f++) {
            glBegin(GL_POLYGON);
            if(shading==FLAT){
                glNormal3d(aux_obj->face_table[f].normal.x,aux_obj->face_table[f].normal.y,aux_obj->face_table[f].normal.z);
            }
            for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {

                v_index = aux_obj->face_table[f].vertex_table[v];

                if(shading==SMOOTH){
                    glNormal3d(aux_obj->vertex_table[v_index].normal.x,
                               aux_obj->vertex_table[v_index].normal.y,
                               aux_obj->vertex_table[v_index].normal.z);

                }
                glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                           aux_obj->vertex_table[v_index].coord.y,
                           aux_obj->vertex_table[v_index].coord.z);



            }
            glEnd();
        }
        aux_obj = aux_obj->siguiente;
    }
    /*Do the actual drawing*/
    glFlush();
}
