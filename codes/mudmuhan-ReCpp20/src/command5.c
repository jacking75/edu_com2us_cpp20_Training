/*
 * COMMAND5.C:
 *
 *  Additional user routines.
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

extern long all_broad_time;
extern long login_time[PMAX];
extern char title_cut_index[PMAX];

/**********************************************************************/
/*              attack                                                */
/**********************************************************************/

/* This function allows the player pointed to by the first parameter */
/* to attack a monster.                                              */

int attack(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    long        i, t;
    int     fd;

    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK);

    if(ply_ptr->class==0) {
        print(fd,"����� ������ ������ ������ ���� �ֽ��ϴ�.");
        return 0;
    }
    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
        print(fd, "������ �����Ͻ÷�����?");
        return(0);
    }

    rom_ptr = ply_ptr->parent_rom;

    if(ply_is_attacking(ply_ptr,cmnd)) {
        print(fd, "����� ���� �ο�� ���ݾƿ�!");
        return(0);
    }

    crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
               cmnd->str[1], cmnd->val[1]);

    if(!crt_ptr) {
        cmnd->str[1][0] = up(cmnd->str[1][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 2) {
            print(fd, "�׷��� ���� �����ϴ�.");
            return(0);
        }

    }

    if(F_ISSET(ply_ptr,PDMINV)) F_CLR(ply_ptr,PDMINV);

    if(t-login_time[fd]<120) login_time[fd]-=120;
    attack_crt(ply_ptr, crt_ptr);

    return(0);

}

/**********************************************************************/
/*              attack_crt                */
/**********************************************************************/

/* This function does the actual attacking.  The first parameter contains */
/* a pointer to the attacker and the second contains a pointer to the     */
/* victim.  A 1 is returned if the attack restults in death.          */

int attack_crt(ply_ptr, crt_ptr)
creature    *ply_ptr;
creature    *crt_ptr;
{
    long    i, t;
    int fd, m, n, p, lev, addprof;
    int j, count=1;
    float st=0;
    int tes=0;

    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK); 


    if(t < i)
        return(0); 
        if (crt_ptr->type != MONSTER) {
         if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)) {
                print(fd, "����� %S%j �ʹ� �����ؼ� �׷��� �����ϴ�.", crt_ptr->name,"3");
                return(0);
         }
        else
                del_charm_crt(ply_ptr->name, crt_ptr);

    }
    F_CLR(ply_ptr, PHIDDN);
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "\n����� ����� ������ �巯�����ϴ�.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ������ �巯�����ϴ�.",
                  ply_ptr);
    }

    ply_ptr->lasttime[LT_ATTCK].ltime = t;
    if(F_ISSET(ply_ptr, PHASTE))
        ply_ptr->lasttime[LT_ATTCK].interval = 1;
    else
        ply_ptr->lasttime[LT_ATTCK].interval = 1;
        if(F_ISSET(ply_ptr, PBLIND)){
                ply_ptr->lasttime[LT_ATTCK].interval = 6;
    }
    if(crt_ptr->type == MONSTER) {
        if(F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "����� %s�� ��ĥ �� �����ϴ�.\n",
                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            return(0);
        }

        if(add_enm_crt(ply_ptr->name, crt_ptr) < 0) {
            print(fd, "����� %M%j �����մϴ�.", crt_ptr,"3");
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %M%j �����մϴ�.",
                      ply_ptr, "1",crt_ptr,"3");
        }


        if(F_ISSET(crt_ptr, MMGONL)) {
            print(fd, "\n����� ����� %M���� �ƹ� �ҿ��� ���µ� �մϴ�.",
                crt_ptr);
            return(0);
        }
        if(F_ISSET(crt_ptr, MENONL) && ply_ptr->class < CARETAKER) {
            if(!ply_ptr->ready[WIELD-1] ||
                ply_ptr->ready[WIELD-1]->adjustment < 1) {
                print(fd, "\n����� ����� %M���� �ƹ� �ҿ��� ���µ� �մϴ�.",
                    crt_ptr);
                return(0);
            }
        }
    }
    else {
        if(F_ISSET(ply_ptr->parent_rom, RNOKIL)) {
            print(fd, "�� �������� �ο� �� �����ϴ�.");
            return(0);
        }

		if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->level < 128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "����� ���ϴٴ°� �Ƽ���.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->level <128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ������ �� �����ϴ�.");
                    return (0);
                }
		}
		else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && ply_ptr->level < 128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "����� ���ϴٴ°� �Ƽ���.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && ply_ptr->level <128 && !F_ISSET(ply_ptr->parent_rom, RSUVIV)) {
                    print(fd, "�� ����ڴ� ���ؼ� ������ �� �����ϴ�.");
                    return (0);
                }
		}

        ply_ptr->lasttime[LT_ATTCK].interval += 3;
        print(crt_ptr->fd, "\n%M%j ����� �����մϴ�!", ply_ptr,"1");
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                   "\n%M%j %M%j �����մϴ�!", ply_ptr, "1",crt_ptr,"3");
    }

 count = 1;
 if(F_ISSET(ply_ptr, PUPDMG)) {
	if((ply_ptr->class == INVINCIBLE && ply_ptr->level > 100) ||
		(ply_ptr->class > INVINCIBLE)) {
		if((ply_ptr->level-97)/10 + mrand(0,3) > 2)
			count++;
	}
	if(ply_ptr->class > INVINCIBLE && mrand(1, 4) == 1)
			count++;
 }
 for(j=0;j<count;j++) {

    if(ply_ptr->ready[WIELD-1]) {
        if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
            print(fd, "\n%S%j �μ��� ���Ƚ��ϴ�.",
                  ply_ptr->ready[WIELD-1]->name,"1");
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� %S%j �μ��� ���Ƚ��ϴ�.",
                ply_ptr, ply_ptr->ready[WIELD-1]->name,"1");
            add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
            ply_ptr->ready[WIELD-1] = 0;
            return(0);
        }
    }

    n = ply_ptr->thaco - crt_ptr->armor/8;

    if (F_ISSET(ply_ptr, PFEARS)) n += 2;
    if (F_ISSET(ply_ptr, PBLIND)) n += 5;

    if(mrand(1,30) >= n) { /* ������ 20 */
        if(ply_ptr->ready[WIELD-1]) {
            n = mdice(ply_ptr->ready[WIELD-1]) +
                bonus[(int)ply_ptr->strength] +
                profic(ply_ptr, ply_ptr->ready[WIELD-1]->type)/10;
			if(ply_ptr->ready[HELD-1] && ply_ptr->ready[HELD-1]->type < ARMOR)
				n += mdice(ply_ptr->ready[HELD-1])/10;
        }
        else if(ply_ptr->class == BARBARIAN || ply_ptr->class >INVINCIBLE) {
            n = mdice(ply_ptr) +
                bonus[(int)ply_ptr->strength] +
                ((ply_ptr->level+3)/4);
        }
        else {
            n = mdice(ply_ptr) +
                bonus[(int)ply_ptr->strength];
        }

        if(ply_ptr->class == MAGE || ply_ptr->class == CLERIC) {
             if(ply_ptr->ready[WIELD-1]) {
                n = mdice(ply_ptr->ready[WIELD-1]) +
                    bonus[(int)ply_ptr->strength];
             }
             else {
                n = mdice(ply_ptr) + bonus[(int)ply_ptr->strength]; 
             }
        }

        if(crt_ptr->class >= DM) n = 0;

        n = MAX(1,n);

        if(ply_ptr->class == PALADIN) {
            if(ply_ptr->alignment < 0) {
                n /= 2;
                print(fd, "\n����� ������ ����� �������ϴ�.");
            }
            else if(ply_ptr->alignment > 250) {
                n += mrand(1,3);
                print(fd, "\n����� ������ �ɷ��� �谡��ŵ�ϴ�.");
            }
        }

        p = mod_profic(ply_ptr);
        if(mrand(1,100) <= p ||
          (ply_ptr->ready[WIELD-1] &&
          F_ISSET(ply_ptr->ready[WIELD-1],OALCRT))) {
            ANSI(fd, GREEN);
            print(fd, "\n����� %M���� ġ��Ÿ�� ���Ƚ��ϴ�.",crt_ptr);
            ANSI(fd, WHITE);
            broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M�� ġ��Ÿ�� ���Ƚ��ϴ�.", ply_ptr);
            n *= mrand(3,6);
            if(ply_ptr->ready[WIELD-1] && (!F_ISSET(ply_ptr->ready[WIELD-1], ONSHAT)) && ((mrand(1,100) < 3) || (F_ISSET(ply_ptr->ready[WIELD-1],OALCRT)))) {
            if(!F_ISSET(ply_ptr->ready[WIELD-1], ONEWEV)) {
                print(fd, "\n%S%j ����� �μ����ϴ�.",
                    ply_ptr->ready[WIELD-1]->name,"1");
                broadcast_rom(fd, ply_ptr->rom_num,
                    "\n%s�� %S%j ����� �μ����ϴ�.",
                    F_ISSET(ply_ptr, PMALES) ? "��":"�׳�",
                    ply_ptr->ready[WIELD-1]->name,"1");
                free_obj(ply_ptr->ready[WIELD-1]);
                ply_ptr->ready[WIELD-1] = 0;
            }
            }
        }
        else if(mrand(1,100) <= (5-p) && ply_ptr->ready[WIELD-1] &&
            !F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) {
            ANSI(fd, GREEN);
            print(fd, "\n����� ���⸦ ����߷Ƚ��ϴ�.");
            ANSI(fd, WHITE);
            broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M�� ���⸦ ����߷Ƚ��ϴ�.", ply_ptr);
            n = 0;
            add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
            ply_ptr->ready[WIELD-1] = 0;
            compute_thaco(ply_ptr);
        }

        print(fd, "\n����� %M���� %d ��ŭ�� ���ظ� �־����ϴ�.", crt_ptr, n);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %M%j %d��ŭ�� ���ظ� �����ϴ�.",ply_ptr, "1",crt_ptr,"3",n); 
        print(crt_ptr->fd, "\n����� %M���Լ� %d ��ŭ�� ���ظ� �Ծ����ϴ�.",
              ply_ptr, n);

        if(ply_ptr->ready[WIELD-1] && !mrand(0,3))
            ply_ptr->ready[WIELD-1]->shotscur--;

        m = MIN(crt_ptr->hpcur, n);
        crt_ptr->hpcur -= n;

        print(fd, "\n");
        display_status(fd, crt_ptr);
        print(fd, "\n");

        if(crt_ptr->type != PLAYER) {
            add_enm_dmg(ply_ptr->name, crt_ptr, m);
            if(ply_ptr->ready[WIELD-1]) {
                p = MIN(ply_ptr->ready[WIELD-1]->type, 4);
                addprof = (m * crt_ptr->experience) /
                    MAX(crt_ptr->hpmax, 1);
                addprof = MIN(addprof, crt_ptr->experience);
                ply_ptr->proficiency[p] += addprof;
            }
        }

        if(crt_ptr->hpcur < 1) {
            print(fd, "\n����� %M%j �׿����ϴ�.", crt_ptr,"3");
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                      "\n%M%j %M%j �׿����ϴ�.", ply_ptr,"1", crt_ptr,"3");
            die(crt_ptr, ply_ptr);
            return(1);
        }
        else
            check_for_flee(ply_ptr, crt_ptr);
 }
    else {
        print(fd, "\n����� �ƽ��ϴ�.");
        print(crt_ptr->fd, "\n%M%j ����� �ƽ��ϴ�.", ply_ptr,"1");
    }
  }
    return(0);
}

/**********************************************************************/
/*              who                   */
/**********************************************************************/

/* This function outputs a list of all the players who are currently */
/* logged into the game.  It includes their titles and the last      */
/* command they typed.                           */

int who(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    char    str[15];
    int fd, i, j;
    int total=0;
    int sum = 0;

    fd = ply_ptr->fd;

         if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "����� ���� �־� �ֽ��ϴ�!\n");
                ANSI(fd, WHITE);
                return(0);
        }

   for(i=0; i<Tablesize; i++) { 
   	if(!Ply[i].ply) continue;
   	sum ++;
   }

  if((sum > 30) && !((cmnd->num == 2) && !strcmp(cmnd->str[1], "l"))) { 
   print(fd, "%-14s    [���� ����]", "�����");
   print(fd, "  %-14s    [���� ����]\n", "�����");
   print(fd, "-------------------------------");
   print(fd, "-----------------------------\n");
   for(i=0; i<Tablesize; i++) {
      if(!Ply[i].ply) continue;
      if(Ply[i].ply->fd == -1) continue;
      if(F_ISSET(Ply[i].ply, PDMINV) && Ply[i].ply->class == DM && ply_ptr->class < SUB_DM)
      		continue;
      if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < SUB_DM)
      		continue;
	if(F_ISSET(Ply[i].ply, PINVIS) && !F_ISSET(ply_ptr, PDINVI) &&
	  ply_ptr->class < SUB_DM)
	  if(Ply[i].ply!=ply_ptr) continue;
	total++;
	print(fd, "%-14s%s ", Ply[i].ply->name, (F_ISSET(Ply[i].ply, PDMINV) ||
		F_ISSET(Ply[i].ply, PINVIS)) ? "(*)" : "   ");
	print(fd, "[%s%02d ", (Ply[i].ply->level >= 100) ? "" : " ",
		Ply[i].ply->level);
	print(fd, " %-4.4s]  ", class_str[(int)Ply[i].ply->class]);
	if((total%2 == 0) || i == (Tablesize-1))
		print(fd, "\n");
   }
  if(total%2 != 0) print(fd, "\n"); 
}
   else {
#ifdef LASTCOMMAND
    PRINt(fd, "%-23s  %-20s     %-20s\n", "�����", "Title", "������ ���");
#else
    ANSI (fd, BOLD);
    ANSI (fd, BLUE);
    print(fd, "%-13s     ����  %-4s %-8s %-6s %-32s\n", "�����", "����","����","�аŸ�","Īȣ");
#endif
    print(fd, "----------------------------------------------------------------------\n");
    ANSI(fd, NORMAL);
    ANSI(fd, WHITE);
    for(i=0; i<Tablesize; i++) {
        if(!Ply[i].ply) continue;
        if(Ply[i].ply->fd == -1) continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && Ply[i].ply->class == DM &&
           ply_ptr->class < SUB_DM)
            continue;
        if(F_ISSET(Ply[i].ply, PDMINV) && ply_ptr->class < SUB_DM)
            continue;
        if(F_ISSET(Ply[i].ply, PINVIS) && !F_ISSET(ply_ptr, PDINVI) &&
           ply_ptr->class < SUB_DM)
            if(Ply[i].ply!=ply_ptr) continue; /* ����Ǿ �ڽ��� ��������... */
        total++;
        ANSI(fd, WHITE);
        print(fd, "%-13s%s ", Ply[i].ply->name,
              (F_ISSET(Ply[i].ply, PDMINV) ||
              F_ISSET(Ply[i].ply, PINVIS)) ? "(*)":"   ");
         ANSI(fd, GREEN);
        print(fd, "[%s%02d ] ", (Ply[i].ply->level>=100)? "" : " ",
                                Ply[i].ply->level);
 /* �һ��̻� ������ */
/*        if(Ply[i].ply->class>=CARETAKER) {
            ANSI(fd,BOLD);
            ANSI(fd,RED);
        }
        else ANSI(fd,WHITE);  */
        ANSI(fd, WHITE);
        print(fd, "%-4.4s ",class_str[(int)Ply[i].ply->class]);
        ANSI(fd, WHITE);
#ifdef LASTCOMMAND
        strncpy(str, Ply[i].extr->lastcommand, 14);
        for(j=0; j<15; j++)
            if(str[j] == ' ') {
                str[j] = 0;
                break;
            }
        if(!str[0])
            print(fd, "Logged in\n");
        else
            print(fd, "%s\n", str);
#else
        ANSI(fd, CYAN);
        print(fd, "%-8s ", race_str[(int)Ply[i].ply->race]);
		ANSI(fd, WHITE);
		if(F_ISSET(Ply[i].ply, PFAMIL)) { 
			ANSI(fd, YELLOW);
			print(fd, "[%-4.4s] ", family_str[Ply[i].ply->daily[DL_EXPND].max]);
		}
		else {
			ANSI(fd, BLUE);
			print(fd, "[�߸�] ");
		}
		ANSI(fd, WHITE);
         title_cut_index[fd]=1;
         print(fd, "%s\n", title_ply(ply_ptr,Ply[i].ply));
         title_cut_index[fd]=0;

#endif
    }
 }
    if(total!=1) print(fd, "\n�� %d���� ����ڰ� ���Ѵ����� �̿��ϰ� �ֽ��ϴ�.",total);
    else         print(fd, "\n��� ȥ�ڼ� �ܷ��� ���Ѵ����� �̿��ϰ� �ֽ��ϴ�.");

	if(AT_WAR)
		print(fd, "\n%s �аŸ��� %s �аŸ��� �������Դϴ�.",
			family_str[AT_WAR/16], family_str[AT_WAR%16]);

    return(0);

}
 
/**********************************************************************/
/*                              whois                                 */
/**********************************************************************/
/* The whois function displays a selected player's name, class, level *
 * title, age and gender */
      
int whois(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
 
    if(cmnd->num < 2) {
        print(fd, "������ �˻��Ͻ÷�����?");
        return(0);
    }
 
    lowercize(cmnd->str[1], 1);
    crt_ptr = find_who(cmnd->str[1]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) || F_ISSET(ply_ptr, PBLIND) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "���� �̿����� �ƴմϴ�.");
        return(0);
    }
 
        ANSI(fd, YELLOW); 
        print(fd, "%-18s  %-4s [����] %-4s %-20s  %-4s  %-10s\n", "�����", "����", "����", "Īȣ", "����", "����");
        print(fd, "----------------------------------------------------------------------------\n");
        print(fd, "%-18s  %-4s [ %02d ] %-4.4s %-20s  %-4d  ",
                crt_ptr->name,
                F_ISSET(crt_ptr, PMALES) ? " ��":" ��",  
                crt_ptr->level,
                class_str[(int)crt_ptr->class],
                title_ply(ply_ptr,crt_ptr), 
                18 + crt_ptr->lasttime[LT_HOURS].interval/86400L);
print(fd, "%-10s", race_str[(int)crt_ptr->race]);
        ANSI(fd, NORMAL);
        ANSI(fd, WHITE);
    return(0);
}

/**********************************************************************/
/*              search                    */
/**********************************************************************/

/* This function allows a player to search a room for hidden objects,  */
/* exits, monsters and players.                        */

int search(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    xtag    *xp;
    otag    *op;
    ctag    *cp;
    long    i, t;
    int fd, chance, found = 0;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    chance = 15 + 5*bonus[(int)ply_ptr->piety] + ((ply_ptr->level+3)/4)*2;
    chance = MIN(chance, 90);
    if(ply_ptr->class == RANGER)
		chance = 100;
/*
        chance += ((ply_ptr->level+3)/4)*8;
*/
    if(F_ISSET(ply_ptr, PBLIND))
        chance = MIN(chance, 20);
    if(ply_ptr->class >= CARETAKER)
        chance = 100;

    t = time(0);
    i = LT(ply_ptr, LT_SERCH);

    if(t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    F_CLR(ply_ptr, PHIDDN);

    ply_ptr->lasttime[LT_SERCH].ltime = t;
    if(ply_ptr->class == RANGER)
        ply_ptr->lasttime[LT_SERCH].interval = 3;
    else
        ply_ptr->lasttime[LT_SERCH].interval = 7;

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

    broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �ֺ��� ������ �������ϴ�.", ply_ptr);

    if(found)
        broadcast_rom(fd, ply_ptr->rom_num, "\n%s�� �� �߰��Ѱ� ������!",
                  F_ISSET(ply_ptr, MMALES) ? "��":"�׳�");
    else
        print(fd, "����� �ƹ��͵� ã�� ���߽��ϴ�.\n");

    return(0);

}

/**********************************************************************/
/*              ply_suicide               */
/**********************************************************************/

/* This function is called whenever the player explicitly asks to     */
/* commit suicide.  It then calls the suicide() function which takes  */
/* over that player's input.                          */

int ply_suicide(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    suicide(ply_ptr->fd, 1, "");
    return(DOPROMPT);
}

/**********************************************************************/
/*              suicide                   */
/**********************************************************************/

/* This function allows a player to kill himself, thus erasing his entire */
/* player file.                               */

void suicide(fd, param, str)
int fd;
int param;
char    *str;
{
    char    file[120],file2[120], file3[120], temp[120],temp2[120], temp3[120];
    creature *ply_ptr;
    long t;
    char buf[256];

    ply_ptr = Ply[fd].ply;

    switch(param) {
        case 1:
            ANSI(fd, BOLD);
            ANSI(fd, RED);
            print(fd, "��ſ� ���� �����͸� ������ �����մϴ�.\n");
            print(fd, "����� ���� ��ȣ�� �־��ֽʽÿ� : "); 
            ANSI(fd, NORMAL);
            ANSI(fd, WHITE);
            F_SET(Ply[fd].ply,PREADI);
            RETURN(fd, suicide, 2);
        case 2:
            F_CLR(Ply[fd].ply,PREADI);
            if(!strcmp(ply_ptr->password, str)) {
            	print(fd, "��¥��? (��¥��/������)");
            	F_SET(Ply[fd].ply,PREADI);
            	RETURN(fd, suicide, 3);
            }
            else {
            	print(fd, "��ȣ�� Ʋ���ϴ�.\n�������� �ʾҽ��ϴ�.");
            	RETURN(fd, command, 1);
            }
        case 3:
            F_CLR(Ply[fd].ply,PREADI);
            if(!strcmp(str,"��¥��")) {
			if(F_ISSET(ply_ptr, PFAMIL)) {
		edit_member(ply_ptr->name, ply_ptr->class, ply_ptr->daily[DL_EXPND].max, 2);
		}
                broadcast_all("\n### %s���� �ڻ��û�� �Ͽ����ϴ�.\n", Ply[fd].ply->name);
                all_broad_time=time(0);
                sprintf(file, "%s/%s/%s", PLAYERPATH,first_han(Ply[fd].ply->name), Ply[fd].ply->name);
                 sprintf(file2,"%s/alias/%s",PLAYERPATH,Ply[fd].ply->name);
                 sprintf(file3,"%s/bank/%s",PLAYERPATH,Ply[fd].ply->name);
			sprintf(temp, "mv %s %s/suic/%s", file, PLAYERPATH, Ply[fd].ply->name);
			sprintf(temp2, "mv %s %s/suic/%s.alias", file2, PLAYERPATH, Ply[fd].ply->name);
            sprintf(temp3, "mv %s %s/suic/%s.bank", file3, PLAYERPATH, Ply[fd].ply->name);
#ifdef SUICIDE
                t = time(0);
                strcpy(buf,ctime(&t));
                buf[strlen(buf)-1] = 0;
                logn("SUICIDE","%s : %s (%s)���� �ڻ��û�� �Ͽ����ϴ�.\n", buf,Ply[fd].ply->name, Ply[fd].io->address);
#endif
                disconnect(fd);
			system(temp);
			system(temp2);
            system(temp3);         
                return;
            }
            else {
                print(fd, "�������� �ʾҽ��ϴ�.");
                RETURN(fd, command, 1);
            }
    }
}

/**********************************************************************/
/*              hide                      */
/**********************************************************************/

/* This command allows a player to try and hide himself in the shadows */
/* or it can be used to hide an object in a room.              */

int hide(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    room    *rom_ptr;
    object  *obj_ptr;
    long    i, t;
    int fd, chance;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    i = LT(ply_ptr, LT_HIDES);
    t = time(0);

    if(i > t) {
        please_wait(fd, i-t);
        return(0);
    }

    ply_ptr->lasttime[LT_HIDES].ltime = t;
    ply_ptr->lasttime[LT_HIDES].interval = (ply_ptr->class == THIEF ||
        ply_ptr->class == ASSASSIN || ply_ptr->class == RANGER) ? 5:15;

    if(cmnd->num == 1) {

        if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN || ply_ptr->class >= CARETAKER ) 
            chance = MIN(90, 5 + 6*((ply_ptr->level+3)/4) +
                3*bonus[(int)ply_ptr->dexterity]);
        else if(ply_ptr->class == RANGER)
            chance = 5 + 10*((ply_ptr->level+3)/4) +
                3*bonus[(int)ply_ptr->dexterity];
        else
            chance = MIN(90, 5 + 2*((ply_ptr->level+3)/4) +
                3*bonus[(int)ply_ptr->dexterity]);

        print(fd, "����� �ֽ� ������� �մϴ�.");

        if(F_ISSET(ply_ptr, PBLIND))
            chance = MIN(chance, 20);

        if(mrand(1,100) <= chance) {
            F_SET(ply_ptr, PHIDDN);
            print(fd, "\n����� ���������� �������ϴ�.");
            broadcast_rom(fd, ply_ptr->rom_num,
                   "\n%M%j �׸��� ���̷� �������ϴ�.", ply_ptr,"1");
            }
        else {
            F_CLR(ply_ptr, PHIDDN);
            broadcast_rom(fd, ply_ptr->rom_num,
                  "\n%M%j �ֽ� ������� �մϴ�.", ply_ptr,"1");
        }


        return(0);

    }

    obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
               cmnd->str[1], cmnd->val[1]);

    if(!obj_ptr) {
        print(fd, "�׷����� ���� �����.");
        return(0);
    }

    if(F_ISSET(obj_ptr,ONOTAK)){
        print(fd,"����� �װ��� ���� �� �����ϴ�.");
        return (0);
   }
    if(ply_ptr->class == THIEF || ply_ptr->class == ASSASSIN)
        chance = MIN(90, 10 + 5*((ply_ptr->level+3)/4) +
            5*bonus[(int)ply_ptr->dexterity]);
    else if(ply_ptr->class == RANGER)
        chance = 5 + 9*((ply_ptr->level+3)/4) +
            3*bonus[(int)ply_ptr->dexterity];
    else
        chance = MIN(90, 5 + 3*((ply_ptr->level+3)/4) +
            3*bonus[(int)ply_ptr->dexterity]);

    print(fd, "����� �װ��� ���ܺ����� �մϴ�.");
    broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %1i%j ���ܺ����� �մϴ�.",
              ply_ptr, "1",obj_ptr,"3");

    if(mrand(1,100) <= chance) {
        F_SET(obj_ptr, OHIDDN);
        print(fd, "\n����� ���������� ������ϴ�.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M%j %1i%j ��� ����ϴ�.",
              ply_ptr, "1", obj_ptr,"3");
        }
    else
        F_CLR(obj_ptr, OHIDDN);

    return(0);

}

/************************************************************************/
/************************************************************************/

/*  Display information on creature given to player given.              */

int flag_list(ply_ptr, cmnd)
creature        *ply_ptr;
cmd                     *cmnd;
{
    int         fd;

    fd = ply_ptr->fd;

    print(fd,"���� ���� ����:\n");
    print(fd,"  �̾߱���: %s\n", F_ISSET(ply_ptr, PIGNOR) ? "�̼���" : " ���� ");
    print(fd,"  �����  : %s\n", F_ISSET(ply_ptr, PNOBRD) ? "�̼���" : " ���� ");
	print(fd,"  ȯȣ���  : %s\n", F_ISSET(ply_ptr, PNOBR2) ? "�̼���" : " ���� ");
	print(fd,"  ���纸��  : %s\n", F_ISSET(ply_ptr, PDSCRP) ? " ���� " : "�̼���");
    print(fd,"  ��ȯ      : %s\n", F_ISSET(ply_ptr, PNOSUM) ? " �Ұ� " : " ���� ");
    print(fd,"  ������ġ  : ");
        if(F_ISSET(ply_ptr, PWIMPY)) print(fd, "%-6d\n", ply_ptr->WIMPYVALUE);
        else                         print(fd, "�̼���\n");
    print(fd,"  �����    : %s\n", F_ISSET(ply_ptr, PNOCMP) ? " ���� " : "�̼���");
    print(fd,"  ����      : %s\n", F_ISSET(ply_ptr, PPROMP) ? " ��� " : "�̼���");
    print(fd,"  ����      : %s\n", F_ISSET(ply_ptr, PLECHO) ? " ���� " : "�̼���");
    print(fd,"  ��        : %s\n", F_ISSET(ply_ptr, PANSIC) ? " ��� " : "�̻��");
    print(fd,"  ������    : %s\n", F_ISSET(ply_ptr, PBRIGH) ? " ��� " : "�̻��");
    print(fd,"  ���̸�    : %s\n", F_ISSET(ply_ptr, PNORNM) ? "�̼���" : " ��� ");
    print(fd,"  ª������  : %s\n", F_ISSET(ply_ptr, PNOSDS) ? "�̼���" : " ��� ");
    print(fd,"  �伳��    : %s\n", F_ISSET(ply_ptr, PNOLDS) ? "�̼���" : " ��� ");
    print(fd,"  �ⱸ      : %s\n", F_ISSET(ply_ptr, PNOEXT) ? "�׷���" : "�ؽ�Ʈ");
    print(fd,"  �аŸ���ȯ: %s\n", F_ISSET(ply_ptr, PFRTUN) ? " ���� " : "�̼���");

/*
    if(F_ISSET(ply_ptr, PNOAAT)) strcat(str, "������������ ��ȯ�մϴ�.\n");
            if(F_ISSET(ply_ptr, PHASTE)) strcat(str, "Ȱ�������� �⸦ �����ϰ� �ֽ��ϴ�\n");
            if(F_ISSET(ply_ptr, PPRAYD)) strcat(str, "���� ��ȣ�� �ް��ֽ��ϴ�\n");
            if(F_ISSET(ply_ptr, PPOWER)) strcat(str, "������� ���� ������ �ֽ��ϴ�\n");
            if(F_ISSET(ply_ptr, PSLAYE)) strcat(str, "����� ���⿡ ��Ⱑ �����ϴ�\n");
            if(F_ISSET(ply_ptr, PMEDIT)) strcat(str, "����� �������� �繰�� ��Ծ�ϴ�\n");

            print(fd,"%s\n",str);
*/
    print(fd, "[���� ����]�̶�� ġ�ø� �ڼ��� ���������� �� �� �ֽ��ϴ�.\n");
    return(0);
}
/**********************************************************************/
/*              set                   */
/**********************************************************************/

/* This function allows a player to set certain one-bit flags.  The flags */
/* are settings for options that include brief and verbose display.  The  */
/* clear function handles the turning off of these flags.         */

int set(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int fd,i;
    struct {
        char *str;
        int flags;
    } setkeys[]={
        { "�̾߱���", PIGNOR },
        { "�����",   PNOBRD },
		{ "ȯȣ���", 	PNOBR2 },
		{ "���纸��",	PDSCRP },
        { "��ȯ",       PNOSUM },
        { "�����",     PNOCMP },
        { "����",       PPROMP },
        { "����",       PLECHO },
        { "��",         PANSIC },
        { "������",     PBRIGH },
        { "���̸�",     PNORNM },
        { "ª������",   PNOSDS },
        { "�伳��",     PNOLDS },
        { "�ⱸ",       PNOEXT },
        { NULL,         -1 },
    };

    fd =  ply_ptr->fd;

    if(cmnd->num == 1) {
                flag_list(ply_ptr,cmnd);
        return(0);
    }

    i=0;
    while(setkeys[i].str!=NULL) {
        if(!strcmp(cmnd->str[1], setkeys[i].str)) {
            if(F_ISSET(ply_ptr, setkeys[i].flags))
                F_CLR(ply_ptr, setkeys[i].flags );
            else
                F_SET(ply_ptr, setkeys[i].flags );
            break;
        }
        i++;
    }

    if(!strcmp(cmnd->str[1], "������ġ")) {
        F_SET(ply_ptr, PWIMPY);
        ply_ptr->WIMPYVALUE = cmnd->val[1] == 1L ? 10 : cmnd->val[1];
        ply_ptr->WIMPYVALUE = MAX(ply_ptr->WIMPYVALUE, 2);
        if(ply_ptr->WIMPYVALUE==0) F_CLR(ply_ptr,PWIMPY);
    }
    else if(!strcmp(cmnd->str[1], "�аŸ���ȯ")) {
    	if(F_ISSET(ply_ptr, PFAMIL)) {
    		F_SET(ply_ptr, PFRTUN);
    		print(fd, "�аŸ� ������ ��ȯ�� �մϴ�.\n");
    	}
    	else print(fd, "����� �аŸ��� ���ԵǾ� ���� �ʽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_SET(ply_ptr, PHEXLN);
        print(fd, "Hexline enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "eavesdropper")) {
        F_SET(ply_ptr, PEAVES);
        print(fd, "Eavesdropper mode enabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "~robot~")) {
        F_SET(ply_ptr, PROBOT);
        print(fd, "Robot mode on.\n");
    }
    else if(!strcmp(cmnd->str[1], "��������")) {
        F_SET(ply_ptr, PNOAAT);
        print(fd, "�������� �����մϴ�.\n");
    }

    flag_list(ply_ptr,cmnd);

    return(0);

}

/**********************************************************************/
/*              clear                     */
/**********************************************************************/

/* Like set, this function allows a player to alter the value of a part- */
/* icular player flag.                           */

int clear(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    int fd;

    fd = ply_ptr->fd;

    if(cmnd->num == 1) {
        print(fd, "[���� ����]�̶�� ġ�ø� ��� �������׵��� �� �� �ֽ��ϴ�.\n");
        return(0);
    }

    if(!strcmp(cmnd->str[1], "�����ź�")) {
        F_CLR(ply_ptr, PNOBRD);
        print(fd, "�������� ����� ����ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "�����")) { /* compact */
        F_CLR(ply_ptr, PNOCMP);
        print(fd, "�������� �޼����� ����Ҷ� ������ �������� �ʽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "����")) {
        F_CLR(ply_ptr, PLECHO);
        print(fd, "����� �޼����� ������� �ʽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "���̸�")) {
        F_SET(ply_ptr, PNORNM);
        print(fd, "�������� ���̸��� ������� �ʽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "����")) {
        F_SET(ply_ptr, PNOSDS);
        print(fd, "���� ������ ������ ���� �ʽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "�Ϲ�")) {
        F_SET(ply_ptr, PNOLDS);
        print(fd, "���� �ڼ��� ������ ���� �ʽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "hexline")) {
        F_CLR(ply_ptr, PHEXLN);
        print(fd, "Hex line disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "������ġ")) {
        F_CLR(ply_ptr, PWIMPY);
        print(fd, "������ġ ������ �����Ǿ����ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "eavesdropper")) {
        F_CLR(ply_ptr, PEAVES);
        print(fd, "Eavesdropper mode disabled.\n");
    }
    else if(!strcmp(cmnd->str[1], "����")) {
        F_CLR(ply_ptr, PPROMP);
        print(fd, "����� ���¸� �������� �ʽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "~robot~")) {
        F_CLR(ply_ptr, PROBOT);
        print(fd, "Robot mode off.\n");
    }
    else if(!strcmp(cmnd->str[1], "��")) {
        F_CLR(ply_ptr, PANSIC);
        print(fd, "�������� �޼����� ��� ������� ��µ˴ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "��ȯ�ź�")) {
        F_CLR(ply_ptr, PNOSUM);
        print(fd, "�������� �ٸ������ ����� ��ȯ�� �� �ֽ��ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "�̾߱���ź�")) {
        F_CLR(ply_ptr, PIGNOR);
        print(fd, "�������� �������� �̾߱⸦ ����ϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "��������")) {
        F_CLR(ply_ptr, PNOAAT);
        print(fd, "�������� �ڵ����� �����մϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "������")) {
        F_CLR(ply_ptr,PBRIGH);
        print(fd,"��ο� ������ ����մϴ�.\n");
    }
    else if(!strcmp(cmnd->str[1], "�аŸ���ȯ")) {
    	F_CLR(ply_ptr, PFRTUN);
    	print(fd,"�������� ��ȯ�մϴ�.\n");
    }
    else
        print(fd, "�߸� �����Ǿ����ϴ�.\n");

    return(0);

}

/**********************************************************************/
/*              quit                      */
/**********************************************************************/

/* This function checks to see if a player is allowed to quit yet.  It  */
/* checks to make sure the player isn't in the middle of combat, and if */
/* so, the player is not allowed to quit (and 0 is returned).       */

int quit(ply_ptr, cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
    long    i, t;
    int fd;

    fd = ply_ptr->fd;

    t = time(0);
    i = LT(ply_ptr, LT_ATTCK) + 20;

    if(ply_is_attacking(ply_ptr,cmnd) && t < i) {
        please_wait(fd, i-t);
        return(0);
    }

    update_ply(ply_ptr);
    return(DISCONNECT);
}

