/*
 * COMMAND6.C:
 *
 *      Additional user routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              yell                                  */
/**********************************************************************/

/* This function allows a player to yell something that will be heard */
/* not only in his room, but also in all rooms adjacent to him.  In   */
/* the adjacent rooms, however, people will not know who yelled.      */

int yell(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room            *rom_ptr;
        xtag            *xp;
        char            str[300];
        int             index = -1, i, fd;
        int             len;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        len = strlen(cmnd->fullstr);
        for(i=0; i<len; i++) {
                if(cmnd->fullstr[i] == ' ') {
                        index = i+1;
                        break;
                }
        }
        str[256]=0;

        if(index == -1 || strlen(&cmnd->fullstr[index]) < 1) {
                print(fd, "�������� ��ġ������?");
                return(0);
        }
        if(F_ISSET(ply_ptr, PSILNC)) {
                print(fd, "����� ��Ҹ��� �ʹ� ���ؼ� ��ĥ�� �����ϴ�.");
                return(0);
        }
        F_CLR(ply_ptr, PHIDDN);
        print(fd, "��. �����ϴ�.");

        broadcast_rom(fd, rom_ptr->rom_num, "\n%M�� \"%s!\"��� ��Ĩ�ϴ�.", ply_ptr,
                      &cmnd->fullstr[0]);

        sprintf(str, "\n�������� \"%s!\"��� ���ƽ��ϴ�.", &cmnd->fullstr[0]);

        xp = rom_ptr->first_ext;
        while(xp) {
                if(is_rom_loaded(xp->ext->room))
                        broadcast_rom(fd, xp->ext->room, "%s", str);
                xp = xp->next_tag;
        }

        return(0);

}

/**********************************************************************/
/*                              go                                    */
/**********************************************************************/

/* This function allows a player to go through an exit, other than one */
/* of the 6 cardinal directions.                                       */

int go(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
		FILE			*fp;
        room            *rom_ptr, *old_rom_ptr;
        creature        *crt_ptr;
        ctag            *cp, *temp;
        exit_           *ext_ptr;
        long            i, t;
        int             fd, old_rom_num, fall, dmg, n, chance;
		char		file[80], name[10][15];
		int			j;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;


        if(cmnd->num < 2) {
                print(fd, "���� ���� ��������?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "�׷� �ⱸ�� �����ϴ�.");
                return(0);
        }

        if(F_ISSET(ply_ptr, PSILNC)) {
        	print(fd, "����� ������ ���� �����ϴ�.");
        	return(0);
        }

		if(ply_is_attacking(ply_ptr, cmnd)) {
			print(fd, "�ο�� �߿��� �̵��� �� �����ϴ�.");
			return(0);
		}

        if(F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "�� �ⱸ�� ��� �ֽ��ϴ�.");
                return(0);
        }
        else if(F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "�� �ⱸ�� ���� �ֽ��ϴ�.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP)) {
                print(fd, "�װ����� ���Ƽ��� �� �� �ֽ��ϴ�.");
                return(0);
        }

        t = Time%24L;
        if(F_ISSET(ext_ptr, XNGHTO) && (t>6 && t < 20)) {
                print(fd, "�� �ⱸ�� �㿡�� �� �� �ֽ��ϴ�.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XDAYON) && (t<6 || t > 20)) {
                print(fd, "�� �ⱸ�δ� ������ �� �� �ֽ��ϴ�.");
                return(0);
        }

    if(F_ISSET(ext_ptr,XPGUAR)){
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

        if(F_ISSET(ext_ptr, XNAKED) && weight_ply(ply_ptr)) {
                print(fd, "�� ������� �� �� �����ϴ�.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XFEMAL) && F_ISSET(ply_ptr, PMALES)){
                print(fd, "������ �� �� �ֽ��ϴ�. �����ΰ�~~");
                return(0);
        }

        if(F_ISSET(ext_ptr, XMALES) && !F_ISSET(ply_ptr, PMALES)){
                print(fd, "������ �� �� �ֽ��ϴ�.");
                return(0);
        }

        if((F_ISSET(ext_ptr, XCLIMB) || F_ISSET(ext_ptr, XREPEL)) &&
           !F_ISSET(ply_ptr, PLEVIT)) {
                fall = (F_ISSET(ext_ptr, XDCLIM) ? 50:0) + 50 -
                       fall_ply(ply_ptr);

                if(mrand(1,100) < fall) {
                        dmg = mrand(5, 15+fall/10);
                        if(ply_ptr->hpcur <= dmg){
                           print(fd, "����� ������ �ٰ����°Ͱ��� ������ ��ϴ�.");
                           ply_ptr->hpcur=0;
                           broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �����̿� �������� �׾����ϴ�.", ply_ptr);
                           die(ply_ptr, ply_ptr);
                           return(0);
                        }

                        ply_ptr->hpcur -= dmg;
                        print(fd, "����� �����̿� �������� %d ��ŭ�� ��ó�� �Ծ����ϴ�.",
                              dmg);
                        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �����̿� ���������ϴ�.",
                                      ply_ptr);

                        if(ply_ptr->hpcur < 1) {
                           print(fd, "����� ������ �ٰ����°� ���� ������ ��ϴ�.");
                               die(ply_ptr, ply_ptr);
                        }
                        if(F_ISSET(ext_ptr, XCLIMB))
                                return(0);
                }
        }

        i = LT(ply_ptr, LT_ATTCK);
        t = time(0);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
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
                strcpy(rom_ptr->track, ext_ptr->name);

        old_rom_num = rom_ptr->rom_num;
        old_rom_ptr = rom_ptr;

        load_rom(ext_ptr->room, &rom_ptr);
        if(rom_ptr == old_rom_ptr) {
                print(fd, "�� ������ ������ �����ϴ�.");
                return(0);
        }

        n = count_vis_ply(rom_ptr);

        if(rom_ptr->lolevel > ply_ptr->level && ply_ptr->class < INVINCIBLE/*CARETAKER*/) {
                print(fd, "���� %d �̻��̾�� �� ������ �� �� �ֽ��ϴ�.",
                      rom_ptr->lolevel);
                return(0);
        }
        else if(ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel &&
                ply_ptr->class < CARETAKER) {
                print(fd, "�װ����� ������ ���� %d���Ͽ��߸� �մϴ�.",
                      rom_ptr->hilevel+1);
                return(0);
        }
        else if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
                (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
                (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
                print(fd, "�� �濡 �ִ� ����ڰ� �ʹ� �����ϴ�.");
                return(0);
        }
        else if(F_ISSET(rom_ptr, RFAMIL) && !F_ISSET(ply_ptr, PFAMIL)) {
        	print(fd, "�װ����� �аŸ� �����ڸ� �� �� �ֽ��ϴ�.");
        	return(0);
        }
        else if(ply_ptr->class < DM && F_ISSET(rom_ptr, RONFML) && (ply_ptr->daily[DL_EXPND].max != rom_ptr->special)) {
        	print(fd, "�װ��� ����� �� �� ���� ���Դϴ�.");
        	return(0);
        }
        else if(ply_ptr->class < DM && F_ISSET(rom_ptr, RONMAR) &&
        	(ply_ptr->daily[DL_MARRI].max != rom_ptr->special)) {
		sprintf(file, "%s/invite/invite_%d", PLAYERPATH, rom_ptr->special);
	if(file_exists(file)) {
	fp = fopen(file, "r");
	for(j=0; j<10; j++) {
		fscanf(fp, "%s", name[j]);
		if(!strcmp(ply_ptr->name, name[j])) goto mmm;
	}
	fclose(fp);
	}
        	print(fd, "�װ��� �������Դϴ�.");
        	return(0);
        }
mmm:

        if(strcmp(ext_ptr->name, "��") && strcmp(ext_ptr->name, "��")
           && strcmp(ext_ptr->name, "��") && !F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M�� %s������ �����ϴ�.",
                              ply_ptr, ext_ptr->name);
        }

        else if(!F_ISSET(ply_ptr, PDMINV)) {
                broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M�� %s������ �����ϴ�.",
                              ply_ptr, ext_ptr->name);
        }

        del_ply_rom(ply_ptr, ply_ptr->parent_rom);
        add_ply_rom(ply_ptr, rom_ptr);

        cp = ply_ptr->first_fol;
        while(cp) {
                if(cp->crt->rom_num == old_rom_num && cp->crt->type != MONSTER)
                        go(cp->crt, cmnd);
                if(F_ISSET(cp->crt, MDMFOL) && cp->crt->type == MONSTER) {
                del_crt_rom(cp->crt, old_rom_ptr);
                broadcast_rom(fd, old_rom_ptr->rom_num, "\n%M%j ��Ȳ�ϴ� %s������ �����ϴ�.", cp->crt,"1",ext_ptr->name);
                add_crt_rom(cp->crt, rom_ptr, 1);
                add_active(cp->crt);
                }
                cp = cp->next_tag;
        }

        if(is_rom_loaded(old_rom_num)) {
                cp = old_rom_ptr->first_mon;
                while(cp) {
                        if(!F_ISSET(cp->crt, MFOLLO) || F_ISSET(cp->crt, MDMFOL)) {
                                cp = cp->next_tag;
                                continue;
                        }
                        if(!cp->crt->first_enm) {
                                cp = cp->next_tag;
                                continue;
                        }
                        if(strcmp(cp->crt->first_enm->enemy, ply_ptr->name)) {
                                cp = cp->next_tag;
                                continue;
                        }
                        if(mrand(1,50) > 10 - ply_ptr->dexterity +
                           cp->crt->dexterity) {
                                cp = cp->next_tag;
                                continue;
                        }
                        print(fd, "%M%j ����� ����ɴϴ�.", cp->crt,"1");
                        broadcast_rom(fd, old_rom_num, "\n%M%j %M%j ���󰩴ϴ�.",
                                      cp->crt,"1", ply_ptr,"3");
                        temp = cp->next_tag;
                        crt_ptr = cp->crt;
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
/*                              openexit                              */
/**********************************************************************/

/* This function allows a player to open a door if it is not already */
/* open and if it is not locked.                                     */

int openexit(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        exit_   *ext_ptr;
        room    *rom_ptr;
        int     fd;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
                print(fd, "������ ���� ��������?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "�׷� �ⱸ�� �����ϴ�.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "�װ��� ����� �ֽ��ϴ�.");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "���� ������ �ֽ��ϴ�.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_CLR(ext_ptr, XCLOSD);
        ext_ptr->ltime.ltime = time(0);

        print(fd, "����� %s�� �ⱸ�� �������ϴ�.", ext_ptr->name);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %s�� �ⱸ�� �������ϴ�.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              closeexit                             */
/**********************************************************************/

/* This function allows a player to close a door, if the door is not */
/* already closed, and if indeed it is a door.                       */

int closeexit(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        exit_   *ext_ptr;
        room    *rom_ptr;
        int     fd;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;

        if(cmnd->num < 2) {
                print(fd, "������ �ݰ� ��������?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "�׷� �ⱸ�� �����ϴ�.");
                return(0);
        }

        if(F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "���� ������ �ֽ��ϴ�.");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XCLOSS)) {
                print(fd, "����� �� �ⱸ�� ���� �� �����ϴ�.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_SET(ext_ptr, XCLOSD);

        print(fd, "����� %s�� �ⱸ�� �ݽ��ϴ�.", ext_ptr->name);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %s�� �ⱸ�� �ݽ��ϴ�.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              unlock                                */
/**********************************************************************/

/* This function allows a player to unlock a door if he has the correct */
/* key, and it is locked.                                               */

int unlock(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        exit_   *ext_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "������ Ǯ�� ��������?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "������ Ǯ�� ��������?");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "�װ��� ����� ���� �ʽ��ϴ�.");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "�� ������ ��������?");
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
                print(fd, "����� �׷����� ���� ���� �ʽ��ϴ�.");
                return(0);
        }

        if(obj_ptr->type != KEY) {
                print(fd, "�װ��� ���谡 �ƴմϴ�.");
                return(0);
        }

        if(obj_ptr->shotscur < 1) {
                print(fd, "%I%j �ν��� ���Ƚ��ϴ�.", obj_ptr,"1");
                return(0);
        }

        if(obj_ptr->ndice != ext_ptr->key) {
                print(fd, "���谡 ���� �ʽ��ϴ�.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_CLR(ext_ptr, XLOCKD);
        ext_ptr->ltime.ltime = time(0);
        obj_ptr->shotscur--;

        if(obj_ptr->use_output[0])
                print(fd, "%s", obj_ptr->use_output);
        else
                print(fd, "## ��Ĭ ##");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %s�� �ⱸ�� Ǯ�����ϴ�.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              lock                                  */
/**********************************************************************/

/* This function allows a player to lock a door with the correct key. */

int lock(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        exit_   *ext_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "������ �ᱼ������?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "������ �ᱼ������?");
                return(0);
        }

        if(F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "�װ��� �̹� ����� �ֽ��ϴ�.");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "�� ������ �ᱸ������?");
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
                print(fd, "����� �׷����� ������ ���� �ʽ��ϴ�.");
                return(0);
        }

        if(obj_ptr->type != KEY) {
                print(fd, "%I%j ���谡 �ƴմϴ�.", obj_ptr,"0");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XLOCKS)) {
                print(fd, "����� �װ��� �ᱼ�� �����ϴ�.");
                return(0);
        }

        if(!F_ISSET(ext_ptr, XCLOSD)) {
                print(fd, "���� ���� �ݾƾ� �ɰ� ������.");
                return(0);
        }

        if(obj_ptr->shotscur < 1) {
                print(fd, "%I%j �μ��� ���Ƚ��ϴ�.", obj_ptr,"1");
                return(0);
        }

        if(obj_ptr->ndice != ext_ptr->key) {
                print(fd, "���谡 ���� �ʽ��ϴ�.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        F_SET(ext_ptr, XLOCKD);

        print(fd, "## ��Ĭ ##");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %s�� �ⱸ�� ��ɽ��ϴ�.",
                      ply_ptr, ext_ptr->name);

        return(0);

}

/**********************************************************************/
/*                              picklock                              */
/**********************************************************************/

/* This function is called when a thief or assassin attempts to pick a */
/* lock.  If the lock is pickable, there is a chance (depending on the */
/* player's level) that the lock will be picked.                       */

int picklock(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        exit_   *ext_ptr;
        long    i, t;
        int     fd, chance;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->class != THIEF && ply_ptr->class < INVINCIBLE) {
                print(fd, "���ϸ� �ڹ��踦 �� �� �ֽ��ϴ�.");
                return(0);
        }

        if(cmnd->num < 2) {
                print(fd, "������ ���÷�����?");
                return(0);
        }

        ext_ptr = find_ext(ply_ptr, rom_ptr->first_ext,
                           cmnd->str[1], cmnd->val[1]);

        if(!ext_ptr) {
                print(fd, "�׷��� ���� �����ϴ�.");
                return(0);
        }
        if(F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "����� ���� �־� �־� �� �� �����ϴ�.");
                return(0);
        }
        if(!F_ISSET(ext_ptr, XLOCKD)) {
                print(fd, "�װ��� ����� ���� �ʽ��ϴ�.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        i = LT(ply_ptr, LT_PICKL);
        t = time(0);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }

        ply_ptr->lasttime[LT_PICKL].ltime = t;
        ply_ptr->lasttime[LT_PICKL].interval = 10;

        chance = (ply_ptr->class == THIEF) ? 10*((ply_ptr->level+3)/4) :
                 5*((ply_ptr->level+3)/4);
        chance += bonus[ply_ptr->dexterity]*2;

        if(F_ISSET(ext_ptr, XUNPCK))
                chance = 0;

        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %s�� �ⱸ�� ������ �մϴ�.",
                      ply_ptr, ext_ptr->name);

        if(mrand(1,100) <= chance) {
                print(fd, "����� ���� ���µ� �����߽��ϴ�.");
                F_CLR(ext_ptr, XLOCKD);
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���� �����ϴ�.",
                              ply_ptr);
        }
        else
                print(fd, "�����Ͽ����ϴ�!");

        return(0);

}

/**********************************************************************/
/*                              steal                                 */
/**********************************************************************/

/* This function allows a player to steal an item from a monster within  */
/* the game.  Only thieves may steal, and the monster must               */
/* have the object which is being stolen within his inventory.           */

int steal(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        object          *obj_ptr;
        otag            *otag_ptr;
        long            i, t;
        int             fd, chance;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "������ ��ġ������?");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "�������׼� ��ġ������?");
                return(0);
        }

        if(ply_ptr->class != THIEF && ply_ptr->class < INVINCIBLE) {
                print(fd, "���ϸ� ��ĥ�� �ֽ��ϴ�.");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        i = LT(ply_ptr, LT_STEAL);
        t = time(0);

        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, "����� ����� ������ �巯���ϴ�.");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ������ �巯���ϴ�.",
                              ply_ptr);
        }

        ply_ptr->lasttime[LT_STEAL].ltime = t;
        ply_ptr->lasttime[LT_STEAL].interval = 5;

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(ply_ptr, PBLIND)) {
                        print(fd, "�׷��� ���� �����ϴ�.");
                        return(0);
                }

        }

        if(crt_ptr->type == MONSTER) {
                if(F_ISSET(crt_ptr, MUNKIL)) {
                        print(fd, "����� %s�� ��ĥ�� �����ϴ�.",
                                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                        return(0);
                }
                if(is_enm_crt(ply_ptr->name, crt_ptr)) {
                        print(fd, "%s�� �ο�� ���� �ƴմϴ�.",
                              F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                        return(0);
                }
        }
        else {
                if(F_ISSET(rom_ptr, RNOKIL)) {
                        print(fd, "�� �濡���� ��ĥ �� �����ϴ�.");
                        return(0);
                }

                if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->class<SUB_DM) {
                    print(fd, "����� ���ؼ� ��ĥ �� �����ϴ�.");
                    return (0);
                }

                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->class<SUB_DM) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.");
                    return (0);
                }
        }
        if(F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "����� ���� �־� ��ĥ �� �����ϴ�.");
                return(0);
        }
        obj_ptr = find_obj(ply_ptr, crt_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
                print(fd, "%s�� �׷� ������ ���� ���� �ʽ��ϴ�.",
                      F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }

        chance = (ply_ptr->class == THIEF) ? 4*((ply_ptr->level+3)/4) :
                 3*((ply_ptr->level+3)/4);
        chance += bonus[ply_ptr->dexterity]*3;
        if(crt_ptr->level > ply_ptr->level)
                chance -= 15*(((crt_ptr->level+3)/4) - ((ply_ptr->level+3)/4));
        if(obj_ptr->questnum) chance = 0;
        chance = MIN(chance, 65);

        if(obj_ptr->questnum || F_ISSET(crt_ptr, MUNSTL))
                chance = 0;

        if(ply_ptr->class == DM)
                chance = 100;
               
        if(crt_ptr->class == DM)  chance =0;
        if(F_ISSET(obj_ptr, ONEWEV)) chance =0;
        if(obj_ptr->questnum && ply_ptr->class < DM) chance=0;
        if(obj_ptr->first_obj) {
            for(otag_ptr=obj_ptr->first_obj; otag_ptr; otag_ptr=otag_ptr->next_tag) {
                if(otag_ptr->obj->questnum && ply_ptr->class < DM) chance=0;
                if(F_ISSET(otag_ptr->obj, ONEWEV) && ply_ptr->class < DM) chance=0;
            }
        }



        if(mrand(1,100) <= chance) {
                print(fd, "���ƽ��ϴ�.");
                del_obj_crt(obj_ptr, crt_ptr);
                add_obj_crt(obj_ptr, ply_ptr);
                savegame_nomsg(ply_ptr);
                if(crt_ptr->type == PLAYER) savegame_nomsg(crt_ptr);
                if(crt_ptr->type == PLAYER) {
                        ply_ptr->lasttime[LT_PLYKL].ltime = t;
                        ply_ptr->lasttime[LT_PLYKL].interval =
                                (long)mrand(7,10) * 86400L;

                }
        }

        else {
                print(fd, "�����Ͽ����ϴ�.%s", crt_ptr->type == MONSTER ?
                      "\n�װ� ����� �����մϴ�." : "");
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                              "\n%M�� %M���Լ� ������ ��ġ���� �մϴ�.",
                              ply_ptr, crt_ptr);

                if(crt_ptr->type == PLAYER)
                        print(crt_ptr->fd, "\n%M�� ��ſ��Լ� %1i%j ��ġ���� �մϴ�.",
                              ply_ptr, obj_ptr,"3");
                else
                        add_enm_crt(ply_ptr->name, crt_ptr);
        }

        return(0);

}
