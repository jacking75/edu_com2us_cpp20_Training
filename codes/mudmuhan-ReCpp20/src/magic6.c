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
/*                              resist_magic(보마진)                             */
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
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SRMAGI) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
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
                        "%M이 허공에 보마부을 그리며 주문을 외웠습니다.\n갑자기 땅속에서 오행중 금의 정령들이 올라와 \n보마진을 형성합니다.\n", ply_ptr);
                if(how == CAST) {
                        print(fd, "당신은 허공에 보마부를 그리며 주문을 외웁니다.\n땅속에서 오행중 금의 수호령들이 올라와 보마진을 형성합니다.\n");
                        ply_ptr->mpcur -= 12;
                        ply_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                    print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
                    ply_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
                }
                else {
                        print(fd, "\n갑자기 땅속에서 오행중 금의 수호령들이 올라와 \n보마진을 형성합니다.\n");
                        ply_ptr->lasttime[LT_RMAGI].interval = 1200L;
                }
                return(1);
        }
        else {

                if(how == POTION) {
                        print(fd, "\n그 물건은 약병은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);
                if(!crt_ptr) {
                        print(fd, "\n그런 사람은 존재하지 않습니다.\n");
                        return(0);
                }

                F_SET(crt_ptr, PRMAGI);
                crt_ptr->lasttime[LT_RMAGI].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "%M이 %M의 몸에 보마부를 그리며 주문을\n외웠습니다.\n갑자기 땅속에서 금의 수호령들이 올라와 보마진을 \n형성합니다.\n",
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M이 당신의 몸에 보마부를 그리며 주문을\n외웠습니다.\n갑자기 땅속에서 금의 수호령들이 올라와 보마진을 \n형성합니다.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "\n당신은 %M의 몸에 보마부를 그리며 주문을\n외웠습니다.\n땅속에서 금의 수호령들이 올라와 보마진을 형성합니다.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 12;
                        crt_ptr->lasttime[LT_RMAGI].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            crt_ptr->lasttime[LT_RMAGI].interval += 800L;
        }                                
                }

                else {
                        print(fd, "\n땅속에서 금의 수호령들이 올라와 %M의 주위에 \n보마진을 형성합니다.\n",
                                crt_ptr);
                        crt_ptr->lasttime[LT_RMAGI].interval = 1200L;
                }

                return(1);
        }

}

/************************************************************************/
/*                              know_alignment(선악감지술)              */
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
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SKNOWA) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_KNOWA].ltime = time(0);
                F_SET(ply_ptr, PKNOWA);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M이 선악감지 주문을 외웁니다.", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n당신은 선악을 감지할 수 있는 식별력이 높아졌습니다.\n");
                        ply_ptr->mpcur -= 6;
                        ply_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                   print(fd,"\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
                   ply_ptr->lasttime[LT_KNOWA].interval += 800L;
                }                                
                }
                else {
                        print(fd, "\n당신은 선악을 감지할 수 있는 식별력이 높아졌습니다.\n");
                        ply_ptr->lasttime[LT_KNOWA].interval = 1200L;
                }
                return(1);
        }
        else {

                if(how == POTION) {
                        print(fd, "\n그 물건은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);
                if(!crt_ptr) {
                        print(fd, "\n그런 사람이 존재하지 않습니다.\n");
                        return(0);
                }

                F_SET(crt_ptr, PKNOWA);
                crt_ptr->lasttime[LT_KNOWA].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "\n%M이 %M에게 선악감지 주문을 외웁니다.\n그는 선악을 감지할 수 있는 식별력이 높아졌습니다.\n",
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M이 당신에게 선악감지 주문을 외웁니다.\n당신은 선악을 감지할 수 있는 식별력이 높아졌습니다.\n",
                        ply_ptr);

                if(how == CAST) {
                        print(fd, "당신은 %M에게 선악감지 주문을 외웁니다.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 6;
                        crt_ptr->lasttime[LT_KNOWA].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                   print(fd,"\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
                   crt_ptr->lasttime[LT_KNOWA].interval += 800L;
                }                                
                }

                else {
                        print(fd, "%M이 선악을 감지할 수 있는 식별력이 높아졌습니다.\n",
                                crt_ptr);
                        crt_ptr->lasttime[LT_KNOWA].interval = 1200L;
                }

                return(1);
        }

}

/**********************************************************************/
/*                      remove-curse(저주해소)                                */
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
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SREMOV) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
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
                        print(fd, "\n당신은 오른손에 성스러운 기운을 모으자 붉은\n빛이 퍼져나갑니다.\n당신의 몸에 걸렸던 저주가 풀리기 시작합니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M이 오른손에 성스러운 기운을 모으자 붉은\n빛이 퍼져나갑니다.\n성스러운 기운이 그의 몸에 걸렸던 저주가 푸는것이 느껴집니다.\n ",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n물건안에 담겨있던 성스러운 기운이 당신의 \n저주를 풀어줍니다.\n");

                for(i=0; i<MAXWEAR; i++)
                        if(ply_ptr->ready[i])
                                F_CLR(ply_ptr->ready[i], OCURSE);
                F_CLR(ply_ptr, PFEARS);

                return(1);
        }

        /* Cast remove-curse on another player */
        else {

                if(how == POTION) {
                        print(fd, "\n그 물건은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "\n그런 사람이 존재하지 않습니다.\n");
                        return(0);
                }

                if(how == CAST)
                        ply_ptr->mpcur -= 18;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n손을 통해 %M의 몸에 성스러운 기운을\n주입합니다.\n그의 몸에서 저주가 물러가는 것이 느껴집니다.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M이 당신의 몸에 손을 통해 성스러운 기운을\n주입합니다.\n당신의 몸에서 저주가 물러가는 것이 느껴집니다.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 %M의 등에 손을 대고 성스러운 \n기운을 주입합니다.\n그의 몸에서 느껴졌던 저주의 기운이 사라지는 것을\n느낄수 있습니다.\n",
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
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SCURSE) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
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
                        print(fd, "\n당신이 오른손에 기운을 모우자 붉은 기운이 몰려와 .\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M이  .\n ",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n물건안에 담겨있던 .\n");

                for(i=0; i<MAXWEAR; i++)
                        if(ply_ptr->ready[i])
                                F_SET(ply_ptr->ready[i], OCURSE);

                return(1);
        }

        /* Cast remove-curse on another player */
        else {

                if(how == POTION) {
                        print(fd, "\n그 물건은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "\n그런 사람이 존재하지 않습니다.\n");
                        return(0);
                }

                if(how == CAST)
                        ply_ptr->mpcur -= 25;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n손을 통해 %M의 몸에 .\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M이 당신의 몸에 손을 통해 .\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 %M의 등에 손을 대고 .\n",
                                       ply_ptr, crt_ptr);

                        for(i=0; i<MAXWEAR; i++)
                                if(crt_ptr->ready[i])
                                        F_SET(crt_ptr->ready[i], OCURSE);

                        return(1);
                }
        }

        return(1);

}







