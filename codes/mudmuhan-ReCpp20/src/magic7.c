/*
 * MAGIC7.C:
 *
 *  Additional spell-casting routines.
 */

#include "mstruct.h"
#include "mextern.h"

/************************************************************************/
/*              resist_cold(방한진)             */
/************************************************************************/

/* This function allows players to cast the resist cold spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int resist_cold(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n당신의 도력이 부족합니다.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SRCOLD) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }



    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_RCOLD].ltime = time(0);
        F_SET(ply_ptr, PRCOLD);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M이 불타오르는 부적을 삼키며 방한주를 외웁니다.\n그의 주위에 오행중 화의 수호령들이 진을 형성하며 \n주위를 둘러쌉니다.", ply_ptr);
        if(how == CAST) {
            print(fd, "\n당신은 불타오르는 부적을 삼키며 방한주룰 외웁니다.\n당신의 주위에 오행중 화의 수호령들이 진을 형성하며\n주위를 둘러쌉니다.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
    if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            ply_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }
        else {
            print(fd, "\n당신의 주위에 오행중 화의 수호령들이 진을 형성하며\n주위를 둘러쌉니다.\n");
            ply_ptr->lasttime[LT_RCOLD].interval = 1200L;
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
            print(fd, "\n그런 사람이 존재하지 않습니다 .\n");
            return(0);
        }

        F_SET(crt_ptr, PRCOLD);
        crt_ptr->lasttime[LT_RCOLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "\n%M이 %M의 입에 불타오르는 부적을 집어넣으며 \n방한진 주문을 외웁니다.", 
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "\n%M이 당신의 입에 불타오르는 부적을 집어 넣으며\n방한주룰 외웁니다.\n당신의 주위에 오행중 화의 수호령들이 진을 형성하며\n주위를 둘러쌉니다.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "\n당신은 %M의 입에 불타오르는 부적을 집어\n넣으며 방한주룰 외웁니다.\n그의 주위에 화의 수호령들이 진을 형성하며 주위를\n둘러쌉니다.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_RCOLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            crt_ptr->lasttime[LT_RCOLD].interval += 800L;
        }                                
        }

        else {
            print(fd, "\n%M의 주위에 화의 수호령들이 진을 형성하며\n주위를 둘러쌉니다.\n", crt_ptr);
            crt_ptr->lasttime[LT_RCOLD].interval = 1200L;
        }

        return(1);
    }

}


/************************************************************************/
/*                          Breathe-water(수생술)                               */
/************************************************************************/

/* This function allows players to cast the resist cold spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int breathe_water(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n당신의 도력이 부족합니다.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SBRWAT) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }


    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_BRWAT].ltime = time(0);
        F_SET(ply_ptr, PBRWAT);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M이 크게 호흡을 들이키며서 수생술의 \n주문을 외웠습니다.\n그의 가슴이 평소보다 두배나 커져 물속에서 오랫동안\n견딜 수 있을 것 같습니다.\n", ply_ptr);
        if(how == CAST) {
            print(fd, "\n당신은 크게 호흡을 들이키며서 수생술의 주문을 외웁니다.\n가슴이 평소보다 두배나 커져 물속에서 오랫동안\n견딜수 있을 것 같습니다.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            ply_ptr->lasttime[LT_BRWAT].interval += 800L;
        }                                
        }
        else {
            print(fd, "\n당신의 가슴이 평소보다 두배나 커져 물속에서 오랫동안\n견딜수 있을 것 같습니다.\n");
            ply_ptr->lasttime[LT_BRWAT].interval = 1200L;
        }
        return(1);
    }
    else {

        if(how == POTION) {
            print(fd, "\n이 물건은 자신에게만 사용할수 있습니다.\n");
            return(0);
        }

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);
        if(!crt_ptr) {
            print(fd, "\n그런 사람이 존재하지 않습니다 .\n");
            return(0);
        }

        F_SET(crt_ptr, PBRWAT);
        crt_ptr->lasttime[LT_BRWAT].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "\n%M이 %M에게 수생부를 먹이며 주문을 외웠습니다.\n그의 가슴이 평소보다 두배나 커져 물속에서 오랫동안\n견딜수 있을 것 같습니다.\n",
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "\n%M이 당신에게 수생부를 먹이며 주문을 외웠습니다.\n당신의 가슴이 평소보다 두배나 커져 물속에서 오랫동안\n견딜 수 있을 것 같습니다.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "\n당신은 %M에게 수생부를 먹이며 주문을 외웁니다.\n그의 가슴이 평소보다 두배나 커져 물속에서 오랫동안\n견딜 수 있을 것 같습니다.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_BRWAT].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            crt_ptr->lasttime[LT_BRWAT].interval += 800L;
        }                                
        }

        else {
            print(fd, "\n%M의 가슴이 평소보다 두배 커져 물속에서 \n오랫동안 견딜수 있을 것 같습니다.\n", crt_ptr);
            crt_ptr->lasttime[LT_BRWAT].interval = 1200L;
        }

        return(1);
    }

}


/************************************************************************/
/*                      Stone-Shield(지방호)                                    */
/************************************************************************/

/* This function allows players to cast the earth shield spell.  It will */
/* allow the player to resist fire breathed on them by dragons and  */
/* other breathers.                         */

int earth_shield(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd;

    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(ply_ptr->mpcur < 12 && how == CAST) {
        print(fd, "\n당신의 도력이 부족합니다.\n");
        return(0);
    }

    if(!S_ISSET(ply_ptr, SSSHLD) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }
if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 12;
                return(0);
        }

    if(cmnd->num == 2) {
        ply_ptr->lasttime[LT_SSHLD].ltime = time(0);
        F_SET(ply_ptr, PSSHLD);
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M이 벽부선을 던지며 지방호 주문을 외웠습니다.\n땅에서 오행중 토의 수호령들이 올라와 그의 주위에\n진을 형성했습니다.\n", ply_ptr);
        if(how == CAST) {
            print(fd, "\n당신은 벽부선을 던지며 지방호 주문을 외웁니다.\n땅에서 오행중 토의 수호령들이 올라와 당신주위에\n진을 형성합니다.\n");
            ply_ptr->mpcur -= 12;
            ply_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주술력을 강화시킵니다.\n");
            ply_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }
        else {
            print(fd, "\n땅에서 오행중 토의 수호령들이 올라와 당신주위에\n진을 형성합니다.\n");
            ply_ptr->lasttime[LT_SSHLD].interval = 1200L;
        }
        return(1);
    }
    else {

        if(how == POTION) {
            print(fd, "\n이 물건은 자신에게만 사용할수 있습니다.\n");
            return(0);
        }

        cmnd->str[2][0] = up(cmnd->str[2][0]);
        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                   cmnd->str[2], cmnd->val[2]);
        if(!crt_ptr) {
            print(fd, "\n그런 사람이 존재하지 않습니다.\n");
            return(0);
        }

        F_SET(crt_ptr, PSSHLD);
        crt_ptr->lasttime[LT_SSHLD].ltime = time(0);
        broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num, 
            "\n%M이 %M에게 토흙을 뿌리며 지방호 주문을 외웁니다.\n땅에서 오행중 토의 수호령들이 올라와 그의 주위에\n진을 형성합니다.\n",
            ply_ptr, crt_ptr);
        print(crt_ptr->fd, "\n%M이 당신에게 토흙을 뿌리며 지방호 주문을 외웠습니다.\n갑자기 땅에서 오행중 토의 수호령들이 올라와 당신주위에\n진을 형성했습니다.\n", ply_ptr);

        if(how == CAST) {
            print(fd, "\n당신은 %M에게 땅방패 주문을 외웠습니다.\n",
                crt_ptr);
            ply_ptr->mpcur -= 12;
            crt_ptr->lasttime[LT_SSHLD].interval = MAX(300, 1200 +
                bonus[ply_ptr->intelligence]*600);
        if (F_ISSET(ply_ptr->parent_rom,RPMEXT)){
            print(fd,"\n이 방의 기운이 당신의 주문을 강화시킵니다.\n");
            crt_ptr->lasttime[LT_SSHLD].interval += 800L;
        }                                
        }

        else {
            print(fd, "\n땅에서 오행중 토의 수호령들이 올라와 %M의\n주위에 진을 형성합니다.\n", crt_ptr);
            crt_ptr->lasttime[LT_SSHLD].interval = 1200L;
        }

        return(1);
    }

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                  locate_person(천리안)                               */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int locate_player(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    room        *rom_ptr;
    int     fd, chance;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(fd < 0) return(0);

    if(!S_ISSET(ply_ptr, SLOCAT) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }
    if(ply_ptr->mpcur < 15 && how == CAST) {
        print(fd, "\n당신의 도력이 부족합니다.\n");
        return(0);
    }


 
    if(cmnd->num < 3) {
        print(fd, "\n누구와 연결합니까?\n");
        return(0);
    }
 
    lowercize(cmnd->str[2], 1);
    crt_ptr = find_who(cmnd->str[2]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "\n그런 사람은 존재하지 않습니다.\n");
        return(0);
    }
    if(crt_ptr->class == DM && ply_ptr->class != DM) {
        print(fd,"그 사람의 정신력이 너무 높아 투시를 할 수 없습니다.");
        return 0;
    }
    if(F_ISSET(crt_ptr, PMARRI)) {
    	print(fd,"그 사람의 사생활은 엿볼 수가 없습니다.");
    	return 0;
    }
    if(spell_fail(ply_ptr)) {
                if(how==CAST)
                        ply_ptr->mpcur -= 15;
                return(0);
        } 
        broadcast_rom(fd, ply_ptr->rom_num, 
            "\n%M이 천리안 주문을 외웠습니다.\n", ply_ptr);
        if(how == CAST) {
            print(fd, "\n당신의 마음을 %M에게 집중했습니다.\n",
                crt_ptr);
            ply_ptr->mpcur -= 15;
        }

    chance =  50 + (((ply_ptr->level+3)/4) - ((crt_ptr->level+3)/4))*5  +
        (bonus[ply_ptr->intelligence] - bonus[crt_ptr->intelligence])*5; 

    chance += (ply_ptr->class == MAGE) ? 5 : 0;
    chance = MIN(85,chance);

    if ((crt_ptr->class < SUB_DM) && (mrand(1,100) < chance)){
        display_rom(ply_ptr,crt_ptr->parent_rom);   
        chance += (crt_ptr->class == MAGE) ? 5 : 0;
        chance = MIN(85,chance);

        chance = 60 + (((crt_ptr->level+3)/4) - ((ply_ptr->level+3)/4))*5  +
           (bonus[crt_ptr->intelligence] - bonus[ply_ptr->intelligence])*5;
        chance += (crt_ptr->class == MAGE) ? 5 : 0;
        chance = MIN(85,chance);

        if  (mrand(1,100) < chance)
                print(crt_ptr->fd, "\n%M이 당신의 눈으로 주위를 보고 있습니다.\n",
                    ply_ptr);
    }
    else{
        print(fd,"\n당신의 정신은 연결될수 없습니다.\n");

        chance = 65 + (((crt_ptr->level+3)/4) - ((ply_ptr->level+3)/4))*5  +
           (bonus[crt_ptr->intelligence] - bonus[ply_ptr->intelligence])*5;

        if  (mrand(1,100) < chance)
        print(crt_ptr->fd, "\n%M이 당신의 눈으로 보려합니다.\n",
            ply_ptr);

    }
 
    return(1);
}
 
 
/**********************************************************************/
/*                          drain_exp(백치술)                               */
/**********************************************************************/
/* The spell drain_exp causes a player to lose a selected amout of  *
 * exp.  When a player loses exp, the player's magical realm and    *
 * weapon procifiency will reflect the change.  This spell is not   *
 * intended to be learned or casted by a player.  The 4th parameter *
 * if given, will base the exp loss on the DnS damage of the object */

int drain_exp(ply_ptr, cmnd, how, obj_ptr)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
object  *obj_ptr;
{
    room        *rom_ptr;
    creature    *crt_ptr;
    int     fd;
    long    loss;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(!S_ISSET(ply_ptr, SDREXP) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }
 
    if(how == CAST && ply_ptr->class < DM) {
        print(fd, "\n그런 주문을 외울수 없습니다.\n");
        return(0);
        }      

    if(how ==  SCROLL){
        print(fd, "\n그런 주문을 외울수 없습니다.\n");
        return(0);
        }      

    /* drain exp on self */
    if(cmnd->num == 2) {
 
        if(how == POTION || how == WAND) 
            loss= dice(((ply_ptr->level+3)/4), ((ply_ptr->level+3)/4),(((ply_ptr->level+3)/4))*10);

        else if(how == CAST)
            loss = dice(((ply_ptr->level+3)/4), ((ply_ptr->level+3)/4),1)*10;
            loss = MIN(loss, ply_ptr->experience);

        if(how == CAST || how == WAND)  {
            print(fd, "\n당신은 갑자기 멍청해 지면서 지금까지 싸워왔던 경험들이\n생각나지 않습니다.\n!!악~~~ 경험치가 얼마인지도 모르겠다.!!\n");
            print(fd, "\n당신은 %d만큼의 경험들이 생각나지 않습니다.\n",loss);
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M이 자신에게 백치술 주문을 외웁니다.\n그는 갑자기 멍청해진듯 주위를 두리번 거립니다.\n",
                      ply_ptr);
        }
        else if(how == POTION){
            print(fd, "\n당신은 갑자기 멍청해 지면서 지금까지 싸워왔던 경험들이\n생각나지 않습니다.\n!!악~~~ 경험치가 얼마인지도 모르겠다.!!\n");
            print(fd, "\n당신은 %d만큼의 경험들이 생각나지 않습니다.\n",loss);
        }
            ply_ptr->experience -= loss;    
            lower_prof(ply_ptr,loss);
 
    }
 
    /* energy drain a monster or player */
    else {
 
        if(how == POTION || how == WAND) 
            loss = mdice(obj_ptr);
        else if(how == CAST)
            loss = dice(((ply_ptr->level+3)/4), ((ply_ptr->level+3)/4),1)*30;

        if(how == POTION) {
            print(fd, "\n이 물건은 자신에게만 사용할수 있습니다.\n");
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
                print(fd, "\n그런 사람이 존재하지 않습니다 .\n");
                return(0);
            }
        }
 
 
        loss = MIN(loss, crt_ptr->experience);
        if(how == CAST || how == WAND) {
            print(fd, "\n당신은 %M에게 백치술의 주문을 외웁니다.\n그는 갑자기 멍청해진듯 주위를 두리번 거립니다.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 %M에게 백치술의 주문을 외웁니다.\n그는 갑자기 멍청해진듯 주위를 두리번 거립니다.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd,
                     "\n%M이 당신에게 백치술의 주문을 외웁니다.\n당신은 갑자기 멍청해지면서 지금까지 싸워왔던 경험들이\n생각나지 않습니다.\n!!악~~~ 경험치가 얼마인지도 모르겠다.!!\n",
         ply_ptr);
           print(crt_ptr->fd,"\n당신은 %d만큼의 경험들이 생각나지 않습니다.\n",loss);
           print(fd,"\n%M이 싸웠던 얼마간의 경험들을 잊어버린것\n같습니다.\n",crt_ptr);
        }
 
           crt_ptr->experience -= loss;    
            lower_prof(crt_ptr,loss);
    }
 
    return(1);
 
}
               
/**********************************************************************/
/*                      remove disease(치료)                            */
/**********************************************************************/
 
int rm_disease(ply_ptr, cmnd, how)
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
        print(fd, "\n당신의 도력이 부족합니다.\n");
        return(0);
    }
 
        if(ply_ptr->class != CLERIC &&
           ply_ptr->class < INVINCIBLE && how == CAST) {
                print(fd, "\n이 주술은 불제자만이 사용할 수 있습니다.\n");
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
            print(fd, "\n당신은 생사과를 먹으며 치료 주문을 외웁니다.\n병마에 시달리던 당신의 몸이 활기를 띄기 시작합니다.\n");
            broadcast_rom(fd, ply_ptr->rom_num, 
                      "\n%M이 생사과를 먹으며 치료 주문을 외웠습니다.\n병마에 시달리던 그의 몸이 활기를 띄기 시작합니다.\n",
                      ply_ptr);
        }
        else if(how == POTION & F_ISSET(ply_ptr, PDISEA))
            print(fd, "\n병마에 시달리던 당신의 몸이 활기를 띄기 시작합니다.\n");
        else if(how == POTION)
            print(fd, "\n당신의 병이 해소되어 몸이 거뜬해집니다.\n");
 
        F_CLR(ply_ptr, PDISEA);
 
    }
 
    else {
 
        if(how == POTION) {
            print(fd, "\n이 물건은 자신에게만 사용할수 있습니다.\n");
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
                print(fd, "\n그런사람이 존재하지 않습니다 .\n");
                return(0);
            }
        }
 
        if(how == CAST) 
            ply_ptr->mpcur -= 12;
 
        F_CLR(crt_ptr, PDISEA);
 
        if(how == CAST || how == SCROLL || how == WAND) {
            print(fd, "\n당신은 %M의 혈도를 누르고 내공의 힘을 통해\n치료를 시작합니다.\n그의 몸에 막혀 있던 혈이 풀리면서 차츰 활기를 \n띄기 시작합니다.\n", crt_ptr);
            broadcast_rom2(fd, crt_ptr->fd, ply_ptr->rom_num,
                       "\n%M이 %M의 혈도를 누르고 내공의 힘을 통해\n치료를 시작합니다.\n그의 몸이 차츰 활기를 띄기 시작하는 것이 보입니다.\n",
                       ply_ptr, crt_ptr);
            print(crt_ptr->fd, "\n%M이 당신의 혈도를 누르고 내공의 힘을 통해 치료를 시작합니다.\n당신의 몸에 기공이 들어와 막힌 혈을 풀자 차츰 \n활기를 띄기 시작합니다.\n", ply_ptr);
        }
 
    }
 
    return(1);
 
}
 


/************************************************************************/
/*                           object_send(전송)                               */
/************************************************************************/
/* The send object spell allows a mage  or DM/Caretaker to magically     *
 * transport an object to another login (detectable) player.  The maxium *
 * weight of the object being transported is based on the player's       *
 * level and intellignece. */

int object_send(ply_ptr, cmnd, how)
creature    *ply_ptr;
cmd     *cmnd;
int     how;
{
    creature    *crt_ptr;
    object      *obj_ptr;
    room        *rom_ptr;
    otag        *otag_ptr;
    int         fd, cost;
 
    fd = ply_ptr->fd;
    rom_ptr = ply_ptr->parent_rom;

    if(!S_ISSET(ply_ptr, STRANO) && how == CAST) {
        print(fd, "\n당신은 아직 그런 주문을 터득하지 못했습니다.\n");
        return(0);
    }

    if(ply_ptr->class != MAGE && ply_ptr->class < INVINCIBLE && how == CAST) {
       print(fd, "\n이 주술은 도술사들만이 사용할 수 있습니다.\n");
       return(0);
    }                         

    if(ply_ptr->level <20){
        print(fd, "\n당신의 능력이 부족합니다.\n");
        return(0);
    }
 
    if(cmnd->num < 4) {
        print(fd, "\n누구에게 무얼 보냅니까?\n");
        return(0);
    }

    lowercize(cmnd->str[3], 1);
    crt_ptr = find_who(cmnd->str[3]);
 
    if(!crt_ptr || F_ISSET(crt_ptr, PDMINV) ||
       (F_ISSET(crt_ptr, PINVIS) && !F_ISSET(ply_ptr, PDINVI))) {
        print(fd, "\n그런 사람이 존재하지 않습니다 .\n");
        return(0);
    }
 
    obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
        cmnd->str[2], cmnd->val[2]);
 
    if(!obj_ptr) {
        print(fd, "\n그런 물건이 존재하지 않습니다.\n");
        return(0);
    }

        if(obj_ptr->questnum) {
                print(fd, "\n임무에 관련되어 다른자에게 보낼 수 없습니다.\n");
                return(0);
        }

        if(F_ISSET(obj_ptr, ONEWEV)) {
        		print(fd, "\n이벤트 아이템은 다른자에게 보낼 수 없습니다.\n");
        		return(0);
        }

    if(obj_ptr->first_obj) {
        for(otag_ptr=obj_ptr->first_obj; otag_ptr; otag_ptr=otag_ptr->next_tag) {
            if(otag_ptr->obj->questnum && ply_ptr->class < DM) {
        		print(fd, "\n전송에 실패했습니다.\n");
        		return(0);
            }
            if(F_ISSET(otag_ptr->obj, ONEWEV) && ply_ptr->class < DM) {
        		print(fd, "\n전송에 실패했습니다.\n");
        		return(0);
            }
        }
    }

    cost = 5 + bonus[ply_ptr->intelligence] + (((ply_ptr->level+3)/4) - 5)*2;
    if (obj_ptr->weight > cost){
        print(fd,"\n%I의 전송은 당신의 능력으로 너무 무거워 \n보낼 수 없습니다.\n", obj_ptr);
        return(0);
    }

    cost = 8 + (obj_ptr->weight)/4;
    if(ply_ptr->mpcur < cost && how == CAST) {
        print(fd, "\n당신의 도력이 부족합니다.\n");
        return(0);
    } else if(how == CAST) {
            ply_ptr->mpcur -= cost;
        }
        if(spell_fail(ply_ptr)) {
                return(0);
        }
    

    if (max_weight(crt_ptr) < weight_ply(crt_ptr)+obj_ptr->weight){
        print(fd,"\n%M은 %1i을 가질 수 없습니다 .\n",crt_ptr,obj_ptr);
        return(0);
    }
    del_obj_crt(obj_ptr, ply_ptr);
    add_obj_crt(obj_ptr, crt_ptr);

    savegame_nomsg(ply_ptr);
    if(crt_ptr->type==PLAYER) savegame_nomsg(crt_ptr);

    if(!F_ISSET(ply_ptr, PDMINV)){
            broadcast_rom(fd, ply_ptr->rom_num, "\n%M이 물건을 어디론가로 날려버렸습니다.\n", ply_ptr);
    }
    print(fd,"\n당신이 %i에 정신집중을 하면서 전송주를 외웁니다.\n그 물건이 공중으로 떠오르면서 어디론가로 총알같이\n날라갑니다.\n",
        obj_ptr);
    print(fd, "\n당신은 %1i을 %M에게 보냈습니다.\n",
        obj_ptr, crt_ptr);
    if(!F_ISSET(ply_ptr, PDMINV)){
         print(crt_ptr->fd, "\n갑자기 하늘에서 무언가가 하늘에서 뚝 떨어졌습니다.\n자세히 보니 %1i인데, %M이 물건을 전송한 것 같습니다.\n", obj_ptr, ply_ptr);
    }
    return(1);
 
}


