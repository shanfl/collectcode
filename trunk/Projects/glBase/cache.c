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
	File:		cache.c

	Function:	cache class - for doing our own malloc.
				Implements a circular cache. No need to free since old
				objects just get overwritten. Based on Quake's surface
				caching code.
				If VAR/Fence is supported, fast memory may be requested
				(from graphics or AGP memory).
*/

#include "opengl.h"
#include "cache.h"
#include "error.h"

cch cch_create(size_t size, bool fast)
{
	cch c = (cch)malloc(sizeof(struct cch_str));
	if (c) {
		c->fast = fast;
		c->fastBase = NULL;
		c->base = c->rover = NULL;
		c->size = 0;
		cch_init(c, size);
	}
	return c;
}

void cch_destroy(cch c)
{
	if (c->fast && c->fastBase)
		globalGL.wglFreeMemoryNV(c->fastBase);
	if (c->base)
		free(c->base);
	free(c);
}

void cch_init(cch c, size_t size)
{
	if (!c)
		return;
	if (c->fast && c->fastBase)
		globalGL.wglFreeMemoryNV(c->fastBase);
	if (c->base) {
		cch_flush(c);
		free(c->base);
	}
	c->fastBase = NULL;
	c->base = c->rover = NULL;
	c->size = 0;
	if (size <= 0)
		return;

	/* round size up to page size */
	size = (size + 8191) & ~8191;

	c->base = (cchobj)malloc(size);
	if (!c->base)
		return;
	c->size = size;

	c->rover = c->base;
	c->base->next = NULL;
	c->base->owner = NULL;
	c->base->size = c->size;

	if (c->fast && globalGL.supportsVARFence) {
		c->fastBase = (unsigned char*)globalGL.wglAllocateMemoryNV(
							size, 0.0f, 0.0f, 1.0f);
		if (!c->fastBase)
			c->fast = false;
	}
	else
		c->fast = false;
}

void cch_flush(cch c)
{
	cchobj obj;

	if (!c || !c->base)
		return;

	for (obj = c->base; obj; obj = obj->next) {
		if (c->fast) {
			if(!globalGL.glTestFenceNV(obj->fence))
				globalGL.glFinishFenceNV(obj->fence);
			globalGL.glDeleteFencesNV(1, &(obj->fence));
		}
		if (obj->owner)
			*obj->owner = NULL;
	}

	c->rover = c->base;
	c->base->next = NULL;
	c->base->owner = NULL;
	c->base->size = c->size;
}

void cch_initFrame(cch c)
{
	c->wrapped = c->thrashed = false;
	c->initRover = c->rover;
}

cchobj cch_alloc(cch c, size_t size, cchobj *owner)
{
	cchobj obj;
	bool wrapped_this_time;

	/* Allow for size of cchobj_str */
	size += sizeof(struct cchobj_str);

	/* 4 byte align */
	size = (size + 3) & ~3;

	if (size > c->size) {
		err_report("cch_alloc: %i > cache size of %i", size, c->size);
		return NULL;
	}

	/* If there is not size bytes after the rover, reset to the cache base */
	wrapped_this_time = false;
	if (!c->rover)
		c->rover = c->base;
	else if (cch_free(c) < size) {
		wrapped_this_time = true;
		c->rover = c->base;
	}

	/* Collect and free objects until the rover block is large enough */
	obj = c->rover;
	if (c->fast) {
		if(!globalGL.glTestFenceNV(c->rover->fence))
			globalGL.glFinishFenceNV(c->rover->fence);
		globalGL.glDeleteFencesNV(1, &(c->rover->fence));
	}
	if (c->rover->owner)
		*(c->rover->owner) = NULL;
	while (obj->size < size) {
		/* Free another object */
		c->rover = c->rover->next;
		if (c->fast) {
			if(!globalGL.glTestFenceNV(c->rover->fence))
				globalGL.glFinishFenceNV(c->rover->fence);
			globalGL.glDeleteFencesNV(1, &(c->rover->fence));
		}
		if (c->rover->owner)
			*(c->rover->owner) = NULL;
		obj->size += c->rover->size;
		obj->next = c->rover->next;
	}

	/* Create a fragment out of any leftovers */
	if (obj->size - size > 63) {
		c->rover = (cchobj)(((unsigned char*)obj) + size);
		c->rover->size = obj->size - size;
		c->rover->next = obj->next;
		c->rover->owner = NULL;
		obj->next = c->rover;
		obj->size = size;
	}
	else
		c->rover = obj->next;

	*owner = obj;
	obj->owner = owner;
	if (c->fast)
		globalGL.glGenFencesNV(1, &(obj->fence));

	if (c->wrapped) {
		if (wrapped_this_time || (c->rover >= c->initRover))
			c->thrashed = true;
	}
	else if (wrapped_this_time)
		c->wrapped = true;

	if (c->fast)
		obj->data = 
			(((unsigned char*)(obj + 1)) - (unsigned char*)c->base) + c->fastBase;
	else
		obj->data = (unsigned char*)(obj + 1);

	return obj;
}

void *cch_malloc(cch c, size_t size, cchobj *owner)
{
	cchobj obj = cch_alloc(c, size, owner);
	if (obj)
		return obj->data;
	return NULL;
}

void cch_dump(cch c)
{
	cchobj obj;
	printf("cch_dump cache size %i\n", c->size);
	for (obj = c->base; obj; obj = obj->next) {
		if (obj == c->rover)
			printf("ROVER:\n");
		printf("%p : %i bytes\n",obj, obj->size);
	}
	fflush(stdout);
}
