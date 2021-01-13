/*
 * OBJECT.C:
 *
 *	Routines that deal with objects and items.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

int check_event_obj(crt_ptr)
creature *crt_ptr;
{
    otag *op, *op2;
    if(!crt_ptr->first_obj) return 0;
    op=crt_ptr->first_obj;
    while(1) {
        if(F_ISSET(op->obj, OEVENT)) return 1;
        if(op->obj->questnum) return 1;

        if(op->obj->first_obj) {
            op2=op->obj->first_obj;
            while(1) {
                if(F_ISSET(op2->obj, OEVENT)) return 1;
                if(op2->obj->questnum) return 1;
                if(!op2->next_tag) break;
                op2=op2->next_tag;
            }
        }    
        if(!op->next_tag) return 0;
        op=op->next_tag;
    } 
}

/**********************************************************************/
/*				add_obj_obj			      */
/**********************************************************************/

/* This function adds the object pointed to by the first parameter to  */
/* the contents of the object pointed to by the second parameter.  It  */
/* is done alphabetically.					       */

void add_obj_obj(obj_ptr1, obj_ptr2)
object	*obj_ptr1;
object	*obj_ptr2;
{
	otag	*op, *temp, *prev;

	obj_ptr1->parent_obj = obj_ptr2;
	obj_ptr1->parent_crt = 0;
	obj_ptr1->parent_rom = 0;

	op = (otag *)malloc(sizeof(otag));
	if(!op)
		merror("add_obj_obj", FATAL);
	op->obj = obj_ptr1;
	op->next_tag = 0;

	if(!obj_ptr2->first_obj) {
		obj_ptr2->first_obj = op;
		return;
	}

	temp = obj_ptr2->first_obj;
	if(strcmp(temp->obj->name, obj_ptr1->name) > 0 ||
	   (!strcmp(temp->obj->name, obj_ptr1->name) &&
	   temp->obj->adjustment > obj_ptr1->adjustment)) {
		op->next_tag = temp;
		obj_ptr2->first_obj = op;
		return;
	}

	while(temp) {
		if(strcmp(temp->obj->name, obj_ptr1->name) > 0 ||
		   (!strcmp(temp->obj->name, obj_ptr1->name) &&
		   temp->obj->adjustment > obj_ptr1->adjustment))
			break;
		prev = temp;
		temp = temp->next_tag;
	}
	op->next_tag = prev->next_tag;
	prev->next_tag = op;

}

/**********************************************************************/
/*				del_obj_obj			      */
/**********************************************************************/

/* This function removes the object pointed to by the first parameter */
/* from the object pointed to by the second.			      */

void del_obj_obj(obj_ptr1, obj_ptr2)
object	*obj_ptr1;
object	*obj_ptr2;
{
	otag 	*temp, *prev;

	obj_ptr1->parent_crt = 0;
	if(obj_ptr2->first_obj->obj == obj_ptr1) {
		temp = obj_ptr2->first_obj->next_tag;
		free(obj_ptr2->first_obj);
		obj_ptr2->first_obj = temp;
		return;
	}

	prev = obj_ptr2->first_obj;
	temp = prev->next_tag;
	while(temp) {
		if(temp->obj == obj_ptr1) {
			prev->next_tag = temp->next_tag;
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next_tag;
	}
}

/**********************************************************************/
/*				find_obj			      */
/**********************************************************************/

/* This function is used to search through a room's, creature's or	*/
/* object's inventory in order to look for a particular item.  The	*/
/* first parameter contains a pointer to the creature who is doing the	*/
/* search.  The second is a pointer to the first object tag in the 	*/
/* list to be searched.  The third parameter is a string that contains	*/
/* the name of the object being searched for.  The last parameter 	*/
/* contains the version of the string being searched for.		*/

object *find_obj(ply_ptr, first_ot, str, val)
creature	*ply_ptr;
otag		*first_ot;
char		*str;
int		val;
{
	otag	*op;
	int	match=0, found=0;

	op = first_ot;
	while(op) {
		if(EQUAL(op->obj, str) &&
		   (F_ISSET(ply_ptr, PDINVI) ?
		   1:!F_ISSET(op->obj, OINVIS))) {
			match++;
			if(match == val) {
				found = 1;
				break;
			}
		}
		op = op->next_tag;
	}

	if(found)
		return(op->obj);
	else
		return(0);
}

/**********************************************************************/
/*				list_obj			      */
/**********************************************************************/

/* This function produces a string which lists all the objects in a	*/
/* player's, room's or object's inventory.  The first parameter holds	*/
/* a pointer to the string which will be produced.  The second is a	*/
/* pointer to the player who is having the list produced.  The last	*/
/* is a pointer to the first object tag in the list of items that	*/
/* it being listed.							*/

int list_obj(str, ply_ptr, first_otag)
char		*str;
creature	*ply_ptr;
otag		*first_otag;
{
	otag	*op;
	int	m, n=0, flags=0;

	if(F_ISSET(ply_ptr, PDINVI))
		flags |= INV;
	if(F_ISSET(ply_ptr, PDMAGI))
		flags |= MAG;

	str[0] = 0;
	op = first_otag;
	while(op && strlen(str) < 1970) {
		if((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) &&
		   !F_ISSET(op->obj, OHIDDN) && !F_ISSET(op->obj, OSCENE)) {
			m=1;
			while(op->next_tag) {
				if(!strcmp(op->next_tag->obj->name,
					   op->obj->name) &&
				   (op->next_tag->obj->adjustment ==
				   op->obj->adjustment ||
				   !F_ISSET(ply_ptr, PDMAGI)) &&
				   (F_ISSET(ply_ptr, PDINVI) ?
				   1:!F_ISSET(op->next_tag->obj, OINVIS)) &&
				   !F_ISSET(op->next_tag->obj, OHIDDN) &&
				   !F_ISSET(op->next_tag->obj, OSCENE)) {
					m++;
					op = op->next_tag;
				}
				else
					break;
			}
			strcat(str, obj_str(op->obj, m, flags));

			strcat(str, ", ");
			n++;
		}
		op = op->next_tag;
	}
	if(n) {
		str[strlen(str)-2] = 0;
		return(1);
	}
	else
		return(0);

}

/**********************************************************************/
/*				weight_obj			      */
/**********************************************************************/

/* This function computes the total amount of weight of an object and */
/* all its contents.						      */

int weight_obj(obj_ptr)
object	*obj_ptr;
{
	int	n;
	otag	*op;

	n = obj_ptr->weight;
	op = obj_ptr->first_obj;
	while(op) {
		if(!F_ISSET(op->obj, OWTLES))
			n += weight_obj(op->obj);
		op = op->next_tag;
	}

	return(n);

}


/************************************************************************/
/*				rand_enchant				*/
/************************************************************************/

/* This function randomly enchants an object if its random-enchant flag */
/* is set.								*/

void rand_enchant(obj_ptr)
object	*obj_ptr;
{
	char m;

	m = mrand(1,100);
	if(m > 98) {
		F_SET(obj_ptr, OENCHA);
		obj_ptr->adjustment = 3;
		obj_ptr->pdice += 3;
	}
	else if(m > 90) {
		F_SET(obj_ptr, OENCHA);
		obj_ptr->adjustment = 2;
		obj_ptr->pdice += 2;
	}
	else if(m > 50) {
		F_SET(obj_ptr, OENCHA);
		obj_ptr->adjustment = 1;
		obj_ptr->pdice += 1;
	}

	obj_ptr->pdice = MAX(obj_ptr->pdice, obj_ptr->adjustment);
}
