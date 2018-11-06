#include <GL/glut.h>
#include <iostream>
#include <windows.h>
#include <vector>
#include <math.h>
#define WIDTH 1024
#define HEIGHT 768
#define TIMER 17
#define GM 0.401408
#define ESCAPEVOLTEX 0.112			//오차값 +-0.4%미만

void SetupRC();
void RenderScene(void);
void ChangeSize(int w, int h);
void TimerFunction(int value);
void processNormalKeys(unsigned char key, int x, int y);
void processMouse(int button, int state, int x, int y);
void processMouseActiveMotion(int x, int y);
float Sphere1_degree = 0.0f;
float rocket_digree = 0;
float multiTime = 10000;
float rotate_theta_x = 0;
float rotate_theta_y = 0;
float rotate_theta_z = 0;
int keyDown = 0;
int nowX, nowY;
int pause=0;
using namespace std;
typedef struct Object {
	double x, y;
	double vx, vy;
	double m;
	int r, g, b;
}obj;
vector<obj> objList;

obj operator+(obj desc, obj src) {
	obj tmp= {
		desc.x, desc.y,
		(desc.vx*desc.m + src.vx*src.m) / (desc.m + src.m), (desc.vy*desc.m + src.vy*src.m) / (desc.m + src.m),
		desc.m + src.m,
		(desc.r*desc.m + src.r*src.m) / (desc.m+src.m), (desc.g*desc.m + src.g*src.m) / (desc.m + src.m), (desc.b*desc.m + src.b*src.m) / (desc.m + src.m)
	};
	return tmp;
}
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Univers Drawing");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(processNormalKeys);
	glutTimerFunc(TIMER, TimerFunction, 1);
	SetupRC();
	glutMainLoop();
	return 0;
}
void SetupRC()
{
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
}
void RenderScene(void)
{
	// 윈도우를 지정된 색으로 clear한다.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 1, 1, 0.1);
	// World Transfrom
	glPushMatrix();
	/////////////////////// Drawing /////////////////////
	if (!pause) {
		if (objList.size() > 1) {
			for (size_t i = 0; i < objList.size()-1; i++) {
				for (size_t j = i + 1; j < objList.size(); j++) {
					if ((objList[i].x - objList[j].x)*(objList[i].x - objList[j].x) + (objList[i].y - objList[j].y) * (objList[i].y - objList[j].y)
						< (sqrt(objList[i].m) + sqrt(objList[j].m))*(sqrt(objList[i].m) + sqrt(objList[j].m)) * 2) {
						obj tmp = objList[i] + objList[j];
						objList.erase(objList.begin() + j);
						objList.erase(objList.begin() + i);
						objList.push_back(tmp);
						i -= 2;
						break;
					}
				}
			}
			vector<obj> tmpList = objList;
			for (unsigned int i = 0; i < objList.size() - 1; i++) {
				for (unsigned int j = i + 1; j < objList.size(); j++) {
					double dx = tmpList[i].x - tmpList[j].x
						, dy = tmpList[i].y - tmpList[j].y;
					double d = sqrt(dx*dx + dy*dy);
					double Fx = tmpList[i].m*tmpList[j].m / (d*d)  * dx / (fabs(dx) + fabs(dy)) / 100 * TIMER
						, Fy = tmpList[i].m*tmpList[j].m / (d*d)  * dy / (fabs(dx) + fabs(dy)) / 100 * TIMER;
					double axi = -Fx / tmpList[i].m
						, axj = Fx / tmpList[j].m
						, ayi = -Fy / tmpList[i].m
						, ayj = Fy / tmpList[j].m;
					objList[i].vx += axi;
					objList[i].vy += ayi;
					objList[j].vx += axj;
					objList[j].vy += ayj;
				}
			}
			for (unsigned int i = 0; i < objList.size(); i++) {
				objList[i].vx -= fabs(tmpList[i].vx)*tmpList[i].vx*1.2255343 * 100 * 0.3 / objList[i].m;
				objList[i].vy -= fabs(tmpList[i].vy)*tmpList[i].vy*1.2255343 * 100 * 0.3 / objList[i].m;
			}
		}
		for (unsigned int i = 0; i < objList.size(); i++) {
			objList[i].x += objList[i].vx*1000;
			objList[i].y += objList[i].vy*1000;
			if (objList[i].x > 500 || objList[i].x < -500 || objList[i].y>500 || objList[i].y < -500) 
				objList.erase(objList.begin() + i--);
		}
	}
	/////////////////////////////////////////////////////
	for (unsigned int i = 0; i < objList.size(); i++) {
		printf("%.4f %.4f %.4f %.4f %.4f %3d %3d %3d\n",objList[i].x, objList[i].y, objList[i].vx, objList[i].vy, objList[i].m, objList[i].r,objList[i].g,objList[i].b);
		glPushMatrix();
		glLoadIdentity();
		//glColor3d(1,1,1);
		glColor3d((double)objList[i].r / 256, (double)objList[i].g / 256, (double)objList[i].b / 256);
		glTranslated(objList[i].x, objList[i].y, 0);
		glutSolidSphere(sqrt(objList[i].m), 50, 50);

		glPopMatrix();
	}
	glPopMatrix();
	glutSwapBuffers();
}
void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;
	// h가 0일 때
	if (h == 0)
		h = 1;
	// 윈도우의 폭과 넓이 설정
	glViewport(0, 0, w, h);

	//원근투영 사용
	GLfloat fAspect;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	fAspect = (float)w / (float)h;
	gluPerspective(60.0, fAspect, 1.0, 500.0);
	glTranslatef(0.0f, 0.0f, -500.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glutSwapBuffers();
}
void TimerFunction(int value)
{
	Sphere1_degree = (int)(Sphere1_degree + 5) % 360;
	glutPostRedisplay();
	glutTimerFunc(TIMER, TimerFunction, 1);
}
void processNormalKeys(unsigned char key, int x, int y)
{
	if (key == 27)//esc
		exit(0);//프로그램 종료
	if (key == ' ') {
		objList.push_back({ (float)((x-512)/1.33),(float)((384 - y)/1.33),0,0,10,rand()%256,rand()%256,rand()%256});
	}if (key == 's' || key == 'S') {
		pause = 1;
	}if (key == 'g' || key == 'G') {
		pause = 0;
	}
}
void processMouse(int button, int state, int x, int y)
{
	if ((state == GLUT_DOWN))
	{
		if (button == GLUT_LEFT) {
			keyDown = 1;
			nowX = x;
			nowY = y;
		}
	}
	if (state == GLUT_UP) {
		if (button == GLUT_LEFT) {
			keyDown = 0;
		}
	}
}
void processMouseActiveMotion(int x, int y)
{
}