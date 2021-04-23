#include "definitions.h"
#include "load_obj.h"
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

extern object3d *_first_object;
extern object3d *_selected_object;

extern GLdouble _ortho_x_min, _ortho_x_max;
extern GLdouble _ortho_y_min, _ortho_y_max;
extern GLdouble _ortho_z_min, _ortho_z_max;

#define TRASLACION 0
#define ROTACION 1
#define ESCALADO 2

#define GLOBAL 0
#define LOCAL 1

#define OBJETO 0
#define CAMARA 1
#define LUZ 2

int tipo_transformacion = TRASLACION; //Valor por defecto de la transformación
int elemento = OBJETO;
int SR = GLOBAL; //Control del sistema de referencia

/* NOTAS

 M_nueva * M_vieja * obj = transformacion respecto a su propio sistema de referencia 
M_vieja * M_nueva * obj = transformacion respecto al sistema de referencia global 

Las funciones que interesan son: glScale, glTranslate, glRotate. No escalan, ni trasladan ni rotan, solamente calculan matrices y multiplican matrices.
Toman la matriz de transformaci�n del objeto y la multiplican por la derecha a la matriz activa. 
Dejan en la matriz activa el resultado de la multiplicaci�n.


glScale / glRotate / glTranslate
neptr = malloc ()
glGetFloatv (GLMODELVIEW_MATRIZ, nptr->M)

*/

void print_help()
{
	printf("Practica de GC. Este programa tiene objetos 3D. \n");
	printf("Transformaciones.  \n\n");
	printf("\n\n");
	printf("FUNCIONES PRINCIPALES \n");
	printf("<?>\t\t Mostrar ayuda \n");
	printf("<ESC>\t\t Salir del programa \n");
	printf("<F>\t\t Cargar un objeto\n");
	printf("<TAB>\t\t Seleccionar una de los objetos cargados\n");
	printf("<DEL>\t\t Eliminar el objeto seleccionado\n");
	printf("<CTRL + ->\t Aumentar el area de visualizacion\n");
	printf("<CTRL + +>\t Disminuir el area de visualizacion\n");
	printf("\n\n");
}

void borrarNodoDeMemoria(elem_Matriz *nodoptr)
{
	if (nodoptr)
	{
		if (nodoptr->aptr)
			borrarNodoDeMemoria(nodoptr->aptr);
		free(nodoptr);
	}
}

void anadir_nodo(elem_Matriz *nodoptr)
{
	nodoptr->siguiente = _selected_object->pMptr;
	nodoptr->aptr = 0;
	borrarNodoDeMemoria(_selected_object->pMptr->aptr);
	_selected_object->pMptr->aptr = nodoptr;
	_selected_object->pMptr = nodoptr;
}

int rehacer()
{
	if (_selected_object && _selected_object->pMptr->aptr)
	{
		_selected_object->pMptr = _selected_object->pMptr->aptr;
		return 1;
	}
	else
	{
		return 0;
	}
}

int deshacer()
{
	if (_selected_object && _selected_object->pMptr->siguiente)
	{
		_selected_object->pMptr = _selected_object->pMptr->siguiente;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y)
{

	char *fname = malloc(sizeof(char) * 128);
	int read = 0;
	object3d *auxiliar_object = 0;
	GLdouble wd, he, midx, midy;
	elem_Matriz *aux_matrixptr = (elem_Matriz *)malloc(sizeof(elem_Matriz));

	switch (key)
	{
	case 'f':
	case 'F':
		/*Ask for file*/
		printf("%s", KG_MSSG_SELECT_FILE);
		scanf("%s", fname);
		/*Allocate memory for the structure and read the file*/
		auxiliar_object = (object3d *)malloc(sizeof(object3d));
		read = read_wavefront(fname, auxiliar_object);
		switch (read)
		{
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
			/*Read OK*/

		case 0: /* AGREGO LOS OBJETOS A LA LISTA DE OBJETOS */
			//reservo el espacio de memoria para inicializar la matriz de transformaciones.
			auxiliar_object->pMptr = (elem_Matriz *)malloc(sizeof(elem_Matriz));
			auxiliar_object->pMptr->siguiente = 0;
			auxiliar_object->pMptr->aptr = 0;
			int i;
			//Cargo la matríz identidad a 0s
			for (i = 0; i < 16; i++)
			{
				auxiliar_object->pMptr->M[i] = 0;
			}
			//Cargo la diagonal principal a 1s
			auxiliar_object->pMptr->M[0] = 1;
			auxiliar_object->pMptr->M[5] = 1;
			auxiliar_object->pMptr->M[10] = 1;
			auxiliar_object->pMptr->M[15] = 1;

			auxiliar_object->next = _first_object;
			_first_object = auxiliar_object;
			_selected_object = _first_object;
			printf("%s\n", KG_MSSG_FILEREAD);
			break;
		}
		break;

	case 9: /* <TAB> */
		_selected_object = _selected_object->next;
		/*The selection is circular, thus if we move out of the list we go back to the first element*/
		if (_selected_object == 0)
			_selected_object = _first_object;
		break;

	case 127: /* <SUPR> */
		if (_first_object != 0)
		{
			/*Erasing an object depends on whether it is the first one or not*/
			if (_selected_object == _first_object)
			{
				/*To remove the first object we just set the first as the current's next*/
				_first_object = _first_object->next;
				/*Once updated the pointer to the first object it is save to free the memory*/
				free(_selected_object);
				/*Finally, set the selected to the new first one*/
				_selected_object = _first_object;
			}
			else
			{
				/*In this case we need to get the previous element to the one we want to erase*/
				auxiliar_object = _first_object;
				while (auxiliar_object->next != _selected_object)
					auxiliar_object = auxiliar_object->next;
				/*Now we bypass the element to erase*/
				auxiliar_object->next = _selected_object->next;
				/*free the memory*/
				free(_selected_object);
				/*and update the selection*/
				_selected_object = auxiliar_object;
			}
			break;
		}
		break;

	case '-':
		//FUNCION DE DISMINUIR CAMPO DE VISION
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
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
		}
		else if ((elemento == OBJETO))
		{
			if (_selected_object)
			{
				/* ESCALAR EN TODOS LOS EJES */
				printf("Escalar - en todos los ejes\n");
				glMatrixMode(GL_MODELVIEW);
				if (SR == LOCAL)
					glLoadMatrixf(_selected_object->pMptr->M);
				else
					glLoadIdentity();
				glScalef(0.9f, 0.9f, 0.9f);
				if (SR == GLOBAL)
					glMultMatrixf(_selected_object->pMptr->M);
				glGetFloatv(GL_MODELVIEW_MATRIX, aux_matrixptr->M);
				anadir_nodo(aux_matrixptr);
			}
			else
			{
				printf("No se seleccion� ning�n objeto.\n");
			}
		}
		else
		{
			printf("Esto no es un objeto.\n");
		}
		break;

	case '+':
		//FUNCION DE AUMENTAR CAMPO DE VISION
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			/*Increase the projection plane; compute the new dimensions*/
			wd = (_ortho_x_max - _ortho_x_min) * KG_STEP_ZOOM;
			he = (_ortho_y_max - _ortho_y_min) * KG_STEP_ZOOM;
			/*In order to avoid moving the center of the plane, we get its coordinates*/
			midx = (_ortho_x_max + _ortho_x_min) / 2;
			midy = (_ortho_y_max + _ortho_y_min) / 2;
			/*The the new limits are set, keeping the center of the plane*/
			_ortho_x_max = midx + wd / 2;
			_ortho_x_min = midx - wd / 2;
			_ortho_y_max = midy + he / 2;
			_ortho_y_min = midy - he / 2;
		}
		else if ((elemento == OBJETO))
		{
			if (_selected_object)
			{
				printf("Escalar + en todos los ejes\n");
				glMatrixMode(GL_MODELVIEW);
				if (SR == LOCAL)
					glLoadMatrixf(_selected_object->pMptr->M);
				else
					glLoadIdentity();
				glScalef(1.1f, 1.1f, 1.1f);
				if (SR == GLOBAL)
					glMultMatrixf(_selected_object->pMptr->M);
				glGetFloatv(GL_MODELVIEW_MATRIX, aux_matrixptr->M);
				anadir_nodo(aux_matrixptr);
			}
			else
			{
				printf("No hay ningun objeto seleccionado\n");
			}
		}
		else
		{
			printf("El elemento actual no es un objeto\n");
		}
		break;

	case '?':
		print_help();
		break;

	case 27: /* <ESC> */
		exit(0);
		break;

	case 'M':
	case 'm':
		tipo_transformacion = TRASLACION;
		printf("** MODO DE TRASLACION - ACTIVADO**\n");
		break;

	case 'B':
	case 'b':
		tipo_transformacion = ROTACION;
		printf("** MODO DE ROTACION - ACTIVADO **\n");
		break;

	case 'T':
	case 't':
		tipo_transformacion = ESCALADO;
		printf("** MODO DE ESCALADO - ACTIVADO **\n");
		break;

	case 'G':
	case 'g':
		SR = GLOBAL;
		printf("** SISTEMA DE REFERENCIA GLOBAL - ACTIVADO **\n");
		break;

	case 'L':
	case 'l':
		SR = LOCAL;
		printf("** SISTEMA DE REFERENCIA LOCAL - ACTIVADO **\n");
		break;

	case 'O':
	case 'o':
		elemento = OBJETO;
		printf("Aplicar transformaciones al objeto seleccionado\n");
		break;

	case 'K':
	case 'k':
		elemento = CAMARA;
		printf("Aplicar transformaciones a la camara actual\n");
		break;

	case 'A':
	case 'a':
		elemento = LUZ;
		printf("Aplicar transformaciones a la luz actual\n");
		break;

	case 25: /* <CTRL + Y> */
		if (rehacer())
			printf("Rehacer\n");
		else
			printf("No hay acciones para rehacer\n");
		break;

	case 26: /* <CTRL + Z> */
		if (deshacer())
			printf("Deshacer\n");
		else
			printf("No hay acciones para deshacer\n");
		break;

	default:
		/*In the default case we just print the code of the key. This is usefull to define new cases*/
		printf("%d %c\n", key, key);
	}
	/*In case we have do any modification affecting the displaying of the object, we redraw them*/
	glutPostRedisplay();
}

/**
 * @brief Callback function to control the special keys of the keyboard
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void special_keys(int key, int x, int y)
{
	elem_Matriz *aux_matrixptr = (elem_Matriz *)malloc(sizeof(elem_Matriz));

	if (_selected_object == 0)
	{
		printf("OBJETO SIN SELECCIONAR\n");
	}
	else if ((key < 100) || (key > 105))
	{
		printf("TECLA SIN DEFINIR: %d %c\n", key, key);
	}
	else
	{
		if (elemento == OBJETO)
		{
			glMatrixMode(GL_MODELVIEW); //se elige la matriz GL_MODELVIEW
			if (SR == LOCAL)
				glLoadMatrixf(_selected_object->pMptr->M); //si el sistema de referencia es local, se carga la matriz de transformaciones
			else
				glLoadIdentity(); //CC se carga la matriz identidad

			switch (key)
			{
			
			case GLUT_KEY_DOWN:
				if (tipo_transformacion == TRASLACION)
				{
					printf("Transladar -Y\n");
					glTranslatef(0.0f, -0.1f, 0.0f);
				}
				else if (tipo_transformacion == ESCALADO)
				{
					printf("Escalar -Y\n");
					glScalef(1.0f, 0.9f, 1.0f);
				}
				else
				{
					printf("Rotar -X\n");
					glRotatef(10.0f, -1.0f, 0.0f, 0.0f);
				}
				break;

			case GLUT_KEY_UP:
				if (tipo_transformacion == TRASLACION)
				{
					printf("Transladar +Y\n");
					glTranslatef(0.0f, 0.1f, 0.0f);
				}
				else if (tipo_transformacion == ESCALADO)
				{
					printf("Escalar +Y\n");
					glScalef(1.0f, 1.1f, 1.0f);
				}
				else
				{
					printf("Rotar +X\n");
					glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
				}
				break;

			case GLUT_KEY_RIGHT:
				if (tipo_transformacion == TRASLACION)
				{
					printf("Transladar +X\n");
					glTranslatef(0.1f, 0.0f, 0.0f);
				}
				else if (tipo_transformacion == ESCALADO)
				{
					printf("Escalar +X\n");
					glScalef(1.1f, 1.0f, 1.0f);
				}
				else
				{
					printf("Rotar +Y\n");
					glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
				}
				break;

			case GLUT_KEY_PAGE_DOWN:
				if (tipo_transformacion == TRASLACION)
				{
					printf("Transladar -Z\n");
					glTranslatef(0.0f, 0.0f, -0.1f);
				}
				else if (tipo_transformacion == ESCALADO)
				{
					printf("Escalar -Z\n");
					glScalef(1.0f, 1.0f, 0.9f);
				}
				else
				{
					printf("Rotar -Z\n");
					glRotatef(10.0f, 0.0f, 0.0f, -1.0f);
				}
				break;

			case GLUT_KEY_PAGE_UP:
				if (tipo_transformacion == TRASLACION)
				{
					printf("Transladar +Z\n");
					glTranslatef(0.0f, 0.0f, 1.0f);
				}
				else if (tipo_transformacion == ESCALADO)
				{
					printf("Escalar +Z\n");
					glScalef(1.0f, 1.0f, 1.1f);
				}
				else
				{
					printf("Rotar +Z\n");
					glRotatef(10.0f, 0.0f, 0.0f, 0.1f);
				}
				break;

			case GLUT_KEY_LEFT:
				if (tipo_transformacion == TRASLACION)
				{
					printf("Transladar -X\n");
					glTranslatef(-0.1f, 0.0f, 0.0f);
				}
				else if (tipo_transformacion == ESCALADO)
				{
					printf("Escalar -X\n");
					glScalef(0.9f, 1.0f, 1.0f);
				}
				else
				{
					printf("Rotar -Y\n");
					glRotatef(10.0f, 0.0f, -1.0f, 0.0f);
				}
				break;

			} //cierra el switch de evaluaci�n de 'key'

			if (SR == GLOBAL)
			{
				glMultMatrixf(_selected_object->pMptr->M);
			}

			glGetFloatv(GL_MODELVIEW_MATRIX, aux_matrixptr->M);
			anadir_nodo(aux_matrixptr);
			glutPostRedisplay();
		}
		else if (elemento == CAMARA)
		{
			// TRANSFORMACIONES DE CAMARA
		}
		else
		{
			// TRANSFORMACIONES DE LUZ
		}
	}
}
