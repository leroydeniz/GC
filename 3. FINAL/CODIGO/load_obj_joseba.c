#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "definitions.h"
#include <math.h>
#define MAXLINE 200

/*
 * Auxiliar function to process each line of the file
 */
static int sreadint(char * lerroa, int * zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
        zenbakiak[kont++] = zbk;
    }
    return (kont);
}

static int sreadint2(char * lerroa, int * zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
	while ((*s != ' ')&&(*s !='\0')) s++;  // jump vector normal information
        zenbakiak[kont++] = zbk;
    }
printf("%d numbers in the line\n",kont);
    return (kont);
}
/**
 * @brief Function to read wavefront files (*.obj)
 * @param file_name Path of the file to be read
 * @param object_ptr Pointer of the object3d type structure where the data will be stored
 * @return Result of the reading: 0=Read ok, 1=File not found, 2=Invalid file, 3=Empty file
 */
int read_wavefront(char * file_name, object3d * object_ptr) {
    vertex *vertex_table;
    face *face_table;
    int num_vertices = -1, num_faces = -1, count_vertices = 0, count_faces = 0;
    FILE *obj_file;
    char line[MAXLINE], line_1[MAXLINE], aux[45];
    int k;
    int i, j;
    int values[MAXLINE];

    point3 p1,p2,p3;
    vector3 v1,v2;

    face *f;
    vertex *v;

    float d;
    /*
     * The function reads twice the file. In the first read the number of
     * vertices and faces is obtained. Then, memory is allocated for each
     * of them and in the second read the actual information is read and
     * loaded. Finally, the object structure is created
     */
    if ((obj_file = fopen(file_name, "r")) == NULL) return (1);
    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        i = 0;
        while (line[i] == ' ') i++;
        if ((line[0] == '#') && ((int) strlen(line) > 5)) {
            i += 2;
            j = 0;
            while (line[i] != ' ') line_1[j++] = line[i++];
            i++;
            line_1[j] = '\0';
            j = 0;
            while ((line[i] != ' ') && (line[i] != '\0'))
                aux[j++] = line[i++];
            aux[j] = 0;
            if (strcmp(aux, "vertices") == 0) {
                num_vertices = atoi(line_1);
            }
            if (strncmp(aux, "elements", 7) == 0) {
                num_faces = atoi(line_1);
            }
        } else {
            if (strlen(line) > 6) {
                if (line[i] == 'f' && line[i + 1] == ' ')
                    count_faces++;
                else if (line[i] == 'v' && line[i + 1] == ' ')
                    count_vertices++;
            }
        }
    }
    fclose(obj_file);
printf("1 pasada: num vert = %d (%d), num faces = %d(%d) \n",num_vertices,count_vertices,num_faces,count_faces);
    if ((num_vertices != -1 && num_vertices != count_vertices) || (num_faces != -1 && num_faces != count_faces)) {
        printf("WARNING: full file format: (%s)\n", file_name);
        //return (2);
    }
    if (num_vertices == 0 || count_vertices == 0) {
        printf("No vertex found: (%s)\n", file_name);
        return (3);
    }
    if (num_faces == 0 || count_faces == 0) {
        printf("No faces found: (%s)\n", file_name);
        return (3);
    }

    num_vertices = count_vertices;
    num_faces = count_faces;

    vertex_table = (vertex *) malloc(num_vertices * sizeof (vertex));
    face_table = (face *) malloc(num_faces * sizeof (face));

    obj_file = fopen(file_name, "r");
    k = 0;
    j = 0;

    for (i = 0; i < num_vertices; i++)
        vertex_table[i].num_faces = 0;

    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        switch (line[0]) {
            case 'v':
            if (line[1] == ' ')  // vn not interested
		{
                sscanf(line + 2, "%lf%lf%lf", &(vertex_table[k].coord.x),
                        &(vertex_table[k].coord.y), &(vertex_table[k].coord.z));
                k++;
		}
               break;

            case 'f':
	    if (line[1] == ' ') // fn not interested
                {
                for (i = 2; i <= (int) strlen(line); i++)
                    line_1[i - 2] = line[i];
		line_1[i-2] = '\0';
                face_table[j].num_vertices = sreadint2(line_1, values);
                face_table[j].vertex_table = (int *) malloc(face_table[j].num_vertices * sizeof (int));
                for (i = 0; i < face_table[j].num_vertices; i++) {
                    face_table[j].vertex_table[i] = values[i] - 1;
                    vertex_table[face_table[j].vertex_table[i]].num_faces++;
                    }
                j++;
                }
              break;
        }
    }

    fclose(obj_file);

printf("2 pasada\n");

    /*
     * Information read is introduced in the structure */
    object_ptr->vertex_table = vertex_table;
    object_ptr->face_table = face_table;
    object_ptr->num_vertices = num_vertices;
    object_ptr->num_faces = num_faces;


    /*
     * The maximum and minimum coordinates are obtained **/
    object_ptr->max.x = object_ptr->vertex_table[0].coord.x;
    object_ptr->max.y = object_ptr->vertex_table[0].coord.y;
    object_ptr->max.z = object_ptr->vertex_table[0].coord.z;
    object_ptr->min.x = object_ptr->vertex_table[0].coord.x;
    object_ptr->min.y = object_ptr->vertex_table[0].coord.y;
    object_ptr->min.z = object_ptr->vertex_table[0].coord.z;

    for (i = 1; i < object_ptr->num_vertices; i++)
    {
        if (object_ptr->vertex_table[i].coord.x < object_ptr->min.x)
            object_ptr->min.x = object_ptr->vertex_table[i].coord.x;

        if (object_ptr->vertex_table[i].coord.y < object_ptr->min.y)
            object_ptr->min.y = object_ptr->vertex_table[i].coord.y;

        if (object_ptr->vertex_table[i].coord.z < object_ptr->min.z)
            object_ptr->min.z = object_ptr->vertex_table[i].coord.z;

        if (object_ptr->vertex_table[i].coord.x > object_ptr->max.x)
            object_ptr->max.x = object_ptr->vertex_table[i].coord.x;

        if (object_ptr->vertex_table[i].coord.y > object_ptr->max.y)
            object_ptr->max.y = object_ptr->vertex_table[i].coord.y;

        if (object_ptr->vertex_table[i].coord.z > object_ptr->max.z)
            object_ptr->max.z = object_ptr->vertex_table[i].coord.z;

    }

    for(i=0;i<object_ptr->num_vertices;i++){
        v=&object_ptr->vertex_table[i];
        v->normal.x=0;
        v->normal.y=0;
        v->normal.z=0;
    }
    for(i=0;i<object_ptr->num_faces;i++){
        f=&object_ptr->face_table[i];

        p1=object_ptr->vertex_table[f->vertex_table[0]].coord;
        p2=object_ptr->vertex_table[f->vertex_table[1]].coord;
        p3=object_ptr->vertex_table[f->vertex_table[2]].coord;

        v1.x=(p2.x-p1.x);
        v1.y=(p2.y-p1.y);
        v1.z=(p2.z-p1.z);


        v2.x=p3.x-p1.x;
        v2.y=p3.y-p1.y;
        v2.z=p3.z-p1.z;

        f->normal.x=(v1.y*v2.z-v1.z*v2.y);
        f->normal.y=-(v1.x*v2.z-v1.z*v2.x);
        f->normal.z=(v1.x*v2.y-v1.y*v2.x);

        d=sqrt(pow(f->normal.x,2)+pow(f->normal.y,2)+pow(f->normal.z,2));

        f->normal.x/=d;
        f->normal.y/=d;
        f->normal.z/=d;
        for (j=0;j<f->num_vertices;j++){
            v=&object_ptr->vertex_table[f->vertex_table[j]];
            v->normal.x+=f->normal.x;
            v->normal.y+=f->normal.y;
            v->normal.z+=f->normal.z;
        }


    }
    for(i=0;i<object_ptr->num_vertices;i++){
        v=&object_ptr->vertex_table[i];
        d=sqrt(pow(v->normal.x,2)+pow(v->normal.y,2)+pow(v->normal.z,2));
        v->normal.x/=d;
        v->normal.y/=d;
        v->normal.z/=d;
    }

    return (0);
}
