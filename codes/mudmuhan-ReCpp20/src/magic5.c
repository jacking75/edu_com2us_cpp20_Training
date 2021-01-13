/*
 * MAGIC5.C:
 *
 *      Additional spell-casting routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

/**********************************************************************/
/*                              recall                                 */
/**********************************************************************/

/* This function allows a player to teleport himself or another player */
/* to room #1 (Town Square)                                           */

int recall(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 30 && how == CAST) {
                print(fd, "\n당신이 도력이 부족합니다.\n");
                return(0);
        }

        if(ply_ptr->class != CLERIC && 
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n불제자만이 이 주술을 사용할 수 있습니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SRECAL) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
                return(0);
        }

        /* Cast recall on self */
        if(cmnd->num == 2) {

                if(how == CAST)
                        ply_ptr->mpcur -= 30;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "귀환 주문을 외웠습니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M이 %s 자신에게 귀환 주문을 외웠습니다.\n",
                                      ply_ptr,
                                      F_ISSET(ply_ptr, PMALES) ? "그":"그녀");
                }
                else if(how == POTION) {
                        print(fd, "당신의 모습이 어지러이 흔들립니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num,
                                "%M이 사라졌습니다.", ply_ptr);
                }

                if(load_rom(1001, &new_rom) < 0) {
                        print(fd, "주문이 실패했습니다.\n");
                        return(0);
                }

                del_ply_rom(ply_ptr, rom_ptr);
                add_ply_rom(ply_ptr, new_rom);

                return(1);
        }

        /* Cast word of recall on another player */
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
                        ply_ptr->mpcur -=30;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "귀환 주문을 %M에게 외웠습니다.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "%M이 당신에게 귀환 주문을 외웠습니다.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "%M이 %M에게 귀환 주문을 외웠습니다.",
                                       ply_ptr, crt_ptr);

                        if(load_rom(1, &new_rom) < 0) {
                                print(fd, "주문이 실패했습니다.\n");
                                return(0);
                        }

                        del_ply_rom(crt_ptr, rom_ptr);
                        add_ply_rom(crt_ptr, new_rom);

                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              summon                              */
/**********************************************************************/

/* This function allows players to cast summon spells on anyone who is */
/* in the game, taking that person to your current room.         */

int summon(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd, n;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class == INVINCIBLE || ply_ptr->class == CARETAKER) {
		if(ply_ptr->mpcur < 100 && how == CAST) {
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
                }
        }
        else {
        	if(ply_ptr->mpcur < 50 && how == CAST) {
        	print(fd, "\n당신의 도력이 부족합니다.\n");
        	return(0);
        	}
        }

        if(!S_ISSET(ply_ptr, SSUMMO) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }
		if(mrand(1,100) < 51) {
				print(fd, "\n소환에 실패를 하였습니다.\n");
				ply_ptr->mpcur -= 50;
				return(0);
		}

        if(cmnd->num == 2) {
                print(fd, "\n자신을 소환하다뇨?.\n");
                return(0);
        }

        else {

                if(how == POTION) {
                        print(fd, "\n그 물건은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_who(cmnd->str[2]);

                if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
                        print(fd, 
                              "\n그런 사람을 못 찾습니다.\n");
                        return(0);
                }

                if(how == CAST) { 
                	if(ply_ptr->class == INVINCIBLE || ply_ptr->class == CARETAKER)
                        	ply_ptr->mpcur -= 100;
                        else 	ply_ptr->mpcur -= 50;
                }

                n = count_vis_ply(rom_ptr);
                if(F_ISSET(rom_ptr, RNOTEL) ||
                  (F_ISSET(rom_ptr, RONEPL) && n > 0) ||
                  (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
                  (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
                        print(fd, "주문이 공중으로 빨려듭니다.\n");
                        return(0);
                }
                if(F_ISSET(rom_ptr, RFAMIL) && !F_ISSET(crt_ptr, PFAMIL)) {
                	print(fd, "그사람은 패거리 가입자가 아닙니다.");
                	return(0);
                }
                if(F_ISSET(rom_ptr, RONFML) && (crt_ptr->daily[DL_EXPND].max != rom_ptr->special)) {
                	print(fd, "그사람은 이곳에 올수 없습니다.");
                	return(0);
                }

                if(rom_ptr->lolevel > crt_ptr->level ||
                   (crt_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel) ||
                   F_ISSET(crt_ptr, PNOSUM)) {
                        print(fd, "\n주문이 실패했습니다.\n");
                        if(F_ISSET(crt_ptr,PNOSUM)) 
                            print(fd,"상대가 소환 거부 중입니다.");
                        return(0);
                }

                if(F_ISSET(crt_ptr->parent_rom,RNOLEA))
                {
                        print(fd, "\n소환 주문이 %M을 찾지 못합니다.\n",crt_ptr);
                        return(0);
                }

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n당신은 %M을 소환하기 위해 주문을 외웁니다.\n주문을 마치자 짙은 안개가 끼더니 갑자기 사라지면서 \n그가 나타났습니다.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n당신주위에 짙은 안개가 끼더니 알 수 없는 힘에 이끌려 어디론가 날라갑니다.\n안개가 걷히자 %M이 당신앞에 서 있습니다.\n",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 소환주문을 외우자 짙은 안개가 깔리더니 갑자기 %M이 나타났습니다.\n",
                                       ply_ptr, crt_ptr);

                        del_ply_rom(crt_ptr, crt_ptr->parent_rom);
                        add_ply_rom(crt_ptr, rom_ptr);

                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              heal(완치술)                                   */
/**********************************************************************/

/* This function will cause the heal spell to be cast on a player or    */
/* another monster.  It heals all hit points damage but only works 3      */
/* times a day.                                                         */

int heal(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd, heal;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 20 && how == CAST) {
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n불제자와 무사만이 이 주술을 사용할 수 있습니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SFHEAL) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }

        /* Heal self */
        if(cmnd->num == 2 ) {

                if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
                   ply_ptr->class < CARETAKER) {
                        print(fd, "\n당신의 몸이 너무 피곤해 이 주술을 더 이상 펼칠 수 없습니다.\n");
                        return(0);
                }

                ply_ptr->hpcur = ply_ptr->hpmax;

                if(how == CAST) 
                        ply_ptr->mpcur -= 20;

                if(how == CAST || how == SCROLL) {
                        print(fd, "\n당신은 천부공을 끌어올리며 완치 주문을 외웁니다.\n천상의 기운들이 당신의 몸으로 모이면서 체력을 최상으로 \n올려 줍니다.\n");
                        broadcast_rom(fd, ply_ptr->rom_num, 
                                      "\n%M이 천부공 자세를 취하면서 완치주문을 외웠습니다.\n천상의 기운들이 그에게로 모이는 것이 느껴집니다.\n",
                                      ply_ptr);
                        return(1);
                }
                else {
                        print(fd, "\n갑자기 몸이 펴지면서 몸의 체력이 최고의 상태로 올라갑니다.\n\"이야~~얍.. 힘이 넘친다.\"\n");
                        return(1);
                }
        }

        /* Cast heal on another player or monster */
        else {

                if(how == POTION) {
                        print(fd, "그 물건은 자신에게만 사용가능합니다.\n");
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
                                print(fd, "\n그런 사람이 존재하지 습니다.\n");
                                return(0);
                        }
                }

                if(!dec_daily(&ply_ptr->daily[DL_FHEAL]) && how == CAST &&
                   ply_ptr->class < CARETAKER && ply_ptr->type != MONSTER) {
                        print(fd, "\n당신의 몸이 너무 피곤해 이 주술을 더 이상 펼칠 수 없습니다.\n");
                        return(0);
                }

                crt_ptr->hpcur = crt_ptr->hpmax;

                if(how == CAST) 
                        ply_ptr->mpcur -= 20;
                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "당신은 %M에게 완치부적을 먹이며 주문을 외웁니다.\n갑자기 그의 몸에서 심한 진동이 일어나면서 체력이 \n회복되는 것이 느껴집니다.", crt_ptr);
                        print(crt_ptr->fd, "%M이 당신에게 완치부적을 먹이며 주문을 외웁니다.\n갑자기 당신의 몸에서 심한 진동이 일어나면서 체력이 \n회복되는 것이 느껴집니다.",
                              ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "%M이 %M에게 완치부적을 먹이며 주문을 외웁니다.\n그의 몸이 심한 진동이 일으키다가 체력이 회복었는지\n펄쩍펄쩍 뛰어 다닙니다.\n",
                                       ply_ptr, crt_ptr);
                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              track(추적)                                    */
/**********************************************************************/

/* This function allows rangers to cast the track spell which takes them */
/* to any other player in the game.                                       */

int magictrack(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd, n;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->class != RANGER && ply_ptr->class < INVINCIBLE &&
           how==CAST) {
                print(fd, "\n포졸만이 이 주술을 사용할 수 있습니다.\n");
                return(0);
        }

        if(ply_ptr->mpcur < 13 && how == CAST) {
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, STRACK) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }

        if(cmnd->num == 2) {
                print(fd, "\n당신 자신을 추적한다고요?.\n");
                return(0);
        }

        else {

                if(how == POTION) {
                        print(fd, "\n그 물건은 자신에게만 사용할수 있습니다.\n");
                        return(0);
                }

                cmnd->str[2][0] = up(cmnd->str[2][0]);
                crt_ptr = find_who(cmnd->str[2]);

                if(!crt_ptr || crt_ptr == ply_ptr || F_ISSET(crt_ptr, PDMINV)) {
                        print(fd, 
                              "\n그런 사람은 존재하지 않습니다.\n");
                        return(0);
                }

                if(crt_ptr->class > CARETAKER) {
                        print(fd, "\n그 사람이 어디에 있는지 추적할수 없습니다.\n");
                        return(0);
                }
/*                
                if(F_ISSET(crt_ptr, PMARRI)) {
                	print(fd, "\n그 사람이 어디에 있는지 추적할 수 없습니다.\n");
                	return(0);
                }
*/

                n = count_vis_ply(crt_ptr->parent_rom);
                if(F_ISSET(crt_ptr->parent_rom, RNOTEL) ||
                  (F_ISSET(crt_ptr->parent_rom, RONEPL) && n > 0) ||
                  (F_ISSET(crt_ptr->parent_rom, RTWOPL) && n > 1) ||
                  (F_ISSET(crt_ptr->parent_rom, RTHREE) && n > 2)) {
                        print(fd, "\n주문이 실패했습니다.\n");
                        return(0);
                }
                if(F_ISSET(crt_ptr->parent_rom, RFAMIL) && !F_ISSET(ply_ptr, PFAMIL)) {
                	print(fd, "그 사람이 있는 곳으로 갈 수가 없습니다.");
                	return(0);
                }
                if(F_ISSET(crt_ptr->parent_rom, RONFML) && (ply_ptr->daily[DL_EXPND].max != crt_ptr->parent_rom->special)) {
                	print(fd, "그 사람이 있는 곳으로 갈 수가 없습니다.");
                	return(0);
                }

                if(crt_ptr->parent_rom->lolevel > ply_ptr->level ||
                   (ply_ptr->level > crt_ptr->parent_rom->hilevel &&
                   crt_ptr->parent_rom->hilevel)) {
                        print(fd, "\n주문이 실패했습니다.\n");
                        return(0);
                }

                if(!dec_daily(&ply_ptr->daily[DL_TRACK]) && how == CAST &&
                   ply_ptr->class < CARETAKER) {
                        print(fd, "\n당신의 몸이 너무 피곤해 이 주술을 더 이상 펼칠 수 없습니다.\n");
                        return(0);
                }

                if(how == CAST)
                        ply_ptr->mpcur -= 13;

                if(how == CAST || how == SCROLL || how == WAND) {
                        print(fd, "\n!!당신은 %M의 흔적을 찾아내는데 성공했습니다.!!\n그를 추적하여 달려갑니다.\n", crt_ptr);
                        print(crt_ptr->fd, 
                              "\n%M이 당신의 흔적을 찾아 내는데 성공하여 당신을 \n찾아 왔습니다.\n", ply_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 %M의 흔적을 찾아내는데 성공하여 \n추적을 시작했습니다.\n", ply_ptr, crt_ptr);

                        del_ply_rom(ply_ptr, rom_ptr);
                        add_ply_rom(ply_ptr, crt_ptr->parent_rom);

                        return(1);
                }
        }

        return(1);

}

/**********************************************************************/
/*                              levitate(부양술)                        */
/**********************************************************************/

/* This function allows players to cast the levitate spell, thus allowing */
/* them to levitate over traps or up mountain cliffs.                        */

int levitate(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 10 && how == CAST) {
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SLEVIT) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 10;
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_LEVIT].ltime = time(0);
                F_SET(ply_ptr, PLEVIT);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M이 기공을 끌어 모으며 부양술의 주문을 외우자\n몸이 살짝 떠오릅니다.", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n당신은 기공을 모으며 부양술의 주문을 외우자 몸이\n살짝 떠오릅니다.\n");
                        ply_ptr->mpcur -= 10;
                        ply_ptr->lasttime[LT_LEVIT].interval = 2400L +
                                bonus[ply_ptr->intelligence]*600L;
if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            ply_ptr->lasttime[LT_LEVIT].interval += 800L;
        }                                
                }
                else {
                        print(fd, "\n당신의 몸이 공중으로 살짝 떠오릅니다.\n");
                        ply_ptr->lasttime[LT_LEVIT].interval = 1200L;
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
                        print(fd, "\n그런 사람은 존재하지 않습니다.\n");
                        return(0);
                }

                F_SET(crt_ptr, PLEVIT);
                crt_ptr->lasttime[LT_LEVIT].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "\n%M이 %M에게 부양부적을 붙히며 주문을 외웁니다.\n주문을 외우자 그의 몸이 살짝 떠오릅니다.\n", ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M이 당신에게 부양부적을 붙히며 주문을 외웁니다.\n당신의 몸이 살짝 떠오르기 시작 합니다.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "\n당신은 %M에게 부양술을 걸 수 없습니다.\n",crt_ptr);
                        ply_ptr->mpcur -= 10;
                        crt_ptr->lasttime[LT_LEVIT].interval = 2400L +
                                bonus[ply_ptr->intelligence]*600L;
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
                  crt_ptr->lasttime[LT_LEVIT].interval += 800L;
                }                                
                }

                else {
                        print(fd, "%M이 떠다니기 시작합니다.\n", crt_ptr);
                        crt_ptr->lasttime[LT_LEVIT].interval = 1200L;
                }

                return(1);
        }

}

/************************************************************************/
/*                              resist_fire(방열진)                                */
/************************************************************************/

/* This function allows players to cast the resist fire spell.  It will        */
/* allow the player to resist fire breathed on them by dragons and        */
/* other breathers.                                                       */

int resist_fire(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr, *new_rom;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 12 && how == CAST) {
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SRFIRE) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 12;
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_RFIRE].ltime = time(0);
                F_SET(ply_ptr, PRFIRE);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M이 방열진의 금패를 들어올리면서 주문을 외웁습니다.\n오행중 수의 수호령들이 나타나 그의 주위에 진을 형성합니다.\n", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n당신은 방열진의 금패를 들어올리면서 주문을 외웁니다.\n오행중 수의 수호령들이 나타나 당신주위에서 진을 형성합니다.\n");
                        ply_ptr->mpcur -= 12;
                        ply_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
                  ply_ptr->lasttime[LT_RFIRE].interval += 800L;
                }                                
                }
                else {
                        print(fd, "\n갑자기 오행중 수의 수호령들이 나타나 당신주위에 \n진을 형성합니다.\n");
                        ply_ptr->lasttime[LT_RFIRE].interval = 1200L;
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

                F_SET(crt_ptr, PRFIRE);
                crt_ptr->lasttime[LT_RFIRE].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "%M이 %M에게 방열부적을 붙이며 주문을 외웁니다.\n오행중 수의 수호령들이 나타나 그의 주위에 진을 형성합니다.\n", 
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M이 당신에게 방열부적을 붙이며 주문을 외웁니다.\n갑자기 오행중 수의 수호령들이 나타나 당신주위에 \n진을 형성합니다.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "당신은 %M에게 방열부적을 붙이며 주문을 외웁니다.\n오행중 수의 수호령들이 나타나 그의 주위에 진을 형성합니다.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 12;
                        crt_ptr->lasttime[LT_RFIRE].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n이방의 기운이 당신의 주문을 강화시킵니다.\n");
                  crt_ptr->lasttime[LT_RFIRE].interval += 800L;
                }                                
                }

                else {
                        print(fd, "\n오행중 수의 수호령들이 나타나 %M의 주위에 \n진을 형성합니다.\n", crt_ptr);
                        crt_ptr->lasttime[LT_RFIRE].interval = 1200L;
                }

                return(1);
        }

}


/************************************************************************/
/*                              fly(비상술)                              */
/************************************************************************/

/* This function allows players to cast the fly spell.  It will              */
/* allow the player to fly to areas that are otherwise unreachable */
/* by foot.                                                              */

int fly(ply_ptr, cmnd, how)
creature *ply_ptr;
cmd            *cmnd;
int              how;
{
        creature *crt_ptr;
        room           *rom_ptr;
        int              fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        if(ply_ptr->mpcur < 15 && how == CAST) {
                print(fd, "\n당신의 도력이 부족합니다.\n");
                return(0);
        }

        if(!S_ISSET(ply_ptr, SFLYSP) && how == CAST) {
                print(fd, "\n당신은 아직 그런 주술을 터득하지 못했습니다.\n");
                return(0);
        }
        if(spell_fail(ply_ptr)) {
                if(how == CAST)
                     ply_ptr->mpcur -= 15;
                return(0);
        }

        if(cmnd->num == 2) {
                ply_ptr->lasttime[LT_FLYSP].ltime = time(0);
                F_SET(ply_ptr, PFLYSP);
                broadcast_rom(fd, ply_ptr->rom_num, 
                        "\n%M이 비상술의 주문을 외우자 몸이 떠오르며 \n하늘로 날기 시작합니다.\n", ply_ptr);
                if(how == CAST) {
                        print(fd, "\n당신은 비상술의 주문을 외우자 몸이 공기처럼 가벼워지며\n하늘로 떠올라 날기 시작합니다.\n");
                        ply_ptr->mpcur -= 15;
                        ply_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
                  ply_ptr->lasttime[LT_FLYSP].interval += 600L;
                }                                
                }
                else {
                        print(fd, "당신의 몸이 하늘로 떠오르며 날수 있습니다!\n");
                        ply_ptr->lasttime[LT_FLYSP].interval = 1200L;
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

                F_SET(crt_ptr, PFLYSP);
                crt_ptr->lasttime[LT_FLYSP].ltime = time(0);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
                        "\n%M이 %M에게 비상부를 붙히며 주문을 외웠습니다.\n그의 몸이 하늘로 떠오르며 날기 시작합니다.\n", 
                        ply_ptr, crt_ptr);
                print(crt_ptr->fd, "\n%M이 당신에게 비상부를 붙히며 주문을 외웠습니다.\n갑자기 당신의 몸이 공기처럼 가벼워지며 하늘로 떠올라\n날기 시작합니다.\n", ply_ptr);

                if(how == CAST) {
                        print(fd, "\n당신은 %M에게 비상부를 붙히며 주문을 외웁니다.\n그의 몸이 하늘로 떠오르며 날기 시작합니다.\n",
                                crt_ptr);
                        ply_ptr->mpcur -= 15;
                        crt_ptr->lasttime[LT_FLYSP].interval = MAX(300, 1200 +
                                bonus[ply_ptr->intelligence]*600);
                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                  print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
                  crt_ptr->lasttime[LT_FLYSP].interval += 600L;
                }                                
                }

                else {
                        print(fd, "%M의 몸이 하늘로 떠오르며 날기 시작합니다.\n", crt_ptr);
                        crt_ptr->lasttime[LT_FLYSP].interval = 1200L;
                }

                return(1);
        }

}
