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
uniform sampler2D gradTexture;
varying vec4 v_texCoord4D;

/*
 * To create offsets of one texel and one half texel in the
 * texture lookup, we need to know the texture image size.
 */
#define ONE 0.00390625
#define ONEHALF 0.001953125
// The numbers above are 1/256 and 0.5/256, change accordingly
// if you change the code to use another texture size.

/*
 * The interpolation function. This could be a 1D texture lookup
 * to get some more speed, but it's not the main part of the algorithm.
 */
float fade(float t) {
  // return t*t*(3.0-2.0*t); // Old fade, yields discontinuous second derivative
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}

/*
 * 4D classic noise. Slow, but very useful. 4D simplex noise is a lot faster.
 *
 * This function performs 8 texture lookups and 16 dependent texture lookups,
 * 16 dot products, 4 mix operations and a lot of additions and multiplications.
 * Needless to say, it's not super fast. But it's not dead slow either.
 */
float noise(vec4 P)
{
  vec4 Pi = ONE*floor(P)+ONEHALF; // Integer part, scaled so +1 moves one texel
                                  // and offset 1/2 texel to sample texel centers
  vec4 Pf = fract(P);      // Fractional part for interpolation

  // "n0000" is the noise contribution from (x=0, y=0, z=0, w=0), and so on
  float perm00xy = texture2D(permTexture, Pi.xy).a ;
  float perm00zw = texture2D(permTexture, Pi.zw).a ;
  vec4 grad0000 = texture2D(gradTexture, vec2(perm00xy, perm00zw)).rgba * 4.0 -1.0;
  float n0000 = dot(grad0000, Pf);

  float perm01zw = texture2D(permTexture, Pi.zw  + vec2(0.0, ONE)).a ;
  vec4  grad0001 = texture2D(gradTexture, vec2(perm00xy, perm01zw)).rgba * 4.0 - 1.0;
  float n0001 = dot(grad0001, Pf - vec4(0.0, 0.0, 0.0, 1.0));

  float perm10zw = texture2D(permTexture, Pi.zw  + vec2(ONE, 0.0)).a ;
  vec4  grad0010 = texture2D(gradTexture, vec2(perm00xy, perm10zw)).rgba * 4.0 - 1.0;
  float n0010 = dot(grad0010, Pf - vec4(0.0, 0.0, 1.0, 0.0));

  float perm11zw = texture2D(permTexture, Pi.zw  + vec2(ONE, ONE)).a ;
  vec4  grad0011 = texture2D(gradTexture, vec2(perm00xy, perm11zw)).rgba * 4.0 - 1.0;
  float n0011 = dot(grad0011, Pf - vec4(0.0, 0.0, 1.0, 1.0));

  float perm01xy = texture2D(permTexture, Pi.xy + vec2(0.0, ONE)).a ;
  vec4  grad0100 = texture2D(gradTexture, vec2(perm01xy, perm00zw)).rgba * 4.0 - 1.0;
  float n0100 = dot(grad0100, Pf - vec4(0.0, 1.0, 0.0, 0.0));

  vec4  grad0101 = texture2D(gradTexture, vec2(perm01xy, perm01zw)).rgba * 4.0 - 1.0;
  float n0101 = dot(grad0101, Pf - vec4(0.0, 1.0, 0.0, 1.0));

  vec4  grad0110 = texture2D(gradTexture, vec2(perm01xy, perm10zw)).rgba * 4.0 - 1.0;
  float n0110 = dot(grad0110, Pf - vec4(0.0, 1.0, 1.0, 0.0));

  vec4  grad0111 = texture2D(gradTexture, vec2(perm01xy, perm11zw)).rgba * 4.0 - 1.0;
  float n0111 = dot(grad0111, Pf - vec4(0.0, 1.0, 1.0, 1.0));

  float perm10xy = texture2D(permTexture, Pi.xy + vec2(ONE, 0.0)).a ;
  vec4  grad1000 = texture2D(gradTexture, vec2(perm10xy, perm00zw)).rgba * 4.0 - 1.0;
  float n1000 = dot(grad1000, Pf - vec4(1.0, 0.0, 0.0, 0.0));

  vec4  grad1001 = texture2D(gradTexture, vec2(perm10xy, perm01zw)).rgba * 4.0 - 1.0;
  float n1001 = dot(grad1001, Pf - vec4(1.0, 0.0, 0.0, 1.0));

  vec4  grad1010 = texture2D(gradTexture, vec2(perm10xy, perm10zw)).rgba * 4.0 - 1.0;
  float n1010 = dot(grad1010, Pf - vec4(1.0, 0.0, 1.0, 0.0));

  vec4  grad1011 = texture2D(gradTexture, vec2(perm10xy, perm11zw)).rgba * 4.0 - 1.0;
  float n1011 = dot(grad1011, Pf - vec4(1.0, 0.0, 1.0, 1.0));

  float perm11xy = texture2D(permTexture, Pi.xy + vec2(ONE, ONE)).a ;
  vec4  grad1100 = texture2D(gradTexture, vec2(perm11xy, perm00zw)).rgba * 4.0 - 1.0;
  float n1100 = dot(grad1100, Pf - vec4(1.0, 1.0, 0.0, 0.0));

  vec4  grad1101 = texture2D(gradTexture, vec2(perm11xy, perm01zw)).rgba * 4.0 - 1.0;
  float n1101 = dot(grad1101, Pf - vec4(1.0, 1.0, 0.0, 1.0));

  vec4  grad1110 = texture2D(gradTexture, vec2(perm11xy, perm10zw)).rgba * 4.0 - 1.0;
  float n1110 = dot(grad1110, Pf - vec4(1.0, 1.0, 1.0, 0.0));

  vec4  grad1111 = texture2D(gradTexture, vec2(perm11xy, perm11zw)).rgba * 4.0 - 1.0;
  float n1111 = dot(grad1111, Pf - vec4(1.0, 1.0, 1.0, 1.0));

  // Blend contributions along x
  float fadex = fade(Pf.x);
  vec4 n_x0 = mix(vec4(n0000, n0001, n0010, n0011),
                  vec4(n1000, n1001, n1010, n1011), fadex);
  vec4 n_x1 = mix(vec4(n0100, n0101, n0110, n0111),
                  vec4(n1100, n1101, n1110, n1111), fadex);

  // Blend contributions along y
  vec4 n_xy = mix(n_x0, n_x1, fade(Pf.y));

  // Blend contributions along z
  vec2 n_xyz = mix(n_xy.xy, n_xy.zw, fade(Pf.z));

  // Blend contributions along w
  float n_xyzw = mix(n_xyz.x, n_xyz.y, fade(Pf.w));

  // We're done, return the final noise value.
  return n_xyzw;
}


void main( void )
{
  float n = noise(v_texCoord4D);

  gl_FragColor = vec4(0.5 + 0.5 * vec3(n, n, n), 1.0);
}
