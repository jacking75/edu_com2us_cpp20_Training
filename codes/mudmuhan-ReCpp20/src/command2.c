/*
 * COMMAND2.C:
 *
 *      Addition user command routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include "board.h"
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

extern long login_time[PMAX];

/**********************************************************************/
/*                              look                                  */
/**********************************************************************/

/* This function is called when a player tries to look at the room he */
/* is in, or at an object in the room or in his inventory.            */

int look(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room            *rom_ptr;
	room			*new_rom;
    object          *obj_ptr;
	object			*bnk_ptr;
    creature        *crt_ptr;
	exit_			*ext_ptr;
	char	file[80];
    char            str[2048];
    int             fd, n, match=0;

    if(cmnd->num < 2) {
        display_rom(ply_ptr, ply_ptr->parent_rom);
        return(0);
    }

    fd = ply_ptr->fd;
    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, BOLD);
        ANSI(fd, RED);
        print(fd, "´ç½ÅÀº ´«ÀÌ ¸Ö¾î ÀÖ½À´Ï´Ù!");
        ANSI(fd, WHITE);
        ANSI(fd, NORMAL);
        return(0);
    }
    rom_ptr = ply_ptr->parent_rom;
	ext_ptr=find_ext(ply_ptr, rom_ptr->first_ext, 
					cmnd->str[1], cmnd->val[1]);
	if(ext_ptr) {
		if(F_ISSET(ext_ptr, XCLOSD)) {
			print(fd, "±× Ãâ±¸´Â ´ÝÇô ÀÖ½À´Ï´Ù.");
			return(0);
		}
		sprintf(file, "%s/r%02d/r%05d", ROOMPATH,ext_ptr->room/1000, ext_ptr->room);
		if(!file_exists(file)) {
			print(fd, "Áöµµ°¡ ¾ø½À´Ï´Ù.");
			return(0);
		}
		load_rom(ext_ptr->room, &new_rom);
		if(!new_rom || rom_ptr == new_rom) {
			print(fd, "Áöµµ°¡ ¾ø½À´Ï´Ù.");
			return(0);
		}
		if(F_ISSET(new_rom, RONMAR) || F_ISSET(new_rom, RONFML)) {
			print(fd, "±× ¹æÀº º¼ ¼ö°¡ ¾ø½À´Ï´Ù.");
			return(0);
		}
		display_rom(ply_ptr, new_rom);
		return(0);
	}

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1],
               cmnd->val[1]);

    if(!obj_ptr || !cmnd->val[1]) {
        for(n=0; n<MAXWEAR; n++) {
            if(!ply_ptr->ready[n])
                continue;
            if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                match++;
            else continue;
            if(cmnd->val[1] == match || !cmnd->val[1]) {
                obj_ptr = ply_ptr->ready[n];
                break;
            }
        }
    }

    if(!obj_ptr)
        obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

    if(obj_ptr) {

        if(obj_ptr->special==SP_BOARD) {
            board_obj[fd]=obj_ptr;
            list_board(fd,0,"");
            return(DOPROMPT);
        }
        if(obj_ptr->special) {
            n = special_obj(ply_ptr, cmnd, SP_MAPSC);
            if(n != -2) return(MAX(n, 0));
        }

        if(obj_ptr->description[0])
            print(fd, "%s\n", obj_ptr->description);
        else
            print(fd, "Æ¯º°ÇÑ Á¡ÀÌ ¾ø½À´Ï´Ù.\n");

        if(F_ISSET(ply_ptr, PKNOWA)) {
            if(F_ISSET(obj_ptr, OGOODO))
                print(fd, "Çª¸¥ ±¤Ã¤°¡ »¸¾î ³ª¿À°í ÀÖ½À´Ï´Ù.\n");
            if(F_ISSET(obj_ptr, OEVILO))
                print(fd, "ºÓÀº ±¤Ã¤°¡ »¸¾î ³ª¿À°í ÀÖ½À´Ï´Ù.\n");
        }

        if(F_ISSET(obj_ptr, OCONTN)) {
            strcpy(str, "³»¿ë¹°: ");
            n = list_obj(&str[8], ply_ptr, obj_ptr->first_obj);
            if(n)
                print(fd, "%s.\n", str);
        }

        if(obj_ptr->type <= MISSILE) {
            print(fd, "%I%j ", obj_ptr,"0");
            switch(obj_ptr->type) {
            case SHARP: print(fd, "¸Å¿ì ³¯Ä«·Î¿î 'µµ'ÀÔ´Ï´Ù.\n"); break;
            case THRUST: print(fd, "¸Å¿ì °ø°ÝÀûÀÎ '°Ë'ÀÔ´Ï´Ù.\n"); break;
            case POLE: print(fd, "³¯ÀÌ ¹ÙÂ¦ ¼± 'Ã¢'ÀÔ´Ï´Ù.\n"); break;
            case BLUNT: print(fd, "¸Å¿ì À§·ÂÀûÀÎ 'ºÀ'ÀÔ´Ï´Ù.\n"); break;
            case MISSILE: print(fd, "¸Å¿ì °­·ÂÇÏ°Ô º¸ÀÌ´Â '±Ã'ÀÔ´Ï´Ù.\n"); break;
            }
        }

        if(obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR ||
           obj_ptr->type == LIGHTSOURCE || obj_ptr->type == WAND ||
           obj_ptr->type == KEY) {
            if(obj_ptr->shotscur < 1)
                print(fd, "±×°ÍÀº ºÎ¼­Á® ¹ö·È°Å³ª ´Ù ½á¹ö·È½À´Ï´Ù.\n");
            else if(obj_ptr->shotscur <= obj_ptr->shotsmax/10)
                print(fd, "±×°ÍÀº °ð ºÎ¼­Áú°Í °°½À´Ï´Ù.\n");
        }

        return(0);
    }

    if(!strcmp(cmnd->str[1],"³ª")) crt_ptr=ply_ptr;
    else crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
               cmnd->val[1]);
    if(crt_ptr) {

        if(!strncmp(crt_ptr->name, ply_ptr->name, strlen(crt_ptr->name))) {
          print(fd,"´ç½ÅÀº °Å¿ïÀ» µé°í ÀÚ½ÅÀ» º¾´Ï´Ù.\n");
          broadcast_rom(fd,rom_ptr->rom_num,
             "\n%M%j °Å¿ïÀ» µé°í ÀÚ½ÅÀ» ¹Ù¶ó º¾´Ï´Ù.",ply_ptr,"1");
        }
        else {
          print(fd, "´ç½ÅÀº %1M%j º¾´Ï´Ù.\n", crt_ptr,"3");
          broadcast_rom(fd,rom_ptr->rom_num,
             "\n%M%j %M%j º¾´Ï´Ù.",ply_ptr,"1",crt_ptr,"3");
        }
        if(crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PMARRI)) {
            print(fd, "%s´Â %s´Ô°ú °áÈ¥ÇÑ ±âÈ¥ÀÚÀÔ´Ï´Ù.\n", 
                    F_ISSET(crt_ptr, PMALES) ? "±×":"±×³à", &crt_ptr->key[2][1]);
        }            
        
		if(crt_ptr->type != PLAYER) {
        if(crt_ptr->description[0])
            print(fd, "%s\n", crt_ptr->description);
        else
            print(fd, "Æ¯º°ÇÑ °ÍÀº º¸ÀÌÁö ¾Ê½À´Ï´Ù.\n");
		}
		else {
			print(fd, "%s´Â %s¼­ ÀÖ½À´Ï´Ù.\n", F_ISSET(crt_ptr, PMALES) ? "±×":"±×³à",
				crt_ptr->description);
		}
        if(F_ISSET(ply_ptr, PKNOWA) && crt_ptr->alignment!=0) {
            print(fd, "%s¿¡°Ô¼­ ",
                F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
            if(crt_ptr->alignment < 0)
                print(fd, "ºÓÀº ±¤Ã¤");
            else print(fd, "Çª¸¥ ±¤Ã¤");
            print(fd, "°¡ »¸¾î ³ª¿À°í ÀÖ½À´Ï´Ù.\n");
        }
        if(crt_ptr->hpcur < (crt_ptr->hpmax*9)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*8)/10) )
            print(fd, "%s´Â °¡º­¿î »óÃ³¸¦ ÀÔ¾ú½À´Ï´Ù.\n",
                  F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*8)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*6)/10) )
            print(fd, "%s´Â ¿©·¯±ºµ¥ »óÃ³¸¦ ÀÔ¾ú½À´Ï´Ù.\n",
                  F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*6)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*4)/10) )
            print(fd, "%s´Â ¸¹Àº »óÃ³¸¦ ÀÔ¾ú½À´Ï´Ù.\n",
                  F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*4)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*2)/10) )
            print(fd, "%s´Â ½É°¢ÇÑ »óÃ³¸¦ ÀÔ¾ú½À´Ï´Ù.\n",
                  F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*2)/10)
            print(fd, "%s´Â Á×±â Á÷ÀüÀÔ´Ï´Ù.\n",
                  F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
        if(is_enm_crt(ply_ptr->name, crt_ptr))
            print(fd, "%s´Â ´ç½Å¿¡°Ô ¸Å¿ì È­°¡ ³­°Í °°½À´Ï´Ù.\n",
                  F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
        if(crt_ptr->first_enm) {
            if(!strcmp(crt_ptr->first_enm->enemy, ply_ptr->name))
                print(fd, "%s´Â ´ç½Å°ú ½Î¿ì°í ÀÖ½À´Ï´Ù.\n",
                      F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
            else
                print(fd, "%s´Â %S%j ½Î¿ì°í ÀÖ½À´Ï´Ù.\n",
                      F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à",
                      crt_ptr->first_enm->enemy,"2");
        }
        consider(ply_ptr, crt_ptr);
        equip_list(fd, crt_ptr);
        return(0);
    }

    cmnd->str[1][0] = up(cmnd->str[1][0]);
    crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply, cmnd->str[1],
               cmnd->val[1]);

    if(crt_ptr) {
        print(fd, "´ç½ÅÀº %M%j º¾´Ï´Ù.\n",crt_ptr,"3");
        broadcast_rom(fd,rom_ptr->rom_num,"\n%M%j %M%j º¾´Ï´Ù.",ply_ptr,"1",crt_ptr,"3");
        if(crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PMARRI)) {
            print(fd, "%s´Â %s´Ô°ú °áÈ¥ÇÑ ±âÈ¥ÀÚÀÔ´Ï´Ù.\n", 
                    F_ISSET(crt_ptr, PMALES) ? "±×":"±×³à", &crt_ptr->key[2][1]);
        }            
		if(crt_ptr->type != PLAYER) {
        if(crt_ptr->description[0])
            print(fd, "%s\n", crt_ptr->description);
        else
            print(fd, "Æ¯º°ÇÑ °ÍÀº º¸ÀÌÁö ¾Ê½À´Ï´Ù.\n");
		}
		else {
			print(fd, "%s´Â %s¼­ ÀÖ½À´Ï´Ù.\n", F_ISSET(crt_ptr, PMALES) ? "±×":"±×³à",
				crt_ptr->description);
		}
        if(F_ISSET(ply_ptr, PKNOWA) && crt_ptr->alignment>=-100 && crt_ptr->alignment<101) {
            print(fd, "%s¿¡°Ô¼­ ",
                F_ISSET(crt_ptr, MMALES) ? "±×":"±×³à");
            if(crt_ptr->alignment < -100)
                print(fd, "ºÓÀº ±¤Ã¤");
            else print(fd, "Çª¸¥ ±¤Ã¤");
            print(fd, "°¡ »¸¾î ³ª¿À°í ÀÖ½À´Ï´Ù.\n");
        }
        if(crt_ptr->hpcur < (crt_ptr->hpmax*3)/10)
            print(fd, "%s´Â °¡º­¿î »óÃ³¸¦ ÀÔ¾ú½À´Ï´Ù.\n",
                  F_ISSET(crt_ptr, PMALES) ? "±×":"±×³à");
        equip_list(fd, crt_ptr);
        return(0);
    }

    else
        print(fd, "±×·± °Ç º¸ÀÌÁö ¾Ê½À´Ï´Ù.\n");

    return(0);

}

/**********************************************************************/
/*                              move                                  */
/**********************************************************************/

/* This function takes the player using the socket descriptor specified */
/* in the first parameter, and attempts to move him in one of the six   */
/* cardinal directions (n,s,e,w,u,d) if possible.                       */

int move(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
	FILE			*fp;
    room            *rom_ptr, *old_rom_ptr;
    creature        *crt_ptr;
    char            tempstr[10];
    char            *str;
	char			file[80], name[10][15];
	int				i;
    ctag            *cp, *temp;
    xtag            *xp;
    int             found=0, fd, old_rom_num, fall, dmg, n, chance;
    long            t;

    rom_ptr = ply_ptr->parent_rom;
    fd = ply_ptr->fd;
    str = cmnd->str[0];

    t=time(0);

/* ÀÌµ¿ µô·¹ÀÌ ¾ø¾Ú.
     = »þ(0);
    if(ply_ptr->lasttime[LT_MOVED].ltime == t) {
        if(++ply_ptr->lasttime[LT_MOVED].misc > 2) {
            please_wait(fd, 1);
            return(0);
        }
    }
    else if(ply_ptr->lasttime[LT_ATTCK].ltime+2 > t) {
        please_wait(fd, 3-t+ply_ptr->lasttime[LT_ATTCK].ltime);
        return(0);
    }
    else if(ply_ptr->lasttime[LT_SPELL].ltime+2 > t) {
        please_wait(fd, 3-t+ply_ptr->lasttime[LT_SPELL].ltime);
        return(0);
    }
    else {
        ply_ptr->lasttime[LT_MOVED].ltime = t;
        ply_ptr->lasttime[LT_MOVED].misc = 1;
    }
*/

/*
    if(!strcmp(str,"¤¤¤µ") || !strcmp(str,"¤µ¤¤") || !strcmp(str,"24") || !strcmp(str,"42") || !strcmp(str,"¼­³²"))
        strcpy(tempstr, "³²¼­");
    else if(!strcmp(str,"¤²¤µ") || !strcmp(str,"¤µ¤²") || !strcmp(str,"84") || !strcmp(str,"48") || !strcmp(str,"¼­ºÏ"))
        strcpy(tempstr, "ºÏ¼­");
    else if(!strcmp(str,"¤¤¤§") || !strcmp(str,"¤§¤¤") || !strcmp(str,"26") || !strcmp(str,"62") || !strcmp(str,"µ¿³²"))
        strcpy(tempstr, "³²µ¿");
    else if(!strcmp(str,"¤²¤§") || !strcmp(str,"¤§¤²") || !strcmp(str,"86") || !strcmp(str,"68") || !strcmp(str,"µ¿ºÏ"))
        strcpy(tempstr, "ºÏµ¿");
    else  */
         if(str[0]=='2' || !strcmp(str,"É¥£¢") || !strcmp(str,"¤¤")) strcpy(tempstr, "³²");
    else if(str[0]=='4' || !strcmp(str,"É¬£¢") || !strcmp(str,"¤µ")) strcpy(tempstr, "¼­");
    else if(str[0]=='6' || !strcmp(str,"É¦£¢") || !strcmp(str,"¤§")) strcpy(tempstr, "µ¿");
    else if(str[0]=='8' || !strcmp(str,"Éª£¢") || !strcmp(str,"¤²")) strcpy(tempstr, "ºÏ");
    else if(str[0]=='3' || !strcmp(str,"É©£¢") || !strcmp(str,"¤±")) strcpy(tempstr, "¹Ø");
    else if(str[0]=='9' || !strcmp(str,"É®£¢") || !strcmp(str,"¤·")) strcpy(tempstr, "À§");
    else if(!strcmp(str,"³ª°¡")) strcpy(tempstr, "¹Û");
    else strcpy(tempstr, str);

    xp = rom_ptr->first_ext;
    while(xp) {
        if(!strcmp(xp->ext->name, tempstr) &&
            !F_ISSET(xp->ext,XNOSEE)){
            found = 1;
            break;
        }
        xp = xp->next_tag;
    }

    if(!found) {
        print(fd, "±æÀÌ ¸·Çô ÀÖ½À´Ï´Ù.");
        return(0);
    }

    if(F_ISSET(ply_ptr, PSILNC)) {
    	print(fd, "´ç½ÅÀº ¿òÁ÷ÀÏ¼ö ¾ø½À´Ï´Ù.");
    	return(0);
    }

	if(ply_is_attacking(ply_ptr, cmnd)) {
		print(fd, "½Î¿ì´Â Áß¿¡´Â ÀÌµ¿ÇÒ ¼ö ¾ø½À´Ï´Ù.");
		return(0);
	}

/*
    if(!F_ISSET(ply_ptr, PFLYSP) || !F_ISSET(ply_ptr, PLEVIT)) {
    		dmg = mrand(1,200);
    }
    else 	dmg = mrand(1,400);

    if(dmg==30 || dmg==93 || dmg==75 || dmg==100 || dmg==150 || 
       dmg==200 || dmg == 120 || dmg == 4 || dmg == 8 || dmg== 105 ||
       dmg == 127) {
        print(fd, "µ¹¿¡ °É·Á ³Ñ¾îÁ³½À´Ï´Ù. Äô!");
        return(0);
    }
*/

    if(F_ISSET(xp->ext, XLOCKD)) {
        print(fd, "¹®ÀÌ Àá°Ü ÀÖ½À´Ï´Ù.");
        return(0);
    }
    else if(F_ISSET(xp->ext, XCLOSD)) {
        print(fd, "¹®ÀÌ ´ÝÇô ÀÖ½À´Ï´Ù.");
        return(0);
    }

    if(F_ISSET(xp->ext, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
        print(fd, "±× ÂÊÀ¸·Î´Â ³¯¾Æ¼­ °¡¾ß µÉ°Í °°±º¿ä.");
        return(0);
    }

    t = Time%24L;
    if(F_ISSET(xp->ext, XNGHTO) && (t>6 && t < 20)) {
        print(fd, "±× Ãâ±¸´Â ¹ã¿¡¸¸ ¿­·Á ÀÖ½À´Ï´Ù.");
        return(0);
    }

    if(F_ISSET(xp->ext, XDAYON) && (t<6 || t > 20)){
        print(fd, "±× Ãâ±¸´Â ¹ã¿¡´Â ´ÝÇô ÀÖ½À´Ï´Ù.");
        return(0);
    }

    if(F_ISSET(xp->ext,XPGUAR)){
    cp = rom_ptr->first_mon;
    while(cp) {
        if(F_ISSET(cp->crt, MPGUAR)) {
        if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < SUB_DM) {
            print(fd, "%M%j ´ç½ÅÀÇ ±æÀ» ¸·½À´Ï´Ù.", cp->crt,"1");
            return(0);
        }
        if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < SUB_DM) {
            print(fd, "%M%j ´ç½ÅÀÇ ±æÀ» ¸·½À´Ï´Ù.", cp->crt,"1");
            return(0);
        }
        }
        cp = cp->next_tag;
    }
    }

    if(F_ISSET(xp->ext, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
        print(fd, "¿©¼º¸¸ µé¾î°¥¼ö ÀÖ½À´Ï´Ù. ¿©ÅÁÀÎ°¡~~");
        return(0);
    }

    if(F_ISSET(xp->ext, XMALES) && !F_ISSET(ply_ptr, PMALES)){
        print(fd, "³²¼º¸¸ µé¾î°¥¼ö ÀÖ½À´Ï´Ù.");
        return(0);
    }

    if(F_ISSET(xp->ext, XNAKED) && weight_ply(ply_ptr)) {
        print(fd, "¹» °¡Áö°í´Â µé¾î°¥¼ö ¾ø½À´Ï´Ù.");
        return(0);
    }

    if((F_ISSET(xp->ext, XCLIMB) || F_ISSET(xp->ext, XREPEL)) &&
       !F_ISSET(ply_ptr, PLEVIT)) {
        fall = (F_ISSET(xp->ext, XDCLIM) ? 50:0) + 50 -
               fall_ply(ply_ptr);

        if(mrand(1,100) < fall) {
            dmg = mrand(5,15+fall/10);
            if(ply_ptr->hpcur <= dmg){
                print(fd, "´ç½ÅÀº Á×À½ÀÌ ´Ù°¡¿À´Â°Í°°Àº ´À³¦ÀÌ µì´Ï´Ù.");
                ply_ptr->hpcur=0;
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j ±¸µ¢ÀÌ¿¡ ¶³¾îÁ®¼­ Á×¾ú½À´Ï´Ù.", ply_ptr,"1");
                die(ply_ptr, ply_ptr);
                return(0);
            }
            ply_ptr->hpcur -= dmg;
            print(fd, "´ç½ÅÀº ±¸µ¢ÀÌ¿¡ ¶³¾îÁ®¼­ %d ¸¸Å­ÀÇ »óÃ³¸¦ ÀÔ¾ú½À´Ï´Ù",
                  dmg);
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j ±¸µ¢ÀÌ¿¡ ¶³¾îÁ³½À´Ï´Ù.",
                      ply_ptr,"1");
            if(F_ISSET(xp->ext, XCLIMB))
                return(0);
        }
    }

    if((ply_ptr->class == ASSASSIN || ply_ptr->class == THIEF || ply_ptr->class >
 INVINCIBLE) && (F_ISSET(ply_ptr, PHIDDN))) {

        chance = MIN(85, 5 + 6*((ply_ptr->level+3)/4) +
                3*bonus[ply_ptr->dexterity]);
        if(F_ISSET(ply_ptr, PBLIND))
                chance = MIN(20, chance);

        if(mrand(1,100) > chance)
        {
            print(fd,"´ç½ÅÀº Àº½Å¼úÀ» »ç¿ëÇÏ´Âµ¥ ½ÇÆÐÇÏ¿´½À´Ï´Ù.\n");
            F_CLR(ply_ptr, PHIDDN);

            cp = rom_ptr->first_mon;
            while(cp) {
                if(F_ISSET(cp->crt, MBLOCK) &&
                is_enm_crt(ply_ptr->name, cp->crt) &&
                F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < SUB_DM) {
                    print(fd, "%M°¡ ´ç½ÅÀÇ ±æÀ» °¡·Î¸·½À´Ï´Ù.\n", cp->crt);
                    return(0);
                    }
                cp = cp->next_tag;
            }
         }
     }
     else F_CLR(ply_ptr, PHIDDN);


    if(!F_ISSET(rom_ptr, RPTRAK))
        strcpy(rom_ptr->track, tempstr);

    old_rom_num = rom_ptr->rom_num;
    old_rom_ptr = rom_ptr;

    load_rom(xp->ext->room, &rom_ptr);
    if(rom_ptr == old_rom_ptr) {
        print(fd, "±×ÂÊÀ¸·Î Áöµµ°¡ ¾ø½À´Ï´Ù. ½Å¿¡°Ô ¿¬¶ôÇØ ÁÖ¼¼¿ä.");
        return(0);
    }

    n = count_vis_ply(rom_ptr);

    if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class <INVINCIBLE/*CARETAKER*/) {
        print(fd, "·¹º§ %d ÀÌ»óÀÌ¾î¾ß ±× °÷À¸·Î °¥ ¼ö ÀÖ½À´Ï´Ù.",
              rom_ptr->lolevel);
        return(0);
    }
    else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
        ply_ptr->class < CARETAKER) {
        print(fd, "±×°÷À¸·Î °¥·Á¸é ·¹º§ %dÀÌÇÏ¿©¾ß¸¸ ÇÕ´Ï´Ù.",
              rom_ptr->hilevel);
        return(0);
    }
    else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
        (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
        (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
        print(fd, "±× ¹æ¿¡ ÀÖ´Â »ç¿ëÀÚ°¡ ³Ê¹« ¸¹½À´Ï´Ù.");
        return(0);
    }
    else if((F_ISSET(rom_ptr, RFAMIL)) && !F_ISSET(ply_ptr, PFAMIL)) {
    	print(fd, "±×°÷¿¡´Â ÆÐ°Å¸® °¡ÀÔÀÚ¸¸ °¥ ¼ö ÀÖ½À´Ï´Ù. ");
    	return(0);
    }
    else if(ply_ptr->class < DM && (F_ISSET(rom_ptr, RONFML)) && (ply_ptr->daily[DL_EXPND].max != rom_ptr->special)) {
    	print(fd, "±×°÷Àº ´ç½ÅÀÌ °¥¼ö ¾ø´Â °÷ÀÔ´Ï´Ù.");
    	return(0);
    }
    else if(ply_ptr->class < DM && (F_ISSET(rom_ptr, RONMAR)) &&
    	(ply_ptr->daily[DL_MARRI].max != rom_ptr->special)) {
		sprintf(file, "%s/invite/invite_%d", PLAYERPATH, rom_ptr->special);
	if(file_exists(file)) {
	fp = fopen(file, "r");
	for(i=0; i<10; i++) {
		fscanf(fp, "%s", name[i]);
		if(!strcmp(ply_ptr->name, name[i])) {
			fclose(fp);
			goto mmm;
		}
	}
	fclose(fp);
	}
    	print(fd, "±×°÷Àº »çÀ¯ÁöÀÔ´Ï´Ù.");
    	return(0);
    }

mmm:
    if(t-login_time[fd]<120) login_time[fd]-=120;

    if(ply_ptr->class == DM && !F_ISSET(ply_ptr, PDMINV)){
        broadcast_rom(fd, old_rom_ptr->rom_num, "\n%MÀÌ %sÂÊÀ¸·Î °¬½À´Ï´Ù.",
                  ply_ptr, tempstr);
    }
    if(!F_ISSET(ply_ptr, PDMINV) && ply_ptr->class < DM) {
        broadcast_rom(fd, old_rom_ptr->rom_num, "\n%MÀÌ %sÂÊÀ¸·Î °¬½À´Ï´Ù.",
                  ply_ptr, tempstr);
    }

    del_ply_rom(ply_ptr, ply_ptr->parent_rom);
    add_ply_rom(ply_ptr, rom_ptr);

/*
    if(ply_ptr->class == RANGER){
             auto_search(ply_ptr);
    }
*/

    cp = ply_ptr->first_fol;
    while(cp) {
        if(cp->crt->rom_num == old_rom_num && cp->crt->type != MONSTER)
            move(cp->crt, cmnd);
        if(F_ISSET(cp->crt, MDMFOL) && cp->crt->rom_num == old_rom_num && cp->crt->type == MONSTER) {
        del_crt_rom(cp->crt, old_rom_ptr);
        broadcast_rom(fd, old_rom_ptr->rom_num, "\n%MÀÌ %sÂÊÀ¸·Î °¬½À´Ï´Ù.", cp->crt, tempstr);
        add_crt_rom(cp->crt, rom_ptr, 1);
        add_active(cp->crt);
        }
    cp = cp->next_tag;
    }

    if(is_rom_loaded(old_rom_num)) {
        cp = old_rom_ptr->first_mon;
        while(cp) {
            if(!F_ISSET(cp->crt, MFOLLO) || F_ISSET(cp->crt, MDMFOL)) {
                /* Åõ¸íÀÏ¶§´Â ¾Èµû¶ó°¡°Ô..*/
                if((!F_ISSET(cp->crt,MDINVI) &&
                    F_ISSET(ply_ptr,PINVIS)) || F_ISSET(ply_ptr,PDMINV)) {
                    cp = cp->next_tag;
                    continue;
                }
            }
            if(!cp->crt->first_enm) {
                cp = cp->next_tag;
                continue;
            }
            if(strcmp(cp->crt->first_enm->enemy, ply_ptr->name)) {
                cp = cp->next_tag;
                continue;
            }
            if(mrand(1,50) > 15 - ply_ptr->dexterity +
               cp->crt->dexterity) {
                cp = cp->next_tag;
                continue;
            }
            print(fd, "\n%M%j ´ç½ÅÀ» µû¶ó¿É´Ï´Ù.", cp->crt,"1");
            broadcast_rom(fd, old_rom_num, "\n%M%j %M%j µû¶ó°©´Ï´Ù.",
                      cp->crt, "1", ply_ptr,"3");
            temp = cp->next_tag;
            crt_ptr = cp->crt;
            if(F_ISSET(crt_ptr, MPERMT))
                die_perm_crt(ply_ptr, crt_ptr);
            del_crt_rom(crt_ptr, old_rom_ptr);
            add_crt_rom(crt_ptr, rom_ptr, 1);
            add_active(crt_ptr);
            F_CLR(crt_ptr, MPERMT);
            cp = temp;

        }
    }

    check_traps(ply_ptr, rom_ptr);
    return(0);
}

/**********************************************************************/
/*                              say                                   */
/**********************************************************************/

/* This function allows the player specified by the socket descriptor */
/* in the first parameter to say something to all the other people in */
/* the room.                                                          */

/* say = ¸» */
int say(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room            *rom_ptr;
    int             index = -1, i, fd;
    int             len;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    len = strlen(cmnd->fullstr);
    if(cmnd->fullstr[len-1]==' ' || cmnd->fullstr[len-1]=='.' || \
       cmnd->fullstr[len-1]=='!' || cmnd->fullstr[len-1]=='?') index=len;
    else    for(i=len-1; i>=0; i--) {
            if(cmnd->fullstr[i] == ' ') {
                cmnd->fullstr[i]=0;
                index = i;
                break;
            }
        }

    cmnd->fullstr[255] = 0;

    if(index == -1 || strlen(&cmnd->fullstr[0]) ==0) {
        print(fd, "¹» ¸»ÇÏ°í ½ÍÀ¸¼¼¿ä?");
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
        print(fd, "¸»À» ÇØ º¸¾ÒÁö¸¸ ÀÌ ¹æ ¹ÛÀÇ »ç¶÷µéÀº µé¸®Áö ¾Ê´Âµí ÇÏ±º¿ä.");
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);
    if(F_ISSET(ply_ptr, PLECHO)){
        print(fd, "´ç½ÅÀº \"%s\"¶ó°í ¸»ÇÕ´Ï´Ù.", &cmnd->fullstr[0]);
    }
    else
        print(fd, "¿¹. ÁÁ½À´Ï´Ù.");

    broadcast_rom(fd, rom_ptr->rom_num, "\n%C%M%j \"%s\"¶ó°í ¸»ÇÕ´Ï´Ù.%D",
              "36",ply_ptr,"1", &cmnd->fullstr[0],"37");

    cmnd->fullstr[index]=' ';
    return(0);

}

/**********************************************************************/
/*                              get                                   */
/**********************************************************************/

/* This function allows players to get things lying on the floor or   */
/* inside another object.                                             */

int get(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room            *rom_ptr;
    object          *obj_ptr, *cnt_ptr, *obj_temp;
    ctag            *cp;
    int             fd, n, match=0;
    int cnt=0,i;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "¹» ÁÖ¿ì½Ã°Ô¿ä?");
        return(0);
    }

    rom_ptr = ply_ptr->parent_rom;
    F_CLR(ply_ptr, PHIDDN);

    for(i=0,cnt=0; i<MAXWEAR; i++)
        if(ply_ptr->ready[i]) cnt++;
    cnt += count_inv(ply_ptr, -1);

    if(cmnd->num == 2) {

        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MGUARD))
                break;
            cp = cp->next_tag;
        }

        obj_temp=NULL;
        obj_temp = find_obj(ply_ptr, rom_ptr->first_obj,
                   "ÃÊÀÎÀÇ µ¹", cmnd->val[1]);
        if(obj_temp && ply_ptr->class < CARETAKER) {
            if(obj_temp->value == 1001) {
                print(fd, "ÃÊÀÎÀÇ µ¹ÀÇ ÈûÀÌ ´ç½ÅÀÌ ¹«¾ð°¡¸¦ ÁÝ´Â°ÍÀ» ¹æÇØÇÕ´Ï´Ù.");
                return(0);
            }   
        }    

        if(cp && ply_ptr->class < CARETAKER) {
            print(fd, "%M%j ´ç½ÅÀÌ ¾î¶²°ÍÀ» ÁÝ´Â °ÍÀ» ¹æÇØÇÕ´Ï´Ù.", cp->crt,"1");
            return(0);
        }
        if(F_ISSET(ply_ptr, PBLIND)) {
            print(fd, "±×·± °Ç º¸ÀÌÁö ¾Ê½À´Ï´Ù.");
            return(0);
        }
        if(!strcmp(cmnd->str[1], "¸ðµÎ")) {
            get_all_rom(ply_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "¸ðµç",4)) {
            get_all_rom(ply_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "±×·±°Ç ¿©±â ¾ø¾î¿ä.");
            return(0);
        }

        if(F_ISSET(obj_ptr, OINVIS)) {
            print(fd, "±×·±°Ç ¿©±â ¾ø¾î¿ä.");
            return(0);
        }

        if(F_ISSET(obj_ptr, ONOTAK) || F_ISSET(obj_ptr, OSCENE)) {
            print(fd, "ÁÖÀ» ¼ö ÀÖ´Â ¹°°ÇÀÌ ¾Æ´Õ´Ï´Ù.");
            return(0);
        }

        if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
           max_weight(ply_ptr) || cnt>150) {
            print(fd, "´ç½ÅÀº ´õÀÌ»ó °¡Áú ¼ö ¾ø½À´Ï´Ù.\n");
            return(0);
        }

        if(obj_ptr->questnum && Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
            print(fd, "´ç½ÅÀº ±×°ÍÀ» ÁÖ¿ï¼ö ¾ø½À´Ï´Ù. %s.",
                  "ÀÌ¹Ì ´ç½ÅÀº ÀÓ¹«¸¦ ¿Ï¼öÇÏ¿´½À´Ï´Ù.");
            return(0);
        }

        if(F_ISSET(obj_ptr, OTEMPP)) {
            F_CLR(obj_ptr, OPERM2);
            F_CLR(obj_ptr, OTEMPP);
        }

        if(F_ISSET(obj_ptr, OPERMT))
            get_perm_obj(obj_ptr);

        F_CLR(obj_ptr, OHIDDN);
        del_obj_rom(obj_ptr, rom_ptr);
        print(fd, "´ç½ÅÀº %1i%j ÁÝ½À´Ï´Ù.", obj_ptr,"3");
        if(obj_ptr->questnum) {
            print(fd, "ÀÓ¹«¸¦ ¿Ï¼öÇÏ¿´½À´Ï´Ù. ¹ö¸®Áö ¸¶½Ê½Ã¿ä!.");
            print(fd, "¹ö¸®¸é ´Ù½Ã´Â ÁÖ¿ï ¼ö°¡ ¾ø½À´Ï´Ù.");
            Q_SET(ply_ptr, obj_ptr->questnum-1);
            ply_ptr->experience += quest_exp[obj_ptr->questnum-1];
            print(fd, "´ç½ÅÀº °æÇèÄ¡ %ld¸¦ ¹Þ¾Ò½À´Ï´Ù.",
                quest_exp[obj_ptr->questnum-1]);
            add_prof(ply_ptr,quest_exp[obj_ptr->questnum-1]);
        }
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j ÁÝ½À´Ï´Ù.",
                  ply_ptr, "1", obj_ptr,"3");

        if(obj_ptr->type == MONEY) {
            ply_ptr->gold += obj_ptr->value;
            free_obj(obj_ptr);
            print(fd, "\n´ç½ÅÀº ÀÌÁ¦ %ld³ÉÀ» °®°í ÀÖ½À´Ï´Ù.",
                ply_ptr->gold);
        }
        else
            add_obj_crt(obj_ptr, ply_ptr);
        return(0);
    }

    else {

        cnt_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);
/* by. latok */
        if(!cnt_ptr)
            cnt_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
                       cmnd->str[1], cmnd->val[1]);

        if(!cnt_ptr || !cmnd->val[1]) {
            for(n=0; n<MAXWEAR; n++) {
                if(!ply_ptr->ready[n]) continue;
                if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
                    match++;
                else continue;
                if(match == cmnd->val[1] || !cmnd->val[1]) {
                    cnt_ptr = ply_ptr->ready[n];
                    break;
                }
            }
        }

        if(!cnt_ptr) {
            print(fd, "±×·±°ÍÀº º¸ÀÌÁö ¾Ê½À´Ï´Ù.");
            return(0);
        }

        if(!F_ISSET(cnt_ptr, OCONTN)) {
            print(fd, "±×°ÍÀº ´ã´Â Á¾·ù°¡ ¾Æ´Õ´Ï´Ù.");
            return(0);
        }

        if(!strcmp(cmnd->str[2], "¸ðµÎ")) {
            get_all_obj(ply_ptr, cnt_ptr,cmnd->str[2]);
            return(0);
        }
        if(!strncmp(cmnd->str[2], "¸ðµç",4)) {
            get_all_obj(ply_ptr, cnt_ptr,cmnd->str[2]+4);
            return(0);
        }
        obj_ptr = find_obj(ply_ptr, cnt_ptr->first_obj,
                   cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
            print(fd, "±× ¾È¿¡ ±×·±°ÍÀº ¾ø¾î¿ä.");
            return(0);
        }

        if((weight_ply(ply_ptr) + weight_obj(obj_ptr) >
           max_weight(ply_ptr) && cnt_ptr->parent_rom) || cnt>150) {
            print(fd, "´ç½ÅÀº ´õÀÌ»ó °¡Áú ¼ö ¾ø½À´Ï´Ù.");
            return(0);
        }

        if(F_ISSET(obj_ptr, OPERMT))
            get_perm_obj(obj_ptr);

        cnt_ptr->shotscur--;
        del_obj_obj(obj_ptr, cnt_ptr);
        print(fd, "´ç½ÅÀº %1i¿¡¼­ %1i%j ²¨³À´Ï´Ù.", cnt_ptr, obj_ptr,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%MÀÌ %1i¿¡¼­ %1i%j ²¨³À´Ï´Ù.",
                  ply_ptr, cnt_ptr, obj_ptr,"3");

	if(F_ISSET(cnt_ptr, OPERMT)) {
		resave_rom(rom_ptr->rom_num);
#ifdef NODUPLE
		savegame_nomsg(ply_ptr);
#endif
	}

        if(obj_ptr->type == MONEY) {
            ply_ptr->gold += obj_ptr->value;
            free_obj(obj_ptr);
            print(fd, "\n´ç½ÅÀº ÀÌÁ¦ %ld³ÉÀ» °¡Áö°í ÀÖ½À´Ï´Ù.",
                ply_ptr->gold);
        }
        else
            add_obj_crt(obj_ptr, ply_ptr);

        return(0);
    }

}

/**********************************************************************/
/*                              get_all_rom                           */
/**********************************************************************/

/* This function will cause the player pointed to by the first parameter */
/* to get everything he is able to see in the room.                      */

void get_all_rom(ply_ptr,part_obj)
creature        *ply_ptr;
char *part_obj;
{
    room    *rom_ptr;
    object  *obj_ptr, *last_obj;
    otag    *op;
    char    str[2048];
    int     fd, n = 1, found = 0, heavy = 0, dogoldmsg = 0;
    int index=1;
    int i,cnt;

        for(i=0,cnt=0; i<MAXWEAR; i++)
                if(ply_ptr->ready[i]) cnt++;
        cnt += count_inv(ply_ptr, -1);


    if(!strcmp(part_obj,"¸ðµÎ")) {
        index=0;
    }
    last_obj = 0; str[0] = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    op = rom_ptr->first_obj;
    while(op) {
        if(!F_ISSET(op->obj, OSCENE) &&
           !F_ISSET(op->obj, ONOTAK) && !F_ISSET(op->obj, OHIDDN) &&
           (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS))) {

            if(index) {
                obj_ptr = find_obj(ply_ptr, op, part_obj, 1);
                if(!obj_ptr) break;

                while(op->obj!=obj_ptr) op=op->next_tag;
                op=op->next_tag;
                found++;
            }
            else {
                found++;
                obj_ptr = op->obj;
                op = op->next_tag;
            }
            if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
              max_weight(ply_ptr) || cnt>150) {
                heavy++;
                continue;
            }
            if(obj_ptr->questnum &&
               Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
                heavy++;
                continue;
            }
            if(F_ISSET(obj_ptr, OTEMPP)) {
                F_CLR(obj_ptr, OPERM2);
                F_CLR(obj_ptr, OTEMPP);
            }
            if(F_ISSET(obj_ptr, OPERMT))
                get_perm_obj(obj_ptr);
            F_CLR(obj_ptr, OHIDDN);
            cnt++;
            if(obj_ptr->questnum) {
                print(fd,"ÀÓ¹«¸¦ ¿Ï¼öÇÏ¿´½À´Ï´Ù! ¹ö¸®Áö ¸¶½Ê½Ã¿ä!.\n");
                print(fd,"¹ö¸®¸é ´Ù½Ã ÁÖ¿ï ¼ö ¾ø½À´Ï´Ù.");
                Q_SET(ply_ptr, obj_ptr->questnum-1);
                ply_ptr->experience +=
                    quest_exp[obj_ptr->questnum-1];
                print(fd, "\n´ç½ÅÀº °æÇèÄ¡ %ld À» ¹Þ¾Ò½À´Ï´Ù.",
                    quest_exp[obj_ptr->questnum-1]);
            add_prof(ply_ptr,quest_exp[obj_ptr->questnum-1]);
            }
            del_obj_rom(obj_ptr, rom_ptr);
            if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
               last_obj->adjustment == obj_ptr->adjustment)
                n++;
            else if(last_obj) {
                strcat(str, obj_str(last_obj, n, 0));
                strcat(str, ", ");
                n=1;
            }
            if(obj_ptr->type == MONEY) {
                strcat(str, obj_str(obj_ptr, 1, 0));
                strcat(str, ", ");
                ply_ptr->gold += obj_ptr->value;
                free_obj(obj_ptr);
                last_obj = 0;
                dogoldmsg = 1;
            }
            else {
                add_obj_crt(obj_ptr, ply_ptr);
                last_obj = obj_ptr;
            }
        }
        else
            op = op->next_tag;
    }

    if(found && last_obj)
        strcat(str, obj_str(last_obj, n, 0));
    else if(!found) {
        print(fd, "¿©±â¿¡´Â ¾Æ¹«°Íµµ ¾ø½À´Ï´Ù.");
        return;
    }

    if(dogoldmsg && !last_obj)
        str[strlen(str)-2] = 0;

    if(heavy) {
        print(fd, "°¡Áö°í ÀÖ´Â°ÍÀÌ ³Ê¹« ¹«°Å¿ö ´õÀÌ»ó °¡Áú ¼ö°¡ ¾ø½À´Ï´Ù.\n");
        if(heavy == found) return;
    }

    if(!strlen(str)) return;

    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %S%j ÁÝ½À´Ï´Ù.", ply_ptr, "1",str,"3");
    print(fd, "´ç½ÅÀº %S%j ÁÝ½À´Ï´Ù.", str,"3");
    if(dogoldmsg)
        print(fd, "\n´ç½ÅÀº ÀÌÁ¦ %ld³ÉÀ» °¡Áö°í ÀÖ½À´Ï´Ù.",
            ply_ptr->gold);

}

/**********************************************************************/
/*                              get_all_obj                           */
/**********************************************************************/

/* This function allows a player to get the entire contents from a        */
/* container object.  The player is pointed to by the first parameter and */
/* the container by the second.                                           */

void get_all_obj(ply_ptr, cnt_ptr,part_obj)
creature        *ply_ptr;
object          *cnt_ptr;
char *part_obj;
{
    room    *rom_ptr;
    object  *obj_ptr, *last_obj;
    otag    *op;
    char    str[2048];
    int     fd, n = 1, found = 0, heavy = 0;
    int index=1;
    int i,cnt;

        for(i=0,cnt=0; i<MAXWEAR; i++)
                if(ply_ptr->ready[i]) cnt++;
        cnt += count_inv(ply_ptr, -1);

    if(!strcmp(part_obj,"¸ðµÎ")) {
        index=0;
    }
    last_obj = 0; str[0] = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    op = cnt_ptr->first_obj;
    while(op) {
        if(!F_ISSET(op->obj, OSCENE) &&
           !F_ISSET(op->obj, ONOTAK) && !F_ISSET(op->obj, OHIDDN) &&
           (F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS))) {
            if(index) {
                obj_ptr = find_obj(ply_ptr, op, part_obj, 1);
                if(!obj_ptr) break;
                while(op->obj!=obj_ptr) op=op->next_tag;
                op=op->next_tag;
                found++;
            }
            else {
                found++;
                obj_ptr = op->obj;
                op = op->next_tag;
            }

            if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
               max_weight(ply_ptr) || cnt>150) {
                heavy++;
                continue;
            }
            if(F_ISSET(obj_ptr, OTEMPP)) {
                F_CLR(obj_ptr, OPERM2);
                F_CLR(obj_ptr, OTEMPP);
            }
            if(F_ISSET(obj_ptr, OPERMT))
                get_perm_obj(obj_ptr);
            cnt_ptr->shotscur--;
            cnt++;
            del_obj_obj(obj_ptr, cnt_ptr);
            if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
                last_obj->adjustment == obj_ptr->adjustment)
                n++;
            else if(last_obj) {
                strcat(str, obj_str(last_obj, n, 0));
                strcat(str, ", ");
                n = 1;
            }
            if(obj_ptr->type == MONEY) {
                ply_ptr->gold += obj_ptr->value;
                free_obj(obj_ptr);
                last_obj = 0;
                print(fd, "\n´ç½ÅÀº ÀÌÁ¦ %ld³ÉÀ» °¡Áö°í ÀÖ½À´Ï´Ù.",
                    ply_ptr->gold);
            }
            else {
                add_obj_crt(obj_ptr, ply_ptr);
                last_obj = obj_ptr;
            }
        }
        else
            op = op->next_tag;
    }

    if(found && last_obj)
        strcat(str, obj_str(last_obj, n, 0));
    else if(!found) {
        print(fd, "±× ¾È¿¡´Â ¾Æ¹«°Íµµ ¾ø½À´Ï´Ù.");
        return;
    }

    if(heavy) {
        print(fd, "°¡Áö°í ÀÖ´Â ¹°°ÇÀÌ ³Ê¹« ¹«°Å¿ö µé ¼ö°¡ ¾ø½À´Ï´Ù.\n");
        if(heavy == found) return;
    }

    if(!strlen(str)) return;

    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i¿¡¼­ %S%j ²¨³À´Ï´Ù.",
              ply_ptr, "1",cnt_ptr, str,"3");
    print(fd, "´ç½ÅÀº %1i¿¡¼­ %S%j ²¨³À´Ï´Ù.", cnt_ptr, str,"3");
    if(F_ISSET(cnt_ptr, OPERMT)) {
    	resave_rom(rom_ptr->rom_num);
#ifdef NODUPLE
    	savegame_nomsg(ply_ptr);
#endif
    }

}

/**********************************************************************/
/*                              get_perm_obj                          */
/**********************************************************************/

/* This function is called whenever someone picks up a permanent item */
/* from a room.  The item's room-permanent flag is cleared, and the   */
/* inventory-permanent flag is set.  Also, the room's permanent       */
/* time for that item is updated.                                     */

void get_perm_obj(obj_ptr)
object  *obj_ptr;
{
    object  *temp_obj;
    room    *rom_ptr;
    long    t;
    int     i;

    t = time(0);

    F_SET(obj_ptr, OPERM2);
    F_CLR(obj_ptr, OPERMT);

    rom_ptr = obj_ptr->parent_rom;
    if(!rom_ptr) return;

    for(i=0; i<10; i++) {
        if(!rom_ptr->perm_obj[i].misc) continue;
        if(rom_ptr->perm_obj[i].ltime + rom_ptr->perm_obj[i].interval >
           t) continue;
        if(load_obj(rom_ptr->perm_obj[i].misc, &temp_obj) < 0)
            continue;
        if(!strcmp(temp_obj->name, obj_ptr->name)) {
            rom_ptr->perm_obj[i].ltime = t;
            free_obj(temp_obj);
            break;
        }
        free_obj(temp_obj);
    }
}

/**********************************************************************/
/*                              inventory                             */
/**********************************************************************/

/* This function outputs the contents of a player's inventory.        */

int inventory(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    otag            *op;
    char            str[2048];
    int             m, n, fd, flags = 0;

    fd = ply_ptr->fd;

    if(F_ISSET(ply_ptr, PBLIND)) {
        ANSI(fd, BLUE);
        print(fd, "´ç½ÅÀº ´«ÀÌ ¸Ö¾î¼­ ¾Æ¹«°Íµµ º¼ ¼ö°¡ ¾ø½À´Ï´Ù!");
        ANSI(fd, WHITE);
        return(0);
    }
    if(F_ISSET(ply_ptr, PDINVI))
        flags |= INV;
    if(F_ISSET(ply_ptr, PDMAGI))
        flags |= MAG;

    op = ply_ptr->first_obj; n=0; str[0]=0;
    strcat(str, "¼ÒÁöÇ°:\n  ");
    if(!op) {
        strcat(str, "¾øÀ½.");
        print(fd, "%s", str);
        return(0);
    }
    while(op) {
        if(F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) {
            m=1;
            while(op->next_tag) {
                if(!strcmp(op->next_tag->obj->name,
                       op->obj->name) &&
                   op->next_tag->obj->adjustment ==
                   op->obj->adjustment &&
                   (F_ISSET(ply_ptr, PDINVI) ?
                   1:!F_ISSET(op->next_tag->obj, OINVIS))) {
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
        print(fd, "%s.", str);
    }

    return(0);

}

/**********************************************************************/
/*                              drop                                  */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to drop an object in the room at which he is located.             */
void drop_money();

int drop(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room            *rom_ptr;
    otag            *otag_ptr;
    object          *obj_ptr, *cnt_ptr;
    int             fd, n, match=0;
    int len;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "¹«¾ùÀ» ¹ö¸®½Ç·Á±¸¿ä?");
        return(0);
    }

    rom_ptr = ply_ptr->parent_rom;
    F_CLR(ply_ptr, PHIDDN);

    if(cmnd->num == 2) {

        len=strlen(cmnd->str[1]);
        if(len>2 && !strcmp(&cmnd->str[1][len-2],"³É")) {
            drop_money(ply_ptr, cmnd);
            return(0);
        }
        if(!strcmp(cmnd->str[1], "¸ðµÎ")) {
            drop_all_rom(ply_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "¸ðµç",4)) {
            drop_all_rom(ply_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "´ç½ÅÀº ±×·±°ÍÀ» °®°í ÀÖÁö ¾Ê½À´Ï´Ù.");
            return(0);
        }
/*               ÀÓ¹«¾ÆÀÌÅÛ ¹ö¸®Áö ¸øÇÏ°Ô ÇÔ */

        if(obj_ptr->questnum && ply_ptr->class<DM) {
            print(fd, "ÀÓ¹« ¾ÆÀÌÅÛÀº ¹ö¸®Áö ¸øÇÕ´Ï´Ù.");
            return(0);
        }
       if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < DM) {
       		print(fd, "ÀÌº¥Æ® ¾ÆÀÌÅÛÀº ¹ö¸®Áö ¸øÇÕ´Ï´Ù.");
       		return(0);
       	} 

       if(obj_ptr->first_obj) {
           for(otag_ptr=obj_ptr->first_obj; otag_ptr; otag_ptr=otag_ptr->next_tag) {
                if(otag_ptr->obj->questnum && ply_ptr->class<DM) {
                    print(fd, "ÀÓ¹« ¾ÆÀÌÅÛÀÌ µé¾îÀÖÀ¸¸é ¹ö¸®Áö ¸øÇÕ´Ï´Ù.");
                    return(0);
                }
               if(F_ISSET(otag_ptr->obj, ONEWEV) && ply_ptr->class < DM) {
       	        	print(fd, "ÀÌº¥Æ® ¾ÆÀÌÅÛÀÌ µé¾îÀÖÀ¸¸é ¹ö¸®Áö ¸øÇÕ´Ï´Ù.");
       		        return(0);
               } 
           }
       }

        del_obj_crt(obj_ptr, ply_ptr);
#ifdef NODUPLE
        savegame_nomsg(ply_ptr);
#endif
        print(fd, "´ç½ÅÀº %1i%j ¹ö·È½À´Ï´Ù.", obj_ptr,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j ¹ö·È½À´Ï´Ù.",
                  ply_ptr,"1", obj_ptr,"3");
        if(!F_ISSET(rom_ptr, RDUMPR))
            add_obj_rom(obj_ptr, rom_ptr);
        else {
            free_obj(obj_ptr);
            ply_ptr->gold += 10;
            ply_ptr->experience += 2;
            print(fd, "\n´ç½ÅÀÇ ¹°°ÇÀ» Á¦¹°·Î ¹ÙÃÆ½À´Ï´Ù.\n´ç½ÅÀº ¾à°£ÀÇ »ó±Ý°ú °æÇèÀ» ¹Þ¾Ò½À´Ï´Ù.");
        }
        return(0);
    }

    else {

        cnt_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[2], cmnd->val[2]);
/**** ¼öÁ¤¿ä±¸ÇÔ ****/
        if(!cnt_ptr)
            cnt_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
                        cmnd->str[2], cmnd->val[2]);

        if(!cnt_ptr || !cmnd->val[2]) {
            for(n=0; n<MAXWEAR; n++) {
                if(!ply_ptr->ready[n]) continue;
                if(EQUAL(ply_ptr->ready[n], cmnd->str[2]))
                    match++;
                else continue;
                if(match == cmnd->val[2] || !cmnd->val[2]) {
                    cnt_ptr = ply_ptr->ready[n];
                    break;
                }
            }
        }

        if(!cnt_ptr) {
            print(fd, "±×·± ¹°°ÇÀº ¾ø½À´Ï´Ù.");
            return(0);
        }

        if(!F_ISSET(cnt_ptr, OCONTN)) {
            print(fd, "±×°ÍÀº ´ãÀ»¼ö ÀÖ´Â°ÍÀÌ ¾Æ´Õ´Ï´Ù.");
            return(0);
        }

        if(!strcmp(cmnd->str[1], "¸ðµÎ")) {
            drop_all_obj(ply_ptr, cnt_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "¸ðµç",4)) {
            drop_all_obj(ply_ptr, cnt_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "´ç½ÅÀº ±×·±°ÍÀ» °®°í ÀÖÁö ¾Ê½À´Ï´Ù.");
            return(0);
        }

        if(obj_ptr == cnt_ptr) {
            print(fd, "±×°ÍÀ» ±×°Í ÀÚ½ÅÇÑÅ×´Â ³ÖÀ»¼ö ¾ø½À´Ï´Ù.");
            return(0);
        }

        if(obj_ptr->questnum && ply_ptr->class<DM) {
            print(fd, "ÀÓ¹« ¾ÆÀÌÅÛÀº ¹ö¸®Áö ¸øÇÕ´Ï´Ù.");
            return(0);
        }
       if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < DM) {
            print(fd, "ÀÌº¥Æ® ¾ÆÀÌÅÛÀº ¹ö¸®Áö ¸øÇÕ´Ï´Ù.");
            return(0);
        } 

        if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
            print(fd, "%I¾È¿¡ ´õÀÌ»ó ³ÖÀ» ¼ö ¾ø½À´Ï´Ù.\n", cnt_ptr);
            return(0);
        }

        if(F_ISSET(obj_ptr, OCONTN)) {
            print(fd, "´ãÀ»¼ö ÀÖ´Â ¹°°Ç ¾È¿¡ ´ãÀ» ¼ö ÀÖ´Â ¹°°ÇÀº ³ÖÀ» ¼ö ¾ø½À´Ï´Ù.\n");
        }

        if(F_ISSET(cnt_ptr, OCNDES)) {
        print(fd, "%1i%j %1i%j »ïÄÑ ¹ö·Á ÈçÀûµµ ¾øÀÌ »ç¶óÁý´Ï´Ù!\n", obj_ptr, "3",cnt_ptr,"1");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%MÀÌ %1i%j %1i¿¡ ³Ö½À´Ï´Ù.\n",
                  ply_ptr, obj_ptr,"3", cnt_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        free(obj_ptr);
            return(0);
        }

        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_obj(obj_ptr, cnt_ptr);
        cnt_ptr->shotscur++;
        print(fd, "´ç½ÅÀº %1i%j %1i ¾È¿¡ ³Ö½À´Ï´Ù.\n", obj_ptr,"3", cnt_ptr);
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j %1i¾È¿¡ ³Ö¾ú½À´Ï´Ù.",
                  ply_ptr, "1", obj_ptr,"3", cnt_ptr);
        if(F_ISSET(cnt_ptr, OPERMT) && F_ISSET(ply_ptr->parent_rom, RBANK)) {
        	resave_rom(rom_ptr->rom_num);
#ifdef NODUPLE
        	savegame_nomsg(ply_ptr);
#endif
        }
        return(0);
    }

}


void drop_money(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        object		*obj_ptr;
        long            amt;
        int             fd;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;

        amt = atol(cmnd->str[1]);
        if(amt < 1) {
                print(fd, "µ·ÀÇ ´ÜÀ§´Â À½¼ö°¡ µÉ¼ö ¾ø½À´Ï´Ù.");
                return;
        }
        if(amt > ply_ptr->gold) {
                print(fd, "´ç½ÅÀº ±×¸¸Å­ÀÇ µ·À» °¡Áö°í ÀÖÁö ¾Ê½À´Ï´Ù.");
                return;
        }

        load_obj(0, &obj_ptr);
        sprintf(obj_ptr->name, "%d³É", amt);
        obj_ptr->value = amt;
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);

        ply_ptr->gold -= amt;
#ifdef NODUPLE
        savegame_nomsg(ply_ptr);
#endif


        print(fd, "´ç½ÅÀº %ld³ÉÀ» ¹ö·È½À´Ï´Ù.\n",amt);

        broadcast_rom2(fd, ply_ptr->fd, ply_ptr->rom_num,
                       "\n%M%j %ld³ÉÀ» ¹ö·È½À´Ï´Ù.", ply_ptr,"1", amt);

}

/**********************************************************************/
/*                              drop_all_rom                          */
/**********************************************************************/

/* This function is called when a player wishes to drop his entire    */
/* inventory into the the room.                                       */

void drop_all_rom(ply_ptr,part_obj)
creature        *ply_ptr;
char *part_obj;
{
    object  *obj_ptr,*last_obj=NULL;
    room    *rom_ptr;
    otag    *op;
    char    str[2048], str2[2048];
    int     fd, n=1,found=0;
    int index=1;

    if(!strcmp(part_obj,"¸ðµÎ")) index=0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    str[0]=0;

    if(check_event_obj(ply_ptr)) {
        print(fd, "ÀÓ¹« ¾ÆÀÌÅÛÀÌ ÀÖ¾î ¸ðµÎ ¹ö¸± ¼ö ¾ø½À´Ï´Ù.\n");
        return;
    }    

    op = ply_ptr->first_obj;
    while(op) {
        if(F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) {
            if(index) {
                obj_ptr = find_obj(ply_ptr, op, part_obj, 1);
                if(!obj_ptr) break;
                while(op->obj!=obj_ptr) op=op->next_tag;
                op=op->next_tag;
                found++;
            }
            else {
                found++;
                obj_ptr = op->obj;
                op = op->next_tag;
            }

            if(obj_ptr->questnum && ply_ptr->class < DM) continue;
            if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < DM) continue;
            if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
               last_obj->adjustment == obj_ptr->adjustment)
                n++;
            else if(last_obj) {
                strcat(str, obj_str(last_obj, n, 0));
                strcat(str, ", ");
                n=1;
                if(F_ISSET(rom_ptr, RDUMPR)) free_obj(last_obj);
            }
            del_obj_crt(obj_ptr, ply_ptr);
            if(!F_ISSET(rom_ptr, RDUMPR)) {
                add_obj_rom(obj_ptr, rom_ptr);
                last_obj=obj_ptr;
            }
            else {
                if(n!=1) free_obj(obj_ptr);
                else last_obj=obj_ptr;
                ply_ptr->gold+=10;
            }
        }
        else
            op = op->next_tag;
    }
    if(found && last_obj) {
        strcat(str, obj_str(last_obj, n, 0));
        if(F_ISSET(rom_ptr, RDUMPR)) free_obj(last_obj);
    }
    else {
        print(fd, "´ç½ÅÀº ¾Æ¹«°Íµµ °¡Áö°í ÀÖÁö ¾Ê½À´Ï´Ù.");
        return;
    }

#ifdef NODUPLE
    savegame_nomsg(ply_ptr);
#endif

    broadcast_rom(fd, rom_ptr->rom_num, "\n%MÀÌ %S%j ¹ö·È½À´Ï´Ù.", ply_ptr, str,"3");
    print(fd, "´ç½ÅÀº %S%j ¹ö·È½À´Ï´Ù.", str,"3");

    if(F_ISSET(rom_ptr, RDUMPR))
        print(fd, "\n´ç½ÅÀÇ ¹°°ÇÀ» Á¦¹°·Î ¹ÙÃÆ½À´Ï´Ù.\n´ç½ÅÀº ¾à°£ÀÇ »ó±ÝÀ» ¹Þ¾Ò½À´Ï´Ù.");
}

/**********************************************************************/
/*                              drop_all_obj                          */
/**********************************************************************/

/* This function drops all the items in a player's inventory into a      */
/* container object, if possible.  The player is pointed to by the first */
/* parameter, and the container by the second.                           */

void drop_all_obj(ply_ptr, cnt_ptr,part_obj)
creature        *ply_ptr;
object          *cnt_ptr;
char *part_obj;
{
    object  *obj_ptr, *last_obj;
    room    *rom_ptr;
    otag    *op;
    char    str[2048];
    int     fd, n = 1, found = 0, full = 0;
    int index=1;

    if(!strcmp(part_obj,"¸ðµÎ")) {
        index=0;
    }
    fd = ply_ptr->fd;

    last_obj = 0; str[0] = 0;
    rom_ptr = ply_ptr->parent_rom;

    op = ply_ptr->first_obj;
    while(op) {
        if((F_ISSET(ply_ptr, PDINVI) ? 1:!F_ISSET(op->obj, OINVIS)) &&
           op->obj != cnt_ptr) {
            if(index) {
                obj_ptr = find_obj(ply_ptr, op, part_obj, 1);
                if(!obj_ptr) break;
                while(op->obj!=obj_ptr) op=op->next_tag;
                op=op->next_tag;
                found++;
            }
            else {
                found++;
                obj_ptr = op->obj;
                op = op->next_tag;
            }

            if(obj_ptr->questnum && ply_ptr->class<DM) continue;
            if(F_ISSET(obj_ptr, ONEWEV)) continue;
            if(F_ISSET(obj_ptr, OCONTN)) {
                full++;
                continue;
            }
            if(F_ISSET(cnt_ptr, OCNDES))
            {
                full++;
                del_obj_crt(obj_ptr, ply_ptr);
                free(obj_ptr);
                continue;

            }

            if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
                full++;
                continue;
            }
            cnt_ptr->shotscur++;
            del_obj_crt(obj_ptr, ply_ptr);
            add_obj_obj(obj_ptr, cnt_ptr);
            if(last_obj && !strcmp(last_obj->name, obj_ptr->name) &&
               last_obj->adjustment == obj_ptr->adjustment)
                n++;
            else if(last_obj) {
                strcat(str, obj_str(last_obj, n, 0));
                strcat(str, ", ");
                n = 1;
            }
            last_obj = obj_ptr;
        }
        else
            op = op->next_tag;
    }

    if(found && last_obj)
        strcat(str, obj_str(last_obj, n, 0));
    else {
        print(fd, "´ç½ÅÀº ±×°Í ¾È¿¡ ³ÖÀ» ¹°°ÇÀ» ¾Æ¹«°Íµµ °®°í ÀÖÁö ¾Ê½À´Ï´Ù.");
        return;
    }

    if(full)
        print(fd, "%I¾È¿¡ ´õÀÌ»ó ¹°°ÇÀ» ³ÖÀ» ¼ö ¾ø½À´Ï´Ù.", cnt_ptr);

    if(full == found) return;
#ifdef NODUPLE
    savegame_nomsg(ply_ptr);
#endif


    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %S%j %1i¾È¿¡ ³Ö½À´Ï´Ù.", ply_ptr,"1",
              str,"3",cnt_ptr);
    print(fd, "´ç½ÅÀº %S%j %1i¾È¿¡ ³Ö½À´Ï´Ù.", str,"3", cnt_ptr);
    if(F_ISSET(cnt_ptr, OPERMT)) {
    	resave_rom(rom_ptr->rom_num);
#ifdef NODUPLE
    	savegame_nomsg(ply_ptr);
#endif
    }

}

/**********************************************************************/
/*                            burnÅÂ¿ö                                */
/**********************************************************************/

/* Á¦»çÀåÀÌ ¾Æ´Ñ°÷¿¡¼­µµ ¼Ò°¢ÇÒ¼ö ÀÖ°ÔÇÏ´Â ¸í·ÉÀÌ´Ù  */
long ply_burn_time[PMAX];

int burn(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room            *rom_ptr;
    object          *obj_ptr, *cnt_ptr;
    int             fd, n, match=0;
    long t;

    fd = ply_ptr->fd;

    if(cmnd->num < 2) {
        print(fd, "¹«¾ùÀ» ÅÂ¿ì½Ã·Á±¸¿ä?");
        return(0);
    }
    t=time(0);
    if(ply_burn_time[fd]>t) {
        please_wait(fd,ply_burn_time[fd]-t);
        return 0;
    }

    rom_ptr = ply_ptr->parent_rom;
    F_CLR(ply_ptr, PHIDDN);


    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr) {
        print(fd, "´ç½ÅÀº ±×·±°ÍÀ» °®°í ÀÖÁö ¾Ê½À´Ï´Ù.");
        return(0);
    }
	if(F_ISSET(obj_ptr, ONOBUN)) {
		print(fd, "¼Ò°¢ÇÒ¼ö ¾ø´Â ¾ÆÀÌÅÛÀÔ´Ï´Ù.");
		return(0);
	}
/*               ÀÓ¹«¾ÆÀÌÅÛ ¹ö¸®Áö ¸øÇÏ°Ô ÇÔ */
    if(obj_ptr->questnum && ply_ptr->class<SUB_DM && obj_ptr->shotscur > 0) {
        print(fd, "ÀÓ¹« ¾ÆÀÌÅÛÀº ÅÂ¿ìÁö ¸øÇÕ´Ï´Ù.");
        return(0);
    }
	if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < SUB_DM && obj_ptr->shotscur > 0) {
		print(fd, "ÀÌº¥Æ® ¾ÆÀÌÅÛÀº ¼Ò°¢ÇÒ¼ö ¾ø½À´Ï´Ù.");
		return(0);
	}
    del_obj_crt(obj_ptr, ply_ptr);
    print(fd, "´ç½ÅÀº %1i%j ÅÂ¿ü½À´Ï´Ù.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j ÅÂ¿ü½À´Ï´Ù.",
              ply_ptr,"1", obj_ptr,"3");
    free_obj(obj_ptr);
    ply_ptr->gold += 4;
    ply_ptr->experience += 1;
    print(fd, "\n´ç½ÅÀº ¾à°£ÀÇ »ó±Ý°ú °æÇèÀ» ¹Þ¾Ò½À´Ï´Ù.");
    if(((time(0)+mrand(1,100))%250)==9) {
        print(fd,"\n½ÅÀÌ ´ç½ÅÀÇ Á¤¼ºÀÌ °¼¸¤ÇØ¼­ °æÇèÄ¡¿Í µ·º­¶ôÀ» ³»¸³´Ï´Ù.");
        broadcast_rom(fd,rom_ptr->rom_num,"\n½ÅÀÌ %M¿¡°Ô °æÇèÄ¡¿Í µ·º­¶ôÀ» ³»¸³´Ï´Ù.",ply_ptr);
        ply_ptr->gold+=100000;
        ply_ptr->experience+=10;
    }
    ply_burn_time[fd]=t+2;

    return(0);
}

/**********************************************************************/
/*                                auto_search                         */
/**********************************************************************/

/* ÀÚµ¿ ¼ö»ö  */

int auto_search(ply_ptr)
creature    *ply_ptr;
{
    room    *rom_ptr;
    xtag    *xp;
    otag    *op;
    ctag    *cp;
    int fd, chance, found = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    chance = 15 + 5*bonus[(int)ply_ptr->piety] + (ply_ptr->level/4)*2;
    chance = MIN(chance, 90);
    if(ply_ptr->class == RANGER)
        chance += ((ply_ptr->level+3)/4)*8;
    if(F_ISSET(ply_ptr, PBLIND))
        chance = MIN(chance, 20);


    xp = rom_ptr->first_ext;
    while(xp) {
        if(F_ISSET(xp->ext, XSECRT) && mrand(1,100) <= chance)
           if((!F_ISSET(xp->ext, XINVIS) || F_ISSET(ply_ptr,PDINVI))
            && !F_ISSET(xp->ext, XNOSEE)){
            found++;
            print(fd, "\nÃâ±¸¸¦ Ã£¾Ò½À´Ï´Ù: %s.", xp->ext->name);
        }
        xp = xp->next_tag;
    }

    op = rom_ptr->first_obj;
    while(op) {
        if(F_ISSET(op->obj, OHIDDN) && mrand(1,100) <= chance)
        if(!F_ISSET(op->obj, OINVIS) || F_ISSET(ply_ptr,PDINVI)) { 
            found++;
            print(fd, "\n´ç½ÅÀº %1i%j Ã£¾Ò½À´Ï´Ù.", op->obj,"3");
        }
        op = op->next_tag;
    }

    cp = rom_ptr->first_ply;
    while(cp) {
        if(F_ISSET(cp->crt, PHIDDN) && !F_ISSET(cp->crt, PDMINV) &&
           mrand(1,100) <= chance)
        if(!F_ISSET(cp->crt, PINVIS) || F_ISSET(ply_ptr,PDINVI)) {
            found++;
            print(fd, "\n´ç½ÅÀº ¼û¾îÀÖ´Â %S%j Ã£¾Æ³»¾ú½À´Ï´Ù.", cp->crt->name,"3");
        }
        cp = cp->next_tag;
    }

    cp = rom_ptr->first_mon;
    while(cp) {
        if(F_ISSET(cp->crt, MHIDDN) && mrand(1,100) <= chance)
        if(!F_ISSET(cp->crt, MINVIS) || F_ISSET(ply_ptr,PDINVI)) {
            found++;
            print(fd, "\n´ç½ÅÀº ¼û¾îÀÖ´Â %1M%j Ã£¾Æ³»¾ú½À´Ï´Ù.", cp->crt,"3");
        }
        cp = cp->next_tag;
    }

    if(found)
        broadcast_rom(fd, ply_ptr->rom_num, "\n%s°¡ ¹» ¹ß°ßÇÑ°Í °°±º¿ä!",
                  F_ISSET(ply_ptr, MMALES) ? "±×":"±×³à");

    return(0);

}


