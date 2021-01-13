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
/*                              detectinvis(은둔감지술)                         */
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
                        "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SDINVI) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
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
                        "\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            ply_ptr->lasttime[LT_DINVI].interval += 600L;
        }                                
                }
                else
                        ply_ptr->lasttime[LT_DINVI].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,"\n당신은 버들잎을 두눈에 비비며 은둔감지술의 주문을\n외웁니다.\n두눈에 푸른광안이 떠오르며 숨어있는 자들을 볼수\n있게되었습니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                 "\n%M이 버들잎을 두눈에 비비며 자신에게 은둔감지술의\n주문을 외웠습니다.\n 그의 눈에서 푸른광안이 떠오릅니다.\n",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n.갑자기 두눈에 푸른광안이 떠오르며 숨어있는 자들을 볼수 있게 되었습니다.\n");

                F_SET(ply_ptr, PDINVI);

                return(1);
        }

        /* Cast detect-invisibility on another player */
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

                crt_ptr->lasttime[LT_DINVI].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_DINVI].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == MAGE)
                                crt_ptr->lasttime[LT_DINVI].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            crt_ptr->lasttime[LT_DINVI].interval += 600L;
        }                                
                }
                else
                        crt_ptr->lasttime[LT_DINVI].interval = 1200;

                F_SET(crt_ptr, PDINVI);

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n당신은 %M의 인당혈을 찍으며 은둔감지술을 외웁니다.\n그의 눈에서 푸른광안이 떠오릅니다.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M이 당신의 인당혈을 찍으며 은둔감지술을 외웠습니다.\n갑자기 두눈에 푸른광안이 떠오르며 숨어있는 자들을 볼수\n있게 되었습니다.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 %M의 인당혈을 찍으며 은둔감지술을 외웁니다.\n그의 눈에서 푸른광안이 떠오릅니다.\n",
                                       ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              detectmagic(주문감지술)                */
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
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SDMAGI) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
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
            print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            ply_ptr->lasttime[LT_DMAGI].interval += 600L;
        }                                
                }
                else
                        ply_ptr->lasttime[LT_DMAGI].interval = 1200;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd,"\n당신은 됴화잎을 눈에 비비며 주문감지술을 외웁니다.\n당신의 눈에서 은빛광안이 떠오르며 주술에 관한 안목이\n넓어졌습니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M이 됴화잎을 눈에 비비며 주문감지술을 외웠습니다.\n갑자기 그의 눈에서 은빛광안이 떠오릅니다.\n",
                                      ply_ptr);
                }
                else if(how == POTION)
                        print(fd, "\n갑자기 두눈에 은빛광안이 떠오르며 주술에 관한 안목이 \n넓어졌습니다.\n");

                F_SET(ply_ptr, PDMAGI);

                return(1);
        }

        /* Cast detect-magic on another player */
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

                crt_ptr->lasttime[LT_DMAGI].ltime = t;
                if(how == CAST) {
                        crt_ptr->lasttime[LT_DMAGI].interval = MAX(300, 1200 + 
                                bonus[ply_ptr->intelligence]*600);
                        if(ply_ptr->class == MAGE)
                                crt_ptr->lasttime[LT_DMAGI].interval += 
                                60*((ply_ptr->level+3)/4);
                        ply_ptr->mpcur -= 10;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            crt_ptr->lasttime[LT_DMAGI].interval += 600L;
        }                                
                }
                else
                        crt_ptr->lasttime[LT_DMAGI].interval = 1200;

                F_SET(crt_ptr, PDMAGI);

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n당신은 %M의 백회혈을 찍으며 주문감지술의 \n주문을 외웁니다.\n갑자기 그의 두눈에 은빛광안이 떠오릅니다.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M이 당신의 백회혈을 찍으며 주문감지술의 \n주문을 외웁니다.\n갑자기 두눈에 은빛광안이 떠오르며 주술에 관한 안목이 넓어졌습니다.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 %M의 백회혈을 찍으며 주문감지술의 \n주문을 외웁니다.\n갑자기 그의 두눈에 은빛광안이 떠오릅니다.\n.",
                                       ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              teleport(공간이동술)                            */
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
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, STELEP) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
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
                              "\n%M이 공간이동술을 사용하여 어디론가로 사라졌습니다.\n", ply_ptr);
                if(how == CAST || how == SCROLL)
                        print(fd, "\n당신은 공간이동술을 사용하기위해 발구름질을 시작합니다.\n공간이동술의 주문을 외우자 몸이 총알같이 빨라지면서\n어디론가로 날라갔습니다.\n");
                else
                        print(fd, "\n공간이동술의 주문을 외우자 몸이 총알같이 빨라지면서\n어디론가로 날라갔습니다.\n");

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
                        print(fd, "\n그 물건은 자신에게만 사용가능합니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[2], cmnd->val[2]);

                if(!crt_ptr) {
                        print(fd, "\n그런 사람이 존재하지 않습니다.\n");
                        return(0);
                }
                if(F_ISSET(crt_ptr, PRMAGI) && ply_ptr->level < 128 && ((((ply_ptr->level+3)/4)-((crt_ptr->level+3)/4))*10) < 50) {
                        print(fd, "\n%M을 공간이동 시키기엔 당신의 주문이 너무 약합니다.\n", crt_ptr);
                        print(crt_ptr->fd, "\n%M이 공간이동술을 사용하여 당신을 이동 시키려 합니다.\n", ply_ptr);
                                if(how == CAST)
                                ply_ptr->mpcur -= 20;
                        return(0);
                }

                if(how == CAST)
                        ply_ptr->mpcur -=20;
 
                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n공간이동술 주문을 %M에게 외웁니다.\n그의 몸이 안개에 휩싸이며 모습이 사라졌습니다.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M이 당신에게 공간이동술 주문을 외웠습니다.\n당신의 몸이 안개에 휩싸이며 어디론가로 이동됩니다.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 %M에게 공간이동술 주문을 외웠습니다.\n그의 몸이 안개에 휩싸이며 모습이 사라졌습니다.\n",
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
/*                              enchant(주술)                                  */
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
                print(fd, "\n도술사들만이 주술을 걸수있습니다.\n");
                return(0);
        }

        if(ply_ptr->mpcur < 25 && how == CAST) {
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SENCHA) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }

        if(cmnd->num < 3) {
                print(fd, "\n무엇에다 주술을 겁니다?\n");
                return(0);
        }

        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[2], cmnd->val[2]);

        if(!obj_ptr) {
                print(fd, "\n당신 소지품에 그런 물건이 없습니다.\n");
                return(0);
        }

        if(F_ISSET(obj_ptr, OENCHA)) {
                print(fd, "\n벌써 주술이 걸려있습니다.\n");
                return(1);
        }

        if(how == CAST) {
                if(!dec_daily(&ply_ptr->daily[DL_ENCHA]) && 
                   ply_ptr->class < CARETAKER) {
                        print(fd, "\n당신은 탈진해서 더 이상 주술을 걸수 없습니다.\n좀 쉬어야겠는데요?\n");
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

        print(fd, "\n%i에다가 주술을 걸자 갑자기 밝은 빛을 내다가 사라졌습니다.\n", obj_ptr);
        broadcast_rom(fd, rom_ptr->rom_num, "%M이 %1i에다가 주술을 걸었습니다.", ply_ptr,
                      obj_ptr);

        return(1);
}
