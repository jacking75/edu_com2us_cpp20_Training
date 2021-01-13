/*
 * DM3.C:
 *
 *	DM functions
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*				dm_set				      */
/**********************************************************************/

/* This function allows a DM to set a variable within a currently */
/* existing data structure in the game.				  */

int dm_set(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	int	fd;

	fd = ply_ptr->fd;

	if(ply_ptr->class < DM)
		return(PROMPT);

	if(cmnd->num < 2) {
		print(fd, "Set what?\n");
		return;
	}

	switch(low(cmnd->str[1][0])) {
	case 'x': return(dm_set_ext(ply_ptr, cmnd));
	case 'r': return(dm_set_rom(ply_ptr, cmnd));
	case 'c':
	case 'p':
	case 'm': return(dm_set_crt(ply_ptr, cmnd));
	case 'i':
	case 'o': return(dm_set_obj(ply_ptr, cmnd));
	default: print(fd, "Invalid option.  *set <x|r|c|o> <options>\n");
		 return(0);
	}

}

/**********************************************************************/
/*				dm_set_rom			      */
/**********************************************************************/

/* This function allows a DM to set a characteristic of a room. */

int dm_set_rom(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr;
	int	num, fd;

	rom_ptr = ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	if(cmnd->num < 3) {
		print(fd, "Syntax: *set r [trf] [<value>]\n");
		return(0);
	}

	switch(low(cmnd->str[2][0])) {
	case 't':
		rom_ptr->traffic = cmnd->val[2];
		print(fd, "Traffic is now %d%%.\n", cmnd->val[2]);
		return(0);
	case 's':
		rom_ptr->special = cmnd->val[2];
		print(fd, "Special is now %d.\n", cmnd->val[2]);
		return(0);
	case 'r':
		num = atoi(&cmnd->str[2][1]);
		if(num < 1 || num > 10) return(PROMPT);
		rom_ptr->random[num-1] = cmnd->val[2];
		print(fd, "Random #%d is now %d.\n", num, cmnd->val[2]);
		return(0);
	case 'f':
		num = cmnd->val[2];
		if(num < 1 || num > 64) return(PROMPT);
		if(F_ISSET(rom_ptr, num-1)) {
			F_CLR(rom_ptr, num-1);
			print(fd, "Room flag #%d off.\n", num);
		}
		else {
			F_SET(rom_ptr, num-1);	
			print(fd, "Room flag #%d on.\n", num);
		}
		return(0);
	case 'b':
		if (low(cmnd->str[2][1]) == 'l') {
			rom_ptr->lolevel = (char) cmnd->val[2];
			print(fd, "Low level boundary %d\n", cmnd->val[2]);
		}
		else if (low(cmnd->str[2][1]) == 'h') {
			rom_ptr->hilevel = (char) cmnd->val[2];
			print(fd, "Upper level boundary %d\n", cmnd->val[2]);
		}
		return(0);
	case 'x':
		
		rom_ptr->trap = (short) cmnd->val[2];
		print(fd, "Room has trap #%d set.", cmnd->val[2]);
		return(0);
	default:
		print(fd, "Invalid option.\n");
		return(0);
	}
}

/**********************************************************************/
/*				dm_set_crt			      */
/**********************************************************************/

/* This function allows a DM to set a defining characteristic of */
/* a creature or player						 */

int dm_set_crt(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	creature	*crt_ptr;
	int		num, fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 4) {
		print(fd, "Syntax: *set c <name> <a|con|c|dex|e|f|g|hm|h|int|l|mm|m|\n                       pie|p#|r#|str> [<value>]\n");
		return(0);
	}

	cmnd->str[2][0] = up(cmnd->str[2][0]);
	crt_ptr = find_who(cmnd->str[2]);
	if(!crt_ptr || (ply_ptr->class<SUB_DM&& F_ISSET(crt_ptr, PDMINV)))
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				   cmnd->str[2], cmnd->val[2]);
	if(!crt_ptr) {
		cmnd->str[2][0] = low(cmnd->str[2][0]);
		crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
				   cmnd->str[2], cmnd->val[2]);
	}

	if(!crt_ptr) {
		print(fd, "Creature not found.\n");
		return(0);
	}

	switch(low(cmnd->str[3][0])) {
	case 'a':
		if (!strcmp(cmnd->str[3], "ar") && 
			crt_ptr->type == MONSTER){
			crt_ptr->armor = 100-cmnd->val[3];
			return(PROMPT);
		}
		crt_ptr->alignment = cmnd->val[3];
		print(fd, "Alignment set.\n");
		return(PROMPT);
	case 'c':
		if(!strcmp(cmnd->str[3], "con")) {
			crt_ptr->constitution = cmnd->val[3];
			return(PROMPT);
		}

		if(ply_ptr->class < DM)
                return(PROMPT);
				
		crt_ptr->class = cmnd->val[3];
		if (crt_ptr->type == PLAYER && cmnd->val[3] == DM)
        if(strcmp(crt_ptr->name, DMNAME) &&
           strcmp(crt_ptr->name, DMNAME2) && strcmp(crt_ptr->name, DMNAME3) &&
           strcmp(crt_ptr->name, DMNAME4) && strcmp(crt_ptr->name, DMNAME5) &&
           strcmp(crt_ptr->name, DMNAME6) && strcmp(crt_ptr->name, DMNAME7))
                crt_ptr->class = SUB_DM;      
		print(fd, "Class set.\n");
		return(PROMPT);
	case 'd':
		if(!strcmp(cmnd->str[3], "dex")) {
			crt_ptr->dexterity = cmnd->val[3];
			print(fd, "Dexterity set.\n");
			return(PROMPT);
		} 
		else if (!strcmp(cmnd->str[3], "dn") && 
			crt_ptr->type == MONSTER){
			crt_ptr->ndice = cmnd->val[3];
			print(fd, "Number of dice set.\n");
			return(PROMPT);
		}
		else if (!strcmp(cmnd->str[3], "ds") && 
			crt_ptr->type == MONSTER){
			crt_ptr->sdice = cmnd->val[3];
			print(fd, "Sides of dice set.\n");
			return(PROMPT);
		}
		else if (!strcmp(cmnd->str[3], "dp") && 
			crt_ptr->type == MONSTER){
			crt_ptr->pdice = cmnd->val[3];
			print(fd, "Plus on dice set.\n");
			return(PROMPT);
		}
		break;
	case 'e':
		crt_ptr->experience = cmnd->val[3];
		print(fd, "%M has %ld experience.\n", crt_ptr, cmnd->val[3]);
		return(0);
	case 'f':
		num = cmnd->val[3];
		if(num < 1 || num > 64) return(PROMPT);
		if(F_ISSET(crt_ptr, num-1)) {
			F_CLR(crt_ptr, num-1);
			print(fd, "%M's flag #%d off.\n", crt_ptr, num);
		}
		else {
			F_SET(crt_ptr, num-1);
			print(fd, "%M's flag #%d on.\n", crt_ptr, num);
		}
		return(0);
	case 'g':
		crt_ptr->gold = cmnd->val[3];
		print(fd, "%M has %ld gold.\n", crt_ptr, cmnd->val[3]);
		return(0);
	case 'h':
		if(cmnd->str[3][1] == 'm'){
			crt_ptr->hpmax = cmnd->val[3];
			print(fd, "Hits max set.\n");
		}
		else {
			crt_ptr->hpcur = cmnd->val[3];
			print(fd, "Hits set.\n");
		}
		return(PROMPT);
	case 'i':
		if(!strcmp(cmnd->str[3], "int")) {
			crt_ptr->intelligence = cmnd->val[3];
			print(fd, "Intellegence set.\n");
			return(PROMPT);
		}
		break;
	case 'l':
		crt_ptr->level = cmnd->val[3];
		print(fd, "Level set.\n");
		return(PROMPT);
	case 'm':
		if(cmnd->str[3][1] == 'm'){
			crt_ptr->mpmax = cmnd->val[3];
			print(fd, "Magic pts max set.\n");
		}
		else {
			crt_ptr->mpcur = cmnd->val[3];
			print(fd, "Magic pts set.\n");
		}
		return(PROMPT);
	case 'p':
		if(!strcmp(cmnd->str[3], "pie")) {
			crt_ptr->piety = cmnd->val[3];
			print(fd, "Piety set.\n");
			return(PROMPT);
		}
		num = atoi(&cmnd->str[3][1]);
		if(num < 0 || num > 4) return(0);
		crt_ptr->proficiency[num] = cmnd->val[3];
		print(fd, "%M given %d shots in prof#%d.\n", crt_ptr, 
		      cmnd->val[3], num);
		return(0);
	case 'r':
		if(!cmnd->str[3][1]) {
			crt_ptr->race = cmnd->val[3];
			print(fd, "Race set.\n");
			return(PROMPT);
		}
		num = atoi(&cmnd->str[3][1]);
		if(num < 0 || num > 3) return(PROMPT);
		crt_ptr->realm[num] = cmnd->val[3];
		print(fd, "%M given %d shots in realm#%d.\n", crt_ptr,
			cmnd->val[3], num);
		return(0);
	case 's':
		if(!strcmp(cmnd->str[3], "str")) {
			crt_ptr->strength = cmnd->val[3];
			print(fd, "Strength set.\n");
			return(PROMPT);
		}
		else if(!strcmp(cmnd->str[3], "sp") && crt_ptr->type == PLAYER) {
			crt_ptr->daily[DL_MARRI].max = cmnd->val[3];
			print(fd, "special num set.\n");
			return(PROMPT);
		}
		break;
	case 't':
		if (!strcmp(cmnd->str[3], "thac") && 
			crt_ptr->type == MONSTER){
			crt_ptr->thaco = cmnd->val[3];
			print(fd, "THAC0 set.\n");
			return(PROMPT);
		}
		break;
	}
	print(fd, "Invalid option.\n");
	return(0);
}

/*********************************************************************/
/*********************************************************************/

int dm_set_ext(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room	*rom_ptr, *rom_ptr2;
	int	num, fd;

	fd = ply_ptr->fd;

	if (low(cmnd->str[1][1]) == 'f'){
		dm_set_xflg(ply_ptr, cmnd);
		return(0);
	}
	if(cmnd->num < 3) {
		print(fd, "Syntax: *set [#] x <name> <#> [. or name]\n");
		return(0);
	}

	if(cmnd->val[1] == 1)
		rom_ptr = ply_ptr->parent_rom;
	else
		if(load_rom(cmnd->val[1], &rom_ptr) < 0) {
			print(fd, "Room %d does not exist.\n", cmnd->val[1]);
			return(0);
		}

	cmnd->str[2][24] = 0;
	expand_exit_name(cmnd->str[2]);

	if(cmnd->val[2] == 1) {
		print(fd, "Link exit to which room?\n");
		return(0);
	}
	else if(cmnd->val[2] == 0) {
		if(del_exit(&rom_ptr, cmnd->str[2]))
			print(fd, "Exit %s deleted.\n", cmnd->str[2]);
		else
			print(fd, "Exit %s not found.\n", cmnd->str[2]);
	}
	else if(cmnd->num > 3) {
		if(*cmnd->str[3] == '.')
			if(!opposite_exit_name(cmnd->str[2], cmnd->str[3]))
				strcpy(cmnd->str[3], cmnd->str[2]);
		link_rom(&rom_ptr, cmnd->val[2], cmnd->str[2]);
		if(load_rom(cmnd->val[2], &rom_ptr2) < 0)
			print(fd, "Room %d does not exist.\n", cmnd->val[1]);
		else {
			link_rom(&rom_ptr2, rom_ptr->rom_num, cmnd->str[3]);
			resave_rom(rom_ptr2->rom_num);
		}

		print(fd, "Room #%d linked to room #%d in %s direction, both ways.\n", rom_ptr->rom_num, cmnd->val[2], cmnd->str[2]);
	}
	else {
		link_rom(&rom_ptr, cmnd->val[2], cmnd->str[2]);
		print(fd, "Room #%d linked to room #%d in %s direction.\n",
			rom_ptr->rom_num, cmnd->val[2], cmnd->str[2]);
	}

	resave_rom(rom_ptr->rom_num);

	return(0);
}

/************************************************************************/
/*				dm_set_obj				*/
/************************************************************************/

int dm_set_obj(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	room		*rom_ptr;
	creature	*crt_ptr;
	object		*obj_ptr;
	int		n, fd, match=0;
	long		num;
	char		flags[25];

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(cmnd->num < 4) {
		print(fd, "Syntax: *set o <name> [<crt>] <ad|ar|dn|ds|dp|f|m|sm|s|t|v|wg|wr> [<value>]\n");
		return(0);
	}

	if(cmnd->num == 4) {
		strcpy(flags, cmnd->str[3]);
		num = cmnd->val[3];
		obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[2],
			cmnd->val[2]);
		if(!obj_ptr)
			obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				cmnd->str[2], cmnd->val[2]);
		if(!obj_ptr) {
			print(fd, "Object not found.\n");
			return(0);
		}
	}

	else if(cmnd->num == 5) {
		strcpy(flags, cmnd->str[4]);
		num = cmnd->val[4];
		cmnd->str[3][0] = up(cmnd->str[3][0]);
		crt_ptr = find_who(cmnd->str[3]);
		if(!crt_ptr) {
			cmnd->str[3][0] = low(cmnd->str[3][0]);
			crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
				cmnd->str[3], cmnd->val[3]);
		}
		if(!crt_ptr) {
			cmnd->str[3][0] = up(cmnd->str[3][0]);
			crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
				cmnd->str[3], cmnd->val[3]);
		}
		if(!crt_ptr) {
			print(fd, "Creature not found.\n");
			return(0);
		}

		obj_ptr = find_obj(ply_ptr, crt_ptr->first_obj,
			cmnd->str[2], cmnd->val[2]);
		if(!obj_ptr || !cmnd->val[2]) {
			for(n=0; n<MAXWEAR; n++) {
				if(!crt_ptr->ready[n]) continue;
				if(EQUAL(crt_ptr->ready[n], cmnd->str[2]))
					match++;
				else continue;
				if(cmnd->val[1] == match || !cmnd->val[2]) {
					obj_ptr = crt_ptr->ready[n];
					break;
				}
			}
		}
		if(!obj_ptr) {
			print(fd, "Object not found.\n");
			return(0);
		}
	}

	switch(flags[0]) {
	case 'a':
		switch(flags[1]) {
		case 'd': obj_ptr->adjustment = num; print(fd, "Adjustment set.\n"); return(PROMPT);
		case 'r': obj_ptr->armor = num; print(fd, "Armor set.\n");return(PROMPT);
		}
		break;
	case 'd':
		switch(flags[1]) {
		case 'n': obj_ptr->ndice = num; print(fd, "Dice # set.\n");return(PROMPT);
		case 's': obj_ptr->sdice = num; print(fd, "Dice sides set.\n");return(PROMPT);
		case 'p': obj_ptr->pdice = num; print(fd, "Dice plus set.\n");return(PROMPT);
		}
		break;
	case 'f':
		if(num < 1 || num > 64) return(PROMPT);
		if(F_ISSET(obj_ptr, num-1)) {
			F_CLR(obj_ptr, num-1);
			print(fd, "%I's flag #%d off.\n", obj_ptr, num);
		}
		else {
			F_SET(obj_ptr, num-1);
			print(fd, "%I's flag #%d on.\n", obj_ptr, num);
		}
		return(0);
	case 'm':
		obj_ptr->magicpower = num;
		print(fd, "Magic power set.\n");
		return(PROMPT);
	case 's':
		if(flags[1] == 'm'){
            if (num > 1000){
                    print(fd, "당신은 비리를 저지를려구요?!! 뽀뽀 백.\n");
            }
            else {
                    obj_ptr->shotsmax = num;
                    print(fd, "Max shots set.\n");
            }         
		}
		else {
            if (num > 1000){
                    print(fd, "당신은 비리를 저지를려구요?!! 뽀뽀 백.\n");
            }
            else {
		  	         obj_ptr->shotscur = num;
			         print(fd, "Current shots set.\n");
            }         
		}
		return(PROMPT);
	case 't':
		if ((num<0 || num>4) && ply_ptr->class<DM) return(PROMPT);
		switch(num) {
		case SHARP: obj_ptr->type = SHARP; print(fd, "Object is a sharp weapon.\n");return(PROMPT);
		case THRUST: obj_ptr->type = THRUST; print(fd, "Object is a thrust weapon.\n");return(PROMPT);
		case BLUNT: obj_ptr->type = BLUNT; print(fd, "Object is a blunt weapon.\n");return(PROMPT);
		case POLE: obj_ptr->type = POLE; print(fd, "Object is a pole weapon.\n");return(PROMPT);
		case MISSILE: obj_ptr->type = MISSILE; print(fd, "Object is a missile weapon.\n");return(PROMPT);
		}
		break;
	case 'v':
		obj_ptr->value = num;
		print(fd, "Value set.\n");
		return(PROMPT);
	case 'w':
		switch(flags[1]) {
		case 'g': obj_ptr->weight = num; print(fd, "Weight set.\n");return(PROMPT);
		case 'r': obj_ptr->wearflag = num; print(fd, "Wear location set.\n");return(PROMPT);
		}
		break;
	}

	print(fd, "Invalid option.\n");
	return(0);
}


/**********************************************************************/
/*                       dm_set_xflg                                  */
/**********************************************************************/

/* This function allows a DM to set a characteristic of a exit. */

int dm_set_xflg(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    xtag    *xp;
    int     num, fd;
    char    found=0;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(cmnd->num < 3) {
        print(fd, "Syntax: *set xf <exit> <value>\n");
        return(0);
    }

    xp = rom_ptr->first_ext;
        while(xp) {
                if(!strcmp(xp->ext->name, cmnd->str[2]) && 
                        !F_ISSET(xp->ext,XNOSEE)){
                        found = 1;
                        break;
                }
                xp = xp->next_tag;
        }
 
        if(!found) {
                print(fd, "Exit not found.\n");
                return(0);
        }
           
    num = cmnd->val[2];
     if(num < 1 || num > 32) return(PROMPT);
        if(F_ISSET(xp->ext, num-1)) {
            F_CLR(xp->ext, num-1);
            print(fd, "%s exit flag #%d off.\n", xp->ext->name, num);
        }
        else {
            F_SET(xp->ext, num-1);
            print(fd, "%s exit flag #%d on.\n", xp->ext->name, num);
        }

        return(0);   
}
/***********************************************************************/
/***********************************************************************/

void link_rom(rom_ptr, tonum, dir)
room	**rom_ptr;
short	tonum;
char	*dir;
{
	exit_	*ext;
	xtag	*xp, *prev = 0, *temp;

	xp = (*rom_ptr)->first_ext;

	while(xp) {
		ext = xp->ext;
		prev = xp;
		xp = xp->next_tag;
		if (!strcmp(ext->name, dir)) {
			strcpy(ext->name, dir);
			ext->room = tonum;
			return;
		}
	}

	temp = (xtag *)malloc(sizeof(xtag));
	ext = (exit_ *)malloc(sizeof(exit_));

	zero(ext, sizeof(exit_));
	strcpy(ext->name, dir);
	ext->room = tonum;

	temp->next_tag = 0;
	temp->ext = ext;

	if (prev)
		prev->next_tag = temp;
	else
		(*rom_ptr)->first_ext = temp;
}

/*********************************************************************/
/*********************************************************************/
int del_exit(rom_ptr, dir)
room	**rom_ptr;
char	*dir;
{
	xtag	*xp, *prev = 0;

	xp = (*rom_ptr)->first_ext;

	while(xp) {
		if (!strcmp(xp->ext->name, dir)) {
			if(prev)
				prev->next_tag = xp->next_tag;
			else
				(*rom_ptr)->first_ext = xp->next_tag;

			free(xp->ext);
			free(xp);
			return(1);
		}
		prev = xp;
		xp = xp->next_tag;
	}

	return(0);
}

/*********************************************************************/
/*********************************************************************/
void expand_exit_name(name)
char *name;
{
	if(!strcmp(name, "n"))  strcpy(name, "north");
	else if(!strcmp(name, "s"))  strcpy(name, "south");
	else if(!strcmp(name, "e"))  strcpy(name, "east");
	else if(!strcmp(name, "w"))  strcpy(name, "west");
	else if(!strcmp(name, "sw"))  strcpy(name, "southwest");
	else if(!strcmp(name, "nw"))  strcpy(name, "northwest");
	else if(!strcmp(name, "se"))  strcpy(name, "southeast");
	else if(!strcmp(name, "ne"))  strcpy(name, "northeast");
	else if(!strcmp(name, "d")) strcpy(name, "door");
	else if(!strcmp(name, "o")) strcpy(name, "out");
	else if(!strcmp(name, "p")) strcpy(name, "passage");
	else if(!strcmp(name, "t")) strcpy(name, "trap door");
}

/*********************************************************************/
/*********************************************************************/
int opposite_exit_name(name, name2)
char *name, *name2;
{
	if(!strcmp(name, "south"))  strcpy(name2, "north");
	else if(!strcmp(name, "north"))  strcpy(name2, "south");
	else if(!strcmp(name, "west"))  strcpy(name2, "east");
	else if(!strcmp(name, "east"))  strcpy(name2, "west");
	else if(!strcmp(name, "northeast"))  strcpy(name2, "southwest");
	else if(!strcmp(name, "southeast"))  strcpy(name2, "northwest");
	else if(!strcmp(name, "northwest"))  strcpy(name2, "southeast");
	else if(!strcmp(name, "southwest"))  strcpy(name2, "northeast");
	else return(0);		/* search failed */

	return(1);		/* search successful */
}

/************************************************************************/
/*				dm_log					*/
/************************************************************************/

/* This function allows DMs to peruse the log file while in the game.	*/
/* If *log r is typed, then the log file is removed (i.e. cleared).	*/

int dm_log(ply_ptr, cmnd)
creature *ply_ptr;
cmd *cmnd;
{
	char fn[80], fm[80];

	if(ply_ptr->class < DM) 
		return(PROMPT);

	sprintf(fn, "%s/log", LOGPATH);
	sprintf(fm, "%s/log_fl", LOGPATH);

	if(cmnd->num == 2 && !strcmp(cmnd->str[1], "r")) {
		unlink(fn);
		print(ply_ptr->fd, "Log파일을 삭제했습니다.\n");
		return(0);
	}
	if(cmnd->num == 2 && !strcmp(cmnd->str[1], "f")) { 
		view_file(ply_ptr->fd, 1, fm);
		return(DOPROMPT);
	}
	else {
	view_file(ply_ptr->fd, 1, fn);
	return(DOPROMPT);
	}
}

/************************************************************************/
/*				dm_loadlockout				*/
/************************************************************************/

/* This function allows the DM to force the game to reload the lockout	*/
/* file.  Use this command when you have modified the lockout file and	*/
/* don't want to restart the game to put it into effect.		*/

int dm_loadlockout(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	if(ply_ptr-> class < DM)
		return(PROMPT);

	load_lockouts();
	print(ply_ptr->fd, "Lockout file read in.\n");
	return(0);
}

/************************************************************************/
/*				dm_finger				*/
/************************************************************************/

/* This command allows a DM to finger the machine that a player is 	*/
/* calling from.  If a second argument is passed, then finger will	*/
/* do a finger on the person named.  The DM can also request to 	*/
/* explicitly name the site he is fingering using the @ in front of	*/
/* the address.								*/

int dm_finger(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
#ifdef FINGERACCT
	char		addr[80];
	char		name[80];
	char		path[80];
	char		fdstr[5];
	creature	*crt_ptr;

	if(ply_ptr->class < SUB_DM)
		return(PROMPT);

	if(cmnd->num < 2) {
		print(ply_ptr->fd, "누구를 Finger검색 합니까?\n");
		return(0);
	}

	if(cmnd->str[1][0] == '@')
		strcpy(addr, &cmnd->str[1][1]);
	else {
		cmnd->str[1][0] = up(cmnd->str[1][0]);
		crt_ptr = find_who(cmnd->str[1]);
		if(!crt_ptr) {
			print(ply_ptr->fd, "완전한 이름을 사용하세요\n");
			return(0);
		}
		strcpy(addr, Ply[crt_ptr->fd].io->address);
	}

	if(!vfork())  {
		if(cmnd->num == 3)
			strncpy(name, cmnd->str[2], 79);
		else
			name[0] = 0;

		sprintf(path, "%s/finger", BINPATH);
		sprintf(fdstr, "%d", ply_ptr->fd);
		execl(path, "finger", fdstr, addr, name, 0);
		exit(0);
/* no idea why this is in here causes spawning frps should be exit */
		/*return(PROMPT); */
	}
	else {
		print(ply_ptr->fd, "Forking to %s.\n", addr);
		print(ply_ptr->fd, 
			"Output will arrive shortly.\n");
		return(0);
	}

#endif
}

/************************************************************************/
/*				dm_list					*/
/************************************************************************/

/* This function allows DMs to fork a "list" process.  List is the 	*/
/* utility program that allows one to nicely output a list of monsters,	*/
/* objects or rooms in the game.  There are many flags provided with	*/
/* the command, and they can be entered here in the same way that they	*/
/* are entered on the command line.  					*/

int dm_list(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	char		path[80];
	char		fdstr[10];
	int		i;

	if(ply_ptr->class < SUB_DM)
		return(PROMPT);

	if(cmnd->num < 2) {
		print(ply_ptr->fd, "무엇의 리스트를 봅니까?\n");
		return(0);
	}
	if(!vfork()) {
		sprintf(path, "%s/list", BINPATH);
		sprintf(fdstr, "-s%d", ply_ptr->fd);

		for(i=cmnd->num; i<COMMANDMAX; i++)
			cmnd->str[i][0] = 0;

		execl(path, "list", cmnd->str[1], fdstr, cmnd->str[2], 
			cmnd->str[3], cmnd->str[4], 0);
		exit(0);
	}
	else return(0);

}

/************************************************************************/
/*				dm_info					*/
/************************************************************************/

/* This command allows the DM to view various system statistics		*/

int dm_info(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	extern int	Rsize, Csize, Osize, Numplayers, Numwaiting;
	int		fd;
	extern short	Random_update_interval;

	if(ply_ptr->class < SUB_DM)
		return(PROMPT);

	fd = ply_ptr->fd;
	print(fd, "Internal Cache Queue Sizes:\n");
	print(fd, "   Rooms: %-5d   Monsters: %-5d   Objects: %-5d\n\n",
		Rsize, Csize, Osize);
	print(fd, "Wander update: %d\n", Random_update_interval);

	print(fd, "      Players: %d  Queued: %d\n\n", Numplayers, Numwaiting);

}
