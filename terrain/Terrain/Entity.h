#ifndef TYPES
#include "glTypes.h"
#endif

#ifndef ENTITY

#define ENTITY

class CEntity
{
private:
  CEntity*                next;
protected:

  char*                   m_entity_type;

public:
                          CEntity (void);
  class CEntity*          Next (void);
  virtual void            Render (void);
  virtual void            RenderFadeIn (void);
  virtual void            FadeStart (void);
  virtual void            Update (void);
  virtual char*           Type (void);

};

void      EntityUpdate (void);
void      EntityInit (void);
CEntity*  EntityFindType (char* type, CEntity* start);
void      EntityTerm (void);
void      EntityRender (void);
void      EntityRenderFadeIn (void);
void      EntityFadeStart (void);

#endif