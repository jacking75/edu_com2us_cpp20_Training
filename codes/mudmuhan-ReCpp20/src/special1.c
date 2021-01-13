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
		print(fd, "�װ��� ������ �����ϴ�.\n");
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
		/* �������� ��ƾ �θ� */
	default:
		print(fd, "�ƹ��͵� �����ϴ�.\n");
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
			print(fd, "���� �����ϱ�?\n");
			return(0);
		}
		n = special_obj(ply_ptr, cmnd, special);
		if(n == -1) return(0);
		if(n == -2) {
			print(fd, "���� �Ϸ��� �ϴµ���?.\n");
			return(0);
		}
		else return(n);
	default:
		print(ply_ptr->fd, "�ƹ��� �ϵ� �Ͼ�� �ʽ��ϴ�.\n");
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
	broadcast_rom(fd, ply_ptr->rom_num, "%M�� %i�� �������ϴ�.", ply_ptr, obj_ptr);

	if(strlen(Ply[fd].extr->tempstr[3]) >= strlen(obj_ptr->use_output)) {
		if(strcmp(Ply[fd].extr->tempstr[3], obj_ptr->use_output)) {
			dmg = mrand(20,40);
			ply_ptr->hpcur -= dmg;
			print(fd, "����� %d���� ���ظ� �Ծ����ϴ�!\n", dmg);
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M�� %i�κ��� ���ظ� �Ծ����ϴ�!", ply_ptr, obj_ptr);
			Ply[fd].extr->tempstr[3][0] = 0;

			if(ply_ptr->hpcur < 1) {
				print(fd, "����� �׾����ϴ�.\n");
				die(ply_ptr, ply_ptr);
			}
		}

		else {
			for(i=1, xp=rom_ptr->first_ext;
				xp && i < obj_ptr->pdice;
				i++, xp = xp->next_tag) ;
			if(!xp) return(0);
			print(fd, "����� %s�� �������ϴ�!\n", xp->ext->name);
			broadcast_rom(fd, ply_ptr->rom_num,
				"%M�� %s�� �������ϴ�!", ply_ptr, xp->ext->name);
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
         print(fd, "����� ���� ������ �Ǹ��� �����ϴ�.\n");
         return(0);
      }
	if(cmnd->num != 2) {
		print(fd, "��� �аŸ��� ������ �Ͻ÷����?");
		return(0);
	}

	for(num=1; num<ftotal; num++) {
		if(!strcmp(cmnd->str[1], family_str[num])) {
			crt_ptr = find_who(fmboss_str[num]);
			if(!crt_ptr) {
				print(fd, "������� �θ��� %s���� �̿����� �ƴմϴ�.", fmboss_str[num]);
				return(0);
			}
		break;
		}
		fal++;
	}
	if(fal >= 15) {
		print(fd, "�׷� �аŸ��� �����ϴ�.");
		return(0);
	}
	if(fnum == num) {
		print(fd, "�ڱ� �ڽŵ�� �ο�÷����?");
		return(0);
	}

	if(!AT_WAR) {
		  if(!CALLWAR2) {
			broadcast_all("\n### %s �аŸ��� %s���� �������� �մϴ�.\n\n",
					family_str[fnum], family_str[num]);
		CALLWAR1 = fnum;
		CALLWAR2 = num;
		return(0);
		}
		if(CALLWAR1 == fnum) {
			broadcast("\n### %s �аŸ����� �������� ����մϴ�.\n", family_str[fnum]);
			CALLWAR1 = CALLWAR2 = 0;
			return(0);
		}
		if(CALLWAR2 == fnum) {
			if(CALLWAR1 != num) {
				print(fd, "�ٸ� �аŸ����� ������ ��û�صΰ� �ֽ��ϴ�.");
				return(0);
			}
			else {
				broadcast("\n### %s �аŸ����� �������� �޾Ƶ鿴���ϴ�.\n",
					family_str[fnum], family_str[num]);
				AT_WAR = (fnum*16) + num;
				return(0);
			}
		}
		else {
			print(fd, "�ٸ� �аŸ����� ���� ������ �غ����Դϴ�.");
			return(0);
		}
	 }

	else
	   print(fd, "���� �������Դϴ�.\n");

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
                log_pl("���� %s�� %s : ���ֹ��Ⱦ��.!\n", ply_ptr->name, ply_ptr->ready[i]->name);
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
                log_pl("���� %s�� %s : ���ֹ��Ⱦ��.!\n", ply_ptr->name, obj_tag->obj->name);
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
        if(is_bad_item(op->obj)) log_pl("�� ����");
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
                log_pl("���� %s�� %s : ���ֹ��Ⱦ��.!\n", obj_ptr->parent_crt->name, obj_tag->obj->name);
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
    if(strncmp(obj_ptr->name, "���콺", 6) == 0) {
        if(obj_ptr->parent_crt) 
        log_pl("\n���콺 %s : %s K[%s]", obj_ptr->name, obj_ptr->parent_crt->name, obj_ptr->key[2]);
        else log_pl("\n���콺 %s : %s K[%s]", obj_ptr->name, obj_ptr->parent_obj->parent_crt->name, obj_ptr->key[2]);
        return 1;
    } 
    if(obj_ptr->armor > 50) {
        if(obj_ptr->parent_crt) 
        log_pl("\n����!! %s : %s ���� : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->armor);
        else log_pl("\n����!! %s : %s ���� : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->armor);
        return 1;
    }
    if((haha=obj_ptr->ndice*obj_ptr->sdice+obj_ptr->pdice) > 100) {
        if(obj_ptr->parent_crt) 
        log_pl("\n����!! %s : %s ���ݷ� : %d", obj_ptr->parent_crt->name, obj_ptr->name, haha);
        else log_pl("\n����!! %s : %s ���ݷ� : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, haha);
        return 1;
    }
    if(F_ISSET(obj_ptr, OCONTN) && (obj_ptr->shotsmax > 20)) {
        if(obj_ptr->parent_crt) 
        log_pl("\n����!! %s : %s ������ : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        else log_pl("\n����!! %s : %s ������ : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        return 1;
    }
    if(F_ISSET(obj_ptr, OSPECI) && (obj_ptr->pdice == 4)) {
        if(obj_ptr->parent_crt) 
        log_pl("\n����!! %s : %s �̻��� : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->pdice);
        else log_pl("\n����!! %s : %s �̻��� : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->pdice);
        return 1;
    }
    if((obj_ptr->type == POTION) && (obj_ptr->shotscur > 500)) {
        if(obj_ptr->parent_crt) 
        log_pl("\n����!! %s : %s ���ȸ�� : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        else log_pl("\n����!! %s : %s ���ȸ�� : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        return 1;
    }
    if(obj_ptr->shotscur > 1000) {
        if(obj_ptr->parent_crt) 
        log_pl("\n����!! %s : %s ���ȸ�� : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        else log_pl("\n����!! %s : %s ���ȸ�� : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotscur);
        return 1;
    }
    if(obj_ptr->shotsmax > 1000) {
        if(obj_ptr->parent_crt) 
        log_pl("\n����!! %s : %s �ִ�ȸ�� : %d", obj_ptr->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        else log_pl("\n����!! %s : %s �ִ�ȸ�� : %d", obj_ptr->parent_obj->parent_crt->name, obj_ptr->name, obj_ptr->shotsmax);
        return 1;
    }
    return 0;
}

