/*
 * MAGIC2.C:
 *
 *  Additional user routines dealing with magic spells.  
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*              vigor  ( ȸ�� )                   */
/**********************************************************************/

/* This function will cause the vigor spell to be cast on a player or   */
/* another monster.  It heals 1d6 hit points plus any bonuses for       */
/* intelligence.  If the healer is a cleric then there is an additional */
/* point of healing for each level of the cleric.           */

int vigor(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, heal;
    int 	expadd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 2 && how == CAST) {
        print(fd, "����� ������ �����մϴ�.\n");
        return(0);
    }

    if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && ply_ptr->class < INVINCIBLE && how == CAST) {
	    print(fd, "�����ڿ� ���縸 �� �� �ִ� �����Դϴ�.\n");
	    return(0);
    }

    if(!S_ISSET(ply_ptr, SVIGOR) && how == CAST) {
        print(fd, "����� ���� �� �ּ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }

    if(ply_ptr->class == BARBARIAN || ply_ptr->class == FIGHTER) {
        if(how != POTION) {
            if(spell_fail(ply_ptr)){
                if(how == CAST)
                        ply_ptr->mpcur -= 2;
                return(0);
            }
        }
    }

    /* Vigor self */
    if(cmnd->num == 2) {

        if(how == CAST){
            heal = MAX(bonus[ply_ptr->intelligence], 
              bonus[ply_ptr->piety]) + 
            ((ply_ptr->class == CLERIC) ? ((ply_ptr->level+3)/4) +
              mrand(1,1+((ply_ptr->level+3)/4)/2) : 0) + 
            ((ply_ptr->class == PALADIN) ? ((ply_ptr->level+3)/4)/2 +
              mrand(1,1+((ply_ptr->level+3)/4)/4) : 0) +
            mrand(1,6);
            ply_ptr->mpcur -= 2;
            if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                heal += mrand(1,3);
                print(fd, "�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            }
        }
        else
            heal = mrand(1,6);

        heal = MAX(1, heal);

        ply_ptr->hpcur += MAX(1, heal);

        if(ply_ptr->hpcur > ply_ptr->hpmax)
            ply_ptr->hpcur = ply_ptr->hpmax;


        if(how == CAST || how == SCROLL) {
            print(fd, "����� ������ �ϰ� ȸ�� �ֹ��� �ܿ�ϴ�.\n���� ���Ⱑ �¸��� �����鼭 ü���� ���Ǿ����ϴ�.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                  "\n%M�� ������ �ϰ� �ֹ��� �ܿ�ϴ�.\n���� ���Ⱑ ���� ������ ���̴� ���� ���Դϴ�.\n",
                  ply_ptr);
            return(1);
        }
        else {
            print(fd, "����� ü���� ���Ǿ����ϴ�.\n");
            return(1);
        }
    }

    /* Cast vigor on another player or monster */
    else {
        if(how == POTION) {
            print(fd, "�� ������ �ڽſ��Ը� ����� �� �ֽ��ϴ�.\n");
            return(0);
        }

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->val[2]);

            if(!crt_ptr) {
                print(fd,"�׷� ����� �������� �ʽ��ϴ�.\n");
                return(0);
            }
        }

        if(how == CAST){
            heal = MAX(bonus[ply_ptr->intelligence], 
              bonus[ply_ptr->piety]) + 
            ((ply_ptr->class == CLERIC) ? ((ply_ptr->level+3)/4) +
              mrand(1,1+((ply_ptr->level+3)/4)/2) : 0) + 
            ((ply_ptr->class == PALADIN) ? ((ply_ptr->level+3)/4)/2 +
              mrand(1,1+((ply_ptr->level+3)/4)/4) : 0) +
            mrand(1,6);
            ply_ptr->mpcur -= 2;
            if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                heal += mrand(1,3);
                print(fd,"�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            }
        }
        else
            heal = mrand(1,6);

        heal = MAX(1, heal);

        crt_ptr->hpcur += MAX(1, heal);
        expadd = MAX(1, heal);

        if(crt_ptr->hpcur > crt_ptr->hpmax) { 
            crt_ptr->hpcur = crt_ptr->hpmax;
            expadd = 0;
        }
/*  ȸ�� ���� ����ġ�� �ִ� �κ�.
        ply_ptr->experience += expadd; 
    �ϴ��� ��ƾ�� ���� �ʾ���.  */
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "����� ������ �ϰ� %M�� ȸ���� ����ϴ� �ֹ��� �ܿ�ϴ�.\n���� ���Ⱑ ���� ������ ������ �ֽ��ϴ�.\n",
                    crt_ptr);
            print(crt_ptr->fd,"%M�� ������ �ϰ� ����� ȸ���� ����ϴ� �ֹ��� �ܿ�ϴ�.\n���� ���Ⱑ ����� ���� �����鼭 ü���� ���Ǿ����ϴ�.\n",
                            ply_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                  "\n%M�� %M���� ������ �ϰ� ȸ���� ����ϴ� �ֹ��� �ܿ�ϴ�.\n���� ���Ⱑ ����� ���� ��ġ�� ���� ������ ���̴�\n���� �������ϴ�.\n",
                  ply_ptr, crt_ptr);
            return(1);
        }
    }

    return(1);

}

/**********************************************************************/
/*              curepoison   ( �ص� )             */
/**********************************************************************/

/* This function allows a player to cast a curepoison spell on himself, */
/* another player or a monster.  It will remove any poison that is in   */
/* that player's system.                        */

int curepoison(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 6 && how == CAST) {
        print(fd,"����� ������ �����մϴ�.\n");
        return(0);
    }

/*  if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && ply_ptr->class < INVINCIBLE && how == CAST) {
	    print(fd, "�����ڿ� ���縸 �� �� �ִ� �����Դϴ�.\n");
	    return(0);
	}  */

    if(!S_ISSET(ply_ptr, SCUREP) && how == CAST) {
        print(fd, "����� ���� �� �ּ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
    if(how != POTION) {
        if(spell_fail(ply_ptr)) {
           if(how==CAST)
                 ply_ptr->mpcur -= 6;
           return(0);
        }
    }

    /* Curepoison self */
    if(cmnd->num == 2) {

        if(how == CAST)
            ply_ptr->mpcur -= 6;

        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd,"����� ���������� ������ ¤���鼭 �ص� �ֹ��� �ܿ�ϴ�.\n�հ��� ������ ���� ������� ���������°��� ���Դϴ�.\n");
            print(fd,"��� ���� ���� �ִ� ���� ��� �����������ϴ�.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                   "\n%M�� ������ ¤���鼭 �ص� �ֹ��� �ܿ�ϴ�.\n���� �հ��� ������ ���� ������� ���������� ���� ���Դϴ�.\n",
                   ply_ptr);
        }
        else if(how == POTION && F_ISSET(ply_ptr, PPOISN))
            print(fd, "\n������� ��ȭ�Ǵ� ���� ���� �� �ֽ��ϴ�.\n");
        else if(how == POTION)
            print(fd, "\n�ص��� �����ϼ̽��ϴ�.\n");

        F_CLR(ply_ptr, PPOISN);

    }

    /* Cure a monster or player */
    else {

        if(how == POTION) {
            print(fd,"�� ������ �ڽſ��Ը� ����� �� �ֽ��ϴ�.\n");
            return(0);
        }

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->str[2]);

            if(!crt_ptr) {
                print(fd,"�׷� ���� �������� �ʽ��ϴ�.\n");
                return(0);
            }
        }

        if(how == CAST) 
            ply_ptr->mpcur -= 6;

        F_CLR(crt_ptr, PPOISN);

        if(how == CAST || how == SCROLL || how == WAND) {
           print(fd,"%M�� ������ ¤���鼭 �ص� �ֹ��� �ܿ�ϴ�.\n���� �հ��� ������ ���� ������� ���������� ���� ���Դϴ�.\n",
                        crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                  "\n%M�� %M�� ������ ¤���鼭 �ص� �ֹ��� �ܿ�ϴ�.\n���� �հ��� ������ ���� ������� ���������� ���� ���Դϴ�.\n",
                  ply_ptr, crt_ptr);
            print(crt_ptr->fd,
                "%M�� ����� ������ ¤���鼭 �ص� �ֹ��� �ܿ�ϴ�.\n����� �հ��� ������ ������� ���������� ���� �������ϴ�.\n",
                ply_ptr);
        }

    }

    return(1);

}

/**********************************************************************/
/*              light  ( �߱� )                  */
/**********************************************************************/

/* This spell allows a player to cast a light spell which will illuminate */
/* any darkened room for a period of time (depending on level).       */

int light(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    int fd;

    fd = ply_ptr->fd;

    if(ply_ptr->mpcur < 5 && how == CAST) {
        print(fd, "����� ������ �����մϴ�.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SLIGHT) && how == CAST) {
        print(fd,"����� ���� �� �ּ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
    if(spell_fail(ply_ptr)) {
        if(how==CAST)
                ply_ptr->mpcur -= 5;
        return(0);
    }

    if(how == CAST){
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT))
        print(fd,"�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
        ply_ptr->mpcur -= 5;
    }

    F_SET(ply_ptr, PLIGHT);
    ply_ptr->lasttime[LT_LIGHT].ltime = time(0);
    ply_ptr->lasttime[LT_LIGHT].interval = 300L+((ply_ptr->level+3)/4)*300L +
        (F_ISSET(ply_ptr->parent_rom,RPMEXT)) ? 600L : 0;

    print(fd,"����� �޼տ� �߱� �ֹ��� �ɾ����ϴ�.\n�޼տ��� Ȳ�ݺ��� �վ��� ���� ������ ���� �ݴϴ�.\n");
    broadcast_rom(fd, ply_ptr->rom_num,
            "\n%M�� ���� �տ� �߱� �ֹ��� �ɾ����ϴ�.\n���� �տ��� Ȳ�ݻ��� ������ ���� �վ��� ���ɴϴ�.\n",
            ply_ptr);

    return(1);

}

/**********************************************************************/
/*              protection (��ȣ��)               */
/**********************************************************************/

/* This function allows a spellcaster to cast a protection spell either */
/* on himself or on another player, improving the armor class by a      */
/* score of 10.                             */

int protection(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    long        t;
    int     fd, heal;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
    t = time(0);

    if(ply_ptr->mpcur < 10 && how == CAST) {
        print(fd,"����� ������ �����մϴ�.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SPROTE) && how == CAST) {
        print(fd,"����� ���� �� �ּ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }

    if(spell_fail(ply_ptr)) {
        if(how==CAST)
              ply_ptr->mpcur -= 10;
        return(0);
    }

    /* Cast protection on self */
    if(cmnd->num == 2) {

        F_SET(ply_ptr, PPROTE);
        compute_ac(ply_ptr);

        ply_ptr->lasttime[LT_PROTE].ltime = t;
        if(how == CAST) {
            ply_ptr->lasttime[LT_PROTE].interval = MAX(300, 1200 + 
                bonus[ply_ptr->intelligence]*600);
            if(ply_ptr->class == CLERIC || ply_ptr->class == 
               PALADIN)
                ply_ptr->lasttime[LT_PROTE].interval += 
                60*((ply_ptr->level+3)/4);
            ply_ptr->mpcur -= 10;
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            ply_ptr->lasttime[LT_PROTE].interval += 800L;
        }
        }
        else
            ply_ptr->lasttime[LT_PROTE].interval = 1200;

        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "�μ����� ���� ���� �� ��ȣ���� �ֹ��� �ܿ�ϴ�.\n���� ��ȣ�ɵ��� ��� ������ �ѷ��θ� ����� ���� �����߽��ϴ�.\n");
            broadcast_rom(fd, ply_ptr->rom_num,
                  "\n%M�� �μ��� ������ ��ȣ���� �ֹ��� �ܿ�ϴ�.\n���� ��ȣ�ɵ��� ���� ������ �ѷ��θ� ����� ���� �����߽��ϴ�.\n",
                  ply_ptr);
        }
        else if(how == POTION)
            print(fd, "\n���� ��ȣ�ɵ��� ��� ������ �ѷ��θ� ����� ���� �����߽��ϴ�.\n");
        return(1);
    }

    /* Cast protection on another player */
    else {

        if(how == POTION) {
            print(fd, "�� ������ �ڽſ��Ը� ����� �� �ֽ��ϴ�.\n");
            return(0);
        }

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
            print(fd, "�׷� ����� �������� �ʽ��ϴ�.\n");
            return(0);
        }

        F_SET(crt_ptr, PPROTE);
        compute_ac(crt_ptr);

        crt_ptr->lasttime[LT_PROTE].ltime = t;
        if(how == CAST) {
            crt_ptr->lasttime[LT_PROTE].interval = MAX(300, 1200 + 
                bonus[ply_ptr->intelligence]*600);
            if(ply_ptr->class == CLERIC || ply_ptr->class == 
               PALADIN)
                crt_ptr->lasttime[LT_PROTE].interval += 
                60*((ply_ptr->level+3)/4);
            ply_ptr->mpcur -= 10;
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd, "�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            crt_ptr->lasttime[LT_PROTE].interval += 800L;
        }
        }
        else
            crt_ptr->lasttime[LT_PROTE].interval = 1200;

        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd,"%M�� ���� ��ȣ���� �׸��� ��ȣ���� �ֹ��� �ɾ����ϴ�.\n���� ��ȣ�ɵ��� ���� ������ �ѷ��θ� ����� ���� �����߽��ϴ�.\n",
                    crt_ptr);
            print(crt_ptr->fd, 
                    "%M�� ����� ���� ��ȣ���� �׸��� �ֹ��� �ɾ����ϴ�.\n���� ��ȣ�ɵ��� ����� ������ �ѷ��θ� ����� ���� �����߽��ϴ�.\n",
                    ply_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                    "%M�� %M�� ���� ��ȣ���� �׸��� �ֹ��� �ɾ����ϴ�.\n���� ��ȣ�ɵ��� ���� ������ �ѷ��θ� ����� ���� �����߽��ϴ�.\n",
                    ply_ptr, crt_ptr);
            return(1);
        }
    }

    return(1);

}

/**********************************************************************/
/*              mend (����ȸ��)                     */
/**********************************************************************/

/* This function will cause the mend spell to be cast on a player or    */
/* another monster.  It heals 2d6 hit points plus any bonuses for       */
/* intelligence.  If the healer is a cleric then there is an additional */
/* point of healing for each level of the cleric.           */

int mend(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, heal;
    int expadd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 4 && how == CAST) {
        print(fd,"����� ������ �����մϴ�.\n");
        return(0);
    }

/*  if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && ply_ptr->class < INVINCIBLE && how == CAST) {
	    print(fd, "�����ڿ� ���縸 �� �� �ִ� �����Դϴ�.\n");
	    return(0);
    }  */


    if(!S_ISSET(ply_ptr, SMENDW) && how == CAST) {
        print(fd,"����� ���� �� �ּ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
    if(ply_ptr->class == BARBARIAN || ply_ptr->class == FIGHTER 
			|| ply_ptr->class == ASSASSIN) {
            if(how != POTION) {
               if(spell_fail(ply_ptr)) {
                   if(how==CAST)
                        ply_ptr->mpcur -= 4;
                   return(0);
               }
            }
    }

    /* Mend self */
    if(cmnd->num == 2) {

        if(how == CAST){
            heal = MAX(bonus[ply_ptr->intelligence], 
              bonus[ply_ptr->piety]) + 
            (((ply_ptr->class == CLERIC)||(ply_ptr->class >= INVINCIBLE)) ? ((ply_ptr->level+3)/4)*2 + 
              mrand(1, 1+((ply_ptr->level+3)/4)/2) : 0) + 
            ((ply_ptr->class == PALADIN) ? ((ply_ptr->level+3)/4) +
              mrand(1, 1+((ply_ptr->level+3)/4)/3) : 0) +
            dice(2,6,0);
            ply_ptr->mpcur -= 4;
            if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                heal += mrand(1,6)+1;
                print(fd,"�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            }
        }
        else
            heal = dice(2,6,0);

        heal = MAX(1, heal);

        ply_ptr->hpcur += MAX(1, heal);

        if(ply_ptr->hpcur > ply_ptr->hpmax)
            ply_ptr->hpcur = ply_ptr->hpmax;


        if(how == CAST || how == SCROLL) {
            print(fd,"����� ����Ƚ��� �ڼ��� ���ϸ� ����ȸ���� �ֹ��� �ܿ�ϴ�.\n������ �߰ſ� ����� ����� ���� ������ ü���� ����ŵ�ϴ�.\n");
            broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M�� ����Ƚ��� �ڼ��� ���ϸ� ����ȸ���� �ֹ��� �ܿ�ϴ�.\n������ �߰ſ� ����� �׿��� �귯���� ���� �������ϴ�.\n",
                ply_ptr);
            return(1);
        }
        else {
            print(fd,"���� ü���� ���� ȸ���Ǿ����ϴ�.\n");
            return(1);
        }
    }

    /* Cast mend on another player or monster */
    else {

        if(how == POTION) {
            print(fd,"�� ������ �ڽſ��Ը� ����� �� �ֽ��ϴ�.\n");
            return(0);
        }

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
            cmnd->str[2][0] = low(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                       cmnd->str[2], cmnd->val[2]);

            if(!crt_ptr) {
                print(fd,"�׷��� ����� �������� �ʽ��ϴ�.\n");
                return(0);
            }
        }

        if(how == CAST) {
            heal = MAX(bonus[ply_ptr->intelligence],
              bonus[ply_ptr->piety]) + 
            ((ply_ptr->class == CLERIC) ? ((ply_ptr->level+3)/4)*2 + 
              mrand(1, 1+((ply_ptr->level+3)/4)/2) : 0) + 
            ((ply_ptr->class == PALADIN) ? ((ply_ptr->level+3)/4) +
              mrand(1, 1+((ply_ptr->level+3)/4)/3) : 0) +
            dice(2,6,0);
            ply_ptr->mpcur -= 4;
            if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                heal += mrand(1,6)+1;
                print(fd,"�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            }
        }
        else
            heal = dice(2,6,0);

        heal = MAX(1, heal);

        crt_ptr->hpcur += MAX(1, heal);
        expadd = MAX(1, heal);

        if(crt_ptr->hpcur > crt_ptr->hpmax) { 
            crt_ptr->hpcur = crt_ptr->hpmax;
            expadd = 0;
        }
/*   ���� ���� ����ġ�� �ִ� �κ�.
        ply_ptr->experience += expadd; 
     �ϴ��� ���� �ʾҾ��.   */
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "����� %M���� ������ �����ϸ� ����ȸ���� �ֹ��� �̴ϴ�.\n���� ���ȿ��� ������ �߰ſ� ����� �������ϴ�.", crt_ptr);
            print(crt_ptr->fd,"%M�� ��ſ��� ������ �����ϸ� ����ȸ���� �ֹ��� �̴ϴ�.\n����� ���ȿ��� ������ �߰ſ� ���� ü���� ���� ���Ǵ�\n���� �������ϴ�.\n",ply_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
               "\n%M�� %M���� ������ �����ϸ� ����ȸ���� �ֹ��� �̴ϴ�.\n�׿��� �߰ſ� ������ ����� �귯���� ���� �������ϴ�.\n",
               ply_ptr, crt_ptr);
            return(1);
        }
    }

    return(1);

}
