/*
 *      MAGIC8.C:
 *
 *      Additional spell-casting routines.
 *
 */

#include "mstruct.h"
#include "mextern.h"

/***********************************************************************/
/*                              room_vigor(전회복)                        */
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
                print(fd, "주문이 실패했습니다.");
                return(0);
        }
        if(!S_ISSET(ply_ptr, SRVIGO) && (how != WAND || how != SCROLL)) {
                print(fd, "당신은 아직 그런 주문을 터득하지 못했습니다.");
                return(0);
        }  
        if(ply_ptr->class != CLERIC && ply_ptr->class < INVINCIBLE) {
                print(fd, "이 주술은 불제자만이 사용할 수 있습니다.");
                                return(PROMPT);
        }
        if(ply_ptr->mpcur < 12 && (how != WAND || how != SCROLL)) {
                print(fd, "당신의 도력이 부족합니다");
                                return(0);
        }
        if(how == CAST)
                ply_ptr->mpcur-=12;

        if(spell_fail(ply_ptr)) {
                return(0);
        }

                cp = ply_ptr->parent_rom->first_ply;

                print(fd,"당신은 가부좌를 틀고서 전회복 주문을 외웁니다.\n방안에 눈이 뜰 수 없을 정도의 빛이 가득차다가 사라집니다.\n방안의 모든사람이 체력이 회복되었는 것을 느낄수 있습니다.\n");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 가부좌를 틀고서 전회복 주문을 외웁니다.\n방안에 눈이 뜰 수 없을 정도의 빛이 가득차다가 사라집니다.\n방안의 모든사람이 체력이 회복되었는 것을 느낄수 있습니다.\n"
                , ply_ptr);
         
                heal = mrand(1,6) + bonus[ply_ptr->piety];            

                if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
                        heal += mrand(1,3);
                        print(fd,"\n이 방의 기운의 당신의 주술력을 강화시킵니다\n");
                }        
                while(cp){
                        cp_tmp = cp->next_tag;
                        if(cp->crt->type != MONSTER) {
                                if(cp->crt != ply_ptr) {
                        print(cp->crt->fd,"당신의 몸에서도 회복의 기운이 솟아오름을 느낄 수 있습니다.\n",ply_ptr);
                                }
                        cp->crt->hpcur += heal;
                        cp->crt->hpcur = MIN(cp->crt->hpmax, cp->crt->hpcur);
                        }
                        cp = cp_tmp;
                }

        return(1);
}

/**********************************************************************/
/*                      remove blindness(개안술)               */
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
        print(fd, "당신의 도력이 부족합니다");
        return(0);
    }
 
    if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
       ply_ptr->class < INVINCIBLE && how == CAST) {
            print(fd, "이 기술은 불제자와 무사만이 사용할 수 있습니다.");
            return(0);
    }                         
 
    if(!S_ISSET(ply_ptr, SRMBLD) && how == CAST) {
        print(fd, "당신은 아직 그런 주문을 터득하지 못했습니다.");
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
            print(fd, "당신의 이마에 개안부를 붙히며 개안술 주문을 외웁니다.\n감겼던 눈이 움찔거리다가 갑자기 눈앞이 밝아집니다.");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M이 자신의 이마에 개안부를 붙히며 개안술 \n주문을 외웁니다.\n감겼던 그의 눈이 움찔거리다가 갑자기 눈을 확 뜹니다.",
                      ply_ptr);
        }
        else if(how == POTION & F_ISSET(ply_ptr, PBLIND))
            print(fd, "감겼던 눈이 움찔거리다가 갑자기 눈앞이 밝아집니다.");
        else if(how == POTION)
            print(fd, "약을 먹자 당신 눈에 걸린 주술이 스르르 풀리는 것을 느낍니다.");
 
        F_CLR(ply_ptr, PBLIND);
 
    }
 
    else {
 
        if(how == POTION) {
            print(fd, "이 물건은 자신에게만 사용할수 있습니다.");
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
                print(fd, "그런 사람이 존재하지 않습니다.");
                return(0);
            }
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
        F_CLR(crt_ptr, PBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "당신은 %M의 이마에 개안부를 붙히고서 주문을 \n외웁니다.\n그의 감겼던 눈이 움찔거리다가 갑자기 확 뜹니다.", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 %M의 이마에 개안부를 붙히고서 \n주문을 외웁니다.\n그의 감겼던 눈이 움찔거리다가 갑자기 확 뜹니다.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "%M이 당신의 이마에 개안부를 붙히고서 주문을\n외웁니다.\n감겼던 당신의 눈이 움찔거리다가 갑자기 밝아집니다.\n", ply_ptr);
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
        print(fd, "당신의 도력이 부족합니다");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SBLIND) && how == CAST) {
        print(fd, "당신은 아직 그런 주문을 터득하지 못했습니다.");
        return(0);
    }

    if(ply_ptr->class<SUB_DM) {
        print(fd,"당신은 사용할 권한이 없는 주문입니다.");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "당신의 모습이 드러납니다.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 모습니 나타나기 시작합니다.\n",
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
            print(fd, "당신은 두손가락을 독수리 발톱모양을 하고서 실명\n주문을 걸었습니다.\n손가락에서 검은안개같은 기운이 나와 당신의 눈을 찌릅니다.\n악~~~ 내눈.. 당신의 눈이 떠지질 않습니다.");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M이 두손가락을 독수리 발톱모양을 하고서 실명\n주문을 걸었습니다.\n손가락에서 검은안개같은 기운이 그의 눈을 찌르자\n괴성을 지릅니다. 악~~ 내눈..",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "갑자기 당신의 눈이 감기더니 눈이 떠지질 않습니다.");
 
    }
 
    /* blind a monster or player */
    else {
        if(how == POTION) {
            print(fd, "그 물건은 자신에게만 사용할수 있습니다.");
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
                print(fd, "그런 사람이 존재하지 않습니다.");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "당신은 %s에게 주술을 걸 수 없습니다.",
                F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
            return(0);
        }
 
                if (crt_ptr->type == PLAYER)
                        F_SET(crt_ptr,PBLIND);
                else
                        F_SET(crt_ptr,MBLIND);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "당신의 손가락을 %M의 눈을 향하고서 실명 \n주문를 외웁니다.\n검은안개같은 기운이 손가락에서 나와 그의 눈을\n찌르자 괴성을 지릅니다. 악~~ 내눈..\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 손가락을 %M의 눈을 향하고서 실명\n주문를 외웠습니다.\n검은안개같은 기운이 손가락에서 나와 그의 눈을 \n찌르자 괴성을 지릅니다. 악~~ 내눈..\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "\n%M이 손가락을 당신의 눈을 향하고서 실명 주문를 외웁니다.\n검은안개같은 기운이 손가락에서 나와 당신의 눈을\n 찌르자 괴성을 지릅니다. 악~~ 내눈..\n당신의 앞이 눈이 감겨서 보이질 않습니다.\n",
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
        print(fd, "당신의 도력이 부족합니다.");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SFEARS) && how == CAST) {
        print(fd, "당신은 아직 그런 주문을 터득하지 못했습니다.");
        return(0);
    }
 
    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "당신의 모습이 드러납니다.");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 모습이 나타나기 시작합니다.",
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
            print(fd, "당신은 실수로 지옥구슬을 떨어뜨렸습니다.\n갑자기 당신이 무서워하던 것들이 나타나 당신을 둘러쌉니다.\n악~~~ 저리가~~ 당신은 공포에 떨기 시작합니다.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M이 실수로 지옥구슬을 떨어뜨렸습니다.\n갑자기 구슬이 펑하고 터지더니 갑자기 그가 괴성을\n지르기 시작합니다. 악~~~ 저리가~~\n그가 공포에 떨기 시작하는데 당신의 눈에는 아무것도 보이지 않습니다.",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "갑자기 당신이 무서워하던 것들이 나타나 당신을 둘러쌉니다.\n악~~~ 저리가~~ 당신은 공포에 떨기 시작합니다.");
 
    }
 
    /* fear a monster or player */
    else {
        if(how == POTION) {
            print(fd, "그 물건은 자신에게만 사용할수 있습니다.");
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
                print(fd, "그런 사람이 존재 하지 않습니다.");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "당신은 %s에게 주술을 걸 수 없습니다.",
                F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
            return(0);
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MPERMT)) {
            print(fd, "%M의 주위에 공포의 기운이 둘러쌉니다.\n하지만, 그가 기합을 지르자 금새 그 기운이 사라졌습니다.",crt_ptr);
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
            print(fd, "당신은 지옥구술을 %M에게 던졌습니다.\n구슬이 펑하고 터지면서 공포의 기운이 그를 둘러쌉니다.\n악~~~ 저리가~~ 갑자기 그가 괴성을 지르면서 공포에\n떨기 시작합니다.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 %M에게 지옥구술을 던졌습니다.\n구슬이 펑하고 터지자 갑자기 그가 괴성을 지릅니다. 악~~~ 저리가~~\n그는 공포에 떨지만 당신의 눈에는 아무것도 보이지 않습니다.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "\n%M이 당신에게 지옥구슬을 던졌습니다.\n갑자기 당신이 무서워하던 것들이 나타나 당신을 둘러쌉니다.\n\"악~~~ 저리가~~\" 당신은 괴성을 지르며 공포에 떨기\n시작합니다.\n",
                  ply_ptr);
        }
 
        if(crt_ptr->type != PLAYER)
            add_enm_crt(ply_ptr->name, crt_ptr);
 
    }
 
    return(1);
 
}            


/**********************************************************************/
/*                            silence(봉합구)                                 */
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
        print(fd, "당신의 도력이 부족합니다.");
        return(0);
    }
 
    if(!S_ISSET(ply_ptr, SSILNC) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }
 
    if(ply_ptr->class<SUB_DM) {
        print(fd,"그 주문을 펼치기엔 당신의 능력이 부족합니다.");
        return 0;
    }

    if(F_ISSET(ply_ptr, PINVIS)) {
        F_CLR(ply_ptr, PINVIS);
        print(fd, "당신의 모습이 드러납니다.\n");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 모습이 나타나개 시작합니다.",
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
            print(fd, "\n당신은 실수로 봉합구 주문을 자신에게 걸었습니다.\n억... 당신은 입을 벌려 말을 하려 하지만 목소리가\n사라졌습니다.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M이 실수로 봉합구 주문을 자신에게 걸었습니다.\n그는 입을 벌려 말을 하려 하지만 목소리가 들리지 않습니다.\n",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "\n당신이 먹은것이 목에 걸려 목소리가 나오지 않습니다.\n");
 
    }
 
    /* silence a monster or player */
    else {
        if(how == POTION) {
            print(fd, "\n그 물건은 자신에게만 사용할수 있습니다.\n");
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
                print(fd, "\n그런 사람이 존재하지 않습니다.\n");
                return(0);
            }
 
        }
 
        if(crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MUNKIL)) {
            print(fd, "\n당신은 %s에게 주술을 걸 수 없습니다.\n",
                F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
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
            print(fd, "\n당신은 잽싸게 쫓아가 %M의 목을 치면서 \n봉합구 주문을 외웁니다.\n그는 입을 벌려 말을 하려 하지만 목소리가 들이지 않습니다.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 잽싸게 쫓아가 %M의 목을 치면서 \n봉합구 주문을 외웁니다.\n그는 입을 벌려 말을 하려 하지만 목소리가 들이지 않습니다.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, 
                  "\n%M이 잽싸게 쫓아와 당신의 목을 치면서 봉합구\n주문을 외웁니다.\n당신은 입을 벌려 말을 하려 하지만 목소리가 들이지 않습니다.\n",
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
        print(fd, "\n당신의 도력이 부족합니다\n");
        return(0);
    }
 
        if(ply_ptr->class != CLERIC && ply_ptr->class != PALADIN &&
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n이 주술은 불제자와 무사만이 사용할 수 있습니다.\n");
                return(0);
        }                         
 
    if(!S_ISSET(ply_ptr, SRMDIS) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
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
                      F_ISSET(ply_ptr, PMALES) ? "그":"그녀");
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
/*              charm (이혼대법)                                         */
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
        print(fd, "당신의 도력이 부족합니다");
        return(0);
    }

     
    if(!S_ISSET(ply_ptr, SCHARM) && how == CAST) {
        print(fd, "당신은 아직 그런 주문을 터득하지 못했습니다.");
        return(0);
    }
	if(F_ISSET(rom_ptr, RSUVIV)) {
		print(fd, "대련장에서는 이 주문을 사용할 수 없습니다.");
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
            print(fd, "당신은 심심해서 거울을 보며 이혼대법을 사용합니다.\n기분이 좋아지면서 괜히 맞아도 황홀한 기분이\n듭니다. 나 좀 때려줘..");
            broadcast_rom(fd, ply_ptr->rom_num,
                      "\n%M이 이혼대법의 주술을 거는 거울을 봅니다.\n거울을 보고나자 당신을 보면서 괜히 히죽히죽\n거립니다. 이 자식 미쳤나?",
                      ply_ptr);
        }
        else if(how == POTION)
            print(fd, "기분이 좋아지면서 괜히 맞아도 황홀한 기분이\n듭니다. 나 좀 때려줘..");


    }

    else {

        if(how == POTION) {
            print(fd, "그 물건은 자신에게만 사용할수 있습니다.");
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
                print(fd, "그런 사람이 존재하지 않습니다.");
                return(0);
            }
        }

        if(how == CAST)
            ply_ptr->mpcur -= 15;

        if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
           (crt_ptr->type != PLAYER && F_ISSET(crt_ptr, MRMAGI)))
            dur /= 2;
        if((ply_ptr->level < crt_ptr->level) || F_ISSET(crt_ptr, MNOCHA)) {
            print(fd, "상대의 기가 당신의 주문을 반탄시킵니다.");
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
               "\%M이 이혼대법을 %M에게 걸려고 합니다.\n",
                ply_ptr, crt_ptr);
            if(crt_ptr->type == PLAYER)
              print(crt_ptr->fd, "%M이 당신에게 이혼대법을 걸으려 합니다.\n",ply_ptr);
            return(0);
        }

        if(how == CAST || how == SCROLL || how == WAND) {

            print(fd, "당신은 %M에게 거울을 비추며 이혼대법을 겁니다.\n거울을 보고나자 당신을 보면서 괜히 히죽히죽\n거립니다. 드디어 맛 갔군.", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 %M에게 거울을 비추며 이혼대법을 겁니다.\n거울을 보고나자 당신을 보면서 괜히 히죽히죽\n거립니다. 저 자식이 미쳤나?",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "\n%M이 당신에게 거울을 비추며 이혼대법을 겁니다.\n괜히 기분이 좋아지면서 맞아도 황홀한 기분이\n듭니다. 나 좀 때려줘..", ply_ptr);
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
                        print(fd,"당신의 주문이 실패했습니다.");
                        return(1);
                }
                else
                        return(0);

        case BARBARIAN:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5); 
                if(n>chance) {
                        print(fd,"당신의 주문이 실패했습니다.");
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
                        print(fd,"당신의 주문이 실패했습니다.");
                        return(1);
                }
                else
                        return(0);

        case FIGHTER:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+10; 
                if(n>chance) {
                        print(fd,"당신의 주문이 실패했습니다.");
                        return(1);
                }
                else
                        return(0);

        case MAGE:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+75; 
                if(n>chance) {
                        print(fd,"당신의 주문이 실패했습니다.");
                        return(1);
                }
                else
                        return(0);

/*        case MONK:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*6)+25; 
                if(n>chance) {
                        print(fd,"당신의 주문이 실패 했습니다.");
                        return(1);
                }
                else
                        return(0); */

        case PALADIN:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*5)+50; 
                if(n>chance) {
                        print(fd,"당신의 주문이 실패했습니다.");
                        return(1);
                }
                else
                        return(0);

        case RANGER:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*4)+56; 
                if(n>chance) {
                        print(fd,"당신의 주문이 실패했습니다.");
                        return(1);
                }
                else
                        return(0);

        case THIEF:
                chance = ((((ply_ptr->level+3)/4)+bonus[ply_ptr->intelligence])*6)+22; 
                if(n>chance) {
                        print(fd,"당신의 주문이 실패했습니다.");
                        return(1);
                }
                else
                        return(0);

        default:
                return(0);
    }
}

