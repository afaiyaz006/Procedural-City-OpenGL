/*
13/10/2023
A.Faiyaz
ID:19201006
*/

#include <iostream>
#include <GL/glew.h>
#include <vector>
#include <time.h>       /* time */
#include <math.h>
#include "camera.h"
#include "BmpLoader.h"
using namespace std;
using namespace glm;
vector<vector<double>> pos;
vector<double> heights;
//Create the Camera
Camera camera;
//number of cubes
int number_of_cubes = 100;

// light related
bool isAmbient = true;
bool isDiffuse = true;
bool isSpecular = true;
bool isLight = true;
unsigned int ID = 0;

// beizier curve surface related
int wired = 0;
int shcpt = 1;
int animat = 0;
const int L = 8;
const int dgre = 3;
int ncpt = L + 1;
int clikd = 0;
const int nt = 40;				//number of slices along x-direction
const int ntheta = 20;
bool turnonTurbine = false;
int rotateTurbine = 0;

//eida  
GLfloat ctrlpoints[L + 1][3] =
{
	{ 0.0, 2.0, 0.0},
	{-0.3, 3.0, 0.0},
	{ 0.1, 4.0, 0.0},
	{0.5, 2.0, 0.0},
	{1.0, 1.5, 0.0},
	{1.4, 1.4, 0.0},
	{1.8, 0.4, 0.0},
	{2.0, 1.0, 0.0},
	{ 2.0, 0, 0.35}
};


//some function declarations
void light();
class Window {
public:
	Window() {
		this->interval = 1000 / 60;		//60 FPS
		this->window_handle = -1;
	}
	int window_handle, interval;
	ivec2 size;
	float window_aspect;
} window;

//Invalidate the window handle when window is closed
void CloseFunc() {
	window.window_handle = -1;
}
//Resize the window and properly update the camera viewport
void ReshapeFunc(int w, int h) {
	if (h > 0) {
		window.size = ivec2(w, h);
		window.window_aspect = float(w) / float(h);
	}
	camera.SetViewport(0, 0, window.size.x, window.size.y);
}
void generatePositions() {
	// calculate shits
	srand(time(NULL));
	pos.clear();
	for (int i = 0; i < number_of_cubes; i++) {
		double posX = rand() % 20;
		double posZ = rand() % 20;
		pos.push_back({ posX,posZ });
	}
	double customheights[] = { 0.3,0.2,0.5,1.0,2.0,3.0 };
	for (int i = 0; i < number_of_cubes; i++) {
		heights.push_back(customheights[rand() % 6]);
	}

}
void LoadTexture(const char* filename)
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	BmpLoader bl(filename);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData);
}

static void getNormal3p
(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
	GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

	Ux = x2 - x1;
	Uy = y2 - y1;
	Uz = z2 - z1;

	Vx = x3 - x1;
	Vy = y3 - y1;
	Vz = z3 - z1;

	Nx = Uy * Vz - Uz * Vy;
	Ny = Uz * Vx - Ux * Vz;
	Nz = Ux * Vy - Uy * Vx;

	glNormal3f(Nx, Ny, Nz);
}


//Keyboard input for camera, also handles exit case
void KeyboardFunc(unsigned char c, int x, int y) {
	switch (c) {
	case 'w':
		camera.Move(FORWARD);
		break;
	case 'a':
		camera.Move(LEFT);
		break;
	case 's':
		camera.Move(BACK);
		break;
	case 'd':
		camera.Move(RIGHT);
		break;
	case 'q':
		camera.Move(DOWN);
		break;
	case 'e':
		camera.Move(UP);
		break;
	case 'g':
		generatePositions();
		break;
	case 'l':
		if (isLight) {
			isLight = false;
		}
		else {
			isLight = true;
		}
		light();
		break;
	case 'y':
		if (isDiffuse) {
			isDiffuse = false;
		}
		else {
			isDiffuse = true;
		}
		light();
		break;
	case 'z':
		if (isSpecular) {
			isSpecular = false;
		}
		else {
			isSpecular = true;
		}
		light();
		break;
	case 'v':
		if (isAmbient) {
			isAmbient = false;
		}
		else {
			isAmbient = true;
		}
		light();
		break;
	case 't':
		if (turnonTurbine) {
			turnonTurbine = false;
		}
		else {
			turnonTurbine = true;
		}
		break;
	case 'r':
		camera.SetLookAt(glm::vec3(0, 0, 0));
		break;
	case 'x':
	case 27:
		exit(0);
		return;
	default:

		break;
	}
}

void SpecialFunc(int c, int x, int y) {}
void CallBackPassiveFunc(int x, int y) {}
//Used when person clicks mouse
void CallBackMouseFunc(int button, int state, int x, int y) {
	camera.SetPos(button, state, x, y);
}
//Used when person drags mouse around
void CallBackMotionFunc(int x, int y) {
	camera.Move2D(x, y);
}
static GLfloat v_cube[8][3] =
{
	{0.0, 0.0, 0.0}, //0
	{0.0, 0.0, 1.0}, //1
	{1.0, 0.0, 1.0}, //2
	{1.0, 0.0, 0.0}, //3
	{0.0, 1.0, 0.0}, //4
	{0.0, 1.0, 1.0}, //5
	{1.0, 1.0, 1.0}, //6
	{1.0, 1.0, 0.0}  //7
};

static GLubyte quadIndices[6][4] =
{
	{0, 1, 2, 3}, //bottom
	{4, 5, 6, 7}, //top
	{5, 1, 2, 6}, //front
	{0, 4, 7, 3}, // back is clockwise
	{2, 3, 7, 6}, //right
	{1, 5, 4, 0}  //left is clockwise
};

void light() {
	
		GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
		GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
		GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light_position[] = { 0, 5, 0, 1.0 };
		if (isLight) {
			glEnable(GL_LIGHTING);
			glEnable(GL_NORMALIZE);
			glEnable(GL_LIGHT0);

			if (isAmbient == true) { glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); }
			else { glLightfv(GL_LIGHT0, GL_AMBIENT, no_light); }

			if (isDiffuse == true) { glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); }
			else { glLightfv(GL_LIGHT0, GL_DIFFUSE, no_light); }

			if (isSpecular == true) { glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); }
			else { glLightfv(GL_LIGHT0, GL_SPECULAR, no_light); }

			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		}
		else {
			glDisable(GL_LIGHT0);
		}
	
	/* GLfloat spot_direction[] = { 0.0, -1.0, 0.0 };
	 glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	 glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 10.0); */

}


//control points
long long nCr(int n, int r)
{
	if (r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
	long long ans = 1;
	int i;

	for (i = 1; i <= r; i++)
	{
		ans *= n - r + i;
		ans /= i;
	}

	return ans;
}

//polynomial interpretation for N points
void BezierCurve(double t, float xy[2])
{
	double y = 0;
	double x = 0;
	t = t > 1.0 ? 1.0 : t;
	for (int i = 0; i <= L; i++)
	{
		int ncr = nCr(L, i);
		double oneMinusTpow = pow(1 - t, double(L - i));
		double tPow = pow(t, double(i));
		double coef = oneMinusTpow * tPow * ncr;
		x += coef * ctrlpoints[i][0];
		y += coef * ctrlpoints[i][1];

	}
	xy[0] = float(x);
	xy[1] = float(y);

	//return y;
}

///////////////////////
void setNormal(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
	GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

	Ux = x2 - x1;
	Uy = y2 - y1;
	Uz = z2 - z1;

	Vx = x3 - x1;
	Vy = y3 - y1;
	Vz = z3 - z1;

	Nx = Uy * Vz - Uz * Vy;
	Ny = Uz * Vx - Ux * Vz;
	Nz = Ux * Vy - Uy * Vx;

	glNormal3f(-Nx, -Ny, -Nz);
}
void showControlPoints()
{
	glPointSize(5.0);
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_POINTS);
	for (int i = 0; i <= L; i++)
		glVertex3fv(&ctrlpoints[i][0]);
	glEnd();
}

void drawValley(glm::vec3 ambientColorVector = glm::vec3(0.25, 0.1, 0.1),
	glm::vec3 diffColorVector = glm::vec3(0.5, 0.2, 0.2))
{

	//necessary transformations 
	glRotatef(90, 0.0, 0.0, 1.0);
	glScalef(0.2, 0.2, 0.2);
	glTranslatef(10, -3, 2.5); // y x z
	glScalef(0.5, 0.5, 0.5);
	glTranslatef(-10, 0, 0);
	glScalef(10, 1, 1);
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_ambient[] = { ambientColorVector.x, ambientColorVector.y, ambientColorVector.z , 1.0 };
	GLfloat mat_diffuse[] = { diffColorVector.x,diffColorVector.y,diffColorVector.z, 1.0 };

	GLfloat mat_specular[] = { 1.0,1.0,1.0, 1.0 };
	GLfloat mat_shininess = 500.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, no_mat);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);

	if (wired)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

	}
	else
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	int i, j;
	float x, y, z, r;				//current coordinates
	float x1, y1, z1, r1;			//next coordinates
	float theta;

	const float startx = 0, endx = ctrlpoints[L][0];
	//number of angular slices
	const float dx = (endx - startx) / nt;	//x step size
	const float dtheta = 2 * 3.1416 / ntheta;		//angular step size

	float t = 0;
	float dt = 1.0 / nt;
	float xy[2];
	BezierCurve(t, xy);
	x = xy[0];
	r = xy[1];
	//rotate about z-axis
	float p1x, p1y, p1z, p2x, p2y, p2z;
	for (i = 0; i < nt; ++i)  			//step through x
	{
		theta = 0;
		t += dt;
		BezierCurve(t, xy);
		x1 = xy[0];
		r1 = xy[1];

		//draw the surface composed of quadrilaterals by sweeping theta
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= ntheta; ++j)
		{
			theta += dtheta;
			double cosa = cos(theta);
			double sina = sin(theta);
			y = r * cosa;
			y1 = r1 * cosa;	//current and next y
			z = r * sina;
			z1 = r1 * sina;	//current and next z

			//edge from point at x to point at next x
			glVertex3f(x, y, z);

			if (j > 0)
			{
				setNormal(p1x, p1y, p1z, p2x, p2y, p2z, x, y, z);
			}
			else
			{
				p1x = x;
				p1y = y;
				p1z = z;
				p2x = x1;
				p2y = y1;
				p2z = z1;

			}
			glVertex3f(x1, y1, z1);

			//forms quad with next pair of points with incremented theta value
		}
		glEnd();
		x = x1;
		r = r1;
	} //for i

}


void drawCubeWithNormal(
	glm::vec3 ambientColorVector = glm::vec3(0.25, 0.1, 0.1),
	glm::vec3 diffColorVector = glm::vec3(0.5, 0.2, 0.2),
	bool texture=false
)
{
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_ambient[] = { ambientColorVector.x, ambientColorVector.y, ambientColorVector.z , 1.0 };
	GLfloat mat_diffuse[] = { diffColorVector.x,diffColorVector.y,diffColorVector.z, 1.0 };
	
	GLfloat mat_specular[] = { 1.0,1.0,1.0, 1.0 };
	GLfloat mat_shininess = 500.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, no_mat);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);


	glBegin(GL_QUADS);

	for (GLint i = 0; i < 6; i++)
	{
		getNormal3p(v_cube[quadIndices[i][0]][0], v_cube[quadIndices[i][0]][1], v_cube[quadIndices[i][0]][2],
			v_cube[quadIndices[i][1]][0], v_cube[quadIndices[i][1]][1], v_cube[quadIndices[i][1]][2],
			v_cube[quadIndices[i][2]][0], v_cube[quadIndices[i][2]][1], v_cube[quadIndices[i][2]][2]);
		glVertex3fv(&v_cube[quadIndices[i][0]][0]);
		if (texture && i>1) {
			glTexCoord2f(1, 1);
		}
		glVertex3fv(&v_cube[quadIndices[i][1]][0]);
		if (texture && i>1) {
			glTexCoord2f(1, 0);
		}
		glVertex3fv(&v_cube[quadIndices[i][2]][0]);
		if (texture && i>1) {
			glTexCoord2f(0, 0);
		}
		glVertex3fv(&v_cube[quadIndices[i][3]][0]);
		if (texture && i>1) {
			glTexCoord2f(0, 1);
		}
	}
	glEnd();
}
void drawCube(
	glm::vec4 bottomcolorVector = glm::vec4(1.0, 0.0, 0.0, 0.0),
	glm::vec4 topcolorVector = glm::vec4(1.0, 0.0, 0.0, 0.0),
	glm::vec4 frontcolorVector = glm::vec4(1.0, 0.0, 0.0, 0.0),
	glm::vec4 backcolorVector = glm::vec4(1.0, 0.0, 0.0, 0.0),
	glm::vec4 rightcolorVector = glm::vec4(1.0, 0.0, 0.0, 0.0),
	glm::vec4 leftcolorVector = glm::vec4(1.0, 0.0, 0.0, 0.0)


) {


	float colors[6][4] = {
	   bottomcolorVector.x,bottomcolorVector.y,bottomcolorVector.z,bottomcolorVector.w,
	   topcolorVector.x,topcolorVector.y,topcolorVector.z,topcolorVector.w,
	   frontcolorVector.x,frontcolorVector.y,frontcolorVector.z,frontcolorVector.w,
	   backcolorVector.x,backcolorVector.y,backcolorVector.z,backcolorVector.w,
	   rightcolorVector.x,rightcolorVector.y,rightcolorVector.z,rightcolorVector.w,
	   leftcolorVector.x,leftcolorVector.y,leftcolorVector.z,leftcolorVector.w,

	};

	glBegin(GL_QUADS);
	for (GLint i = 0; i < 6; i++)
	{
		//glColor3fv(&colors[i][0]); // You can use colors array for each face
		glColor4fv(colors[i]);

		

		glVertex3fv(&v_cube[quadIndices[i][0]][0]);
		glVertex3fv(&v_cube[quadIndices[i][1]][0]);
		glVertex3fv(&v_cube[quadIndices[i][2]][0]);
		glVertex3fv(&v_cube[quadIndices[i][3]][0]);
	}
	glEnd();

}


void drawGround(glm::vec4 ground_color = glm::vec4(0.341, 1, 0.408, 1)) {
	/*drawCube(
		glm::vec4(ground_color.x,ground_color.y,ground_color.z,ground_color.w),
		glm::vec4(ground_color.x, ground_color.y, ground_color.z, ground_color.w),
		glm::vec4(ground_color.x, ground_color.y, ground_color.z, ground_color.w),
		glm::vec4(ground_color.x, ground_color.y, ground_color.z, ground_color.w),
		glm::vec4(ground_color.x, ground_color.y, ground_color.z, ground_color.w),
		glm::vec4(ground_color.x, ground_color.y, ground_color.z, ground_color.w)

	);*/
	glm::vec3 ambientColorVector = glm::vec3(0.086, 0.98, 0.176);
	glm::vec3 diffColorVector = glm::vec3(0.835, 1, 0.835);
	drawCubeWithNormal(ambientColorVector,diffColorVector,false);

}

void drawBase(glm::vec4 base_color = glm::vec4(0.702, 0.702, 0.702, 1)) {
	
	/*drawCube(
		glm::vec4(base_color.x, base_color.y, base_color.z, base_color.w),
		glm::vec4(base_color.x, base_color.y, base_color.z, base_color.w),
		glm::vec4(base_color.x, base_color.y, base_color.z, base_color.w),
		glm::vec4(base_color.x, base_color.y, base_color.z, base_color.w),
		glm::vec4(base_color.x, base_color.y, base_color.z, base_color.w),
		glm::vec4(base_color.x, base_color.y, base_color.z, base_color.w)

	);*/


	drawCubeWithNormal(glm::vec3(0.122, 0.122, 0.122), glm::vec3(0.89, 0.89, 0.89),false);

	
	//drawCubeWithMatProp();
	
}
void gridGenerate(double x, double y, double z) {
	double dx = 2.0;
	double dz = 2.0;
	double x_init = x;
	for (int i = 0; i < 9; i++) {

		for (int j = 0; j < 9; j++) {
			glPushMatrix();
			glScalef(0.5, 0.5, 0.5);
			glTranslatef(x + dx,0, z);
			x += dx;
			drawBase();
			glPopMatrix();

		}
		z += dz;
		x = x_init;
	}
}
void drawBuilding() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 1);
	glScalef(0.5, 10, 0.5);
	//drawCube();
	drawCubeWithNormal(glm::vec3(1.0,1.0,1.0),glm::vec3(0.5, 0.5, 0.5),true);
}
void drawWindTurbine() {
	glPushMatrix();

	glScalef(0.3, 2.0, 0.3);
	drawCubeWithNormal(glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), false);
	glPopMatrix();
	glPushMatrix();



	glTranslatef(-0.1, 2, -1.2);
	glTranslatef(1, 0, 1.3);
	glRotatef(rotateTurbine, 1, 0, 0);
	glTranslatef(-1, 0, -1.3);
	glScalef(0.1, 0.3, 2.5);

	drawCubeWithNormal(glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), false);

	glPopMatrix();
	
	
}
void proceduralGenerate() {

	for (int i = 0; i < number_of_cubes; i++) {
		glPushMatrix();
		glScalef(0.5, 0.05, 0.5);
		glTranslatef(pos[i][0], 0, pos[i][1]);

		drawBase();
		glTranslatef(0.5 / 2, 0.05 / 2, 0.5 / 2);
		glScalef(1, heights[i], 1);
		drawBuilding();
		glPopMatrix();

	}
	for (int i = 0; i < 5; i++) {
		glPushMatrix();
		glScalef(0.3, 0.3, 0.3);
		glTranslatef(39, 0, 5+5*i);
		drawWindTurbine();
		glPopMatrix();
	}
	
}




//Drawing function
void DisplayFunc() {
	//glEnable(GL_CULL_FACE);
	glClearColor(0.635, 0.941, 1, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.size.x, window.size.y);
	
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
	glShadeModel(GL_SMOOTH);   // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
	
	
	glm::mat4 model, view, projection;
	camera.Update();
	camera.GetMatricies(projection, view, model);
	
	glm::mat4 mvp = projection * view * model;	//Compute the mvp matrix
	glLoadMatrixf(glm::value_ptr(mvp));
	
	//light();
	
	
	///eikhane aktasi

	glPushMatrix();
	glScalef(10, 0.5, 10);
	glTranslatef(0.0f, -1.0f, 0.0f);
	drawGround();
	glPopMatrix();
	
	proceduralGenerate();
	glScalef(10, 0.5, 10);
	
	
	glTranslatef(1.0f, -1.0f, 0.0f);
	glPushMatrix();
	drawGround();
	glPopMatrix();
	
	
	glPushMatrix();
	
	drawValley(glm::vec3(0.086, 0.98, 0.176),glm::vec3(0.086, 0.98, 0.176));
	if (shcpt)
	{

		showControlPoints();
	}
	glPopMatrix();
	if (turnonTurbine) {
		rotateTurbine += 1;
		rotateTurbine = rotateTurbine % 360;
		
	}
	

	
	glutSwapBuffers();
}
//Redraw based on fps set for window
void TimerFunc(int value) {
	if (window.window_handle != -1) {
		glutTimerFunc(window.interval, TimerFunc, value);
		glutPostRedisplay();

	}
}

int main(int argc, char* argv[]) {
	//glut boilerplate
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 512);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	//Setup window and callbacks
	window.window_handle = glutCreateWindow("Pcity");
	glutReshapeFunc(ReshapeFunc);
	glutDisplayFunc(DisplayFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutMouseFunc(CallBackMouseFunc);
	glutMotionFunc(CallBackMotionFunc);
	glutPassiveMotionFunc(CallBackPassiveFunc);
	glutTimerFunc(window.interval, TimerFunc, 0);

	//glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		cerr << "GLEW failed to initialize." << endl;
		return 0;
	}
	//Setup camera
	camera.SetMode(FREE);
	camera.SetPosition(glm::vec3(0, 0, -1));
	camera.SetLookAt(glm::vec3(0, 0, 0));
	camera.SetClipping(.1, 1000);
	camera.SetFOV(45);
	//generate
	generatePositions();
	
	// light lagachi
	light();
	// texture lagachi
	LoadTexture("building_texture.bmp");
	
	std::cout << "Press l to toggle light 0\n";

	std::cout << "Press y to toggle diffuse light 0\n";

	std::cout << "Press z to toggle specular light 0\n";

	std::cout << "Press v to toggle ambient light 0\n";

	std::cout << "Press r to reset camera \n";

	std::cout << "press t to turn on wind turbine\n";
	//koshto hoccche!!!
	glutMainLoop();
	return 0;
}