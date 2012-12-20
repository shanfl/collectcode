#ifndef ENTITY
#include "entity.h"
#endif

struct point
{
  int   x;
  int   y;
};

class CPointer : public CEntity
{

private:
  GLvector                m_position;
  point                   m_last_mouse;
  point                   m_last_cell;
  int                     m_texture;
  float                   m_pulse;

public:
                          CPointer ();

  void                    RenderFade (void);
  void                    Render (void);
  void                    Update (void);
  point                   Selected (void);
};