/*
 * MAGIC6.C:
 *
 *      Additional spell-casting routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/************************************************************************/
/*                              resist_magic(������)                             */
/************************************************************************/

/* This function allows players to cast the resist-magic spell.  It  */
/* will allow the player to resist magical attacks from monsters   */

int resist_magic(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 12 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SRMAGI) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 12;
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_RMAGI].ltime = time(0);
                F_SET(ply_ptr, PRMAGI);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "%M�� ����� �������� �׸��� �ֹ��� �ܿ����ϴ�.\n���ڱ� ���ӿ��� ������ ���� ���ɵ��� �ö�� \n�������� �����մϴ�.\n", ply_ptr);
                if(how == CAST) {
                        print(fd, "����� ����� �����θ� �׸��� �ֹ��� �ܿ�ϴ�.\n���ӿ��� ������ ���� ��ȣ�ɵ��� �ö�� �������� �����մϴ�.\n");
                        ply_ptr->mpcur -= 12;
                        ply_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                    print(fd,"\n�� ���� ����� ����� �ֹ��� ��ȭ��ŵ�ϴ�.\n");
                    ply_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
                }
                else {
                        print(fd, "\n���ڱ� ���ӿ��� ������ ���� ��ȣ�ɵ��� �ö�� \n�������� �����մϴ�.\n");
                        ply_ptr->lasttime[LT_RMAGI].interval = 1200L;
                }
                return(1);
        }
        else {

                if(how == POTION) {
                        print(fd, "\n�� ������ �ິ�� �ڽſ��Ը� ����Ҽ� �ֽ��ϴ�.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);
                if(!crt_ptr) {
                        print(fd, "\n�׷� ����� �������� �ʽ��ϴ�.\n");
                        return(0);
                }

                F_SET(crt_ptr, PRMAGI);
                crt_ptr->lasttime[LT_RMAGI].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "%M�� %M�� ���� �����θ� �׸��� �ֹ���\n�ܿ����ϴ�.\n���ڱ� ���ӿ��� ���� ��ȣ�ɵ��� �ö�� �������� \n�����մϴ�.\n",
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M�� ����� ���� �����θ� �׸��� �ֹ���\n�ܿ����ϴ�.\n���ڱ� ���ӿ��� ���� ��ȣ�ɵ��� �ö�� �������� \n�����մϴ�.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "\n����� %M�� ���� �����θ� �׸��� �ֹ���\n�ܿ����ϴ�.\n���ӿ��� ���� ��ȣ�ɵ��� �ö�� �������� �����մϴ�.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 12;
                        crt_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
            crt_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
                }

                else {
                        print(fd, "\n���ӿ��� ���� ��ȣ�ɵ��� �ö�� %M�� ������ \n�������� �����մϴ�.\n",
                                crt_ptr);
                        crt_ptr->lasttime[LT_RMAGI].interval = 1200L;
                }

                return(1);
        }

}

/************************************************************************/
/*                              know_alignment(���ǰ�����)              */
/************************************************************************/

/* This spell allows the caster to determine what alignment another       */
/* creature or player is by looking at it.                          */

int know_alignment(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 6 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SKNOWA) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_KNOWA].ltime = time(0);
                F_SET(ply_ptr, PKNOWA);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M�� ���ǰ��� �ֹ��� �ܿ�ϴ�.", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n����� ������ ������ �� �ִ� �ĺ����� ���������ϴ�.\n");
                        ply_ptr->mpcur -= 6;
                        ply_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                   print(fd,"\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
                   ply_ptr->lasttime[LT_KNOWA].interval += 800L;
                }                                
                }
                else {
                        print(fd, "\n����� ������ ������ �� �ִ� �ĺ����� ���������ϴ�.\n");
                        ply_ptr->lasttime[LT_KNOWA].interval = 1200L;
                }
                return(1);
        }
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

                F_SET(crt_ptr, PKNOWA);
                crt_ptr->lasttime[LT_KNOWA].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "\n%M�� %M���� ���ǰ��� �ֹ��� �ܿ�ϴ�.\n�״� ������ ������ �� �ִ� �ĺ����� ���������ϴ�.\n",
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M�� ��ſ��� ���ǰ��� �ֹ��� �ܿ�ϴ�.\n����� ������ ������ �� �ִ� �ĺ����� ���������ϴ�.\n",
                        ply_ptr);

                if(how == CAST) {
                        print(fd, "����� %M���� ���ǰ��� �ֹ��� �ܿ�ϴ�.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 6;
                        crt_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                   print(fd,"\n�� ���� ����� ����� �ּ����� ��ȭ��ŵ�ϴ�.\n");
                   crt_ptr->lasttime[LT_KNOWA].interval += 800L;
                }                                
                }

                else {
                        print(fd, "%M�� ������ ������ �� �ִ� �ĺ����� ���������ϴ�.\n",
                                crt_ptr);
                        crt_ptr->lasttime[LT_KNOWA].interval = 1200L;
                }

                return(1);
        }

}

/**********************************************************************/
/*                      remove-curse(�����ؼ�)                                */
/**********************************************************************/

/* This function allows a player to remove a curse on all the items       */
/* in his inventory or on another player's inventory                      */

int remove_curse(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd, i;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 18 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SREMOV) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 18;
                return(0);
        }


        /* Cast remove-curse on self */
        if(cmnd->num == 2) {

                if(how == CAST)
                        ply_ptr->mpcur -= 18;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n����� �����տ� �������� ����� ������ ����\n���� ���������ϴ�.\n����� ���� �ɷȴ� ���ְ� Ǯ���� �����մϴ�.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M�� �����տ� �������� ����� ������ ����\n���� ���������ϴ�.\n�������� ����� ���� ���� �ɷȴ� ���ְ� Ǫ�°��� �������ϴ�.\n ",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n���Ǿȿ� ����ִ� �������� ����� ����� \n���ָ� Ǯ���ݴϴ�.\n");

                for(i=0; i<MAXWEAR; i++)
                        if(ply_ptr->ready[i])
                                F_CLR(ply_ptr->ready[i], OCURSE);
                F_CLR(ply_ptr, PFEARS);

                return(1);
        }

        /* Cast remove-curse on another player */
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

                if(how == CAST)
                        ply_ptr->mpcur -= 18;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n���� ���� %M�� ���� �������� �����\n�����մϴ�.\n���� ������ ���ְ� �������� ���� �������ϴ�.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M�� ����� ���� ���� ���� �������� �����\n�����մϴ�.\n����� ������ ���ְ� �������� ���� �������ϴ�.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� %M�� � ���� ��� �������� \n����� �����մϴ�.\n���� ������ �������� ������ ����� ������� ����\n������ �ֽ��ϴ�.\n",
                                       ply_ptr, crt_ptr);

                        for(i=0; i<MAXWEAR; i++)
                                if(crt_ptr->ready[i])
                                        F_CLR(crt_ptr->ready[i], OCURSE);
                        F_CLR(crt_ptr, PFEARS);

                        return(1);
                }
        }

        return(1);

}

int curse(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd, i;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 25 && how == CAST) {
                print(fd, "\n����� ������ �����մϴ�.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SCURSE) && how == CAST) {
                print(fd, "\n����� ���� �׷� �ֹ��� �͵����� ���߽��ϴ�.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 25;
                return(0);
        }


        /* Cast remove-curse on self */
        if(cmnd->num == 2) {

                if(how == CAST)
                        ply_ptr->mpcur -= 25;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n����� �����տ� ����� ����� ���� ����� ������ .\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M��  .\n ",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n���Ǿȿ� ����ִ� .\n");

                for(i=0; i<MAXWEAR; i++)
                        if(ply_ptr->ready[i])
                                F_SET(ply_ptr->ready[i], OCURSE);

                return(1);
        }

        /* Cast remove-curse on another player */
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

                if(how == CAST)
                        ply_ptr->mpcur -= 25;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n���� ���� %M�� ���� .\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M�� ����� ���� ���� ���� .\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M�� %M�� � ���� ��� .\n",
                                       ply_ptr, crt_ptr);

                        for(i=0; i<MAXWEAR; i++)
                                if(crt_ptr->ready[i])
                                        F_SET(crt_ptr->ready[i], OCURSE);

                        return(1);
                }
        }

        return(1);

}







