/*
 * FILES3.C:
 *
 *	File I/O Routines.
 *
 *	Copyright (C) 1991 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/************************************************************************/
/*				write_obj_to_mem			*/
/************************************************************************/

/* Save an object to a block of memory.					*/
/* This function recursively saves the items that are contained inside	*/
/* the object as well.  If perm_only != 0 then only permanent objects	*/
/* within the object are saved with it.  This function returns the	*/
/* number of bytes that were written.					*/

int write_obj_to_mem(buf, obj_ptr, perm_only)
char	*buf;
object 	*obj_ptr;
char 	perm_only;
{
	int 	n, cnt, cnt2=0, error=0;
	char	*bufstart;
	otag	*op;

	bufstart = buf;

	memcpy(buf, obj_ptr, sizeof(object));
	buf += sizeof(object);
	
	cnt = count_obj(obj_ptr, perm_only);
	memcpy(buf, &cnt, sizeof(int));
	buf += sizeof(int);

	if(cnt > 0) {
		op = obj_ptr->first_obj;
		while(op) {
			if(!perm_only || (perm_only && 
			   (F_ISSET(op->obj, OPERMT) || 
			   F_ISSET(op->obj, OPERM2)))) {
				if((n = write_obj_to_mem(buf, 
				    op->obj, perm_only)) < 0)
					error = 1;
				else
					buf += n;
				cnt2++;
			}
			op = op->next_tag;
		}
	}

	if(cnt != cnt2 || error)
		return(-1);
	else
		return(buf - bufstart);

}

/************************************************************************/
/*				write_crt_to_mem			*/
/************************************************************************/

/* Save a creature to memory.  This function 				*/
/* also saves all the items the creature is holding.  If perm_only != 0 */
/* then only those items which the creature is carrying that are	*/
/* permanent will be saved.						*/

int write_crt_to_mem(buf, crt_ptr, perm_only)
char		*buf;
creature 	*crt_ptr;
char 		perm_only;
{
	int 	n, cnt, cnt2=0, error=0;
	char	*bufstart;
	otag	*op;

	bufstart = buf;

	memcpy(buf, crt_ptr, sizeof(creature));
	buf += sizeof(creature);

	cnt = count_inv(crt_ptr, perm_only);
	memcpy(buf, &cnt, sizeof(int));
	buf += sizeof(int);

	if(cnt > 0) {
		op = crt_ptr->first_obj;
		while(op) {
			if(!perm_only || (perm_only && 
			   (F_ISSET(op->obj, OPERMT) || 
			   F_ISSET(op->obj, OPERM2)))) {
				if((n = write_obj_to_mem(buf, op->obj,
				    perm_only)) < 0)
					error = 1;
				else
					buf += n;
				cnt2++;
			}
			op = op->next_tag;
		}
	}

	if(cnt != cnt2 || error)
		return(-1);
	else
		return(buf - bufstart);
}

/************************************************************************/
/*				read_obj_from_mem			*/
/************************************************************************/

/* Loads the object from memory, returns the number of bytes read,	*/
/* and also loads every object which it might contain.  Returns -1 if	*/
/* there was an error.							*/

int read_obj_from_mem(buf, obj_ptr)
char	*buf;
object 	*obj_ptr;
{
	int 		n, cnt, error=0;
	char		*bufstart;
	otag		*op;
	otag		**prev;
	object 		*obj;

	bufstart = buf;

	memcpy(obj_ptr, buf, sizeof(object));
	buf += sizeof(object);

	obj_ptr->first_obj = 0;
	obj_ptr->parent_obj = 0;
	obj_ptr->parent_rom = 0;
	obj_ptr->parent_crt = 0;
	if(obj_ptr->shotscur > obj_ptr->shotsmax)
		obj_ptr->shotscur = obj_ptr->shotsmax;

	memcpy(&cnt, buf, sizeof(int));
	buf += sizeof(int);

	prev = &obj_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if((n = read_obj_from_mem(buf, obj)) < 0)
					error = 1;
				else
					buf += n;
				obj->parent_obj = obj_ptr;
				op->obj = obj;
				op->next_tag = 0;
				*prev = op;
				prev = &op->next_tag;
			}
			else
				merror("read_obj", FATAL);
		}
		else
			merror("read_obj", FATAL);
	}

	if(error)
		return(-1);
	else
		return(buf - bufstart);
}

/************************************************************************/
/*				read_crt_from_mem			*/
/************************************************************************/

/* Loads a creature from memory & returns bytes read.  The creature is	*/
/* loaded at the mem location specified by the second parameter.  In	*/
/* addition, all the creature's objects have memory allocated for them	*/
/* and are loaded as well.  Returns -1 on fail.				*/

int read_crt_from_mem(buf, crt_ptr)
char		*buf;
creature 	*crt_ptr;
{
	int 		n, cnt, error=0;
	char		*bufstart;
	otag		*op;
	otag		**prev;
	object 		*obj;

	bufstart = buf;

	memcpy(crt_ptr, buf, sizeof(creature));
	buf += sizeof(creature);

	crt_ptr->first_obj = 0;
	crt_ptr->first_fol = 0;
	crt_ptr->first_enm = 0;
	crt_ptr->parent_rom = 0;
	crt_ptr->following = 0;
	for(n=0; n<20; n++)
		crt_ptr->ready[n] = 0;
	if(crt_ptr->mpcur > crt_ptr->mpmax)
		crt_ptr->mpcur = crt_ptr->mpmax;
	if(crt_ptr->hpcur > crt_ptr->hpmax)
		crt_ptr->hpcur = crt_ptr->hpmax;

	memcpy(&cnt, buf, sizeof(int));
	buf += sizeof(int);

	prev = &crt_ptr->first_obj;
	while(cnt > 0) {
		cnt--;
		op = (otag *)malloc(sizeof(otag));
		if(op) {
			obj = (object *)malloc(sizeof(object));
			if(obj) {
				if((n = read_obj_from_mem(buf, obj)) < 0)
					error = 1;
				else
					buf += n;
				obj->parent_crt = crt_ptr;
				op->obj = obj;
				op->next_tag = 0;
				*prev = op;
				prev = &op->next_tag;
			}
			else
				merror("read_crt", FATAL);
		}
		else
			merror("read_crt", FATAL);
	}

	if(error)
		return(-1);
	else
		return(buf - bufstart);
}
