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
        print(fd, "����� ���� �־� �ֽ��ϴ�!");
        ANSI(fd, WHITE);
        ANSI(fd, NORMAL);
        return(0);
    }
    rom_ptr = ply_ptr->parent_rom;
	ext_ptr=find_ext(ply_ptr, rom_ptr->first_ext, 
					cmnd->str[1], cmnd->val[1]);
	if(ext_ptr) {
		if(F_ISSET(ext_ptr, XCLOSD)) {
			print(fd, "�� �ⱸ�� ���� �ֽ��ϴ�.");
			return(0);
		}
		sprintf(file, "%s/r%02d/r%05d", ROOMPATH,ext_ptr->room/1000, ext_ptr->room);
		if(!file_exists(file)) {
			print(fd, "������ �����ϴ�.");
			return(0);
		}
		load_rom(ext_ptr->room, &new_rom);
		if(!new_rom || rom_ptr == new_rom) {
			print(fd, "������ �����ϴ�.");
			return(0);
		}
		if(F_ISSET(new_rom, RONMAR) || F_ISSET(new_rom, RONFML)) {
			print(fd, "�� ���� �� ���� �����ϴ�.");
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
            print(fd, "Ư���� ���� �����ϴ�.\n");

        if(F_ISSET(ply_ptr, PKNOWA)) {
            if(F_ISSET(obj_ptr, OGOODO))
                print(fd, "Ǫ�� ��ä�� ���� ������ �ֽ��ϴ�.\n");
            if(F_ISSET(obj_ptr, OEVILO))
                print(fd, "���� ��ä�� ���� ������ �ֽ��ϴ�.\n");
        }

        if(F_ISSET(obj_ptr, OCONTN)) {
            strcpy(str, "���빰: ");
            n = list_obj(&str[8], ply_ptr, obj_ptr->first_obj);
            if(n)
                print(fd, "%s.\n", str);
        }

        if(obj_ptr->type <= MISSILE) {
            print(fd, "%I%j ", obj_ptr,"0");
            switch(obj_ptr->type) {
            case SHARP: print(fd, "�ſ� ��ī�ο� '��'�Դϴ�.\n"); break;
            case THRUST: print(fd, "�ſ� �������� '��'�Դϴ�.\n"); break;
            case POLE: print(fd, "���� ��¦ �� 'â'�Դϴ�.\n"); break;
            case BLUNT: print(fd, "�ſ� �������� '��'�Դϴ�.\n"); break;
            case MISSILE: print(fd, "�ſ� �����ϰ� ���̴� '��'�Դϴ�.\n"); break;
            }
        }

        if(obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR ||
           obj_ptr->type == LIGHTSOURCE || obj_ptr->type == WAND ||
           obj_ptr->type == KEY) {
            if(obj_ptr->shotscur < 1)
                print(fd, "�װ��� �μ��� ���Ȱų� �� ����Ƚ��ϴ�.\n");
            else if(obj_ptr->shotscur <= obj_ptr->shotsmax/10)
                print(fd, "�װ��� �� �μ����� �����ϴ�.\n");
        }

        return(0);
    }

    if(!strcmp(cmnd->str[1],"��")) crt_ptr=ply_ptr;
    else crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
               cmnd->val[1]);
    if(crt_ptr) {

        if(!strncmp(crt_ptr->name, ply_ptr->name, strlen(crt_ptr->name))) {
          print(fd,"����� �ſ��� ��� �ڽ��� ���ϴ�.\n");
          broadcast_rom(fd,rom_ptr->rom_num,
             "\n%M%j �ſ��� ��� �ڽ��� �ٶ� ���ϴ�.",ply_ptr,"1");
        }
        else {
          print(fd, "����� %1M%j ���ϴ�.\n", crt_ptr,"3");
          broadcast_rom(fd,rom_ptr->rom_num,
             "\n%M%j %M%j ���ϴ�.",ply_ptr,"1",crt_ptr,"3");
        }
        if(crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PMARRI)) {
            print(fd, "%s�� %s�԰� ��ȥ�� ��ȥ���Դϴ�.\n", 
                    F_ISSET(crt_ptr, PMALES) ? "��":"�׳�", &crt_ptr->key[2][1]);
        }            
        
		if(crt_ptr->type != PLAYER) {
        if(crt_ptr->description[0])
            print(fd, "%s\n", crt_ptr->description);
        else
            print(fd, "Ư���� ���� ������ �ʽ��ϴ�.\n");
		}
		else {
			print(fd, "%s�� %s�� �ֽ��ϴ�.\n", F_ISSET(crt_ptr, PMALES) ? "��":"�׳�",
				crt_ptr->description);
		}
        if(F_ISSET(ply_ptr, PKNOWA) && crt_ptr->alignment!=0) {
            print(fd, "%s���Լ� ",
                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            if(crt_ptr->alignment < 0)
                print(fd, "���� ��ä");
            else print(fd, "Ǫ�� ��ä");
            print(fd, "�� ���� ������ �ֽ��ϴ�.\n");
        }
        if(crt_ptr->hpcur < (crt_ptr->hpmax*9)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*8)/10) )
            print(fd, "%s�� ������ ��ó�� �Ծ����ϴ�.\n",
                  F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*8)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*6)/10) )
            print(fd, "%s�� �������� ��ó�� �Ծ����ϴ�.\n",
                  F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*6)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*4)/10) )
            print(fd, "%s�� ���� ��ó�� �Ծ����ϴ�.\n",
                  F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*4)/10 && (crt_ptr->hpcur > (crt_ptr->hpmax*2)/10) )
            print(fd, "%s�� �ɰ��� ��ó�� �Ծ����ϴ�.\n",
                  F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
        if(crt_ptr->hpcur < (crt_ptr->hpmax*2)/10)
            print(fd, "%s�� �ױ� �����Դϴ�.\n",
                  F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
        if(is_enm_crt(ply_ptr->name, crt_ptr))
            print(fd, "%s�� ��ſ��� �ſ� ȭ�� ���� �����ϴ�.\n",
                  F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
        if(crt_ptr->first_enm) {
            if(!strcmp(crt_ptr->first_enm->enemy, ply_ptr->name))
                print(fd, "%s�� ��Ű� �ο�� �ֽ��ϴ�.\n",
                      F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            else
                print(fd, "%s�� %S%j �ο�� �ֽ��ϴ�.\n",
                      F_ISSET(crt_ptr, MMALES) ? "��":"�׳�",
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
        print(fd, "����� %M%j ���ϴ�.\n",crt_ptr,"3");
        broadcast_rom(fd,rom_ptr->rom_num,"\n%M%j %M%j ���ϴ�.",ply_ptr,"1",crt_ptr,"3");
        if(crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PMARRI)) {
            print(fd, "%s�� %s�԰� ��ȥ�� ��ȥ���Դϴ�.\n", 
                    F_ISSET(crt_ptr, PMALES) ? "��":"�׳�", &crt_ptr->key[2][1]);
        }            
		if(crt_ptr->type != PLAYER) {
        if(crt_ptr->description[0])
            print(fd, "%s\n", crt_ptr->description);
        else
            print(fd, "Ư���� ���� ������ �ʽ��ϴ�.\n");
		}
		else {
			print(fd, "%s�� %s�� �ֽ��ϴ�.\n", F_ISSET(crt_ptr, PMALES) ? "��":"�׳�",
				crt_ptr->description);
		}
        if(F_ISSET(ply_ptr, PKNOWA) && crt_ptr->alignment>=-100 && crt_ptr->alignment<101) {
            print(fd, "%s���Լ� ",
                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            if(crt_ptr->alignment < -100)
                print(fd, "���� ��ä");
            else print(fd, "Ǫ�� ��ä");
            print(fd, "�� ���� ������ �ֽ��ϴ�.\n");
        }
        if(crt_ptr->hpcur < (crt_ptr->hpmax*3)/10)
            print(fd, "%s�� ������ ��ó�� �Ծ����ϴ�.\n",
                  F_ISSET(crt_ptr, PMALES) ? "��":"�׳�");
        equip_list(fd, crt_ptr);
        return(0);
    }

    else
        print(fd, "�׷� �� ������ �ʽ��ϴ�.\n");

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

/* �̵� ������ ����.
     = ��(0);
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
    if(!strcmp(str,"����") || !strcmp(str,"����") || !strcmp(str,"24") || !strcmp(str,"42") || !strcmp(str,"����"))
        strcpy(tempstr, "����");
    else if(!strcmp(str,"����") || !strcmp(str,"����") || !strcmp(str,"84") || !strcmp(str,"48") || !strcmp(str,"����"))
        strcpy(tempstr, "�ϼ�");
    else if(!strcmp(str,"����") || !strcmp(str,"����") || !strcmp(str,"26") || !strcmp(str,"62") || !strcmp(str,"����"))
        strcpy(tempstr, "����");
    else if(!strcmp(str,"����") || !strcmp(str,"����") || !strcmp(str,"86") || !strcmp(str,"68") || !strcmp(str,"����"))
        strcpy(tempstr, "�ϵ�");
    else  */
         if(str[0]=='2' || !strcmp(str,"ɥ��") || !strcmp(str,"��")) strcpy(tempstr, "��");
    else if(str[0]=='4' || !strcmp(str,"ɬ��") || !strcmp(str,"��")) strcpy(tempstr, "��");
    else if(str[0]=='6' || !strcmp(str,"ɦ��") || !strcmp(str,"��")) strcpy(tempstr, "��");
    else if(str[0]=='8' || !strcmp(str,"ɪ��") || !strcmp(str,"��")) strcpy(tempstr, "��");
    else if(str[0]=='3' || !strcmp(str,"ɩ��") || !strcmp(str,"��")) strcpy(tempstr, "��");
    else if(str[0]=='9' || !strcmp(str,"ɮ��") || !strcmp(str,"��")) strcpy(tempstr, "��");
    else if(!strcmp(str,"����")) strcpy(tempstr, "��");
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
        print(fd, "���� ���� �ֽ��ϴ�.");
        return(0);
    }

    if(F_ISSET(ply_ptr, PSILNC)) {
    	print(fd, "����� �����ϼ� �����ϴ�.");
    	return(0);
    }

	if(ply_is_attacking(ply_ptr, cmnd)) {
		print(fd, "�ο�� �߿��� �̵��� �� �����ϴ�.");
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
        print(fd, "���� �ɷ� �Ѿ������ϴ�. ��!");
        return(0);
    }
*/

    if(F_ISSET(xp->ext, XLOCKD)) {
        print(fd, "���� ��� �ֽ��ϴ�.");
        return(0);
    }
    else if(F_ISSET(xp->ext, XCLOSD)) {
        print(fd, "���� ���� �ֽ��ϴ�.");
        return(0);
    }

    if(F_ISSET(xp->ext, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
        print(fd, "�� �����δ� ���Ƽ� ���� �ɰ� ������.");
        return(0);
    }

    t = Time%24L;
    if(F_ISSET(xp->ext, XNGHTO) && (t>6 && t < 20)) {
        print(fd, "�� �ⱸ�� �㿡�� ���� �ֽ��ϴ�.");
        return(0);
    }

    if(F_ISSET(xp->ext, XDAYON) && (t<6 || t > 20)){
        print(fd, "�� �ⱸ�� �㿡�� ���� �ֽ��ϴ�.");
        return(0);
    }

    if(F_ISSET(xp->ext,XPGUAR)){
    cp = rom_ptr->first_mon;
    while(cp) {
        if(F_ISSET(cp->crt, MPGUAR)) {
        if(!F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < SUB_DM) {
            print(fd, "%M%j ����� ���� �����ϴ�.", cp->crt,"1");
            return(0);
        }
        if(F_ISSET(cp->crt, MDINVI) && ply_ptr->class < SUB_DM) {
            print(fd, "%M%j ����� ���� �����ϴ�.", cp->crt,"1");
            return(0);
        }
        }
        cp = cp->next_tag;
    }
    }

    if(F_ISSET(xp->ext, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
        print(fd, "������ ���� �ֽ��ϴ�. �����ΰ�~~");
        return(0);
    }

    if(F_ISSET(xp->ext, XMALES) && !F_ISSET(ply_ptr, PMALES)){
        print(fd, "������ ���� �ֽ��ϴ�.");
        return(0);
    }

    if(F_ISSET(xp->ext, XNAKED) && weight_ply(ply_ptr)) {
        print(fd, "�� ������� ���� �����ϴ�.");
        return(0);
    }

    if((F_ISSET(xp->ext, XCLIMB) || F_ISSET(xp->ext, XREPEL)) &&
       !F_ISSET(ply_ptr, PLEVIT)) {
        fall = (F_ISSET(xp->ext, XDCLIM) ? 50:0) + 50 -
               fall_ply(ply_ptr);

        if(mrand(1,100) < fall) {
            dmg = mrand(5,15+fall/10);
            if(ply_ptr->hpcur <= dmg){
                print(fd, "����� ������ �ٰ����°Ͱ��� ������ ��ϴ�.");
                ply_ptr->hpcur=0;
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j �����̿� �������� �׾����ϴ�.", ply_ptr,"1");
                die(ply_ptr, ply_ptr);
                return(0);
            }
            ply_ptr->hpcur -= dmg;
            print(fd, "����� �����̿� �������� %d ��ŭ�� ��ó�� �Ծ����ϴ�",
                  dmg);
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j �����̿� ���������ϴ�.",
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
            print(fd,"����� ���ż��� ����ϴµ� �����Ͽ����ϴ�.\n");
            F_CLR(ply_ptr, PHIDDN);

            cp = rom_ptr->first_mon;
            while(cp) {
                if(F_ISSET(cp->crt, MBLOCK) &&
                is_enm_crt(ply_ptr->name, cp->crt) &&
                F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < SUB_DM) {
                    print(fd, "%M�� ����� ���� ���θ����ϴ�.\n", cp->crt);
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
        print(fd, "�������� ������ �����ϴ�. �ſ��� ������ �ּ���.");
        return(0);
    }

    n = count_vis_ply(rom_ptr);

    if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class <INVINCIBLE/*CARETAKER*/) {
        print(fd, "���� %d �̻��̾�� �� ������ �� �� �ֽ��ϴ�.",
              rom_ptr->lolevel);
        return(0);
    }
    else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
        ply_ptr->class < CARETAKER) {
        print(fd, "�װ����� ������ ���� %d���Ͽ��߸� �մϴ�.",
              rom_ptr->hilevel);
        return(0);
    }
    else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
        (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
        (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
        print(fd, "�� �濡 �ִ� ����ڰ� �ʹ� �����ϴ�.");
        return(0);
    }
    else if((F_ISSET(rom_ptr, RFAMIL)) && !F_ISSET(ply_ptr, PFAMIL)) {
    	print(fd, "�װ����� �аŸ� �����ڸ� �� �� �ֽ��ϴ�. ");
    	return(0);
    }
    else if(ply_ptr->class < DM && (F_ISSET(rom_ptr, RONFML)) && (ply_ptr->daily[DL_EXPND].max != rom_ptr->special)) {
    	print(fd, "�װ��� ����� ���� ���� ���Դϴ�.");
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
    	print(fd, "�װ��� �������Դϴ�.");
    	return(0);
    }

mmm:
    if(t-login_time[fd]<120) login_time[fd]-=120;

    if(ply_ptr->class == DM && !F_ISSET(ply_ptr, PDMINV)){
        broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M�� %s������ �����ϴ�.",
                  ply_ptr, tempstr);
    }
    if(!F_ISSET(ply_ptr, PDMINV) && ply_ptr->class < DM) {
        broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M�� %s������ �����ϴ�.",
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
        broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M�� %s������ �����ϴ�.", cp->crt, tempstr);
        add_crt_rom(cp->crt, rom_ptr, 1);
        add_active(cp->crt);
        }
    cp = cp->next_tag;
    }

    if(is_rom_loaded(old_rom_num)) {
        cp = old_rom_ptr->first_mon;
        while(cp) {
            if(!F_ISSET(cp->crt, MFOLLO) || F_ISSET(cp->crt, MDMFOL)) {
                /* �����϶��� �ȵ��󰡰�..*/
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
            print(fd, "\n%M%j ����� ����ɴϴ�.", cp->crt,"1");
            broadcast_rom(fd, old_rom_num, "\n%M%j %M%j ���󰩴ϴ�.",
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

/* say = �� */
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
        print(fd, "�� ���ϰ� ��������?");
        return(0);
    }
    if(F_ISSET(ply_ptr, PSILNC)) {
        print(fd, "���� �� �������� �� �� ���� ������� �鸮�� �ʴµ� �ϱ���.");
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);
    if(F_ISSET(ply_ptr, PLECHO)){
        print(fd, "����� \"%s\"��� ���մϴ�.", &cmnd->fullstr[0]);
    }
    else
        print(fd, "��. �����ϴ�.");

    broadcast_rom(fd, rom_ptr->rom_num, "\n%C%M%j \"%s\"��� ���մϴ�.%D",
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
        print(fd, "�� �ֿ�ðԿ�?");
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
                   "������ ��", cmnd->val[1]);
        if(obj_temp && ply_ptr->class < CARETAKER) {
            if(obj_temp->value == 1001) {
                print(fd, "������ ���� ���� ����� ���𰡸� �ݴ°��� �����մϴ�.");
                return(0);
            }   
        }    

        if(cp && ply_ptr->class < CARETAKER) {
            print(fd, "%M%j ����� ����� �ݴ� ���� �����մϴ�.", cp->crt,"1");
            return(0);
        }
        if(F_ISSET(ply_ptr, PBLIND)) {
            print(fd, "�׷� �� ������ �ʽ��ϴ�.");
            return(0);
        }
        if(!strcmp(cmnd->str[1], "���")) {
            get_all_rom(ply_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "���",4)) {
            get_all_rom(ply_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "�׷��� ���� �����.");
            return(0);
        }

        if(F_ISSET(obj_ptr, OINVIS)) {
            print(fd, "�׷��� ���� �����.");
            return(0);
        }

        if(F_ISSET(obj_ptr, ONOTAK) || F_ISSET(obj_ptr, OSCENE)) {
            print(fd, "���� �� �ִ� ������ �ƴմϴ�.");
            return(0);
        }

        if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
           max_weight(ply_ptr) || cnt>150) {
            print(fd, "����� ���̻� ���� �� �����ϴ�.\n");
            return(0);
        }

        if(obj_ptr->questnum && Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
            print(fd, "����� �װ��� �ֿ�� �����ϴ�. %s.",
                  "�̹� ����� �ӹ��� �ϼ��Ͽ����ϴ�.");
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
        print(fd, "����� %1i%j �ݽ��ϴ�.", obj_ptr,"3");
        if(obj_ptr->questnum) {
            print(fd, "�ӹ��� �ϼ��Ͽ����ϴ�. ������ ���ʽÿ�!.");
            print(fd, "������ �ٽô� �ֿ� ���� �����ϴ�.");
            Q_SET(ply_ptr, obj_ptr->questnum-1);
            ply_ptr->experience += quest_exp[obj_ptr->questnum-1];
            print(fd, "����� ����ġ %ld�� �޾ҽ��ϴ�.",
                quest_exp[obj_ptr->questnum-1]);
            add_prof(ply_ptr,quest_exp[obj_ptr->questnum-1]);
        }
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j �ݽ��ϴ�.",
                  ply_ptr, "1", obj_ptr,"3");

        if(obj_ptr->type == MONEY) {
            ply_ptr->gold += obj_ptr->value;
            free_obj(obj_ptr);
            print(fd, "\n����� ���� %ld���� ���� �ֽ��ϴ�.",
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
            print(fd, "�׷����� ������ �ʽ��ϴ�.");
            return(0);
        }

        if(!F_ISSET(cnt_ptr, OCONTN)) {
            print(fd, "�װ��� ��� ������ �ƴմϴ�.");
            return(0);
        }

        if(!strcmp(cmnd->str[2], "���")) {
            get_all_obj(ply_ptr, cnt_ptr,cmnd->str[2]);
            return(0);
        }
        if(!strncmp(cmnd->str[2], "���",4)) {
            get_all_obj(ply_ptr, cnt_ptr,cmnd->str[2]+4);
            return(0);
        }
        obj_ptr = find_obj(ply_ptr, cnt_ptr->first_obj,
                   cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
            print(fd, "�� �ȿ� �׷����� �����.");
            return(0);
        }

        if((weight_ply(ply_ptr) + weight_obj(obj_ptr) >
           max_weight(ply_ptr) && cnt_ptr->parent_rom) || cnt>150) {
            print(fd, "����� ���̻� ���� �� �����ϴ�.");
            return(0);
        }

        if(F_ISSET(obj_ptr, OPERMT))
            get_perm_obj(obj_ptr);

        cnt_ptr->shotscur--;
        del_obj_obj(obj_ptr, cnt_ptr);
        print(fd, "����� %1i���� %1i%j �����ϴ�.", cnt_ptr, obj_ptr,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M�� %1i���� %1i%j �����ϴ�.",
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
            print(fd, "\n����� ���� %ld���� ������ �ֽ��ϴ�.",
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


    if(!strcmp(part_obj,"���")) {
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
                print(fd,"�ӹ��� �ϼ��Ͽ����ϴ�! ������ ���ʽÿ�!.\n");
                print(fd,"������ �ٽ� �ֿ� �� �����ϴ�.");
                Q_SET(ply_ptr, obj_ptr->questnum-1);
                ply_ptr->experience +=
                    quest_exp[obj_ptr->questnum-1];
                print(fd, "\n����� ����ġ %ld �� �޾ҽ��ϴ�.",
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
        print(fd, "���⿡�� �ƹ��͵� �����ϴ�.");
        return;
    }

    if(dogoldmsg && !last_obj)
        str[strlen(str)-2] = 0;

    if(heavy) {
        print(fd, "������ �ִ°��� �ʹ� ���ſ� ���̻� ���� ���� �����ϴ�.\n");
        if(heavy == found) return;
    }

    if(!strlen(str)) return;

    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %S%j �ݽ��ϴ�.", ply_ptr, "1",str,"3");
    print(fd, "����� %S%j �ݽ��ϴ�.", str,"3");
    if(dogoldmsg)
        print(fd, "\n����� ���� %ld���� ������ �ֽ��ϴ�.",
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

    if(!strcmp(part_obj,"���")) {
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
                print(fd, "\n����� ���� %ld���� ������ �ֽ��ϴ�.",
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
        print(fd, "�� �ȿ��� �ƹ��͵� �����ϴ�.");
        return;
    }

    if(heavy) {
        print(fd, "������ �ִ� ������ �ʹ� ���ſ� �� ���� �����ϴ�.\n");
        if(heavy == found) return;
    }

    if(!strlen(str)) return;

    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i���� %S%j �����ϴ�.",
              ply_ptr, "1",cnt_ptr, str,"3");
    print(fd, "����� %1i���� %S%j �����ϴ�.", cnt_ptr, str,"3");
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
        print(fd, "����� ���� �־ �ƹ��͵� �� ���� �����ϴ�!");
        ANSI(fd, WHITE);
        return(0);
    }
    if(F_ISSET(ply_ptr, PDINVI))
        flags |= INV;
    if(F_ISSET(ply_ptr, PDMAGI))
        flags |= MAG;

    op = ply_ptr->first_obj; n=0; str[0]=0;
    strcat(str, "����ǰ:\n  ");
    if(!op) {
        strcat(str, "����.");
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
        print(fd, "������ �����Ƿ�����?");
        return(0);
    }

    rom_ptr = ply_ptr->parent_rom;
    F_CLR(ply_ptr, PHIDDN);

    if(cmnd->num == 2) {

        len=strlen(cmnd->str[1]);
        if(len>2 && !strcmp(&cmnd->str[1][len-2],"��")) {
            drop_money(ply_ptr, cmnd);
            return(0);
        }
        if(!strcmp(cmnd->str[1], "���")) {
            drop_all_rom(ply_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "���",4)) {
            drop_all_rom(ply_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "����� �׷����� ���� ���� �ʽ��ϴ�.");
            return(0);
        }
/*               �ӹ������� ������ ���ϰ� �� */

        if(obj_ptr->questnum && ply_ptr->class<DM) {
            print(fd, "�ӹ� �������� ������ ���մϴ�.");
            return(0);
        }
       if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < DM) {
       		print(fd, "�̺�Ʈ �������� ������ ���մϴ�.");
       		return(0);
       	} 

       if(obj_ptr->first_obj) {
           for(otag_ptr=obj_ptr->first_obj; otag_ptr; otag_ptr=otag_ptr->next_tag) {
                if(otag_ptr->obj->questnum && ply_ptr->class<DM) {
                    print(fd, "�ӹ� �������� ��������� ������ ���մϴ�.");
                    return(0);
                }
               if(F_ISSET(otag_ptr->obj, ONEWEV) && ply_ptr->class < DM) {
       	        	print(fd, "�̺�Ʈ �������� ��������� ������ ���մϴ�.");
       		        return(0);
               } 
           }
       }

        del_obj_crt(obj_ptr, ply_ptr);
#ifdef NODUPLE
        savegame_nomsg(ply_ptr);
#endif
        print(fd, "����� %1i%j ���Ƚ��ϴ�.", obj_ptr,"3");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j ���Ƚ��ϴ�.",
                  ply_ptr,"1", obj_ptr,"3");
        if(!F_ISSET(rom_ptr, RDUMPR))
            add_obj_rom(obj_ptr, rom_ptr);
        else {
            free_obj(obj_ptr);
            ply_ptr->gold += 10;
            ply_ptr->experience += 2;
            print(fd, "\n����� ������ ������ ���ƽ��ϴ�.\n����� �ణ�� ��ݰ� ������ �޾ҽ��ϴ�.");
        }
        return(0);
    }

    else {

        cnt_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[2], cmnd->val[2]);
/**** �����䱸�� ****/
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
            print(fd, "�׷� ������ �����ϴ�.");
            return(0);
        }

        if(!F_ISSET(cnt_ptr, OCONTN)) {
            print(fd, "�װ��� ������ �ִ°��� �ƴմϴ�.");
            return(0);
        }

        if(!strcmp(cmnd->str[1], "���")) {
            drop_all_obj(ply_ptr, cnt_ptr,cmnd->str[1]);
            return(0);
        }
        if(!strncmp(cmnd->str[1], "���",4)) {
            drop_all_obj(ply_ptr, cnt_ptr,cmnd->str[1]+4);
            return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                   cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
            print(fd, "����� �׷����� ���� ���� �ʽ��ϴ�.");
            return(0);
        }

        if(obj_ptr == cnt_ptr) {
            print(fd, "�װ��� �װ� �ڽ����״� ������ �����ϴ�.");
            return(0);
        }

        if(obj_ptr->questnum && ply_ptr->class<DM) {
            print(fd, "�ӹ� �������� ������ ���մϴ�.");
            return(0);
        }
       if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < DM) {
            print(fd, "�̺�Ʈ �������� ������ ���մϴ�.");
            return(0);
        } 

        if(cnt_ptr->shotscur >= cnt_ptr->shotsmax) {
            print(fd, "%I�ȿ� ���̻� ���� �� �����ϴ�.\n", cnt_ptr);
            return(0);
        }

        if(F_ISSET(obj_ptr, OCONTN)) {
            print(fd, "������ �ִ� ���� �ȿ� ���� �� �ִ� ������ ���� �� �����ϴ�.\n");
        }

        if(F_ISSET(cnt_ptr, OCNDES)) {
        print(fd, "%1i%j %1i%j ���� ���� ������ ���� ������ϴ�!\n", obj_ptr, "3",cnt_ptr,"1");
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M�� %1i%j %1i�� �ֽ��ϴ�.\n",
                  ply_ptr, obj_ptr,"3", cnt_ptr);
        del_obj_crt(obj_ptr, ply_ptr);
        free(obj_ptr);
            return(0);
        }

        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_obj(obj_ptr, cnt_ptr);
        cnt_ptr->shotscur++;
        print(fd, "����� %1i%j %1i �ȿ� �ֽ��ϴ�.\n", obj_ptr,"3", cnt_ptr);
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j %1i�ȿ� �־����ϴ�.",
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
                print(fd, "���� ������ ������ �ɼ� �����ϴ�.");
                return;
        }
        if(amt > ply_ptr->gold) {
                print(fd, "����� �׸�ŭ�� ���� ������ ���� �ʽ��ϴ�.");
                return;
        }

        load_obj(0, &obj_ptr);
        sprintf(obj_ptr->name, "%d��", amt);
        obj_ptr->value = amt;
        add_obj_rom(obj_ptr, ply_ptr->parent_rom);

        ply_ptr->gold -= amt;
#ifdef NODUPLE
        savegame_nomsg(ply_ptr);
#endif


        print(fd, "����� %ld���� ���Ƚ��ϴ�.\n",amt);

        broadcast_rom2(fd, ply_ptr->fd, ply_ptr->rom_num,
                       "\n%M%j %ld���� ���Ƚ��ϴ�.", ply_ptr,"1", amt);

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

    if(!strcmp(part_obj,"���")) index=0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    str[0]=0;

    if(check_event_obj(ply_ptr)) {
        print(fd, "�ӹ� �������� �־� ��� ���� �� �����ϴ�.\n");
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
        print(fd, "����� �ƹ��͵� ������ ���� �ʽ��ϴ�.");
        return;
    }

#ifdef NODUPLE
    savegame_nomsg(ply_ptr);
#endif

    broadcast_rom(fd, rom_ptr->rom_num, "\n%M�� %S%j ���Ƚ��ϴ�.", ply_ptr, str,"3");
    print(fd, "����� %S%j ���Ƚ��ϴ�.", str,"3");

    if(F_ISSET(rom_ptr, RDUMPR))
        print(fd, "\n����� ������ ������ ���ƽ��ϴ�.\n����� �ణ�� ����� �޾ҽ��ϴ�.");
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

    if(!strcmp(part_obj,"���")) {
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
        print(fd, "����� �װ� �ȿ� ���� ������ �ƹ��͵� ���� ���� �ʽ��ϴ�.");
        return;
    }

    if(full)
        print(fd, "%I�ȿ� ���̻� ������ ���� �� �����ϴ�.", cnt_ptr);

    if(full == found) return;
#ifdef NODUPLE
    savegame_nomsg(ply_ptr);
#endif


    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %S%j %1i�ȿ� �ֽ��ϴ�.", ply_ptr,"1",
              str,"3",cnt_ptr);
    print(fd, "����� %S%j %1i�ȿ� �ֽ��ϴ�.", str,"3", cnt_ptr);
    if(F_ISSET(cnt_ptr, OPERMT)) {
    	resave_rom(rom_ptr->rom_num);
#ifdef NODUPLE
    	savegame_nomsg(ply_ptr);
#endif
    }

}

/**********************************************************************/
/*                            burn�¿�                                */
/**********************************************************************/

/* �������� �ƴѰ������� �Ұ��Ҽ� �ְ��ϴ� ����̴�  */
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
        print(fd, "������ �¿�÷�����?");
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
        print(fd, "����� �׷����� ���� ���� �ʽ��ϴ�.");
        return(0);
    }
	if(F_ISSET(obj_ptr, ONOBUN)) {
		print(fd, "�Ұ��Ҽ� ���� �������Դϴ�.");
		return(0);
	}
/*               �ӹ������� ������ ���ϰ� �� */
    if(obj_ptr->questnum && ply_ptr->class<SUB_DM && obj_ptr->shotscur > 0) {
        print(fd, "�ӹ� �������� �¿��� ���մϴ�.");
        return(0);
    }
	if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < SUB_DM && obj_ptr->shotscur > 0) {
		print(fd, "�̺�Ʈ �������� �Ұ��Ҽ� �����ϴ�.");
		return(0);
	}
    del_obj_crt(obj_ptr, ply_ptr);
    print(fd, "����� %1i%j �¿����ϴ�.", obj_ptr,"3");
    broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %1i%j �¿����ϴ�.",
              ply_ptr,"1", obj_ptr,"3");
    free_obj(obj_ptr);
    ply_ptr->gold += 4;
    ply_ptr->experience += 1;
    print(fd, "\n����� �ణ�� ��ݰ� ������ �޾ҽ��ϴ�.");
    if(((time(0)+mrand(1,100))%250)==9) {
        print(fd,"\n���� ����� ������ �����ؼ� ����ġ�� �������� �����ϴ�.");
        broadcast_rom(fd,rom_ptr->rom_num,"\n���� %M���� ����ġ�� �������� �����ϴ�.",ply_ptr);
        ply_ptr->gold+=100000;
        ply_ptr->experience+=10;
    }
    ply_burn_time[fd]=t+2;

    return(0);
}

/**********************************************************************/
/*                                auto_search                         */
/**********************************************************************/

/* �ڵ� ����  */

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
            print(fd, "\n�ⱸ�� ã�ҽ��ϴ�: %s.", xp->ext->name);
        }
        xp = xp->next_tag;
    }

    op = rom_ptr->first_obj;
    while(op) {
        if(F_ISSET(op->obj, OHIDDN) && mrand(1,100) <= chance)
        if(!F_ISSET(op->obj, OINVIS) || F_ISSET(ply_ptr,PDINVI)) { 
            found++;
            print(fd, "\n����� %1i%j ã�ҽ��ϴ�.", op->obj,"3");
        }
        op = op->next_tag;
    }

    cp = rom_ptr->first_ply;
    while(cp) {
        if(F_ISSET(cp->crt, PHIDDN) && !F_ISSET(cp->crt, PDMINV) &&
           mrand(1,100) <= chance)
        if(!F_ISSET(cp->crt, PINVIS) || F_ISSET(ply_ptr,PDINVI)) {
            found++;
            print(fd, "\n����� �����ִ� %S%j ã�Ƴ������ϴ�.", cp->crt->name,"3");
        }
        cp = cp->next_tag;
    }

    cp = rom_ptr->first_mon;
    while(cp) {
        if(F_ISSET(cp->crt, MHIDDN) && mrand(1,100) <= chance)
        if(!F_ISSET(cp->crt, MINVIS) || F_ISSET(ply_ptr,PDINVI)) {
            found++;
            print(fd, "\n����� �����ִ� %1M%j ã�Ƴ������ϴ�.", cp->crt,"3");
        }
        cp = cp->next_tag;
    }

    if(found)
        broadcast_rom(fd, ply_ptr->rom_num, "\n%s�� �� �߰��Ѱ� ������!",
                  F_ISSET(ply_ptr, MMALES) ? "��":"�׳�");

    return(0);

}


