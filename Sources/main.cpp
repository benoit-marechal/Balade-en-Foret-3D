/**********************************************
* Projet Synthese d'image
* Sujet : Balade en foret tropical
* Auteur : Anthony Maréchal et Benoit Maréchal
**********************************************/

// *** Inclusion des librairies
#include "stdafx.h" 
#include <map>
#include <math.h>
#include "cameraFreeFly.h"
#include "Point.h"
#include <iostream> 
#include <fstream> 
#include <stdlib.h>
#include <stdio.h>


//using namespace std; 

/*****************************
		Constantes
******************************/
#define l 100
#define L 100

#define MAX 255 
#define NBPOINT   l*L
#define NBFACE (L-1)* 2 * (l-1)

//FRACTALE
#define drand48 1.0/RAND_MAX*rand
#define srand48 srand
#define max2 30000 /* nombre d'iterations */
#define seed 68111 

/******************************
		Variables globales
*******************************/

//animation
double anim=0.0;
// Point du terrain (les sommets)
Point pt[NBPOINT];

// Face terrain
int ft[NBFACE][3] ;

// Tampon des hauteurs
float Ybuffer[l][L] ;

// face courante (rouge)
Point fc[3]; 

// Permet de savoir si le mode marche est activé
int marche = 1;

// Permet de savoir si les lumieres sont activé
int lumiere = 0;

// Permet de passer en mode filaire
int filaire = 0;

int maillage1 = 0;

float K = 4;

// Variable char contenant le caractere presse par l'utilitateur
// utile pour l'ecoute du clavier
char presse;

// Hauteur max
float hauteurMax = 10;

float fdensite = 0;

char * map = "ile.pgm";

// Variable décrivant l'angle de la camera et les coordonnées de la souris
int theta,phi,cx,cy,cxold,cyold;

 GLuint IdTex[14]; /* tableau d'Id pour les textures */



/*
 On recupere les methodes de la camera
 et on les passe en static, pour pouvoir
 les passer a glut (sinon c pas possible...)
*/
static cameraFreeFly* camera;
extern "C" void OnMouseMotion(int x,int y)
{
   camera->OnMouseMotion( x, y);
}
extern "C" void OnKeyboard(int touche,int x,int y)
{
   camera->OnKeyboard( touche, x, y);
}
 

// Variable décrivant l'angle du cube, et les coordonnees de la souris
int anglex,angley,x,y,xold,yold;




/***********************************************
		 Prototype des fonctions 
*************************************************/


// "affichage"  est la fonction appelee en boucle.
void affichage();

/* Fonction décrivant les comportements que doit avoir
le clavier, selon la touche pressée.*/
void clavier(unsigned char touche,int x,int y);
void clavier2(int touche,int x,int y); // pour les touches spéciales


/* reshape défini le viewport de la fenetre selon 2 coordonnees.
*/
void reshape(int x,int y);
/*
idle() décrit le comportement du programme lorsqu'un evenement
n'est pas intersepté. C'est donc dans cette fonction qu'on 
défini les animations
*/
void idle();
/*
* mouse() est l'equivalente de la fonction clavier() mais
pour la souris. A l'aide des constantes GLUT on 
décrit donc le comportement du programme en fonction du bouton
de son etat et dans coordonnées appuyées.
*/
void mouse(int bouton,int etat,int x,int y);

/*mousemotion decrit ce qu'il faut faire lorsqu'on
bouge la souris */
void mousemotion(int x,int y);
// Déplacement sans bouton appuyé
void mousemotion2(int x,int y);

// Initialise le maillage de triangle (la hauteur vaut toujours 0)
void initMaillage();

// Initialise la hauteur de chaque point du maillage (a partir d'une heightmap)
void initHauteur(char *);

// Initialise le menu contextuel
void initMenu();

void initArbres();

// Action a realiser en fonction du menu selectionné
void gereMenu1(int value);

// Pour le mode marche qui suit dénivelé du terrain
void marcheur();

// Affiche le Terrain
void afficheTerrain();

// Affiche lumiere
void afficheLumiere();

// Affiche le triedre
void afficheTriedre();

// Affiche un arbre
void afficheArbre(int numTexture,float scale, float x, float y);

// Affiche l'arbre fractale
void afficheFractale(float xp, float yp);

// Affiche les nuages
void afficheNuage(float , float , float );

/***************
 TEXTURE ARBRE
****************/


/* OpenGL texture info */
typedef struct
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint	internalFormat;
  GLuint id;

  GLubyte *texels;

} gl_texture_t;


#pragma pack(push, 1)

/* tga header */
typedef struct
{
  GLubyte id_lenght;          /* size of image id */
  GLubyte colormap_type;      /* 1 is has a colormap */
  GLubyte image_type;         /* compression type */

  short	cm_first_entry;       /* colormap origin */
  short	cm_length;            /* colormap length */
  GLubyte cm_size;            /* colormap size */

  short	x_origin;             /* bottom left x coord origin */
  short	y_origin;             /* bottom left y coord origin */

  short	width;                /* picture width (in pixels) */
  short	height;               /* picture height (in pixels) */

  GLubyte pixel_depth;        /* bits per pixel: 8, 16, 24 or 32 */
  GLubyte image_descriptor;   /* 24 bits = 0x00; 32 bits = 0x80 */

} tga_header_t;
#pragma pack(pop)

// Notre structure point 
typedef struct
{
  float x;
  float y;
  float z;
} point;


/******** PROTO *************/
void ReadTGA8bits(FILE *, GLubyte *, gl_texture_t *);
void ReadTGA16bits(FILE *, gl_texture_t *);
void ReadTGA24bits(FILE *,gl_texture_t *);
void ReadTGA32bits(FILE *,gl_texture_t *);
void ReadTGAgray8bits(FILE *,gl_texture_t *);
void ReadTGAgray16bits(FILE *,gl_texture_t *);
void ReadTGA8bitsRLE(FILE *, GLubyte *, gl_texture_t *);
void ReadTGA16bitsRLE(FILE *,gl_texture_t *);
void ReadTGA24bitsRLE(FILE *, gl_texture_t *);
void ReadTGA32bitsRLE(FILE *,gl_texture_t *);
void ReadTGAgray8bitsRLE(FILE *,gl_texture_t *);
void ReadTGAgray16bitsRLE(FILE *,gl_texture_t *);
GLuint loadTGATexture (const char *filename,int iden);

/* texture id for exemple */
GLuint texId = 0;
GLuint texId2 = 0;
GLuint texId3 = 0;
GLuint texId4 = 0;
GLuint texId5 = 0;
GLuint texId6 = 0;
GLuint texId7 = 0;
GLuint texId8 = 0;
GLuint texId9 = 0;
GLuint texId10 = 0;
GLuint texId11 = 0;
GLuint texId12 = 0;
GLuint texId13 = 0;
GLuint texId14 = 0; //nuage

char  *arbre1="arbre1.tga";
char  *arbre2="arbre2.tga";
char  *arbre3="arbre3.tga";
char  *arbre4="arbre4.tga";
char  *arbre5="arbre5.tga";
char  *arbre6="arbre6.tga";
char  *arbre7="arbre7.tga";
char  *arbre8="arbre9.tga";
char  *arbre9="arbre8.tga";
char  *sol1="sol1_32.tga";
char  *sol2="sol2_32.tga";
char  *sol3="sol3_32.tga";
char  *soleil="soleil.tga";
char  *nuage="nuage.tga"; //nuage

int nbArbres = 200;
int tarbreX[l*L];
int tarbreY[l*L];
int tarbreN[l*L];

GLfloat color[]= {0.7,0.7,0.7,0.0};

// Fonction Principale
int main(int argc,char **argv)
{

// 1 - Initialisation de glut et creation de la fenetre
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(200,200);
	glutInitWindowSize(1000,600);
	glutCreateWindow("Balade en foret tropical");
	glutSetCursor(GLUT_CURSOR_NONE);

// 2 - Initialisation d'OpenGL
	// Init de la camera
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(70,(double)800/600,1,1000);

	glClearColor(0.4,0.6,1,0.0);
	glColor3f(1.0,1.0,1.0);
	glPointSize(2.0);
	glEnable(GL_DEPTH_TEST); // activation du z-buffer

//	initialisation du menu	
	initMenu();


// TEXTURE ARBRE
	
  // On charge la texture TGA
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.9);
  glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

  //Chargement de tte les textures dans IdTex
  glGenTextures (14, IdTex);		
  if (!(texId = loadTGATexture (arbre1,IdTex[0]))){ exit (-1); }
if (!(texId2 = loadTGATexture (arbre2,IdTex[1]))){ exit (-1); }
if (!(texId3 = loadTGATexture (arbre3,IdTex[2]))){ exit (-1); }
if (!(texId4 = loadTGATexture (arbre4,IdTex[3]))){ exit (-1); }
if (!(texId5 = loadTGATexture (arbre5,IdTex[4]))){ exit (-1); }
if (!(texId6 = loadTGATexture (arbre6,IdTex[5]))){ exit (-1); }
if (!(texId7 = loadTGATexture (arbre7,IdTex[6]))){ exit (-1); }
if (!(texId8 = loadTGATexture (arbre8,IdTex[7]))){ exit (-1); }
if (!(texId9 = loadTGATexture (arbre9,IdTex[8]))){ exit (-1); }
if (!(texId10 = loadTGATexture (sol1,IdTex[9]))){ exit (-1); }
if (!(texId11 = loadTGATexture (sol2,IdTex[10]))){ exit (-1); }
if (!(texId12 = loadTGATexture (sol3,IdTex[11]))){ exit (-1); }
if (!(texId13 = loadTGATexture (soleil,IdTex[12]))){ exit (-1); }
if (!(texId14 = loadTGATexture (nuage,IdTex[13]))){ exit (-1); }

// 3 - Enregistrement des fonctions de rappel
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutSpecialFunc(OnKeyboard); // Gere par la camera
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);
  glutPassiveMotionFunc(OnMouseMotion); // Gere par la camera
  glutIdleFunc(idle);

// 4 - Initialisation de la camera
	Vector3D  vect  = Vector3D(25,25,100);
	camera = new cameraFreeFly(vect);

// 5 - Création du terrain
	initMaillage();
	initHauteur(map);

// 6 - Génération de la position des arbres

	initArbres();

	
// Entree dans la boucle principale glut
  glutMainLoop();

  return 0;
}



/*******************
Fonction d'affichage de la camera, de la scene et des lumieres
**********************/
void affichage()
{
	// Initialisations

	// Effacement de l'image avec la couleur de fond */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Lumiere
	if(lumiere==1)
		afficheLumiere();
	else
		glDisable(GL_LIGHT0);

	// Mode marche
	if(marche==1)
		marcheur();

	// Placement de la camera
	camera->look();

	glFogi(GL_FOG_MODE,GL_EXP);
	glFogfv(GL_FOG_COLOR,color);
	glFogf(GL_FOG_START,180);
	glFogf(GL_FOG_END,300);
	glFogf(GL_FOG_DENSITY,fdensite);

	glEnable(GL_FOG);

	// On plante les ARBRES
	for(int i=0;i<nbArbres;i++) {
		afficheArbre(tarbreN[i],5,tarbreX[i],tarbreY[i]);
	}
	

	// Affichage du terrain
	afficheTerrain();

	// Soleil
	glPushMatrix();
		glRotatef(90,1.0,0.0,0.0);
		glRotatef(45,0.0,1.0,0.0);
		glTranslated(100,50,-100);

		glEnable(GL_TEXTURE_2D); 
			glBindTexture (GL_TEXTURE_2D, IdTex[12]);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-15, -15, 0.0f);

				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(15, -15, 0.0f);

				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(15, 15, 0.0f);

				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-15, 15, 0.0f);
		glEnd();
	glPopMatrix();


	// Affichage du triedre
	afficheTriedre();


	// Affiche les //nuage
	for(int i=0;i<nbArbres;i++) {	
		afficheNuage((i+1)%5,tarbreX[i],tarbreY[i]);
	}


// Fin de l'affichage
glFlush(); // Vide tampon d'affichage
glutSwapBuffers(); // Echange des buffers
glutPostRedisplay();
}

/********************************
		Fonctions
	d'Initialisations
*********************************/
void initMaillage()
{

// Création des points du maillage
int indice;
for(int i=0;i<L;i++)
{
	for(int j=0;j<l;j++)
	{
		indice = (i*l) + j;

		if(i%2 == 0) {
		pt[indice].x = (j);
		pt[indice].y = (i) ;
		pt[indice].z = 0;

		}
		else{	
			if(maillage1 != 1)
		pt[indice].x = (j)  +0.5;
			else
				pt[indice].x = (j) ;
		pt[indice].y = (i)  ;
		pt[indice].z = 0;

		}
		pt[indice].r = 0.5;
		pt[indice].g = 0.5;
		pt[indice].b = 0.5;
	}
}

// Réalisation des faces en reliant les sommets
// Calcul du nombre de faces :
printf("Nombre de faces %i : \n",(NBPOINT-l)*2);


// On reli les sommets en face (L a changé en 'l' )
int cpt=0;

for(int i=0;i<(NBPOINT-l);i++)
{
int test= i / l;
if ( (test % 2) == 0 ) // ligne pair
{
	if ( ((i+1)%l) !=0 )
	{

		ft[cpt][0]= i ;
		ft[cpt][1]= i+1 ;
		ft[cpt][2]= i+l ;

		cpt++;
	}

	if ( (i%L) != 0)
	{
		ft[cpt][0]= i ;
		ft[cpt][1]= i+l ;
		ft[cpt][2]= i+l-1 ;

		cpt++;
	} 
} // fin if
else // ligne impair
{
if ( ((i+1)%l) !=0 )
	{
		ft[cpt][0]= i ;
		ft[cpt][1]= i+l+1 ;
		ft[cpt][2]= i+l ;

		cpt++;
		ft[cpt][0]= i ;
		ft[cpt][1]= i+1 ;
		ft[cpt][2]= i+l+1 ;
		cpt++;
	} // fin if
}

} // fin for
}

void initHauteur(char * filename)
{
// 1 - Chargement du fichier
	std::ifstream entree(filename);
	char type[MAX]; // P1 ou P2 etc
	char dim[MAX]; // dimension
	char n[MAX];

	if (!entree) {
		std::cout << "Impossible de lire le fichier "<<std::endl;
	}
	else {	
		entree.getline(type,MAX);
	printf("Lecture de la Map pgm (heightmap) \n",&type);
		entree.getline(dim,MAX);
		entree.getline(dim,MAX);
	printf("dimension : %s \n",&dim);
	int i=0;
		while ( entree.eof()==0 && i<NBPOINT ) {
			entree.getline(n,MAX);
			int tt = strtol(n,NULL,10); // convertion en entier
			double tt2 = tt;
			float ff = (tt2/255)*hauteurMax;
			float fff = hauteurMax - ff;
			pt[i].z = fff;
			int ii = i/l; // Tampon de Hauteur Y Buffer
			int jj = i%l;
			Ybuffer[ii][jj] = fff;
			// Couleur en fonction de la hauteur
			pt[i].r = fff / hauteurMax ;
			pt[i].g = 1 - pt[i].r ;
			pt[i].b = 0;

		i++;
		}
	}
	entree.close();
}

// Initialisation du menu
void initMenu()
{
	glutCreateMenu(gereMenu1);
	glutAddMenuEntry("Terrain plat", '0');
  glutAddMenuEntry("Terrain smiland", '1'); 
  glutAddMenuEntry("Terrain escarpé", '2'); 
  glutAddMenuEntry("Terrain ile", '3'); 
  glutAddMenuEntry("Terrain ein", 'c'); 
  glutAddMenuEntry(" ", ' '); 
  glutAddMenuEntry("Mode Marche", '4'); 
  glutAddMenuEntry("Mode Vol libre", '5'); 
  glutAddMenuEntry(" ", ' '); 
  glutAddMenuEntry("Hauteur Max = 20", '8'); 
  glutAddMenuEntry("Hauteur Max = 40", '9'); 
  glutAddMenuEntry(" ", ' '); 
  glutAddMenuEntry("Mode filaire ON", 'a'); 
  glutAddMenuEntry("Mode filaire OFF", 'b'); 
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void gereMenu1(int value)
{
char key = value;
	switch(key)
	{
	case '0':	
				map = "plat.pgm";
				initHauteur(map);
				glutPostRedisplay();
				break;

   case '1':	map  = "smiland.pgm";
				initHauteur(map);
				glutPostRedisplay();
				break;

   case '2':	map = "escarpe.pgm";
				initHauteur(map);
				glutPostRedisplay();
				break;

   case '3':	map = "ile.pgm";
				initHauteur(map);
				glutPostRedisplay();
				break;

   case '4':	marche = 1;
				glutPostRedisplay();
				break;

   case '5':	marche = 0;
				glutPostRedisplay();
				break;

   case '6':	lumiere = 1;
				glutPostRedisplay();
				break;

   case '7':	lumiere = 0;
				glutPostRedisplay();
				break;

   case '8':	hauteurMax = 20;
				initHauteur(map);
				glutPostRedisplay();
				break;

   case '9':	hauteurMax = 40;
				initHauteur(map);
				glutPostRedisplay();
				break;

   case 'a':	filaire = 1;
				glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
				glutPostRedisplay();
				break;

   case 'b':	filaire = 0;
				glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
				glutPostRedisplay();
				break;

   case 'c':		
				map = "ein.pgm";
				initHauteur(map);
				glutPostRedisplay();
				break;
	}
}

void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
    case 'p': /* affichage du carre plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glutPostRedisplay();
      break;
    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glutPostRedisplay();
      break;
    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      glutPostRedisplay();
      break;
    case 'd': /* Activation désaction du Z-buffer */
      glEnable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'D':
      glDisable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
	 // Hauteur du terrain
    case 'h':
		hauteurMax += 0.5;
		initHauteur(map);
      glutPostRedisplay();
      break;
    case 'H':
		hauteurMax -= 0.5;
		initHauteur(map);
      glutPostRedisplay();
      break;

    case 'o':
		fdensite +=0.01;
      glutPostRedisplay();
      break;
    case 'O':
		fdensite -=0.01;
      glutPostRedisplay();
      break;
    case 'r':

      glutPostRedisplay();
      break;
    case 'a':
	  nbArbres+=60;
	  initArbres();
      glutPostRedisplay();
      break; 
    case 'A':
	
	  nbArbres-=60;
	  initArbres();
      glutPostRedisplay();
      break; 

    case 'm':
	
	  maillage1=1;
	  initMaillage();
      glutPostRedisplay();
      break; 

    case 'M':
	
	  maillage1=0;
	  initMaillage();

      glutPostRedisplay();
      break; 

    case 'k':
	
	  K+=0.1;
      glutPostRedisplay();
      break; 

    case 'K':
		K-=0.1;
      glutPostRedisplay();
      break; 

    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);
    }
}

void reshape(int x,int y)
{
  if (x<y)
    glViewport(0,(y-x)/2,x,x);
  else 
    glViewport((x-y)/2,0,y,y);

  glViewport(0,0,1000,600);

}

void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
    presse=0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x,int y)
  {
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex-=(x-xold); 
      angley-=(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }
    
    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
  }

void marcheur() 
{
	// DEBUT MARCHEUR
	long X = (camera->_position.X); // Parti entiere de X
	long Y = (camera->_position.Y); // Parti entiere de Y

	double x = camera->_position.X - X;
	double y = camera->_position.Y - Y;

	if ( y <= (-(x)+1) )
	{
		if (Y % 2 == 0) {
		fc[0].x = X;
		fc[0].y = Y;
		fc[0].z = Ybuffer[Y][X];

		fc[1].x = 1+X;
		fc[1].y = Y;
		fc[1].z = Ybuffer[Y][(1+X)];
	
		fc[2].x = X+0.5;
		fc[2].y = 1+Y;
		fc[2].z = Ybuffer[(1+Y)][X];
		}
		else
		{
		fc[0].x = X+0.5;
		fc[0].y = Y;
		fc[0].z = Ybuffer[Y][X];

		fc[1].x = X+1;
		fc[1].y = Y+1;
		fc[1].z = Ybuffer[(Y+1)][(1+X)];
	
		fc[2].x = X;
		fc[2].y = 1+Y;
		fc[2].z = Ybuffer[(1+Y)][X];
		
		}
	//	std::cout << "\nX 1+Y" << X << " "<< (1+Y);
	}
	else
	{
		if (Y % 2 == 0) {
		fc[0].x = 1+X;
		fc[0].y = Y;
		fc[0].z = Ybuffer[Y][(1+X)];

		fc[1].x = 1+X+0.5;
		fc[1].y = 1+Y;
		fc[1].z = Ybuffer[(1+Y)][(1+X)];
	
		fc[2].x = X+0.5;
		fc[2].y = 1+Y;
		fc[2].z = Ybuffer[(1+Y)][X];
		}
		else {
		fc[0].x = X+0.5;
		fc[0].y = Y;
		fc[0].z = Ybuffer[Y][X];

		fc[1].x = 1+X;
		fc[1].y = Y+1;
		fc[1].z = Ybuffer[(1+Y)][1+X];
	
		fc[2].x = X;
		fc[2].y = 1+Y;
		fc[2].z = Ybuffer[1+Y][X];
		}

	}

	// Placement de la camera en fonction des controles utilisateur


/************* Calcul position de la camera pour suivre le terrain *******************/

// Equation du plan P (la face)
		// On trouve 2 vecteur issus du plan formé par les 3 points
		  	// Vecteur AB
			float xAB = fc[1].x - fc[0].x;
			float yAB = fc[1].y - fc[0].y;
			float zAB = fc[1].z - fc[0].z;

			Vector3D AB = Vector3D(xAB,yAB,zAB);
			// Vecteur AC
			float xAC = fc[2].x - fc[0].x;
			float yAC = fc[2].y - fc[0].y;
			float zAC = fc[2].z - fc[0].z;
			Vector3D AC = Vector3D(xAC,yAC,zAC);

		// On trouve la normal N (Produit vectoriel de AB et AC)
			Vector3D N = AB.crossProduct(AC);

			float Pd =  - (N.X * fc[0].x) - (N.Y *fc[0].y) - (N.Z*fc[0].z) ;
		
// Equation de la droite D (passant par la camera)
			// On récupere 2 points passant par cette droite 
			float xA1 =  camera->_position.X;
			float yA1 =  camera->_position.Y;
			float zA1 =  camera->_position.Z;

			float xA2 = xA1;
			float yA2 = yA1;
			float zA2 = -1;

			// Vecteur directeur U issus de la droite D
			float xU = xA2 - xA1 ;
			float yU = yA2 - yA1 ;
			float zU = zA2 - zA1 ;

			// Calcul de t
		
			float t = ((N.X * xA1) + (N.Y * yA1) + (N.Z * zA1) + Pd ) / ( (N.X * xU) + (N.Y * yU) + (N.Z * zU));
			// Calcul du point M d'intersection de D avec P
			float xM = xA1 + (t * xU);
			float yM = yA1 + (t * yU);
			float zM = zA1 + (-t * zU);
			
		
			camera->_position.Z =zM + K;
			camera->_target = camera->_position + camera->_forward;

	// Dessin de la face courante (rouge)
		
		glBegin(GL_TRIANGLES);
		int j;
		  for (j=0;j<3;j++)
		{
		  glColor3f(1.0,0.0,0.0);
		  glVertex3f(fc[j].x,fc[j].y,fc[j].z);
		}
		glEnd();
		glColor3f(1.0,1.0,1.0);
		
// FIN MARCHEUR
}

void afficheTerrain()
{
	
  for (int i=0;i<NBFACE;i++)
		{

			if (filaire == 0) {
				glEnable(GL_TEXTURE_2D); 		
				if ( pt[ft[i][1]].z < 1.5*hauteurMax/3)
				glBindTexture (GL_TEXTURE_2D, IdTex[9]);
				else if (pt[ft[i][1]].z < 4*hauteurMax/5)
				glBindTexture (GL_TEXTURE_2D, IdTex[10]);
				else
				glBindTexture (GL_TEXTURE_2D, IdTex[11]);

			}
		
		glBegin(GL_TRIANGLES);

		if (filaire==0)
			glTexCoord2f(0.0f, 0.0f);
		else {
			glColor3f(0.,0.,0.);
			glClearColor(1.,1.,1.,0.0);
		}

		  glVertex3f(pt[ft[i][0]].x,pt[ft[i][0]].y,pt[ft[i][0]].z);

		  if (filaire ==0)
			glTexCoord2f(1.0f, 0.0f);
		  glVertex3f(pt[ft[i][1]].x,pt[ft[i][1]].y,pt[ft[i][1]].z);

		  
		  if (filaire ==0) {
			glTexCoord2f(1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
		  }
		  glVertex3f(pt[ft[i][2]].x,pt[ft[i][2]].y,pt[ft[i][2]].z);

		glEnd();

		if (filaire ==0)
			glDisable(GL_TEXTURE_2D);
	}
}


// Affiche les lumiere
void afficheLumiere()
{
glEnable(GL_LIGHTING);  // Active l'éclairage
glEnable(GL_LIGHT0); 	// Allume la lumière n°1

GLfloat px=0,py=1,pz=5,pra=1, pva=1, pba=1, paa=1,prs=1, pvs=1, pbs=1, pas=1,prd=1, pvd=1, pbd=1, pad=1;

GLfloat posamb[4] = { pra, pva, pba, paa };
GLfloat posdif[4] = { prd, pvd, pbd, pad };
GLfloat posspec[4] = { prs, pvs, pbs, pas };

GLfloat pposition[4] = { 50.0, 50.0,30.0,1.0};

glLightfv(GL_LIGHT0, GL_AMBIENT, posamb);
glLightfv(GL_LIGHT0, GL_DIFFUSE, posdif);
glLightfv(GL_LIGHT0, GL_SPECULAR, posspec);
glLightfv(GL_LIGHT0, GL_POSITION, pposition);

 // Dessin de la sphere de lumiere
glPushMatrix(); 
	glTranslatef(pposition[0],pposition[1],pposition[2]); 
	glColor3f(1.,1.,0.);
	glutSolidSphere(0.2,20,20);
glPopMatrix();	
}


void afficheTriedre()
{
// Dessin de la sphere Origine
		glPushMatrix(); 
			glColor3f(1.,1.,1.);
			glutSolidSphere(0.2,20,20);
		glPopMatrix();

// Dessin de la sphere en X
		glPushMatrix(); 
			glTranslatef(1.,0.,0.);  
			glColor3f(1.,0.,0.);
			glutSolidSphere(0.2,20,20);
		glPopMatrix();

// Dessin de la sphere en Y
		glPushMatrix(); 
			glTranslatef(0.,1.,0.);  
			glColor3f(0.,1.,0.);
			glutSolidSphere(0.2,20,20);
		glPopMatrix();

// Dessin de la sphere en Z
		glPushMatrix();
			glTranslatef(0.,0.,1.);  
			glColor3f(0.,0.,1.);
			glutSolidSphere(0.2,20,20);
		glPopMatrix();
			glColor3f(1.,1.,1.);
}


// Affiche un arbre
void afficheArbre(int numTex,float scale, float x, float y)
{
	
float scalex=scale;
float scaley=scale;

if (numTex == 3){ //buisson a fleur
	scalex = scale-3;
	scaley = scale-3;
}else if (numTex == 8){ //tropical large
	scalex = scalex;
}else if (numTex == 1){ //typique haut
	scalex = scalex+1;
	scaley = scaley+1;
}/*else if (numTex == 2){ //palmier "piquant" 
	scalex = scalex-1;
	scaley = scaley+2;
}else if (numTex == 8){ //tropical large
	scalex = scalex;
}else if (numTex == 7){ //petit palmier
	scalex = scalex-3;
	scaley = scaley;
}
*/
long X = x;// Parti entiere de x
long Y = y; // Parti entiere de Y


	glPushMatrix();
	glRotatef(90,1.0,0.0,0.0);
	glTranslated(x,Ybuffer[Y][X]+scaley,-y);

	glEnable(GL_TEXTURE_2D); 
		glBindTexture (GL_TEXTURE_2D, IdTex[numTex]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-scalex, -scaley, 0.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(scalex, -scaley, 0.0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(scalex, scaley, 0.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-scalex, scaley, 0.0f);
	glEnd();

		glPushMatrix();
		glRotatef(90,0.0,1.0,0.0);	
				
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-scalex, -scaley, 0.0f);

				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(scalex, -scaley, 0.0f);

				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(scalex, scaley, 0.0f);

				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-scalex, scaley, 0.0f);
			glEnd();
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);
glPopMatrix();
}

void afficheNuage(float scale, float x, float y){

glPushMatrix();
		glRotatef(90,1.0,0.0,0.0);
		glRotatef(90,1.0,0.0,0.0);
		glTranslated(3*x,-anim+(3*y),-30.0);

		glEnable(GL_TEXTURE_2D); 
			glBindTexture (GL_TEXTURE_2D, IdTex[13]);
			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-scale, -scale, 0.0f);

				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(scale, -scale, 0.0f);

				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(scale, scale, 0.0f);

				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-scale, scale, 0.0f);
		glEnd();
	glPopMatrix();
}



void initArbres()
{
	for(int i=0;i<nbArbres;i++)
	{
	 tarbreX[i] = rand() % l;
	 tarbreY[i]  = rand() % L;
	 tarbreN[i] = rand() % 8  ;
	}
}

void idle()
{	anglex=anglex+2; 
	angley=angley+2;
	
	anim=fmod((anim+0.07),300.0); //anim

	glutPostRedisplay(); 
}



void
GetTextureInfo (tga_header_t *header, gl_texture_t *texinfo)
{
  texinfo->width = header->width;
  texinfo->height = header->height;

  switch (header->image_type)
    {
    case 3:  /* grayscale 8 bits */
    case 11: /* grayscale 8 bits (RLE) */
      {
	if (header->pixel_depth == 8)
	  {
	    texinfo->format = GL_LUMINANCE;
	    texinfo->internalFormat = 1;
	  }
	else /* 16 bits */
	  {
	    texinfo->format = GL_LUMINANCE_ALPHA;
	    texinfo->internalFormat = 2;
	  }

	break;
      }

    case 1:  /* 8 bits color index */
    case 2:  /* BGR 16-24-32 bits */
    case 9:  /* 8 bits color index (RLE) */
    case 10: /* BGR 16-24-32 bits (RLE) */
      {
	/* 8 bits and 16 bits images will be converted to 24 bits */
	if (header->pixel_depth <= 24)
	  {
	    texinfo->format = GL_RGB;
	    texinfo->internalFormat = 3;
	  }
	else /* 32 bits */
	  {
	    texinfo->format = GL_RGBA;
	    texinfo->internalFormat = 4;
	  }

	break;
      }
    }
}


gl_texture_t *
ReadTGAFile (const char *filename)
{
  FILE *fp;
  gl_texture_t *texinfo;
  tga_header_t header;
  GLubyte *colormap = NULL;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return NULL;
    }

  /* read header */
  fread (&header, sizeof (tga_header_t), 1, fp);

  texinfo = (gl_texture_t *)malloc (sizeof (gl_texture_t));
  GetTextureInfo (&header, texinfo);
  fseek (fp, header.id_lenght, SEEK_CUR);

  /* memory allocation */
  texinfo->texels = (GLubyte *)malloc (sizeof (GLubyte) *
	texinfo->width * texinfo->height * texinfo->internalFormat);
  if (!texinfo->texels)
    {
      free (texinfo);
      return NULL;
    }

  /* read color map */
  if (header.colormap_type)
    {
      /* NOTE: color map is stored in BGR format */
      colormap = (GLubyte *)malloc (sizeof (GLubyte)
	* header.cm_length * (header.cm_size >> 3));
      fread (colormap, sizeof (GLubyte), header.cm_length
	* (header.cm_size >> 3), fp);
    }

  /* read image data */
  switch (header.image_type)
    {
    case 0:
      /* no data */
      break;

    case 1:
      /* uncompressed 8 bits color index */
      ReadTGA8bits (fp, colormap, texinfo);
      break;

    case 2:
      /* uncompressed 16-24-32 bits */
      switch (header.pixel_depth)
	{
	case 16:
	  ReadTGA16bits (fp, texinfo);
	  break;

	case 24:
	  ReadTGA24bits (fp, texinfo);
	  break;

	case 32:
	  ReadTGA32bits (fp, texinfo);
	  break;
	}

      break;

    case 3:
      /* uncompressed 8 or 16 bits grayscale */
      if (header.pixel_depth == 8)
	ReadTGAgray8bits (fp, texinfo);
      else /* 16 */
	ReadTGAgray16bits (fp, texinfo);

      break;

    case 9:
      /* RLE compressed 8 bits color index */
      ReadTGA8bitsRLE (fp, colormap, texinfo);
      break;

    case 10:
      /* RLE compressed 16-24-32 bits */
      switch (header.pixel_depth)
	{
	case 16:
	  ReadTGA16bitsRLE (fp, texinfo);
	  break;

	case 24:
	  ReadTGA24bitsRLE (fp, texinfo);
	  break;

	case 32:
	  ReadTGA32bitsRLE (fp, texinfo);
	  break;
	}

      break;

    case 11:
      /* RLE compressed 8 or 16 bits grayscale */
      if (header.pixel_depth == 8)
	ReadTGAgray8bitsRLE (fp, texinfo);
      else /* 16 */
	ReadTGAgray16bitsRLE (fp, texinfo);

      break;

    default:
      /* image type is not correct */
      fprintf (stderr, "error: unknown TGA image type %i!\n", header.image_type);
      free (texinfo->texels);
      free (texinfo);
      texinfo = NULL;
      break;
    }

  /* no longer need colormap data */
  if (colormap)
    free (colormap);

  fclose (fp);
  return texinfo;
}


GLuint
loadTGATexture (const char *filename,int iden)
{
  gl_texture_t *tga_tex = NULL;
  GLuint tex_id = 0;

  tga_tex = ReadTGAFile (filename);

  if (tga_tex && tga_tex->texels)
    {
      /* generate texture */
      glBindTexture (GL_TEXTURE_2D, iden);
	  
      /* setup some parameters for texture filters and mipmapping */
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR/*_MIPMAP_LINEAR*/);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP);

      glTexImage2D (GL_TEXTURE_2D, 0, tga_tex->internalFormat,
		    tga_tex->width, tga_tex->height, 0, tga_tex->format,
		    GL_UNSIGNED_BYTE, tga_tex->texels);


      tex_id = tga_tex->id;

      /* OpenGL has its own copy of texture data */
      free (tga_tex->texels);
      free (tga_tex);
    }

  return tex_id;
}


void
ReadTGA8bits (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
{
  int i;
  GLubyte color;

  for (i = 0; i < texinfo->width * texinfo->height; ++i)
    {
      /* read index color byte */
      color = (GLubyte)fgetc (fp);

      /* convert to RGB 24 bits */
      texinfo->texels[(i * 3) + 2] = colormap[(color * 3) + 0];
      texinfo->texels[(i * 3) + 1] = colormap[(color * 3) + 1];
      texinfo->texels[(i * 3) + 0] = colormap[(color * 3) + 2];
    }
}


void
ReadTGA16bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;
  unsigned short color;

  for (i = 0; i < texinfo->width * texinfo->height; ++i)
    {
      /* read color word */
      color = fgetc (fp) + (fgetc (fp) << 8);

      /* convert BGR to RGB */
      texinfo->texels[(i * 3) + 0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
      texinfo->texels[(i * 3) + 1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
      texinfo->texels[(i * 3) + 2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
    }
}


void
ReadTGA24bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;

  for (i = 0; i < texinfo->width * texinfo->height; ++i)
    {
      /* read and convert BGR to RGB */
      texinfo->texels[(i * 3) + 2] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 3) + 1] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 3) + 0] = (GLubyte)fgetc (fp);
    }
}


void
ReadTGA32bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;

  for (i = 0; i < texinfo->width * texinfo->height; ++i)
    {
      /* read and convert BGRA to RGBA */
      texinfo->texels[(i * 4) + 2] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 4) + 1] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 4) + 0] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 4) + 3] = (GLubyte)fgetc (fp);
    }
}


void
ReadTGAgray8bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;

  for (i = 0; i < texinfo->width * texinfo->height; ++i)
    {
      /* read grayscale color byte */
      texinfo->texels[i] = (GLubyte)fgetc (fp);
    }
}


void
ReadTGAgray16bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;

  for (i = 0; i < texinfo->width * texinfo->height; ++i)
    {
      /* read grayscale color + alpha channel bytes */
      texinfo->texels[(i * 2) + 0] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 2) + 1] = (GLubyte)fgetc (fp);
    }
}


void
ReadTGA8bitsRLE (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte color;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  /* run-length packet */
	  color = (GLubyte)fgetc (fp);

	  for (i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[0] = colormap[(color * 3) + 2];
	      ptr[1] = colormap[(color * 3) + 1];
	      ptr[2] = colormap[(color * 3) + 0];
	    }
	}
      else
	{
	  /* non run-length packet */
	  for (i = 0; i < size; ++i, ptr += 3)
	    {
	      color = (GLubyte)fgetc (fp);

	      ptr[0] = colormap[(color * 3) + 2];
	      ptr[1] = colormap[(color * 3) + 1];
	      ptr[2] = colormap[(color * 3) + 0];
	    }
	}
    }
}


void
ReadTGA16bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  unsigned short color;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
    {
      /* read first byte */
      packet_header = fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  /* run-length packet */
	  color = fgetc (fp) + (fgetc (fp) << 8);

	  for (i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
	      ptr[1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
	      ptr[2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
	    }
	}
      else
	{
	  /* non run-length packet */
	  for (i = 0; i < size; ++i, ptr += 3)
	    {
	      color = fgetc (fp) + (fgetc (fp) << 8);

	      ptr[0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
	      ptr[1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
	      ptr[2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
	    }
	}
    }
}


void
ReadTGA24bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte rgb[3];
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  /* run-length packet */
	  fread (rgb, sizeof (GLubyte), 3, fp);

	  for (i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[0] = rgb[2];
	      ptr[1] = rgb[1];
	      ptr[2] = rgb[0];
	    }
	}
      else
	{
	  /* non run-length packet */
	  for (i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[2] = (GLubyte)fgetc (fp);
	      ptr[1] = (GLubyte)fgetc (fp);
	      ptr[0] = (GLubyte)fgetc (fp);
	    }
	}
    }
}


void
ReadTGA32bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte rgba[4];
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 4)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  /* run-length packet */
	  fread (rgba, sizeof (GLubyte), 4, fp);

	  for (i = 0; i < size; ++i, ptr += 4)
	    {
	      ptr[0] = rgba[2];
	      ptr[1] = rgba[1];
	      ptr[2] = rgba[0];
	      ptr[3] = rgba[3];
	    }
	}
      else
	{
	  /* non run-length packet */
	  for (i = 0; i < size; ++i, ptr += 4)
	    {
	      ptr[2] = (GLubyte)fgetc (fp);
	      ptr[1] = (GLubyte)fgetc (fp);
	      ptr[0] = (GLubyte)fgetc (fp);
	      ptr[3] = (GLubyte)fgetc (fp);
	    }
	}
    }
}


void
ReadTGAgray8bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte color;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height))
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  /* run-length packet */
	  color = (GLubyte)fgetc (fp);

	  for (i = 0; i < size; ++i, ptr++)
	    *ptr = color;
	}
      else
	{
	  /* non run-length packet */
	  for (i = 0; i < size; ++i, ptr++)
	    *ptr = (GLubyte)fgetc (fp);
	}
    }
}


void
ReadTGAgray16bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte color, alpha;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 2)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  /* run-length packet */
	  color = (GLubyte)fgetc (fp);
	  alpha = (GLubyte)fgetc (fp);

	  for (i = 0; i < size; ++i, ptr += 2)
	    {
	      ptr[0] = color;
	      ptr[1] = alpha;
	    }
	}
      else
	{
	  /* non run-length packet */
	  for (i = 0; i < size; ++i, ptr += 2)
	    {
	      ptr[0] = (GLubyte)fgetc (fp);
	      ptr[1] = (GLubyte)fgetc (fp);
	    }
	}
    }
}
