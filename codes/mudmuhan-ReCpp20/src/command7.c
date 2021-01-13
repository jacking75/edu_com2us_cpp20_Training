/*
 * COMMAND7.C:
 *
 *      Additional user routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */
#include <stdlib.h>
#include "mstruct.h"
#include "mextern.h"
extern long all_broad_time;
/**********************************************************************/
/*                              flee                                  */
/**********************************************************************/
/* This function allows a player to flee from an enemy.  If successful */
/* the player will drop his readied weapon and run through one of the  */
/* visible exits, losing 10% or 1000 experience, whichever is less.    */
int flee(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        xtag    *xp;
        ctag    *cp, *temp;
        char    found = 0;
        int     fd, n;
        long    i, t;
        rom_ptr = ply_ptr->parent_rom;
        fd = ply_ptr->fd;
        t = time(0);
        i = MAX(ply_ptr->lasttime[LT_ATTCK].ltime,
                ply_ptr->lasttime[LT_SPELL].ltime);
        if(t < i && !F_ISSET(ply_ptr, PFEARS)) {
                please_wait(fd, i-t);
                return(0);
        }
        t = Time%24L;
        xp = rom_ptr->first_ext;
        if(xp) do {
                if(F_ISSET(xp->ext, XCLOSD)) continue;
                if((F_ISSET(xp->ext, XCLIMB) || F_ISSET(xp->ext, XDCLIM)) &&
(!F_ISSET(ply_ptr, PLEVIT))) continue;
                if(F_ISSET(xp->ext, XNAKED) && weight_ply(ply_ptr)) continue;
        if(F_ISSET(xp->ext, XFEMAL) && F_ISSET(ply_ptr, PMALES)) continue;
        if(F_ISSET(xp->ext, XMALES) && !F_ISSET(ply_ptr, PMALES)) continue;
                if(F_ISSET(xp->ext, XFLYSP) && !F_ISSET(ply_ptr, PFLYSP))
                        continue;
                if(!F_ISSET(ply_ptr, PDINVI) && F_ISSET(xp->ext, XINVIS))
                        continue;
                if(F_ISSET(xp->ext, XSECRT)) continue;
                if(F_ISSET(xp->ext, XNOSEE)) continue;
                if(F_ISSET(xp->ext, XNGHTO) && (t>6 && t < 20)) continue;
                if(F_ISSET(xp->ext, XDAYON) && (t<6 || t > 20))  continue;
    if(F_ISSET(xp->ext,XPGUAR)){
        cp = rom_ptr->first_mon;
        while(cp) {
            if(F_ISSET(cp->crt, MPGUAR) &&
            !F_ISSET(ply_ptr, PINVIS) && ply_ptr->class < CARETAKER) {
                found = 1;
                break;
            }
            cp = cp->next_tag;
        }
        if(found)
                continue;
    }
            /* 비전투시 도망처리 */
           if (!ply_is_attacking(ply_ptr,cmnd)){
               print(fd,"누구에게서 도망가시려구요?");
               return 0;
            }
/* if(mrand(1,100) < 70) break;     */
                if(mrand(1,100) < (65 + bonus[ply_ptr->dexterity]*5))
                        break;
        } while(xp = xp->next_tag);

        if(xp && F_ISSET(xp->ext,52) && mrand(1,5) < 2 && !F_ISSET(ply_ptr, PFEARS))
                xp = 0;
        if(!xp) {
                print(fd, "\n당신은 겁에 질려 다리가 떨어지지 않습니다!");
                return(0);
        }
/*
        if(ply_ptr->ready[WIELD-1] && !ply_ptr->ready[WIELD-1]->questnum &&
           !F_ISSET(ply_ptr->ready[WIELD-1], OCURSE)) {
                add_obj_rom(ply_ptr->ready[WIELD-1], rom_ptr);
                ply_ptr->ready[WIELD-1] = 0;
                compute_thaco(ply_ptr);
                compute_ac(ply_ptr);
                print(fd, "\n당신은 무기를 버리고 줄행랑을 칩니다.");
        }
        else
 */               print(fd, "\n당신은 줄행랑을 칩니다.");
        F_CLR(ply_ptr, PHIDDN);
        if(F_ISSET(ply_ptr, PFEARS)) {
                ANSI(fd, RED);
                print(fd, "\n당신은 겁에 질린듯 얼굴이 창백하게 변해 도망을 갑니다!");
                ANSI(fd, WHITE);
        }

        if(!F_ISSET(rom_ptr, RPTRAK))
                strcpy(rom_ptr->track, xp->ext->name);
        broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j %s쪽으로 도망을 갑니다.",
                      ply_ptr, "1",xp->ext->name);
        if (ply_ptr->class == PALADIN)
                if (ply_ptr->level > 20) {
                n = ((ply_ptr->level+3)/4)*10;
                n = MIN(ply_ptr->experience, n);
                print(fd,"당신은 도망을 간 댓가로 %d 만큼의 경험치를 잃었습니다.",n);
                ply_ptr->experience -= n;
                lower_prof(ply_ptr,n);
        }
        load_rom(xp->ext->room, &rom_ptr);
        if(rom_ptr->lolevel > ply_ptr->level ||
           (ply_ptr->level > rom_ptr->hilevel && rom_ptr->hilevel)) {
                print(fd, "어떤 힘에 의해 다시 되돌아 왔습니다.");
                broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j 도착하였습니다.",ply_ptr,"1");
                return(0);
        }
        n = count_vis_ply(rom_ptr);
        if((F_ISSET(rom_ptr, RONEPL) && n > 0) ||
           (F_ISSET(rom_ptr, RTWOPL) && n > 1) ||
           (F_ISSET(rom_ptr, RTHREE) && n > 2)) {
                print(fd, "도망갈려는 방의 정원이 가득 찼습니다!");
                broadcast_rom(fd, rom_ptr->rom_num, "\n%M%j 도착하였습니다.",ply_ptr,"1");
                return(0);
        }
        del_ply_rom(ply_ptr, ply_ptr->parent_rom);
        add_ply_rom(ply_ptr, rom_ptr);
        check_traps(ply_ptr, rom_ptr);
        return(0);
}
/**********************************************************************/
/*                              list                                  */
/**********************************************************************/
/* This function allows a player to list the items for sale within a */
/* shoppe.                                                           */
int list(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr, *dep_ptr;
        otag    *op;
        int     fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(F_ISSET(rom_ptr, RSHOPP)) {
                if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
                        print(fd, "살 물건이 없습니다.");
                        return(0);
                }
                print(fd, "상품들:");
                op = dep_ptr->first_obj;
                while(op) {
                        print(fd, "\n   %-30s   가격: %ld",
                              obj_str(op->obj, 1, CAP), op->obj->value);
                        op = op->next_tag;
                }
        }
        else
                print(fd, "여기는 상점이 아닙니다.");
        return(0);
}
/**********************************************************************/
/*                              buy                                   */
/**********************************************************************/
/* This function allows a player to buy something from a shoppe.      */
int buy(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr, *dep_ptr;
        object  *obj_ptr, *obj_ptr2;
        int     fd,i,cnt=0;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        for(i=0,cnt=0; i<MAXWEAR; i++)
           if(ply_ptr->ready[i]) cnt++;
        cnt += count_inv(ply_ptr, 0);
        if(!F_ISSET(rom_ptr, RSHOPP)) {
                print(fd, "여기는 상점이 아닙니다.");
                return(0);
        }
        if(cmnd->num < 2) {
                print(fd, "무엇을 사시려구요?");
                return(0);
        }
        if(load_rom(rom_ptr->rom_num+1, &dep_ptr) < 0) {
                print(fd, "살 물건이 없습니다.");
                return(0);
        }
        obj_ptr = find_obj(ply_ptr, dep_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
        if(!obj_ptr) {
                print(fd, "그런 물건은 팔지 않습니다.");
                return(0);
        }
        if(ply_ptr->gold < obj_ptr->value) {
                print(fd, "돈도 없으면서... 외상사절!");
                return(0);
        }
                if(weight_ply(ply_ptr) + weight_obj(obj_ptr) >
                   max_weight(ply_ptr) || cnt>200) {
                   print(fd, "당신은 더이상 가질 수 없습니다.");
                   return(0);
               }
        obj_ptr2 = (object *)malloc(sizeof(object));
        if(!obj_ptr2)
                merror("buy", FATAL);
        F_CLR(ply_ptr, PHIDDN);
        *obj_ptr2 = *obj_ptr;
        F_CLR(obj_ptr2, OPERM2);
        F_CLR(obj_ptr2, OPERMT);
        F_CLR(obj_ptr2, OTEMPP);
        add_obj_crt(obj_ptr2, ply_ptr);
        ply_ptr->gold -= obj_ptr2->value;
        print(fd, "당신은 %1i%j 샀습니다",obj_ptr2,"3");
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %1i%j 샀습니다.",
                      ply_ptr, obj_ptr2,"3");
        return(0);
}
/**********************************************************************/
/*                              sell                                  */
/**********************************************************************/
/* This function will allow a player to sell an object in a pawn shoppe */
int sell(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     gold, fd, poorquality = 0;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(!F_ISSET(rom_ptr, RPAWNS)) {
                print(fd, "여기는 전당포가 아닙니다.");
                return(0);
        }
        if(cmnd->num < 2) {
                print(fd, "무엇을 파시려구요?");
                return(0);
        }
        F_CLR(ply_ptr, PHIDDN);
        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
        if(!obj_ptr) {
                print(fd, "당신은 그런 물건을 갖고 있지 않습니다.");
                return(0);
        }
        gold = MIN(obj_ptr->value / 2, 100000);
        if((obj_ptr->type <= MISSILE || obj_ptr->type == ARMOR) &&
           obj_ptr->shotscur <= obj_ptr->shotsmax/8)
                poorquality = 1;
        if((obj_ptr->type == WAND || obj_ptr->type == KEY) && obj_ptr->shotscur < 1)
                poorquality = 1;
        if(gold < 20 || poorquality) {
                print(fd, "전당포주인이 \"그런 쓰레기는 안사요!\"라고 말합니다.");
                return(0);
        }
        if(F_ISSET(obj_ptr, ONEWEV)) {
                        print(fd, "전당포주인이 \"그런건 안사요!\"라고 말합니다.");
                        return(0);
        }
		if(obj_ptr->first_obj) {
			print(fd, "전당포주인이 \"그 안에 뭔가가 들어있군요.\"라고 말합니다.");
			return(0);
		}
        if(obj_ptr->type == SCROLL || obj_ptr->type == POTION) {
                print(fd, "전당포주인이 \"두루마기나 독약같은것은 안사요!\"라고 말합니다.");
                return(0);
        }
        if(((time(0)+mrand(1, 100))%250) == 9) {
                print(fd, "제가 %i%j 사죠.\n 오늘은 기분이 좋으니 두배로 드리죠.\n
                        전당포주인이 당신에게 %d냥을 줍니다.", obj_ptr, "3",gold*2);
                broadcast_rom(fd, ply_ptr->rom_num,  "\n%M이 전당포주인에게  %1i%j 팝니다.",
                        ply_ptr, obj_ptr, "3");
                ply_ptr->gold += gold;
        }
        else {
        print(fd, "제가 %i%j 사죠.\n전당포주인이 당신에게 %d냥을 줍니다.", obj_ptr, "3",gold);
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 전당포주인에게 %1i%j 팝니다.",
                      ply_ptr, obj_ptr,"3");
        }
        ply_ptr->gold += gold;
        del_obj_crt(obj_ptr, ply_ptr);
        free_obj(obj_ptr);
        return(0);
}
/**********************************************************************/
/*                              value                                 */
/**********************************************************************/
/* This function allows a player to find out the pawn-shop value of an */
/* object, if he is in the pawn shop.                                  */
int value(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        long    value;
        int     fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(!F_ISSET(rom_ptr, RPAWNS) && !F_ISSET(rom_ptr, RREPAI)) {
                print(fd, "전당포에 가셔서 물건의 가치를 알아보세요.");
                return(0);
        }
        if(cmnd->num < 2) {
                print(fd, "어떤 물건의 가치를 알고 싶으세요?");
                return(0);
        }
        F_CLR(ply_ptr, PHIDDN);
        obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
                           cmnd->str[1], cmnd->val[1]);
        if(!obj_ptr) {
                print(fd, "당신은 그런 물건을 갖고 있지 않습니다.");
                return(0);
        }
        if(F_ISSET(rom_ptr, RPAWNS)) {
                value = MIN(obj_ptr->value / 2, 100000L);
                print(fd, "상점주인이 \"%I이라면 %ld냥  정도는 드릴 수  있어요.\"라고 말합니다.",
                      obj_ptr, value);
        }
        else {
                value = obj_ptr->value / 4;
                print(fd,
                      "상점주인이 \"%I%j 수리하는데 %ld냥이 듭니다.\"라고 말합니다.",
                      obj_ptr, "3",value);
        }
        broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 %i의 가치를 알아봅니다.",
                      ply_ptr, obj_ptr);
        return(0);
}
/**********************************************************************/
/*                              backstab                              */
/**********************************************************************/
/* This function allows thieves and assassins to backstab a monster.  */
/* If successful, a damage multiplier is given to the player.  The    */
/* player must be successfully hidden for the backstab to work.  If   */
/* the backstab fails, then the player is forced to wait double the   */
/* normal amount of time for his next attack.                         */
int backstab(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        room            *rom_ptr;
        long            i, t;
        int             fd, m, n, p, addprof;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(ply_ptr->class != THIEF && ply_ptr->class != ASSASSIN &&
           ply_ptr->class < INVINCIBLE) {
                print(fd, "도둑과 자객만 사용할 수 있는 기술입니다.");
                return(0);
        }
        if(cmnd->num < 2 || F_ISSET(ply_ptr, PBLIND)) {
                print(fd, "누구를 기습하시려구요?");
                return(0);
        }
        if(!ply_ptr->ready[WIELD-1] || (ply_ptr->ready[WIELD-1]->type
           != SHARP && ply_ptr->ready[WIELD-1]->type != THRUST)) {
                print(fd, "기습을 하시려면 도나 검종류의 무기가 필요합니다.");
                return(0);
        }
        t = time(0);
        i = LT(ply_ptr, LT_ATTCK);
        if(t < i) {
                please_wait(fd, i-t);
                return(0);
        }
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                           cmnd->str[1], cmnd->val[1]);
        if(!crt_ptr) {
                cmnd->str[1][0] = up(cmnd->str[1][0]);
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                   cmnd->str[1], cmnd->val[1]);
                if(!crt_ptr || crt_ptr == ply_ptr || strlen(cmnd->str[1]) < 2) {
                        print(fd, "그런건 여기 없어요.");
                        return(0);
                }
        }
        if(crt_ptr->type != PLAYER && is_enm_crt(ply_ptr->name, crt_ptr)) {
                print(fd, "당신은 %s와 싸울 수 없습니다.",
                      F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                return(0);
        }
        if(crt_ptr->type == PLAYER) {
                if(F_ISSET(rom_ptr, RNOKIL)) {
                        print(fd, "이 방에서는 싸울 수 없습니다.");
                        return(0);
                }
		if(!F_ISSET(ply_ptr, PFAMIL) || !F_ISSET(crt_ptr, PFAMIL)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "당신은 선해서 다른 사람을 공격할 수 없습니다.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "그 사용자는 선해서 보호받고 있습니다.");
                    return (0);
                }
		}
		else if(check_war(ply_ptr->daily[DL_EXPND].max, crt_ptr->daily[DL_EXPND].max)) {
                if(!F_ISSET(ply_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "당신은 선해서 다른 사람을 공격할 수 없습니다.");
                    return (0);
                }
                if(!F_ISSET(crt_ptr, PCHAOS) && !F_ISSET(rom_ptr, RSUVIV)) {
                    print(fd, "그 사용자는 선해서 보호받고 있습니다.");
                    return (0);
                }
		}
                if(is_charm_crt(ply_ptr->name, crt_ptr) && F_ISSET(ply_ptr, PCHARM)){
                        print (fd,  "당신은 %S%j 아끼기 때문에 그를 해칠 수 없습니다.", crt_ptr->name,"3");
                        return(0);
                }
                print(crt_ptr->fd, "\n%M%j 당신의 뒤로 몰래 기어가 옆구리를 쿡~ 찌릅니다.", ply_ptr,"1");
        }
        else if(F_ISSET(crt_ptr, MUNKIL)) {
                print(fd, "당신은 %s를 해칠수 없습니다.",
                        F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                return(0);
        }
        if(is_charm_crt(crt_ptr->name, ply_ptr) && F_ISSET(ply_ptr, PCHARM)) {
                print(fd, "당신은  %s를  너무 좋아해서   그렇게 할 용기가   나지 않는군요.",crt_ptr->name);
                return(0);
        }
        if(F_ISSET(ply_ptr, PINVIS)) {
                F_CLR(ply_ptr, PINVIS);
                print(fd, "당신의 모습이 서서히 드러납니다.");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%M의 모습이 서서히 드러납니다.",
                              ply_ptr);
        }
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        if(ply_ptr->dexterity > 18)
                ply_ptr->lasttime[LT_ATTCK].interval = 1;
        else
                ply_ptr->lasttime[LT_ATTCK].interval = 2;
        if(crt_ptr->type != PLAYER) {
                if(add_enm_crt(ply_ptr->name, crt_ptr) < 0) {
                        print(fd, "당신은 %M의  뒤로 몰래  기어가 옆구리를  쿡~ 찌릅니다.", crt_ptr);
                        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                                       "\n%M이 %M의   옆구리를 쿡~  찌릅니다.",  ply_ptr, crt_ptr);
                        if(F_ISSET(crt_ptr, MMGONL)) {
                print(fd, "당신의 무기는 %M에게는 아무 소용이 없습니다.", crt_ptr);
                return(0);
                }
                if(F_ISSET(crt_ptr, MENONL)) {
                if(!ply_ptr->ready[WIELD-1] ||
                        ply_ptr->ready[WIELD-1]->adjustment < 1) {
                        print(fd, "당신의 무기는 %M에게 아무 소용이 없습니다.", crt_ptr);
                        return(0);
            }
        }
                }
        }
        else {
                print(fd, "당신은 %M의 뒤로 몰래 기어가 옆구리를 쿡~ 찌릅니다.", crt_ptr);
                print(crt_ptr->fd, "%M이 당신의 옆구리를 쿡~ 찌릅니다.", ply_ptr);
                broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                               "\n%M이 %M의 옆구리를 쿡~ 찌릅니다!", ply_ptr, crt_ptr);
        }
        if(ply_ptr->ready[WIELD-1]) {
                if(ply_ptr->ready[WIELD-1]->shotscur < 1) {
                        print(fd, "\n%S%j 부서졌습니다.",
                              ply_ptr->ready[WIELD-1]->name,"1");
                        add_obj_crt(ply_ptr->ready[WIELD-1], ply_ptr);
                        ply_ptr->ready[WIELD-1] = 0;
                        broadcast_rom(fd, ply_ptr->rom_num,
                                      "\n%s가 기습을 시도했지만 상대방이 피했습니다.",
                                      F_ISSET(ply_ptr, PMALES) ? "그":"그녀");
                        ply_ptr->lasttime[LT_ATTCK].interval *= 2;
                        return(0);
                }
        }
        n = ply_ptr->thaco - crt_ptr->armor/8 + 2;
        if(!F_ISSET(ply_ptr, PHIDDN))
                n = 21;
        F_CLR(ply_ptr, PHIDDN);
        if(mrand(1,20) >= n) {
                if(ply_ptr->ready[WIELD-1])
                        n = mdice(ply_ptr->ready[WIELD-1]);
                else
                        n = mdice(ply_ptr);
                if(ply_ptr->class == THIEF)
                        n *= (mrand(20,35) / 10);
                else
                        n *= 5;
                m = MIN(crt_ptr->hpcur, n);
                if(crt_ptr->type != PLAYER) {
                        add_enm_dmg(ply_ptr->name, crt_ptr, m);
                        if(ply_ptr->ready[WIELD-1]) {
                                p = MIN(ply_ptr->ready[WIELD-1]->type, 4);
                                addprof = (m * crt_ptr->experience) /
                                        crt_ptr->hpmax;
                                addprof = MIN(addprof, crt_ptr->experience);
                                ply_ptr->proficiency[p] += addprof;
                        }
                }
                                if(crt_ptr->class > CARETAKER) n = 1;
                 crt_ptr->hpcur -= n;
                print(fd, "\n당신은 %d 만큼의 피해를 주었습니다.", n);
                print(crt_ptr->fd,  "\n당신은  %M에게서  %d   만큼의 피해를   입었습니다.", ply_ptr, n);
                if(crt_ptr->hpcur < 1) {
                        print(fd, "\n당신은 %M%j 죽였습니다.", crt_ptr,"3");
                        broadcast_rom(fd, ply_ptr->rom_num,
                                      "\n%M%j %M%j 죽였습니다.", ply_ptr,"1", crt_ptr,"3");
                        if(ply_ptr->ready[WIELD-1])
                                ply_ptr->ready[WIELD-1]->shotscur--;
                        die(crt_ptr, ply_ptr);
                }
                else
                        check_for_flee(ply_ptr, crt_ptr);
       }
        else {
                print(fd, "\n허공을 쳤습니다.");
                broadcast_rom(fd, ply_ptr->rom_num, "\n%s가 기습을 시도했지만 상대방이 피했습니다.",
                              F_ISSET(ply_ptr, PMALES) ? "그":"그녀");
                ply_ptr->lasttime[LT_ATTCK].interval *= 3;
        }
        return(0);
}
/**********************************************************************/
/*                              train                                 */
/**********************************************************************/
/* This function allows a player to train if he is in the correct     */
/* training location and has enough gold and experience.  If so, the  */
/* character goes up a level.                                         */
int train(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        long    goldneeded, expneeded, lv;
        int     fd, i, fail = 0, bit[4];
        int up_num=0;
        fd = ply_ptr->fd;
        /* 초인 수련 금지 */
        if(ply_ptr->class == CARETAKER) {
                print(fd, "당신은 수련할수 없는 직업입니다.");
                /*
                ANSI(fd, RED);
                ply_ptr->mpmax = ply_ptr->mpmax + 10;
                ply_ptr->hpmax = ply_ptr->hpmax + 10;
                print(fd, "당신은 눈을 감고 수련을 합니다.");
                ANSI(fd, WHITE);
                */
                return(0);
        }
        if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "당신은 눈이 멀어 수련할 수 없습니다!");
                ANSI(fd, WHITE);
                return(0);
        }
        /* 잠력격발 사용중에 수련시 잠력격발 해제 */
        if(F_ISSET(ply_ptr, PUPDMG)) {
                F_CLR(ply_ptr, PUPDMG);
                ply_ptr->pdice -= 5;
                ply_ptr->hpmax -= 100;
                ply_ptr->mpmax -= 100;
                ply_ptr->hpcur = ply_ptr->hpmax;
                ply_ptr->mpcur = ply_ptr->mpmax;
        }
        rom_ptr = ply_ptr->parent_rom;
        if(!F_ISSET(rom_ptr, RTRAIN)) {
                print(fd, "이 곳은 수련할 수 있는곳이 아닙니다!");
                return(0);
        }
        
        ply_ptr->class--;
        bit[0] = ply_ptr->class & 1;
        bit[1] = ply_ptr->class & 2;
        bit[2] = ply_ptr->class & 4;
        ply_ptr->class++;
        for(i=0; i<3; i++) {
                if((bit[i]?1:0)!=(F_ISSET(rom_ptr, RTRAIN+3-i)?1:0)) fail=1;
                /*
                if(!((bit[i] && F_ISSET(rom_ptr, RTRAIN+3-i)) ||
                   (!bit[i] && !F_ISSET(rom_ptr, RTRAIN+3-i))))
                        fail = 1;
                */
        }
        if(ply_ptr->class>8) fail=0;
        if(fail) {
                print(fd, "당신이 수련하는곳은 여기가 아닙니다.");
                return(0);
        }
        if(ply_ptr->level <= MAXALVL)
                expneeded = needed_exp[ply_ptr->level-1];
        else
            expneeded = needed_exp[MAXALVL-2]
                +((long)ply_ptr->level-MAXALVL+1)*5000000;
        if(ply_ptr->level<MAXALVL) goldneeded = (expneeded / 10L)/2;
        else                       goldneeded = (needed_exp[MAXALVL-2] /10L)/2;
        if(expneeded > ply_ptr->experience) {
                print(fd, "당신은 %ld 만큼의 경험치가 더 필요합니다.",
                      expneeded - ply_ptr->experience);
                return(0);
        }
        if(goldneeded > ply_ptr->gold) {
                print(fd, "돈도 없으면서... 돈 벌어오세요.\n");
                print(fd, "당신은 수련하는데 %ld냥이 듭니다.", goldneeded);
                return(0);
        }
       /*   무적 전환 */
        if((ply_ptr->level==100) && (ply_ptr->class < INVINCIBLE)) {
                ply_ptr->class = INVINCIBLE;
                        if(F_ISSET(ply_ptr, PFAMIL)) {
                                edit_member(ply_ptr->name,                  ply_ptr->class, 
ply_ptr->daily[DL_EXPND].max, 3);
                        }
                ply_ptr->level = 1;
                ply_ptr->experience = 0;
        }

        /* 초인전환 */
        if((ply_ptr->level>=127) && (ply_ptr->class == INVINCIBLE)) {
                ply_ptr->class = CARETAKER;
                ply_ptr->level = 127;
                        if(F_ISSET(ply_ptr, PFAMIL)) {
                                edit_member(ply_ptr->name,                  ply_ptr->class, 
ply_ptr->daily[DL_EXPND].max, 3);
                        }
              ply_ptr->gold -= goldneeded;
              ply_ptr->hpmax = 800;
              ply_ptr->mpmax = 600;
                        ply_ptr->ndice = 4;
                        ply_ptr->sdice = 4;
                        ply_ptr->pdice = 4;
              broadcast_all("\n### %s님께서 초인이 되셨습니다!!",ply_ptr->name);
                print(fd,"\n축하합니다! 당신은 초인이 되었습니다!!");
                all_broad_time=time(0);
                return(0);
        } 
        else {
    up_num=0;
    do {
        if(ply_ptr->level==100 && (ply_ptr->class < INVINCIBLE)) break;
        ply_ptr->gold -= goldneeded;
        up_level(ply_ptr);
        up_num++;
        if(ply_ptr->level <= MAXALVL)
            expneeded = needed_exp[ply_ptr->level-1];
        else
            expneeded = needed_exp[MAXALVL-2]
                +((long)ply_ptr->level-MAXALVL+1)*5000000;
        if(ply_ptr->level<MAXALVL) goldneeded = (expneeded / 10L)/2;
        else                       goldneeded = (needed_exp[MAXALVL-2] /10L)/2;
    } while(expneeded <= ply_ptr->experience && goldneeded <= ply_ptr->gold);

        if(up_num<=1)
            broadcast_all("\n### %s님이 %d레벨로 올랐습니다!",
                ply_ptr->name,ply_ptr->level);
        else
            broadcast_all("\n### %s님이 %d레벨로 %d단계 올랐습니다!",
                ply_ptr->name,ply_ptr->level,up_num);
        print(fd, "\n축하합니다! 당신의 레벨이 올랐습니다!");
        all_broad_time=time(0);
        return(0);
        }
}
/**********************************************************************/
/*                              forge제련                             */
/**********************************************************************/
/*  새로운 자신만의 무기를 제작하는 루틴이다     */
/*  아이템 번호 900번부터 도 검 봉 창 궁 순으로 나간다 */
void select_arm();
int forge(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

/*
        print(fd, "버그로 잠시 사용중지합니다\n");
        return(0); 
*/
        if(!F_ISSET(rom_ptr, RFORGE)) {
                print(fd, "여기는 대장간이 아닙니다.");
                return(0);
        }
 
        F_SET(ply_ptr, PNOBRD);    
        select_arm(fd, 1, NULL);
        F_CLR(ply_ptr, PNOBRD);
        
        return(DOPROMPT);
}
/**********************************************************************/
/*                             select_arm                            */
/**********************************************************************/
/* 무기의 특성을 설계하는 부분이다.             */
char forge1[PMAX][16];
char forge2[PMAX][16];
void select_arm(fd, param, str)
int fd;
int        param;
char       *str;
{
    int     i;
    long    sum=0;
    int     num[5];
    creature *ply_ptr;
    object  *obj_ptr;
    ply_ptr=Ply[fd].ply;
    switch(param) {
    case 1:
        print(fd,"\n\n");
        print(fd, "어떤 종류의 무기를 원하십니까?\n1. 도 2. 검 3. 봉 4. 창 5. 궁 ?\n:");
        F_SET(Ply[fd].ply,PREADI);   
        output_buf();
        Ply[fd].io->intrpt &= ~1;
        RETURN(fd, select_arm, 2);
    case 2:
        F_CLR(Ply[fd].ply,PREADI); 
        if(str[0]>='1' && str[0]<='5') {
            if(load_obj(900+str[0]-'1', &obj_ptr) < 0) {
                print(ply_ptr->fd, "Error (%d)\n", 900+str[0]-'1');
                return;
            }
        }
        else {
            print(fd, "하나를 선택하시오: ");
            F_SET(Ply[fd].ply,PREADI); 
            RETURN(fd, select_arm, 2);
        }
        /* 포인터 obj_ptr을 long형으로 바꿔 저장. */
        sprintf(forge1[fd],"%lu",(long)obj_ptr);
        print(fd, "\n타격치에 영향을 주는 재료를 선택하십시요.\n");
        print(fd, "1.강 철 오만냥 2.귀금속 이십만냥 3.금강석 삼십만냥\n:");
        F_SET(Ply[fd].ply,PREADI); 
        RETURN(fd, select_arm, 3);
    case 3:
        F_CLR(Ply[fd].ply,PREADI); 
        obj_ptr=(object *)atol(forge1[fd]);
        switch(low(str[0])) {
              case '1':
                    obj_ptr->sdice = 3;
                    sum = 50000;
                    break;
              case '2':
                    obj_ptr->sdice = 4;
                    sum = 200000;
                    break;
              case '3':
                    if((ply_ptr->class == CLERIC)||(ply_ptr->class == PALADIN)||(ply_ptr->class == MAGE)) {
                        print(fd, "당신은 이런 무기를 사용할 능력이 없습니다.\n");
                        print(fd, "다른재료를 선택하십시요\n");
                        F_SET(Ply[fd].ply,PREADI); 
                        RETURN(fd, select_arm, 3);
                    }
                    F_SET(obj_ptr,OCLSEL);
                    F_SET(obj_ptr,OASSNO);
                    F_SET(obj_ptr,OBARBO);
                    F_SET(obj_ptr,OFIGHO);
                    F_SET(obj_ptr,ORNGRO);
                    F_SET(obj_ptr,OTHIEO);
                    obj_ptr->sdice = 5;
                    sum = 300000;
              break;
              default: print(fd, "하나를 선택하시오: ");
                   F_SET(Ply[fd].ply,PREADI); 
                   RETURN(fd, select_arm, 3);
        }
        sprintf(forge2[fd],"%lu",sum);
        print(fd, "\n훌륭한  무기는  담금질로 내구력이  향상됩니다.\n몇번의  담금질을 원합니까?\n");
        print(fd, "1. 100번 오만냥 2. 200번 이십만냥  3. 300번 오십만냥\n4. 400번 백만냥 5. 500번 이백만냥\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI); 
        RETURN(fd, select_arm, 4);
    case 4:
        F_CLR(Ply[fd].ply,PREADI); 
        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);
        switch(low(str[0])) {
              case '1':
                   obj_ptr->shotsmax = 100;
                   obj_ptr->shotscur = 100;
                   sum += 50000;
                   break;
              case '2':
                   obj_ptr->shotsmax = 200;
                   obj_ptr->shotscur = 200;
                   sum += 200000;
                   break;
              case '3':
                   obj_ptr->shotsmax = 300;
                   obj_ptr->shotscur = 300;
                   sum += 500000;
                   break;
              case '4':
                   obj_ptr->shotsmax = 400;
                   obj_ptr->shotscur = 400;
                   sum += 1000000;
                   break;
              case '5':
                   obj_ptr->shotsmax = 500;
                   obj_ptr->shotscur = 500;
                   sum += 2000000;
                   break;
              default: print(fd, "하나를 선택하시오: ");
                   F_SET(Ply[fd].ply,PREADI);  
                   RETURN(fd, select_arm, 4);
        }
        sprintf(forge2[fd],"%lu",sum);
        print(fd, "\n당신의 무기의 이름을 지으십시요.\n");
        print(fd, "나중에 이름을 고칠수 없으니 조심해서 지으셔야 합니다\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);   
        RETURN(fd, select_arm, 5);
    case 5:
        F_CLR(Ply[fd].ply,PREADI);   
        obj_ptr=(object *)atol(forge1[fd]);
        if(strchr(str,'(') || strchr(str,')')) {
            print(fd,"이름에는 괄호가 들어갈수 없습니다.\n이름을 다시 넣으십시요:");
            RETURN(fd,select_arm,5);
        }
        if(strlen(str) > 20) {
                        print(fd, "입력된 이름이 너무  깁니다.\n이름을 다시 넣으십시요(3자이상 20자이하): ");
                        RETURN(fd, select_arm, 5);
        }
        if(strlen(str) < 3) {
                        print(fd, "입력된 이름이 너무  짧습니다.\n이름을 다시 넣으십시요(3자이상 20자이하): ");
                        F_SET(Ply[fd].ply,PREADI);          
                        RETURN(fd, select_arm, 5);
        }
        strncpy(obj_ptr->name, str, 20);
        print(fd, "\n모든것에 만족하십니까? (예/아니오)\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);    
        RETURN(fd, select_arm, 6);
    case 6:
        F_CLR(Ply[fd].ply,PREADI);         
        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);
        if(!strncmp(str,"예",2)) {
                   if (ply_ptr->gold < sum) {
                        print(fd, "\n당신은 그 조건의 무기를  만들 만한 돈이 없습니다.\n주인이 당신에게 \"외상은 안되요~\"라고 말합니다.");
                        free(obj_ptr);
                        break;
                   }
                   add_obj_crt(obj_ptr,ply_ptr);
                   print(fd, "\n주인이 당신에게 새로 제작된 무기를 건네줍니다.");
                   broadcast_rom(fd,   ply_ptr->rom_num,  "\n%M이   무기를  제련하였습니다.",ply_ptr);
                   ply_ptr->gold -= sum;
        }
        else {
            print(fd,"무기 제련을 취소하였습니다.");
            free(obj_ptr);
        }
        break;
    }
    RETURN(fd, command, 1);
}

/*************************************************************************************
*****/

/**********************************************************************/
/*                              newforge제련                             */
/**********************************************************************/
/*  새로운 자신만의 무기를 제작하는 루틴이다     */
/*  아이템 번호 900번부터 도 검 봉 창 궁 순으로 나간다 */
void select_newarm();

int newforge(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room    *rom_ptr;
        object  *obj_ptr;
        int     fd;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;


/*
        print(fd, "버그로 잠시 사용중지합니다\n");
        return(0); 
*/
        if(!F_ISSET(rom_ptr, RFORGE)) {
                print(fd, "여기는 대장간이 아닙니다.");
                return(0);
        }

	    if(!(rom_ptr->rom_num == 611 )) {
		       print(fd, "여기서는 무기를 만들 수가 없습니다.");
 	           return(0);
        }
 
        F_SET(ply_ptr, PNOBRD);    
        select_newarm(fd, 1, NULL);
        F_CLR(ply_ptr, PNOBRD);
        

        return(DOPROMPT);

}

/**********************************************************************/
/*                             select_arm                            */
/**********************************************************************/

/* 무기의 특성을 설계하는 부분이다.             */
char forge1[PMAX][16];
char forge2[PMAX][16];

void select_newarm(fd, param, str)
int fd;
int        param;
char       *str;
{
    int     i;
    long    sum=0;
    int     num[5];
    creature *ply_ptr;
    object  *obj_ptr;

    ply_ptr=Ply[fd].ply;
    switch(param) {
    case 1:
        print(fd,"\n\n");
        print(fd, "어떤 종류의 무기를 원하십니까?\n1. 도 2. 검 3. 봉 4. 창 5. 궁 ?\n:");
        F_SET(Ply[fd].ply,PREADI);   
        output_buf();
        Ply[fd].io->intrpt &= ~1;

        RETURN(fd, select_newarm, 2);
    case 2:
        F_CLR(Ply[fd].ply,PREADI); 

        if(str[0]>='1' && str[0]<='5') {
            if(load_obj(900+str[0]-'1', &obj_ptr) < 0) {
                print(ply_ptr->fd, "Error (%d)\n", 900+str[0]-'1');
                return;
            }
        }
        else {
            print(fd, "하나를 선택하시오: ");
            F_SET(Ply[fd].ply,PREADI); 
            RETURN(fd, select_newarm, 2);
        }

        /* 포인터 obj_ptr을 long형으로 바꿔 저장. */
        sprintf(forge1[fd],"%lu",(long)obj_ptr);

        print(fd, "\n타격치에 영향을 주는 재료를 선택하십시요.\n");
        print(fd, "1.에메랄드 100만냥 2.티타늄 200만냥  3.일루션 300만냥\n:");
        F_SET(Ply[fd].ply,PREADI); 

        RETURN(fd, select_newarm, 3);
    case 3:
        F_CLR(Ply[fd].ply,PREADI); 

        obj_ptr=(object *)atol(forge1[fd]);

        switch(low(str[0])) {
              case '1':
		    F_SET(obj_ptr,OENCHA);
		    obj_ptr->ndice = 4;
                    obj_ptr->sdice = 5;
		    obj_ptr->pdice = 5;
                    sum = 1000000;
                    break;
              case '2':
		    F_SET(obj_ptr,OENCHA);
		    obj_ptr->ndice = 5;
            obj_ptr->sdice = 5;
		    obj_ptr->pdice = 5;
                    sum = 2000000;
                    break;
              case '3':
		    F_SET(obj_ptr,OENCHA);
		    obj_ptr->ndice = 6;
            obj_ptr->sdice = 5;
		    obj_ptr->pdice = 5;
                    sum = 3000000;
              break;
              default: print(fd, "하나를 선택하시오: ");
                   F_SET(Ply[fd].ply,PREADI); 

                   RETURN(fd, select_newarm, 3);
        }

        sprintf(forge2[fd],"%lu",sum);
        print(fd, "\n훌륭한  무기는  담금질로 내구력이  향상됩니다.\n몇번의  담금질을 원합니까?\n");
        print(fd, "1. 100번  백만냥 2. 300번  2백만냥 3. 500번  3백만냥\n4. 700번 4백만냥 5. 900번 5백만냥\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI); 

        RETURN(fd, select_newarm, 4);
    case 4:
        F_CLR(Ply[fd].ply,PREADI); 

        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);

        switch(low(str[0])) {
              case '1':
                   obj_ptr->shotsmax = 100;
                   obj_ptr->shotscur = 100;
                   sum += 1000000;
                   break;
              case '2':
                   obj_ptr->shotsmax = 200;
                   obj_ptr->shotscur = 200;
                   sum += 2000000;
                   break;

              case '3':
                   obj_ptr->shotsmax = 300;
                   obj_ptr->shotscur = 300;
                   sum += 3000000;
                   break;

              case '4':
                   obj_ptr->shotsmax = 400;
                   obj_ptr->shotscur = 400;
                   sum += 4000000;
                   break;

              case '5':
                   obj_ptr->shotsmax = 500;
                   obj_ptr->shotscur = 500;
                   sum += 5000000;
                   break;

              default: print(fd, "하나를 선택하시오: ");
                   F_SET(Ply[fd].ply,PREADI);  

                   RETURN(fd, select_newarm, 4);
        }

        sprintf(forge2[fd],"%lu",sum);

        print(fd, "\n당신의 무기의 이름을 지으십시요.\n");
        print(fd, "나중에 이름을 고칠수 없으니 조심해서 지으셔야 합니다\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);   

        RETURN(fd, select_newarm, 5);
    case 5:
        F_CLR(Ply[fd].ply,PREADI);   

        obj_ptr=(object *)atol(forge1[fd]);

        if(strchr(str,'(') || strchr(str,')')) {
            print(fd,"이름에는 괄호가 들어갈수 없습니다.\n이름을 다시 넣으십시요:");
            RETURN(fd,select_newarm,5);
        }
        if(strlen(str) > 20) {
                        print(fd, "입력된 이름이 너무  깁니다.\n이름을 다시 넣으십시요(3자이상 20자이하): ");
                        RETURN(fd, select_newarm, 5);
        }
        if(strlen(str) < 3) {
                        print(fd, "입력된 이름이 너무  짧습니다.\n이름을 다시 넣으십시요(3자이상 20자이하): ");
                        F_SET(Ply[fd].ply,PREADI);          

                        RETURN(fd, select_newarm, 5);
        }

        strncpy(obj_ptr->name, str, 20);

        print(fd, "\n모든것에 만족하십니까? (예/아니오)\n");
        print(fd, ": ");
        F_SET(Ply[fd].ply,PREADI);    

        RETURN(fd, select_newarm, 6);

    case 6:
        F_CLR(Ply[fd].ply,PREADI);         

        obj_ptr=(object *)atol(forge1[fd]);
        sum=atol(forge2[fd]);

        if(!strncmp(str,"예",2)) {
                   if (ply_ptr->gold < sum) {
                        print(fd, "\n당신은 그 조건의 무기를  만들 만한 돈이 없습니다.\n주인이 당신에게 \"외상은 안되요~\"라고 말합니다.");
                        free(obj_ptr);
                        break;
                   }
                   add_obj_crt(obj_ptr,ply_ptr);

                   print(fd, "\n주인이 당신에게 새로 제작된 무기를 건네줍니다.");
                   broadcast_rom(fd,   ply_ptr->rom_num,  "\n%M이   무기를  제련하였습니다.",ply_ptr);

                   ply_ptr->gold -= sum;
        }
        else {
            print(fd,"무기 제련을 취소하였습니다.");
            free(obj_ptr);
        }
        break;
    }

    RETURN(fd, command, 1);
}

/*************************************************************************************
***************/



void change_class_ok();
int change_class(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        int fd,new_class=0,i;
        room    *rom_ptr;
        fd=ply_ptr->fd;
        rom_ptr=ply_ptr->parent_rom;
        if(F_ISSET(ply_ptr, PBLIND)){
                ANSI(fd, RED);
                print(fd, "당신은 눈이 멀어 직업전환을 할 수 없습니다!");
                ANSI(fd, WHITE);
                return(0);
        }
        if(!F_ISSET(rom_ptr, RTRAIN)) {
                print(fd, "이 곳은 수련장이 아닙니다!");
                return(0);
        }
        for(i=0; i<3; i++) {
                new_class*=2;
                new_class|=F_ISSET(rom_ptr,RTRAIN+i+1)?1:0;
        }
        new_class++;
        if(ply_ptr->class>8) {
            print(fd,"당신은 직업전환을 할 수 없는 직업을 갖고 있습니다.");
            return(0);
        }
        if(new_class==ply_ptr->class) {
                print(fd, "직업전환을 하려면 자신이 수련하는곳에서는 할 수 없습니다.");
                return(0);
        }
        if(ply_ptr->experience<100000) {
                print(fd, "직업전환을 하려면 경험치 10만이 필요합니다.");
                return(0);
        }
        change_class_ok(ply_ptr->fd,1,"");
        return (DOPROMPT);
}
int chg_class_main(ply_ptr)
creature *ply_ptr;
{
        room    *rom_ptr;
        long    goldneeded, expneeded, lv;
        int     fd, i;
        int n,new_class=0;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        for(i=0; i<3; i++) {
                new_class*=2;
                new_class|=F_ISSET(rom_ptr,RTRAIN+i+1)?1:0;
        }
        new_class++;
        ply_ptr->experience -= 100000;
        ply_ptr->class=new_class;
        n = exp_to_lev(ply_ptr->experience);
        while(ply_ptr->level > n)
            down_level(ply_ptr);
        print(fd, "\n당신의 직업이 전환되었습니다.");
                if(F_ISSET(ply_ptr, PFAMIL)) {
                        edit_member(ply_ptr->name,                          ply_ptr->class, 
ply_ptr->daily[DL_EXPND].max, 3);
                }
        return(0);
}
void change_class_ok(fd,param,str)
int fd;
int param;
char *str;
{
    switch(param) {
        case 1:
            print(fd,"직업전환을 하려면 경험치 10만이 필요합니다.\n");
            print(fd,"정말로 직업전환을 하시겠습니까?(예/아니오): ");
            F_SET(Ply[fd].ply,PREADI);
            output_buf();
            Ply[fd].io->intrpt &= ~1;
            RETURN(fd,change_class_ok,2);
        case 2:
            F_CLR(Ply[fd].ply,PREADI);
            if(!strncmp(str,"예",2)) {
                chg_class_main(Ply[fd].ply);
            }
            else
               print(fd,"직업전환이 되지 않았습니다");
            RETURN(fd,command,1);
    }
}
/**********************************************************************/
/*                       magic_stop ( 혈도봉쇄 )                     */
/**********************************************************************/
/* 이기능은 적의 도력을 막는 루틴이다.                              */
/*                                                                    */
int magic_stop(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              chance, m, dur, dmg, fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(cmnd->num < 2) {
                print(fd,
                        "\n누구의 혈도를 봉쇄하실려구요?\n");
                return(0);
        }
        if(ply_ptr->class != RANGER && ply_ptr->class < INVINCIBLE) {
                print(fd,
                        "\n포졸만이 혈도봉쇄를 사용할수 있습니다.\n");
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
                        "\n당신은 %s의 혈도를 봉쇄할수 없습니다.\n",
                        F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                return(0);
        }
        add_enm_crt(ply_ptr->name, crt_ptr);
        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 20L;
        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->dexterity]*6;
        chance = MIN(chance, 80);
		if(ply_ptr->class == RANGER) chance += 10;
        if(mrand(1,100) > chance) {
                print(fd,
                        "\n당신은 적의 혈도를 재빨리 봉쇄했습니다.\n그러나 적이 살짝 피해 빗나갔습니다.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M이 적의 혈도를 재빨리 봉쇄했습니다.\n그러나 %M가 살짝 피했습니다.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }
                print(fd,
                        "\n당신은 적의 혈도를 재빨리 봉쇄했습니다.\n적의 혈도를 짚어 주문을 봉쇄했습니다.\n ",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M이 적의 혈도를 재빨리 봉쇄했습니다.\n%M의 혈도가 짚혀 주문이 봉쇄되었습니다.\n",
                                    ply_ptr, crt_ptr);
                if (mrand(25,100) <= chance) {
                        dmg = crt_ptr->hpcur /2;
                        print(fd,"%M의 급소를 짚어서 %d의 피해를 입혔습니다.\n",  crt_ptr, dmg );
                        broadcast_rom(fd, ply_ptr->rom_num,
                                 "%M이 %m의 급소를 짚어서 %d의 피해를 입혔습니다.\n", ply_ptr, crt_ptr, dmg);
                        crt_ptr->hpcur -= dmg;
                        m = MIN(crt_ptr->hpcur, dmg);
                        add_enm_dmg(ply_ptr->name, crt_ptr, m);
                        if(crt_ptr->hpcur < 1) {
                              print(fd, "\n당신은 %M%j 죽였습니다.", crt_ptr,"3");
                              broadcast_rom(fd, ply_ptr->rom_num,
                                         "\n%M%j    %M%j    죽였습니다.",    ply_ptr,"1", crt_ptr,"3");
                              die(crt_ptr, ply_ptr);
                         }
                         else
                              check_for_flee(ply_ptr, crt_ptr);
                }
 
                dur = bonus[ply_ptr->dexterity]*2 + dice(2,6,0);

                if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
                   (crt_ptr->type   !=   PLAYER  &&    (F_ISSET(crt_ptr,   MRMAGI)   || F_ISSET(crt_ptr, MRBEFD))))
                        dur = 5;
                else
                        dur = MAX(15, dur);

                crt_ptr->lasttime[LT_SPELL].ltime = time(0);
                dur = MIN(20, dur);
                crt_ptr->lasttime[LT_SPELL].interval = dur;

        return(0);
}
/**********************************************************************/
/*                       중독                                         */
/**********************************************************************/
int poison_mon(ply_ptr, cmnd)
creature *ply_ptr;
cmd            *cmnd;
{
        creature *crt_ptr;
        room           *rom_ptr;
        long            i, t;
        int              dmg, dur, chance, m, fd;
        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;
        if(cmnd->num < 2) {
                print(fd,
                        "\n누구를 중독시키시려고요?\n");
                return(0);
        }
        if(ply_ptr->class != ASSASSIN && ply_ptr->class < INVINCIBLE) {
                print(fd,
                        "\n자객만이 가능합니다.\n");
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
                        "\n당신은 %s를 중독시킬수 없습니다.\n",
                        F_ISSET(crt_ptr, MMALES) ? "그":"그녀");
                return(0);
        }
        add_enm_crt(ply_ptr->name, crt_ptr);
        ply_ptr->lasttime[LT_TURNS].ltime = t;
        ply_ptr->lasttime[LT_ATTCK].ltime = t;
        ply_ptr->lasttime[LT_TURNS].interval = 15L; 
        chance = (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*20 +
                 bonus[ply_ptr->dexterity]*6;
        chance = MIN(chance, 80);
        if(mrand(1,100) > chance) {
                print(fd,
                        "\n당신은 적에게 독을 뿌렸습니다.\n그러나 적이 살짝 피해 실패했습니다.\n",
                        crt_ptr);
                broadcast_rom(fd, ply_ptr->rom_num,
                                "\n%M이 적에게 독을 뿌렸습니다.\n그러나  %M가 살짝 피했습니다.\n",
                                ply_ptr, crt_ptr);
                return(0);
        }
        print(fd,
                "\n당신은 적에게 독을 뿌렸습니다.\n%M의 몸이 중독되었습니다.\n ",
                        crt_ptr);
        broadcast_rom(fd, ply_ptr->rom_num,
                "\n%M이 적에게 독을 뿌렸습니다.\n%M의 몸이 중독되었습니다.\n",
                ply_ptr, crt_ptr);
        F_SET(crt_ptr, MPOISN);
                if (mrand(10,100) <= chance) {
                        dmg = crt_ptr->hpmax /3 ;
                        print(fd,"%M을 중독을  시켜서 %d의  피해를 입혔습니다.\n",  crt_ptr, dmg );
                        broadcast_rom(fd, ply_ptr->rom_num,
                                 "%M이 %m을 중독을  시켜서 %d의  피해를 입혔습니다.\n", ply_ptr, crt_ptr, dmg);
                        crt_ptr->hpcur -= dmg;
                        m = MIN(crt_ptr->hpcur, dmg);
                        add_enm_dmg(ply_ptr->name, crt_ptr, m);
                        if(crt_ptr->hpcur < 1) {
                              print(fd, "\n당신은 %M%j 죽였습니다.", crt_ptr,"3");
                              broadcast_rom(fd, ply_ptr->rom_num,
                                         "\n%M%j    %M%j    죽였습니다.",    ply_ptr,"1", crt_ptr,"3");
                              die(crt_ptr, ply_ptr);
                         }
                         else
                              check_for_flee(ply_ptr, crt_ptr);
                }
 
                dur = bonus[ply_ptr->dexterity]*2 + dice(2,6,0);

                if((crt_ptr->type == PLAYER && F_ISSET(crt_ptr, PRMAGI)) ||
                   (crt_ptr->type   !=   PLAYER  &&    (F_ISSET(crt_ptr,   MRMAGI)   || F_ISSET(crt_ptr, MRBEFD))))
                        dur = 3;
                else
                        dur = MAX(5, dur);

                crt_ptr->lasttime[LT_SPELL].ltime = time(0);
                dur = MIN(10, dur);
                crt_ptr->lasttime[LT_SPELL].interval = dur;
        
        return(0);
}


