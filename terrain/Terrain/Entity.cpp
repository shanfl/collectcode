/*-----------------------------------------------------------------------------

  Entity.cpp

  Copyright (c) 2005 Shamus Young
  All Rights Reserved

-------------------------------------------------------------------------------

  An entity is any renderable object in the world.  This is an abstract class.
  

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <math.h>
#include <gl\gl.h>

#include "camera.h"
#include "entity.h"
#include "map.h"
#include "sky.h"
#include "terrain.h"

static class CEntity*       head;

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityUpdate (void)
{

  CEntity*        e;

  for (e = head; e; e = e->Next ()) 
    e->Update ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityInit (void)
{



}


/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CEntity* EntityFindType (char* type, CEntity* start)
{

  CEntity*    e;

  if (!start)
    start = head;
  for (e = start; e; e = e->Next ()) {
    if (!stricmp (type, e->Type ()))
      return e;
  }
  return NULL;

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityTerm (void)
{

  CEntity*    e;
  CEntity*    next;

  e = head;
  while (e) {
    next = e->Next ();
    delete e;
    e = next;
  }

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityRender (void)
{

  class CEntity*  e;

  for (e = head; e; e = e->Next ())
    e->Render ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityRenderFadeIn (void)
{

  class CEntity*  e;

  for (e = head; e; e = e->Next ())
    e->RenderFadeIn ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

void EntityFadeStart (void)
{

  class CEntity*  e;

  for (e = head; e; e = e->Next ())
    e->FadeStart ();

}

/*-----------------------------------------------------------------------------

-----------------------------------------------------------------------------*/

CEntity::CEntity (void)
{

  next = head;
  head = this;
  m_entity_type = "none";

}

class CEntity* CEntity::Next (void)
{

  return next;

}

void CEntity::Render (void)
{

}

void CEntity::RenderFadeIn (void)
{

  //by default, perfom a normal render for the LOD fade-in
  Render ();

}

void CEntity::Update (void)
{


}

void CEntity::FadeStart ()
{

}


char* CEntity::Type ()
{

  return m_entity_type;

}