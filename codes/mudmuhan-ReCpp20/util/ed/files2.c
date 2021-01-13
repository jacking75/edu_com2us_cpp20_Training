/*
 * FILES2.C:
 *
 *	Additional file routines, including memory management.
 *
 *	Copyright (C) 1991 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

typedef struct queue_tag {		/* General queue tag data struct */
	int			index;
	struct queue_tag	*next;
	struct queue_tag	*prev;
} qtag;

typedef struct rsparse {		/* Sparse pointer array for rooms */
	room 			*rom;
	qtag			*q_rom;
} rsparse;

typedef struct csparse {		/* Sparse pointer array for creatures */
	creature		*crt;
	qtag			*q_crt;
} csparse;

typedef struct osparse {		/* Sparse pointer array for objects */
	object			*obj;
	qtag			*q_obj;
} osparse;

static rsparse	Rom[RMAX];	/* Pointer array declared */
static csparse	Crt[CMAX];
static osparse	Obj[OMAX];

static qtag	*Romhead=0;	/* Queue header and tail pointers */
static qtag	*Romtail=0;
static qtag	*Crthead=0;
static qtag	*Crttail=0;
static qtag	*Objhead=0;
static qtag	*Objtail=0;

static int	Rsize=0;	/* Queue sizes */
static int	Csize=0;
static int	Osize=0;

/**********************************************************************/
/*				load_rom			      */
/**********************************************************************/

/* This function accepts a room number as its first argument and then   */
/* returns a dblpointer to that room's data in the second parameter.    */
/* If the room has already been loaded, the pointer is simply returned. */
/* Otherwise, the room is loaded into memory.  If a maximal number of   */
/* rooms is already in the memory, then the least recently used room    */
/* is stored back to disk, and the memory is freed.		        */

int load_rom(index, rom_ptr)
short	index;
room	**rom_ptr;
{
	int	fd;
	qtag	*qt;
	char	file[256];

	if(index >= RMAX || index < 0)
		return(-1);

	/* Check if room is already loaded, and if so return pointer */

	if(Rom[index].rom) {
		front_queue(&Rom[index].q_rom, &Romhead, &Romtail, &Rsize);
		*rom_ptr = Rom[index].rom;
	}

	/* Otherwise load the room, store rooms if queue size becomes */
	/* too big, and return a pointer to the newly loaded room     */

	else {
		sprintf(file, "%s/r%05d", ROOMPATH, index);
		fd = open(file, O_RDONLY, 0);
		if(fd < 0)
			return(-1);
		*rom_ptr = (room *)malloc(sizeof(room));
		if(!*rom_ptr)
			merror("load_rom", FATAL);
		if(read_rom(fd, *rom_ptr) < 0) {
			close(fd);
			return(-1);
		}
		close(fd);

		(*rom_ptr)->rom_num = index;

		qt = (qtag *)malloc(sizeof(qtag));
		if(!qt)
			merror("load_rom", FATAL);
		qt->index = index;
		Rom[index].rom = *rom_ptr;
		Rom[index].q_rom = qt;
		put_queue(&qt, &Romhead, &Romtail, &Rsize);

		while(Rsize > RQMAX) {

			pull_queue(&qt, &Romhead, &Romtail, &Rsize);
			if(Rom[qt->index].rom->first_ply) {
				put_queue(&qt, &Romhead, &Romtail, &Rsize);
				continue;
			}
			sprintf(file, "%s/r%05d", ROOMPATH, qt->index);
			unlink(file);
			fd = open(file, O_RDWR | O_CREAT, ACC);
			if(fd < 1)
				return(-1);
			if(!Rom[qt->index].rom)
				merror("load_rom", NONFATAL);
			if(write_rom(fd, Rom[qt->index].rom, PERMONLY) < 0) {
				close(fd);
				return(-1);
			}
			close(fd);
			free_rom(Rom[qt->index].rom);
			Rom[qt->index].rom = 0;
			free(qt);
		}
	}

	return(0);

}

int is_rom_loaded(num)
int	num;
{
	return(Rom[num].rom != 0);
}

/**********************************************************************/
/*				reload_rom			      */
/**********************************************************************/

/* This function reloads a room from disk, if it's already loaded.  This  */
/* allows you to make changes to a room, and then reload it, even if it's */
/* already in the memory room queue.					  */

int reload_rom(num)
int	num;
{
	room	*rom_ptr;
	ctag	*cp;
	otag	*op;
	char	file[80];
	int	fd;

	if(!Rom[num].rom)
		return(0);

	sprintf(file, "%s/r%05d", ROOMPATH, num);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0)
		return(-1);
	rom_ptr = (room *)malloc(sizeof(room));
	if(!rom_ptr)
		merror("reload_rom", FATAL);
	if(read_rom(fd, rom_ptr) < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	rom_ptr->first_ply = Rom[num].rom->first_ply;
	Rom[num].rom->first_ply = 0;

	if(!rom_ptr->first_mon) {
		rom_ptr->first_mon = Rom[num].rom->first_mon;
		Rom[num].rom->first_mon = 0;
	}

	if(!rom_ptr->first_obj) {
		rom_ptr->first_obj = Rom[num].rom->first_obj;
		Rom[num].rom->first_obj = 0;
	}

	free_rom(Rom[num].rom);
	Rom[num].rom = rom_ptr;

	cp = rom_ptr->first_ply;
	while(cp) {
		cp->crt->parent_rom = rom_ptr;
		cp = cp->next_tag;
	}

	cp = rom_ptr->first_mon;
	while(cp) {
		cp->crt->parent_rom = rom_ptr;
		cp = cp->next_tag;
	}

	op = rom_ptr->first_obj;
	while(op) {
		op->obj->parent_rom = rom_ptr;
		op = op->next_tag;
	}

	return(0);

}

/**********************************************************************/
/*				resave_rom			      */
/**********************************************************************/

/* This function saves an already-loaded room back to memory without */
/* altering its position on the queue.				     */

int resave_rom(num)
int	num;
{
	char	file[80];
	int	fd;

	if(!Rom[num].rom)
		return(0);

	sprintf(file, "%s/r%05d", ROOMPATH, num);
	unlink(file);
	fd = open(file, O_RDWR | O_CREAT, ACC);
	if(fd < 1)
		return(-1);
	if(!Rom[num].rom)
		merror("resave_rom", NONFATAL);
	if(write_rom(fd, Rom[num].rom, PERMONLY) < 0) {
		close(fd);
		return(-1);
	}
	close(fd);

	return(0);

}

/**********************************************************************/
/*				resave_all_rom			      */
/**********************************************************************/

/* This function saves all memory-resident rooms back to disk.  If the */
/* permonly parameter is non-zero, then only permanent items in those  */
/* rooms are saved back.					       */

void resave_all_rom(permonly)
int	permonly;
{
	qtag 	*qt;
	char	file[80];
	int	fd;

	qt = Romhead;
	while(qt) {
		if(!Rom[qt->index].rom) {
			qt = qt->next;
			continue;
		}

		sprintf(file, "%s/r%05d", ROOMPATH, qt->index);
		fd = open(file, O_RDWR | O_CREAT, ACC);
		if(fd < 1)
			return;
		if(write_rom(fd, Rom[qt->index].rom, permonly) < 0) {
			close(fd);
			return;
		}
		close(fd);
		qt = qt->next;
	}
}

/**********************************************************************/
/*				save_all_ply			      */
/**********************************************************************/

/* This function saves all players currently in memory.		      */

void save_all_ply()
{
	int i;

	for(i=2; i<Tablesize; i++) {
		if(Ply[i].ply && Ply[i].io && Ply[i].ply->name[0])
			savegame(Ply[i].ply, 0);
	}
}

/**********************************************************************/
/*				flush_rom			      */
/**********************************************************************/

/* This function flushes out the room queue and clears the room sparse */
/* pointer array, without saving anything to file.  It also clears all */
/* memory used by loaded rooms.  Call this function before leaving the */
/* program.							       */

void flush_rom()
{
	qtag *qt;

	while(1) {
		pull_queue(&qt, &Romhead, &Romtail, &Rsize);
		if(!qt) break;
		free_rom(Rom[qt->index].rom);
		Rom[qt->index].rom = 0;
		free(qt);
	}
}

/**********************************************************************/
/*				flush_crt			      */
/**********************************************************************/

/* This function flushes out the monster queue and clears the monster */
/* sparse pointer array without saving anything to file.  It also     */
/* clears all memory used by loaded creatures.  Call this function    */
/* before leaving the program.					      */

void flush_crt()
{
	qtag *qt;

	while(1) {
		pull_queue(&qt, &Crthead, &Crttail, &Csize);
		if(!qt) break;
		free_crt(Crt[qt->index].crt);
		Crt[qt->index].crt = 0;
		free(qt);
	}
}

/**********************************************************************/
/*				flush_obj			      */
/**********************************************************************/

/* This function flushes out the object queue and clears the object */
/* sparse pointer array without saving anything to file.  It also   */
/* clears all memory used by loaded objects.  Call this function    */
/* leaving the program.						    */

void flush_obj()
{
	qtag *qt;

	while(1) {
		pull_queue(&qt, &Objhead, &Objtail, &Osize);
		if(!qt) break;
		free_obj(Obj[qt->index].obj);
		Obj[qt->index].obj = 0;
		free(qt);
	}
}

/**********************************************************************/
/*				load_crt			      */
/**********************************************************************/

/* This function returns a pointer to the monster given by the index in */
/* the first parameter.  The pointer is returned in the second.  If the */
/* monster is already in memory, then a pointer is merely returned.     */
/* Otherwise, the monster is loaded into memory and a pointer is re-    */
/* turned.  If there are too many monsters in memory, then the least    */
/* recently used one is freed from memory.				*/

int load_crt(index, mon_ptr)
short		index;
creature	**mon_ptr;
{
	int	fd;
	long	n;
	qtag	*qt;
	char	file[256];

	if(index >= CMAX || index < 0)
		return(-1);

	/* Check if monster is already loaded, and if so return pointer */

	if(Crt[index].crt) {
		front_queue(&Crt[index].q_crt, &Crthead, &Crttail, &Csize);
		*mon_ptr = (creature *)malloc(sizeof(creature));
		**mon_ptr = *Crt[index].crt;
	}

	/* Otherwise load the monster, erase monsters if queue size          */
	/* becomes too big, and return a pointer to the newly loaded monster */

	else {
		sprintf(file, "%s/m%02d", MONPATH, index/MFILESIZE);
		fd = open(file, O_RDONLY, 0);
		if(fd < 0) {
			*mon_ptr = 0;
			return(-1);
		}
		*mon_ptr = (creature *)malloc(sizeof(creature));
		if(!*mon_ptr)
			merror("load_crt", FATAL);
		n = lseek(fd, (long)((index%MFILESIZE)*sizeof(creature)), 0);
		if(n < 0L) {
			free(*mon_ptr);
			close(fd);
			*mon_ptr = 0;
			return(-1);
		}
		n = read(fd, *mon_ptr, sizeof(creature));
		close(fd);
		if(n < sizeof(creature)) {
			free(*mon_ptr);
			*mon_ptr = 0;
			return(-1);
		}

		(*mon_ptr)->fd = -1;
		qt = (qtag *)malloc(sizeof(qtag));
		if(!qt)
			merror("load_crt", FATAL);
		qt->index = index;
		Crt[index].crt = (creature *)malloc(sizeof(creature));
		*Crt[index].crt = **mon_ptr;
		Crt[index].q_crt = qt;
		put_queue(&qt, &Crthead, &Crttail, &Csize);

		while(Csize > CQMAX) {
			pull_queue(&qt, &Crthead, &Crttail, &Csize);
			free_crt(Crt[qt->index].crt);
			Crt[qt->index].crt = 0;
			free(qt);
		}
	}

	(*mon_ptr)->lasttime[LT_HEALS].ltime = time(0);
	(*mon_ptr)->lasttime[LT_HEALS].interval = 60L;

	return(0);

}

/**********************************************************************/
/*				load_obj			      */
/**********************************************************************/

/* This function loads the object specified by the first parameter, and */
/* returns a pointer to it in the second parameter.  If the object has  */
/* already been loaded before, then a pointer is merely returned.       */
/* Otherwise, the object is loaded into memory and the pointer is       */
/* returned.  If there are too many objects in memory, then the least   */
/* recently used objects are freed from memory.				*/

int load_obj(index, obj_ptr)
short	index;
object	**obj_ptr;
{
	int	fd;
	long	n;
	qtag	*qt;
	char	file[256];

	if(index >= OMAX || index < 0)
		return(-1);

	/* Check if object is already loaded, and if so return pointer */

	if(Obj[index].obj) {
		front_queue(&Obj[index].q_obj, &Objhead, &Objtail, &Osize);
		*obj_ptr = (object *)malloc(sizeof(object));
		**obj_ptr = *Obj[index].obj;
	}

	/* Otherwise load the object, erase objects if queue size           */
	/* becomes too big, and return a pointer to the newly loaded object */

	else {
		sprintf(file, "%s/o%02d", OBJPATH, index/OFILESIZE);
		fd = open(file, O_RDONLY, 0);
		if(fd < 0)
			return(-1);
		*obj_ptr = (object *)malloc(sizeof(object));
		if(!*obj_ptr)
			merror("load_obj", FATAL);
		n = lseek(fd, (long)((index%OFILESIZE)*sizeof(object)), 0);
		if(n < 0L) {
			close(fd);
			return(-1);
		}
		n = read(fd, *obj_ptr, sizeof(object));
		close(fd);
		if(n < sizeof(object))
			return(-1);

		qt = (qtag *)malloc(sizeof(qtag));
		if(!qt)
			merror("load_obj", FATAL);
		qt->index = index;
		Obj[index].obj = (object *)malloc(sizeof(object));
		*Obj[index].obj = **obj_ptr;
		Obj[index].q_obj = qt;
		put_queue(&qt, &Objhead, &Objtail, &Osize);
		while(Osize > OQMAX) {
			pull_queue(&qt, &Objhead, &Objtail, &Osize);
			free_obj(Obj[qt->index].obj);
			Obj[qt->index].obj = 0;
			free(qt);
		}
	}

	return(0);

}

/***********************************************************************/
/*				save_ply			       */
/***********************************************************************/

/* This function saves the player specified by the string in the first */
/* parameter, and uses the player in the second parameter.	       */

int save_ply(str, ply_ptr)
char		*str;
creature	*ply_ptr;
{
	char	file[80];
	int	fd, n;
#ifdef COMPRESS
	char	*a_buf, *b_buf;
	int	size;
#endif

	sprintf(file, "%s/%s", PLAYERPATH, str);
	unlink(file);
	fd = open(file, O_RDWR | O_CREAT, ACC);
	if(fd < 0)
		return(-1);

#ifdef COMPRESS
	a_buf = (char *)malloc(80000);
	if(!a_buf) merror(FATAL, "Memory allocation");
	n = write_crt_to_mem(a_buf, ply_ptr, 0);
	if(n > 80000) merror(FATAL, "Player too large");
	b_buf = (char *)malloc(n);
	if(!b_buf) merror(FATAL, "Memory allocation");
	size = compress(a_buf, b_buf, n);
	n = write(fd, b_buf, size);
	free(a_buf);
	free(b_buf);
#else
	n = write_crt(fd, ply_ptr, 0);
	if(n < 0) {
		close(fd);
		return(-1);
	}
#endif

	close(fd);
	return(0);

}

/***********************************************************************/
/*				load_ply			       */
/***********************************************************************/

/* This function loads the player specified by the string in the first */
/* parameter, and returns the player in the second parameter.	       */

int load_ply(str, ply_ptr)
char		*str;
creature	**ply_ptr;
{
	char	file[80];
	int	fd, n;
#ifdef COMPRESS
	char	*a_buf, *b_buf;
	int	size;
#endif

	sprintf(file, "%s/%s", PLAYERPATH, str);
	fd = open(file, O_RDONLY, 0);
	if(fd < 0) 
		return(-1);

	*ply_ptr = (creature *)malloc(sizeof(creature));
	if(!*ply_ptr)
		merror("load_ply", FATAL);

#ifdef COMPRESS
	a_buf = (char *)malloc(30000);
	if(!a_buf) merror(FATAL, "Memory allocation");
	size = read(fd, a_buf, 30000);
	if(size >= 30000) merror(FATAL, "Player too large");
	if(size < 1) {
		close(fd);
		return(-1);
	}
	b_buf = (char *)malloc(80000);
	if(!b_buf) merror(FATAL, "Memory allocation");
	n = uncompress(a_buf, b_buf, size);
	if(n > 80000) merror(FATAL, "Player too large");
	n = read_crt_from_mem(b_buf, *ply_ptr, 0);
	free(a_buf);
	free(b_buf);
#else
	n = read_crt(fd, *ply_ptr);
	if(n < 0) {
		close(fd);
		return(-1);
	}
#endif

	close(fd);
	return(0);

}

/**********************************************************************/
/*				put_queue			      */
/**********************************************************************/

/* put_queue places the queue tag pointed to by the first paramater onto */
/* a queue whose head and tail tag pointers are the second and third     */
/* parameters.  If parameters 2 & 3 are 0, then a new queue is created.  */
/* The fourth parameter points to a queue size counter which is 	 */
/* incremented.							         */

void put_queue(qt, headptr, tailptr, sizeptr)
qtag	**qt;
qtag	**headptr;
qtag	**tailptr;
int	*sizeptr;
{
	*sizeptr = *sizeptr + 1;

	if(!*headptr) {
		*headptr = *qt;
		*tailptr = *qt;
		(*qt)->next = 0;
		(*qt)->prev = 0;
	}

	else {
		(*headptr)->prev = *qt;
		(*qt)->next = *headptr;
		(*qt)->prev = 0;
		*headptr = *qt;
	}
}

/**********************************************************************/
/*				pull_queue			      */
/**********************************************************************/

/* pull_queue removes the last queue tag on the queue specified by the */
/* second and third parameters and returns that tag in the first       */
/* parameter.  The fourth parameter points to a queue size counter     */
/* which is decremented.					       */

void pull_queue(qt, headptr, tailptr, sizeptr)
qtag 	**qt;
qtag	**headptr;
qtag	**tailptr;
int	*sizeptr;
{
	if(!*tailptr)
		*qt = 0;
	else {
		*sizeptr = *sizeptr - 1;
		*qt = *tailptr;
		if((*qt)->prev) {
			(*qt)->prev->next = 0;
			*tailptr = (*qt)->prev;
		}
		else {
			*headptr = 0;
			*tailptr = 0;
		}
	}
}

/**********************************************************************/
/*				front_queue			      */
/**********************************************************************/

/* front_queue removes the queue tag pointed to by the first parameter */
/* from the queue (specified by the second and third parameters) and   */
/* places it back at the head of the queue.  The fourth parameter is a */
/* pointer to a queue size counter, and it remains unchanged.          */

void front_queue(qt, headptr, tailptr, sizeptr)
qtag	**qt;
qtag	**headptr;
qtag	**tailptr;
int	*sizeptr;
{
	if((*qt)->prev) {
		((*qt)->prev)->next = (*qt)->next;
		if(*qt == *tailptr)
			*tailptr = (*qt)->prev;
	}
	if((*qt)->next) {
		((*qt)->next)->prev = (*qt)->prev;
		if(*qt == *headptr)
			*headptr = (*qt)->next;
	}
	if(!(*qt)->prev && !(*qt)->next) {
		*headptr = 0;
		*tailptr = 0;
	}
	(*qt)->next = 0;
	(*qt)->prev = 0;
	*sizeptr = *sizeptr - 1;

	put_queue(qt, headptr, tailptr, sizeptr);
}

