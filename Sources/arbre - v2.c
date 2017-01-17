/********************************************************/
/*                     cube.c                           */
/********************************************************/
/* Affiche a l'ecran un cube en 3D                      */
/********************************************************/

// inclusion des fichiers d'en-tete Glut

//#include <glut.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <GL/glut.h>


/**********************************
****        TEXTURES        *******
**********************************/

//k : toute les texture ds se tableau
int IdTex[2]; /* tableau d'Id pour les 2 textures */

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


/* texture id for exemple 
GLuint texId = 0;
*/

//////////////////////////////////////////////////////////////////// fin TEXTURE STRUCTURE
// Notre structure point 
typedef struct
{
  float x;
  float y;
  float z;
} point;

// Contient la position et la couleur des 4 points des faces
point p[8]={
    {-0.5,-0.5, 0},
    { 0.5,-0.5, 0},
    { 0.5, 0.5, 0},
    {-0.5, 0.5, 0},
    { 0,-0.5, -0.5},
    { 0,-0.5, 0.5},
    { 0, 0.5, 0.5},
    { 0, 0.5, -0.5}
    };

// Decrit comment sont relier les 1 plaque de 4 points 
int f[2][4]={
  {0,1,2,3},
  {4,5,6,7}
	};

char presse;
int anglex,angley,x,y,xold,yold;

// Prototype des fonctions 
void affichage();
void clavier(unsigned char touche,int x,int y);
void reshape(int x,int y);
void idle();
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);

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

//k
//GLuint Nom;

/* texture id for exemple */
GLuint texId = 0;
GLuint texId2 = 0;
GLuint texId3 = 0;
char const *arbre3="arbre1.tga";
char const *arbre2="beau_palmier2.tga";
char const *arbre1="tropicarbre.tga";

int main(int argc,char **argv)
{
  // initialisation de glut et creation de la fenetre
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(500,500);
  glutCreateWindow("cube");

  // Initialisation d'OpenGL 
  glClearColor(0.5,0.5,0.5,0.5);
  glColor3f(1.0,1.0,1.0);
  glPointSize(2.0);


  // On charge la texture TGA
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER,0.5);
  glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

  //Chargement de tte les textures dans IdTex
  glGenTextures (2, IdTex);		//deux texture, IdTex tableau de 2 entier
  if (!(texId = loadTGATexture (arbre1,IdTex[0]))){ exit (-1); }
if (!(texId2 = loadTGATexture (arbre2,IdTex[1]))){ exit (-1); }
//if (!(texId3 = loadTGATexture (arbre3))){ exit (-1); }


  glEnable(GL_DEPTH_TEST);				//Active le depth test
  glEnable(GL_TEXTURE_2D); 				//Active le texturing

  // enregistrement des fonctions de rappel 
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);

  // Entree dans la boucle principale glut 
  glutMainLoop();
  return 0;
}


void affichage()
{
  int i,j;
  // effacement de l'image avec la couleur de fond
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glLoadIdentity();
  glRotatef(-angley,1.0,0.0,0.0);
  glRotatef(-anglex,0.0,1.0,0.0);
  
  //gluLookAt(0.5,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0);

  //Dessin des deux plaques a la position demandée
  //glTranslatef (0.2, 0.2, 0.5);

  	  
  glBindTexture (GL_TEXTURE_2D, IdTex[0]);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 0.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 0.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 0.0f);
  glEnd();
  
  
  glBindTexture (GL_TEXTURE_2D, IdTex[1]);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(0.0f, -1.0f, -1.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.0f, -1.0f, 1.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, 1.0f);

    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0.0f, 1.0f, -1.0f);
  glEnd();
  //glDisable(GL_TEXTURE_2D);

  //glDisable();  ==> glDisable (GL_TEXTURE_2D);
  // On echange les buffers
  glutSwapBuffers();

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
    case 'd':
      glEnable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'D':
      glDisable(GL_DEPTH_TEST);
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
}

void mouse(int button, int state,int x,int y)
{
  // si on appuie sur le bouton gauche 
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
  {
    presse = 1; // le booleen presse passe a 1 (vra
    xold = x; // on sauvegarde la position de la souris
    yold=y;
  }
  // si on relache le bouton gauche
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) 
    presse=0; // le booleen presse passe a 0 (faux) 
}

void mousemotion(int x,int y)
  {
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold); 
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }
    
    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
  }



/***************************************
**************     TEXTURES ************
****************************************/

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




/////////////////////////////////////////////////////////////////////////////Choix lecture image


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
      //glGenTextures (1, &tga_tex->id);
      //glGenTextures (2, IdTex);
      glBindTexture (GL_TEXTURE_2D, iden);
	  
	  /*
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_ALPHA_TEST);
		*/
      /* setup some parameters for texture filters and mipmapping */
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR/*_MIPMAP_LINEAR*/);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D (GL_TEXTURE_2D, 0, tga_tex->internalFormat,
		    tga_tex->width, tga_tex->height, 0, tga_tex->format,
		    GL_UNSIGNED_BYTE, tga_tex->texels);

      /*
      gluBuild2DMipmaps (GL_TEXTURE_2D, tga_tex->internalFormat,
			 tga_tex->width, tga_tex->height,
			 tga_tex->format, GL_UNSIGNED_BYTE, tga_tex->texels);
      */

      tex_id = tga_tex->id;

      /* OpenGL has its own copy of texture data */
      free (tga_tex->texels);
      free (tga_tex);
    }

  return tex_id;
}




///////////////////////////////////////////////////////////////////////// LECTURE DE L'iMAGE


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