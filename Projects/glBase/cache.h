/*
Copyright (C) 2000-2001 Adrian Welbourn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/*
	File:		cache.h

	Function:	cache class - for doing our own malloc.
				Implements a circular cache. No need to free since old
				objects just get overwritten. Based on Quake's surface
				caching code.
				If VAR/Fence is supported, fast memory may be requested
				(from graphics or AGP memory).
*/

#ifndef CACHE_H
#define CACHE_H

#include "general.h"

typedef struct cchobj_str {			/* object in cache */
	size_t size;					/* size of object in cache */
	struct cchobj_str *next;		/* next object in cache */
	struct cchobj_str **owner;		/* to be cacheable an object must contain
									   a cchobj pointer */
	int fence;						/* NV fence */
	unsigned char *data;			/* pointer a size element array */
} *cchobj;

typedef struct cch_str {			/* cache */
	bool	fast;					/* objects in fast memory? */
	size_t	size;					/* cache size */
	cchobj	base;					/* cache base pointer */
	cchobj	rover;					/* cache object rover */
	cchobj	initRover;				/* used to detect cache thrashing */
	unsigned char *fastBase;		/* fast memory base pointer */
	bool	wrapped, thrashed;
} *cch;

#define cch_free(c) ((c)->size-((unsigned char*)(c)->rover-(unsigned char*)(c)->base))
#define cch_thrashed(c) ((c)->thrashed)

cch cch_create(size_t size, bool fast);
void cch_destroy(cch c);
void cch_init(cch c, size_t size);
void cch_flush(cch c);
void cch_initFrame(cch c);
cchobj cch_alloc(cch c, size_t size, cchobj *owner);
void *cch_malloc(cch c, size_t size, cchobj *owner);
void cch_dump(cch c);

#endif /* CACHE_H */
