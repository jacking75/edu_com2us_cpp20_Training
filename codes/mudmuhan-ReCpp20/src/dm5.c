/*
 * DM5.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

#include <ctype.h>

/*******************************************************************/
/*                             dm_replace                          */
/*******************************************************************/
/* The dm_replace command allows a caretaker+ to search and replace *
 * selected word in a rooms description.  If the search word does   *
 * exists, an error is returned.  NOTE: the command format is       *
 * derived by the txt_parse function. The search pattern may only   *
 * one word, while the repalcement pattern may consist of any       *
 * number of characters, spaces, symbols. The line which the        *
 * repalcement occurs, wil not be reformated. */

int dm_replace(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room    *rom_ptr;
    int     rom, fd, i;
    char    *desc, *pattern, *replace, *tmp;
    char    domain =0;
    int     len1, len2, len3, len4;
    int     value;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < DM)
        return(PROMPT);

    txt_parse(cmnd->fullstr,&pattern,&value,&replace);


    if (!pattern || !replace) {
        print(fd,"syntax:*replace <pattern> <replacement>\n");
        return(0);
    }
    domain = 0;
    desc =  rom_ptr->short_desc;
    

    len1 = (desc) ? strlen(desc) : 0;
    len2 = (pattern) ? strlen(pattern) : 0;
    len3 = (replace) ? strlen(replace) : 0;
    len4 = len1 + len3 - len2;

    i=desc_search(desc, pattern,&value); 

    if (i== -1 && value){
        domain = 1;
        desc =  rom_ptr->long_desc;
        i=desc_search(desc, pattern,&value); 
        len1 = (desc) ? strlen(desc) : 0;
        len4 = len1 + len3 - len2;
    }

    if(i<0){
        print(fd, "Pattern not found.\n");
        return(0);
    }

    tmp = (char *)malloc(len4+1);
    if(!tmp)
           merror("dm_replace", FATAL);     

    memcpy(tmp,desc,i);
    memcpy(&tmp[i],replace,len3);
    memcpy(&tmp[i+len3],&desc[i+len2],len1-(i+len2));
    tmp[len4] = 0; 
    

    free(desc);

    if (domain == 0)
        rom_ptr->short_desc = tmp;
    else
        rom_ptr->long_desc = tmp; 
    
    return(0);
}
/*******************************************************************/
/*                             dm_delete                           */
/*******************************************************************/
/* The dm_pattern command allows a caretaker+ to search and pattern *
 * selected word in a rooms description.  If the search word does   *
 * exists, an error is returned.  NOTE: the command format is       *
 * derived by the txt_parse function. The search flag may only   *
 * one word, while the repalcement flag may consist of any       *
 * number of characters, spaces, symbols. The line which the        *
 * repalcement occurs, wil not be reformated. */

int dm_delete(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room    *rom_ptr;
    int     rom, fd, i;
    char    *desc, *flag, *pattern, *tmp;
    char    domain =0, dcase = 0;
    int     len1, len2;
    int     value;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < DM)
        return(PROMPT);

    txt_parse(cmnd->fullstr,&flag,&value,&pattern);

    if (!flag) {
        print(fd,"syntax:*delete [-PESLA] <delete_word>\n");
        return(0);
    }

    if (flag[0] == '-'){
        if (strlen(flag) > 1)
        switch(flag[1]) {
        case 'S':
        free(rom_ptr->short_desc);
        rom_ptr->short_desc = NULL;
        return (0);
            break;
        case 'L':
        free(rom_ptr->long_desc);
        rom_ptr->long_desc = NULL;
        return (0);
            break;
       case 'A':
        free(rom_ptr->short_desc);
        free(rom_ptr->long_desc);
        rom_ptr->short_desc = rom_ptr->long_desc = NULL;
        return (0);
            break;
       case 'E':
        dcase = 1;
        break;
       case 'P': case 'D':
        dcase = 0;
		if (!pattern){
            print(fd,"syntax:*delete [-PESLA] <delete_word>\n");
            return(0);
		}
        break;
       default:
            print(fd,"syntax:*delete [-PESLA] <delete_word>\n");
            return(0);
        break;
    }
    else {
           print(fd,"syntax:*delete [-PESLA] <delete_word>\n");
           return(0);
    }
    }
    else {
        tmp = flag;
        flag = pattern;
        pattern = tmp;
    }


/* search for word / phrase */
        domain = 0;
        desc =  rom_ptr->short_desc;

        len1 = (desc) ? strlen(desc) : 0;
        len2 = (pattern) ? strlen(pattern) : 0;

        i=desc_search(desc, pattern,&value); 

        if (i== -1 && value){
            domain = 1;
            desc =  rom_ptr->long_desc;
            i=desc_search(desc, pattern,&value); 
            len1 = (desc) ? strlen(desc) : 0;
        }

        if(i<0){
            print(fd, "Pattern not found.\n");
            return(0);
        }

/*delete word / phrase */   
    if (dcase == 1){
        tmp = (char *)malloc(i+1);
        if(!tmp)
            merror("dm_delete", FATAL);     

        memcpy(tmp,desc,i);
        tmp[i] = 0; 
    }
    else{
        tmp = (char *)malloc(len1-len2+1);
        if(!tmp)
            merror("dm_delete", FATAL);     

        memcpy(tmp,desc,i);
        memcpy(&tmp[i],&desc[i+len2],len1-(i+len2));
        tmp[len1-len2] = 0; 
    }

        free(desc);

        if (domain == 0)
            rom_ptr->short_desc = tmp;
        else
            rom_ptr->long_desc = tmp; 
       
    return(0);
}

/**************************************************************/
/*        desc_search                  */
/**************************************************************/
 
/* The desc_search function searches the given string (desc)   *
 * for the given pattern (pattern) and returns the location of *
 * the 'val' occurace of the pattern.  Note: the value of      *
 * 'val' will change to reflect to needed number of matchs.    *
 * This is done to allow to search several realted, but not    *
 * conencted strings for the pattern as if searching 1 string  */
 
int desc_search(desc,pattern,val)
char    *desc;
char    *pattern;
int *val;
{
    int i, j, len, len2;
    char    c, match = 0, found= 0;
 
    if(!desc || !pattern)
        return (-1);
    len = strlen(pattern); 
    len2 = strlen(desc);
    i = 0;
 
    while(i+len <= len2){
    if(memcmp(pattern,&desc[i],len) == 0){
           (*val)--;
           if((*val)==0) {
                found = 1;
                break;
            }
          }
        i++;
    }
    if(found)
        return(i);
    else
        return(-1);
}
       
/*==================================================================*/
/*                          txt_parse                               */
/*==================================================================*/
/* txt parse is design to parse a string (str), and return the 2nd  *
 * in the string as the pararmetr pattern, if the 2nd word of the   *
 * is followed by a number, then the number value is assignmed to   *
 * val. The remained of of the string is teh assigned to repalce    *
 * parameter. Note: white spaces inbetween the paraments are not    *
 * counted.  If txt_parse is unable to assign a value to  either of *
 * the char parameters, (due to lack of words), the parameters will *
 * be assigned the null value.*/

void txt_parse(str,pattern,val,replace)
        char    *str;
        char    **pattern;
        int     *val;
        char    **replace;
 
{
        int     i,j, index, len;
        char    *tmp;

/* remove command */
        len = strlen(str); 
        index = len;
        for(i=0; i < len ; i++)
            if (str[i] == ' ' ){
                    index = i;
                    break;
            }
 
    while (str[index] == ' ')
        index++;


/*extract search pattern */                
    len = strlen(&str[index]);
    j =  len;
    if(!len)
        *pattern = 0;
    else{
        for(i=0; i < len; i++)
            if (str[i + index] == ' '){
                j = i;
                break;
        }

        tmp = (char *)malloc(j+1);
    if(!tmp)
           merror("txt_parse", FATAL);  

        memcpy(tmp,&str[index],j);
        tmp[j] = 0;
        *pattern = tmp;
        index += j;
    } 

    while (str[index] == ' ')
        index++;


/* check number place of pattern */
    if(isdigit(str[index])){
        *val = atoi(&str[index]);
        if(*val < 1) *val = 1;
        while(isdigit(str[++index]));
        while (str[index] == ' ')
            index++;
        }
    else
        *val = 1;


/* remaining string = repalcement */ 
    len = strlen(&str[index]);
    if (!len)
        *replace = 0;
    else{
        tmp = (char *)malloc(len+1);
    if(!tmp)
           merror("txt_parse", FATAL);  
        memcpy(tmp,&str[index],len);
        tmp[len] = 0;
        *replace = tmp;
    }
    return;
}       

/*==================================================================*/
/*                          dm_nameroom                             */
/*==================================================================*/
int dm_nameroom(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    int     len,i=0;
	int		index;
    char    *tmp;
    room    *rom_ptr;

    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->class < DM)
        return(PROMPT);

	index = cut_command(cmnd->fullstr);
    len = strlen(cmnd->fullstr);

    if (!len) {
                print(ply_ptr->fd, "무엇으로 이름을 바꿉니까?\n");
                return(0);
    }

    if (len>79) {
                print(ply_ptr->fd, "이름이 너무 깁니다.\n");
                return(0);
    }
        

    memcpy(rom_ptr->name,&cmnd->fullstr[i],len);
    rom_ptr->name[len] = 0;
	paste_command(cmnd->fullstr, index);
	print(ply_ptr->fd, "이름을 변경하였습니다.\n");
    return(0);

}

/*==================================================================*/

/*==================================================================*/
/* The dm_append command allows a DM to append a text line onto   *
 * a rooms's description.  The default is a new line appended to  *
 * the end of the long text description. The user can select to   *
 * to append to the short descroption (-s) or the end of the      *
 * with no new line (-n) or both (-ns) */

int dm_append(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    int    len, len2, i =0;
    int    fd;
    room   *rom_ptr;
    char   *buf, *desc, nnl = 0, ds = 0;
    int    index;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < DM)
        return(PROMPT);
/*
    len = strlen(cmnd->fullstr);
    while (i < len){
        if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
            break;
        i++;
    }
    i++;
*/
   i=0;
   len=index=cut_command(cmnd->fullstr);

   if (i >= len){
        print(fd,"syntax: *append [-sn] <text>\n");
	return(0);
    }

    if (cmnd->fullstr[i]  == '-'){
        if (strlen(&cmnd->fullstr[i]) < 4){
            print(fd,"syntax: *append [-sn] <text>\n");
            return(0);
        }
        i++;

        if (cmnd->fullstr[i] == 's'){
            ds =1; 
            if (cmnd->fullstr[i+1] == 'n')
                nnl = 1;
        }
        else if (cmnd->fullstr[i] == 'n'){
            nnl = 1;
            if (cmnd->fullstr[i+1] == 's')
               ds =1; 
        }

       while(i < len){
           if (nnl && cmnd->fullstr[i] == ' ')
                break;
           if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
               break;
            i++;
       }
       i++;
       if (i >= len){
           print(fd,"syntax: *append [-sn] <text>\n");
           return(0);
       }
    }

    if (ds){
        desc = rom_ptr->short_desc;
        if (!rom_ptr->short_desc)
            nnl = 1;
    }
    else{
        desc = rom_ptr->long_desc;
        if (!rom_ptr->long_desc)
            nnl = 1;
    }


    len = (&cmnd->fullstr[i]) ? strlen(&cmnd->fullstr[i]) : 0;
    len2 =  (desc) ?  strlen(desc) : 0;

    if (nnl) {
        buf  = (char *)malloc(len2 + len+1);
        memcpy(buf,desc,len2);
        memcpy(&buf[len2],&cmnd->fullstr[i],len);
        buf[len+len2] = 0;
    }
    else {
        buf  = (char *)malloc(len2 + len+2);
        memcpy(buf,desc,len2);
        buf[len2] = '\n';
        memcpy(&buf[len2+1],&cmnd->fullstr[i],len);
        buf[len+len2+1] = 0;
    }

    free(desc);
    if (ds)
        rom_ptr->short_desc = buf;
    else
        rom_ptr->long_desc = buf;

    cmnd->fullstr[index]=' ';
    return(0);
}
/*==================================================================*/

/*==================================================================*/
/* The dm_prepend command allows a DM to prepend a text line onto   *
 * a rooms's description.  The default is a new line prepend to  *
 * the end of the long text description. The user can select to   *
 * to prepend to the short descroption (-s) or the end of the      *
 * with no new line (-n) or both (-ns) */

int dm_prepend(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    int    len, len2, i =0;
    int    fd;
    room   *rom_ptr;
    char   *buf, *desc, nnl = 0, ds = 0;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;

    if(ply_ptr->class < DM)
        return(PROMPT);

    len = strlen(cmnd->fullstr);
    while (i < len){
        if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
            break;
        i++;
    }
    i++;

   if (i >= len){
        print(fd,"syntax: *prepend [-sn] <text>\n");
	return(0);
    }

    if (cmnd->fullstr[i]  == '-'){
        if (strlen(&cmnd->fullstr[i]) < 4){
            print(fd,"syntax: *prepend [-sn] <text>\n");
            return(0);
        }
        i++;

        if (cmnd->fullstr[i] == 's'){
            ds =1; 
            if (cmnd->fullstr[i+1] == 'n')
                nnl = 1;
        }
        else if (cmnd->fullstr[i] == 'n'){
            nnl = 1;
            if (cmnd->fullstr[i+1] == 's')
               ds =1; 
        }

       while(i < len){
           if (cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
               break;
            i++;
       }
       i++;
       if (i >= len){
           print(fd,"syntax: *prepend [-sn] <text>\n");
           return(0);
       }
    }

    if (ds){
        desc = rom_ptr->short_desc;
        if (!rom_ptr->short_desc)
            nnl = 1;
    }
    else{
        desc = rom_ptr->long_desc;
        if (!rom_ptr->long_desc)
            nnl = 1;
    }


    len = (&cmnd->fullstr[i]) ? strlen(&cmnd->fullstr[i]) : 0;
    len2 =  (desc) ?  strlen(desc) : 0;

    if (nnl) {
        buf  = (char *)malloc(len2 + len+1);
        memcpy(buf,&cmnd->fullstr[i],len);
        memcpy(&buf[len],desc,len2);
        buf[len+len2] = 0;
    }
    else {
        buf  = (char *)malloc(len2 + len+2);
        memcpy(buf,&cmnd->fullstr[i],len);
        buf[len] = '\n';
        memcpy(&buf[len+1],desc,len2);
        buf[len+len2+1] = 0;
    }

    free(desc);
    if (ds)
        rom_ptr->short_desc = buf;
    else
        rom_ptr->long_desc = buf;
    return(0);
}

int dm_moonstone(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int rtn, rom_num;
    room *new_rom=NULL;
    object *obj_ptr=NULL;

    do {
       rom_num = mrand(1, RMAX-1);
       rtn = load_rom(rom_num, &new_rom);
          if(rtn > -1 && F_ISSET(new_rom, RNOTEL))
              rtn = -1;
    } while(rtn < 0);

    if(strlen(new_rom->name) < 2) return;

    load_obj(640, &obj_ptr);
    obj_ptr->shotsmax+=mrand(1,20);
    obj_ptr->shotscur=obj_ptr->shotsmax;
    add_obj_rom(obj_ptr, new_rom);
    broadcast("\n%s에 초인의 돌이 떨어졌습니다.", new_rom->name);

    return(0);
}
int dm_monster(ply_ptr, cmnd)
creature    *ply_ptr;
cmd *cmnd;
{
        int rtn, rom_num;
        room *new_rom=NULL;
        creature *crt_ptr=NULL;
        int tmp=0;

        for (tmp; tmp < 10; tmp++) {
           rom_num = mrand(3601, 3630);

           load_crt(mrand(265, 299), &crt_ptr);
           load_rom(rom_num, &new_rom);
           add_crt_rom(crt_ptr, new_rom);
        }
           broadcast("\n무적존을 강탈하려고 드레니아 몹이 침공했습니다.");
           broadcast("\n라작이 부하들에게 외칩니다. \"일반은 건드리지 말아아라\"");

           return(0);  
}

/**********************************************************************/
/*				dm_help				      */
/**********************************************************************/
/* This function allows a DM to obtain a list of flags for rooms, exits, */
/* monsters, players and objects.                                        */

int dm_help(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	char 	file[80];
	int	fd, c=0, match=0, num=0;
	
	fd = ply_ptr->fd;

	if (ply_ptr->class < DM){
		return(0);
	}

	strcpy(file, DOCPATH);

	if(cmnd->num < 2) {
		strcat(file, "/dm_helpfile");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	if(!strcmp(cmnd->str[1], "mflags")) {
		strcat(file, "/mflags");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "oflags")) {
		strcat(file, "/oflags");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "pflags")) {
		strcat(file, "/pflags");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}
	
	else if(!strcmp(cmnd->str[1], "rflags")) {
		strcat(file, "/rflags");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "xflags")) {
		strcat(file, "/xflags");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "sflags")) {
		strcat(file, "/sflags");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "titles")) {          
		strcat(file, "/titles");                   
		view_file(fd, 1, file);                    
		return(DOPROMPT);                          
	}                                                  

	else if(!strcmp(cmnd->str[1], "oset")) {
		strcat(file, "/oset");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "pset")) {
		strcat(file, "/pset");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "char")) {
		strcat(file, "/char");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "wear")) {
		strcat(file, "/wear");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "otypes")) {
		strcat(file, "/otypes");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "realms")) {
		strcat(file, "/realms");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "exp")) {
		strcat(file, "/exp");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "scrolls")) {
		strcat(file, "/scrolls");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else if(!strcmp(cmnd->str[1], "관리")) {
		strcat(file, "/관리");
		view_file(fd, 1, file);
		return(DOPROMPT);
	}

	else {
		print(fd,"That dm help file does not exist.\n");
		return(0);
	}
}
