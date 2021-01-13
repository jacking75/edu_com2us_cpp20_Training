/*
 * COMMAND7.C:
 *
 *      Additional user routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */
#include <stdlib.h>
#include "mstruct.h"
#include "mextern.h"
extern long all_broad_time;
/**********************************************************************/
/*                              flee                                  */
/**********************************************************************/
/* This function allows a player to flee from an enemy.  If successful */
/* the player will drop his readied weapon and run through one of the  */
/* visible exits, losing 10% or 1000 experience, whichever is less.    */
int flee(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        xtag    *xp;
        ctag    *cp, *temp;
        char    found = 0;
        int     fd, n;
        long    i, t;
        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;
        t = time(0);
        i = MAX(ply_ptr->lasttime[LT_ATTCK].ltime,
                ply_ptr->lasttime[LT_SPELL].ltime);
        if(t < i && !F_ISSET(ply_ptr, PFEARS)) {
                please_wait(fd, i-t);
                return(0);
        }
        t = Time%24L;
        xp = rom_ptr->first_ext;
        if(xp) do {
                if(F_ISSET(xp->ext, XCLOSD)) continue;
                if((F_ISSET(xp->ext, XCLIMB) || F_ISSET(xp->ext, XDCLIM)) &&
(!F_ISSET(ply_ptr, PLEVIT))) continue;
                if(F_ISSET(xp->ext, XNAKED) && weight_ply(ply_ptr)) continue;
        if(F_ISSET(xp->ext, XFEMAL) && F_ISSET(ply_ptr, PMALES)) continue;
        if(F_ISSET(xp->ext, XMALES) && !F_ISSET(ply_ptr, PMALES)) continue;
                if(F_ISSET(xp->ext, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP))
                        continue;
                if(!F_ISSET(ply_ptr, PDINVI) && F_ISSET(xp->ext, XINVIS))
                        continue;
                if(F_ISSET(xp->ext, XSECRT)) continue;
                if(F_ISSET(xp->ext, XNOSEE)) continue;
                if(F_ISSET(xp->ext, XNGHTO) && (t>6 && t < 20)) continue;
                if(F_ISSET(xp->ext, XDAYON) && (t<6 || t > 20))  continue;
    if(F_ISSET(xp->ext,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR) &&
            !F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
                found = 1;
                break;
            }
            cp = cp->next_tag;
        }
        if(found)
                continue;
    }
            /* �������� ����ó�� */
           if (!ply_is_attacking(ply_ptr,cmnd)){
               print(fd,"�������Լ� �������÷�����?");
               return 0;
            }
/* if(mrand(1,100) < 70) break;     */
                if(mrand(1,100) < (65 + bonus[ply_ptr->dexterity]*5))
                        break;
        } while(xp = xp->next_tag);

        if(xp && F_ISSET(xp->ext,52) && mrand(1,5) < 2 && !F_ISSET(ply_ptr, PFEARS))
                xp = 0;
        if(!xp) {
                print(fd, "\n����� �̿� ���� �ٸ��� �������� �ʽ��ϴ�!");
                return(0);
        }
/*
        if(ply_ptr->ready[WIELD-1] && !ply_ptr->ready[WIELD-1]->questnum &&
           !F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) {
                add_obj_rom(ply_ptr->ready[WIELD-1], rom_ptr);
                ply_ptr->ready[WIELD-1] = 0;
                compute_thaco(ply_ptr);
                compute_ac(ply_ptr);
                print(fd, "\n����� ���⸦ ������ ������� Ĩ�ϴ�.");
        }
        else
 */               print(fd, "\n����� ������� Ĩ�ϴ�.");
        F_CLR(ply_ptr, PHIDDN);
        if(F_ISSET(ply_ptr, PFEARS)) {
                ANSI(fd, RED);
                print(fd, "\n����� �̿� ������ ���� â���ϰ� ���� ������ ���ϴ�!");
                ANSI(fd, WHITE);
        }

        if(!F_ISSET(rom_ptr, RPTRAK))
                strcpy(rom_ptr->track, xp->ext->name);
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %s������ ������ ���ϴ�.",
                      ply_ptr, "1",xp->ext->name);
        if (ply_ptr->class == PALADIN)
                if (ply_ptr->level > 20) {
                n = ((ply_ptr->level+3)/4)*10;
                n = MIN(ply_ptr->experience, n);
                print(fd,"����� ������ �� �񰡷� %d ��ŭ�� ����ġ�� �Ҿ����ϴ�.",n);
                ply_ptr->experience -= n;
                lower_prof(ply_ptr,n);
        }
        load_rom(xp->ext->room, &rom_ptr);
        if(rom_ptr->lolevel > ply_ptr->level ||
           (ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel)) {
                print(fd, "� ���� ���� �ٽ� �ǵ��� �Խ��ϴ�.");
                broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j �����Ͽ����ϴ�.",ply_ptr,"1");
                return(0);
        }
        n = count_vis_ply(rom_ptr);
        if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
           (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
           (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
                print(fd, "���������� ���� ������ ���� á���ϴ�!");
                broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j �����Ͽ����ϴ�.",ply_ptr,"1");
                return(0);
        }
        del_ply_rom(ply_ptr, ply_ptr->parent_rom);
        add_ply_rom(ply_ptr, rom_ptr);
        check_traps(ply_ptr, rom_ptr);
        return(0);
}
/**********************************************************************/
/*                              list                                  */
/**********************************************************************/
/* This function allows a player to list the items for sale within a */
/* shoppe.                                                           */
int list(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr, *dep_ptr;
        otag    *op;
        int     fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(F_ISSET(rom_ptr, RSHOPP)) {
                if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
                        print(fd, "�� ������ �����ϴ�.");
                        return(0);
                }
                print(fd, "��ǰ��:");
                op = dep_ptr->first_obj;
                while(op) {
                        print(fd, "\n   %-30s   ����: %ld",
                              obj_str(op->obj, 1, CAP), op->obj->value);
                        op = op->next_tag;
                }
        }
        else
                print(fd, "����� ������ �ƴմϴ�.");
        return(0);
}
/**********************************************************************/
/*                              buy                                   */
/**********************************************************************/
/* This function allows a player to buy something from a shoppe.      */
int buy(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr, *dep_ptr;
        object  *obj_ptr, *obj_ptr2;
        int     fd,i,cnt=0;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        for(i=0,cnt=0; i<MAXWEAR; i++)
           if(ply_ptr->ready[i]) cnt++;
        cnt += count_inv(ply_ptr, 0);
        if(!F_ISSET(rom_ptr, RSHOPP)) {
                print(fd, "����� ������ �ƴմϴ�.");
                return(0);
        }
        if(cmnd->num < 2) {
                print(fd, "������ ��÷�����?");
                return(0);
        }
        if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
                print(fd, "�� ������ �����ϴ�.");
                return(0);
        }
        obj_ptr = find_obj(ply_ptr, dep_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
        if(!obj_ptr) {
                print(fd, "�׷� ������ ���� �ʽ��ϴ�.");
                return(0);
        }
        if(ply_ptr->gold < obj_ptr->value) {
                print(fd, "���� �����鼭... �ܻ����!");
                return(0);
        }
                if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
                   max_weight(ply_ptr) || cnt>200) {
                   print(fd, "����� ���̻� ���� �� �����ϴ�.");
                   return(0);
               }
        obj_ptr2 = (object *)malloc(sizeof(object));
        if(!obj_ptr2)
                merror("buy", FATAL);
        F_CLR(ply_ptr, PHIDDN);
        *obj_ptr2 = *obj_ptr;
        F_CLR(obj_ptr2, OPERM2);
        F_CLR(obj_ptr2, OPERMT);
        F_CLR(obj_ptr2, OTEMPP);
        add_obj_crt(obj_ptr2, ply_ptr);
        ply_ptr->gold -= obj_ptr2->value;
        print(fd, "����� %1i%j ����ϴ�",obj_ptr2,"3");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %1i%j ����ϴ�.",
                      ply_ptr, obj_ptr2,"3");
        return(0);
}
/**********************************************************************/
/*                              sell                                  */
/**********************************************************************/
/* This function will allow a player to sell an object in a pawn shoppe */
int sell(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     gold, fd, poorquality = 0;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(!F_ISSET(rom_ptr, RPAWNS)) {
                print(fd, "����� �������� �ƴմϴ�.");
                return(0);
        }
        if(cmnd->num < 2) {
                print(fd, "������ �Ľ÷�����?");
                return(0);
        }
        F_CLR(ply_ptr, PHIDDN);
        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
        if(!obj_ptr) {
                print(fd, "����� �׷� ������ ���� ���� �ʽ��ϴ�.");
                return(0);
        }
        gold = MIN(obj_ptr->value / 2, 100000);
        if((obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR) &&
           obj_ptr->shotscur <= obj_ptr->shotsmax/8)
                poorquality = 1;
        if((obj_ptr->type == WAND || obj_ptr->type == KEY) && obj_ptr->shotscur < 1)
                poorquality = 1;
        if(gold < 20 || poorquality) {
                print(fd, "������������ \"�׷� ������� �Ȼ��!\"��� ���մϴ�.");
                return(0);
        }
        if(F_ISSET(obj_ptr, ONEWEV)) {
                        print(fd, "������������ \"�׷��� �Ȼ��!\"��� ���մϴ�.");
                        return(0);
        }
		if(obj_ptr->first_obj) {
			print(fd, "������������ \"�� �ȿ� ������ ����ֱ���.\"��� ���մϴ�.");
			return(0);
		}
        if(obj_ptr->type == SCROLL || obj_ptr->type == POTION) {
                print(fd, "������������ \"�η縶�⳪ ���ర������ �Ȼ��!\"��� ���մϴ�.");
                return(0);
        }
        if(((time(0)+mrand(1, 100))%250) == 9) {
                print(fd, "���� %i%j ����.\n ������ ����� ������ �ι�� �帮��.\n
                        ������������ ��ſ��� %d���� �ݴϴ�.", obj_ptr, "3",gold*2);
                broadcast_rom(fd, ply_ptr->rom_num,  "\n%M�� ���������ο���  %1i%j �˴ϴ�.",
                        ply_ptr, obj_ptr, "3");
                ply_ptr->gold += gold;
        }
        else {
        print(fd, "���� %i%j ����.\n������������ ��ſ��� %d���� �ݴϴ�.", obj_ptr, "3",gold);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ���������ο��� %1i%j �˴ϴ�.",
                      ply_ptr, obj_ptr,"3");
        }
        ply_ptr->gold += gold;
        del_obj_crt(obj_ptr, ply_ptr);
        free_obj(obj_ptr);
        return(0);
}
/**********************************************************************/
/*                              value                                 */
/**********************************************************************/
/* This function allows a player to find out the pawn-shop value of an */
/* object, if he is in the pawn shop.                                  */
int value(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        long    value;
        int     fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(!F_ISSET(rom_ptr, RPAWNS) && !F_ISSET(rom_ptr, RREPAI)) {
                print(fd, "�������� ���ż� ������ ��ġ�� �˾ƺ�����.");
                return(0);
        }
        if(cmnd->num < 2) {
                print(fd, "� ������ ��ġ�� �˰� ��������?");
                return(0);
        }
        F_CLR(ply_ptr, PHIDDN);
        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
        if(!obj_ptr) {
                print(fd, "����� �׷� ������ ���� ���� �ʽ��ϴ�.");
                return(0);
        }
        if(F_ISSET(rom_ptr, RPAWNS)) {
                value = MIN(obj_ptr->value / 2, 100000L);
                print(fd, "���������� \"%I�̶�� %ld��  ������ �帱 ��  �־��.\"��� ���մϴ�.",
                      obj_ptr, value);
        }
        else {
                value = obj_ptr->value / 4;
                print(fd,
                      "���������� \"%I%j �����ϴµ� %ld���� ��ϴ�.\"��� ���մϴ�.",
                      obj_ptr, "3",value);
        }
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %i�� ��ġ�� �˾ƺ��ϴ�.",
                      ply_ptr, obj_ptr);
        return(0);
}
/**********************************************************************/
/*                              backstab                              */
/**********************************************************************/
/* This function allows thieves and assassins to backstab a monster.  */
/* If successful, a damage multiplier is given to the player.  The    */
/* player must be successfully hidden for the backstab to work.  If   */
/* the backstab fails, then the player is forced to wait double the   */
/* normal amount of time for his next attack.                         */
int backstab(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        long            i, t;
        int             fd, m, n, p, addprof;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(ply_ptr->class != THIEF && ply_ptr->class != ASSASSIN &&
           ply_ptr->class < INVINCIBLE) {
                print(fd, "���ϰ� �ڰ��� ����� �� �ִ� ����Դϴ�.");
                return(0);
        }
        if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "������ ����Ͻ÷�����?");
                return(0);
        }
        if(!ply_ptr->ready[WIELD-1] || (ply_ptr->ready[WIELD-1]->type
           != SHARP && ply_ptr->ready[WIELD-1]->type != THRUST)) {
                print(fd, "����� �Ͻ÷��� ���� �������� ���Ⱑ �ʿ��մϴ�.");
                return(0);
        }
        t = time(0);
        i = LT(ply_ptr, LT_ATTCK);
        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
        if(!crt_ptr) {
                cmnd->str[1][0] = up(cmnd->str[1][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[1], cmnd->val[1]);
                if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 2) {
                        print(fd, "�׷��� ���� �����.");
                        return(0);
                }
        }
        if(crt_ptr->type != PLAYER && is_enm_crt(ply_ptr->name, crt_ptr)) {
                print(fd, "����� %s�� �ο� �� �����ϴ�.",
                      F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }
        if(crt_ptr->type == PLAYER) {
                if(F_ISSET(rom_ptr, RNOKIL)) {
                        print(fd, "�� �濡���� �ο� �� �����ϴ�.");
                        return(0);
                }
		if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����� ������ �� �����ϴ�.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.");
                    return (0);
                }
		}
		else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "����� ���ؼ� �ٸ� ����� ������ �� �����ϴ�.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ��ȣ�ް� �ֽ��ϴ�.");
                    return (0);
                }
		}
                if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)){
                        print (fd,  "����� %S%j �Ƴ��� ������ �׸� ��ĥ �� �����ϴ�.", crt_ptr->name,"3");
                        return(0);
                }
                print(crt_ptr->fd, "\n%M%j ����� �ڷ� ���� �� �������� ��~ ��ϴ�.", ply_ptr,"1");
        }
        else if(F_ISSET(crt_ptr, MUNKIL)) {
                print(fd, "����� %s�� ��ĥ�� �����ϴ�.",
                        F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }
        if(is_charm_crt(crt_ptr->name, ply_ptr) && F_ISSET(ply_ptr, PCHARM)) {
                print(fd, "�����  %s��  �ʹ� �����ؼ�   �׷��� �� ��Ⱑ   ���� �ʴ±���.",crt_ptr->name);
                return(0);
        }
        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, "����� ����� ������ �巯���ϴ�.");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ������ �巯���ϴ�.",
                              ply_ptr);
        }
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        if(ply_ptr->dexterity > 18)
                ply_ptr->lasttime[LT_ATTCK].interval = 1;
        else
                ply_ptr->lasttime[LT_ATTCK].interval = 2;
        if(crt_ptr->type != PLAYER) {
                if(add_enm_crt(ply_ptr->name, crt_ptr) < 0) {
                        print(fd, "����� %M��  �ڷ� ����  �� ��������  ��~ ��ϴ�.", crt_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� %M��   �������� ��~  ��ϴ�.",  ply_ptr, crt_ptr);
                        if(F_ISSET(crt_ptr, MMGONL)) {
                print(fd, "����� ����� %M���Դ� �ƹ� �ҿ��� �����ϴ�.", crt_ptr);
                return(0);
                }
                if(F_ISSET(crt_ptr, MENONL)) {
                if(!ply_ptr->ready[WIELD-1] ||
                        ply_ptr->ready[WIELD-1]->adjustment < 1) {
                        print(fd, "����� ����� %M���� �ƹ� �ҿ��� �����ϴ�.", crt_ptr);
                        return(0);
            }
        }
                }
        }
        else {
                print(fd, "����� %M�� �ڷ� ���� �� �������� ��~ ��ϴ�.", crt_ptr);
                print(crt_ptr->fd, "%M�� ����� �������� ��~ ��ϴ�.", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                               "\n%M�� %M�� �������� ��~ ��ϴ�!", ply_ptr, crt_ptr);
        }
        if(ply_ptr->ready[WIELD-1]) {
                if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
                        print(fd, "\n%S%j �μ������ϴ�.",
                              ply_ptr->ready[WIELD-1]->name,"1");
                        add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
                        ply_ptr->ready[WIELD-1] = 0;
                        broadcast_rom(fd, ply_ptr->rom_num,
                                      "\n%s�� ����� �õ������� ������ ���߽��ϴ�.",
                                      F_ISSET(ply_ptr, PMALES) ? "��":"�׳�");
                        ply_ptr->lasttime[LT_ATTCK].interval *= 2;
                        return(0);
                }
        }
        n = ply_ptr->thaco - crt_ptr->armor/8 + 2;
        if(!F_ISSET(ply_ptr, PHIDDN))
                n = 21;
        F_CLR(ply_ptr, PHIDDN);
        if(mrand(1,20) >= n) {
                if(ply_ptr->ready[WIELD-1])
                        n = mdice(ply_ptr->ready[WIELD-1]);
                else
                        n = mdice(ply_ptr);
                if(ply_ptr->class == THIEF)
                        n *= (mrand(20,35) / 10);
                else
                        n *= 5;
                m = MIN(crt_ptr->hpcur, n);
                if(crt_ptr->type != PLAYER) {
                        add_enm_dmg(ply_ptr->name, crt_ptr, m);
                        if(ply_ptr->ready[WIELD-1]) {
                                p = MIN(ply_ptr->ready[WIELD-1]->type, 4);
                                addprof = (m * crt_ptr->experience) /
                                        crt_ptr->hpmax;
                                addprof = MIN(addprof, crt_ptr->experience);
                                ply_ptr->proficiency[p] += addprof;
                        }
                }
                                if(crt_ptr->class > CARETAKER) n = 1;
                 crt_ptr->hpcur -= n;
                print(fd, "\n����� %d ��ŭ�� ���ظ� �־����ϴ�.", n);
                print(crt_ptr->fd,  "\n�����  %M���Լ�  %d   ��ŭ�� ���ظ�   �Ծ����ϴ�.", ply_ptr, n);
                if(crt_ptr->hpcur < 1) {
                        print(fd, "\n����� %M%j �׿����ϴ�.", crt_ptr,"3");
                        broadcast_rom(fd, ply_ptr->rom_num,
                                      "\n%M%j %M%j �׿����ϴ�.", ply_ptr,"1", crt_ptr,"3");
                        if(ply_ptr->ready[WIELD-1])
                                ply_ptr->ready[WIELD-1]->shotscur--;
                        die(crt_ptr, ply_ptr);
                }
                else
                        check_for_flee(ply_ptr, crt_ptr);
       }
        else {
                print(fd, "\n����� �ƽ��ϴ�.");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%s�� ����� �õ������� ������ ���߽��ϴ�.",
                              F_ISSET(ply_ptr, PMALES) ? "��":"�׳�");
                ply_ptr->lasttime[LT_ATTCK].interval *= 3;
        }
        return(0);
}
/**********************************************************************/
/*                              train                                 */
/**********************************************************************/
/* This function allows a player to train if he is in the correct     */
/* training location and has enough gold and experience.  If so, the  */
/* character goes up a level.                                         */
int train(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        long    goldneeded, expneeded, lv;
        int     fd, i, fail = 0, bit[4];
        int up_num=0;
        fd = ply_ptr->fd;
        /* ���� ���� ���� */
        if(ply_ptr->class == CARETAKER) {
                print(fd, "����� �����Ҽ� ���� �����Դϴ�.");
                /*
                ANSI(fd, RED);
                ply_ptr->mpmax = ply_ptr->mpmax + 10;
                ply_ptr->hpmax = ply_ptr->hpmax + 10;
                print(fd, "����� ���� ���� ������ �մϴ�.");
                ANSI(fd, WHITE);
                */
                return(0);
        }
        if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "����� ���� �־� ������ �� �����ϴ�!");
                ANSI(fd, WHITE);
                return(0);
        }
        /* ��°ݹ� ����߿� ���ý� ��°ݹ� ���� */
        if(F_ISSET(ply_ptr, PUPDMG)) {
                F_CLR(ply_ptr, PUPDMG);
                ply_ptr->pdice -= 5;
                ply_ptr->hpmax -= 100;
                ply_ptr->mpmax -= 100;
                ply_ptr->hpcur = ply_ptr->hpmax;
                ply_ptr->mpcur = ply_ptr->mpmax;
        }
        rom_ptr = ply_ptr->parent_rom;
        if(!F_ISSET(rom_ptr, RTRAIN)) {
                print(fd, "�� ���� ������ �� �ִ°��� �ƴմϴ�!");
                return(0);
        }
        
        ply_ptr->class--;
        bit[0] = ply_ptr->class & 1;
        bit[1] = ply_ptr->class & 2;
        bit[2] = ply_ptr->class & 4;
        ply_ptr->class++;
        for(i=0; i<3; i++) {
                if((bit[i]?1:0)!=(F_ISSET(rom_ptr, RTRAIN+3-i)?1:0)) fail=1;
                /*
                if(!((bit[i] && F_ISSET(rom_ptr, RTRAIN+3-i)) ||
                   (!bit[i] && !F_ISSET(rom_ptr, RTRAIN+3-i))))
                        fail = 1;
                */
        }
        if(ply_ptr->class>8) fail=0;
        if(fail) {
                print(fd, "����� �����ϴ°��� ���Ⱑ �ƴմϴ�.");
                return(0);
        }
        if(ply_ptr->level <= MAXALVL)
                expneeded = needed_exp[ply_ptr->level-1];
        else
            expneeded = needed_exp[MAXALVL-2]
                +((long)ply_ptr->level-MAXALVL+1)*5000000;
        if(ply_ptr->level<MAXALVL) goldneeded = (expneeded / 10L)/2;
        else                       goldneeded = (needed_exp[MAXALVL-2] /10L)/2;
        if(expneeded > ply_ptr->experience) {
                print(fd, "����� %ld ��ŭ�� ����ġ�� �� �ʿ��մϴ�.",
                      expneeded - ply_ptr->experience);
                return(0);
        }
        if(goldneeded > ply_ptr->gold) {
                print(fd, "���� �����鼭... �� ���������.\n");
                print(fd, "����� �����ϴµ� %ld���� ��ϴ�.", goldneeded);
                return(0);
        }
       /*   ���� ��ȯ */
        if((ply_ptr->level==100) && (ply_ptr->class < INVINCIBLE)) {
                ply_ptr->class = INVINCIBLE;
                        if(F_ISSET(ply_ptr, PFAMIL)) {
                                edit_member(ply_ptr->name,                  ply_ptr->class, 
ply_ptr->daily[DL_EXPND].max, 3);
                        }
                ply_ptr->level = 1;
                ply_ptr->experience = 0;
        }

        /* ������ȯ */
        if((ply_ptr->level>=127) && (ply_ptr->class == INVINCIBLE)) {
                ply_ptr->class = CARETAKER;
                ply_ptr->level = 127;
                        if(F_ISSET(ply_ptr, PFAMIL)) {
                                edit_member(ply_ptr->name,                  ply_ptr->class, 
ply_ptr->daily[DL_EXPND].max, 3);
                        }
              ply_ptr->gold -= goldneeded;
              ply_ptr->hpmax = 800;
              ply_ptr->mpmax = 600;
                        ply_ptr->ndice = 4;
                        ply_ptr->sdice = 4;
                        ply_ptr->pdice = 4;
              broadcast_all("\n### %s�Բ��� ������ �Ǽ̽��ϴ�!!",ply_ptr->name);
                print(fd,"\n�����մϴ�! ����� ������ �Ǿ����ϴ�!!");
                all_broad_time=time(0);
                return(0);
        } 
        else {
    up_num=0;
    do {
        if(ply_ptr->level==100 && (ply_ptr->class < INVINCIBLE)) break;
        ply_ptr->gold -= goldneeded;
        up_level(ply_ptr);
        up_num++;
        if(ply_ptr->level <= MAXALVL)
            expneeded = needed_exp[ply_ptr->level-1];
        else
            expneeded = needed_exp[MAXALVL-2]
                +((long)ply_ptr->level-MAXALVL+1)*5000000;
        if(ply_ptr->level<MAXALVL) goldneeded = (expneeded / 10L)/2;
        else                       goldneeded = (needed_exp[MAXALVL-2] /10L)/2;
    } while(expneeded <= ply_ptr->experience && goldneeded <= ply_ptr->gold);

        if(up_num<=1)
            broadcast_all("\n### %s���� %d������ �ö����ϴ�!",
                ply_ptr->name,ply_ptr->level);
        else
            broadcast_all("\n### %s���� %d������ %d�ܰ� �ö����ϴ�!",
                ply_ptr->name,ply_ptr->level,up_num);
        print(fd, "\n�����մϴ�! ����� ������ �ö����ϴ�!");
        all_broad_time=time(0);
        return(0);
        }
}
/**********************************************************************/
/*                              forge����                             */
/**********************************************************************/
/*  ���ο� �ڽŸ��� ���⸦ �����ϴ� ��ƾ�̴�     */
/*  ������ ��ȣ 900������ �� �� �� â �� ������ ������ */
void select_arm();
int forge(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

/*
        print(fd, "���׷� ��� ��������մϴ�\n");
        return(0); 
*/
        if(!F_ISSET(rom_ptr, RFORGE)) {
                print(fd, "����� ���尣�� �ƴմϴ�.");
                return(0);
        }
 
        F_SET(ply_ptr, PNOBRD);    
        select_arm(fd, 1, NULL);
        F_CLR(ply_ptr, PNOBRD);
        
        return(DOPROMPT);
}
/**********************************************************************/
/*                             select_arm                            */
/**********************************************************************/
/* ������ Ư���� �����ϴ� �κ��̴�.             */
char forge1[PMAX][16];
char forge2[PMAX][16];
void select_arm(fd, param, str)
int fd;
int        param;
char       *str;
{
    int     i;
    long    sum=0;
    int     num[5];
    creature *ply_ptr;
    object  *obj_ptr;
    ply_ptr=Ply[fd].ply;
    switch(param) {
    case 1:
        print(fd,"\n\n");
        print(fd, "� ������ ���⸦ ���Ͻʴϱ�?\n1. �� 2. �� 3. �� 4. â 5. �� ?\n:");
        F_SET(Ply[fd].ply,PREADI);   
        output_buf();
        Ply[fd].io->intrpt &= ~1;
        RETURN(fd, select_arm, 2);
    case 2:
        F_CLR(Ply[fd].ply,PREADI); 
        if(str[0]>='1' && str[0]<='5') {
            if(load_obj(900+str[0]-'1', &obj_ptr) < 0) {
                print(ply_ptr->fd, "Error (%d)\n", 900+str[0]-'1');
                return;
            }
        }
        else {
            print(fd, "�ϳ��� �����Ͻÿ�: ");
            F_SET(Ply[fd].ply,PREADI); 
            RETURN(fd, select_arm, 2);
        }
        /* ������ obj_ptr�� long������ �ٲ� ����. */
        sprintf(forge1[fd],"%lu",(long)obj_ptr);
        print(fd, "\nŸ��ġ�� ������ �ִ� ��Ḧ �����Ͻʽÿ�.\n");
        print(fd, "1.�� ö ������ 2.�ͱݼ� �̽ʸ��� 3.�ݰ��� ��ʸ���\n:");
        F_SET(Ply[fd].ply,PREADI); 
        RETURN(fd, select_arm, 3);
    case 3:
        F_CLR(Ply[fd].ply,PREADI); 
        obj_ptr=(object *)atol(forge1[fd]);
        switch(low(str[0])) {
              case '1':
                    obj_ptr->sdice = 3;
                    sum = 50000;
                    break;
              case '2':
                    obj_ptr->sdice = 4;
                    sum = 200000;
                    break;
              case '3':
                    if((ply_ptr->class == CLERIC)||(ply_ptr->class == PALADIN)||(ply_ptr->class == MAGE)) {
                        print(fd, "����� �̷� ���⸦ ����� �ɷ��� �����ϴ�.\n");
                        print(fd, "�ٸ���Ḧ �����Ͻʽÿ�\n");
                        F_SET(Ply[fd].ply,PREADI); 
                        RETURN(fd, select_arm, 3);
                    }
                    F_SET(obj_ptr,OCLSEL);
                    F_SET(obj_ptr,OASSNO);
                    F_SET(obj_ptr,OBARBO);
                    F_SET(obj_ptr,OFIGHO);
                    F_SET(obj_ptr,ORNGRO);
                    F_SET(obj_ptr,OTHIEO);
                    obj_ptr->sdice = 5;
                    sum = 300000;
              break;
              default: print(fd, "�ϳ��� �����Ͻÿ�: ");
                   F_SET(Ply[fd].ply,PREADI); 
                   RETURN(fd, select_arm, 3);
        }
        sprintf(forge2[fd],"%lu",sum);
        print(fd, "\n�Ǹ���  �����  ������� ��������  ���˴ϴ�.\n�����  ������� ���մϱ�?\n");
        print(fd, "1. 100�� ������ 2. 200�� �̽ʸ���  3. 300�� ���ʸ���\n4. 400�� �鸸�� 5. 500�� �̹鸸��\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI); 
        RETURN(fd, select_arm, 4);
    case 4:
        F_CLR(Ply[fd].ply,PREADI); 
        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);
        switch(low(str[0])) {
              case '1':
                   obj_ptr->shotsmax = 100;
                   obj_ptr->shotscur = 100;
                   sum += 50000;
                   break;
              case '2':
                   obj_ptr->shotsmax = 200;
                   obj_ptr->shotscur = 200;
                   sum += 200000;
                   break;
              case '3':
                   obj_ptr->shotsmax = 300;
                   obj_ptr->shotscur = 300;
                   sum += 500000;
                   break;
              case '4':
                   obj_ptr->shotsmax = 400;
                   obj_ptr->shotscur = 400;
                   sum += 1000000;
                   break;
              case '5':
                   obj_ptr->shotsmax = 500;
                   obj_ptr->shotscur = 500;
                   sum += 2000000;
                   break;
              default: print(fd, "�ϳ��� �����Ͻÿ�: ");
                   F_SET(Ply[fd].ply,PREADI);  
                   RETURN(fd, select_arm, 4);
        }
        sprintf(forge2[fd],"%lu",sum);
        print(fd, "\n����� ������ �̸��� �����ʽÿ�.\n");
        print(fd, "���߿� �̸��� ��ĥ�� ������ �����ؼ� �����ž� �մϴ�\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);   
        RETURN(fd, select_arm, 5);
    case 5:
        F_CLR(Ply[fd].ply,PREADI);   
        obj_ptr=(object *)atol(forge1[fd]);
        if(strchr(str,'(') || strchr(str,')')) {
            print(fd,"�̸����� ��ȣ�� ���� �����ϴ�.\n�̸��� �ٽ� �����ʽÿ�:");
            RETURN(fd,select_arm,5);
        }
        if(strlen(str) > 20) {
                        print(fd, "�Էµ� �̸��� �ʹ�  ��ϴ�.\n�̸��� �ٽ� �����ʽÿ�(3���̻� 20������): ");
                        RETURN(fd, select_arm, 5);
        }
        if(strlen(str) < 3) {
                        print(fd, "�Էµ� �̸��� �ʹ�  ª���ϴ�.\n�̸��� �ٽ� �����ʽÿ�(3���̻� 20������): ");
                        F_SET(Ply[fd].ply,PREADI);          
                        RETURN(fd, select_arm, 5);
        }
        strncpy(obj_ptr->name, str, 20);
        print(fd, "\n���Ϳ� �����Ͻʴϱ�? (��/�ƴϿ�)\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);    
        RETURN(fd, select_arm, 6);
    case 6:
        F_CLR(Ply[fd].ply,PREADI);         
        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);
        if(!strncmp(str,"��",2)) {
                   if (ply_ptr->gold < sum) {
                        print(fd, "\n����� �� ������ ���⸦  ���� ���� ���� �����ϴ�.\n������ ��ſ��� \"�ܻ��� �ȵǿ�~\"��� ���մϴ�.");
                        free(obj_ptr);
                        break;
                   }
                   add_obj_crt(obj_ptr,ply_ptr);
                   print(fd, "\n������ ��ſ��� ���� ���۵� ���⸦ �ǳ��ݴϴ�.");
                   broadcast_rom(fd,   ply_ptr->rom_num,  "\n%M��   ���⸦  �����Ͽ����ϴ�.",ply_ptr);
                   ply_ptr->gold -= sum;
        }
        else {
            print(fd,"���� ������ ����Ͽ����ϴ�.");
            free(obj_ptr);
        }
        break;
    }
    RETURN(fd, command, 1);
}

/*************************************************************************************
*****/

/**********************************************************************/
/*                              newforge����                             */
/**********************************************************************/
/*  ���ο� �ڽŸ��� ���⸦ �����ϴ� ��ƾ�̴�     */
/*  ������ ��ȣ 900������ �� �� �� â �� ������ ������ */
void select_newarm();

int newforge(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;


/*
        print(fd, "���׷� ��� ��������մϴ�\n");
        return(0); 
*/
        if(!F_ISSET(rom_ptr, RFORGE)) {
                print(fd, "����� ���尣�� �ƴմϴ�.");
                return(0);
        }

	    if(!(rom_ptr->rom_num == 611 )) {
		       print(fd, "���⼭�� ���⸦ ���� ���� �����ϴ�.");
 	           return(0);
        }
 
        F_SET(ply_ptr, PNOBRD);    
        select_newarm(fd, 1, NULL);
        F_CLR(ply_ptr, PNOBRD);
        

        return(DOPROMPT);

}

/**********************************************************************/
/*                             select_arm                            */
/**********************************************************************/

/* ������ Ư���� �����ϴ� �κ��̴�.             */
char forge1[PMAX][16];
char forge2[PMAX][16];

void select_newarm(fd, param, str)
int fd;
int        param;
char       *str;
{
    int     i;
    long    sum=0;
    int     num[5];
    creature *ply_ptr;
    object  *obj_ptr;

    ply_ptr=Ply[fd].ply;
    switch(param) {
    case 1:
        print(fd,"\n\n");
        print(fd, "� ������ ���⸦ ���Ͻʴϱ�?\n1. �� 2. �� 3. �� 4. â 5. �� ?\n:");
        F_SET(Ply[fd].ply,PREADI);   
        output_buf();
        Ply[fd].io->intrpt &= ~1;

        RETURN(fd, select_newarm, 2);
    case 2:
        F_CLR(Ply[fd].ply,PREADI); 

        if(str[0]>='1' && str[0]<='5') {
            if(load_obj(900+str[0]-'1', &obj_ptr) < 0) {
                print(ply_ptr->fd, "Error (%d)\n", 900+str[0]-'1');
                return;
            }
        }
        else {
            print(fd, "�ϳ��� �����Ͻÿ�: ");
            F_SET(Ply[fd].ply,PREADI); 
            RETURN(fd, select_newarm, 2);
        }

        /* ������ obj_ptr�� long������ �ٲ� ����. */
        sprintf(forge1[fd],"%lu",(long)obj_ptr);

        print(fd, "\nŸ��ġ�� ������ �ִ� ��Ḧ �����Ͻʽÿ�.\n");
        print(fd, "1.���޶��� 100���� 2.ƼŸ�� 200����  3.�Ϸ�� 300����\n:");
        F_SET(Ply[fd].ply,PREADI); 

        RETURN(fd, select_newarm, 3);
    case 3:
        F_CLR(Ply[fd].ply,PREADI); 

        obj_ptr=(object *)atol(forge1[fd]);

        switch(low(str[0])) {
              case '1':
		    F_SET(obj_ptr,OENCHA);
		    obj_ptr->ndice = 4;
                    obj_ptr->sdice = 5;
		    obj_ptr->pdice = 5;
                    sum = 1000000;
                    break;
              case '2':
		    F_SET(obj_ptr,OENCHA);
		    obj_ptr->ndice = 5;
            obj_ptr->sdice = 5;
		    obj_ptr->pdice = 5;
                    sum = 2000000;
                    break;
              case '3':
		    F_SET(obj_ptr,OENCHA);
		    obj_ptr->ndice = 6;
            obj_ptr->sdice = 5;
		    obj_ptr->pdice = 5;
                    sum = 3000000;
              break;
              default: print(fd, "�ϳ��� �����Ͻÿ�: ");
                   F_SET(Ply[fd].ply,PREADI); 

                   RETURN(fd, select_newarm, 3);
        }

        sprintf(forge2[fd],"%lu",sum);
        print(fd, "\n�Ǹ���  �����  ������� ��������  ���˴ϴ�.\n�����  ������� ���մϱ�?\n");
        print(fd, "1. 100��  �鸸�� 2. 300��  2�鸸�� 3. 500��  3�鸸��\n4. 700�� 4�鸸�� 5. 900�� 5�鸸��\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI); 

        RETURN(fd, select_newarm, 4);
    case 4:
        F_CLR(Ply[fd].ply,PREADI); 

        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);

        switch(low(str[0])) {
              case '1':
                   obj_ptr->shotsmax = 100;
                   obj_ptr->shotscur = 100;
                   sum += 1000000;
                   break;
              case '2':
                   obj_ptr->shotsmax = 200;
                   obj_ptr->shotscur = 200;
                   sum += 2000000;
                   break;

              case '3':
                   obj_ptr->shotsmax = 300;
                   obj_ptr->shotscur = 300;
                   sum += 3000000;
                   break;

              case '4':
                   obj_ptr->shotsmax = 400;
                   obj_ptr->shotscur = 400;
                   sum += 4000000;
                   break;

              case '5':
                   obj_ptr->shotsmax = 500;
                   obj_ptr->shotscur = 500;
                   sum += 5000000;
                   break;

              default: print(fd, "�ϳ��� �����Ͻÿ�: ");
                   F_SET(Ply[fd].ply,PREADI);  

                   RETURN(fd, select_newarm, 4);
        }

        sprintf(forge2[fd],"%lu",sum);

        print(fd, "\n����� ������ �̸��� �����ʽÿ�.\n");
        print(fd, "���߿� �̸��� ��ĥ�� ������ �����ؼ� �����ž� �մϴ�\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);   

        RETURN(fd, select_newarm, 5);
    case 5:
        F_CLR(Ply[fd].ply,PREADI);   

        obj_ptr=(object *)atol(forge1[fd]);

        if(strchr(str,'(') || strchr(str,')')) {
            print(fd,"�̸����� ��ȣ�� ���� �����ϴ�.\n�̸��� �ٽ� �����ʽÿ�:");
            RETURN(fd,select_newarm,5);
        }
        if(strlen(str) > 20) {
                        print(fd, "�Էµ� �̸��� �ʹ�  ��ϴ�.\n�̸��� �ٽ� �����ʽÿ�(3���̻� 20������): ");
                        RETURN(fd, select_newarm, 5);
        }
        if(strlen(str) < 3) {
                        print(fd, "�Էµ� �̸��� �ʹ�  ª���ϴ�.\n�̸��� �ٽ� �����ʽÿ�(3���̻� 20������): ");
                        F_SET(Ply[fd].ply,PREADI);          

                        RETURN(fd, select_newarm, 5);
        }

        strncpy(obj_ptr->name, str, 20);

        print(fd, "\n���Ϳ� �����Ͻʴϱ�? (��/�ƴϿ�)\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);    

        RETURN(fd, select_newarm, 6);

    case 6:
        F_CLR(Ply[fd].ply,PREADI);         

        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);

        if(!strncmp(str,"��",2)) {
                   if (ply_ptr->gold < sum) {
                        print(fd, "\n����� �� ������ ���⸦  ���� ���� ���� �����ϴ�.\n������ ��ſ��� \"�ܻ��� �ȵǿ�~\"��� ���մϴ�.");
                        free(obj_ptr);
                        break;
                   }
                   add_obj_crt(obj_ptr,ply_ptr);

                   print(fd, "\n������ ��ſ��� ���� ���۵� ���⸦ �ǳ��ݴϴ�.");
                   broadcast_rom(fd,   ply_ptr->rom_num,  "\n%M��   ���⸦  �����Ͽ����ϴ�.",ply_ptr);

                   ply_ptr->gold -= sum;
        }
        else {
            print(fd,"���� ������ ����Ͽ����ϴ�.");
            free(obj_ptr);
        }
        break;
    }

    RETURN(fd, command, 1);
}

/*************************************************************************************
***************/



void change_class_ok();
int change_class(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        int fd,new_class=0,i;
        room    *rom_ptr;
        fd=ply_ptr->fd;
        rom_ptr=ply_ptr->parent_rom;
        if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "����� ���� �־� ������ȯ�� �� �� �����ϴ�!");
                ANSI(fd, WHITE);
                return(0);
        }
        if(!F_ISSET(rom_ptr, RTRAIN)) {
                print(fd, "�� ���� �������� �ƴմϴ�!");
                return(0);
        }
        for(i=0; i<3; i++) {
                new_class*=2;
                new_class|=F_ISSET(rom_ptr,RTRAIN+i+1)?1:0;
        }
        new_class++;
        if(ply_ptr->class>8) {
            print(fd,"����� ������ȯ�� �� �� ���� ������ ���� �ֽ��ϴ�.");
            return(0);
        }
        if(new_class==ply_ptr->class) {
                print(fd, "������ȯ�� �Ϸ��� �ڽ��� �����ϴ°������� �� �� �����ϴ�.");
                return(0);
        }
        if(ply_ptr->experience<100000) {
                print(fd, "������ȯ�� �Ϸ��� ����ġ 10���� �ʿ��մϴ�.");
                return(0);
        }
        change_class_ok(ply_ptr->fd,1,"");
        return (DOPROMPT);
}
int chg_class_main(ply_ptr)
creature *ply_ptr;
{
        room    *rom_ptr;
        long    goldneeded, expneeded, lv;
        int     fd, i;
        int n,new_class=0;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        for(i=0; i<3; i++) {
                new_class*=2;
                new_class|=F_ISSET(rom_ptr,RTRAIN+i+1)?1:0;
        }
        new_class++;
        ply_ptr->experience -= 100000;
        ply_ptr->class=new_class;
        n = exp_to_lev(ply_ptr->experience);
        while(ply_ptr->level > n)
            down_level(ply_ptr);
        print(fd, "\n����� ������ ��ȯ�Ǿ����ϴ�.");
                if(F_ISSET(ply_ptr, PFAMIL)) {
                        edit_member(ply_ptr->name,                          ply_ptr->class, 
ply_ptr->daily[DL_EXPND].max, 3);
                }
        return(0);
}
void change_class_ok(fd,param,str)
int fd;
int param;
char *str;
{
    switch(param) {
        case 1:
            print(fd,"������ȯ�� �Ϸ��� ����ġ 10���� �ʿ��մϴ�.\n");
            print(fd,"������ ������ȯ�� �Ͻðڽ��ϱ�?(��/�ƴϿ�): ");
            F_SET(Ply[fd].ply,PREADI);
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            RETURN(fd,change_class_ok,2);
        case 2:
            F_CLR(Ply[fd].ply,PREADI);
            if(!strncmp(str,"��",2)) {
                chg_class_main(Ply[fd].ply);
            }
            else
               print(fd,"������ȯ�� ���� �ʾҽ��ϴ�");
            RETURN(fd,command,1);
    }
}
/**********************************************************************/
/*                       magic_stop ( �������� )                     */
/**********************************************************************/
/* �̱���� ���� ������ ���� ��ƾ�̴�.                              */
/*                                                                    */
int magic_stop(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              chance, m, dur, dmg, fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(cmnd->num < 2) {
                print(fd,
                        "\n������ ������ �����ϽǷ�����?\n");
                return(0);
        }
        if(ply_ptr->class != RANGER && ply_ptr->class < INVINCIBLE) {
                print(fd,
                        "\n�������� �������⸦ ����Ҽ� �ֽ��ϴ�.\n");
                return(0);
        }
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
        if(!crt_ptr) {
                print(fd,
                        "\n�׷� ������ �������� �ʽ��ϴ�.\n");
                return(0);
        }
        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd,
                        "\n����� ����� ��Ÿ���� �����մϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num,
                                "%M�� ����� ���̱� �����մϴ�.",
                                     ply_ptr);
        }
        i = LT(ply_ptr, LT_TURNS);
        t = time(0);
        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
                print(fd,
                        "\n����� %s�� ������ �����Ҽ� �����ϴ�.\n",
                        F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }
        add_enm_crt(ply_ptr->name, crt_ptr);
        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 20L;
        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->dexterity]*6;
        chance = MIN(chance, 80);
		if(ply_ptr->class == RANGER) chance += 10;
        if(mrand(1,100) > chance) {
                print(fd,
                        "\n����� ���� ������ �绡�� �����߽��ϴ�.\n�׷��� ���� ��¦ ���� ���������ϴ�.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M�� ���� ������ �绡�� �����߽��ϴ�.\n�׷��� %M�� ��¦ ���߽��ϴ�.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }
                print(fd,
                        "\n����� ���� ������ �绡�� �����߽��ϴ�.\n���� ������ ¤�� �ֹ��� �����߽��ϴ�.\n ",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M�� ���� ������ �绡�� �����߽��ϴ�.\n%M�� ������ ¤�� �ֹ��� ����Ǿ����ϴ�.\n",
                                    ply_ptr, crt_ptr);
                if (mrand(25,100) <= chance) {
                        dmg = crt_ptr->hpcur /2;
                        print(fd,"%M�� �޼Ҹ� ¤� %d�� ���ظ� �������ϴ�.\n",  crt_ptr, dmg );
                        broadcast_rom(fd, ply_ptr->rom_num,
                                 "%M�� %m�� �޼Ҹ� ¤� %d�� ���ظ� �������ϴ�.\n", ply_ptr, crt_ptr, dmg);
                        crt_ptr->hpcur -= dmg;
                        m = MIN(crt_ptr->hpcur, dmg);
                        add_enm_dmg(ply_ptr->name, crt_ptr, m);
                        if(crt_ptr->hpcur < 1) {
                              print(fd, "\n����� %M%j �׿����ϴ�.", crt_ptr,"3");
                              broadcast_rom(fd, ply_ptr->rom_num,
                                         "\n%M%j    %M%j    �׿����ϴ�.",    ply_ptr,"1", crt_ptr,"3");
                              die(crt_ptr, ply_ptr);
                         }
                         else
                              check_for_flee(ply_ptr, crt_ptr);
                }
 
                dur = bonus[ply_ptr->dexterity]*2 + dice(2,6,0);

                if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
                   (crt_ptr->type   !=   PLAYER  &&    (F_ISSET(crt_ptr,   MRMAGI)   || F_ISSET(crt_ptr, MRBEFD))))
                        dur = 5;
                else
                        dur = MAX(15, dur);

                crt_ptr->lasttime[LT_SPELL].ltime = time(0);
                dur = MIN(20, dur);
                crt_ptr->lasttime[LT_SPELL].interval = dur;

        return(0);
}
/**********************************************************************/
/*                       �ߵ�                                         */
/**********************************************************************/
int poison_mon(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              dmg, dur, chance, m, fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(cmnd->num < 2) {
                print(fd,
                        "\n������ �ߵ���Ű�÷����?\n");
                return(0);
        }
        if(ply_ptr->class != ASSASSIN && ply_ptr->class < INVINCIBLE) {
                print(fd,
                        "\n�ڰ����� �����մϴ�.\n");
                return(0);
        }
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
        if(!crt_ptr) {

                print(fd,
                        "\n�׷� ������ �������� �ʽ��ϴ�.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd,
                        "\n����� ����� ��Ÿ���� �����մϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num,
                                "%M�� ����� ���̱� �����մϴ�.",
                                     ply_ptr);
        }
        i = LT(ply_ptr, LT_TURNS);
        t = time(0);
        if(i > t) {
                please_wait(fd, i-t);
                return(0);
        }
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
                print(fd,
                        "\n����� %s�� �ߵ���ų�� �����ϴ�.\n",
                        F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
                return(0);
        }
        add_enm_crt(ply_ptr->name, crt_ptr);
        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 15L; 
        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->dexterity]*6;
        chance = MIN(chance, 80);
        if(mrand(1,100) > chance) {
                print(fd,
                        "\n����� ������ ���� �ѷȽ��ϴ�.\n�׷��� ���� ��¦ ���� �����߽��ϴ�.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M�� ������ ���� �ѷȽ��ϴ�.\n�׷���  %M�� ��¦ ���߽��ϴ�.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }
        print(fd,
                "\n����� ������ ���� �ѷȽ��ϴ�.\n%M�� ���� �ߵ��Ǿ����ϴ�.\n ",
                        crt_ptr);
        broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M�� ������ ���� �ѷȽ��ϴ�.\n%M�� ���� �ߵ��Ǿ����ϴ�.\n",
                ply_ptr, crt_ptr);
        F_SET(crt_ptr, MPOISN);
                if (mrand(10,100) <= chance) {
                        dmg = crt_ptr->hpmax /3 ;
                        print(fd,"%M�� �ߵ���  ���Ѽ� %d��  ���ظ� �������ϴ�.\n",  crt_ptr, dmg );
                        broadcast_rom(fd, ply_ptr->rom_num,
                                 "%M�� %m�� �ߵ���  ���Ѽ� %d��  ���ظ� �������ϴ�.\n", ply_ptr, crt_ptr, dmg);
                        crt_ptr->hpcur -= dmg;
                        m = MIN(crt_ptr->hpcur, dmg);
                        add_enm_dmg(ply_ptr->name, crt_ptr, m);
                        if(crt_ptr->hpcur < 1) {
                              print(fd, "\n����� %M%j �׿����ϴ�.", crt_ptr,"3");
                              broadcast_rom(fd, ply_ptr->rom_num,
                                         "\n%M%j    %M%j    �׿����ϴ�.",    ply_ptr,"1", crt_ptr,"3");
                              die(crt_ptr, ply_ptr);
                         }
                         else
                              check_for_flee(ply_ptr, crt_ptr);
                }
 
                dur = bonus[ply_ptr->dexterity]*2 + dice(2,6,0);

                if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
                   (crt_ptr->type   !=   PLAYER  &&    (F_ISSET(crt_ptr,   MRMAGI)   || F_ISSET(crt_ptr, MRBEFD))))
                        dur = 3;
                else
                        dur = MAX(5, dur);

                crt_ptr->lasttime[LT_SPELL].ltime = time(0);
                dur = MIN(10, dur);
                crt_ptr->lasttime[LT_SPELL].interval = dur;
        
        return(0);
}


