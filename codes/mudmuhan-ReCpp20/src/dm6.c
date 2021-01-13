/*
 * DM6.C:
 *
 *  DM functions
 *
 *  Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *  Copyright (C) 1995 Brooke Paul
 *
 */

#include "mstruct.h"
#include "mextern.h"

extern long all_broad_time;

/**********************************************************************/
/*				dm_dust				      */
/**********************************************************************/

/* This function allows a DM to destory a player.		      */

int dm_dust(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        creature        *crt_ptr;
        int             cfd, fd;
        char            str[IBUFSIZE+1]; 
	char		file[80];

        if(ply_ptr->class < SUB_DM)
                return(PROMPT);

	if(cmnd->num < 2) {
		print(ply_ptr->fd, "\n�������� ������ �������?\n");
		return(PROMPT);
	}
        lowercize(cmnd->str[1], 1);
        crt_ptr = find_who(cmnd->str[1]);
        if(!crt_ptr || crt_ptr== ply_ptr) {
                print(ply_ptr->fd, "%s�� �����ϴ�.\n", cmnd->str[1]);
                return(0);
	}
	if(crt_ptr->class >= SUB_DM) {
		ANSI(crt_ptr->fd, RED);
		print(crt_ptr->fd, "%s�� ��ſ��� ������ ������ �մϴ�!\n", ply_ptr->name);
		ANSI(crt_ptr->fd, WHITE);
		return(0);
	}
	cfd = crt_ptr->fd;
	if(!(Ply[cfd].io->fn == command && Ply[cfd].io->fnparam == 1)) {
		print(ply_ptr->fd, "%s�� ���� ���� ������ �����ϴ� .\n", cmnd->str[1]);
		return(0);
	}
	ANSI(cfd, MAGENTA);
	print(cfd, "������ �ϴÿ��� �������ϴ� �ŵ��� �г밡 �����մϴ�!\n");
	ANSI(cfd, WHITE);
	broadcast_rom(cfd, crt_ptr->rom_num,"������ �ϴÿ��� %s���� �������ϴ�.\n",crt_ptr->name);
	broadcast_all("\n### %S%j ����̰� �ǹ��Ƚ��ϴ�! %s���� ���Ǹ� ǥ�Ͻʽÿ�.\n", Ply[cfd].ply->name,"1", F_ISSET(Ply[cfd].ply, PMALES) ? "��":"�׳�");
                sprintf(file, "mv -f %s/%s/%s %s/%s/%s~~",
                   PLAYERPATH, first_han(Ply[cfd].ply->name), Ply[cfd].ply->name,
                   PLAYERPATH, first_han(Ply[cfd].ply->name), Ply[cfd].ply->name); 
                disconnect(cfd);
        broadcast("### �ָ��� �ŵ��� �г뿡 õ�ռҸ��� ����ɴϴ�.\n");
	        system(file);
        all_broad_time=time(0);
                return(0);

}

/**********************************************************************/
/*                              dm_follow                             */
/**********************************************************************/
/*	This function allows a DM to force a monster to follow 	      */
/*	him, and has been made to allow for the movement of	      */
/*      custom monsters (made with the dm_crt_name function).	      */

int dm_follow (ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
	creature	*crt_ptr;
	room		*rom_ptr;
	ctag		*pp, *cp, *prev;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class < DM)
                return(PROMPT);	

	if (cmnd->num < 2) {
		print (fd, "����: <����> *������\n");
		return(0);
	}

        crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

	if(!crt_ptr) {
		print (fd, "�׷� ������ �����ϴ�.\n");
		return (0);
	}

	if(F_ISSET(crt_ptr, MPERMT)) {
		print (fd, "������ �����Դϴ�.\n");
		return(0);
	}
	if(F_ISSET (crt_ptr, MDMFOL)) {
		F_CLR(crt_ptr, MDMFOL);
		print (fd, "%s�� ����� �׸� �����ϴ�.\n", crt_ptr->name);
		cp = ply_ptr->first_fol;
		if(cp->crt == crt_ptr) {
			ply_ptr->first_fol = cp->next_tag;
			free(cp);
		}
		else while (cp) {
			if (cp->crt == crt_ptr) {
				prev->next_tag = cp->next_tag;
				free(cp);
				break;
			}
		     prev = cp;
		     cp = cp->next_tag;
		     }

		crt_ptr->following = 0;
		return(0);
	}
	crt_ptr->following = ply_ptr;
	F_SET(crt_ptr, MDMFOL);
	pp = (ctag *)malloc(sizeof(ctag));
	if(!pp)
		merror("dmfollow", FATAL);
	pp->crt = crt_ptr;
	pp->next_tag = 0;
	
	if(!ply_ptr->first_fol)
		ply_ptr->first_fol = pp;
	else {
		pp->next_tag = ply_ptr->first_fol;
		ply_ptr->first_fol = pp;
	}	
	print (fd, "%s�� ����� �����ϴ�.\n", crt_ptr->name);
	return (0);
}

/************************************************************************/
/*			dm_attack					*/
/************************************************************************/
/* 	This function allows a DM to make a monster attack a given 	*/
/*  player.								*/

int dm_attack (ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
	creature	*atr_ptr, *atd_ptr;
	room		*rom_ptr;
	ctag		*pp, *cp, *prev;
	int		fd;

	fd = ply_ptr->fd;
	rom_ptr = ply_ptr->parent_rom;

	if(ply_ptr->class < DM)
                return(PROMPT);	

	if (cmnd->num < 3) {
		print (fd, "����: <����> <�����> *����\n");
		return(0);
	}

        atr_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1],
                           cmnd->val[1]);

	if(!atr_ptr) {
		print (fd, "�׷� ������ �����ϴ�.\n");
		return (0);
	}

	if(F_ISSET(atr_ptr, MPERMT)) {
		print (fd, "������ �����Դϴ�.\n");
		return(0);
	}

        atd_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[2],
                           cmnd->val[2]);

	if(!atd_ptr) {
	lowercize(cmnd->str[2], 1);
	atd_ptr = find_who(cmnd->str[2]);
	}

	if(!atd_ptr) {
		print (fd, "�׷� ����� �����ϴ�.\n");
		return (0);
	}

	if(F_ISSET(atd_ptr, MPERMT)) {
		print (fd, "������ �����Դϴ�.\n");
		return(0);
	}
	print(fd, "%s�� %s�� �����մϴ�.\n", atd_ptr->name, atr_ptr->name);
	add_enm_crt(atd_ptr->name, atr_ptr);
	broadcast_rom(atd_ptr->fd, atd_ptr->rom_num, "%M�� %m�� �����մϴ�.", atr_ptr, atd_ptr);
	if(atd_ptr->type = PLAYER) 
		print (atd_ptr->fd, "%M�� ����� �����մϴ�!\n", atr_ptr);
	return(0);
}

/***************************************************************************/
/*			list_enm					   */ 
/***************************************************************************/
/*	This function lists the enemy list of a given monster.		   */

int list_enm(ply_ptr, cmnd)
creature *ply_ptr;
cmd     *cmnd;

{
etag	*first_enm;
etag    *ep;
room	*rom_ptr;
creature *crt_ptr;
int	fd, n=0;

	if (ply_ptr->class < SUB_DM)
        	return(0);
	
	rom_ptr= ply_ptr->parent_rom;
	fd = ply_ptr->fd;

	crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon, cmnd->str[1], cmnd->val[1]);

	if(!crt_ptr){
		print(fd, "�׷� ������ �����ϴ�.\n");
		return(0);
	}
	print(ply_ptr->fd,"%s�� ����:\n", crt_ptr->name);

        ep = crt_ptr->first_enm;

    while(ep) {
	n +=1;
	print (fd,"%s.\n", ep->enemy);
        ep = ep->next_tag;
    }
    if (!n)
	print(fd, "����.\n");
    return(0);
}

/**********************************************************************/
/*                              list_charm                            */
/**********************************************************************/
/*      This function allows a DM to see a given players charm list   */
                
int list_charm (ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
                        
creature        *crt_ptr;
int             fd, cfd, n=0;
ctag            *cp;
                                
        if(ply_ptr->class < SUB_DM)
                return(PROMPT);
                         
        if(cmnd->num < 2) {
                print(ply_ptr->fd, "������ �ָ��ڸ� ���ϱ�?\n");
                return(PROMPT);
        }
                
        fd = ply_ptr->fd; 
        lowercize(cmnd->str[1], 1);
        crt_ptr = find_who(cmnd->str[1]);
        if(!crt_ptr) {
                print(ply_ptr->fd, "%s�� �����ϴ�.\n", cmnd->str[1]);
                return(0);
        }
        
        cfd = crt_ptr->fd;
         
        cp = Ply[cfd].extr->first_charm;
        print (fd, "%s�� ���ָ���:\n", crt_ptr->name);
        while(cp) {
		n += 1;
                print(fd,"%s.\n", cp->crt->name); 
                cp = cp->next_tag;
        }
        if(!n)
		print(fd, "����.\n");
	return(0);
}

