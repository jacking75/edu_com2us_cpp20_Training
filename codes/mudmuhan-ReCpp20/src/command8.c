/*
 * COMMAND8.C:
 *
 *      Additional user routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <sys/time.h>

/**********************************************************************/
/*                              give                                  */
/**********************************************************************/

/* This function allows a player to give an item in his inventory to */
/* another player or monster.                                        */

int give(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        object          *obj_ptr;
        otag            *otag_ptr;
        creature        *crt_ptr;
        room            *rom_ptr;
        int             fd;
        int             len,cnt;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 3) {
                print(fd, "�������� �ֽ÷�����?\n");
                return(0);
        }

        len=strlen(cmnd->str[1]);
        if(len>2 && !strcmp(&cmnd->str[1][len-2],"��")) {
                give_money(ply_ptr, cmnd);
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
                print(fd, "����� �׷����� ���� ���� �ʽ��ϴ�.\n");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                           cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
                cmnd->str[2][0] = low(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                                   cmnd->str[2], cmnd->val[2]);
                if(!crt_ptr) {
                        print(fd, "�׷� ����� ���� �����!\n");
                        return(0);
                }
        }

        if(crt_ptr == ply_ptr) {
                print(fd, "�ڽſ��Դ� ������ �� �� �����ϴ�.\n");
                return(0);
        }
		if(crt_ptr->type != PLAYER) {
				print(fd, "�׷� ����� ���� �����!\n");
				return(0);
		}

        if(obj_ptr->questnum && ply_ptr->class < DM) {
                print(fd, "�ӹ��� ���õǴ� ������ Ÿ�ο��� �� �� �����ϴ�.\n");
                return(0);
        }
        if(F_ISSET(obj_ptr, ONEWEV) && ply_ptr->class < DM) {
			if(strcmp(obj_ptr->key[2], "�̺�Ʈ")) {
        	print(fd, "�̺�Ʈ �������� Ÿ�ο��� �� �� �����ϴ�.\n");
        	return(0);
			}
        }

        if(obj_ptr->first_obj) {
            for(otag_ptr=obj_ptr->first_obj; otag_ptr; otag_ptr=otag_ptr->next_tag) {
                if(otag_ptr->obj->questnum && ply_ptr->class < DM) {
                    print(fd, "�ӹ��� ���õǴ� ������ ��������� Ÿ�ο��� �� �� �����ϴ�.\n");
                    return(0);
                }
                if(F_ISSET(otag_ptr->obj, ONEWEV) && ply_ptr->class < DM) {
                  	print(fd, "�̺�Ʈ �������� ��������� Ÿ�ο��� �� �� �����ϴ�.\n");
        	        return(0);
                }
            }
        }

        if(crt_ptr->type == PLAYER)
        	cnt = count_inv(crt_ptr, -1);

        if(crt_ptr->type == PLAYER && (weight_ply(crt_ptr) +
           weight_obj(obj_ptr) > max_weight(crt_ptr)) || cnt>150) {
                print(fd, "%S%j ���̻� ���� �� �����ϴ�.\n", crt_ptr->name,"0");
                return(0);
        }

        del_obj_crt(obj_ptr, ply_ptr);
        add_obj_crt(obj_ptr, crt_ptr);

#ifdef NODUPLE
        savegame_nomsg(ply_ptr);
        if(crt_ptr->type==PLAYER) savegame_nomsg(crt_ptr);
#endif

        print(fd, "����� %M���� %1i%j �ݴϴ�.\n", crt_ptr, obj_ptr,"3");
        print(crt_ptr->fd, "\n%M%j ��ſ��� %1i%j �ݴϴ�.\n", ply_ptr, "1",obj_ptr,"3");
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, "\n%M�� %M���� %1i%j �ݴϴ�.",
                       ply_ptr, crt_ptr, obj_ptr,"3");

        return(0);

}

/**********************************************************************/
/*                              give_money                            */
/**********************************************************************/

/* This function allows a player to give gold to another player.  Gold */
/* is interpreted as gold if it is preceded by a dollar sign.          */

void give_money(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        long            amt;
        int             fd;

        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;

        amt = atol(cmnd->str[1]);
        if(amt < 1) {
                print(fd, "���� ������ ������ �ɼ� �����ϴ�.\n");
                return;
        }
        if(amt > ply_ptr->gold) {
                print(fd, "����� �׸�ŭ�� ���� ������ ���� �ʽ��ϴ�.\n");
                return;
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                           cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr || crt_ptr == ply_ptr) {
                cmnd->str[2][0] = low(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                          cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "�׷� ����� ���� �����!\n");
                        return;
                }
        }

        crt_ptr->gold += amt;
        ply_ptr->gold -= amt;

#ifdef NODUPLE
        savegame_nomsg(ply_ptr);
        if(crt_ptr->type==PLAYER) savegame_nomsg(crt_ptr);
#endif
        print(fd, "����� %M���� %ld���� �־����ϴ�.\n",crt_ptr,amt);

        print(crt_ptr->fd, "\n%M%j ��ſ��� %ld���� �־����ϴ�.\n",
              ply_ptr, "1", amt);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M%j %M���� %ld���� �־����ϴ�.", ply_ptr,"1", crt_ptr, amt);

}

/**********************************************************************/
/*                              repair                                */
/**********************************************************************/

/* This function allows a player to repair a weapon or armor if he is */
/* in a repair shoppe.  There is a chance of breakage.                */

int repair(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        object  *obj_ptr;
        room    *rom_ptr;
        long    cost;
        int     fd, broke;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "������ �����Ͻ÷�����?\n");
                return(0);
        }

        if(!F_ISSET(rom_ptr, RREPAI)) {
                print(fd, "���⼭�� ������ �� �����ϴ�.\n");
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);

        if(!obj_ptr) {
                print(fd, "����� �׷� ������ ���� ���� �ʽ��ϴ�.\n");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        if(F_ISSET(obj_ptr, ONOFIX)) {
                print(fd, "�װ��� �����Ҽ� ���� �����Դϴ�.\n");
                return(0);
        }

        if(obj_ptr->type > MISSILE && obj_ptr->type != ARMOR) {
                print(fd, "������ ������ \"���⳪ ��ȣ���� ������ �� �ֳ�.\"��� ���մϴ�.\n");
                return(0);
        }

        if(obj_ptr->shotscur > MAX(3, obj_ptr->shotsmax/10)) {
                print(fd, "������ ������ \"�װ� ���� �����ѵ�...\"��� ���մϴ�.\n");
                return(0);
        }

        cost = obj_ptr->value / 4;

        if(ply_ptr->gold < cost) {
                print(fd, "������ ������ \"��¥�δ� �������� �� ����. ���� �� ������Գ�..\"��� ���մϴ�.\n");
                return(0);
        }

        print(fd, "����� ������ ���ο��� ������ %ld���� �ǳ��־����ϴ�.\n", cost);
        ply_ptr->gold -= cost;

        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ������ ���� ������ ���ο��� %1i�� �ǳ��־����ϴ�.",
                      ply_ptr, obj_ptr);

        broke = mrand(1,100) + bonus[ply_ptr->piety];
        if((broke <= 15 && obj_ptr->shotscur < 1) ||
           (broke <= 5 && obj_ptr->shotscur > 0)) {
                print(fd, "������ ������ \"�̷�~~! ������ �ϴ� �η��߷ȳ�. �̾��ϳ�\"��� ���մϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "�̷� ������ �Ǽ��� �߽��ϴ�.");
      /*        if(obj_ptr->shotscur > 0) {     */
                        print(fd, "������������ ��ſ��� ���� �����־����ϴ�.\n");
                        ply_ptr->gold += cost;
     /*         }       �� ������ */
                del_obj_crt(obj_ptr, ply_ptr);
                free_obj(obj_ptr);
                return(0);
        }

        if((F_ISSET(obj_ptr, OENCHA) || obj_ptr->adjustment) &&
                mrand(1,50) > ply_ptr->piety) {
                print(fd, "������ ������ \"������ �ٵǾ���.\"��� ���մϴ�.\n");
                if(obj_ptr->type == ARMOR && obj_ptr->wearflag == BODY)
                        obj_ptr->armor =
                                MAX(obj_ptr->armor - obj_ptr->adjustment*2, 0);
                else if(obj_ptr->type == ARMOR)
                        obj_ptr->armor =
                                MAX(obj_ptr->armor - obj_ptr->adjustment, 0);
                else if(obj_ptr->type <= MISSILE) {
                        obj_ptr->shotsmax -= obj_ptr->adjustment*10;
                        obj_ptr->pdice =
                                MAX(obj_ptr->pdice - obj_ptr->adjustment, 0);
                }
                obj_ptr->adjustment = 0;
                F_CLR(obj_ptr, OENCHA);
        }

        obj_ptr->shotscur = (obj_ptr->shotsmax * mrand(5,9)) / 10;

        print(fd, "������ ������ ��ſ��� %i%j �ǵ��� �ݴϴ�.\n�װ��� ���� ����ó�� ���Դϴ�.\n",
                obj_ptr,"3");

        return(0);

}

/**********************************************************************/
/*                              prt_time                              */
/**********************************************************************/

/* This function outputs the time of day (realtime) to the player.    */

int prt_time(ply_ptr)
creature        *ply_ptr;
{
        char    *str;
        long    t;
        int     fd, daytime;

        fd = ply_ptr->fd;

        daytime = (int)(Time % 24L);
        print(fd, "���� �ð�: %s %d��.\n",
                daytime > 11 ? "����":"����",
                daytime%12 == 0 ? 12 : daytime%12);

        t = time(0);
        str = ctime(&t);
        str[strlen(str)-1] = 0;

        print(fd, "���� �ð�: %s (PST).\n", str);

        return(0);

}

/**********************************************************************/
/*                              circle                                */
/**********************************************************************/

/* This function allows fighters and barbarians to run circles about an */
/* enemy, confusing it for several seconds.                             */

int circle(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        long            i, t;
        int             chance, delay, fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "������ ������Ű������?\n");
                return(0);
        }
/*	if(F_ISSET(ply_ptr, SLEEP)) {
		print(fd, "\n�޽��� �ߴ��մϴ�.\n");
		F_CLR(ply_ptr, SLEEP);
	}  */

        if(ply_ptr->class != FIGHTER && ply_ptr->class != BARBARIAN &&
           ply_ptr->class < INVINCIBLE) {
                print(fd, "�ǹ����� �˻縸 ���� �ִ� ����Դϴ�.\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                cmnd->str[1][0] = up(cmnd->str[1][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[1], cmnd->val[1]);

                if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 2) {
                        print(fd, "�׷����� ���� �����ϴ�.\n");
                        return(0);
                }
        }

        if(crt_ptr->type == PLAYER) {
                if(F_ISSET(rom_ptr, RNOKIL)) {
                        print(fd, "�� �濡���� �ο� �� �����ϴ�.\n");
                        return(0);
                }

		if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����ڸ� ������ �� �����ϴ�.\n");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.\n");
                    return (0);
                }
		}
		else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����ڸ� ������ �� �����ϴ�.\n");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.\n");
                    return (0);
                }
		}
        if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(crt_ptr, PCHARM)) {
                print(fd, "����� %s�� �ʹ� �����ؼ� �׷��� �� �� �����ϴ�.\n", crt_ptr->name);
                return(0);
            }

        }

        i = LT(ply_ptr, LT_ATTCK);
        t = time(0);

        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);
        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, "����� ����� ������ �巯���ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ������ �巯���ϴ�.",
                              ply_ptr);
        }

        chance = 50 + (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*10 +
                 (bonus[ply_ptr->dexterity] -bonus[crt_ptr->dexterity])*2;
        if (crt_ptr->type == MONSTER && F_ISSET(crt_ptr, MUNDED))
                chance -= (5 + ((crt_ptr->level+3)/4)*2);
        chance = MIN(80, chance);
        if(F_ISSET(crt_ptr, MNOCIR) || F_ISSET(ply_ptr, PBLIND))
                chance=1;

        if(crt_ptr->type != PLAYER) {
                if(F_ISSET(crt_ptr, MUNKIL)) {
                        print(fd, "����� %s�� ��ĥ�� �����ϴ�.\n",
                                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                        return(0);
                }
                add_enm_crt(ply_ptr->name, crt_ptr);
        }

        if(mrand(1,100) <= chance) {
                if(ply_ptr->class == BARBARIAN)
                        delay = mrand(6,9);
                else
                        delay = mrand(6,12);
                crt_ptr->lasttime[LT_BEFUD].ltime = t;
                crt_ptr->lasttime[LT_BEFUD].interval = delay;
                if(crt_ptr->type == MONSTER)
                      F_SET(crt_ptr, MBEFUD);

                print(fd, "����� �̸����� �Դٰ��� �ϸ鼭 %M%j ������ŵ�ϴ�.\n", crt_ptr,"3");

                print(crt_ptr->fd, "%M�� ��������� �������� ���ƴٴմϴ�.\n", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                               "\n%M�� %M ������ ��۹�� ���ϴ�.", ply_ptr, crt_ptr);
                    
                ply_ptr->lasttime[LT_ATTCK].interval = 2;
        }
        else {
                print(fd, "����� ���� ������Ű�µ� �����Ͽ����ϴ�.\n");
                print(crt_ptr->fd, "\n%M�� ����� ������Ű���� �մϴ�.\n", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                               "\n%M�� %M%j ������Ű���� �մϴ�.", ply_ptr, crt_ptr,"3");
                ply_ptr->lasttime[LT_ATTCK].interval = 3;
        }

        ply_ptr->lasttime[LT_ATTCK].ltime = t;

        return(0);

}

/**********************************************************************/
/*                              bash                                  */
/**********************************************************************/

/* This function allows fighters and barbarians to "bash" an opponent, */
/* doing less damage than a normal attack, but knocking the opponent   */
/* over for a few seconds, leaving him unable to attack back.          */

int bash(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        long            i, t;
        int             m, n, chance, fd, p, addprof;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "���� �����ϱ�?\n");
                return(0);
        }
/*	if(F_ISSET(ply_ptr, SLEEP)){
		print(fd, "\n�޽��� �ߴ��մϴ�.\n");
		F_CLR(ply_ptr, SLEEP);
	} */

        if(ply_ptr->class != FIGHTER && ply_ptr->class != BARBARIAN &&
           ply_ptr->class < INVINCIBLE) {
                print(fd, "�ǹ����� �˻縸 ���� �ִ� ����Դϴ�.\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                cmnd->str[1][0] = up(cmnd->str[1][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[1], cmnd->val[1]);

                if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1])<2) {
                        print(fd, "�׷� ���� ���� �����ϴ�.\n");
                        return(0);
                }

        }

        if(crt_ptr->type != PLAYER && is_enm_crt(ply_ptr->name, crt_ptr)) {
                print(fd, "����� %s�� �ο�� ���Դϴ�.\n",
                      F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }
        else if(crt_ptr->type == PLAYER) {
                if(F_ISSET(rom_ptr, RNOKIL)) {
                        print(fd, "�� �濡���� �ο� �� �����ϴ�.\n");
                        return(0);
                }

		if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����ڸ� ������ �� �����ϴ�.\n");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.\n");
                    return (0);
                }
		}
		else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����ڸ� ������ �� �����ϴ�.\n");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV) ) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.\n");
                    return (0);
                }
		}
        if(is_charm_crt(ply_ptr->name, crt_ptr)&& F_ISSET(crt_ptr, PCHARM)) {
                print(fd, "����� %S%j �ʹ� ������ �׷��� �� �� �����ϴ�.\n", crt_ptr->name,"3");
                return(0);
            }

        }

        i = LT(ply_ptr, LT_ATTCK);
        t = time(0);

        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }

        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].interval = 1;

        F_CLR(ply_ptr, PHIDDN);
        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, "����� ����� ������ �巯���ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ������ �巯���ϴ�.",
                              ply_ptr);
        }

        if(crt_ptr->type != PLAYER) {
                if(F_ISSET(crt_ptr, MUNKIL)) {
                        print(fd, "����� %s�� ��ĥ �� �����ϴ�.\n",
                                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                        return(0);
                }
 if(F_ISSET(crt_ptr, MMGONL)) {
            print(fd, "����� ���Ⱑ %M���� �ƹ��ҿ��� ���µ� �մϴ�.\n",
                crt_ptr);
            return(0);
        }
        if(F_ISSET(crt_ptr, MENONL)) {
            if(!ply_ptr->ready[WIELD-1] ||
                ply_ptr->ready[WIELD-1]->adjustment < 1) {
                print(fd, "����� ���Ⱑ %M���� �ƹ� �ҿ��� ���µ� �մϴ�.\n",
                    crt_ptr);
                return(0);
            }
        }
                add_enm_crt(ply_ptr->name, crt_ptr);
        }

        chance = 50 + (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*10 +
                bonus[ply_ptr->strength]*3 +
                 (bonus[ply_ptr->dexterity] -bonus[crt_ptr->dexterity])*2;
        chance += ply_ptr->class==BARBARIAN ? 5:0;
        chance = MIN(85, chance);
        if(F_ISSET(ply_ptr, PBLIND))
                chance = MIN(20, chance);

        if(mrand(1,100) <= chance) {

                if(ply_ptr->ready[WIELD-1]) {
                        if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
                                print(fd, "\n%S%j �μ��� ���Ƚ��ϴ�.\n",
                                      ply_ptr->ready[WIELD-1]->name,"1");
                                add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
                                ply_ptr->ready[WIELD-1] = 0;
                                return(0);
                        }
                }

                n = ply_ptr->thaco - crt_ptr->armor/8;
                if(mrand(1,20) >= n) {

                        crt_ptr->lasttime[LT_BEFUD].ltime = t;
                        crt_ptr->lasttime[LT_BEFUD].interval = mrand(5,8);
                        if (crt_ptr->type == MONSTER)
                               F_SET(crt_ptr, MBEFUD);

                        if(ply_ptr->ready[WIELD-1])
                                n = mdice(ply_ptr->ready[WIELD-1]) * 1.5;
                        else
                                n = mdice(ply_ptr) * 3;

                        if(ply_ptr->class == BARBARIAN && mrand(1,100) > 50)
                                n = n*1.5;
                        if(ply_ptr->class == FIGHTER && mrand(1,100) > 50)
                                n = n*3;

                        m = MIN(crt_ptr->hpcur, n);
                        if(crt_ptr->type != PLAYER) {
                                add_enm_dmg(ply_ptr->name, crt_ptr, m);
                                if(ply_ptr->ready[WIELD-1]) {
                                        p = MIN(ply_ptr->ready[WIELD-1]->type,
                                                4);
                                        addprof = (m * crt_ptr->experience) /
                                                crt_ptr->hpmax;
                                        addprof = MIN(addprof,
                                                crt_ptr->experience);
                                        ply_ptr->proficiency[p] += addprof;
                                }
                        }
                        if(ply_ptr->class == FIGHTER)
                        	n = MAX(crt_ptr->hpcur /4, n);
						if(crt_ptr->class > CARETAKER) n =1;

                        crt_ptr->hpcur -= n;

                        print(fd, "����� %d���� �Ͱ��� ���߽��ϴ�.\n", n);
                        broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num,
                                       "\n%M�� %m���� �Ͱ��� ���մϴ�.", ply_ptr, crt_ptr);
                        print(crt_ptr->fd, "%M�� %d���� �Ͱ��� ���߽��ϴ�.\n",
                              ply_ptr, n);

                        ply_ptr->lasttime[LT_ATTCK].interval = 1;

                        if(crt_ptr->hpcur < 1) {
                                print(fd, "����� %M%j �׿����ϴ�.\n", crt_ptr,"3");
                                broadcast_rom2(fd, crt_ptr->fd,
                                               ply_ptr->rom_num,
                                               "\n%M%j %M%j �׿����ϴ�.", ply_ptr,"1",
                                               crt_ptr,"3");

                                die(crt_ptr, ply_ptr);
                        }
                        else
                                check_for_flee(ply_ptr, crt_ptr);
                }
                else {
                        print(fd, "����� �Ͱ��� �����߽��ϴ�.\n");
                        print(crt_ptr->fd, "\n%M�� ��ſ��� �Ͱ��� ���Ϸ� �մϴ�.\n", ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num,
                                       "\n%M�� %M���� �Ͱ��� ���Ϸ� �մϴ�.", ply_ptr,
                                       crt_ptr);
                }
        }

        else {
                print(fd, "����� �Ͱ��� �����߽��ϴ�.\n");
                print(crt_ptr->fd, "%M�� ��ſ��� �Ͱ��� ���Ϸ� �մϴ�.\n", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num,
                               "\n%M�� %M���� �Ͱ��� ���Ϸ� �մϴ�.", ply_ptr, crt_ptr);
        }

        return(0);

}

/**********************************************************************/
/*                              savegame                              */
/**********************************************************************/

/* This function saves a player's game.  Since items need to be un-readied */
/* before a player can be saved to a file, this function makes a duplicate */
/* of the player, unreadies everything on the duplicate, and then saves    */
/* the duplicate to the file.  Afterwards, the duplicate is freed from     */
/* memory.                                                                 */

int savegame(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *dum_ptr;
        object          *obj[MAXWEAR];
        int             i, n = 0;

        dum_ptr = (creature *)malloc(sizeof(creature));
        if(!dum_ptr)
                merror("savegame", FATAL);

        *dum_ptr = *ply_ptr;

        for(i=0; i<MAXWEAR; i++) {
                if(dum_ptr->ready[i]) {
                        obj[n++] = dum_ptr->ready[i];
                        add_obj_crt(dum_ptr->ready[i], dum_ptr);
                        dum_ptr->ready[i] = 0;
                }
        }

        if(!dum_ptr->name[0]) return;

        if(save_ply(dum_ptr->name, dum_ptr) < 0)
                merror("savegame", NONFATAL);

        for(i=0; i<n; i++)
                del_obj_crt(obj[i], dum_ptr);

        free(dum_ptr);

        print(ply_ptr->fd, "�����Ͽ����ϴ�.\n");

        return(0);

}


int savegame_nomsg(ply_ptr)
creature        *ply_ptr;
{
        creature        *dum_ptr;
        object          *obj[MAXWEAR];
        int             i, n = 0;

        dum_ptr = (creature *)malloc(sizeof(creature));
        if(!dum_ptr)
                merror("savegame", FATAL);

        *dum_ptr = *ply_ptr;

        for(i=0; i<MAXWEAR; i++) {
                if(dum_ptr->ready[i]) {
                        obj[n++] = dum_ptr->ready[i];
                        add_obj_crt(dum_ptr->ready[i], dum_ptr);
                        dum_ptr->ready[i] = 0;
                }
        }

        if(!dum_ptr->name[0]) return;

        if(save_ply(dum_ptr->name, dum_ptr) < 0)
                merror("savegame", NONFATAL);

        for(i=0; i<n; i++)
                del_obj_crt(obj[i], dum_ptr);

        free(dum_ptr);

        return(0);

}


/**********************************************************************/
/*                              talk                                  */
/**********************************************************************/

/* This function allows players to speak with monsters if the monster */
/* has a talk string set.                                             */

int talk(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room            *rom_ptr;
        creature        *crt_ptr;
        ttag            *tp;
        char            str[160];
        int             fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, "�������� �̾߱��Ͻ÷�����?\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, "�׷� ���� ���� �����ϴ�.\n");
                return(0);
        }

        F_CLR(ply_ptr, PHIDDN);

        if(cmnd->num == 2 || !F_ISSET(crt_ptr, MTALKS)) {
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %M%j �̾߱⸦ �մϴ�.",
                        ply_ptr, crt_ptr,"2");

                if(crt_ptr->talk[0]) {
                        broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M%j %M���� \"%s\"��� �̾߱��մϴ�.", crt_ptr, "1", ply_ptr,
                                crt_ptr->talk);
                        print(fd, "\n%M%j ��ſ��� \"%s\"��� �̾߱��մϴ�.", crt_ptr,"1",
                                crt_ptr->talk);
                }
                else
                        broadcast_rom(-1, ply_ptr->rom_num,
                                "\n%M%j ���� ����� ���ϴ� �ٶ󺾴ϴ�.", crt_ptr,"0");
                if(F_ISSET(crt_ptr, MTLKAG))
                        add_enm_crt(ply_ptr->name, crt_ptr);
        }

        else {
                if(!crt_ptr->first_tlk)
                        if(!load_crt_tlk(crt_ptr))
                                return(PROMPT);

                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %M���� \"%s\"�� ���� ����ϴ�.",
                        ply_ptr, crt_ptr, cmnd->str[2]);

                tp = crt_ptr->first_tlk;
                while(tp) {
                        if(!strcmp(cmnd->str[2], tp->key)) {
                                broadcast_rom(fd, ply_ptr->rom_num,
                                        "\n%M%j %M���� \"%s\"��� �̾߱��մϴ�.", crt_ptr,"1",
                                        ply_ptr, tp->response);
                                print(fd, "\n%M%j ��ſ��� \"%s\"��� �̾߱��մϴ�.\n",
                                        crt_ptr, "1",tp->response);
                                talk_action(ply_ptr, crt_ptr, tp);
                                return(0);
                        }
                        tp = tp->next_tag;
                }
                broadcast_rom(-1, ply_ptr->rom_num, "\n%M%j ����� ���� �Ÿ��ϴ�.", crt_ptr,"1");
                if(F_ISSET(crt_ptr, MTLKAG))
                        add_enm_crt(ply_ptr->name, crt_ptr);
        }

        return(0);

}

/******************************************************************/
/*                      talk_action                               */
/******************************************************************/
/* The talk_action function handles a monster's actin when a     *
 * player asks the monster about a key word.  The format for the *
 * is defined in the monster's talk file.  Currently a monster   *
 * can attack, or cast spells on the player who mentions the key *
 * word or preform any of the defined social commands */

void talk_action(ply_ptr, crt_ptr,tt)
creature        *ply_ptr;
creature        *crt_ptr;
ttag            *tt;
{
cmd             cm;
int     i, n,splno =0;
object  *obj_ptr;
int             (*fn)();
int cnt=0;

for (i = 0; i < COMMANDMAX;i++){
        cm.str[i][0] = 0;
        cm.str[i][24] = 0;
        cm.val[i]       = 0;
}
        cm.fullstr[0] = 0;
        cm.num = 0;

switch(tt->type){
        case 1:         /*attack */
                add_enm_crt(ply_ptr->name, crt_ptr);
                broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
                                        "\n%M%j %M%j �����մϴ�.\n",crt_ptr,"1",ply_ptr,"3");
                print(ply_ptr->fd,"\n%M%j ����� �����մϴ�.\n",crt_ptr,"1");
                break;

        case 2:                         /*action command */
                if(action){
                        strncpy(cm.str[0],tt->action,25);
                        strcat(cm.fullstr,tt->action);
                        cm.val[0]  = 1;
                        cm.num = 1;

                        if(tt->target)
                                if(!strcmp(tt->target,"PLAYER")){
                                        strcpy(cm.str[1],ply_ptr->name);
                                        strcat(cm.fullstr," ");
                                        strcat(cm.fullstr,ply_ptr->name);
                                        cm.val[1]  = 1;
                                        cm.num = 2;
                                }
                        action(crt_ptr,&cm);
                }
                break;
        case 3:                 /*cast */
                if(tt->action){
                        n =  crt_ptr->mpcur;
                        strcpy(cm.str[0],"�ֹ�");
                        strncpy(cm.str[1],tt->action,25);
                        strcpy(cm.str[2],ply_ptr->name);
                        cm.val[0]  = cm.val[1]  = cm.val[2]  = 1;
                        cm.num = 3;
                        sprintf(cm.fullstr,"%s %s �ֹ�",tt->action,ply_ptr->name);

                    i = 0;
                        do {
                                if(!strcmp(tt->action, spllist[i].splstr)) {
                                splno = i;
                                break;
                        }
                        i++;
                } while(spllist[i].splno != -1);

                        if(spllist[i].splno == -1)
                                return;
                        fn = spllist[splno].splfn;
                        if(fn == offensive_spell) {
                                for(i=0; ospell[i].splno != spllist[splno].splno; i++)
                                        if(ospell[i].splno == -1) return;
                                 (*fn)(crt_ptr, &cm, CAST, spllist[splno].splstr,
            &ospell[i]);
                }
                        else if(is_enm_crt(ply_ptr->name,crt_ptr)){
                                print(ply_ptr->fd,"\n%M%j ��ſ��� � �ֹ��� �Ŵ°��� �ź��߽��ϴ�.\n",
                                        crt_ptr,"1");
                                return;
                        }
                        else
                          (*fn)(crt_ptr, &cm, CAST);

                        if (spllist[i].splno  != SRESTO && crt_ptr->mpcur == n)
                                print(ply_ptr->fd,
                        "\n%M%j ������ ��ſ��� �ֹ��� �ɾ��� �� ���ٰ� ����մϴ�.\n",
                        crt_ptr,"1");
                }
                break;
        case 4:
                for(i=0,cnt=0; i<MAXWEAR; i++)
                    if(ply_ptr->ready[i]) cnt++;
                cnt += count_inv(ply_ptr, -1);

                i = atoi(tt->action);
                if (i > 0){
                        n=load_obj(i, &obj_ptr);
            if(n > -1) {
                if(F_ISSET(obj_ptr, ORENCH))
                    rand_enchant(obj_ptr);

                if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
                   max_weight(ply_ptr) || cnt>150) {
                   print(ply_ptr->fd, "����� ���̻� ���� �� �����ϴ�.\n");
                                   break;
               }

                    if(obj_ptr->questnum && Q_ISSET(ply_ptr, obj_ptr->questnum-1)) {
                    print(ply_ptr->fd, "����� �װ��� ���� �� �����ϴ�. %s\n",
                          "����� �̹� �� �ӹ��� �޼��߽��ϴ�.");
                                        break;
                }

                        if(obj_ptr->questnum) {
                         print(ply_ptr->fd, "�ӹ� �޼�!  �װ��� ������ ���ʽÿ�.\n");
                         Q_SET(ply_ptr, obj_ptr->questnum-1);
                         ply_ptr->experience += quest_exp[obj_ptr->questnum-1];
                         print(ply_ptr->fd, "����� ����ġ %ld�� ������ϴ�.\n",
                            quest_exp[obj_ptr->questnum-1]);
                         add_prof(ply_ptr,quest_exp[obj_ptr->questnum-1]);
                        }
                add_obj_crt(obj_ptr, ply_ptr);
                                print(ply_ptr->fd,"\n%M%j ��ſ��� %i%j �ݴϴ�\n",crt_ptr,"1", obj_ptr,"3");
                                broadcast_rom(ply_ptr->fd, ply_ptr->rom_num,
                                        "\n%M%j %M���� %i%j �ݴϴ�.\n", crt_ptr,"1",ply_ptr,obj_ptr,"3");
                        }
                        else
                                print(ply_ptr->fd,"%M�� ��ſ��� �� ������ �ƹ��͵� ������ ���� �ʽ��ϴ�.\n",crt_ptr);
        }
                break;
        default:
                break;
        }
return;
}
/*************************************************************************/
/*                              chg_name                              */
/*************************************************************************/
/* ������ �̸��� �ٲٴ� ��ƾ�̴�                                        */
 
int chg_name(ply_ptr, cmnd)
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
                
    if (cmnd->num < 3){
        print(fd,"\n� ������ ���� �̸����� �ٲٰ� ��������?");
        print(fd," <����> [#] <�̸�> ���\n");
        return(PROMPT);
    }      
                  

       len=cut_command(cmnd->fullstr);
       while(isspace(cmnd->fullstr[i]))
            i++;
       while(!isspace(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;

    cmnd->val[1]= 1;
    if (isdigit(cmnd->fullstr[i]))
        cmnd->val[1] = atoi(&cmnd->fullstr[i]); 

    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], 
                           cmnd->val[1]);    


    if (!obj_ptr){
        print(fd,"�׷� ������ �����.");
        paste_command(cmnd->fullstr,len);
        return(PROMPT);
    }

/*
	if(F_ISSET(obj_ptr, ONEWEV) && !strcmp(obj_ptr->key[2], "�̺�Ʈ")) {
		strcpy(obj_ptr->key[2], ply_ptr->name);
		print(fd, "�̸��� ����Ͽ����ϴ�.");
		paste_command(cmnd->fullstr, len);
		return(PROMPT);
	}
 */   
    if (!(F_ISSET(obj_ptr, OCNAME))){
        print(fd,"��� �Ҽ� �ִ� ������ �ƴմϴ�.");
        paste_command(cmnd->fullstr,len);
        return(PROMPT);
    }


       while(isdigit(cmnd->fullstr[i]))
            i++;
       while(isspace(cmnd->fullstr[i]))
            i++;

    /* no description given */
      if (cmnd->fullstr[i] == 0) {
         paste_command(cmnd->fullstr,len);
         return(PROMPT);      
      }
     
    /*handle object modification */    

    strncpy(obj_ptr->name,&cmnd->fullstr[i],80);
    obj_ptr->name[79]=0;
    F_CLR(obj_ptr, OCNAME);
    print(fd, "\n�̸� ");

    print(fd,"��� �Ǿ����ϴ�.\n");
    F_SET(obj_ptr, ONAMED);
    broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ڽ��� ���ǿ� �̸��� ����մϴ�.",ply_ptr);
    paste_command(cmnd->fullstr,len);
    return PROMPT;
}  

int moon_set(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    object *obj_ptr=NULL;
    int fd;

    fd=ply_ptr->fd;

    if (cmnd->num < 2){
        print(fd,"\n������ �̰��� ��ġ�� ����Ű�ǲ�����?");
        print(fd," <����> [#] <�̸�> ���\n");
        return(PROMPT);
    }      
                  
    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj, cmnd->str[1], cmnd->val[1]);    

    if (!obj_ptr){
        print(fd,"�׷� ������ �����.");
        return(PROMPT);
    }

    if (ply_ptr->rom_num == 1001){
        print(fd,"���忡�� ����� ��ų �� �����ϴ�.");
        return(PROMPT);
    } 

    if (obj_ptr->value != 1001){
        print(fd,"�װ��� �̹� � ��Ҹ� ����ϰ� �ֽ��ϴ�.");
        return(PROMPT);
    }

    broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ������ ���� �̰��� ��Ҹ� ����ŵ�ϴ�.",ply_ptr);
    broadcast_rom(fd, ply_ptr->rom_num, "\n%M������ ���� ���ڱ� ���� ���� ���� �ٽ� ������ ���ϴ�.",ply_ptr);
    print(fd, "\n����� ������ ���� �̰��� ��Ҹ� ����ŵ�ϴ�.");
    print(fd, "\n������ ���� ���ڱ� ���� ���� ���� �ٽ� ������ ���ϴ�.");

    sprintf(obj_ptr->description, "%s�� ������ ��Ÿ��ϴ�.\0", ply_ptr->parent_rom->name);
    sprintf(obj_ptr->key[1], "%s\0", ply_ptr->parent_rom->name);
    obj_ptr->value=ply_ptr->parent_rom->rom_num;

    return(PROMPT);

}

 
/**********************************************************************/
/*                             kick ������                                */
/**********************************************************************/

/* �ǹ����� ������ ����̴�          */

int kick(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        long            i, t;
        int             m, n, chance, fd, p, addprof;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "���� �����մϱ�?\n");
                return(0);
        }

        if(ply_ptr->class != BARBARIAN && ply_ptr->class < INVINCIBLE) {
                print(fd, "�ǹ����� ���� �ִ� ����Դϴ�.\n");
                return(0);
        }

/*	if(F_ISSET(ply_ptr, SLEEP)) {
		print(fd, "�޽��� �ߴ��մϴ�.\n");
		F_CLR(ply_ptr, SLEEP);
	} */

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                cmnd->str[1][0] = up(cmnd->str[1][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[1], cmnd->val[1]);

                if(!crt_ptr || crt_ptr == ply_ptr) {
                        print(fd, "�׷� ���� ���� �����ϴ�.\n");
                        return(0);
                }

        }

        if(crt_ptr->type == PLAYER) {
                if(F_ISSET(rom_ptr, RNOKIL)) {
                        print(fd, "�� �濡���� �ο� �� �����ϴ�.\n");
                        return(0);
                }

		if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����ڸ� ������ �� �����ϴ�.\n");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.\n");
                    return (0);
                }
		}
		else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����ڸ� ������ �� �����ϴ�.\n");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.\n");
                    return (0);
                }
		}
        if(is_charm_crt(ply_ptr->name, crt_ptr)&& F_ISSET(crt_ptr, PCHARM)) {
                print(fd, "����� %S%j �ʹ� ������ �׷��� �� �� �����ϴ�.\n", crt_ptr->name,"3");
                return(0);
            }

        }

        i = LT(ply_ptr, LT_KICK);
        t = time(0);

        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }

        ply_ptr->lasttime[LT_KICK].ltime = t;
        ply_ptr->lasttime[LT_KICK].interval = 2;

        F_CLR(ply_ptr, PHIDDN);
        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, "����� ����� ������ �巯���ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ������ �巯���ϴ�.",
                              ply_ptr);
        }

        if(crt_ptr->type != PLAYER) {
                if(F_ISSET(crt_ptr, MUNKIL)) {
                        print(fd, "����� %s�� ��ĥ �� �����ϴ�.\n",
                                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                        return(0);
                }
 if(F_ISSET(crt_ptr, MMGONL)) {
            print(fd, "����� ������ %M���� �ƹ��ҿ��� ���µ� �մϴ�.\n",
                crt_ptr);
            return(0);
        }
        if(F_ISSET(crt_ptr, MENONL) && ply_ptr->class < CARETAKER) {
            if(!ply_ptr->ready[WIELD-1] ||
                ply_ptr->ready[WIELD-1]->adjustment < 1) {
                print(fd, "����� ������ %M���� �ƹ� �ҿ��� ���µ� �մϴ�.\n",
                    crt_ptr);
                return(0);
            }
        }
                add_enm_crt(ply_ptr->name, crt_ptr);
        }

        chance = 50 + (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*10 +
                bonus[ply_ptr->strength]*3 +
                 (bonus[ply_ptr->dexterity] -bonus[crt_ptr->dexterity])*2;
        chance += ply_ptr->class==BARBARIAN ? 5:0;
		chance += ply_ptr->class>INVINCIBLE ? 5:0;
        chance = MIN(85, chance);
        if(F_ISSET(ply_ptr, PBLIND))
                chance = MIN(20, chance);

        if(mrand(1,100) <= chance) {

                n = ply_ptr->thaco - crt_ptr->armor/8;
                if(mrand(1,20) >= n) {
                        n = mdice(ply_ptr) * 8;
                        m = MIN(crt_ptr->hpcur, n);
                        if(crt_ptr->type != PLAYER) {
                                add_enm_dmg(ply_ptr->name, crt_ptr, m);
                        }
						if(crt_ptr->class > CARETAKER) n = 1;

                        crt_ptr->hpcur -= n;

                        print(fd, "����� ������� %d���� ������ ���߽��ϴ�.\n", n);
                        broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num,
                                       "\n%M�� %M���� ������� ������ ���մϴ�.", ply_ptr, crt_ptr);
                        print(crt_ptr->fd, "%M�� ������� %d���� ������ ���߽��ϴ�.\n",
                              ply_ptr, n);

                        display_status(fd, crt_ptr);
                        print(fd, "\n");

                        ply_ptr->lasttime[LT_KICK].interval = 3;

                        if(crt_ptr->hpcur < 1) {
                                print(fd, "����� %M%j �׿����ϴ�.\n", crt_ptr,"3");
                                broadcast_rom2(fd, crt_ptr->fd,
                                               ply_ptr->rom_num,
                                               "\n%M%j %M%j �׿����ϴ�.", ply_ptr,"1",
                                               crt_ptr,"3");

                                die(crt_ptr, ply_ptr);
                        }
                        else
                                check_for_flee(ply_ptr, crt_ptr);
                }
                else {
                        print(fd, "����� �����Ⱑ �����߽��ϴ�.\n");
                        print(crt_ptr->fd, "\n%M�� ��ſ��� �����⸦ �Ϸ��� �մϴ�.\n", ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num,
                                       "\n%M�� %M���� �����⸦ �Ϸ��� �մϴ�.", ply_ptr,
                                       crt_ptr);
                }
        }

        else {
                print(fd, "����� �����Ⱑ �����߽��ϴ�.\n");
                print(crt_ptr->fd, "%M�� ��ſ��� �����⸦ �Ϸ��� �մϴ�.\n", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, crt_ptr->rom_num,
                               "\n%M�� %M���� �����⸦ �Ϸ��� �մϴ�.", ply_ptr, crt_ptr);
        }

        return(0);

}


void enemy_status(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    creature        *crt_ptr=NULL;
    room            *rom_ptr=NULL;
    ctag            *cp=NULL;
    int             fd,i=0;
    float           st=0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(F_ISSET(ply_ptr, PBLIND)) {
        print(fd, "�ƹ��͵� ������ �ʽ��ϴ�!\n");
        return;
    }

    if(crt_ptr==NULL) {
        if(cmnd->num < 2) {
            print(fd, "������ ���¸� ���ðڽ��ϱ�?\n");
            return;
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
    }

    if(crt_ptr==NULL) {
        print(fd, "�׷����� ���⿡ �����ϴ�.\n");
        return;
    } 

    print(fd, "%s : ", crt_ptr->name);
    display_status(fd, crt_ptr);
    print(fd, "\n");

}
