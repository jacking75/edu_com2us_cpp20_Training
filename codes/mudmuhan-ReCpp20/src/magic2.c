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
/*              vigor  ( 회복 )                   */
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
        print(fd, "당신의 도력이 부족합니다.\n");
        return(0);
    }

    if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && ply_ptr->class < INVINCIBLE && how == CAST) {
	    print(fd, "불제자와 무사만 쓸 수 있는 마법입니다.\n");
	    return(0);
    }

    if(!S_ISSET(ply_ptr, SVIGOR) && how == CAST) {
        print(fd, "당신은 아직 그 주술을 터득하지 못했습니다.\n");
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
                print(fd, "이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            }
        }
        else
            heal = mrand(1,6);

        heal = MAX(1, heal);

        ply_ptr->hpcur += MAX(1, heal);

        if(ply_ptr->hpcur > ply_ptr->hpmax)
            ply_ptr->hpcur = ply_ptr->hpmax;


        if(how == CAST || how == SCROLL) {
            print(fd, "당신은 합장을 하고서 회복 주문을 외웁니다.\n빛의 정기가 온몸에 스며들면서 체력이 향상되었습니다.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                  "\n%M이 합장을 하고서 주문을 외웁니다.\n빛의 정기가 그의 몸으로 모이는 것이 보입니다.\n",
                  ply_ptr);
            return(1);
        }
        else {
            print(fd, "당신의 체력이 향상되었습니다.\n");
            return(1);
        }
    }

    /* Cast vigor on another player or monster */
    else {
        if(how == POTION) {
            print(fd, "그 물건은 자신에게만 사용할 수 있습니다.\n");
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
                print(fd,"그런 사람은 존재하지 않습니다.\n");
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
                print(fd,"이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
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
/*  회복 사용시 경험치를 주는 부분.
        ply_ptr->experience += expadd; 
    일단은 루틴에 넣지 않았음.  */
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "당신은 합장을 하고서 %M의 회복을 기원하는 주문을 외웁니다.\n빛의 정기가 그의 몸으로 스며들고 있습니다.\n",
                    crt_ptr);
            print(crt_ptr->fd,"%M이 합장을 하고서 당신의 회복을 기원하는 주문을 외웁니다.\n빛의 정기가 당신의 몸에 스며들면서 체력이 향상되었습니다.\n",
                            ply_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                  "\n%M이 %M에게 합장을 하고서 회복을 기원하는 주문을 외웁니다.\n빛의 정기가 당신의 몸을 스치며 그의 몸으로 모이는\n것이 느껴집니다.\n",
                  ply_ptr, crt_ptr);
            return(1);
        }
    }

    return(1);

}

/**********************************************************************/
/*              curepoison   ( 해독 )             */
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
        print(fd,"당신의 도력이 부족합니다.\n");
        return(0);
    }

/*  if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && ply_ptr->class < INVINCIBLE && how == CAST) {
	    print(fd, "불제자와 무사만 쓸 수 있는 마법입니다.\n");
	    return(0);
	}  */

    if(!S_ISSET(ply_ptr, SCUREP) && how == CAST) {
        print(fd, "당신은 아직 그 주술을 터득하지 못했습니다.\n");
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
            print(fd,"당신은 오른손으로 혈도를 짚으면서 해독 주문을 외웁니다.\n손가락 끝으로 검은 독기운이 빠져나오는것이 보입니다.\n");
            print(fd,"당신 몸에 남아 있는 독이 모두 빠져나갔습니다.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                   "\n%M이 혈도를 짚으면서 해독 주문을 외웁니다.\n그의 손가락 끝으로 검은 독기운이 빠져나오는 것이 보입니다.\n",
                   ply_ptr);
        }
        else if(how == POTION && F_ISSET(ply_ptr, PPOISN))
            print(fd, "\n독기운이 중화되는 것을 느낄 수 있습니다.\n");
        else if(how == POTION)
            print(fd, "\n해독에 실패하셨습니다.\n");

        F_CLR(ply_ptr, PPOISN);

    }

    /* Cure a monster or player */
    else {

        if(how == POTION) {
            print(fd,"그 물건은 자신에게만 사용할 수 있습니다.\n");
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
                print(fd,"그런 것은 존재하지 않습니다.\n");
                return(0);
            }
        }

        if(how == CAST) 
            ply_ptr->mpcur -= 6;

        F_CLR(crt_ptr, PPOISN);

        if(how == CAST || how == SCROLL || how == WAND) {
           print(fd,"%M이 혈도를 짚으면서 해독 주문을 외웁니다.\n그의 손가락 끝으로 검은 독기운이 빠져나오는 것이 보입니다.\n",
                        crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                  "\n%M이 %M의 혈도를 짚으면서 해독 주문을 외웁니다.\n그의 손가락 끝으로 검은 독기운이 빠져나오는 것이 보입니다.\n",
                  ply_ptr, crt_ptr);
            print(crt_ptr->fd,
                "%M이 당신의 혈도를 짚으면서 해독 주문을 외웁니다.\n당신의 손가락 끝으로 독기운이 빠져나가는 것이 느껴집니다.\n",
                ply_ptr);
        }

    }

    return(1);

}

/**********************************************************************/
/*              light  ( 발광 )                  */
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
        print(fd, "당신의 도력이 부족합니다.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SLIGHT) && how == CAST) {
        print(fd,"당신은 아직 그 주술을 터득하지 못했습니다.\n");
        return(0);
    }
    if(spell_fail(ply_ptr)) {
        if(how==CAST)
                ply_ptr->mpcur -= 5;
        return(0);
    }

    if(how == CAST){
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT))
        print(fd,"이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
        ply_ptr->mpcur -= 5;
    }

    F_SET(ply_ptr, PLIGHT);
    ply_ptr->lasttime[LT_LIGHT].ltime = time(0);
    ply_ptr->lasttime[LT_LIGHT].interval = 300L+((ply_ptr->level+3)/4)*300L +
        (F_ISSET(ply_ptr->parent_rom,RPMEXT)) ? 600L : 0;

    print(fd,"당신의 왼손에 발광 주문을 걸었습니다.\n왼손에서 황금빛이 뿜어져 나와 주위를 밝혀 줍니다.\n");
    broadcast_rom(fd, ply_ptr->rom_num,
            "\n%M이 한쪽 손에 발광 주문을 걸었습니다.\n그의 손에서 황금색의 찬란한 빛이 뿜어져 나옵니다.\n",
            ply_ptr);

    return(1);

}

/**********************************************************************/
/*              protection (수호진)               */
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
        print(fd,"당신의 도력이 부족합니다.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SPROTE) && how == CAST) {
        print(fd,"당신은 아직 그 주술을 터득하지 못했습니다.\n");
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
            print(fd,"이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            ply_ptr->lasttime[LT_PROTE].interval += 800L;
        }
        }
        else
            ply_ptr->lasttime[LT_PROTE].interval = 1200;

        if(how == CAST || how == SCROLL || how == WAND)  {
            print(fd, "두손으로 인을 맺은 뒤 수호진의 주문을 외웁니다.\n빛의 수호령들이 당신 주위를 둘러싸며 방어의 진을 형성했습니다.\n");
            broadcast_rom(fd, ply_ptr->rom_num,
                  "\n%M이 두손을 모으며 수호진의 주문을 외웁니다.\n빛의 수호령들이 그의 주위를 둘러싸며 방어의 진을 형성했습니다.\n",
                  ply_ptr);
        }
        else if(how == POTION)
            print(fd, "\n빛의 수호령들이 당신 주위를 둘러싸며 방어의 진을 형성했습니다.\n");
        return(1);
    }

    /* Cast protection on another player */
    else {

        if(how == POTION) {
            print(fd, "그 물건은 자신에게만 사용할 수 있습니다.\n");
            return(0);
        }

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);

        if(!crt_ptr) {
            print(fd, "그런 사람은 존재하지 않습니다.\n");
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
            print(fd, "이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            crt_ptr->lasttime[LT_PROTE].interval += 800L;
        }
        }
        else
            crt_ptr->lasttime[LT_PROTE].interval = 1200;

        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd,"%M의 몸에 수호인을 그리며 수호진의 주문을 걸었습니다.\n빛의 수호령들이 그의 주위를 둘러싸며 방어의 진을 형성했습니다.\n",
                    crt_ptr);
            print(crt_ptr->fd, 
                    "%M이 당신의 몸에 수호인을 그리며 주문을 걸었습니다.\n빛의 수호령들이 당신의 주위를 둘러싸며 방어의 진을 형성했습니다.\n",
                    ply_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                    "%M이 %M의 몸에 수호인을 그리며 주문을 걸었습니다.\n빛의 수호령들이 그의 주위를 둘러싸며 방어의 진을 형성했습니다.\n",
                    ply_ptr, crt_ptr);
            return(1);
        }
    }

    return(1);

}

/**********************************************************************/
/*              mend (원기회복)                     */
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
        print(fd,"당신의 도력이 부족합니다.\n");
        return(0);
    }

/*  if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN && ply_ptr->class < INVINCIBLE && how == CAST) {
	    print(fd, "불제자와 무사만 쓸 수 있는 마법입니다.\n");
	    return(0);
    }  */


    if(!S_ISSET(ply_ptr, SMENDW) && how == CAST) {
        print(fd,"당신은 아직 그 주술을 터득하지 못했습니다.\n");
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
                print(fd,"이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            }
        }
        else
            heal = dice(2,6,0);

        heal = MAX(1, heal);

        ply_ptr->hpcur += MAX(1, heal);

        if(ply_ptr->hpcur > ply_ptr->hpmax)
            ply_ptr->hpcur = ply_ptr->hpmax;


        if(how == CAST || how == SCROLL) {
            print(fd,"당신은 기공팔식의 자세를 취하며 원기회복의 주문을 외웁니다.\n지기의 뜨거운 기운이 당신의 몸에 가득차 체력을 향상시킵니다.\n");
            broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M이 기공팔식의 자세를 취하며 원기회복의 주문을 외웁니다.\n지기의 뜨거운 기운이 그에게 흘러가는 것이 느껴집니다.\n",
                ply_ptr);
            return(1);
        }
        else {
            print(fd,"몸의 체력이 많이 회복되었습니다.\n");
            return(1);
        }
    }

    /* Cast mend on another player or monster */
    else {

        if(how == POTION) {
            print(fd,"그 물건은 자신에게만 사용할 수 있습니다.\n");
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
                print(fd,"그러한 사람은 존재하지 않습니다.\n");
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
                print(fd,"이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
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
/*   원기 사용시 경험치를 주는 부분.
        ply_ptr->experience += expadd; 
     일단은 넣지 않았어요.   */
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "당신은 %M에게 내공을 주입하며 원기회복의 주문을 겁니다.\n그의 몸안에서 지기의 뜨거운 기운이 느껴집니다.", crt_ptr);
            print(crt_ptr->fd,"%M이 당신에게 내공을 주입하며 원기회복의 주문을 겁니다.\n당신의 몸안에서 지기의 뜨거운 기운과 체력이 많이 향상되는\n것이 느껴집니다.\n",ply_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
               "\n%M이 %M에게 내공을 주입하며 원기회복의 주문을 겁니다.\n그에게 뜨거운 지기의 기운이 흘러가는 것이 느껴집니다.\n",
               ply_ptr, crt_ptr);
            return(1);
        }
    }

    return(1);

}
