/*
 * Perlin noise, classic and simplex, in a GLSL fragment shader.
 * This version makes heavy use of a texture for table lookups.
 *
 * Author: Stefan Gustavson ITN-LiTH (stegu@itn.liu.se) 2004-12-05
 * Simplex indexing functions by Bill Licea-Kane, ATI (bill@ati.com)
 *
 * You may use, modify and redistribute this code free of charge,
 * provided that the author's names and this notice appear intact.
 */

#version 120

uniform sampler2D permTexture;
varying vec2 v_texCoord2D;

/*
 * To create offsets of one texel and one half texel in the
 * texture lookup, we need to know the texture image size.
 */
#define ONE 0.00390625
#define ONEHALF 0.001953125
// The numbers above are 1/256 and 0.5/256, change accordingly
// if you change the code to use another texture size.

/*
 * 2D simplex noise
 */
float snoise(vec2 P) {

// Skew and unskew factors for 2D
// This is (sqrt(3.0)-1.0)/2.0
#define F2 0.366025403784
// This is (3.0-sqrt(3.0))/6.0
#define G2 0.211324865405
// This is 1.0-2.0*G2
#define H2 0.577350269190

  // Skew the (x,y) space to determine which cell of 2 simplices we're in
  float s = (P.x + P.y) * F2;   // Hairy factor for 2D skewing
  vec2 Pi = floor(P + s);
  float t = (Pi.x + Pi.y) * G2; // Hairy factor for unskewing
  vec2 P0 = Pi - t; // Unskew the cell origin back to (x,y) space
  Pi = Pi * ONE + ONEHALF; // Integer part, scaled and offset for texture lookup

  vec2 Pf0 = P - P0;  // The x,y distances from the cell origin
  // For the 2D case, the simplex shape is an equilateral triangle.
  // Find out whether we are above or below the x=y diagonal to
  // determine which of the two triangles we're in.
  vec2 o1 = (Pf0.x > Pf0.y) ? vec2(1.0, 0.0): vec2(0.0, 1.0);

  // Gradient at simplex origin
  vec2 grad0 = texture2D(permTexture, Pi).rg * 4.0 - 1.0;
  // Gradient at middle corner
  vec2 grad1 = texture2D(permTexture, Pi + o1*ONE).rg * 4.0 - 1.0;
  // Gradient at last corner
  vec2 grad2 = texture2D(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  
  vec2 Pf1 = Pf0 - o1 + G2;
  vec2 Pf2 = Pf0 - vec2(H2);

  // Perform all three blend kernel computations in parallel on vec3 data
  vec3 n012 = 0.5 - vec3(dot(Pf0, Pf0), dot(Pf1, Pf1), dot(Pf2, Pf2));
  n012 = max(n012, 0.0);
  n012 *= n012;
  n012 *= n012 * vec3(dot(grad0, Pf0), dot(grad1, Pf1), dot(grad2, Pf2));
  
  // Sum up and scale the result to cover the range [-1,1]
  return 70.0 * (n012.x + n012.y + n012.z);
}

void main( void )
{
  float n = snoise(v_texCoord2D);

  gl_FragColor = vec4(0.5 + 0.5 * vec3(n, n, n), 1.0);
}
