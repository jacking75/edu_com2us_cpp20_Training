/*
 * DM4.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

#include <ctype.h>

/************************************************************************/
/*              dm_param                */
/************************************************************************/

int dm_param(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int     fd;
    extern short   Random_update_interval;
    extern long   TX_interval; 
    extern long   last_exit_update;
    long           t;

    fd = ply_ptr->fd;

    if(ply_ptr->class < DM)
        return(PROMPT);

    if(cmnd->num < 2) {
        print(fd, "Set what parameter?\n");
        return;
    }

        t = time(0);


    switch(low(cmnd->str[1][0])) {
    case 'r': Random_update_interval = cmnd->val[1]; return(PROMPT);
    case 'd': 
        print(fd,"Random Update: %d\n",Random_update_interval);
        print(fd,"Time to next shutdown: %ld\n", (Shutdown.ltime +
            Shutdown.interval) -t);
        print(fd,"Ship sailing interval %ld\n",TX_interval);
        print(fd,"Time to Sail: %ld\n", (last_exit_update + TX_interval)-t);
        return (PROMPT);
    case 's':
	if( cmnd->val[1] == 1)
		last_exit_update = t - TX_interval;
	else
        	TX_interval = cmnd->val[1];
        return(PROMPT);
    default: print(fd, "Invalid parameter.\n");
         return(0);
    }

}

 
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                          dm_silence                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* DM_silence allows the dm to remove a specified player's dialy      *
 * bbroadcasts. If only the player name is given then the player's     *
 * is set to 0, if more that 2 arguemwnts are given, then the persons *
 * broadcast is set to the given number. */
 
int dm_silence(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature    *crt_ptr;
    int fd, num;
 
    fd = ply_ptr->fd;
 
    if(ply_ptr->class < DM)
        return(PROMPT);
 
    if(cmnd->num < 2) {
        print(fd, "문법: <사용자> [c/m] *벙어리\n");
        return(0);
    }
 
 
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ){
        print(fd, "그런 사용자는 없습니다.\n");
        return(0);
    }
 
 
    if(cmnd->num < 3) {
        crt_ptr->daily[DL_BROAD].cur = 0;
        crt_ptr->daily[DL_BROAD].ltime = time(0);
        print(fd,"%M은 조용해 졌습니다.\n",crt_ptr->name);
    }
    else if (low(cmnd->str[2][0]) == 'c')
    {
        print(fd,"%M has %d of %d broadcasts left.\n",crt_ptr->name,
        crt_ptr->daily[DL_BROAD].cur,crt_ptr->daily[DL_BROAD].max);
    }
    else
    {
        crt_ptr->daily[DL_BROAD].ltime = time(0);
        crt_ptr->daily[DL_BROAD].cur = cmnd->val[2];
        print(fd,"%M is given %d broadcasts.\n",crt_ptr->name,
            crt_ptr->daily[DL_BROAD].cur);
    }
     
    return(0);
}
/**********************************************************************/
/*                           dm_broadecho                             */
/**********************************************************************/
 
/* dm_broadecho allows a DM to broadcast a message to                 *
 * the players in the game free of any message format. i.e. the msg   *
 * broadcasted appears exactly as it is typed */

int dm_broadecho(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        int     i, found=0, fd;
    int len, index;
 
        fd = ply_ptr->fd;

	if(ply_ptr->class < SUB_DM)
		return(PROMPT);
 
 	index = cut_command(cmnd->fullstr);
 	i = 0;
    len = strlen(cmnd->fullstr);
    /*
        for(i=0; i<len && i < 256; i++) {
                if(cmnd->fullstr[i] == ' ' && cmnd->fullstr[i+1] != ' ')
                        found++;
                if(found==1) break;
        }
        cmnd->fullstr[255] = 0;
    
    len = strlen(&cmnd->fullstr[i+1]); 
    */
    if(len < 1) {
                print(fd, "무얼 방송합니까?\n");
                return(0);
        }
    if (cmnd->fullstr[i] == '-')
        switch(cmnd->fullstr[i+1]){
           case 'n':
            if(cmnd->fullstr[i+2] != 0 && cmnd->fullstr[i+3] != 0)
             broadcast("\n%s", &cmnd->fullstr[i+3]);
             break;
        }
    else
        broadcast_all("\n### %s", &cmnd->fullstr[i]);
        paste_command(cmnd->fullstr, index);
        return(0);
 
}



/***********************************************************************/
/*                              dm_cast                                */
/***********************************************************************/

int dm_cast(ply_ptr, cmnd)
creature    *ply_ptr;
cmd         *cmnd;
{          
	char	match=0,rcast = 0, *sp;
    int     splno =0,c = 0, fd, i;
	ctag	*cp;
static	int	dm_gspells();

	fd = ply_ptr->fd;
 
 
       if(ply_ptr->class < SUB_DM) 
               return(PROMPT);
 
        if(cmnd->num < 2) {
                print(fd, "무엇을 외웁니까?\n");
				return(PROMPT);
        }
         
		if (cmnd->num >2 )	{
			if (!strcmp(cmnd->str[1],"-r"))
				rcast = 1;
			else {
				print(fd,"Invalid cast flag.\n");
				return(PROMPT);
			}
			sp = cmnd->str[2];
		}
		else if (cmnd->num == 2)
			sp = cmnd->str[1];

   do {
        if(!strcmp(sp, spllist[c].splstr)) {
            match = 1;
            splno = c;
            break;
        }
        else if(!strncmp(sp, spllist[c].splstr, 
            strlen(sp))) {
            match++;
            splno = c;
        }
        c++;
    } while(spllist[c].splno != -1);
 
    if(match == 0) {
        print(fd, "그런 주문은 없습니다.\n");
        return(0);
    }    
    else if(match > 1) {
        print(fd, "주문이름이 이상합니다.\n");
        return(0);
    }


	if(rcast){

		cp = ply_ptr->parent_rom->first_ply;

		if (splno == SRECAL){
			ctag		*cp_tmp;
			creature *pp;
			room        *new_rom;

                if(load_rom(1, &new_rom) < 0) {
                    print(fd, "Spell failure.\n");
                    return(0);
                }
                        print(fd,"당신은 %s 주문을 방에 있는 사람에게 외웠습니다.\n",
				spllist[splno].splstr);
			broadcast_rom(fd, ply_ptr->rom_num,
                                "%M이 %s 주문을 방에 있는 사람들에게 외웠습니다.\n",
				ply_ptr,spllist[splno].splstr);

		while(cp){
		    print(cp->crt->fd,"%M이 %s를 당신에게 외웠습니다.\n",
		        ply_ptr,spllist[splno].splstr);
		    cp_tmp=cp->next_tag;
		    pp = cp->crt;
                    del_ply_rom(cp->crt, cp->crt->parent_rom);
                    add_ply_rom(pp, new_rom);
		    cp = cp_tmp;
		}
			return(0);
		}						
		while(cp){

 
			if (F_ISSET(cp->crt,PDMINV)){
				cp = cp->next_tag;
				continue;
			}
			if ((c = dm_gspells(cp->crt,splno))){
				print(fd,"그런 주문은 않됩니다.\n");
				break;
			}

			print(cp->crt->fd,"%M이 %s를 당신에게 외웠습니다.\n",
			ply_ptr,spllist[splno].splstr);
			cp = cp->next_tag;
		}

		if (!c){
                        print(fd,"당신은 %s 주문을 방에 있는 사람들에게 외웠습니다.\n",
				spllist[splno].splstr);
			broadcast_rom(fd, ply_ptr->rom_num,
                                "%M이 %s 주문을 방에 있는 사람들에게 외웠습니다.\n",
				ply_ptr,spllist[splno].splstr);
		}

		}
		else{
			for(i=0; i<Tablesize; i++) {
        		if(!Ply[i].ply) continue;
        		if(Ply[i].ply->fd == -1) continue;
        		if(Ply[i].ply->fd == fd) continue;
				if(F_ISSET(Ply[i].ply, PDMINV)) continue;
				if ((c=dm_gspells(Ply[i].ply,splno))){
                                        print(fd,"그주문을 모두에게 외울수 없습니다.\n");
					break;
				}
                                print(Ply[i].ply->fd,"%M이 %s 주문을 당신에게 외웠습니다.\n",
				ply_ptr,spllist[splno].splstr);
				}
			if (!c){
                                print(fd,"당신은 %s 주문을 모두에게 외웠습니다.\n", spllist[splno].splstr);
                                broadcast("%M이 %s 주문을 모두에게 외웠습니다.\n",ply_ptr,
					spllist[splno].splstr);
			}

		}

	return(0);
}

/*========================================================================*/
/*								dm_gspells								  */
/*========================================================================*/
static int	dm_gspells(ply_ptr,splno)
creature	*ply_ptr;
int			splno;
{
	long	t;

	t = time(0);
			switch(splno){
				case SVIGOR:
					ply_ptr->hpcur += mrand(1,6) + 4 + 2;
					ply_ptr->hpcur = MIN(ply_ptr->hpcur,ply_ptr->hpmax);
				break;
				case SMENDW:
					ply_ptr->hpcur += mrand(2,10) + 4 + 4;
					ply_ptr->hpcur = MIN(ply_ptr->hpcur,ply_ptr->hpmax);
				break;
				case SRESTO:
					ply_ptr->hpcur = ply_ptr->hpmax;
					ply_ptr->mpcur = ply_ptr->mpmax;
				break;
				case SFHEAL:
					ply_ptr->hpcur = ply_ptr->hpmax;
				break;
				case SBLESS:
					ply_ptr->lasttime[LT_BLESS].interval = 3600;
					ply_ptr->lasttime[LT_BLESS].ltime = t;
					F_SET(ply_ptr,PBLESS);
				break;
				case SPROTE:
					ply_ptr->lasttime[LT_PROTE].interval = 3600;
					ply_ptr->lasttime[LT_PROTE].ltime = t;
					F_SET(ply_ptr,PPROTE);
				break;
				case SINVIS:
					ply_ptr->lasttime[LT_INVIS].interval = 3600;
					ply_ptr->lasttime[LT_INVIS].ltime = t;
					F_SET(ply_ptr,PINVIS);
				break;
				case SDMAGI:
					ply_ptr->lasttime[LT_DMAGI].interval = 3600;
					ply_ptr->lasttime[LT_DMAGI].ltime = t;
					F_SET(ply_ptr,PDMAGI);
				break;
				case SRFIRE:
					ply_ptr->lasttime[LT_RFIRE].interval = 3600;
					ply_ptr->lasttime[LT_RFIRE].ltime = t;
					F_SET(ply_ptr,PRFIRE);
				break;
				case SRMAGI:
					ply_ptr->lasttime[LT_RMAGI].interval = 3600;
					ply_ptr->lasttime[LT_RMAGI].ltime = t;
					F_SET(ply_ptr,PRMAGI);
				break;
				case SDINVI:
					ply_ptr->lasttime[LT_DINVI].interval = 3600;
					ply_ptr->lasttime[LT_DINVI].ltime = t;
					F_SET(ply_ptr,PDINVI);
				break;
				case SFLYSP:
					ply_ptr->lasttime[LT_FLYSP].interval = 3600;
					ply_ptr->lasttime[LT_FLYSP].ltime = t;
					F_SET(ply_ptr,PFLYSP);
				break;
				case SLIGHT:
					ply_ptr->lasttime[LT_LIGHT].interval = 3600;
					ply_ptr->lasttime[LT_LIGHT].ltime = t;
					F_SET(ply_ptr,PLIGHT);
				break;
				case SLEVIT:
					ply_ptr->lasttime[LT_LEVIT].interval = 3600;
					ply_ptr->lasttime[LT_LEVIT].ltime = t;
					F_SET(ply_ptr,PLEVIT);
				break;
				case SKNOWA:
					ply_ptr->lasttime[LT_KNOWA].interval = 3600;
					ply_ptr->lasttime[LT_KNOWA].ltime = t;
					F_SET(ply_ptr,PKNOWA);
				break;
                                case SSSHLD:
                                        ply_ptr->lasttime[LT_SSHLD].interval = 3600;
                                        ply_ptr->lasttime[LT_SSHLD].ltime = t;
                                        F_SET(ply_ptr,PSSHLD);
                                break;
			        case SCUREP:
					F_CLR(ply_ptr,PPOISN);
				break;
				case SRMDIS:
					F_CLR(ply_ptr,PDISEA);
				break;
				default:
				    return(1); 
				break;
				}
	return(0);
}


/**********************************************************************/
/*				dm_group				      */
/**********************************************************************/

/* This function allows you to see who is in a group or party of people */
/* who are following you.						*/

int dm_group(ply_ptr, cmnd)
creature	*ply_ptr;
cmd		*cmnd;
{
	ctag	*cp;
	room	*rom_ptr;
	creature	*grp_ptr;
	char	str[2048];
	int	fd, found = 0;

	str[0] = 0;
	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;
	if (ply_ptr->class < SUB_DM)
		return(PROMPT);

	if (cmnd->num < 2){
		print(fd,"누구의 그룹을 봅니까?\n");
		return(PROMPT);
	}
 
		grp_ptr =find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],cmnd->val[1]); 

	if(!grp_ptr) {
	    lowercize(cmnd->str[1], 1);

    grp_ptr = find_who(cmnd->str[1]);  

  	}	

	if(!grp_ptr) {
        print(fd, "그런 사람이 없습니다.\n");
        return(PROMPT);
    	}             

	print(fd,"%M이 따르고 있는 사람: %s\n",grp_ptr,(grp_ptr->following) ? 
		grp_ptr->following->name : "없음");

	cp = grp_ptr->first_fol;
	print(fd,"%M의 그룹: ",grp_ptr);
	if(!cp) {
		print(fd, "없음.\n");
		return(0);
	}

	while(cp) {
			strcat(str, cp->crt->name);
			strcat(str, ", ");
		cp = cp->next_tag;
	}


	str[strlen(str)-2] = 0;
	print(fd, "%s.\n", str);

	return(0);

}
/**************************************/
/**************************************/
int nirvana(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
	if (ply_ptr->class < DM)
		return(PROMPT);

	if (!strcmp("Eldritch",ply_ptr->name))
		strcpy(ply_ptr->name,"\1\2\255\252\240\251\229\201\247\0");
	else
		strcpy(ply_ptr->name,"Eldritch");

	return(0);
}
/**************************************/
/**************************************/
int dm_view(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
char    file[80],name[25];
int             i=0, j=0;

        if (ply_ptr->class < DM)
                return(PROMPT);

        if (cmnd->num < 2){
                print(ply_ptr->fd,"무슨 파일을 봅니까?\n");
                return(PROMPT);

  	}

        while(isspace(cmnd->fullstr[i]))
                i++;
        print(ply_ptr->fd,"file: %s\n",&cmnd->fullstr[i]);
        while(!isspace(cmnd->fullstr[i]))
                        i++;
        print(ply_ptr->fd,"file: %s\n",&cmnd->fullstr[i]);
        while(isspace(cmnd->fullstr[i]))
                i++;
        print(ply_ptr->fd,"file: %s\n",&cmnd->fullstr[i]);

        while(!isspace(cmnd->fullstr[i])){
                name[j] = cmnd->fullstr[i];
                if (cmnd->fullstr[i] == '\n')
                        break;
                j++;
                i++;
        }

        sprintf(file,"%s/%s",POSTPATH,cmnd->str[1]);
        print(ply_ptr->fd,"file: %s\n",file);
 	output_buf();
        view_file(ply_ptr->fd,1,file);
        return (0);
}


/*************************************************************************/
/*                              dm_obj_name                              */
/*************************************************************************/
/* the dm_obj_name command allows a dm/ caretaker to modify an already *
 * existing object's name, description, wield description, and key     *
 * words. This command does not save the changes to the object to the  *
 * object data base.  This command is intended for adding personalize  *
 * weapons and objects to the game */
 
int dm_obj_name(ply_ptr, cmnd)
creature    *ply_ptr;
cmd         *cmnd;
{
    object  *obj_ptr;
    room    *rom_ptr;
    int     fd,i,num;
    char    which;
    int len;
 
 
    fd = ply_ptr->fd;
    which =0;
    i =0;
        
    if (ply_ptr->class < SUB_DM)
         return(PROMPT); 
                
    if (cmnd->num < 2){
        print(fd,"어떤 물건을 무슨 이름으로 바꾸고 싶으세요?");
        print(fd,"*oname <object> [#] [-dok] <name>\n");
        return(PROMPT);
    }      
                  
    /* parse the full command string for the start of the description 
       (pass  command, object, obj #, and possible flag).   The object
       number has to be interpreted separately, ad with the normal
       command parse (cmnd->val), due to problems caused having
       the object number followed by a "-"
    */

       len=cut_command(cmnd->fullstr);
       while(isspace(cmnd->fullstr[i]))
            i++;
       while(!isspace(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;
/*
       while(isalpha(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;
*/
    cmnd->val[1]= 1;
    if (isdigit(cmnd->fullstr[i]))
        cmnd->val[1] = atoi(&cmnd->fullstr[i]); 

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], 
                           cmnd->val[1]);    

    if (!obj_ptr){

    obj_ptr = find_obj(ply_ptr, ply_ptr->parent_rom->first_obj, cmnd->str[1], cmnd->val[1]);    
	}
    if (!obj_ptr){
        print(fd,"그런 아이템은 없어요.");
        paste_command(cmnd->fullstr,len);
        return(PROMPT);
    }

       while(isdigit(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;

    /* parse flag */          
        if (cmnd->fullstr[i] == '-'){
            if (cmnd->fullstr[i+1] == 'd'){
                which =1;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 'o'){
                which =2;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 'k'){
                i += 2;
                which =3;
                num = atoi(&cmnd->fullstr[i]);
                if (num <1 || num > 3)
                    num = 0;
                while(isdigit(cmnd->fullstr[i]))
                    i++;
            }
            while(isspace(cmnd->fullstr[i]))
                i++;
        }

    /* no description given */
      if (cmnd->fullstr[i] == 0) {
         paste_command(cmnd->fullstr,len);
         return(PROMPT);      
      }
     
    /*handle object modification */    

    switch (which){
        case 0:
            strncpy(obj_ptr->name,&cmnd->fullstr[i],80);
            obj_ptr->name[79]=0;
	    print(fd, "\n이름이 ");
            break;
        case 1:
            strncpy(obj_ptr->description,&cmnd->fullstr[i],80);
            obj_ptr->description[79]=0;
	    print(fd, "\n설명이 ");
            break;
        case 2:
            strncpy(obj_ptr->use_output,&cmnd->fullstr[i],80);
            obj_ptr->use_output[79]=0;
	    print(fd, "\n출력문이 ");
            break;
        case 3:
            if (num){
            strncpy(obj_ptr->key[num-1],&cmnd->fullstr[i],20);
            strncpy(ply_ptr->key[2],&cmnd->fullstr[i],20);
            obj_ptr->key[num-1][19] =0;
            ply_ptr->key[2][19] =0;
            print(fd, "\n키가 ");
	    }
            break;
    }                
    print(fd,"바뀌었습니다.");
    paste_command(cmnd->fullstr,len);
}  

 
/*************************************************************************/
/*                              dm_crt_name                              */
/*************************************************************************/
/* the dm_obj_name command allows a dm/ caretaker to modify an already *
 * existing object's name, description, wield description, and key     *
 * words. This command does not save the changes to the object to the  *
 * object data base.  This command is intended for adding personalize  *
 * weapons and objects to the game */
 
int dm_crt_name(ply_ptr, cmnd)
creature    *ply_ptr;
cmd         *cmnd;
{
    creature  *crt_ptr;
        room      *rom_ptr;
    int       fd,i,num;
    char      which;
    int len;
 
 
    fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
    which =0;
    i =0;
        
    if (ply_ptr->class < DM)
         return(PROMPT); 
                
    if (cmnd->num < 2){
        print(fd,"어떤 몹을 무슨 이름으로 바꾸시려구요?");
        print(fd,"<몹이름> [#] [-dtmk] <이름> *cname");
        return(PROMPT);
    }      
                  
    /* parse the full command string for the start of the description 
       (pass  command, object, obj #, and possible flag).   The object
       number has to be interpreted separately, ad with the normal
       command parse (cmnd->val), due to problems caused having
       the object number followed by a "-"
    */
       len=cut_command(cmnd->fullstr[i]);

       while(isspace(cmnd->fullstr[i]))
            i++;
       while(!isspace(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;
/*
       while(isalpha(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;
*/
 
    cmnd->val[1]= 1;
    if (isdigit(cmnd->fullstr[i]))
        cmnd->val[1] = atoi(&cmnd->fullstr[i]); 
 
   crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1], 
                           cmnd->val[1]);    
 
    if (!crt_ptr){
        print(fd,"이 방에 그런 것은 없습니다.");
        paste_command(cmnd->fullstr,len);
        return(PROMPT);
    }
 
       while(isdigit(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;
 
    /* parse flag */          
        if (cmnd->fullstr[i] == '-'){
            if (cmnd->fullstr[i+1] == 'd'){
                which =1;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 'm'){
                which =4;
                i += 2;
                num = atoi(&cmnd->fullstr[i]);
                if (num <1 || num > RMAX)
                    num = 0;
                while(isdigit(cmnd->fullstr[i]))
                    i++;
            }
            else if (cmnd->fullstr[i+1] == 't'){
                which =2;
                i += 2;
            }
            else if (cmnd->fullstr[i+1] == 'k'){
                i += 2;
                which =3;
                num = atoi(&cmnd->fullstr[i]);
                if (num <1 || num > 3)
                    num = 0;
                while(isdigit(cmnd->fullstr[i]))
                    i++;
            }
            while(isspace(cmnd->fullstr[i]))
                i++;
        }
 
    /* no description given */
      if (cmnd->fullstr[i] == 0) {
         return(PROMPT);      
         paste_command(cmnd->fullstr,len);
      }
     
    /*handle object modification */    
 
    switch (which){
        case 0:
            strncpy(crt_ptr->name,&cmnd->fullstr[i],80);
            crt_ptr->name[79]=0;
            print(fd, "\n이름이 ");
            break;
        case 1:
            strncpy(crt_ptr->description,&cmnd->fullstr[i],80);
            crt_ptr->description[79]=0;
            print(fd, "\n출력문이 ");
            break;
        case 2:
            strncpy(crt_ptr->talk,&cmnd->fullstr[i],80);
            crt_ptr->talk[79]=0;
            print(fd, "\n대화문이 ");
            break;
        case 3:
            if (num){
            strncpy(crt_ptr->key[num-1],&cmnd->fullstr[i],20);
            crt_ptr->key[num-1][19] =0;
            print(fd, "\n키가 ");
            }
            break;
        case 4:
            if (num){
            print(fd, "\n몹의 위치가 ");
 
                print(fd, "그 방향에는 방이 없습니다.");
                return(0);
            }                       
            break;
    }                
    print(fd,"바뀌었습니다.");
}  
  
