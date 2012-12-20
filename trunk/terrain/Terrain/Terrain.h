#ifndef ENTITY
#include "entity.h"
#endif

enum build_stage
{
  STAGE_IDLE,         //check for the need to begin building
  STAGE_CLEAR,        //reset the mesh
  STAGE_QUADTREE,     //run the quad tree and figure out what points will be in final mesh
  STAGE_TEXTURES,
  STAGE_COMPILE,      //build the final glList
  STAGE_WAIT_FOR_FADE,
  STAGE_DONE,         
};

class CTerrain : public CEntity
{
private:
  GLvector                m_viewpoint;
  GLvector2*              m_zone_uv;  
  char                    m_stage;
  int                     m_x;
  int                     m_y;
  int                     m_map_size;
  int                     m_map_half;
  int                     m_layer;
  int                     m_zone;
  int                     m_zone_origin_x;
  int                     m_zone_origin_y;
  int                     m_zone_size;
  short*                  m_boundry;
  bool                    m_compile_back;
  bool                    m_use_color;
  bool                    m_fade;
  bool*                   m_point;
  unsigned int            m_list_back;
  unsigned int            m_list_front;
  long                    m_triangles;
  long                    m_vertices;
  long                    m_build_start;
  long                    m_build_time;
  long                    m_compile_time;
  float                   m_tolerance;

  void                    Compile (void);
  void                    CompileBlock (int x, int y, int size);
  void                    CompileTriangle (int x1, int y1, int x2, int y2, int x3, int y3);
  void                    CompileVertex (int x, int y);
  void                    CompileStrip (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
  void                    CompileFan (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int x5, int y5);
  void                    CompileFan (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, int x5, int y5, int x6, int y6);
  void                    GridStep (void);
  void                    DoQuad (int x, int y, int size);
  void                    PointActivate (int x, int y);

public:

                          CTerrain (int size);
                          ~CTerrain ();
  void                    Render (void);
  void                    RenderFadeIn (void);
  void                    Update (void);
  void                    FadeStart (void);

};
