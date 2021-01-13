/*
 * MAGIC4.C:
 *
 *      Additional spell-casting routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              detectinvis(���а�����)                         */
/**********************************************************************/

/* This function allows players to cast the detect-invisible spell which */
/* allows the spell-castee to see invisible items.                   */

int detectinvis(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            t;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        t = time(0);

        if(ply_ptr->mpcur < 10 && how == CAST) {
                print(fd, 
                        "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SDINVI) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

        /* Cast detect-invisibility on self */
        if(cmnd->num == 2) {

                ply_ptr->lasttime[LT_DINVI].ltime = t;
                if(how == CAST) {
                        ply_ptr->lasttime[LT_DINVI].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == MAGE)
                                ply_ptr->lasttime[LT_DINVI].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,
                        "\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            ply_ptr->lasttime[LT_DINVI].interval += 600L;
        }                                
                }
                else
                        ply_ptr->lasttime[LT_DINVI].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,"\n����� �������� �δ��� ���� ���а������� �ֹ���\n�ܿ�ϴ�.\n�δ��� Ǫ�������� �������� �����ִ� �ڵ��� ����\n�ְԵǾ����ϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                 "\n%M�� �������� �δ��� ���� �ڽſ��� ���а�������\n�ֹ��� �ܿ����ϴ�.\n ���� ������ Ǫ�������� �������ϴ�.\n",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n.���ڱ� �δ��� Ǫ�������� �������� �����ִ� �ڵ��� ���� �ְ� �Ǿ����ϴ�.\n");

                F_SET(ply_ptr, PDINVI);

                return(1);
        }

        /* Cast detect-invisibility on another player */
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                crt_ptr->lasttime[LT_DINVI].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_DINVI].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == MAGE)
                                crt_ptr->lasttime[LT_DINVI].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            crt_ptr->lasttime[LT_DINVI].interval += 600L;
        }                                
                }
                else
                        crt_ptr->lasttime[LT_DINVI].interval = 1200;

                F_SET(crt_ptr, PDINVI);

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n����� %M�� �δ����� ������ ���а������� �ܿ�ϴ�.\n���� ������ Ǫ�������� �������ϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M�� ����� �δ����� ������ ���а������� �ܿ����ϴ�.\n���ڱ� �δ��� Ǫ�������� �������� �����ִ� �ڵ��� ����\n�ְ� �Ǿ����ϴ�.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� %M�� �δ����� ������ ���а������� �ܿ�ϴ�.\n���� ������ Ǫ�������� �������ϴ�.\n",
                                       ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              detectmagic(�ֹ�������)                */
/**********************************************************************/

/* This function allows players to cast the detect-magic spell which */
/* allows the spell-castee to see magic items.                         */

int detectmagic(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            t;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        t = time(0);

        if(ply_ptr->mpcur < 10 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SDMAGI) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

        /* Cast detect-magic on self */
        if(cmnd->num == 2) {

                ply_ptr->lasttime[LT_DMAGI].ltime = t;
                if(how == CAST) {
                        ply_ptr->lasttime[LT_DMAGI].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == MAGE)
                                ply_ptr->lasttime[LT_DMAGI].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            ply_ptr->lasttime[LT_DMAGI].interval += 600L;
        }                                
                }
                else
                        ply_ptr->lasttime[LT_DMAGI].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,"\n����� ��ȭ���� ���� ���� �ֹ��������� �ܿ�ϴ�.\n����� ������ ���������� �������� �ּ��� ���� �ȸ���\n�о������ϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M�� ��ȭ���� ���� ���� �ֹ��������� �ܿ����ϴ�.\n���ڱ� ���� ������ ���������� �������ϴ�.\n",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n���ڱ� �δ��� ���������� �������� �ּ��� ���� �ȸ��� \n�о������ϴ�.\n");

                F_SET(ply_ptr, PDMAGI);

                return(1);
        }

        /* Cast detect-magic on another player */
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                crt_ptr->lasttime[LT_DMAGI].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_DMAGI].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == MAGE)
                                crt_ptr->lasttime[LT_DMAGI].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
            crt_ptr->lasttime[LT_DMAGI].interval += 600L;
        }                                
                }
                else
                        crt_ptr->lasttime[LT_DMAGI].interval = 1200;

                F_SET(crt_ptr, PDMAGI);

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n����� %M�� ��ȸ���� ������ �ֹ��������� \n�ֹ��� �ܿ�ϴ�.\n���ڱ� ���� �δ��� ���������� �������ϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M�� ����� ��ȸ���� ������ �ֹ��������� \n�ֹ��� �ܿ�ϴ�.\n���ڱ� �δ��� ���������� �������� �ּ��� ���� �ȸ��� �о������ϴ�.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� %M�� ��ȸ���� ������ �ֹ��������� \n�ֹ��� �ܿ�ϴ�.\n���ڱ� ���� �δ��� ���������� �������ϴ�.\n.",
                                       ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              teleport(�����̵���)                            */
/**********************************************************************/

/* This function allows a player to teleport himself or another player */
/* to another room randomly.                                           */

int teleport(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
	FILE *fp;
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd, rtn, rom_num;
	char file[80];
	char tel_str[20][15];
	int i, tnum, tel_num[20], tel_lev[20];

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 20 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, STELEP) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                        ply_ptr->mpcur -= 20;
                return(0);
        }

        /* Cast teleport on self */
        if(cmnd->num == 2) {

                if(how == CAST)
                        ply_ptr->mpcur -= 20;

                broadcast_rom(fd, ply_ptr->rom_num, 
                              "\n%M�� �����̵����� ����Ͽ� ���а��� ��������ϴ�.\n", ply_ptr);
                if(how == CAST || how == SCROLL)
                        print(fd, "\n����� �����̵����� ����ϱ����� �߱������� �����մϴ�.\n�����̵����� �ֹ��� �ܿ��� ���� �Ѿ˰��� �������鼭\n���а��� ���󰬽��ϴ�.\n");
                else
                        print(fd, "\n�����̵����� �ֹ��� �ܿ��� ���� �Ѿ˰��� �������鼭\n���а��� ���󰬽��ϴ�.\n");

                do {
                        rom_num = mrand(1, RMAX-1);
                        rtn = load_rom(rom_num, &new_rom);
                        if(rtn > -1 && F_ISSET(new_rom, RNOTEL))
                                rtn = -1;
                        else if(rtn > -1 && new_rom->lolevel > ply_ptr->level)
                                rtn = -1;
                        else if(rtn > -1 && ply_ptr->level > new_rom->hilevel &&
                                new_rom->hilevel)
                                rtn = -1;
                } while(rtn < 0);

                del_ply_rom(ply_ptr, rom_ptr);
                add_ply_rom(ply_ptr, new_rom);

                return(1);
        }

        /* Cast teleport on another player */
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ڽſ��Ը� ��밡���մϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }
                if(F_ISSET(crt_ptr, PRMAGI) && ply_ptr->level < 128 && ((((ply_ptr->level+3)/4)-((crt_ptr->level+3)/4))*10) < 50) {
                        print(fd, "\n%M�� �����̵� ��Ű�⿣ ����� �ֹ��� �ʹ� ���մϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, "\n%M�� �����̵����� ����Ͽ� ����� �̵� ��Ű�� �մϴ�.\n", ply_ptr);
                                if(how == CAST)
                                ply_ptr->mpcur -= 20;
                        return(0);
                }

                if(how == CAST)
                        ply_ptr->mpcur -=20;
 
                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n�����̵��� �ֹ��� %M���� �ܿ�ϴ�.\n���� ���� �Ȱ��� �۽��̸� ����� ��������ϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M�� ��ſ��� �����̵��� �ֹ��� �ܿ����ϴ�.\n����� ���� �Ȱ��� �۽��̸� ���а��� �̵��˴ϴ�.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� %M���� �����̵��� �ֹ��� �ܿ����ϴ�.\n���� ���� �Ȱ��� �۽��̸� ����� ��������ϴ�.\n",
                                       ply_ptr, crt_ptr);

                        do {
                                rom_num = mrand(1, RMAX-1);
                                rtn = load_rom(rom_num, &new_rom);
                                if(rtn > -1 && F_ISSET(new_rom, RNOTEL))
                                        rtn = -1;
                                else if(rtn > -1 && 
                                        new_rom->lolevel > ply_ptr->level)
                                        rtn = -1;
                                else if(rtn > -1 && 
                                        ply_ptr->level > new_rom->hilevel && 
                                        new_rom->hilevel)
                                        rtn = -1;
                        } while(rtn < 0);

                        del_ply_rom(crt_ptr, rom_ptr);
                        add_ply_rom(crt_ptr, new_rom);

                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              enchant(�ּ�)                                  */
/**********************************************************************/

/* This function allows mages to enchant weapons at a cost of 25 magic */
/* points.  They can only do it 3 times a day.                          */

int enchant(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        object           *obj_ptr;
        room           *rom_ptr;
        int              fd, adj;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(how == CAST && ply_ptr->class != MAGE && 
           ply_ptr->class < INVINCIBLE) {
                print(fd, "\n������鸸�� �ּ��� �ɼ��ֽ��ϴ�.\n");
                return(0);
        }

        if(ply_ptr->mpcur < 25 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SENCHA) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ּ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "\n�������� �ּ��� �̴ϴ�?\n");
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
                print(fd, "\n��� ����ǰ�� �׷� ������ �����ϴ�.\n");
                return(0);
        }

        if(F_ISSET(obj_ptr, OENCHA)) {
                print(fd, "\n���� �ּ��� �ɷ��ֽ��ϴ�.\n");
                return(1);
        }

        if(how == CAST) {
                if(!dec_daily(&ply_ptr->daily[DL_ENCHA]) && 
                   ply_ptr->class < CARETAKER) {
                        print(fd, "\n����� Ż���ؼ� �� �̻� �ּ��� �ɼ� �����ϴ�.\n�� ����߰ڴµ���?\n");
                        return(0);
                }
                ply_ptr->mpcur -= 25;
        }

        if((ply_ptr->class == MAGE || ply_ptr->class >= INVINCIBLE) &&
           how == CAST) {
                adj = (((ply_ptr->level+3)/4)-5)/5 + 1;
                adj = MIN(3, adj);
        }
        else
                adj = 1;

        obj_ptr->adjustment = MAX(adj, obj_ptr->adjustment);

        if(obj_ptr->type == ARMOR && obj_ptr->wearflag == BODY)
                obj_ptr->armor += adj*2;
        else if(obj_ptr->type == ARMOR)
                obj_ptr->armor += adj;
        else if(obj_ptr->type <= MISSILE) {
                obj_ptr->shotsmax += adj*10;
                obj_ptr->shotscur += adj*10;
                obj_ptr->pdice += adj;
        }
        obj_ptr->value += 500*adj;

        F_SET(obj_ptr, OENCHA);

        print(fd, "\n%i���ٰ� �ּ��� ���� ���ڱ� ���� ���� ���ٰ� ��������ϴ�.\n", obj_ptr);
        broadcast_rom(fd, rom_ptr->rom_num, "%M�� %1i���ٰ� �ּ��� �ɾ����ϴ�.", ply_ptr,
                      obj_ptr);

        return(1);
}
