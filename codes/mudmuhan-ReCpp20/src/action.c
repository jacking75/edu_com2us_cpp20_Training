
/*
 * ACTION.C:
 *
 *      This file contains the routines necessary to achieve action
 *      commands.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"

#define SHIT 349

/**********************************************************************/
/*                              action                                */
/**********************************************************************/

/* This function allows a player to commit an action command.         */

#define OUT1(a,b,m)    {print(fd, a, (msg_ok)?msg:m); \
                        broadcast_rom(fd, ply_ptr->rom_num, b, ply_ptr, (msg_ok)?msg:m);}

#define OUTn1(a,b)     {print(fd, a); \
                        broadcast_rom(fd, ply_ptr->rom_num, b, ply_ptr);}

#define OUT2(a,b,c,m)  {print(fd, a, crt_ptr, (msg_ok)?msg:m); \
                        print(crt_ptr->fd, b, ply_ptr, (msg_ok)?msg:m); \
                        broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num, \
                                                 c, ply_ptr, crt_ptr, (msg_ok)?msg:m);}
#define OUTj2(a,b,c,m,j) {print(fd, a, crt_ptr, j, (msg_ok)?msg:m); \
                        print(crt_ptr->fd, b, ply_ptr, (msg_ok)?msg:m); \
                        broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num, \
                                                 c, ply_ptr, crt_ptr, j,(msg_ok)?msg:m);}

#define OUTn2(a,b,c,m) {print(fd, a, crt_ptr); \
                        print(crt_ptr->fd, b, ply_ptr); \
                        broadcast_rom2(fd, crt_ptr->fd, rom_ptr->rom_num, \
                                                 c, ply_ptr, crt_ptr);}

int action(ply_ptr, cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
        room            *rom_ptr;
        creature        *crt_ptr = 0;
        object          *obj_ptr;
        int             match=0, cmdno=0, c=0, n, fd, num;
        int             match_deep=0,current_deep;
        char            msg[256];
        int             msg_ok=0,i;

        fd = ply_ptr->fd;
        rom_ptr = ply_ptr->parent_rom;

        F_CLR(ply_ptr, PHIDDN);

        if(F_ISSET(ply_ptr, PSILNC)) {
        	print(fd, "�Ѹ��� �Ҽ� �����ϴ�!");
        	return(0);
        }

        do {
                if(!strcmp(cmnd->str[0], cmdlist[c].cmdstr)) {
                        match = 1;
                        cmdno = c;
                        break;
                }
                else if((current_deep=str_compare(cmnd->str[0], cmdlist[c].cmdstr))!=0) {
                        match=1;
                        if(match_deep==0 || match_deep<current_deep) {
                                cmdno = c;
                                match_deep=current_deep;
                        }
                }
                c++;
        } while(cmdlist[c].cmdno);

        if(cmnd->num >= 2) {
                crt_ptr = find_crt(ply_ptr, rom_ptr->first_mon,
                                        cmnd->str[1], cmnd->val[1]);
                if(!crt_ptr) {
                        crt_ptr = find_crt(ply_ptr, rom_ptr->first_ply,
                                                cmnd->str[1], cmnd->val[1]);
                }

                if(!crt_ptr || crt_ptr == ply_ptr) {
                        i=-1;
                }
                else if(!strcmp(cmdlist[cmdno].cmdstr,"����")) i=-1;
                else {
                        for(i=0;i<strlen(cmnd->fullstr);i++) if(cmnd->fullstr[i]!=' ') break;
                        for(;i<strlen(cmnd->fullstr);i++) if(cmnd->fullstr[i]==' ') break;
                }
                strcpy(msg,&cmnd->fullstr[i+1]);
                if((msg_ok=cut_command(msg))!=0) {
                        msg[msg_ok++]=' ';
                        msg[msg_ok]=0;
                        msg_ok=1;
                }
                else msg_ok=0;
        }

        if(!strcmp(cmdlist[cmdno].cmdstr, "����ǥ��")) {
                OUTn1("����� '����ǥ�� ����'�̶� ġ�°� �����̴ϴ�.","\n%M�� ����ǥ���� �����մϴ�.");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����")) {
                if(crt_ptr && crt_ptr!=ply_ptr) 
                         OUTj2("����� %M%j %s���ϴ�.", "\n%M�� ����� %s���ϴ�.","\n%M�� %M%j %s���ϴ�.","","3")
                else OUT1("����� %s���ϴ�.", "\n%M�� %s���ϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�����")) {
                if(crt_ptr && crt_ptr!=ply_ptr) 
                         OUTj2("����� %M%j %s��� ���ϴ�.", "\n%M�� ����� %s��� ���ϴ�.","\n%M�� %M%j %s��� ���ϴ�.","������� �ϰ� ","3")
                else OUT1("����� %s��� ���ϴ�.", "\n%M�� %s��� ���ϴ�.","����� �վ����� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����") || !strcmp(cmdlist[cmdno].cmdstr, "��")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s���� �����Ÿ��ϴ�.", "\n%M�� ��ſ��� %s���� �����Ÿ��ϴ�.","\n%M�� %M���� %s���� �����Ÿ��ϴ�.","")
                else OUT1("����� %s���� �����Ÿ��ϴ�.", "\n%M�� %s���� �����Ÿ��ϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�ƴ�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s���� ���� �����ϴ�.", "\n%M�� ��ſ��� %s���� ���������ϴ�.","\n%M�� %M���� ���� ���������ϴ�.","")
                else OUT1("����� %s���� ���������ϴ�.", "\n%M�� %s���� ���������ϴ�.","")
        }
        else if(!strncmp(cmdlist[cmdno].cmdstr, "��",2)) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s������ �մϴ�.","\n%M�� ��ſ��� %s������ �մϴ�.", "\n%M�� %M���� %s������ �մϴ�.","�������� ")
                else OUT1("����� %s������ �մϴ�.", "\n%M�� %s������ �մϴ�.","�������� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�̼�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s�̼Ҹ� �����ϴ�.","\n%M�� ��ſ��� %s�̼Ҹ� �����ϴ�.", "\n%M�� %M���� %s�̼Ҹ� �����ϴ�.","")
                else OUT1("����� %s�̼Ҹ� �����ϴ�.", "\n%M�� %s�̼Ҹ� �����ϴ�.","���� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M�� ���� %s���� ���ϴ�.","\n%M�� ����� ���� %s���� ���ϴ�.", "\n%M�� %M�� ���� %s���� ���ϴ�.","�������� ")
                else OUT1("����� %s���� ���ϴ�.", "\n%M�� %s���� ���ϴ�.","�������� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M�� %s�մϴ�.","\n%M�� ����� %s�մϴ�.", "\n%M�� %M�� %s�մϴ�.","���� �ɽ��� ")
                else OUT1("����� %s�մϴ�.", "\n%M�� %s�մϴ�.","�ɽ��� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��ǰ")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M�� ���� %s��ǰ�� �մϴ�. ����~","\n%M�� ����� ���� %s��ǰ�� �մϴ�. ����~", "\n%M�� %M�� ���� %s��ǰ�� �մϴ�. ����~","")
                else OUT1("����� %s��ǰ�� �մϴ�.", "\n%M�� %s��ǰ�� �մϴ�.","���������� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "ûȥ")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %sûȥ�� �մϴ�.","\n%M�� ��ſ��� %sûȥ�� �մϴ�.", "\n%M�� %M���� %sûȥ�� �մϴ�.","")
                else OUT1("����� %sûȥ�� �մϴ�.", "\n%M�� %sûȥ�� �մϴ�.","ȥ�ڼ� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j ���� %s�����ϴ�.","\n%M�� ����� ���� %s�����ϴ�.","%M�� %M%j ���� %s�����ϴ�.","���� ","3")
                else OUT1("����� %s�����ϴ�.", "\n%M�� %s�����ϴ�.", "���� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�̾�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s�̾��� �մϴ�.","\n%M�� ��ſ��� %s�̾��� �մϴ�.", "\n%M�� %M���� %s�̾��� �մϴ�.","")
                else OUT1("����� %s�̾��� �մϴ�.", "\n%M�� %s�̾��� �մϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�Ǽ�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j %s�Ǽ��� �մϴ�.", "\n%M�� ��Ű� %s�Ǽ��� �մϴ�.", "\n%M�� %M%j %s�Ǽ��� �մϴ�.","","2")
                else OUT1("����� ���� ���о� %s�Ǽ��� û�մϴ�.", "\n%M�� ���� ���о� %s�Ǽ��� û�մϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�������̺�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j %s�������̺긦 �մϴ�.", "\n%M�� ��Ű� %s�������̺긦 �մϴ�.", "\n%M�� %M%j �������̺긦 �մϴ�.","���� ���̵�� ","2")
                else OUT1("����� ���� ��� ������� %s�������ϴ�.","\n%M�� ���� ��� ������� %s�������ϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�ڼ�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s�ڼ��� Ĩ�ϴ�. ¦¦~", "\n%M�� ��ſ��� %s�ڼ��� Ĩ�ϴ�. ¦¦~", "\n%M�� %M���� %s�ڼ��� Ĩ�ϴ�. ¦¦~","")
                else OUT1("����� %s�ڼ��� Ĩ�ϴ�. ¦¦~", "\n%M�� %s�ڼ��� Ĩ�ϴ�. ¦¦~","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��") || !strcmp(cmdlist[cmdno].cmdstr, "���")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s��踦 ���մϴ�.","\n%M�� ��ſ��� %s��踦 ���մϴ�.","\n%M�� %M���� %s��踦 ���մϴ�.","")
                else OUT1("����� ��踦 %s�ǿ�ϴ�. Ǫ��~~~~", "\n%M�� ��踦 %s�ǿ�ϴ�. Ǫ��~~","���ݻ��� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s���� �մϴ�.","\n%M�� ��ſ��� %s���� �մϴ�.", "\n%M�� %M���� %s���� �մϴ�.","������ ")
                else OUT1("����� %s���� �մϴ�.", "\n%M�� %s���� �մϴ�.","������ ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j %s��ϴ�.", "\n%M�� ����� %s��ϴ�.", "\n%M�� %M%j %s��ϴ�.","���� ","3")
                else OUT1("����� ���� �÷� ����� %s��ϴ�.", "\n%M�� ���� �÷� ����� %s��ϴ�.","���� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j %s���� ��ϴ�. '�뱸.�λ�.���~����.��!'", "\n%M�� ��Ű� %s���� ��ϴ�. '�뱸.�λ�.���~����.��!'","\n%M�� %M%j %s���� ��ϴ�. '�뱸.�λ�.���~����.��!'","�ų��� ","2")
                else OUT1("����� %s���� ��ϴ�. '�뱸.�λ�.���~����.��!'", "\n%M�� %s���� ��ϴ�. '�뱸.�λ�.���~����.��!'","ȥ�ڼ� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�뷡")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j ���� %s�뷡�� �θ��ϴ�.", "\n%M�� ����� ���� %s�뷡�� �θ��ϴ�.\n","\n%M�� %M%j ���� %s�뷡�� �θ��ϴ�.\n","��̰� ","3")
                else OUT1("����� %s�뷡�� �θ��ϴ�.", "\n%M�� %s�뷡�� �θ��ϴ�.\n","��̰� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "���")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s������ ���Դϴ�.", "\n%M�� ��ſ��� %s������ ���Դϴ�.","\n%M�� %M���� %s������ ���Դϴ�.","")
                else OUT1("����� %s��ϴ�. �ƾ�~", "\n%M�� %s��ϴ�. �ƾ�~","������ ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�޷�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j %s�޷� �ݴϴ�.", "\n%M�� ����� %s�޷� �ݴϴ�.","\n%M�� %M%j %s�޷� �ݴϴ�.","","3")
                else OUT1("����� %s�޷� �ݴϴ�.", "\n%M�� %s�޷� �ݴϴ�.","�ڽ��� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��Ȳ")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j ���� %s��Ȳ�� �մϴ�.", "\n%M�� ����� ���� %s��Ȳ���մϴ�.","\n%M�� %M%j ���� %s��Ȳ�� �մϴ�.","","3")
                else OUT1("����� %s��Ȳ�� �մϴ�.", "\n%M�� %s��Ȳ�� �մϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "��ũ")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s��ũ�� �մϴ�.", "\n%M�� ��ſ��� %s��ũ�� �մϴ�.","\n%M�� %M���� %s��ũ�� �մϴ�.","")
                else OUT1("����� %s��ũ�� �մϴ�.", "\n%M�� %s��ũ�� �մϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�ǻ�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s�ǻǸ� �մϴ�.", "\n%M�� ��ſ��� %s�ǻǸ� �մϴ�.","\n%M�� %M���� %s�ǻǸ� �մϴ�.","")
                else OUT1("����� %s�ǻǸ� �մϴ�.", "\n%M�� %s�ǻǸ� �մϴ�.","�ڱ� �չٴڿ��� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����") || !strcmp(cmdlist[cmdno].cmdstr, "�߰�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s�ۺ��λ縦 �մϴ�.", "\n%M�� ��ſ��� %s�ۺ��λ縦 �մϴ�.","\n%M�� %M���� %s�ۺ��λ縦 �մϴ�.","")
                else OUT1("����� %s�ۺ��λ縦 �մϴ�.", "\n%M�� %s�ۺ��λ縦 �մϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�ȳ�")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s�λ縦 �մϴ�. \"�ȳ��ϼ���~\"", "\n%M�� ��ſ��� %s�λ縦 �մϴ�. \"�ȳ��ϼ���~\"","\n%M�� %M���� %s�λ縦 �մϴ�. \"�ȳ��ϼ���~\"","")
                else OUT1("����� %s�λ縦 �մϴ�. \"�ȳ��ϼ���~\"", "\n%M�� %s�λ縦 �մϴ�. \"�ȳ��ϼ���~\"","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j ���� %s������ �����Դϴ�.", "\n%M�� ����� ���� %s��Ȳ���մϴ�.","\n%M�� %M%j ���� %s������ �����Դϴ�.","","3")
                else OUT1("����� %s������ �����Դϴ�.", "\n%M�� %s������ �����Դϴ�.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "���")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("����� %M%j %s��ϴ�. �޷ո޷�~~", "\n%M�� ����� %s��ϴ�. �޷ո޷�~~","\n%M�� %M%j ���� %s��ϴ�. �޷ո޷�~~","������� ","3")
                else OUT1("����� %s��ϴ�. �޷ո޷�~~", "\n%M�� %s��ϴ�. �޷ո޷�~~","������� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����")) {
                OUT1("����� %s�����մϴ�.", "\n%M�� %s�����մϴ�.","���ɽ����� ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "�β���")) {
                OUT1("����� %s�β����� �մϴ�.", "\n%M�� %s�β����� �մϴ�.","���� ������ ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "����")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("����� %M���� %s�����մϴ�. \"��Ǭ�ݼ�~~\"", "\n%M�� ��ſ��� %s�����մϴ�. \"��Ǭ�ݼ�~~\"", "\n%M�� %M���� %s�����մϴ�. \"��Ǭ�ݼ�~~\"","")
                else OUT1("����� �ٴڿ� ����� %s�����մϴ�. \"��Ǭ�ݼ�~~\"", "\n%M�� �ٴڿ� ����� %s�����մϴ�. \"��Ǭ�ݼ�~~\"","")
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "����")) {
		if(crt_ptr && crt_ptr!=ply_ptr)
		     OUTj2("����� %M%j %s�����մϴ�.","\n%M�� ����� %s�����մϴ�.","\n%M�� %M%j %s�����մϴ�.","���� ","3")
		else OUT1("����� �����θ� %s�����մϴ�.","\n%M�� �����θ� %s�����մϴ�.","���� ")
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "�Ⱦ�") || !strcmp(cmdlist[cmdno].cmdstr, "���Ⱦ�")) {
		if(crt_ptr && crt_ptr!=ply_ptr)
			OUTj2("����� %M%j %s���Ƚ��ϴ�.","\n%M�� ����� %s���Ƚ��ϴ�.","\n%M�� %M%j %s���Ƚ��ϴ�.","�� ","3")
		else 	OUT1("����� %s�������� �ָ� ���ϴ�.","\n%M�� %s�������� �ָ� ���� �ֽ��ϴ�.","����� ")	
	}
        return(0);
}

