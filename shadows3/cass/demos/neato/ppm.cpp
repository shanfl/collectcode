#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#include "ppm.h"

void read_ppm(const char *file)
{
  FILE *fp = fopen(file,"rb");
  char buff[128];

  fgets(buff, 128, fp);
  while(buff[0] == '#') fgets(buff, 128, fp);
  if(strncmp(buff, "P6", 2))
    { fclose(fp); return; }

  fgets(buff, 128, fp);
  while(buff[0] == '#') fgets(buff, 128, fp);
        
  int w, h;
  sscanf(buff, "%d %d", &w, &h);
        
  fgets(buff, 128, fp);
  while(buff[0] == '#') fgets(buff, 128, fp);

  unsigned char * img = new unsigned char [ w * h * 3];
  fread(img, 1, w * h * 3, fp);


  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, img);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  delete [] img;


}
