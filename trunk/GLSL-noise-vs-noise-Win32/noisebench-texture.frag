#version 120

uniform sampler2D permTexture;
varying vec2 v_texCoord2D;

void main( void )
{
  gl_FragColor = texture2D(permTexture, v_texCoord2D);
}
