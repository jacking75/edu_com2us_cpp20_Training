/*
 * SPECIAL1.C:
 *
 *  User routines dealing with special routines.
 *
 *  Copyright (C) 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

int combo_box();
int AT_WAR, CALLWAR1, CALLWAR2;

/************************************************************************/
/*              special_obj             */
/************************************************************************/

#define SP_INVENTORY 0
#define SP_EQUIPMENT 1
#define SP_ROOM      2

int special_obj(ply_ptr, cmnd, special)
creature    *ply_ptr;
cmd     *cmnd;
int     special;
{
	object  *obj_ptr;
	room    *rom_ptr;
	int fd, n, where;
	char    str[80], str2[160], match=0, i;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	obj_ptr = find_obj(ply_ptr, ply_ptr->first_obj,
		   cmnd->str[1], cmnd->val[1]);
	where = SP_INVENTORY;

	if(!obj_ptr || !cmnd->val[1]) {
		for(n=0; n<MAXWEAR; n++) {
			if(!ply_ptr->ready[n]) continue;
			if(EQUAL(ply_ptr->ready[n], cmnd->str[1]))
				match++;
			else continue;
			if(match == cmnd->val[1] || !cmnd->val[1]) {
				obj_ptr = ply_ptr->ready[n];
				where = SP_EQUIPMENT;
				break;
			}
		}
	}

	if(!obj_ptr) {
		obj_ptr = find_obj(ply_ptr, rom_ptr->first_obj,
				   cmnd->str[1], cmnd->val[1]);
		where = SP_ROOM;
	}

	if(!obj_ptr) {
		print(fd, "그같은 물건이 없습니다.\n");
		return(-1);
	}

	if(obj_ptr->special != special)
		return(-2);

	switch(obj_ptr->special) {
	case SP_MAPSC:
		strcpy(str, obj_ptr->name);
		for(i=0; i<strlen(str); i++)
			if(str[i] == ' ') str[i] = '_';
		sprintf(str2, "%s/%s", OBJPATH, str);
		view_file(fd, 1, str2);
		return(DOPROMPT);
	case SP_COMBO:
		return(combo_box(ply_ptr, obj_ptr));
		/* 선전포고 루틴 부름 */
	default:
		print(fd, "아무것도 없습니다.\n");
	}

	return(0);
}

/************************************************************************/
/*              special_cmd             */
/************************************************************************/

int special_cmd(ply_ptr, special, cmnd)
creature    *ply_ptr;
int     special;
cmd     *cmnd;
{
	int n, fd;

	fd = ply_ptr->fd;

	switch(special) {
	case SP_MAPSC:
	case SP_COMBO:
		if(cmnd->num < 2) {
			print(fd, "무얼 누릅니까?\n");
			return(0);
		}
		n = special_obj(ply_ptr, cmnd, special);
		if(n == -1) return(0);
		if(n == -2) {
			print(fd, "무얼 하려고 하는데요?.\n");
			return(0);
		}
		else return(n);
	default:
		print(ply_ptr->fd, "아무런 일도 일어나지 않습니다.\n");
		return(0);
	}
}

/************************************************************************/
/*              combo_box               */
/************************************************************************/

int combo_box(ply_ptr, obj_ptr)
creature    *ply_ptr;
object      *obj_ptr;
{
	xtag    *xp;
	room    *rom_ptr;
	char    str[80];
	int fd, dmg, i;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	str[0] = obj_ptr->sdice+'0'; str[1] = 0;

	if(obj_ptr->ndice == 1 || strlen(Ply[fd].extr->tempstr[3]) > 70)
		strcpy(Ply[fd].extr->tempstr[3], str);
	else
		strcat(Ply[fd].extr->tempstr[3], str);
 
	print(fd, "Click.\n");
	broadcast_rom(fd, ply_ptr->rom_num, "%M이 %i를 눌렀습니다.", ply_ptr, obj_ptr);

	if(strlen(Ply[fd].extr->tempstr[3]) >= strlen(obj_ptr->use_output)) {
		if(strcmp(Ply[fd].extr->tempstr[3], obj_ptr->use_output)) {
			dmg = mrand(20,40);
			ply_ptr->hpcur -= dmg;
			print(fd, "당신은 %d점의 피해를 입었습니다!\n", dmg);
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M이 %i로부터 피해를 입었습니다!", ply_ptr, obj_ptr);
			Ply[fd].extr->tempstr[3][0] = 0;

			if(ply_ptr->hpcur < 1) {
				print(fd, "당신은 죽었습니다.\n");
				die(ply_ptr, ply_ptr);
			}
		}

		else {
			for(i=1, xp=rom_ptr->first_ext;
				xp && i < obj_ptr->pdice;
				i++, xp = xp->next_tag) ;
			if(!xp) return(0);
			print(fd, "당신은 %s를 열었습니다!\n", xp->ext->name);
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M이 %s를 열었습니다!", ply_ptr, xp->ext->name);
			F_CLR(xp->ext, XLOCKD);
			F_CLR(xp->ext, XCLOSD);
			xp->ext->ltime.ltime = time(0);
		}
	}
	return(0);
}

/************************************************************************/
/*                                  call_war                            */
/************************************************************************/

int call_war(ply_ptr, cmnd)
creature    *ply_ptr;
cmd			*cmnd;
{
	int fd, i;
	int fnum, num, ftotal=16, fal=0;
	creature *crt_ptr;

	fd = ply_ptr->fd;
	fnum = ply_ptr->daily[DL_EXPND].max;
      
      if(!F_ISSET(ply_ptr,PFMBOS) || fnum == 0) {
         print(fd, "당신은 선전 포고할 권리가 없습니다.\n");
         return(0);
      }
	if(cmnd->num != 2) {
		print(fd, "어느 패거리와 전쟁을 하시려고요?");
		return(0);
	}

	for(num=1; num<ftotal; num++) {
		if(!strcmp(cmnd->str[1], family_str[num])) {
			crt_ptr = find_who(fmboss_str[num]);
			if(!crt_ptr) {
				print(fd, "상대편의 두목인 %s님이 이용중이 아닙니다.", fmboss_str[num]);
				return(0);
			}
		break;
		}
		fal++;
	}
	if(fal >= 15) {
		print(fd, "그런 패거리는 없습니다.");
		return(0);
	}
	if(fnum == num) {
		print(fd, "자기 자신들과 싸우시려고요?");
		return(0);
	}

	if(!AT_WAR) {
		  if(!CALLWAR2) {
			broadcast_all("\n### %s 패거리가 %s에게 선전포고를 합니다.\n\n",
					family_str[fnum], family_str[num]);
		CALLWAR1 = fnum;
		CALLWAR2 = num;
		return(0);
		}
		if(CALLWAR1 == fnum) {
			broadcast("\n### %s 패거리에서 선전포고를 취소합니다.\n", family_str[fnum]);
			CALLWAR1 = CALLWAR2 = 0;
			return(0);
		}
		if(CALLWAR2 == fnum) {
			if(CALLWAR1 != num) {
				print(fd, "다른 패거리에서 전쟁을 신청해두고 있습니다.");
				return(0);
			}
			else {
				broadcast("\n### %s 패거리에서 선전포고를 받아들였습니다.\n",
					family_str[fnum], family_str[num]);
				AT_WAR = (fnum*16) + num;
				return(0);
			}
		}
		else {
			print(fd, "다른 패거리에서 먼저 전쟁을 준비중입니다.");
			return(0);
		}
	 }

	else
	   print(fd, "벌써 전쟁중입니다.\n");

	return(0);
}

void check_item(ply_ptr)
creature    *ply_ptr;
{
    otag *obj_tag=NULL;
    otag *obj_next=NULL;
    otag *op=NULL;
    object *obj_haha=NULL;
    object *cnt_ptr=NULL;
    int i,n,tes=0;

    for(i=0; i<MAXWEAR; i++) {
        if(!ply_ptr->ready[i]) continue;
        if(is_bad_item(ply_ptr->ready[i])) {
            if(ply_ptr->ready[i]->shotsmax > 4999) {
                log_pl("나쁜 %s의 %s : 없애버렸어요.!\n", ply_ptr->name, ply_ptr->ready[i]->name);
                free_obj(ply_ptr->ready[i]);
                ply_ptr->ready[i]=NULL;
                save_ply(ply_ptr->name, ply_ptr);
            }
            free_obj2(ply_ptr->ready[i]);
        }
    }

    if(!ply_ptr->first_obj) return;

    for(obj_tag=ply_ptr->first_obj; obj_tag; obj_tag=obj_next) {
        obj_next=obj_tag->next_tag;
        if(F_ISSET(obj_tag->obj, OCONTN)) check_contain(obj_tag->obj);
        if(is_bad_item(obj_tag->obj)) {
            if(obj_tag->obj->shotsmax > 4999) {
                log_pl("나쁜 %s의 %s : 없애버렸어요.!\n", ply_ptr->name, obj_tag->obj->name);
                obj_haha=obj_tag->obj;
                del_obj_crt(obj_haha, ply_ptr);
                free_obj(obj_haha);
                save_ply(ply_ptr->name, ply_ptr);
            }
        }
    } 

    n=load_bank(ply_ptr->name, &cnt_ptr);
    if(n<0) {
        return;
    }


    op=cnt_ptr->first_obj;
    while(op) {
        tes++;
        if(tes>200) break;
        if(!op->obj->parent_crt) op->obj->parent_crt=ply_ptr;
        if(is_bad_item(op->obj)) log_pl("의 은행");
        op=op->next_tag;
    }
    free_obj(cnt_ptr);
    return;
}

void free_obj2(obj_ptr)
object *obj_ptr;
{
    return;
}

void check_contain(obj_ptr)
object *obj_ptr;
{
    otag *obj_tag;
    otag *obj_next;
    object *obj_haha;

    if(!obj_ptr->first_obj) return;

    for(obj_tag=obj_ptr->first_obj; obj_tag; obj_tag=obj_next) {
        obj_next=obj_tag->next_tag;
        if(is_bad_item(obj_tag->obj)) {
            if(obj_tag->obj->shotsmax > 4999) {
                log_pl("나쁜 %s의 %s : 없애버렸어요.!\n", obj_ptr->parent_crt->name, obj_tag->obj->name);
                obj_haha=obj_tag->obj;
                del_obj_obj(obj_haha, obj_ptr);
                free_obj(obj_haha);
                save_ply(obj_ptr->parent_crt->name, obj_ptr->parent_crt);
            }
        }
    } 
}

int is_bad_item(obj_ptr)
object *obj_ptr;
{
    int haha;
    if(strncmp(obj_ptr->name, "제우스", 6) == 0) {
        if(obj_ptr->parent_crt) 
        log_pl("\n제우스 %s : %s K[%s]", obj_ptr->name, obj_ptr->parent_crt->name, obj_ptr->key[2]);
        else log_pl("\n제우스 %s : %s K[%s]", obj_ptr->name, obj_ptr->parent_obj->parent_crt->name, obj_ptr->key[2]);
        return 1;
    } 
    if(obj_ptr->armor > 50) {
        if(obj_ptr->parent_crt) 
        log_pl("\n나쁜!! %s : %s 방어력 : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->armor);
        else log_pl("\n나쁜!! %s : %s 방어력 : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->armor);
        return 1;
    }
    if((haha=obj_ptr->ndice*obj_ptr->sdice+obj_ptr->pdice) > 100) {
        if(obj_ptr->parent_crt) 
        log_pl("\n나쁜!! %s : %s 공격력 : %d", obj_ptr->parent_crt->name, obj_ptr->name, haha);
        else log_pl("\n나쁜!! %s : %s 공격력 : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, haha);
        return 1;
    }
    if(F_ISSET(obj_ptr, OCONTN) && (obj_ptr->shotsmax > 20)) {
        if(obj_ptr->parent_crt) 
        log_pl("\n나쁜!! %s : %s 보따리 : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        else log_pl("\n나쁜!! %s : %s 보따리 : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        return 1;
    }
    if(F_ISSET(obj_ptr, OSPECI) && (obj_ptr->pdice == 4)) {
        if(obj_ptr->parent_crt) 
        log_pl("\n나쁜!! %s : %s 이상해 : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->pdice);
        else log_pl("\n나쁜!! %s : %s 이상해 : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->pdice);
        return 1;
    }
    if((obj_ptr->type == POTION) && (obj_ptr->shotscur > 500)) {
        if(obj_ptr->parent_crt) 
        log_pl("\n나쁜!! %s : %s 사용회수 : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        else log_pl("\n나쁜!! %s : %s 사용회수 : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        return 1;
    }
    if(obj_ptr->shotscur > 1000) {
        if(obj_ptr->parent_crt) 
        log_pl("\n나쁜!! %s : %s 사용회수 : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        else log_pl("\n나쁜!! %s : %s 사용회수 : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        return 1;
    }
    if(obj_ptr->shotsmax > 1000) {
        if(obj_ptr->parent_crt) 
        log_pl("\n나쁜!! %s : %s 최대회수 : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        else log_pl("\n나쁜!! %s : %s 최대회수 : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        return 1;
    }
    return 0;
}

