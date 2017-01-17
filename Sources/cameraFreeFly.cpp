#include "cameraFreeFly.h"
#include <cmath>
#include <GL/glut.h>

// Constructeur
cameraFreeFly::cameraFreeFly(const Vector3D & position)
{
	_position = position; //si aucune position n'est définie on reçoit quand même (0,0,0) en paramètre
    _phi = 0;
    _theta = 0;
	_xold = 0;
	_yold = 0;
    VectorsFromAngles(); 
}

// Lors d'un déplacement de la souris
void cameraFreeFly::OnMouseMotion(int x,int y)
{
	_theta=_theta-(x-_xold); 
	_phi=_phi-(y-_yold);
	//glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */

	_xold=x; /* sauvegarde des valeurs courantes de la position de la souris */
	_yold=y;


	VectorsFromAngles();
	//glutPostRedisplay();
}

// Gestion des touches spéciales du clavier
void cameraFreeFly::OnKeyboard(int touche,int x,int y)
{
switch (touche)
    {
	case GLUT_KEY_LEFT : 
		_position += (_left /3);
		//glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT :
		
		_position -= (_left / 3);
		//glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN : 
		//cameraZ -=0.1;
		_position -= (_forward / 3);
		//glutPostRedisplay();
		break;
	case GLUT_KEY_UP : 
		//cameraZ +=0.1;
		_position += (_forward / 3);
		//glutPostRedisplay();
		break;

	case GLUT_KEY_END : 
		//glutPostRedisplay();
		break;
}
_target = _position + _forward;
//glutPostRedisplay();
}

// Pour les calcules de positionnement de la camera
void cameraFreeFly::VectorsFromAngles()
{
	double M_PI = 3.14159265358979;
	
//	printf("theta %d ", theta);
	static const Vector3D up(0,0,1); //une constante, le vecteur vertical du monde, utilisé dans les calculs

//On limite les valeurs de phi
    if (_phi > 89)
        _phi = 89;
    else if (_phi < -89)
        _phi = -89;
		
//passage des coordonnées sphériques aux coordonnées cartésiennes
 
	double r_temp = cos(_phi * M_PI/180);
    _forward.Z = sin(_phi*M_PI/180);
    _forward.X = r_temp*cos(_theta*M_PI/180);
    _forward.Y = r_temp*sin(_theta*M_PI/180);

    _left = up.crossProduct(_forward);
    _left.normalize();

//avec la position de la caméra et la direction du regard, on calcul facilement ce que regarde la caméra (la cible)
    _target = _position + _forward;
}

// Repostionne la camera avec un gluLookAt
void cameraFreeFly::look()
{
    gluLookAt(_position.X,_position.Y,_position.Z,
              _target.X,_target.Y,_target.Z,
              0,0,1);
}
// Repostionne la camera avec un gluLookAt
void cameraFreeFly::look(float z)
{
    gluLookAt(_position.X,_position.Y,_position.Z+z,
              _target.X,_target.Y,_target.Z,
              0,0,1);
}

// *** Destructeur
cameraFreeFly::~cameraFreeFly(void)
{
}
