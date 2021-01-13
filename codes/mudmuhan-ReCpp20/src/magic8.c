/*
 *      MAGIC8.C:
 *
 *      Additional spell-casting routines.
 *
 */

#include "mstruct.h"
#include "mextern.h"

/***********************************************************************/
/*                              room_vigor(��ȸ��)                        */
/***********************************************************************/

int room_vigor(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd         *cmnd;
{          
        char    *sp;
        int     c = 0, fd, i, heal;
        ctag    *cp;
        ctag    *cp_tmp;

        fd = ply_ptr->fd;
 
        if(how == POTION) {
                print(fd, "�ֹ��� �����߽��ϴ�.");
                return(0);
        }
        if(!S_ISSET(ply_ptr, SRVIGO) && (how != WAND || how != SCROLL)) {
                print(fd, "����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.");
                return(0);
        }  
        if(ply_ptr->class != CLERIC && ply_ptr->class < INVINCIBLE) {
                print(fd, "�� �ּ��� �����ڸ��� ����� �� �ֽ��ϴ�.");
                                return(PROMPT);
        }
        if(ply_ptr->mpcur < 12 && (how != WAND || how != SCROLL)) {
                print(fd, "����� ������ �����մϴ�");
                                return(0);
        }
        if(how == CAST)
                ply_ptr->mpcur-=12;

        if(spell_fail(ply_ptr)) {
                return(0);
        }

                cp = ply_ptr->parent_rom->first_ply;

                print(fd,"����� �����¸� Ʋ�� ��ȸ�� �ֹ��� �ܿ�ϴ�.\n��ȿ� ���� �� �� ���� ������ ���� �������ٰ� ������ϴ�.\n����� ������� ü���� ȸ���Ǿ��� ���� ������ �ֽ��ϴ�.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� �����¸� Ʋ�� ��ȸ�� �ֹ��� �ܿ�ϴ�.\n��ȿ� ���� �� �� ���� ������ ���� �������ٰ� ������ϴ�.\n����� ������� ü���� ȸ���Ǿ��� ���� ������ �ֽ��ϴ�.\n"
                , ply_ptr);
         
                heal = mrand(1,6) + bonus[ply_ptr->piety];            

                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                        heal += mrand(1,3);
                        print(fd,"\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�\n");
                }        
                while(cp){
                        cp_tmp = cp->next_tag;
                        if(cp->crt->type != MONSTER) {
                                if(cp->crt != ply_ptr) {
                        print(cp->crt->fd,"����� �������� ȸ���� ����� �ھƿ����� ���� �� �ֽ��ϴ�.\n",ply_ptr);
                                }
                        cp->crt->hpcur += heal;
                        cp->crt->hpcur = MIN(cp->crt->hpmax, cp->crt->hpcur);
                        }
                        cp = cp_tmp;
                }

        return(1);
}

/**********************************************************************/
/*                      remove blindness(���ȼ�)               */
/**********************************************************************/
 
int rm_blind(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "����� ������ �����մϴ�");
        return(0);
    }
 
    if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
       ply_ptr->class < INVINCIBLE && how == CAST) {
            print(fd, "�� ����� �����ڿ� ���縸�� ����� �� �ֽ��ϴ�.");
            return(0);
    }                         
 
    if(!S_ISSET(ply_ptr, SRMBLD) && how == CAST) {
        print(fd, "����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.");
        return(0);
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }
 
    if(cmnd->num == 2) {
 
        if(how == CAST)
            ply_ptr->mpcur -= 12;
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "����� �̸��� ���Ⱥθ� ������ ���ȼ� �ֹ��� �ܿ�ϴ�.\n����� ���� ����Ÿ��ٰ� ���ڱ� ������ ������ϴ�.");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M�� �ڽ��� �̸��� ���Ⱥθ� ������ ���ȼ� \n�ֹ��� �ܿ�ϴ�.\n����� ���� ���� ����Ÿ��ٰ� ���ڱ� ���� Ȯ ��ϴ�.",
                      ply_ptr);
        }
        else if(how == POTION & F_ISSET(ply_ptr, PBLIND))
            print(fd, "����� ���� ����Ÿ��ٰ� ���ڱ� ������ ������ϴ�.");
        else if(how == POTION)
            print(fd, "���� ���� ��� ���� �ɸ� �ּ��� ������ Ǯ���� ���� �����ϴ�.");
 
        F_CLR(ply_ptr, PBLIND);
 
    }
 
    else {
 
        if(how == POTION) {
            print(fd, "�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.");
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
                print(fd, "�׷� ����� �������� �ʽ��ϴ�.");
                return(0);
            }
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
        F_CLR(crt_ptr, PBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "����� %M�� �̸��� ���Ⱥθ� ������ �ֹ��� \n�ܿ�ϴ�.\n���� ����� ���� ����Ÿ��ٰ� ���ڱ� Ȯ ��ϴ�.", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� %M�� �̸��� ���Ⱥθ� ������ \n�ֹ��� �ܿ�ϴ�.\n���� ����� ���� ����Ÿ��ٰ� ���ڱ� Ȯ ��ϴ�.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "%M�� ����� �̸��� ���Ⱥθ� ������ �ֹ���\n�ܿ�ϴ�.\n����� ����� ���� ����Ÿ��ٰ� ���ڱ� ������ϴ�.\n", ply_ptr);
        }
 
    }
 
    return(1);
 
}
 

/**********************************************************************/
/*                              blind                                 */
/**********************************************************************/
/* The blind  spell prevents a player or monster from seeing. The spell     *
 * results  in a -5 penalty on attacks, and an inability look at objects *
 * players, rooms, or inventory.  Also a player or monster cannot read. */

int blind(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd, dur;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "����� ������ �����մϴ�");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SBLIND) && how == CAST) {
        print(fd, "����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.");
        return(0);
    }

    if(ply_ptr->class<SUB_DM) {
        print(fd,"����� ����� ������ ���� �ֹ��Դϴ�.");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "����� ����� �巯���ϴ�.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ��Ÿ���� �����մϴ�.\n",
                  ply_ptr);
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        }
 
    if(how == CAST) {
        ply_ptr->mpcur -= 15;
        }
 
    /* blind self */
   if(cmnd->num == 2) {
                F_SET(ply_ptr,PBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "����� �μհ����� ������ �������� �ϰ� �Ǹ�\n�ֹ��� �ɾ����ϴ�.\n�հ������� �����Ȱ����� ����� ���� ����� ���� ��ϴ�.\n��~~~ ����.. ����� ���� ������ �ʽ��ϴ�.");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M�� �μհ����� ������ �������� �ϰ� �Ǹ�\n�ֹ��� �ɾ����ϴ�.\n�հ������� �����Ȱ����� ����� ���� ���� ���\n������ �����ϴ�. ��~~ ����..",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "���ڱ� ����� ���� ������� ���� ������ �ʽ��ϴ�.");
 
    }
 
    /* blind a monster or player */
    else {
        if(how == POTION) {
            print(fd, "�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.");
            return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);
 
        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);
 
            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 3) {
                print(fd, "�׷� ����� �������� �ʽ��ϴ�.");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "����� %s���� �ּ��� �� �� �����ϴ�.",
                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            return(0);
        }
 
                if (crt_ptr->type == PLAYER)
                        F_SET(crt_ptr,PBLIND);
                else
                        F_SET(crt_ptr,MBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "����� �հ����� %M�� ���� ���ϰ� �Ǹ� \n�ֹ��� �ܿ�ϴ�.\n�����Ȱ����� ����� �հ������� ���� ���� ����\n��� ������ �����ϴ�. ��~~ ����..\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� �հ����� %M�� ���� ���ϰ� �Ǹ�\n�ֹ��� �ܿ����ϴ�.\n�����Ȱ����� ����� �հ������� ���� ���� ���� \n��� ������ �����ϴ�. ��~~ ����..\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "\n%M�� �հ����� ����� ���� ���ϰ� �Ǹ� �ֹ��� �ܿ�ϴ�.\n�����Ȱ����� ����� �հ������� ���� ����� ����\n ��� ������ �����ϴ�. ��~~ ����..\n����� ���� ���� ���ܼ� ������ �ʽ��ϴ�.\n",
                  ply_ptr);
        }
 
        if(crt_ptr->type != PLAYER)
            add_enm_crt(ply_ptr->name, crt_ptr);
 
    }
 
    return(1);
 
}            

/**********************************************************************/
/*                               fear                                 */
/**********************************************************************/
/* The fear spell causes the monster to have a high wimpy / flee   *
 * percentage and a penality of -2 on all attacks */
 
int fear(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd, dur;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "����� ������ �����մϴ�.");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SFEARS) && how == CAST) {
        print(fd, "����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "����� ����� �巯���ϴ�.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ��Ÿ���� �����մϴ�.",
                  ply_ptr);
    }
 
    if(how == CAST) {
        dur =  600 + mrand(1,30)*10  + bonus[ply_ptr->intelligence]*150;
        ply_ptr->mpcur -= 15;
        }
    else if (how == SCROLL)
        dur =  600 + mrand(1,15)*10  + bonus[ply_ptr->intelligence]*50;
    else 
        dur = 600 + mrand(1,30)*10;
     
        if(spell_fail(ply_ptr)) {
                return(0);
        }
 
    /* fear on self */
   if(cmnd->num == 2) {
        if (F_ISSET(ply_ptr,PRMAGI))
                dur /= 2;
 
        ply_ptr->lasttime[LT_FEARS].ltime = time(0);
        ply_ptr->lasttime[LT_FEARS].interval = dur;
                F_SET(ply_ptr,PFEARS);
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "����� �Ǽ��� ���������� ����߷Ƚ��ϴ�.\n���ڱ� ����� �������ϴ� �͵��� ��Ÿ�� ����� �ѷ��Դϴ�.\n��~~~ ������~~ ����� ������ ���� �����մϴ�.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M�� �Ǽ��� ���������� ����߷Ƚ��ϴ�.\n���ڱ� ������ ���ϰ� �������� ���ڱ� �װ� ������\n������ �����մϴ�. ��~~~ ������~~\n�װ� ������ ���� �����ϴµ� ����� ������ �ƹ��͵� ������ �ʽ��ϴ�.",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "���ڱ� ����� �������ϴ� �͵��� ��Ÿ�� ����� �ѷ��Դϴ�.\n��~~~ ������~~ ����� ������ ���� �����մϴ�.");
 
    }
 
    /* fear a monster or player */
    else {
        if(how == POTION) {
            print(fd, "�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.");
            return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);
 
        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);
 
            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 3) {
                print(fd, "�׷� ����� ���� ���� �ʽ��ϴ�.");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "����� %s���� �ּ��� �� �� �����ϴ�.",
                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            return(0);
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MPERMT)) {
            print(fd, "%M�� ������ ������ ����� �ѷ��Դϴ�.\n������, �װ� ������ ������ �ݻ� �� ����� ��������ϴ�.",crt_ptr);
            return(0);
        }

        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dur /= 2;
 
        crt_ptr->lasttime[LT_FEARS].ltime = time(0);
        crt_ptr->lasttime[LT_FEARS].interval = dur;
                if (crt_ptr->type == PLAYER)
                        F_SET(crt_ptr,PFEARS);
                else
                        F_SET(crt_ptr,MFEARS);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "����� ���������� %M���� �������ϴ�.\n������ ���ϰ� �����鼭 ������ ����� �׸� �ѷ��Դϴ�.\n��~~~ ������~~ ���ڱ� �װ� ������ �����鼭 ������\n���� �����մϴ�.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� %M���� ���������� �������ϴ�.\n������ ���ϰ� ������ ���ڱ� �װ� ������ �����ϴ�. ��~~~ ������~~\n�״� ������ ������ ����� ������ �ƹ��͵� ������ �ʽ��ϴ�.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "\n%M�� ��ſ��� ���������� �������ϴ�.\n���ڱ� ����� �������ϴ� �͵��� ��Ÿ�� ����� �ѷ��Դϴ�.\n\"��~~~ ������~~\" ����� ������ ������ ������ ����\n�����մϴ�.\n",
                  ply_ptr);
        }
 
        if(crt_ptr->type != PLAYER)
            add_enm_crt(ply_ptr->name, crt_ptr);
 
    }
 
    return(1);
 
}            


/**********************************************************************/
/*                            silence(���ձ�)                                 */
/**********************************************************************/
/* Silence  causes a player or monster to lose their voice, makin them */
/* unable to casts spells, use scrolls, speak, yell, or broadcast */

int silence(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int         fd, dur;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "����� ������ �����մϴ�.");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SSILNC) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
 
    if(ply_ptr->class<SUB_DM) {
        print(fd,"�� �ֹ��� ��ġ�⿣ ����� �ɷ��� �����մϴ�.");
        return 0;
    }

    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "����� ����� �巯���ϴ�.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M�� ����� ��Ÿ���� �����մϴ�.",
                  ply_ptr);
    }
 
    if(how == CAST) {
        dur = 3600; 
        ply_ptr->mpcur -= 12;
        }
    else if (how == SCROLL)
        dur =  300 + mrand(1,15)*10  + bonus[ply_ptr->intelligence]*75;
    else 
        dur = 300 + mrand(1,15)*10;

        if(spell_fail(ply_ptr)) {
                return(0);
        }
 
    /* silence on self */
   if(cmnd->num == 2) {
        if (F_ISSET(ply_ptr,PRMAGI))
                dur /= 2;
 
        ply_ptr->lasttime[LT_SILNC].ltime = time(0);
        ply_ptr->lasttime[LT_SILNC].interval = dur;
                F_SET(ply_ptr,PSILNC);
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "\n����� �Ǽ��� ���ձ� �ֹ��� �ڽſ��� �ɾ����ϴ�.\n��... ����� ���� ���� ���� �Ϸ� ������ ��Ҹ���\n��������ϴ�.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M�� �Ǽ��� ���ձ� �ֹ��� �ڽſ��� �ɾ����ϴ�.\n�״� ���� ���� ���� �Ϸ� ������ ��Ҹ��� �鸮�� �ʽ��ϴ�.\n",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "\n����� �������� �� �ɷ� ��Ҹ��� ������ �ʽ��ϴ�.\n");
 
    }
 
    /* silence a monster or player */
    else {
        if(how == POTION) {
            print(fd, "\n�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
            return(0);
        }
 
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                   cmnd->str[2], cmnd->val[2]);
 
        if(!crt_ptr) {
            cmnd->str[2][0] = up(cmnd->str[2][0]);
            crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                       cmnd->str[2], cmnd->val[2]);
 
            if(!crt_ptr || crt_ptr == ply_ptr || 
               strlen(cmnd->str[2]) < 3) {
                print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "\n����� %s���� �ּ��� �� �� �����ϴ�.\n",
                F_ISSET(crt_ptr, MMALES) ? "��":"�׳�");
            return(0);
        }
 
        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dur /= 2;
 
        crt_ptr->lasttime[LT_SILNC].ltime = time(0);
        crt_ptr->lasttime[LT_SILNC].interval = dur;
                if (crt_ptr->type == PLAYER)
                        F_SET(crt_ptr,PSILNC);
                else
                        F_SET(crt_ptr,MSILNC);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "\n����� ��ΰ� �Ѿư� %M�� ���� ġ�鼭 \n���ձ� �ֹ��� �ܿ�ϴ�.\n�״� ���� ���� ���� �Ϸ� ������ ��Ҹ��� ������ �ʽ��ϴ�.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� ��ΰ� �Ѿư� %M�� ���� ġ�鼭 \n���ձ� �ֹ��� �ܿ�ϴ�.\n�״� ���� ���� ���� �Ϸ� ������ ��Ҹ��� ������ �ʽ��ϴ�.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "\n%M�� ��ΰ� �Ѿƿ� ����� ���� ġ�鼭 ���ձ�\n�ֹ��� �ܿ�ϴ�.\n����� ���� ���� ���� �Ϸ� ������ ��Ҹ��� ������ �ʽ��ϴ�.\n",
                  ply_ptr);
        }
 
        if(crt_ptr->type != PLAYER)
            add_enm_crt(ply_ptr->name, crt_ptr);
 
    }
 
    return(1);
 
}            
              
/**********************************************************************/
/*                      remove blindness                              */
/**********************************************************************/
 
int rm_blindness(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;
 
    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n����� ������ �����մϴ�\n");
        return(0);
    }
 
        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n�� �ּ��� �����ڿ� ���縸�� ����� �� �ֽ��ϴ�.\n");
                return(0);
        }                         
 
    if(!S_ISSET(ply_ptr, SRMDIS) && how == CAST) {
        print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
        return(0);
    }
if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        } 
    if(cmnd->num == 2) {
 
        if(how == CAST)
            ply_ptr->mpcur -= 12;
 
        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "\nRemove blindness spell cast on yourself.\n");
            print(fd, "\nYou can see.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M cast remove blindness on %sself.", 
                      ply_ptr,
                      F_ISSET(ply_ptr, PMALES) ? "��":"�׳�");
        }
        else if(how == POTION & F_ISSET(ply_ptr, PBLIND))
            print(fd, "You can see.\n");
        else if(how == POTION)
            print(fd, "Nothing happens.\n");
 
        F_CLR(ply_ptr, PBLIND);
 
    }
 
    else {
 
        if(how == POTION) {
            print(fd, "You can only use a potion on yourself.\n");
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
                print(fd, "That's not here.\n");
                return(0);
            }
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
                if (crt_ptr->type == PLAYER)
                        F_CLR(crt_ptr,PBLIND);
                else
                        F_CLR(crt_ptr,MBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "You cast the remove blindness spell on %M.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "%M cast remove blindness on %M.",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "%M cast remove blindness on you.\nYou can see.\n", ply_ptr);
        }
 
    }
 
    return(1);
 
}
/**********************************************************************/
/*              charm (��ȥ���)                                         */
/**********************************************************************/

int charm(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int      how;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd, n, dur;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "����� ������ �����մϴ�");
        return(0);
    }

     
    if(!S_ISSET(ply_ptr, SCHARM) && how == CAST) {
        print(fd, "����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.");
        return(0);
    }
	if(F_ISSET(rom_ptr, RSUVIV)) {
		print(fd, "����忡���� �� �ֹ��� ����� �� �����ϴ�.");
		return(0);
	}
    if(how == CAST) {
        dur =  300 + mrand(1,30)*10  + bonus[ply_ptr->intelligence]*30;
    }
    else if (how == SCROLL)
        dur =  100 + mrand(1,15)*10  + bonus[ply_ptr->intelligence]*30;
    else
        dur = 100 + mrand(1,15)*10;
 
    if(cmnd->num == 2) {

        if(how == CAST)
            ply_ptr->mpcur -= 15;

        if(spell_fail(ply_ptr)) {
                return(0);
        }

        ply_ptr->lasttime[LT_CHRMD].ltime = time(0);
        ply_ptr->lasttime[LT_CHRMD].interval = dur;

        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "����� �ɽ��ؼ� �ſ��� ���� ��ȥ����� ����մϴ�.\n����� �������鼭 ���� �¾Ƶ� ȲȦ�� �����\n��ϴ�. �� �� ������..");
            broadcast_rom(fd, ply_ptr->rom_num,
                      "\n%M�� ��ȥ����� �ּ��� �Ŵ� �ſ��� ���ϴ�.\n�ſ��� ������ ����� ���鼭 ���� ��������\n�Ÿ��ϴ�. �� �ڽ� ���Ƴ�?",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "����� �������鼭 ���� �¾Ƶ� ȲȦ�� �����\n��ϴ�. �� �� ������..");


    }

    else {

        if(how == POTION) {
            print(fd, "�� ������ �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.");
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
                print(fd, "�׷� ����� �������� �ʽ��ϴ�.");
                return(0);
            }
        }

        if(how == CAST)
            ply_ptr->mpcur -= 15;

        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dur /= 2;
        if((ply_ptr->level < crt_ptr->level) || F_ISSET(crt_ptr, MNOCHA)) {
            print(fd, "����� �Ⱑ ����� �ֹ��� ��ź��ŵ�ϴ�.");
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
               "\%M�� ��ȥ����� %M���� �ɷ��� �մϴ�.\n",
                ply_ptr, crt_ptr);
            if(crt_ptr->type == PLAYER)
              print(crt_ptr->fd, "%M�� ��ſ��� ��ȥ����� ������ �մϴ�.\n",ply_ptr);
            return(0);
        }

        if(how == CAST || how == SCROLL || how == WAND) {

            print(fd, "����� %M���� �ſ��� ���߸� ��ȥ����� �̴ϴ�.\n�ſ��� ������ ����� ���鼭 ���� ��������\n�Ÿ��ϴ�. ���� �� ����.", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M�� %M���� �ſ��� ���߸� ��ȥ����� �̴ϴ�.\n�ſ��� ������ ����� ���鼭 ���� ��������\n�Ÿ��ϴ�. �� �ڽ��� ���Ƴ�?",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "\n%M�� ��ſ��� �ſ��� ���߸� ��ȥ����� �̴ϴ�.\n���� ����� �������鼭 �¾Ƶ� ȲȦ�� �����\n��ϴ�. �� �� ������..", ply_ptr);
            add_charm_crt(crt_ptr, ply_ptr);

            crt_ptr->lasttime[LT_CHRMD].ltime = time(0);
            crt_ptr->lasttime[LT_CHRMD].interval = dur;

            if(crt_ptr->type == PLAYER)
                F_SET(crt_ptr, PCHARM);
            else 
                F_SET(crt_ptr, MCHARM);

        }

    }

    return(1);

}


/****************************************************************************/
/*                      spell_fail                                    */
/****************************************************************************/

/* This function returns 1 if the casting of a spell fails, and 0 if it is  */
/* sucessful.                                                                */

int spell_fail(ply_ptr)
creature *ply_ptr;

{

int      chance, fd, n;


        fd=ply_ptr->fd;
        n = mrand(1,100);

switch(ply_ptr->class) {

        case ASSASSIN:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+30; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

        case BARBARIAN:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5); 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

      /*  case BARD:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+60; 
                if(n>chance) {
                        print(fd,"Your spell fails.");
                        return(1);
                }
                else
                        return(0); */

        case CLERIC:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+65; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

        case FIGHTER:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+10; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

        case MAGE:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+75; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

/*        case MONK:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*6)+25; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� ���� �߽��ϴ�.");
                        return(1);
                }
                else
                        return(0); */

        case PALADIN:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+50; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

        case RANGER:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*4)+56; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

        case THIEF:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*6)+22; 
                if(n>chance) {
                        print(fd,"����� �ֹ��� �����߽��ϴ�.");
                        return(1);
                }
                else
                        return(0);

        default:
                return(0);
    }
}

