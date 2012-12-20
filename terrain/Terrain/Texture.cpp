/*-----------------------------------------------------------------------------

  Texture.cpp

  2006 Shamus Young

-------------------------------------------------------------------------------
  
  This loads in textures.  Nothin' fancy.
  
-----------------------------------------------------------------------------*/

#define MAX_STRING          128

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <gl\glext.h>

#include "console.h"

struct texture
{
  struct texture*   next;
  GLuint            id;
  char              name[16];
  char*             image_name;
  char*             mask_name;
	AUX_RGBImageRec*  image;

};

static struct texture*      head_texture;

/*-----------------------------------------------------------------------------
                          
-----------------------------------------------------------------------------*/

static AUX_RGBImageRec* LoadBMP (char *Filename)				
{

  FILE*           File=NULL;
  
  if (!Filename) 
		return NULL;
  File=fopen(Filename,"r");					
  if (File)	{
		fclose(File);
		return auxDIBImageLoad(Filename);			
	}
	return NULL;							

}

/*-----------------------------------------------------------------------------
                           t e x t u r e   i d
-----------------------------------------------------------------------------*/

static struct texture* LoadTexture (char* name)
{

  char              filename[MAX_STRING];
  struct texture*   t;

  t = new struct texture;
  strcpy (t -> name, name);
  sprintf (filename, "textures/%s.bmp", name);
  t->image = LoadBMP (filename);
  if (!t->image) {
    t->id = 0;
    return t;
  }
  glGenTextures (1, &t->id);// Create The Texture
	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture (GL_TEXTURE_2D, t -> id);
  // Generate The Texture
	glTexImage2D (GL_TEXTURE_2D, 0, 3, 
    t -> image->sizeX, 
    t -> image->sizeY, 
    0, GL_RGB, GL_UNSIGNED_BYTE, 
    t -> image->data);
  free (t -> image);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
  t -> next = head_texture;
  head_texture = t;
  return t;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

unsigned TextureFromName (char* name)
{

  struct texture*       t;

  for (t = head_texture; t; t = t -> next) {
    if (!stricmp (name, t -> name))
      return t -> id;
  }
  t = LoadTexture (name);
  if (t)
    return t->id;
  return 0;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void TextureInit (void)
{


}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void TextureTerm (void)
{

  struct texture*       t;

  while (head_texture) {
    t = head_texture->next;
    free (head_texture);
    head_texture = t;
  }


}
