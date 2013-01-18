// Simple array template.
// Copyright (c) Cass W. Everitt 1999 

#ifndef _GLH_ARRAY_H_
#define _GLH_ARRAY_H_

namespace glh
{
  
  template <class T> class array2
  {
  public:
	typedef T value_type;

	array2(int width=1, int height=1) 
    {
      w = width;
      h = height;
      d = new T [w * h];
      clear(T());
    }
	
	array2(const array2<T> & t)
    {
      w = h = 0;
      d=0;
      (*this) = t;
    }
	
	// intentionally non-virtual 
	~array2() { delete d; }
	
	const array2 & operator = (const array2<T> & t)
    {
      if(w != t.w || h != t.h)
	  {
		delete d;
		w = t.w;
		h = t.h;
		d = new T [w * h];
	  }
	  int sz = w * h;
      for(int i = 0; i < sz; i++) d[i] = t.d[i];
      return *this;
    }
	
	T & operator () (int i, int j)
    { return d[i + j * w]; }
	
	const T & operator () (int i, int j) const
    { return d[i + j * w]; }
	
	int size(int i) const 
	{ 
	  switch (i)
	  {
	  case 0:
		return w;
	  case 1:
		return h;
	  default:
		return 0;
	  }
	}
	
	void clear(const T & val) 
    {
      int sz = w * h;
      for(int i = 0; i < sz; i++) d[i] = val;
    }
	
  private:
	
	int w, h;
	T * d;	
  };
  
}
#endif
