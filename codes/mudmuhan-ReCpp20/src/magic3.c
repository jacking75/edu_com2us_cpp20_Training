/*
 * MAGIC3.C:
 *
 *      Additional spell-casting routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              bless (성현주)                             */
/**********************************************************************/

/* This function allows a player to cast a bless spell on himself or  */
/* on another player, reducing the target's thaco by 1.          */

int bless(ply_ptr, cmnd, how)
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
                        "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SBLESS) && how == CAST) {
                print(fd, 
                        "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 10;
                return(0);
        }

        /* Cast bless on self */
        if(cmnd->num == 2) {

                F_SET(ply_ptr, PBLESS);
                compute_thaco(ply_ptr);

                ply_ptr->lasttime[LT_BLESS].ltime = t;
                if(how == CAST) {
                        ply_ptr->lasttime[LT_BLESS].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == CLERIC || 
                           ply_ptr->class == PALADIN)
                                ply_ptr->lasttime[LT_BLESS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
                            ply_ptr->lasttime[LT_BLESS].interval += 800L;
                        }                                
                }
                else
                        ply_ptr->lasttime[LT_BLESS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,
                                "\n당신은 조용히 눈을 감으며 성현주를 외웁니다.\n성현주를 외우자 머리에서 삼매광이 뿜어져 나와 성스런 기운이\n몸을 휘감습니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M이 조용히 눈을 감으며 성현주를 외웁니다.\n그의 머리에서 삼매광이 뿜어져 나와 성스런 기운이 몸을\n휘감습니다.\n",
                                        ply_ptr);
                }
                else if(how == POTION)
                        print(fd, 
                                "\n하늘에서 천수광이 내려와 성스런 기운들로 몸을 휘감습니다.\n");

                return(1);
        }

        /* Cast bless on another player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "그 물건은 자신에게만 사용할 수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, 
                                "\n그런 사람이 존재하지 않습니다.\n");
                        return(0);
                }

                F_SET(crt_ptr, PBLESS);
                compute_thaco(crt_ptr);

                crt_ptr->lasttime[LT_BLESS].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_BLESS].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == CLERIC || ply_ptr->class == 
                           PALADIN)
                                crt_ptr->lasttime[LT_BLESS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
                            crt_ptr->lasttime[LT_BLESS].interval += 800L;
                        }                                
                }
                else
                        crt_ptr->lasttime[LT_BLESS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, 
                                "\n당신은 한쪽손을 %M의 머리에 얹으며 성현주를 외웁니다.\n그의 머리에서 삼매광이 뿜어져 나와 성스러운 기운이 몸을 휘감습니다.\n",
                                crt_ptr);
                        print(crt_ptr->fd, 
                                "\n%M이 당신의 머리에 한쪽손을 얹으며 성현주를 외웁니다.\n당신의 머리에서 삼매광이 뿜어져 나와 성스러운 기운이 몸을\n휘감습니다.\n",
                                ply_ptr);
                                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                                    "\n%M이 %M의 머리에 한쪽손을 얹으며 성현주를 \n외웁니다.\n그의 머리에서 삼매광이 뿜어져 나와 성스러운 기운이 몸을\n휘감습니다.\n",
                                                    ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              turn ( 방혼술 )                        */
/**********************************************************************/

/* This function allows clerics and paladins to turn undead creatures. */
/* If they succeed then the creature is either disintegrated or harmed */
/* for approximately half of its hit points.                       */

int turn(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              chance, m, dmg, fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd, 
                        "\n누구에게 주문을 거실려고요?\n");
                return(0);
        }

        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < INVINCIBLE) {
                print(fd, 
                        "\n불제자와 무사만이 방혼술을 사용할수 있습니다.\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                print(fd, 
                        "\n그런 괴물은 존재하지 않습니다.\n");
                return(0);
        }

        if(!F_ISSET(crt_ptr, MUNDED)) {
                print(fd, 
                        "\n죽은 괴물에게만 사용가능합니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, 
                        "\n당신의 모습이 나타나기 시작합니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "%M의 모습이 보이기 시작합니다.",
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
                        "\n당신은 %s의 혼을 소멸시킬 수 없습니다.\n",
                        F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                return(0);
        }

        add_enm_crt(ply_ptr->name, crt_ptr);

        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 30L;

        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->piety]*5 + (ply_ptr->class == PALADIN ? 15:25);
        chance = MIN(chance, 80);

        if(mrand(1,100) > chance) {
                print(fd, 
                        "\n부적을 하늘로 날리며 혼을 소환하는 방혼술의 주문을 \n외쳤습니다.하지만 주문이 튕겨져 나오면서 %M가 당신의 주술을 \n견뎌냈습니다.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "\n%M이 부적을 하늘로 날리며 혼을 소환시키는 방혼술의 \n주문을 외칩니다.\n하지만 주문이 튕겨져 나오면서 %M가 그의 주술을 \n견뎌냈습니다.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }

        if(mrand(1,100) > 90 - bonus[ply_ptr->piety]) {
                print(fd,  
                        "\n부적을 하늘로 날리며 혼을 소환시키는 방혼술의 주문을 외칩니다.\n부적이 빙글비글 돌면서 소용돌이를 일으키자 그 자리에서 저승사자가\n올라와 %M의 혼을 소멸시켜 버렸습니다.\n ",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "\n%M이 부적을 하늘로 날리며 혼을 소환시키는 방혼술의 \n주문을 외칩니다.\n부적이 빙글빙글 돌면서 소용돌이를 일으키자 그 자리에서 \n저승사자가 올라와 %M의 혼을소멸시켜 버렸습니다.\n",
                                    ply_ptr, crt_ptr);
                add_enm_dmg(ply_ptr->name, crt_ptr, crt_ptr->hpcur);
                die(crt_ptr, ply_ptr);
        }

        else {
                dmg = MAX(1, crt_ptr->hpcur / 2);
                m = MIN(crt_ptr->hpcur, dmg);
                crt_ptr->hpcur -= dmg;
                add_enm_dmg(ply_ptr->name, crt_ptr, m);
                print(fd,  
                        "\n부적을 하늘로 날리며 혼을 소환시키는 방혼술의 주문을 외칩니다.\n부적이 %M의 몸을 공격하며 %d만큼의 타격을 입혔습니다\n",
                        crt_ptr, dmg);
                broadcast_rom(fd, ply_ptr->rom_num, 
                                "\n%M이 부적을 하늘로 날리며 혼을 소환시키는 방혼술의 \n주문을 외칩니다.\n부적이 %M의 몸을 공격하며 타격을 입혔습니다\n",
                                    ply_ptr,crt_ptr);
                if(crt_ptr->hpcur < 1) {
                        print(fd, 
                                "\n당신은 %M의 혼을 소멸시켰습니다.\n%M의 몸에서 혼이 사라지자 녹아버립니다.\n",
                                    crt_ptr);
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M이 %M의  혼을 소멸시켰습니다.\n\n%M의 혼이 사라지자 몸이 녹아버립니다.\n",
                                            ply_ptr, crt_ptr, crt_ptr);
                        die(crt_ptr, ply_ptr);
                }
        }

        return(0);

}

/**********************************************************************/
/*                          absorb ( 흡성대법 )                       */
/**********************************************************************/

/* 이기능은 적의 체력을 빨아들여 자신의 체력에 더하는 루틴이다.       */
/*                                                                    */

int absorb(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              chance, m, dmg, fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(cmnd->num < 2) {
                print(fd,
                        "\n누구에게 주문을 거실려고요?\n");
                return(0);
        }

        if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE) {
                print(fd,
                        "\n도술사만이 흡성대법을 사용할수 있습니다.\n");
                return(0);
        }

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);

        if(!crt_ptr) {
                print(fd,
                        "\n그런 괴물은 존재하지 않습니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd,
                        "\n당신의 모습이 나타나기 시작합니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num,
                                "%M의 모습이 보이기 시작합니다.",
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
                        "\n당신은 %s의 기를 흡수할수 없습니다.\n",
                        F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                return(0);
        }

        add_enm_crt(ply_ptr->name, crt_ptr);

        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 30L;

        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->intelligence]*5;
        chance = MIN(chance, 80);

        if(mrand(1,100) > chance) {
                print(fd,
                        "\n손을 적에게 뻗으며 기를 흡수하는 흡성대법의 주문을 \n외쳤습니다.하지만 주문이 튕겨져 나오면서 %M가 당신의 주술을 \n견뎌냈습니다.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M이 손을 적에게 뻗으며 기를 흡수하는 흡성대법의 \n주문을 외칩니다.\n하지만 주문이 튕겨져 나오면서 %M가 그의 주술을 \n견뎌냈습니다.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }

                print(fd,
                        "\n손을 적에게 뻗으며 기를 흡수하는 흡성대법의 주문을 외칩니다.\n적의 몸에서 기가 흘러나와 손으로\n빨려 들어갑니다.\n ",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M이 손을 적에게 뻗으며 기를 흡수하는 흡성대법의 주문을 외칩니다.\n%M의 몸에서 기가 흘러나와 손으로 빨려듭니다.\n",
                                    ply_ptr, crt_ptr);

        if(F_ISSET(crt_ptr, MUNDED)) {
                print(fd,
                        "\n윽! 악령의 사악한 기운이 당신에게 흘러듭니다.\n");
                ply_ptr->mpcur = 0;

                return(0);
        }


                dmg = ((ply_ptr->level+73)/4)*10;
                m = MIN(crt_ptr->hpcur, dmg);
                crt_ptr->hpcur -= dmg;
                ply_ptr->hpcur += MAX(1, dmg);
        /*        if(ply_ptr->hpcur > ply_ptr->hpmax)
                     ply_ptr->hpcur = ply_ptr->hpmax;
        */
                add_enm_dmg(ply_ptr->name, crt_ptr, m);
     /*           print(fd, "테스트 %d %d", dmg, m);       */

                if(crt_ptr->hpcur < 1) {
                        print(fd,
                                "\n당신은 %M의 모든기를 흡수했습니다.\n%M의 몸에서 기가 빠지자 쓰러져 버립니다.\n",
                                    crt_ptr, crt_ptr);
                        broadcast_rom(fd, ply_ptr->rom_num,
                                        "\n%M이 %M의  모든기를 흡수했습니다.\n\n %M의 기운이 %M 의 몸 주위로 둘러싸이면서 하얀 기운을 내뿜습니다.\n\n%M의 몸에서 기가 빠지자 쓰러져 버립니다.\n",
                                            ply_ptr, crt_ptr, crt_ptr, ply_ptr, crt_ptr);
                        die(crt_ptr, ply_ptr);
                }

        return(0);

}

/**********************************************************************/
/*                              invisibility ( 은둔술 )                  */
/**********************************************************************/

/* This function allows a player to cast an invisibility spell on himself */
/* or on another player.                                           */

int invisibility(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        ctag            *cp;
        long            t;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        t = time(0);

        if(ply_ptr->mpcur < 15 && how == CAST) {
                print(fd, 
                        "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SINVIS) && how == CAST) {
                print(fd, 
                        "\n당신은 아직 그 주술을 터득하지 못했습니다.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        }

        cp = rom_ptr->first_mon;
        while(cp) {
                if(is_enm_crt(ply_ptr->name, cp->crt)) {
                        print(fd, 
                                "\n지금 싸우고 있잖아요..!!.\n");
                        return(0);
                }
                cp = cp->next_tag;
        }

        /* Cast invisibility on self */
        if(cmnd->num == 2) {

                ply_ptr->lasttime[LT_INVIS].ltime = t;
                if(how == CAST) {
                        ply_ptr->lasttime[LT_INVIS].interval = 1200 + 
                                bonus[ply_ptr->intelligence]*600;
                        if(ply_ptr->class == MAGE)
                                ply_ptr->lasttime[LT_INVIS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 15;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
                            ply_ptr->lasttime[LT_INVIS].interval += 600L;
                        }                                
                }
                else
                        ply_ptr->lasttime[LT_INVIS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,
                                "\n당신은 소명부를 삼키면서 은둔법의 주문을 외웁니다.\n몸이 눈부실 정도로 강렬한 빛을 내다가 갑자기 사라졌습니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M이 소명부를 삼키면서  은둔법의 주문을 외웁니다.\n그의 몸이 눈부실 정도로 강렬한 빛을 내다가 갑자기 \n사라졌습니다.\n ",
                                        ply_ptr->name );
                }
                else if(how == POTION)
                        print(fd, 
                                "\n당신의 몸이 눈부실 정도로 강렬한 빛을 내다가 갑자기 사라졌습니다.\n");

                F_SET(ply_ptr, PINVIS);

                return(1);
        }

        /* Cast invisibility on another player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "\n그 물건은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, 
                                "\n그런 사람은 존재하지 않습니다.\n");
                        return(0);
                }

                crt_ptr->lasttime[LT_INVIS].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_INVIS].interval = 1200 + 
                                bonus[ply_ptr->intelligence]*600;
                        if(ply_ptr->class == MAGE)
                                crt_ptr->lasttime[LT_INVIS].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 15;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                            print(fd,
                                    "\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
                            crt_ptr->lasttime[LT_INVIS].interval += 600L;
                        }                                
                }
                else
                        crt_ptr->lasttime[LT_INVIS].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, 
                                "\n%M에게 소명부를 먹이고 은둔법의 주문을 겁니다.\n%M의 몸이 눈부실 정도로 강렬한 빛을 내다가 갑자기 \n사라졌습니다.\n",
                                    crt_ptr,crt_ptr);
                        print(crt_ptr->fd, 
                                        "\n%M이 당신에게 소명부를 먹이고 은둔법의 주문을 겁니다.\n몸이 눈부실 정도로 강렬한 빛을 내다가 갑자기 사라졌습니다.\n",
                                            ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                        "\n%M이 %M에게 소명부를 먹이고 은둔법의 주문을 겁니다.\n그의 몸이 눈부실 정도로 강렬한 빛을 내다가 갑자기 사라졌습니다.\n",
                                            ply_ptr, crt_ptr);
                        F_SET(crt_ptr, PINVIS);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              restore ( 도주천 )                         */
/**********************************************************************/

/* This function allows a player to cast the restore spell using either */
/* a potion or a wand.  Restore should not be a cast-able spell because */
/* it can restore magic points to full.                                    */

int restore(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(how == CAST && ply_ptr->class < INVINCIBLE) {
                print(fd, 
                        "\n당신은 그 주술을 사용할 능력이 없습니다.\n");
                return(0);
        }

        /* Cast restore on self */
        if(cmnd->num == 2) {

        /* 무적 자신에게 못 외움 */
        if((ply_ptr->class == INVINCIBLE) && (how == CAST)) {
                print(fd,"\n자신에게 외울수 없습니다.\n");
                return(0);
        }
                ply_ptr->hpcur += dice(2,10,0);
                ply_ptr->hpcur = MIN(ply_ptr->hpcur, ply_ptr->hpmax);

                if(mrand(1,100) < 60) {
                        ply_ptr->mpcur = ply_ptr->mpmax;

                if(how == CAST || how == WAND) {
                        print(fd, 
                                "\n당신은 공중으로 날아 올라  도주천의 주문을 외칩니다.\n천지의 기운이 번개와 폭풍을 동반하면서 몰려와 당신의\n몸에 스며들어와 도력을 회복시킵니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num,
                                        "\n%M이 공중으로 날아올라 도주천의 주문을 외칩니다.\n천지의 기운이 번개와 폭풍을 동반하면서 몰려와 도력을 \n회복시킵니다.\n", ply_ptr);
                }
                else if(how == POTION)
                        print(fd, 
                                "\n온몸에 진동이 일어나면서 도력이 충만합니다.\n");

                return(1);
		}
		else {
			if(how == CAST || how == WAND) {
				print(fd,
					"\n당신은 공중으로 날아 올라  도주천의 주문을 외칩니다.\n하지만 아무런 반응도 일어나지 않습니다.\n");
				broadcast_rom(fd, ply_ptr->rom_num,
						"\n%M이 공중으로 날아올라 도주천의 주문을 외칩니다.\n하지만 아무런 반응도 일어나지 않습니다.\n", ply_ptr);
			}
			else { if(how == POTION)
				print(fd,
					"\n도력 회복에 실패했습니다!!\n");
			}
		}
			
        }

        /* Cast restore on another player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "\n그 물건은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if((!crt_ptr) ||(crt_ptr == ply_ptr)) {
                        print(fd, 
                                "\n그런 사람은 존재하지 않습니다.\n");
                        return(0);
                }

                crt_ptr->hpcur += dice(2,10,0);
                crt_ptr->hpcur = MIN(crt_ptr->hpcur, crt_ptr->hpmax);

                if(mrand(1,100) < 60) {
                        crt_ptr->mpcur = crt_ptr->mpmax;

                	if(how == CAST || how == WAND) {
                        	print(fd, 
                                	"\n%M에게 무화연 잎을 먹이며 도주천의 주문을 외웁니다.\n그의 단전에 화기가 모이면서 도력이 회복됩니다.\n", crt_ptr);
                       		print(crt_ptr->fd, 
                                	"\n%M이 당신에게 무화연 잎을 먹이며 도주천의 주문을 \n외웁니다.\n당신의 단전에 화기가 모이면서 도력이 회복됩니다.\n",
                                     	ply_ptr);
                        	broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                        "\n%M이 %M에게 무화연 잎을 먹이며 도주천의 주문을 \n외웁니다.\n그의 도력이 회복되었습니다.\n",
                                        ply_ptr, crt_ptr);
                        	return(1);
                	}
		}
		else {
			if(how == CAST || how == WAND) {
				print(fd,
					"\n%M에게 무화연 잎을 먹이며 도주천의 주문을 외웁니다.\n하지만 아무런 반응도 일어나지 않습니다.\n", crt_ptr);
				print(crt_ptr->fd,
					"\n%M이 당신에게 무화연 잎을 먹이며 도주천의 주문을 \n외웁니다.\n하지만 아무런 반응도 일어나지 않습니다.\n",
					ply_ptr);
				broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
					"\n%M이 %M에게 무화연 잎을 먹이며 도주천의 주문을 \n외웁니다.\n하지만 아무런 반응도 일어나지 않습니다.\n",
					ply_ptr, crt_ptr);
				return(1);
			}
		}
        }

        return(1);

}

/**********************************************************************/
/*                              befuddle ( 혼동술 )                       */
/**********************************************************************/

int befuddle(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd, dur;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 10 && how == CAST) {
                print(fd, 
                        "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SBEFUD) && how == CAST) {
                print(fd, 
                        "\n당신은 아직 그 주술을 터득하지 못했습니다.\n");
                return(0);
        }

        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, 
                        "\n당신의 모습이 보이기 시작합니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M이 보이기 시작합니다.\n",
                              ply_ptr);
        }
        if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 10;
                return(0);
        }

        /* Befuddle self */
        if(cmnd->num == 2) {

                if(how == CAST) {
                        dur = bonus[ply_ptr->intelligence]*2 + 
                              dice(2,6,0) + (ply_ptr->class == MAGE ?
                              ((ply_ptr->level+3)/4)/2 : 0);
                        ply_ptr->mpcur -= 10;
                }
                else
                        dur = dice(2,6,0);

                dur = MAX(6, dur);
                ply_ptr->lasttime[LT_ATTCK].ltime = time(0);
                ply_ptr->lasttime[LT_ATTCK].interval = dur;

                if(how == CAST || how == SCROLL || how == WAND)  {
                        print(fd, 
                                "\n당신은 흑기를 땅에 꼿으며 혼동술의 일종인 흑안법을 \n자신에게 걸었습니다.\n주술을 걸자 갑자기 흑기에서 검은기류가 피어올라 당신의 \n정신을 혼수상태에 빠뜨립니다.");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                        "\n%M이 흑기를 땅에 꼿으며 혼동술의 일종인 흑안법을 \n자신에게 걸었습니다.\n주문을 걸자 갑자기 흑기에서 검은기류가 피어올라 그의 \n정신을 혼수상태에 빠뜨립니다.",ply_ptr);
                }
                else if(how == POTION)
                        print(fd, 
                                "\n당신의 온몸의 피가 역류하면서 주화입마에 빠집니다.\n혼수상태에 빠졌습니다.\n");

        }

        /* Befuddle a monster or player */
        else {

                if(how == POTION) {
                        print(fd, 
                                "\n그 물건은 자신에게만 사용할수 있습니다.\n");
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
                                print(fd, 
                                        "\n그런 사람이 존재하지 않습니다.\n");
                                return(0);
                        }

                }

                if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
                        print(fd, 
                                "\n당신은 %s를 해칠수 없습니다.\n",
                                F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                        return(0);
                }

                if(how == CAST) {
                        dur = bonus[ply_ptr->intelligence] + dice(2,6,0);
                        ply_ptr->mpcur -= 10;
                }
                else
                        dur = dice(2,5,0);

                if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
                   (crt_ptr->type   !=   PLAYER  &&   (F_ISSET(crt_ptr,   MRMAGI)   || 
F_ISSET(crt_ptr, MRBEFD))))
                        dur = 3;
                else
                        dur = MAX(5, dur);

                crt_ptr->lasttime[LT_BEFUD].ltime = time(0);
                crt_ptr->lasttime[LT_BEFUD].interval = dur;
                if(crt_ptr->type == MONSTER)
                        F_SET(crt_ptr, MBEFUD);
                crt_ptr->lasttime[LT_SPELL].ltime = time(0);
                dur = MIN(9, dur);
                crt_ptr->lasttime[LT_SPELL].interval = dur;
                if(crt_ptr->type == PLAYER) {
                        crt_ptr->lasttime[LT_ATTCK].ltime = time(0);
                        crt_ptr->lasttime[LT_ATTCK].interval = dur;
                }


                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "당신은 흑기를 땅에 꼿으며 %s에게 일종인 흑안법을 걸었습니다.\n주술을 걸자 갑자기 흑기에서 검은기류가 피어올라 그의\n정신을 혼수상태에 빠뜨립니다.\n", crt_ptr->name);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                            "\n%M이 흑기를 땅에 꼿으며 혼동술의 일종인 흑안법을 \n%M에게 걸었습니다.\n주술을 걸자 갑자기 흑기에서 검은기류가 피어올라 그의\n정신을 혼수상태에 빠뜨립니다.\n",
                                                ply_ptr, crt_ptr);
                        print(crt_ptr->fd, 
                                        "\n%M이 흑기를 땅에 꼿으며 혼동술의 일종인 흑안법을 당신에게 걸었습니다.\n주술을 걸자 갑자기 흑기에서 검은기류가 피어올라 당신의\n정신을 혼수상태에 빠뜨립니다.\n",
                                            ply_ptr);
                }

                if(crt_ptr->type != PLAYER)
                        add_enm_crt(ply_ptr->name, crt_ptr);

        }

        return(1);

}
