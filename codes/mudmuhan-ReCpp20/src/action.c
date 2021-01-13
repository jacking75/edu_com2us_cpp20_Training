
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
        	print(fd, "한마디도 할수 없습니다!");
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
                else if(!strcmp(cmdlist[cmdno].cmdstr,"생각")) i=-1;
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

        if(!strcmp(cmdlist[cmdno].cmdstr, "감정표현")) {
                OUTn1("당신은 '감정표현 도움'이라 치는게 좋을겁니다.","\n%M이 감정표현을 연구합니다.");
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "보아")) {
                if(crt_ptr && crt_ptr!=ply_ptr) 
                         OUTj2("당신은 %M%j %s봅니다.", "\n%M이 당신을 %s봅니다.","\n%M이 %M%j %s봅니다.","","3")
                else OUT1("당신은 %s봅니다.", "\n%M이 %s봅니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "노려봐")) {
                if(crt_ptr && crt_ptr!=ply_ptr) 
                         OUTj2("당신은 %M%j %s노려 봅니다.", "\n%M이 당신을 %s노려 봅니다.","\n%M이 %M%j %s노려 봅니다.","뱁새눈을 하고 ","3")
                else OUT1("당신은 %s노려 봅니다.", "\n%M이 %s노려 봅니다.","허공을 뚫어져라 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "끄덕") || !strcmp(cmdlist[cmdno].cmdstr, "응")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s고개를 끄덕거립니다.", "\n%M이 당신에게 %s고개를 끄덕거립니다.","\n%M이 %M에게 %s고개를 끄덕거립니다.","")
                else OUT1("당신은 %s고개를 끄덕거립니다.", "\n%M이 %s고개를 끄덕거립니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "아니")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s고개를 가로 젓습니다.", "\n%M이 당신에게 %s고개를 가로젓습니다.","\n%M이 %M에게 고개를 가로젓습니다.","")
                else OUT1("당신은 %s고개를 가로젓습니다.", "\n%M이 %s고개를 가로젓습니다.","")
        }
        else if(!strncmp(cmdlist[cmdno].cmdstr, "감",2)) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s감사해 합니다.","\n%M이 당신에게 %s감사해 합니다.", "\n%M이 %M에게 %s감사해 합니다.","진심으로 ")
                else OUT1("당신은 %s감사해 합니다.", "\n%M이 %s감사해 합니다.","진심으로 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "미소")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s미소를 보냅니다.","\n%M이 당신에게 %s미소를 짓습니다.", "\n%M이 %M에게 %s미소를 짓습니다.","")
                else OUT1("당신은 %s미소를 짓습니다.", "\n%M이 %s미소를 짓습니다.","밝은 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "떨어")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M을 보고 %s벌벌 떱니다.","\n%M이 당신을 보고 %s벌벌 떱니다.", "\n%M이 %M를 보고 %s벌벌 떱니다.","무서워서 ")
                else OUT1("당신은 %s벌벌 떱니다.", "\n%M이 %s벌벌 떱니다.","무서워서 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "해")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M을 %s합니다.","\n%M이 당신을 %s합니다.", "\n%M이 %M을 %s합니다.","보며 심심해 ")
                else OUT1("당신은 %s합니다.", "\n%M이 %s합니다.","심심해 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "하품")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M을 보고 %s하품을 합니다. 아함~","\n%M이 당신을 보고 %s하품을 합니다. 아함~", "\n%M이 %M를 보고 %s하품을 합니다. 아함~","")
                else OUT1("당신은 %s하품을 합니다.", "\n%M이 %s하품을 합니다.","자지러지게 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "청혼")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s청혼을 합니다.","\n%M이 당신에게 %s청혼을 합니다.", "\n%M이 %M에게 %s청혼을 합니다.","")
                else OUT1("당신은 %s청혼을 합니다.", "\n%M이 %s청혼을 합니다.","혼자서 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "웃어")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j 보며 %s웃습니다.","\n%M이 당신을 보며 %s웃습니다.","%M이 %M%j 보며 %s웃습니다.","생긋 ","3")
                else OUT1("당신은 %s웃습니다.", "\n%M이 %s웃습니다.", "생긋 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "미안")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s미안해 합니다.","\n%M이 당신에게 %s미안해 합니다.", "\n%M이 %M에게 %s미안해 합니다.","")
                else OUT1("당신은 %s미안해 합니다.", "\n%M이 %s미안해 합니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "악수")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j %s악수를 합니다.", "\n%M이 당신과 %s악수를 합니다.", "\n%M이 %M%j %s악수를 합니다.","","2")
                else OUT1("당신은 손을 내밀어 %s악수를 청합니다.", "\n%M이 손을 내밀어 %s악수를 청합니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "하이파이브")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j %s하이파이브를 합니다.", "\n%M이 당신과 %s하이파이브를 합니다.", "\n%M이 %M%j 하이파이브를 합니다.","손을 높이들어 ","2")
                else OUT1("당신은 손을 들어 허공에다 %s휘젓습니다.","\n%M이 손을 들어 허공에다 %s휘젓습니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "박수")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s박수를 칩니다. 짝짝~", "\n%M이 당신에게 %s박수를 칩니다. 짝짝~", "\n%M이 %M에게 %s박수를 칩니다. 짝짝~","")
                else OUT1("당신은 %s박수를 칩니다. 짝짝~", "\n%M이 %s박수를 칩니다. 짝짝~","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "흡연") || !strcmp(cmdlist[cmdno].cmdstr, "담배")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s담배를 권합니다.","\n%M이 당신에게 %s담배를 권합니다.","\n%M이 %M에게 %s담배를 권합니다.","")
                else OUT1("당신은 담배를 %s피웁니다. 푸우~~~~", "\n%M이 담배를 %s피웁니다. 푸우~~","뻐금뻐금 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "절")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s절을 합니다.","\n%M이 당신에게 %s절을 합니다.", "\n%M이 %M에게 %s절을 합니다.","공손히 ")
                else OUT1("당신은 %s절을 합니다.", "\n%M이 %s절을 합니다.","공손히 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "찔러")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j %s찌릅니다.", "\n%M이 당신을 %s찌릅니다.", "\n%M이 %M%j %s찌릅니다.","쿡쿡 ","3")
                else OUT1("당신은 손을 올려 허공을 %s찌릅니다.", "\n%M이 손을 올려 허공을 %s찌릅니다.","쿡쿡 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "춤")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j %s춤을 춥니다. '대구.부산.찍고~광주.턴!'", "\n%M이 당신과 %s춤을 춥니다. '대구.부산.찍고~광주.턴!'","\n%M이 %M%j %s춤을 춥니다. '대구.부산.찍고~광주.턴!'","신나게 ","2")
                else OUT1("당신은 %s춤을 춥니다. '대구.부산.찍고~광주.턴!'", "\n%M이 %s춤을 춥니다. '대구.부산.찍고~광주.턴!'","혼자서 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "노래")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j 위해 %s노래를 부릅니다.", "\n%M이 당신을 위해 %s노래를 부릅니다.\n","\n%M이 %M%j 위해 %s노래를 부릅니다.\n","즐겁게 ","3")
                else OUT1("당신은 %s노래를 부릅니다.", "\n%M이 %s노래를 부릅니다.\n","즐겁게 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "울어")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s눈물을 보입니다.", "\n%M이 당신에게 %s눈물을 보입니다.","\n%M이 %M에게 %s눈물을 보입니다.","")
                else OUT1("당신은 %s웁니다. 아앙~", "\n%M이 %s웁니다. 아앙~","슬프게 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "달래")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j %s달래 줍니다.", "\n%M이 당신을 %s달래 줍니다.","\n%M이 %M%j %s달래 줍니다.","","3")
                else OUT1("당신은 %s달래 줍니다.", "\n%M이 %s달래 줍니다.","자신을 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "당황")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j 보고 %s당황해 합니다.", "\n%M이 당신을 보고 %s당황해합니다.","\n%M이 %M%j 보고 %s당황해 합니다.","","3")
                else OUT1("당신은 %s당황해 합니다.", "\n%M이 %s당황해 합니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "윙크")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s윙크를 합니다.", "\n%M이 당신에게 %s윙크를 합니다.","\n%M이 %M에게 %s윙크를 합니다.","")
                else OUT1("당신은 %s윙크를 합니다.", "\n%M이 %s윙크를 합니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "뽀뽀")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s뽀뽀를 합니다.", "\n%M이 당신에게 %s뽀뽀를 합니다.","\n%M이 %M에게 %s뽀뽀를 합니다.","")
                else OUT1("당신은 %s뽀뽀를 합니다.", "\n%M이 %s뽀뽀를 합니다.","자기 손바닥에다 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "바이") || !strcmp(cmdlist[cmdno].cmdstr, "잘가")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s작별인사를 합니다.", "\n%M이 당신에게 %s작별인사를 합니다.","\n%M이 %M에게 %s작별인사를 합니다.","")
                else OUT1("당신은 %s작별인사를 합니다.", "\n%M이 %s작별인사를 합니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "안녕")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s인사를 합니다. \"안녕하세요~\"", "\n%M이 당신에게 %s인사를 합니다. \"안녕하세요~\"","\n%M이 %M에게 %s인사를 합니다. \"안녕하세요~\"","")
                else OUT1("당신은 %s인사를 합니다. \"안녕하세요~\"", "\n%M이 %s인사를 합니다. \"안녕하세요~\"","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "설레")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j 보고 %s마음이 설레입니다.", "\n%M이 당신을 보고 %s당황해합니다.","\n%M이 %M%j 보고 %s마음이 설레입니다.","","3")
                else OUT1("당신은 %s마음이 설레입니다.", "\n%M이 %s마음이 설레입니다.","")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "놀려")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUTj2("당신은 %M%j %s놀립니다. 메롱메롱~~", "\n%M이 당신을 %s놀립니다. 메롱메롱~~","\n%M이 %M%j 보고 %s놀립니다. 메롱메롱~~","약오르게 ","3")
                else OUT1("당신은 %s놀립니다. 메롱메롱~~", "\n%M이 %s놀립니다. 메롱메롱~~","약오르게 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "생각")) {
                OUT1("당신은 %s생각합니다.", "\n%M이 %s생각합니다.","조심스럽게 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "부끄러")) {
                OUT1("당신은 %s부끄러워 합니다.", "\n%M이 %s부끄러워 합니다.","얼굴이 빨개져 ")
        }
        else if(!strcmp(cmdlist[cmdno].cmdstr, "구걸")) {
                if(crt_ptr && crt_ptr!=ply_ptr)
                     OUT2("당신은 %M에게 %s구걸합니다. \"한푼줍쇼~~\"", "\n%M이 당신에게 %s구걸합니다. \"한푼줍쇼~~\"", "\n%M이 %M에게 %s구걸합니다. \"한푼줍쇼~~\"","")
                else OUT1("당신은 바닥에 엎드려 %s구걸합니다. \"한푼줍쇼~~\"", "\n%M이 바닥에 엎드려 %s구걸합니다. \"한푼줍쇼~~\"","")
        }
	else if(!strcmp(cmdlist[cmdno].cmdstr, "구박")) {
		if(crt_ptr && crt_ptr!=ply_ptr)
		     OUTj2("당신은 %M%j %s구박합니다.","\n%M이 당신을 %s구박합니다.","\n%M이 %M%j %s구박합니다.","마구 ","3")
		else OUT1("당신은 스스로를 %s구박합니다.","\n%M이 스스로를 %s구박합니다.","마구 ")
	}
	else if(!strcmp(cmdlist[cmdno].cmdstr, "안아") || !strcmp(cmdlist[cmdno].cmdstr, "껴안아")) {
		if(crt_ptr && crt_ptr!=ply_ptr)
			OUTj2("당신은 %M%j %s껴안습니다.","\n%M이 당신을 %s껴안습니다.","\n%M이 %M%j %s껴안습니다.","꼭 ","3")
		else 	OUT1("당신은 %s껴안으려 애를 씁니다.","\n%M이 %s껴안으려 애를 쓰고 있습니다.","허공을 ")	
	}
        return(0);
}

