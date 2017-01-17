#ifndef FREEFLYCAMERA_H
#define FREEFLYCAMERA_H

#include "vector3d.h"
#include <map>

// #define M_PI = 3.14159265358979;

class cameraFreeFly
{
public:

// Variable de classe
    Vector3D _position;
    Vector3D _target;
    Vector3D _forward;
    Vector3D _left;
    double _theta;
    double _phi;

	int _xold;
	int _yold;

// Constructeur
	cameraFreeFly(const Vector3D & position = Vector3D(0,0,0));

// Méthodes
	// Pour les calcules de positionnement de la camera
	void VectorsFromAngles();

	// Pour définir la nouvelle vue (glutLookAt())
	virtual void look();	// Pour définir la nouvelle vue (glutLookAt())
	virtual void look(float z);

	// Gestion du mouvement de la souris lorsqu'aucun bouon n'est appuyé
	virtual void OnMouseMotion(int x,int y);

	// Gestion des touches spéciales du clavier
	virtual void OnKeyboard(int touche,int x,int y);

public:
// Destructeur
	~cameraFreeFly(void);
};
#endif //FREEFLYCAMERA_H	