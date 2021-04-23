//	Program developed by
//	
//	Informatika Fakultatea
//	Euskal Herriko Unibertsitatea
//	http://www.ehu.eus/if
//
// to compile it: gcc dibujar-puntos.c -lGL -lGLU -lglut
//
// This example shows :the use of GL_POINTS
//


#include <GL/glut.h>
#include <stdio.h>
#include "triangulo.h"


// texturaren informazioa

extern void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr, int * dimyptr);
unsigned char *bufferra;
int dimx,dimy, indice, num_triangles;
hiruki* triangulosptr;
indice = 0;


/* DEVUELVE UN PUNTERO AL PIXEL */
unsigned char * color_textura(float u, float v) {

	int x = (int)(dimx*u);
	int y = (int)(dimy-dimy*v);
	int incr = (int)(3*x)+(dimx*y*3);
	/* debe devolver un puntero al pixel adecuado*/
	return(bufferra+incr);

}


/* FUNCIÓN QUE DIBUJA CADA TRIÁNGULO */
void dibujar_triangulo(hiruki* triangulo_ptr) {
	punto *punto_superior_ptr,*punto_inferior_ptr,*punto_medio_ptr, corte_1,corte_2,corte_3;
	float i, pendiente_yx_SM, pendiente_yu_SM, pendiente_yv_SM, pendiente_yx_SI, pendiente_yu_SI, pendiente_yv_SI, pendiente_yx_MI, pendiente_yu_MI, pendiente_yv_MI;

	/* IDENTIFICA CUÁL ES EL PUNTO INFERIOR, MEDIO Y SUPERIOR, O SI POR EL CONTRARIO TIENE PUNTOS ALINEADOS */
	if(triangulo_ptr->p1.y > triangulo_ptr->p2.y) {
		punto_superior_ptr = &(triangulo_ptr->p1);
		punto_medio_ptr = &(triangulo_ptr->p2);
	} else {
		punto_superior_ptr = &(triangulo_ptr->p2);
		punto_medio_ptr = &(triangulo_ptr->p1);
	}
	if(triangulo_ptr->p3.y < punto_medio_ptr->y) {
		punto_inferior_ptr = &(triangulo_ptr->p3);
	} else {
		punto_inferior_ptr = punto_medio_ptr;
		if(punto_superior_ptr->y > triangulo_ptr->p3.y) {
			punto_medio_ptr =  &(triangulo_ptr->p3);
		} else {
			punto_medio_ptr = punto_superior_ptr;
			punto_superior_ptr = &(triangulo_ptr->p3);
		}
	}


	/* CALCULA LAS PENDIENTES DE CADA RECTA PARA DIBUJAR CADA SEGMENTO */
	pendiente_yx_SM = (punto_superior_ptr->x - punto_medio_ptr->x)/(punto_superior_ptr->y - punto_medio_ptr->y);
	pendiente_yu_SM = (punto_superior_ptr->u - punto_medio_ptr->u)/(punto_superior_ptr->y - punto_medio_ptr->y);
	pendiente_yv_SM = (punto_superior_ptr->v - punto_medio_ptr->v)/(punto_superior_ptr->y - punto_medio_ptr->y);
	pendiente_yx_SI = (punto_superior_ptr->x - punto_inferior_ptr->x)/(punto_superior_ptr->y - punto_inferior_ptr->y);
	pendiente_yu_SI = (punto_superior_ptr->u - punto_inferior_ptr->u)/(punto_superior_ptr->y - punto_inferior_ptr->y);
	pendiente_yv_SI = (punto_superior_ptr->v - punto_inferior_ptr->v)/(punto_superior_ptr->y - punto_inferior_ptr->y);
	pendiente_yx_MI = (punto_medio_ptr->x - punto_inferior_ptr->x)/(punto_medio_ptr->y - punto_inferior_ptr->y);
	pendiente_yu_MI = (punto_medio_ptr->u - punto_inferior_ptr->u)/(punto_medio_ptr->y - punto_inferior_ptr->y);
	pendiente_yv_MI = (punto_medio_ptr->v - punto_inferior_ptr->v)/(punto_medio_ptr->y - punto_inferior_ptr->y);


	/* CALCULA LOS PUNTOS DE CORTE ENTRE LAS RECTAS */
	corte_1.y = punto_superior_ptr-> y;
	corte_1.x = punto_superior_ptr-> x;
	corte_1.u = punto_superior_ptr-> u;
	corte_1.v = punto_superior_ptr-> v;


	corte_2.y = punto_superior_ptr-> y;
	corte_2.x = punto_superior_ptr-> x;
	corte_2.u = punto_superior_ptr-> u;
	corte_2.v = punto_superior_ptr-> v;

	corte_3.y = punto_medio_ptr-> y;
	corte_3.x = punto_medio_ptr-> x;
	corte_3.u = punto_medio_ptr-> u;
	corte_3.v = punto_medio_ptr-> v;


	/* CALCULA CUÁL ES EL PUNTO QUE ESTÁ MÁS ALTO Y DIBUJA EL SEGMENTO QUE CORRESPONDE ENTRE ÉL Y EL DEL MEDIO */
	for( i=punto_superior_ptr->y; i>punto_medio_ptr->y; i-- ) {
		corte_1.y = i;
		corte_1.x = corte_1.x - pendiente_yx_SM;
		corte_1.u = corte_1.u - pendiente_yu_SM;
		corte_1.v = corte_1.v - pendiente_yv_SM;

		corte_2.y = i;
		corte_2.x = corte_2.x - pendiente_yx_SI;
		corte_2.u = corte_2.u - pendiente_yu_SI;
		corte_2.v = corte_2.v - pendiente_yv_SI;

		/* DIBUJA LOS SEGMENTOS PASÁNDOLE SIEMPRE LA ENTRADA PRIMERO Y LUEGO LA SALIDA */
		if(corte_1.x<corte_2.x) {
			segmentos(&corte_1, &corte_2);
		} else {
			segmentos(&corte_2, &corte_1);
		}

	}

	/* DIBUJARÁ LOS SEGMENTOS CON SU TEXTURA, EN EL SUBTRIÁNGULO QUE EMPIEZA EN LA COORDENADA Y DEL PUNTO MEDIO */
	for(i=punto_medio_ptr->y; i>punto_inferior_ptr->y;i--) {

		corte_2.y = i;
		corte_2.x = corte_2.x - pendiente_yx_SI;
		corte_2.u = corte_2.u - pendiente_yu_SI;
		corte_2.v = corte_2.v - pendiente_yv_SI;

		corte_3.y = i;
		corte_3.x = corte_3.x - pendiente_yx_MI;
		corte_3.u = corte_3.u - pendiente_yu_MI;
		corte_3.v = corte_3.v - pendiente_yv_MI;

		/* LLAMA A LA FUNCIÓN SEGMENTO PARA QUE REALICE EL DIBUJO */
		if(corte_3.x < corte_2.x) {
			segmentos(&corte_3, &corte_2);
		} else {
			segmentos(&corte_2,&corte_3);
		}

	}

}

/* DIBUJA LAS ARISTAS DEL TRIÁNGULO, SEGMENTOS ENRTE LOS PUNTOS DE CORTE QUE SE LE PASAN POR PARÁMETRO*/
void segmentos(punto* primer_punto, punto* segundo_punto) {
	float i, pendiente_xu, pendiente_xv;
	unsigned char * color;
	unsigned char r, g, b;
	punto pixel;

	/* CALCULA LAS PENDIENTES DE CADA ARISTA */
	pendiente_xv=(primer_punto->v - segundo_punto->v)/(primer_punto->x - segundo_punto->x);
	pendiente_xu=(primer_punto->u - segundo_punto->u)/(primer_punto->x - segundo_punto->x);

	pixel.y = primer_punto-> y;
	pixel.x = primer_punto-> x;
	pixel.u = primer_punto-> u;
	pixel.v = primer_punto-> v;

	/* DIBUJA LOS SEGMENTOS CON EL COLOR DE LA TEXTURA QUE LE CORRESPONDE DESDE LA IMAGEN DE ORIGEN */
	for(i=primer_punto->x;i<segundo_punto->x;i++) {
		color = color_textura(pixel.u,pixel.v);
		pixel.u = pixel.u + pendiente_xu;
		pixel.v = pixel.v + pendiente_xv;
		r = color[0];
		g = color[1];
		b = color[2]; 
		glBegin( GL_POINTS );
		glColor3ub(r,g,b);
		glVertex3f(i,pixel.y,0.);
		glEnd();
	}

}

/* DIBUJA LOS TRIÁNGULOS */
static void marraztu(void) {

	// borramos lo que haya...
	glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 500.0, 0.0, 500.0,-250.0, 250.0);
	dibujar_triangulo(&triangulosptr[indice]);
	glFlush();

}


// This function will be called whenever the user pushes one key
static void teklatua (unsigned char key, int x, int y) {

	switch(key) {

		case 13: // <ENTER>
			printf ("ENTER: que hay que dibujar el siguiente triángulo.\n");
			/* hacer algo para que se dibuje el siguiente triangulo */
			if(indice == num_triangles-1) {
				indice = 0;  // pero si es el último? hay que controlarlo!
			} else {
				indice++;
			}
			break;

		case 27:  // <ESC>
			exit( 0 );
			break;

		default:
			printf("%d %c\n", key, key );
	}

	// The screen must be drawn to show the new triangle
	glutPostRedisplay();
}


/* INICIO DEL PROGRAMA */
int main(int argc, char** argv) {

	printf("This program draws points in the viewport \n");
	printf("Press <ESC> to finish\n");
	glutInit(&argc,argv);
	glutInitDisplayMode ( GLUT_RGB );
	glutInitWindowSize ( 500, 500 );
	glutInitWindowPosition ( 100, 100 );
	glutCreateWindow( "GL_POINTS" );
	glutDisplayFunc( marraztu );
	glutKeyboardFunc( teklatua );
	
	/* we put the information of the texture in the buffer pointed by bufferra. The dimensions of the texture are loaded into dimx and dimy */ 
	load_ppm("foto.ppm", &bufferra, &dimx, &dimy);
	/* lee la información de los triángulos*/
	cargar_triangulos(&num_triangles, &triangulosptr);
	glClearColor( 0.0f, 0.0f, 0.7f, 1.0f );
	glutMainLoop();
	return 0;   
}

