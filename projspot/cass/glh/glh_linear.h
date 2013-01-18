/*
glh_linear.h
*/

// Author:  Cass W. Everitt

#ifndef GLH_LINEAR_H
#define GLH_LINEAR_H

#ifndef TEMPLATE_FUNCTION
# ifdef WIN32
#  define TEMPLATE_FUNCTION
# else
#  define TEMPLATE_FUNCTION <>
# endif
#endif

#include <math.h>

namespace glh
{
	template <class F> class rotation;
	template <class F> class line;
	template <class F> class plane;
	template <class F> class matrix4;
	
	
	template <int N, class F> 
		class vec_base
	{				
    public:
		typedef F element_type;
		int size() const { return N; }
		
		vec_base(const F & t = F()) 
		{ for(int i = 0; i < N; i++) v[i] = t; }
		vec_base(const F * tp)
		{ for(int i = 0; i < N; i++) v[i] = tp[i]; }
	  
		
		const vec_base<N,F> & operator = (const vec_base<N,F> & rhs)
		{ for(int i = 0; i < N; i++) v[i] = rhs.v[i]; return *this; }
		
		
		const F * get_value() const
		{ return v; }
		
		
		F dot( const vec_base<N,F> & rhs ) const
		{ 
			F r = 0;
			for(int i = 0; i < N; i++) r += v[i]*rhs[i];
			return r;
		}
		
		
		bool equals( const vec_base<N,F> & rhs, F tolerance ) const
		{
			for(int i = 0; i < N; i++)
				if(v[i] < rhs[i] && rhs[i] - v[i] > tolerance)
					return false;
				else if (v[i] - rhs[i] > tolerance)
					return false;
				return true;
		}
		
		F length() const
		{
			F r = 0;
			for(int i = 0; i < N; i++) r += v[i]*v[i]; 
			return sqrt(r);
		}	
		
		
		void  negate()
		{ for(int i = 0; i < N; i++) v[i] = -v[i]; }
	  
		
		F normalize() 
		{ 
			F sum = 0.0;
			for(int i = 0; i < N; i++) sum += v[i]*v[i];
			sum = sqrt(sum);
			{for(int i = 0; i < N; i++) v[i] /= sum;}
			return sum;
		}
		
		
		vec_base<N,F> & set_value( const F * rhs )
		{ for(int i = 0; i < N; i++) v[i] = rhs[i]; return *this; }
		
		F & operator [] ( int i )
		{ return v[i]; }
		
		const F & operator [] ( int i ) const
		{ return v[i]; }
		
		vec_base<N,F> & operator *= ( F d )
		{ for(int i = 0; i < N; i++) v[i] *= d; return *this;}
		
		vec_base<N,F> & operator /= ( F d )
		{ for(int i = 0; i < N; i++) v[i] /= d; return *this;}
		
		vec_base<N,F> & operator += ( const vec_base<N,F> & u )
		{ for(int i = 0; i < N; i++) v[i] += u[i]; return *this;}
		
		vec_base<N,F> & operator -= ( const vec_base<N,F> & u )
		{ for(int i = 0; i < N; i++) v[i] -= u[i]; return *this;}
		

		vec_base<N,F> operator - () const
		{ vec_base<N,F> rv = v; rv.negate(); return rv; }

		vec_base<N,F> operator + ( const vec_base<N,F> &v) const
		{ vec_base<N,F> rt(*this); return rt += v; }

		vec_base<N,F> operator - ( const vec_base<N,F> &v) const
		{ vec_base<N,F> rt(*this); return rt -= v; }

		vec_base<N,F> operator * ( F d) const
		{ vec_base<N,F> rt(*this); return rt *= d; }

		friend bool operator == TEMPLATE_FUNCTION ( const vec_base<N,F> &v1, const vec_base<N,F> &v2 );
		friend bool operator != TEMPLATE_FUNCTION ( const vec_base<N,F> &v1, const vec_base<N,F> &v2 );
		
	  
	protected:
		F v[N];
	};
	
	template <int N, class F> class vec : public vec_base<N,F>
	{
    public:
		vec(const F & t = F()) : vec_base<N,F>(t)
		{}
		vec(const vec_base<N,F> & t) : vec_base<N,F>(t)
		{}
		vec(const F * tp) : vec_base<N,F>(tp)
		{}
	  
	};
	
  
  // vector friend operators
  
	template <int N, class F> inline
		vec_base<N,F> operator * ( const vec_base<N,F> & b, F d )
	{
		vec_base<N,F> rt(b);
		return rt *= d;
	}
	
	template <int N, class F> inline
		vec_base<N,F> operator * ( F d, const vec_base<N,F> & b )
	{ return b*d; }
	
	template <int N, class F> inline
		vec_base<N,F> operator / ( const vec_base<N,F> & b, F d )
	{
		vec_base<N,F> rt(b);
		return rt /= d;
	}
	
	template <int N, class F> inline
		vec_base<N,F> operator + ( const vec_base<N,F> & v1, const vec_base<N,F> & v2 )
	{
		vec_base<N,F> rt(v1);
		return rt += v2;
	}
	
	template <int N, class F> inline
		vec_base<N,F> operator - ( const vec_base<N,F> & v1, const vec_base<N,F> & v2 )
	{
		vec_base<N,F> rt(v1);
		return rt -= v2;
	}
	

	template <int N, class F> inline
		bool operator == ( const vec_base<N,F> & v1, const vec_base<N,F> & v2 )
	{
		for(int i = 0; i < N; i++)
			if(v1[i] != v2[i])
				return false;
			return true;
	}
	
	template <int N, class F> inline
		bool operator != ( const vec_base<N,F> & v1, const vec_base<N,F> & v2 )
	{ return !(v1 == v2); }
	
  
  // partial specialization for vec<2,F>
  
#ifndef WIN32
	template <class F> class vec<2,F> : public vec_base<2,F>
	{
    public:
		vec(const F & t = F()) : vec_base<2,F>(t)
		{}
		vec(const vec_base<2,F> & t) : vec_base<2,F>(t)
		{}
		vec(const F * tp) : vec_base<2,F>(t)
		{}
		
		vec(F x, F y )
		{ v[0] = x; v[1] = y; }
		
		void get_value(F & x, F & y) const
		{ x = v[0]; y = v[1]; }
		
		vec<2,F> & set_value( const F & x, const F & y)
		{ v[0] = x; v[1] = y; return *this; }
		
	};
	
	typedef vec<2,float> vec2f;
	typedef vec<2,short> vec2s;
	typedef vec<2,double> vec2d;
	
#else
	template <class F> class vec2 : public vec<2,F>
	{
		
    public:
		vec2(const F & t = F()) : vec<2,F>(t)
		{}
		vec2(const vec<2,F> & t) : vec<2,F>(t)
		{}
		vec2(const F * tp) : vec<2,F>(tp)
		{}
		
		vec2(F x, F y )
		{ v[0] = x; v[1] = y; }
		
		void get_value(F & x, F & y) const
		{ x = v[0]; y = v[1]; }
		
		vec2<F> & set_value( const F & x, const F & y)
		{ v[0] = x; v[1] = y; return *this; }
		
	};
	
	typedef vec2<float> vec2f;
	typedef vec2<short> vec2s;
	typedef vec2<double> vec2d;
#endif
	
	
	
	// partial specialization for vec<3,F>
	
#ifndef WIN32
	template <class F> class vec<3,F> : public vec_base<3,F>
	{
    public:
		vec(const F & t = F()) : vec_base<3,F>(t)
		{}
		vec(const vec_base<3,F> & t) : vec_base<3,F>(t)
		{}
		vec(const F * tp) : vec_base<3,F>(t)
		{}
		
		vec(F x, F y, F z)
		{ v[0] = x; v[1] = y; v[2] = z; }
		
		void get_value(F & x, F & y, F & z) const
		{ x = v[0]; y = v[1]; z = v[2]; }
		
		vec<3,F> cross( const vec<3,F> &rhs ) const
		{
			vec<3,F> rt;
			rt[0] = v[1]*rhs.v[2]-v[2]*rhs.v[1];
			rt[1] = v[2]*rhs.v[0]-v[0]*rhs.v[2];
			rt[2] = v[0]*rhs.v[1]-v[1]*rhs.v[0];	
			return rt;
		}
		
		vec<3,F> & set_value( const F & x, const F & y, const F & z)
		{ v[0] = x; v[1] = y; v[2] = z; return *this; }
		
	};
	
	typedef vec<3,float> vec3f;
	typedef vec<3,double> vec3d;
  
#else
	template <class F> class vec3 : public vec<3,F>
	{
		
    public:
		vec3(const F & t = F()) : vec<3,F>(t)
		{}
		vec3(const vec_base<3,F> & t) : vec<3,F>(t)
		{}
		vec3(const F * tp) : vec<3,F>(tp)
		{}
		vec3(F x, F y, F z)
		{ v[0] = x; v[1] = y; v[2] = z; }
		
		void get_value(F & x, F & y, F & z) const
		{ x = v[0]; y = v[1]; z = v[2]; }
		
		vec3<F> cross( const vec3<F> &rhs ) const
		{
			vec3<F> rt;
			rt[0] = v[1]*rhs.v[2]-v[2]*rhs.v[1];
			rt[1] = v[2]*rhs.v[0]-v[0]*rhs.v[2];
			rt[2] = v[0]*rhs.v[1]-v[1]*rhs.v[0];	
			return rt;
		}
		
		vec3<F> & set_value( const F & x, const F & y, const F & z)
		{ v[0] = x; v[1] = y; v[2] = z; return *this; }
		
	};

	

	typedef vec3<float> vec3f;
	typedef vec3<double> vec3d;
#endif
  
#ifndef WIN32
  // ugly-but-necessary forward declarations
	template <class F> class vec<4,F>;
	template <class F> vec<3,F> homogenize(const vec<4,F> & v);
	
	// partial specialization for vec<4,F>
	
  template <class F> class vec<4,F> : public vec_base<4,F>
  {
  public:
	  vec(const F & t = F()) : vec_base<4,F>(t)
      {}
	  vec(const vec_base<4,F> & t) : vec_base<4,F>(t)
      {}
	  vec(const F * tp) : vec_base<4,F>(t)
      {}
	  vec(F x, F y, F z, F w)
      { v[0] = x; v[1] = y; v[2] = z; v[3] = w; }
	  
	  void get_value(F & x, F & y, F & z, F & w) const
      { x = v[0]; y = v[1]; z = v[2]; w = v[3]; }
	  
	  void get_real( vec<3,F> &rhs ) const
      { rhs = homogenize(*this); }
	  
	  vec<4,F> & set_value( const F & x, const F & y, const F & z, const F & w)
      { v[0] = x; v[1] = y; v[2] = z; v[3] = w; return *this; }
	  
  };
  
  typedef vec<4,float> vec4f;
  typedef vec<4,double> vec4d;
  
  template <class F> inline
	  vec<3,F> homogenize(const vec<4,F> & v)
  {
	  vec<3,F> rt;
	  rt[0] = v[0]/v[3];
	  rt[1] = v[1]/v[3];
	  rt[2] = v[1]/v[3];
	  return rt;
  }
#else
  // ugly-but-necessary forward declarations
  template <class F> class vec4;
  template <class F> vec3<F> homogenize(const vec4<F> & v);
  
  // partial specialization for vec<4,F>
  
  template <class F> class vec4 : public vec<4,F>
  {
	  
  public:
	  vec4(const F & t = F()) : vec<4,F>(t)
      {}
	  vec4(const vec<4,F> & t) : vec<4,F>(t)
      {}
	  vec4(const F * tp) : vec<4,F>(tp)
      {}
	  vec4(F x, F y, F z, F w)
      { v[0] = x; v[1] = y; v[2] = z; v[3] = w; }
	  
	  void get_value(F & x, F & y, F & z, F & w) const
      { x = v[0]; y = v[1]; z = v[2]; w = v[3]; }
	  
	  void get_real( vec3<F> &rhs ) const
      { rhs = homogenize(*this); }
	  
	  vec4<F> & set_value( const F & x, const F & y, const F & z, const F & w)
      { v[0] = x; v[1] = y; v[2] = z; v[3] = w; return *this; }
	  
  };
  
  typedef vec4<float> vec4f;
  typedef vec4<double> vec4d;
  
  template <class F> inline vec3<F> homogenize(const vec4<F> & v)
  {
	  vec3<F> rt;
	  rt[0] = v[0]/v[3];
	  rt[1] = v[1]/v[3];
	  rt[2] = v[1]/v[3];
	  return rt;
  }
#endif
  
  








  

  //template <class F> inline F to_degrees(F radians) { return radians*180.0/3.14159265358979323846; }
  inline float  to_degrees(float  radians) { return radians*180.0f/3.14159265358979323846f; }
  inline double to_degrees(double radians) { return radians*180.0/3.14159265358979323846; }
  
  
  //template <class F> inline F to_radians(F degrees) { return degrees*3.14159265358979323846/180.0; }
  inline float  to_radians(int    degrees) { return degrees*3.14159265358979323846f/180.0f; }
  inline float  to_radians(float  degrees) { return degrees*3.14159265358979323846f/180.0f; }
  inline double to_radians(double degrees) { return degrees*3.14159265358979323846/180.0; }
  
  template <class F> class rotation
  {
  public:
    
    typedef F element_type;
    
    
    rotation()
	{
	  *this = identity();
	}
    
    rotation( const F v[4] )
	{
	  set_value( v );
	}
    
    
    rotation( F q0, F q1, F q2, F q3 )
	{
	  set_value( q0, q1, q2, q3 );
	}
    
    
    rotation( const matrix4<F> & m )
	{
	  set_value( m );
	}
    
    
    rotation( const vec3f &axis, F radians )
	{
	  set_value( axis, radians );
	}
    
    
    rotation( const vec3f &rotateFrom, const vec3f &rotateTo )
	{
	  set_value( rotateFrom, rotateTo );
	}
    
    const F * get_value() const
	{
	  return  & quaternion[0];
	}
    
    void get_value( F &q0, F &q1, F &q2, F &q3 ) const
	{
	  q0 = quaternion[0];
	  q1 = quaternion[1];
	  q2 = quaternion[2];
	  q3 = quaternion[3];
	}
    
    rotation<F> & set_value( F q0, F q1, F q2, F q3 )
	{
	  quaternion[0] = q0;
	  quaternion[1] = q1;
	  quaternion[2] = q2;
	  quaternion[3] = q3;
	  return *this;
	}
    
    void get_value( vec3f &axis, F &radians ) const
	{
	  radians = acos( quaternion[3] ) * 2;
	  if ( radians == 0.0 )
		  axis = vec3f( 0.0, 0.0, 1.0 );
	  else
	  {
		//axis.set_value( &quaternion[0]);
		axis[0] = quaternion[0];
		axis[1] = quaternion[1];
		axis[2] = quaternion[2];
	    axis.normalize();
	  }
	}
    
    void get_value( matrix4<F> &matrix ) const
	{        F s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
      
	s = 2.0 / ( quaternion[0] * quaternion[0] + quaternion[1] * quaternion[1] +
				quaternion[2] * quaternion[2] + quaternion[3] * quaternion[3] );
      
	xs = quaternion[0] * s;
	ys = quaternion[1] * s;
	zs = quaternion[2] * s;
      
	wx = quaternion[3] * xs;
	wy = quaternion[3] * ys;
	wz = quaternion[3] * zs;
      
	xx = quaternion[0] * xs;
	xy = quaternion[0] * ys;
	xz = quaternion[0] * zs;
      
	yy = quaternion[1] * ys;
	yz = quaternion[1] * zs;
      
	zz = quaternion[2] * zs;
      
	matrix[0][0] = F ( 1.0 - ( yy + zz ) );
	matrix[1][0] = F ( xy + wz );
	matrix[2][0] = F ( xz - wy );
      
	matrix[0][1] = F ( xy - wz );
	matrix[1][1] = F ( 1.0 - ( xx + zz ));
	matrix[2][1] = F ( yz + wx );
      
	matrix[0][2] = F ( xz + wy );
	matrix[1][2] = F ( yz - wx );
	matrix[2][2] = F ( 1.0 - ( xx + yy ));
      
	matrix[0][3] = matrix[1][3] = matrix[2][3] = 
	  matrix[3][0] = matrix[3][1] = matrix[3][2] = 0.0f;
	matrix[3][3] = 1.0f;
      
	}
    
    rotation<F> & set_value( const F * q )
	{
	  quaternion[0] = q[0];
	  quaternion[1] = q[1];
	  quaternion[2] = q[2];
	  quaternion[3] = q[3];
	  return *this;
	}
    
    rotation<F> & set_value( const matrix4<F> & m )
	{
	  F tr, s;
	  int i, j, k;
	  const int nxt[3] = { 1, 2, 0 };
	
	  tr = m[0][0] + m[1][1] + m[2][2];
	
	  if ( tr > 0.0 )
	  {
	    s = sqrt( tr + 1.0 );
	    quaternion[3] = F ( s * 0.5 );
	    s = 0.5 / s;
	    
	    quaternion[0] = F ( ( m[1][2] - m[2][1] ) * s );
	    quaternion[1] = F ( ( m[2][0] - m[0][2] ) * s );
	    quaternion[2] = F ( ( m[0][1] - m[1][0] ) * s );
	  }
	  else
	  {
	    i = 0;
	    if ( m[1][1] > m[0][0] )
	      i = 1;
	    
	    if ( m[2][2] > m[i][i] )
	      i = 2;
	    
	    j = nxt[i];
	    k = nxt[j];
	    
	    s = sqrt( ( m[i][j] - ( m[j][j] + m[k][k] )) + 1.0 );
	    
	    quaternion[i] = F ( s * 0.5 );
	    s = 0.5 / s;
	    
	    quaternion[3] = F ( ( m[j][k] - m[k][j] ) * s );
	    quaternion[j] = F ( ( m[i][j] + m[j][i] ) * s );
	    quaternion[k] = F ( ( m[i][k] + m[k][i] ) * s );
	  }
	
	  return *this;
	}
    
    rotation<F> & set_value( const vec3f &axis, F radians )
	{
	  double radsin = sin( radians / 2 );
	  quaternion[3] = cos( radians / 2 );
	  vec3f a = axis;
	  a.normalize();
	  quaternion[0] = F ( radsin * a[0] );
	  quaternion[1] = F ( radsin * a[1] );
	  quaternion[2] = F ( radsin * a[2] );
	  return *this;
	}
    
    rotation<F> & set_value( const vec3f & rotateFrom, const vec3f & rotateTo )
	{

	  vec3f p1, p2;
	  F alpha;

	  
	  p1 = rotateFrom; p1.normalize();
	  p2 = rotateTo;  p2.normalize();
	
	  alpha = p1.dot(p2);

	  if(alpha == 1.0)
	  { *this = identity(); return *this; }
	  if(alpha == -1.0)
	  {
		  vec3f v;
		  if(p1[0] != p1[1] || p1[0] != p1[2])
			v = vec3f(p1[1], p1[2], p1[0]);
		  else
			v = vec3f(-p1[0], p1[1], p1[2]);
		  v -= p1 * p1.dot(v);
		  v.normalize();
		  set_value(v, to_radians(F(180)));
		  return *this;
	  }

	  p1 = p1.cross(p2);  p1.normalize();
	  alpha = acos(alpha)/2.0;
	
	  p1 *= sin(alpha);
	  alpha = cos(alpha);
	
	  quaternion[0] = p1[0];
	  quaternion[1] = p1[1];
	  quaternion[2] = p1[2];
	  quaternion[3] = alpha;
	  return *this;
	}
    
    rotation<F> & operator *= ( const rotation<F> & q )
	{
	  vec3f  v(  quaternion[0],  quaternion[1],  quaternion[2]);
	  vec3f vp(q.quaternion[0],q.quaternion[1],q.quaternion[2]);
	  F w = quaternion[3], wp = q.quaternion[3];
	
	  vec3f newV;
	  F newW;
	
	  newV = v.cross(vp);
	  vec3f tmpV = vp;
	  tmpV *= w;
	  newV += tmpV;
	  tmpV = v;
	  tmpV *= wp;
	  newV += tmpV;
	  newW = w*wp - v.dot(vp);
	
	  quaternion[0] = newV[0];
	  quaternion[1] = newV[1];
	  quaternion[2] = newV[2];
	  quaternion[3] = newW;
	
	  // normalize...
	
	  newW = ( quaternion[0]*quaternion[0] +
			   quaternion[1]*quaternion[1] +
			   quaternion[2]*quaternion[2] +
			   quaternion[3]*quaternion[3] );
	
	  quaternion[0] /= newW;
	  quaternion[1] /= newW;
	  quaternion[2] /= newW;
	  quaternion[3] /= newW;
	  return *this;
	}
    
    friend bool operator == TEMPLATE_FUNCTION ( const rotation<F> & q1, const rotation<F> & q2 );      
    
    friend bool operator != TEMPLATE_FUNCTION ( const rotation<F> & q1, const rotation<F> & q2 );
    
    friend rotation<F> operator * TEMPLATE_FUNCTION ( const rotation<F> & q1, const rotation<F> & q2 );
    
    bool equals( const rotation<F> & r, F tolerance ) const
	{
	  F t;
	
	  t = ((quaternion[0]-r.quaternion[0])*(quaternion[0]-r.quaternion[0]) +
		   (quaternion[1]-r.quaternion[1])*(quaternion[1]-r.quaternion[1]) +
		   (quaternion[2]-r.quaternion[2])*(quaternion[2]-r.quaternion[2]) +
		   (quaternion[3]-r.quaternion[3])*(quaternion[3]-r.quaternion[3]) );
	  if(t > tolerance) return 0;
	  return 1;
	}
    
    rotation<F> & invert()
	{
	  quaternion[0] *= -1.0;
	  quaternion[1] *= -1.0;
	  quaternion[2] *= -1.0;
	  return *this;
	}
    
    rotation<F> inverse() const
	{
	  rotation<F> r = *this;
	  return r.invert();
	}
    
    void mult_vec( const vec3f &src, vec3f &dst ) const
	{
	  matrix4<F> m;
	  get_value(m);
	  m.mult_matrix_vec(src, dst);
	}

	void mult_vec( vec3f & src_and_dst) const
	{
		mult_vec(vec3f(src_and_dst), src_and_dst);
	}
    
    void scale_angle( F scaleFactor )
	{
	  vec3f axis;
	  F radians;
	
	  get_value(axis, radians);
	  radians *= scaleFactor;
	  set_value(axis, radians);
	}
    
    static rotation<F> slerp( const rotation<F> & rot0, const rotation<F> & rot1, F t )
	{
	  rotation<F> r;
	  vec4f q0(rot0.quaternion);
	  vec4f q1(rot1.quaternion);
	
	  q0.normalize();
	  q1.normalize();
	
	  F omega = acos(q0.dot(q1));
	
	  if(omega == 0.0)
	  {
	    r = rot0;
	    return r;
	  }
	
	  F f1, f2;
	
	  f1 = sin(omega*(1-t))/sin(omega);
	  f2 = sin(omega*t)/sin(omega);
	
	  r.quaternion[0] = f1*rot0.quaternion[0]+f2*rot1.quaternion[0];
	  r.quaternion[1] = f1*rot0.quaternion[1]+f2*rot1.quaternion[1];
	  r.quaternion[2] = f1*rot0.quaternion[2]+f2*rot1.quaternion[2];
	  r.quaternion[3] = f1*rot0.quaternion[3]+f2*rot1.quaternion[3];
	  return r;
	}
    
    static rotation<F> identity()
	{
	  static rotation<F> ident( vec3f( 0.0, 0.0, 1.0 ), 0.0 );
	  return ident;
	}

    rotation<F> operator - () const
	{ return this->inverse(); }
    
    F & operator []( int i )
	{
	  return quaternion[ i ];
	}
    
    const F & operator []( int i ) const
	{
	  return quaternion[ i ];
	}
    
  protected:
    F quaternion[4];
  };
  
  typedef rotation<float> rotationf;
  typedef rotation<double> rotationd;
  
  template <class F>
  bool operator == ( const rotation<F> & q1, const rotation<F> & q2 )
  {
	return (q1.quaternion[0] == q2.quaternion[0] &&
			q1.quaternion[1] == q2.quaternion[1] &&
			q1.quaternion[2] == q2.quaternion[2] &&
			q1.quaternion[3] == q2.quaternion[3] );
  }
  
  template < class F >
  bool operator != ( const rotation<F> & q1, const rotation<F> & q2 )
  {
	return ! ( q1 == q2 );
  }
  
  template <class F>
  rotation<F> operator * ( const rotation<F> & q1, const rotation<F> & q2 )
  {
	rotation<F> r=q1;
	r *= q2;
	return r;
  }
  
  












  
  template <class F> class line
  {
  public:
    
    typedef F element_type;
    
    line()
      {
	set_value(vec<3,F>(0,0,0),vec<3,F>(0,0,1));
      }
    
    line( const vec<3,F> &p0, const vec<3,F> &p1)
      {
	set_value(p0,p1);
      }
    
    void set_value( const vec<3,F> &p0, const vec<3,F> &p1)
      {
	position = p0;
	direction = p1-p0;
	direction.normalize();
      }
    
    bool get_closest_points(const line &line2, 
			  vec<3,F> &pointOnThis,
			  vec<3,F> &pointOnThat)
      {
	
	// quick check to see if parallel -- if so, quit.
	if(fabs(direction.dot(line2.direction)) == 1.0)
	  return 0;
	line<F> l2 = line2;
	
	// Algorithm: Brian Jean
	// 
	register F u;
	register F v;
	vec<3,F> Vr = direction;
	vec<3,F> Vs = l2.direction;
	register F Vr_Dot_Vs = Vr.dot(Vs);
	register F detA = 1.0 - (Vr_Dot_Vs * Vr_Dot_Vs);
	vec<3,F> C = l2.position - position;
	register F C_Dot_Vr =  C.dot(Vr);
	register F C_Dot_Vs =  C.dot(Vs);
	
	u = (C_Dot_Vr - Vr_Dot_Vs * C_Dot_Vs)/detA;
	v = (C_Dot_Vr * Vr_Dot_Vs - C_Dot_Vs)/detA;
	
	pointOnThis = position;
	pointOnThis += direction * u;
	pointOnThat = l2.position;
	pointOnThat += l2.direction * v;
	
	return 1;
      }
    
    vec<3,F> get_closest_point(const vec<3,F> &point)
      {
	vec<3,F> np = point - position;
	vec<3,F> rp = direction*direction.dot(np)+position;
	return rp;
      }
    
    const vec<3,F> & get_position() const {return position;}
    
    const vec<3,F> & get_direction() const {return direction;}
    
  protected:
    vec<3,F> position;
    vec<3,F> direction;
  };
  
  typedef line<float> linef;
  typedef line<double> lined;
  














  
  
  
  
  template <class F> class plane
  {
  public:
	  
	  typedef F element_type;
	  
	  plane()
      {
		  planedistance = 0.0;
		  planenormal.set_value( 0.0, 0.0, 1.0 );
      }
	  
	  
	  plane( const vec<3,F> &p0, const vec<3,F> &p1, const vec<3,F> &p2 )
      {
		  vec<3,F> v0 = p1 - p0;
		  vec<3,F> v1 = p2 - p0;
		  planenormal = v0.cross(v1);  
		  planenormal.normalize();
		  planedistance = p0.dot(planenormal);
      }
	  
	  plane( const vec<3,F> &normal, F distance )
      {
		  planedistance = distance;
		  planenormal = normal;
		  planenormal.normalize();
      }
	  
	  plane( const vec<3,F> &normal, const vec<3,F> &point )
      {
		  planenormal = normal;
		  planenormal.normalize();
		  planedistance = point.dot(planenormal);
      }
	  
	  void offset( F d )
      {
		  planedistance += d;
      }
	  
	  bool intersect( const line<F> &l, vec<3,F> &intersection ) const
      {
		  vec<3,F> pos, dir;
		  vec<3,F> pn = planenormal;
		  F pd = planedistance;
		  
		  pos = l.getPosition();
		  dir = l.getDirection();
		  
		  if(dir.dot(pn) == 0.0) return 0;
		  pos -= pn*pd;
		  // now we're talking about a plane passing through the origin
		  if(pos.dot(pn) < 0.0) pn.negate();
		  if(dir.dot(pn) > 0.0) dir.negate();
		  vec<3,F> ppos = pn * pos.dot(pn);
		  ppos.length()/dir.dot(-pn);
		  pos = (ppos.length()/dir.dot(-pn))*dir;
		  intersection = l.getPosition();
		  intersection += pos;
		  return 1;
      }
	  
	  void transform( const matrix4<F> &matrix )
      {
		  matrix4<F> invtr = matrix.inverse();
		  invtr = invtr.transpose();
		  
		  vec<3,F> pntOnplane = planenormal * planedistance;
		  vec<3,F> newPntOnplane;
		  vec<3,F> newnormal;
		  
		  invtr.multDirMatrix(planenormal, newnormal);
		  matrix.multvecMatrix(pntOnplane, newPntOnplane);
		  
		  newnormal.normalize();
		  planenormal = newnormal;
		  planedistance = newPntOnplane.dot(planenormal);
      }
	  
	  bool is_in_half_space( const vec<3,F> &point ) const
      {
		  
		  if(( point.dot(planenormal) - planedistance) < 0.0)
			  return 0;
		  return 1;
      }
	  
	  
	  F distance( const vec<3,F> & point ) const 
      {
		  return planenormal.dot(point - planenormal*planedistance);
      }
	  
	  const vec<3,F> &get_normal() const
      {
		  return planenormal;
      }
	  
	  
	  F get_distance_from_origin() const
      {
		  return planedistance;
      }
	  
	  
	  friend bool operator == TEMPLATE_FUNCTION ( const plane<F> & p1, const plane<F> & p2 );
	  
	  
	  friend bool operator != TEMPLATE_FUNCTION ( const plane<F> & p1, const plane<F> & p2 );
	  
  protected:
	  vec<3,F> planenormal;
	  F planedistance;
  };
  
  template <class F> bool operator == (const plane<F> & p1, const plane<F> & p2 )
  {
	  return (  p1.planedistance == p2.planedistance && p1.planenormal == p2.planenormal);
  }
  
  template <class F> bool operator != ( const plane<F> & p1, const plane<F> & p2 )
  {
	  return  (p1.planedistance != p2.planedistance || p1.planenormal != p2.planenormal);
  }
  
  typedef plane<float> planef;
  typedef plane<double> planed;
  
  





  // matrix

  
  template <class F> class matrix4
  {
    
  public:
    
    typedef F element_type;
    
    typedef F mat[4][4];
    
    matrix4() { make_identity(); }
    
	matrix4( F * m )
	{
		for(int i=0; i < 4; i++)
			for(int j=0; j < 4; j++)
				matrix[i][j] = m[j*4 + i];
	}
    
    matrix4( F a11, F a12, F a13, F a14,
	       F a21, F a22, F a23, F a24,
		   F a31, F a32, F a33, F a34,
		   F a41, F a42, F a43, F a44 )
	{
		matrix[0][0] = a11;
		matrix[0][1] = a12;
		matrix[0][2] = a13;
		matrix[0][3] = a14;
		
		matrix[1][0] = a21;
		matrix[1][1] = a22;
		matrix[1][2] = a23;
		matrix[1][3] = a24;
		
		matrix[2][0] = a31;
		matrix[2][1] = a32;
		matrix[2][2] = a33;
		matrix[2][3] = a34;
		
		matrix[3][0] = a41;
		matrix[3][1] = a42;
		matrix[3][2] = a43;
		matrix[3][3] = a44;
	}
    
    
    matrix4( const mat &m ) { set_value( m ); }
    
    void set_value( const mat &m )
	{
		matrix[0][0] = m[0][0];
		matrix[0][1] = m[0][1];
		matrix[0][2] = m[0][2];
		matrix[0][3] = m[0][3];
		
		matrix[1][0] = m[1][0];
		matrix[1][1] = m[1][1];
		matrix[1][2] = m[1][2];
		matrix[1][3] = m[1][3];
		
		matrix[2][0] = m[2][0];
		matrix[2][1] = m[2][1];
		matrix[2][2] = m[2][2];
		matrix[2][3] = m[2][3];
		
		matrix[3][0] = m[3][0];
		matrix[3][1] = m[3][1];
		matrix[3][2] = m[3][2];
		matrix[3][3] = m[3][3];
	}
    
    
    void get_value( mat &m ) const
	{
		m[0][0] = matrix[0][0];
		m[0][1] = matrix[0][1];
		m[0][2] = matrix[0][2];
		m[0][3] = matrix[0][3];
		
		m[1][0] = matrix[1][0];
		m[1][1] = matrix[1][1];
		m[1][2] = matrix[1][2];
		m[1][3] = matrix[1][3];
		
		m[2][0] = matrix[2][0];
		m[2][1] = matrix[2][1];
		m[2][2] = matrix[2][2]; 
		m[2][3] = matrix[2][3];
		
		m[3][0] = matrix[3][0];
		m[3][1] = matrix[3][1];
		m[3][2] = matrix[3][2]; 
		m[3][3] = matrix[3][3];
	}
    
    
    const mat& get_value() const
	{
		return matrix;
	}
    
    
    void make_identity()
	{
		matrix[0][0] = 1.0;
		matrix[0][1] = 0.0;
		matrix[0][2] = 0.0; 
		matrix[0][3] = 0.0;
		
		matrix[1][0] = 0.0;
		matrix[1][1] = 1.0; 
		matrix[1][2] = 0.0;
		matrix[1][3] = 0.0;
		
		matrix[2][0] = 0.0;
		matrix[2][1] = 0.0;
		matrix[2][2] = 1.0;
		matrix[2][3] = 0.0;
		
		matrix[3][0] = 0.0; 
		matrix[3][1] = 0.0; 
		matrix[3][2] = 0.0;
		matrix[3][3] = 1.0;
	}
	
	
    static matrix4<F> identity()
	{
		static matrix4<F> mident( 1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0  );
		return mident;
	}
    
    
    void set_rotate( const rotation<F> & q )
	{
		q.get_value( *this );
	}
    
    
    void set_scale( F s )
	{
		matrix[0][0] = s;
		matrix[1][1] = s;
		matrix[2][2] = s;
	}
    
    void set_scale( const vec<3,F> & s )
	{
		matrix[0][0] = s[0];
		matrix[1][1] = s[1];
		matrix[2][2] = s[2];
	}
    
    
    void set_translate( const vec<3,F> & t )
	{
		matrix[0][3] = t[0];
		matrix[1][3] = t[1];
		matrix[2][3] = t[2];
	}
    
    
    matrix4<F> inverse() const
	{
		matrix4<F> minv;
		
		F r1[8], r2[8], r3[8], r4[8];
		F *s[4], *tmprow;
		
		s[0] = &r1[0];
		s[1] = &r2[0];
		s[2] = &r3[0];
		s[3] = &r4[0];
		
		register int i,j,p,jj;
		for(i=0;i<4;i++)
		{
			for(j=0;j<4;j++)
			{
				s[i][j] = matrix[i][j];
				if(i==j) s[i][j+4] = 1.0;
				else s[i][j+4] = 0.0;
			}
		}
		F scp[4];
		for(i=0;i<4;i++)
		{
			scp[i] = fabs(s[i][0]);
			for(j=1;j<4;j++)
				if(fabs(s[i][j]) > scp[i]) scp[i] = fabs(s[i][j]);
				if(scp[i] == 0.0) return minv; // singular matrix!
		}
		
		int pivot_to;
		F scp_max;
		for(i=0;i<4;i++)
		{
			// select pivot row
			pivot_to = i;
			scp_max = fabs(s[i][i]/scp[i]);
			// find out which row should be on top
			for(p=i+1;p<4;p++)
				if(fabs(s[p][i]/scp[p]) > scp_max)
				{ scp_max = fabs(s[p][i]/scp[p]); pivot_to = p; }
				// Pivot if necessary
				if(pivot_to != i)
				{
					tmprow = s[i];
					s[i] = s[pivot_to];
					s[pivot_to] = tmprow;
					F tmpscp;
					tmpscp = scp[i];
					scp[i] = scp[pivot_to];
					scp[pivot_to] = tmpscp;
				}
				
				F mji;
				// perform gaussian elimination
				for(j=i+1;j<4;j++)
				{
					mji = s[j][i]/s[i][i];
					s[j][i] = 0.0;
					for(jj=i+1;jj<8;jj++)
						s[j][jj] -= mji*s[i][jj];
				}
		}
		if(s[3][3] == 0.0) return minv; // singular matrix!
		
		//
		// Now we have an upper triangular matrix.
		//
		//  x x x x | y y y y
		//  0 x x x | y y y y 
		//  0 0 x x | y y y y
		//  0 0 0 x | y y y y
		//
		//  we'll back substitute to get the inverse
		//
		//  1 0 0 0 | z z z z
		//  0 1 0 0 | z z z z
		//  0 0 1 0 | z z z z
		//  0 0 0 1 | z z z z 
		//
		
		F mij;
		for(i=3;i>0;i--)
		{
			for(j=i-1;j > -1; j--)
			{
				mij = s[j][i]/s[i][i];
				for(jj=j+1;jj<8;jj++)
					s[j][jj] -= mij*s[i][jj];
			}
		}
		
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				minv.matrix[i][j] = s[i][j+4] / s[i][i];
			
			return minv;
	}
    
    
    matrix4<F> transpose() const
	{
	    matrix4<F> mtrans;
		
		register int i,j;  
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				mtrans.matrix[i][j] = matrix[j][i];
			
			return mtrans;
	}
    
    
    matrix4<F> & mult_right( const matrix4<F> & b )
	{
		matrix4<F> mt;
		
		mt.matrix[0][0] = ( matrix[0][0] * b.matrix[0][0] +
			matrix[0][1] * b.matrix[1][0] +
			matrix[0][2] * b.matrix[2][0] +
			matrix[0][3] * b.matrix[3][0] );
		
		mt.matrix[0][1] = ( matrix[0][0] * b.matrix[0][1] +
			matrix[0][1] * b.matrix[1][1] +
			matrix[0][2] * b.matrix[2][1] +
			matrix[0][3] * b.matrix[3][1] );
		
		mt.matrix[0][2] = ( matrix[0][0] * b.matrix[0][2] +
			matrix[0][1] * b.matrix[1][2] +
			matrix[0][2] * b.matrix[2][2] +
			matrix[0][3] * b.matrix[3][2] );
		
		mt.matrix[0][3] = ( matrix[0][0] * b.matrix[0][3] +
			matrix[0][1] * b.matrix[1][3] +
			matrix[0][2] * b.matrix[2][3] +
			matrix[0][3] * b.matrix[3][3] );
		
		mt.matrix[1][0] = ( matrix[1][0] * b.matrix[0][0] +
			matrix[1][1] * b.matrix[1][0] +
			matrix[1][2] * b.matrix[2][0] +
			matrix[1][3] * b.matrix[3][0] );
		
		mt.matrix[1][1] = ( matrix[1][0] * b.matrix[0][1] +
			matrix[1][1] * b.matrix[1][1] +
			matrix[1][2] * b.matrix[2][1] + 
			matrix[1][3] * b.matrix[3][1] );
		
		mt.matrix[1][2] = ( matrix[1][0] * b.matrix[0][2] +
			matrix[1][1] * b.matrix[1][2] +
			matrix[1][2] * b.matrix[2][2] +
			matrix[1][3] * b.matrix[3][2] );
		
		mt.matrix[1][3] = ( matrix[1][0] * b.matrix[0][3] +
			matrix[1][1] * b.matrix[1][3] + 
			matrix[1][2] * b.matrix[2][3] +
			matrix[1][3] * b.matrix[3][3] );
		
		mt.matrix[2][0] = ( matrix[2][0] * b.matrix[0][0] +
			matrix[2][1] * b.matrix[1][0] + 
			matrix[2][2] * b.matrix[2][0] + 
			matrix[2][3] * b.matrix[3][0] );
		
		mt.matrix[2][1] = ( matrix[2][0] * b.matrix[0][1] +
			matrix[2][1] * b.matrix[1][1] +
			matrix[2][2] * b.matrix[2][1] +
			matrix[2][3] * b.matrix[3][1] );
		
		mt.matrix[2][2] = ( matrix[2][0] * b.matrix[0][2] + 
			matrix[2][1] * b.matrix[1][2] +
			matrix[2][2] * b.matrix[2][2] + 
			matrix[2][3] * b.matrix[3][2] );
		
		mt.matrix[2][3] = ( matrix[2][0] * b.matrix[0][3] + 
			matrix[2][1] * b.matrix[1][3] +
			matrix[2][2] * b.matrix[2][3] + 
			matrix[2][3] * b.matrix[3][3] );
		
		mt.matrix[3][0] = ( matrix[3][0] * b.matrix[0][0] + 
			matrix[3][1] * b.matrix[1][0] + 
			matrix[3][2] * b.matrix[2][0] + 
			matrix[3][3] * b.matrix[3][0] );
		
		mt.matrix[3][1] = ( matrix[3][0] * b.matrix[0][1] + 
			matrix[3][1] * b.matrix[1][1] + 
			matrix[3][2] * b.matrix[2][1] + 
			matrix[3][3] * b.matrix[3][1] );
		
		mt.matrix[3][2] = ( matrix[3][0] * b.matrix[0][2] + 
			matrix[3][1] * b.matrix[1][2] + 
			matrix[3][2] * b.matrix[2][2] + 
			matrix[3][3] * b.matrix[3][2] );
		
		mt.matrix[3][3] = ( matrix[3][0] * b.matrix[0][3] + 
			matrix[3][1] * b.matrix[1][3] + 
			matrix[3][2] * b.matrix[2][3] + 
			matrix[3][3] * b.matrix[3][3] );
		
		return *this = mt;
	}
    
    matrix4<F> & mult_left( const matrix4<F> & b )
	{
		matrix4<F> mt;
		
		mt.matrix[0][0] = ( b.matrix[0][0] * matrix[0][0] +
			b.matrix[0][1] * matrix[1][0] +
			b.matrix[0][2] * matrix[2][0] +
			b.matrix[0][3] * matrix[3][0] );
		
		mt.matrix[0][1] = ( b.matrix[0][0] * matrix[0][1] +
			b.matrix[0][1] * matrix[1][1] +
			b.matrix[0][2] * matrix[2][1] +
			b.matrix[0][3] * matrix[3][1] );
		
		mt.matrix[0][2] = ( b.matrix[0][0] * matrix[0][2] +
			b.matrix[0][1] * matrix[1][2] +
			b.matrix[0][2] * matrix[2][2] +
			b.matrix[0][3] * matrix[3][2] );
		
		mt.matrix[0][3] = ( b.matrix[0][0] * matrix[0][3] +
			b.matrix[0][1] * matrix[1][3] +
			b.matrix[0][2] * matrix[2][3] + 
			b.matrix[0][3] * matrix[3][3] );
		
		mt.matrix[1][0] = ( b.matrix[1][0] * matrix[0][0] +
			b.matrix[1][1] * matrix[1][0] +
			b.matrix[1][2] * matrix[2][0] +
			b.matrix[1][3] * matrix[3][0] );
		
		mt.matrix[1][1] = ( b.matrix[1][0] * matrix[0][1] +
			b.matrix[1][1] * matrix[1][1] +
			b.matrix[1][2] * matrix[2][1] +
			b.matrix[1][3] * matrix[3][1] );
		
		mt.matrix[1][2] = ( b.matrix[1][0] * matrix[0][2] +
			b.matrix[1][1] * matrix[1][2] +
			b.matrix[1][2] * matrix[2][2] +
			b.matrix[1][3] * matrix[3][2] );
		
		mt.matrix[1][3] = ( b.matrix[1][0] * matrix[0][3] +
			b.matrix[1][1] * matrix[1][3] +
			b.matrix[1][2] * matrix[2][3] +
			b.matrix[1][3] * matrix[3][3] );
		
		mt.matrix[2][0] = ( b.matrix[2][0] * matrix[0][0] +
			b.matrix[2][1] * matrix[1][0] +
			b.matrix[2][2] * matrix[2][0] +
			b.matrix[2][3] * matrix[3][0] );
		
		mt.matrix[2][1] = ( b.matrix[2][0] * matrix[0][1] +
			b.matrix[2][1] * matrix[1][1] +
			b.matrix[2][2] * matrix[2][1] +
			b.matrix[2][3] * matrix[3][1] );
		
		mt.matrix[2][2] = ( b.matrix[2][0] * matrix[0][2] +
			b.matrix[2][1] * matrix[1][2] +
			b.matrix[2][2] * matrix[2][2] +
			b.matrix[2][3] * matrix[3][2] );
		
		mt.matrix[2][3] = ( b.matrix[2][0] * matrix[0][3] + 
			b.matrix[2][1] * matrix[1][3] +
			b.matrix[2][2] * matrix[2][3] +
			b.matrix[2][3] * matrix[3][3] );
		
		mt.matrix[3][0] = ( b.matrix[3][0] * matrix[0][0] +
			b.matrix[3][1] * matrix[1][0] +
			b.matrix[3][2] * matrix[2][0] +
			b.matrix[3][3] * matrix[3][0] );
		
		mt.matrix[3][1] = ( b.matrix[3][0] * matrix[0][1] +
			b.matrix[3][1] * matrix[1][1] +
			b.matrix[3][2] * matrix[2][1] +
			b.matrix[3][3] * matrix[3][1] );
		
		mt.matrix[3][2] = ( b.matrix[3][0] * matrix[0][2] +
			b.matrix[3][1] * matrix[1][2] + 
			b.matrix[3][2] * matrix[2][2] +
			b.matrix[3][3] * matrix[3][2] );
		
		mt.matrix[3][3] = ( b.matrix[3][0] * matrix[0][3] +
			b.matrix[3][1] * matrix[1][3] +
			b.matrix[3][2] * matrix[2][3] +
			b.matrix[3][3] * matrix[3][3] );
		
		return *this = mt;
	}
	
	
    void mult_matrix_vec( const vec<3,F> &src, vec<3,F> &dst ) const
	{
		F w = ( src[0] * matrix[3][0] +
			src[1] * matrix[3][1] +
			src[2] * matrix[3][2] +
			matrix[3][3]          );
		dst[0]  = ( src[0] * matrix[0][0] +
			src[1] * matrix[0][1] + 
			src[2] * matrix[0][2] + 
			matrix[0][3]          ) / w;
		dst[1]  = ( src[0] * matrix[1][0] +
			src[1] * matrix[1][1] +
			src[2] * matrix[1][2] +
			matrix[1][3]          ) / w;
		dst[2]  = ( src[0] * matrix[2][0] +
			src[1] * matrix[2][1] +
			src[2] * matrix[2][2] +
			matrix[2][3]          ) / w;
	}
    
	void mult_matrix_vec( vec3f & src_and_dst) const
	{
		mult_matrix_vec(vec3f(src_and_dst), src_and_dst);
	}
    
    void mult_vec_matrix( const vec<3,F> &src, vec<3,F> &dst ) const
	{
		F w = ( src[0] * matrix[0][3] +
			src[1] * matrix[1][3] + 
			src[2] * matrix[2][3] +
			matrix[3][3]          );
		dst[0]  = ( src[0] * matrix[0][0] +
			src[1] * matrix[1][0] +
			src[2] * matrix[2][0] +
			matrix[3][0]          ) / w;
		dst[1]  = ( src[0] * matrix[0][1] +
			src[1] * matrix[1][1] +
			src[2] * matrix[2][1] +
			matrix[3][1]          ) / w;
		dst[2]  = ( src[0] * matrix[0][2] +
			src[1] * matrix[1][2] + 
			src[2] * matrix[2][2] +
			matrix[3][2]          ) / w;
	}
        
	void mult_vec_matrix( vec3f & src_and_dst) const
	{
		mult_vec_matrix(vec3f(src_and_dst), src_and_dst);
	}
    
    
    void mult_dir_matrix( const vec<3,F> &src, vec<3,F> &dst ) const
	{
		//F w = ( src[0] * matrix[0][3] +
		//	      src[1] * matrix[1][3] + 
		//	      src[2] * matrix[2][3] +
		//	      matrix[3][3]);
		dst[0]  = ( src[0] * matrix[0][0] +
			src[1] * matrix[1][0] +
			src[2] * matrix[2][0] ) ;
		dst[1]  = ( src[0] * matrix[0][1] +
			src[1] * matrix[1][1] +
			src[2] * matrix[2][1] ) ;
		dst[2]  = ( src[0] * matrix[0][2] +
			src[1] * matrix[1][2] + 
			src[2] * matrix[2][2] ) ;
	}
    
    
	void mult_dir_matrix( vec3f & src_and_dst) const
	{
		mult_dir_matrix(vec3f(src_and_dst), src_and_dst);
	}
    
    
    void mult_matrix_dir( const vec<3,F> &src, vec<3,F> &dst ) const
	{
		//F w = ( src[0] * matrix[0][3] +
		//	      src[1] * matrix[1][3] + 
		//	      src[2] * matrix[2][3] +
		//	      matrix[3][3]);
		dst[0]  = ( src[0] * matrix[0][0] +
			src[1] * matrix[0][1] +
			src[2] * matrix[0][2] ) ;
		dst[1]  = ( src[0] * matrix[1][0] +
			src[1] * matrix[1][1] +
			src[2] * matrix[1][2] ) ;
		dst[2]  = ( src[0] * matrix[2][0] +
			src[1] * matrix[2][1] + 
			src[2] * matrix[2][2] ) ;
	}
        
	void mult_matrix_dir( vec3f & src_and_dst) const
	{
		mult_matrix_dir(vec3f(src_and_dst), src_and_dst);
	}
    

    
    F * operator []            ( int i )
	{
		return &matrix[i][0];
	}
    
    
    const F * operator []      ( int i ) const
	{
		return &matrix[i][0];
	}
    
    
    matrix4<F> & operator =      ( const mat & m )
	{
		set_value( m );
		return *this;
	}
    
    
    matrix4<F> & operator =      ( const matrix4<F> & m )
	{
		set_value( m.matrix );
		return *this;
	}
    
    
    matrix4<F> & operator =      ( const rotation<F> & q )
	{
		set_rotate( q );
		return *this;
	}
    
    
    matrix4<F> & operator *=     ( const matrix4<F> & m )
	{
		mult_right( m );
		return *this;
	}
    
    friend matrix4<F> operator * TEMPLATE_FUNCTION ( const matrix4<F> & m1,
		const matrix4<F> & m2 );
    
    
    friend bool operator ==       TEMPLATE_FUNCTION ( const matrix4<F> & m1,
		const matrix4<F> & m2 );
    
    
    friend bool operator !=       TEMPLATE_FUNCTION ( const matrix4<F> & m1,
		const matrix4<F> & m2 );
    
    
    bool equals( const matrix4<F> & m, F tolerance ) const
	{
		register int i,j;
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				if(fabs(matrix[i][j]-m.matrix[i][j]) > tolerance)
					return 0;
				return 1;
	}
    
  protected:
	  mat matrix;
  };
  
  typedef  matrix4<float> matrix4f;
  typedef  matrix4<double> matrix4d;
  
  
  
  template <class F> matrix4<F> operator * ( const matrix4<F> & m1, 
	  const matrix4<F> & m2 )
  {
	  static matrix4<F> product;
	  
	  product = m1;
	  product.mult_right(m2);
	  
	  return product;
  }
  
  template <class F> bool operator ==( const matrix4<F> &m1, const matrix4<F> &m2 )
  {
	  return ( m1.matrix[0][0] == m2.matrix[0][0] &&
		  m1.matrix[0][1] == m2.matrix[0][1] &&
		  m1.matrix[0][2] == m2.matrix[0][2] &&
		  m1.matrix[0][3] == m2.matrix[0][3] &&
		  m1.matrix[1][0] == m2.matrix[1][0] &&
		  m1.matrix[1][1] == m2.matrix[1][1] &&
		  m1.matrix[1][2] == m2.matrix[1][2] &&
		  m1.matrix[1][3] == m2.matrix[1][3] &&
		  m1.matrix[2][0] == m2.matrix[2][0] &&
		  m1.matrix[2][1] == m2.matrix[2][1] &&
		  m1.matrix[2][2] == m2.matrix[2][2] &&
		  m1.matrix[2][3] == m2.matrix[2][3] &&
		  m1.matrix[3][0] == m2.matrix[3][0] &&
		  m1.matrix[3][1] == m2.matrix[3][1] &&
		  m1.matrix[3][2] == m2.matrix[3][2] &&
		  m1.matrix[3][3] == m2.matrix[3][3] );
  }
  
  template <class F> bool operator != ( const matrix4<F> & m1,
	  const matrix4<F> & m2 )
  {
	  return !( m1 == m2 );
  }  
  
}  // namespace glh



#endif

