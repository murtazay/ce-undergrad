/*
 *
 *
 *
 */

#include <GL/glut.h>

/* function prototypes */
void init		(void);
void display	(void);
void reshape	(int,int);

/* User defined abstract datatype */
typedef float	vector2f[2];

/* Global variables */
int 		WINW,WINH;
vector2f	out[] = {{0.1,0.1},{0.3,0.1},{0.25,0.15},{0.25,0.45},{0.75,0.45},{0.8,0.55},{0.85,0.65},{0.8,0.75},{0.75,0.85},{0.15,0.85},{0.15,0.15}};
vector2f	in[] = {{0.25,0.55},{0.7,0.55},{0.75,0.65},{0.7,0.75},{0.25,0.75}};

/*
 *	Main
 *
 */
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

/*
 * Init
 *
 */
void init(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(1.0, 1.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

/*
 * Display
 *
 */
void display(void){

	int 	i, w, h;
	w = WINW/3;
	h = WINH/3;

	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,w,h);
	glBegin(GL_POINTS);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glViewport(w,0,w,h);
	glBegin(GL_LINES);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glViewport(2*w,0,w,h);
	glBegin(GL_LINE_STRIP);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,1.0,1.0,0.0,-1.0,1.0);
	glViewport(0,h,w,h);
	glBegin(GL_LINE_LOOP);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
	glEnd();
	glBegin(GL_LINE_LOOP);
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glLoadIdentity();
	glOrtho(0.0,1.0,0.0,1.0,-1.0,1.0);
	glViewport(w,h,w,h);
	glBegin(GL_TRIANGLES);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glViewport(2*w,h,w,h);
	glBegin(GL_TRIANGLE_STRIP);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glViewport(0,2*h,w,h);
	glBegin(GL_TRIANGLE_FAN);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glViewport(w,2*h,w,h);
	glBegin(GL_QUADS);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glViewport(2*w,2*h,w,h);
	glBegin(GL_POLYGON);
		for(i=0;i<11;i++){
			glVertex2fv(out[i]);
		}
		for(i=0;i<5;i++){
			glVertex2fv(in[i]);
		}
	glEnd();

	glFlush();
	glutSwapBuffers();
}

/*
 * Reshape
 *
 */
void reshape(int w, int h){
	WINW = w;
	WINH = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}