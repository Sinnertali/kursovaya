#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}


}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	double norm(double t1_x, double t1_y, double t1_z, double mid_x, double mid_y, double mid_z, double t2_x, double t2_y, double t2_z, char coord);
	//������ ��������� ���������� ��������
/*	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/


	//glBindTexture(GL_TEXTURE_2D, texId);

	double A[] = { 0, 0, 0 };
	double B[] = { 7, 6, 0 };
	double C[] = { 3, 9, 0 };
	double D[] = { 2, 4, 0 };
	double E[] = { -6, 1, 0 };
	double F[] = { -3, 5, 0 };
	double G[] = { -3, 8, 0 };
	double H[] = { -9, 10, 0 };
	double I[] = { -8,-1,0 };


	double A1[] = { 0, 0, 1 };
	double B1[] = { 7, 6, 1 };
	double C1[] = { 3, 9, 1 };
	double D1[] = { 2, 4, 1 };
	double E1[] = { -6, 1, 1 };
	double F1[] = { -3, 5, 1 };
	double G1[] = { -3, 8, 1 };
	double H1[] = { -9, 10, 1 };
	double I1[] = { -8,-1,1 };



	///��������� ��������������
	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(1, 0, 0);



	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(D);

	glColor4d(1, 0, 0, 0.3);
	glNormal3d(0, 0, 1);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(D1);

	glEnd();



	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0.5, 0, 0);



	glVertex3dv(C);
	glVertex3dv(B);
	glVertex3dv(D);

	glColor4d(0.5, 0, 0, 0.3);
	glNormal3d(0, 0, 1);
	glVertex3dv(C1);
	glVertex3dv(B1);
	glVertex3dv(D1);

	glEnd();


	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0, 1, 0);



	glVertex3dv(A);
	glVertex3dv(E);
	glVertex3dv(D);

	glColor4d(0, 1, 0, 0.3);
	glNormal3d(0, 0, 1);
	glVertex3dv(A1);
	glVertex3dv(E1);
	glVertex3dv(D1);

	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0.7, 0.5, 0);



	glVertex3dv(A);
	glVertex3dv(E);
	glVertex3dv(I);

	glColor4d(1, 0.5, 0, 0.5);
	glNormal3d(0, 0, 1);
	glVertex3dv(A1);
	glVertex3dv(E1);
	glVertex3dv(I1);

	glEnd();


	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(0, 0, 0.5);



	glVertex3dv(F);
	glVertex3dv(E);
	glVertex3dv(I);

	glColor4d(0, 1, 1, 0.3);
	glNormal3d(0, 0, 1);
	glVertex3dv(F1);
	glVertex3dv(E1);
	glVertex3dv(I1);

	glEnd();


	glBegin(GL_TRIANGLES);
	glNormal3d(0, 0, -1);
	glColor3d(1, 0.3, 0);



	glVertex3dv(G);
	glVertex3dv(F);
	glVertex3dv(H);


	glNormal3d(0, 0, 1);
	glVertex3dv(G1);
	glVertex3dv(F1);
	glVertex3dv(H1);

	glEnd();





	///��������� �����

	glBegin(GL_QUADS);

	glColor3d(1, 0.5, 0.5);

	glNormal3d(0, 0, 1);


	glNormal3d(norm(B[0], B[1], 0, B[0], B[1], 1, C[0], C[1], 1, 'x'), norm(B[0], B[1], 0, B[0], B[1], 1, C[0], C[1], 1, 'y'), norm(B[0], B[1], 0, B[0], B[1], 1, C[0], C[1], 1, 'z'));



	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(1, 0);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(0, 1);
	glVertex3dv(C);

	glVertex3dv(C);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(D);

	glVertex3dv(D);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glVertex3dv(E);

	glVertex3dv(E);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glVertex3dv(F);

	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glVertex3dv(G);

	glVertex3dv(G);
	glVertex3dv(G1);
	glVertex3dv(H1);
	glVertex3dv(H);

	glNormal3d(-norm(B[0], B[1], 0, B[0], B[1], 1, C[0], C[1], 1, 'x'), -norm(B[0], B[1], 0, B[0], B[1], 1, C[0], C[1], 1, 'y'), -norm(B[0], B[1], 0, B[0], B[1], 1, C[0], C[1], 1, 'z'));

	glVertex3dv(I);
	glVertex3dv(I1);
	glVertex3dv(A1);
	glVertex3dv(A);

	glEnd();




	///����� ����������



	double r, x1 = 0, y1 = 0, x2 = 0, y2 = 0, x0, y0;
	x0 = 3.5;
	y0 = 3;

	double alf, O1[] = { x0,y0,0 }, O2[] = { x0,y0,1 }, X1[] = { x1,y1,0 }, X2[] = { x2,y2,0 }, Y1[] = { x1,y1,1 }, Y2[] = { x2,y2,1 };
	r = sqrt(7. * 7. + 6. * 6.) / 2;
	alf = atan(y0 / x0);

	for (double i = 0; i < 36; i++)
	{
		x1 = -cos(((180 - i * 5) * 3.14 / 180) + alf) * r + x0;
		y1 = -sin(((180 - i * 5) * 3.14 / 180) + alf) * r + y0;

		x2 = -cos(((180 - (i + 1) * 5) * 3.14 / 180) + alf) * r + x0;
		y2 = -sin(((180 - (i + 1) * 5) * 3.14 / 180) + alf) * r + y0;

		X1[0] = x1;
		X1[1] = y1;
		X2[0] = x2;
		X2[1] = y2;
		Y1[0] = x1;
		Y1[1] = y1;
		Y2[0] = x2;
		Y2[1] = y2;

		glBegin(GL_TRIANGLES);

		glColor3d(0, 1, 0.5);
		glNormal3d(0, 0, -1);
		glVertex3dv(O1);
		glVertex3dv(X1);
		glVertex3dv(X2);

		glColor3d(1, 0, 0.5);
		glNormal3d(0, 0, 1);
		glVertex3dv(O2);
		glVertex3dv(Y1);
		glVertex3dv(Y2);
		glEnd();

		glBegin(GL_QUADS);

		glColor3d(0, 0, 0.5);
		glNormal3d(-norm(x1, y1, 0, x1, y1, 1, x2, y2, 1, 'x'), -norm(x1, y1, 0, x1, y1, 1, x2, y2, 1, 'y'), -norm(x1, y1, 0, x1, y1, 1, x2, y2, 1, 'z'));
		glVertex3dv(X1);
		glVertex3dv(X2);
		glVertex3dv(Y2);
		glVertex3dv(Y1);
		glEnd();

	}

	///����� ���������

	for (y1 = -1; y1 < 10; y1 += 0.01)
	{

		x1 = sqrt(8.45 * 8.45 - ((y1 - 3.92105) * (y1 - 3.92105))) - 14.86842;

		y2 = y1 + 0.01;
		x2 = sqrt(8.45 * 8.45 - ((y2 - 3.92105) * (y2 - 3.92105))) - 14.86842;


		X1[0] = x1;
		X1[1] = y1;
		X2[0] = x2;
		X2[1] = y2;
		Y1[0] = x1;
		Y1[1] = y1;
		Y2[0] = x2;
		Y2[1] = y2;

		glBegin(GL_QUADS);

		glColor3d(0, 1, 0);
		glNormal3d(-norm(x1, y1, 0, x1, y1, 1, x2, y2, 1, 'x'), -norm(x1, y1, 0, x1, y1, 1, x2, y2, 1, 'y'), -norm(x1, y1, 0, x1, y1, 1, x2, y2, 1, 'z'));
		glVertex3dv(X1);
		glTexCoord2d(0, 0);
		glVertex3dv(X2);
		glTexCoord2d(1, 0);
		glVertex3dv(Y2);
		glTexCoord2d(1, 1);
		glVertex3dv(Y1);
		glTexCoord2d(0, 1);


		glEnd();

		glBegin(GL_TRIANGLES);

		glColor3d(0.5, 1, 1);
		glNormal3d(0, 0, -1);
		glVertex3dv(F);
		glVertex3dv(X2);
		glVertex3dv(X1);

		glColor3d(1, 0.5, 1);
		glNormal3d(0, 0, 1);
		glVertex3dv(F1);
		glVertex3dv(Y2);
		glVertex3dv(Y1);

		glEnd();

	}


	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

double norm(double t1_x, double t1_y, double t1_z, double mid_x, double mid_y, double mid_z, double t2_x, double t2_y, double t2_z, char coord)
{
	double a_y = t1_y - mid_y, b_y = t2_y - mid_y, a_x = t1_x - mid_x, b_x = t2_x - mid_x, a_z = t1_z - mid_z, b_z = t2_z - mid_z;
	switch (coord)
	{
	case 'x':
		return  (a_y * b_z - a_z * b_y);
		break;
	case 'y':
		return -1 * (a_x * b_z - a_z * b_x);
		break;
	case 'z':
		return (a_x * b_y - a_y * b_x);
		break;
	}

}