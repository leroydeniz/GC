#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "display.h"
#include "io.h"
#include "definitions.h"
#include "camara.h"

/** GLOBAL VARIABLES **/

GLdouble _window_ratio;                     /*Control of window's proportions */
GLdouble _ortho_x_min,_ortho_x_max;         /*Variables for the control of the orthographic projection*/
GLdouble _ortho_y_min ,_ortho_y_max;        /*Variables for the control of the orthographic projection*/
GLdouble _ortho_z_min,_ortho_z_max;         /*Variables for the control of the orthographic projection*/

GLdouble _frust_x_min,_frust_x_max;         /*Variables for the control of the orthographic projection*/
GLdouble _frust_y_min ,_frust_y_max;        /*Variables for the control of the orthographic projection*/
GLdouble _frust_z_min,_frust_z_max;         /*Variables for the control of the orthographic projection*/

object3d * _first_object= 0;                /*List of objects*/
object3d * _selected_object = 0;            /*Object currently selected*/
camara * _selected_camera=0;
luz *_selected_light=0;
luz luces[8];
int luces_activadas=0;

/*** DEFINICIÓN DE LUCES DE OPENGL ***/
GLint GL_LIGHT[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};


void cargaValores(int j,GLfloat *posicion, GLfloat ambient[4],GLfloat diffuse[4],GLfloat specular[4]){
    int i;
    for(i=0;i<4;i++){
        luces[j].pos[i]=posicion[i];
        luces[j].ambient[i]=ambient[i];
        luces[j].diffuse[i]=diffuse[i];
        luces[j].specular[i]=specular[i];
    }
}

void cargaDireccionFoco(int j,GLfloat *direccion){
    int i;
    for(i=0;i<3;i++)
        luces[j].dir[i]=direccion[i];
}

GLfloat *creaPosicion(GLfloat x, GLfloat y, GLfloat z, GLfloat t){
    GLfloat *posicion;
    posicion=(GLfloat*)malloc(4* sizeof(GLfloat));
    posicion[0]=x;
    posicion[1]=y;
    posicion[2]=z;
    posicion[3]=t;

    return posicion;
}

GLfloat *creaDireccion(GLfloat x, GLfloat y, GLfloat z){
    GLfloat *direccion;
    direccion=(GLfloat*)malloc(3* sizeof(GLfloat));
    direccion[0]=x;
    direccion[1]=y;
    direccion[2]=z;
    return direccion;
}


/*******************************/
/*** INICIALIZACIÓN DE LUCES ***/
/*******************************/

void inicializarLuces(){
    GLfloat white[]   = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat blue[]    = {0.0f, 0.0f, 1.0f, 1.0f};
    GLfloat green[]   = {0.0f, 1.0f, 0.0f, 1.0f};
    GLfloat cyan[]    = {0.0f, 1.0f, 1.0f, 1.0f};
    GLfloat orange[]  = {1.0f, 0.5f, 0.0f, 1.0f};
    GLfloat red[]     = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat magenta[] = {1.0f, 0.0f, 1.0f, 1.0f};
    GLfloat gray[]    = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat black[]   = {0.0f, 0.0f, 0.0f, 0.0f};

    GLfloat blueDark[]    = {0.0f, 0.0f, 0.5f, 1.0f};
    GLfloat greenDark[]   = {0.0f, 0.5f, 0.0f, 1.0f};
    GLfloat cyanDark[]    = {0.0f, 0.5f, 0.5f, 1.0f};
    GLfloat yellowDark[]  = {0.5f, 0.5f, 0.0f, 1.0f};
    GLfloat orangeDark[]  = {0.5f, 0.25f, 0.0f, 1.0f};
    GLfloat redDark[]     = {0.5f, 0.0f, 0.0f, 1.0f};
    GLfloat magentaDark[] = {0.5f, 0.0f, 0.5f, 1.0f};
    GLfloat grayDark[] = {0.15f, 0.15f, 0.15f, 0.1f};

    int i;
    GLfloat *posicion;
    GLfloat *direccion;
    posicion=(GLfloat*)malloc(4*sizeof(GLfloat));
    direccion=(GLfloat*)malloc(3* sizeof(GLfloat));

    luces[2].tipo=FOCO;
    luces[3].tipo=BOMBILLA;
    luces[4].tipo=FOCO;
    luces[5].tipo=BOMBILLA;
    luces[6].tipo=FOCO;
    luces[7].tipo=BOMBILLA;

    /*** LUZ 0 - SOL ***/
    luces[0].tipo=SOL;
    luces[0].pos[0]=0.0;
    luces[0].pos[1]=5.0;
    luces[0].pos[2]=0.0;
    luces[0].pos[3]=0.0;
    glLightfv(GL_LIGHT0 , GL_AMBIENT , white);
    luces[0].estado=ENCENDIDA;

    /*** LUZ 1 - BOMBILLA ***/
    luces[1].tipo=BOMBILLA;
    posicion=creaPosicion(0.0,0.0,10.0,0.0);
    cargaValores(1,posicion,blue,blueDark,white);
    luces[1].estado=APAGADA;
    luces[1].ampFoco=180;

    /*** LUZ 2 - FOCO ***/
    luces[2].tipo=FOCO;
    posicion=creaPosicion(5.0,5.0,0.0,1.0);
    cargaValores(2,posicion,white,yellowDark,white);
    luces[2].estado=APAGADA;
    luces[2].ampFoco=45;
    direccion=creaDireccion(0.0,-1.0,0.0);
    cargaDireccionFoco(2,direccion);


    /*** LUZ 3 ***/
    luces[3].tipo=BOMBILLA;
    posicion=creaPosicion(-3.0,3.0,0.0,0.0);
    cargaValores(3,posicion,green,greenDark,white);
    luces[3].estado=APAGADA;
    luces[3].ampFoco=180;

    /*** LUZ 4 ***/
    luces[4].tipo=FOCO;
    posicion=creaPosicion(0.0,0.0,0.0,1.0);
    cargaValores(4,posicion,blue,blueDark,white);
    luces[4].estado=APAGADA;
    luces[4].ampFoco=45;
    direccion=creaDireccion(1.0,0.0,0.0);
    cargaDireccionFoco(4,direccion);

    /*** LUZ 5 ***/
    luces[5].tipo=BOMBILLA;
    posicion=creaPosicion(-3.0,-3.0,0.0,0.0);
    cargaValores(5,posicion,cyan,cyanDark,white);
    luces[5].estado=APAGADA;
    luces[5].ampFoco=180;

    /*** LUZ 6 ***/
    luces[6].tipo=FOCO;
    posicion=creaPosicion(3.0,3.0,3.0,1.0);
    cargaValores(6,posicion,red,orange,green);
    luces[6].estado=APAGADA;
    luces[6].ampFoco=45;
    direccion=creaDireccion(0.0,0.0,0.0);
    cargaDireccionFoco(6,direccion);

    /*** LUZ 7 ***/
    luces[7].tipo=BOMBILLA;
    posicion=creaPosicion(0.0,0.0,10.0,0.0);
    cargaValores(7,posicion,blue,orange,white);
    luces[7].estado=APAGADA;
    luces[7].ampFoco=180;

    for(i=1;i<8;i++){
        glLightfv(GL_LIGHT[i] , GL_AMBIENT , luces[i].ambient);
        glLightfv(GL_LIGHT[i] , GL_DIFFUSE , luces[i].diffuse );
        glLightfv(GL_LIGHT[i] , GL_SPECULAR , luces[i].specular );
        glLightf(GL_LIGHT[i],GL_CONSTANT_ATTENUATION, 1.0);
        glLightf(GL_LIGHT[i], GL_LINEAR_ATTENUATION, 0.0);
        glLightf(GL_LIGHT[i],GL_QUADRATIC_ATTENUATION, 0.0);
        glLightf(GL_LIGHT[i],GL_SPOT_CUTOFF,luces[i].ampFoco);
        glLightf(GL_LIGHT2,GL_SPOT_EXPONENT, 1.0);
    }

    /*** INICIALIZACIÓN DEL SOL ***/
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);


}

/*** GENERAL INITIALIZATION **/
void initialization (){

    /*Initialization of all the variables with the default values*/
    _ortho_x_min = KG_ORTHO_X_MIN_INIT;
    _ortho_x_max = KG_ORTHO_X_MAX_INIT;
    _ortho_y_min = KG_ORTHO_Y_MIN_INIT;
    _ortho_y_max = KG_ORTHO_Y_MAX_INIT;
    _ortho_z_min = KG_ORTHO_Z_MIN_INIT;
    _ortho_z_max = KG_ORTHO_Z_MAX_INIT;

    _window_ratio = (GLdouble) KG_WINDOW_WIDTH / (GLdouble) KG_WINDOW_HEIGHT;

    _frust_y_max = 0.04;
    _frust_y_min = -0.04;

    _frust_x_min = -0.06;
    _frust_x_max = 0.06;

    _frust_z_min = 0.1;
    _frust_z_max = 1000;


    glEnable(GL_DEPTH_TEST);
    /*Definition of the background color*/
    glClearColor(KG_COL_BACK_R, KG_COL_BACK_G, KG_COL_BACK_B, KG_COL_BACK_A);

    /*Definition of the method to draw the objects*/
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /*** INICIALIZACIÓN DE CÁMARA ***/
    point3 e ={0,0,10};
    point3 at ={0,0,0};
    vector3 up ={0,1,0};

    _selected_camera=inicializarCamara(e,at,up);
    inicializarLuces();


}


/** MAIN FUNCTION **/
int main(int argc, char** argv) {

    /*First of all, print the help information*/
    print_help();

    /* glut initializations */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(KG_WINDOW_WIDTH, KG_WINDOW_HEIGHT);
    glutInitWindowPosition(KG_WINDOW_X, KG_WINDOW_Y);
    glutCreateWindow(KG_WINDOW_TITLE);

    /* set the callback functions */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(k_arrow);

    /* this initialization has to be AFTER the creation of the window */
    initialization();

    /* start the main loop */
    glutMainLoop();
    return 0;
}
