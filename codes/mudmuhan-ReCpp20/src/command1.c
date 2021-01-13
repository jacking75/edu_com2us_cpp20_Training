 /*
 * COMMAND1.C:
 *
 *      Command handling/parsing routines.
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "kstbl.h"
#include "mstruct.h"
#include "mextern.h"
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


char pass_num[PMAX];
long last_login[PMAX];

/**********************************************************************/
/*                              login                                 */
/**********************************************************************/

/* This function is the first function that gets input from a player when */
/* he logs in.  It asks for the player's name and password, and performs  */
/* the according function calls.                                          */

void login(fd, param, str)
int     fd;
int     param;
unsigned char    *str;
{
	FILE *fp;
		int             i;
		extern int      Numplayers;
		unsigned char   tempstr[20], str2[50], str3[50], nastr[20];
		long            t;
		creature        *ply_ptr;
              struct stat f_stat;
              char            tmp[256];
char file[80];
              struct tm *login_tt;
              char *wday[7]={"ÀÏ","¿ù","È­","¼ö","¸ñ","±İ","Åä",};

		switch(param) {
	case -1: str[0]=0;
		case 0:
                            pass_num[fd]=0;
				if(strcmp(Ply[fd].extr->tempstr[0], str)) {
						/* disconnect(fd);
						return; */
						RETURN(fd, login, 1);
				}
				print(fd, "\n´ç½ÅÀÇ ÀÌ¸§Àº ¹«¾ùÀÔ´Ï±î? ");
				RETURN(fd, login, 1);
		case 1:
				if(strlen(str)==0) {
						print(fd, "ÀÌ¸§Àº ÇÑ ÀÚ ÀÌ»óÀÌ¾î¾ß ÇÕ´Ï´Ù.\n");
					print(fd, "\n´ç½ÅÀÇ ÀÌ¸§Àº ¹«¾ùÀÔ´Ï±î? ");
					RETURN(fd, login, 1);
				}

				if(strlen(str) > 12) {
						print(fd, "ÀÌ¸§ÀÌ ³Ê¹« ±é´Ï´Ù.\n\n");
						print(fd, "´ç½ÅÀÇ ÀÌ¸§Àº ¹«¾ùÀÔ´Ï±î? ");
						RETURN(fd, login, 1);
				}

			if(!ishan(str)) {
				print(fd, "ÀÌ¸§Àº ÇÑ±Û·Î ÀûÀ¸¼Å¾ß ÇÕ´Ï´Ù.\n\n");
				print(fd, "´ç½ÅÀÇ ÀÌ¸§Àº ¹«¾ùÀÔ´Ï±î? ");
				RETURN(fd, login, 1);
			}

				lowercize(str, 1);
				str[25]=0;
                  sprintf(tmp,"%s/%s/%s",PLAYERPATH,first_han(str),str);
                  last_login[fd]=0;
                  if (!stat(tmp,&f_stat)) last_login[fd]=f_stat.st_ctime;

				if(load_ply(str, &ply_ptr) < 0) {
						strcpy(Ply[fd].extr->tempstr[0], str);
						print(fd, "\n%S%j ÇÏ½Ã°Ú½À´Ï±î(¿¹/¾Æ´Ï¿À)? ", str,"4");
						RETURN(fd, login, 2);
				}

				else {
						ply_ptr->fd = -1;
						Ply[fd].ply = ply_ptr;
		  if(strcmp(str , Ply[fd].ply->name)) {
 			write(fd, "\nµ¥ÀÌÅ¸°¡ ¼Õ»óµÇ¾ú½À´Ï´Ù.\n",25);
			if(str[0]==0 || pass_num[fd]>=3)
				write(fd,"Á¢¼ÓÀ» ²÷½À´Ï´Ù.",18);
				sleep(1);
				disconnect(fd);
				return;
			}
          if(F_ISSET(ply_ptr, SUICD)) {
/*
              uninit_ply(ply_ptr);
*/
				write(fd, "\nÀÚ»ì ½ÅÃ»ÇÑ ¾ÆÀÌµğÀÔ´Ï´Ù.\n", 18);
					sleep(1);
				disconnect(fd);
				return;
          }
						if(checkdouble(ply_ptr->name)) {
								write(fd, "ÀÌ»óÇÏ±º¿ä.. -_-;.\n", 25);
								disconnect(fd);
								return;
						}
					  print(fd, "¾ÏÈ£¸¦ ³Ö¾î ÁÖ½Ê½Ã¿ä: ");
                                          print(fd,"%c%c%c", 255,251,1);
					  RETURN(fd, login, 3);
				}

		case 2:
				if(strcmp(str,"¿¹") && str[0]!='y' && str[0]!='Y') {
						Ply[fd].extr->tempstr[0][0] = 0;
						print(fd, "´ç½ÅÀÇ ÀÌ¸§Àº ¹«¾ùÀÔ´Ï±î? ");
						RETURN(fd, login, 1);
				}
				else {
						print(fd, "\n[¿£ÅÍ]¸¦ ´©¸£½Ê½Ã¿ä.");
						RETURN(fd, create_ply, 1);
				}

		case 3:
                check_item(Ply[fd].ply);      
				if(strcmp(str, Ply[fd].ply->password)) {
						write(fd, "\n¾ÏÈ£°¡ Æ²¸³´Ï´Ù. ", 18);
                                              pass_num[fd]++;
						if(str[0]==0 || pass_num[fd] >= 3) {
    							t = time(0);
							strcpy(str3, (char *)ctime(&t));
							str3[strlen(str3)-1] = 0;
							log_fl(str3, Ply[fd].ply->name);
							write(fd,"Á¢¼ÓÀ» ²÷½À´Ï´Ù.\n\n",18);
                                                        disconnect(fd);
                                                        return;
                                                }
                                                else {
                                                        print(fd,"´Ù½Ã ÀÔ·ÂÇÏ½Ê½Ã¿ä.\n¾ÏÈ£¸¦ ´Ù½Ã ÀÔ·ÂÇÏ¼¼¿ä: ");
                                                        RETURN(fd, login, 3);
                                                }
				}
				else {
						print(fd, "%c%c%c\n",255,252,1);
						strcpy(tempstr, Ply[fd].ply->name);
						for(i=0; i<Tablesize; i++)
								if(Ply[i].ply && i != fd)
										if(!strcmp(Ply[i].ply->name,
										   Ply[fd].ply->name))
												disconnect(i);
						free_crt(Ply[fd].ply);
				if(load_ply(tempstr, &Ply[fd].ply) < 0)
				{
						write(fd, "Player nolonger exits!\n", 23);
						t = time(0);
						strcpy(str2, (char *)ctime(&t));
						str2[strlen(str2)-1] = 0;
						logn("sui_crash","%s: %s (%s) ´Â ÀÚ»ìÇÏ¿´½À´Ï´Ù.\n",
								str2, Ply[fd].ply->name, Ply[fd].io->address);
						disconnect(fd);
						return;
				}
						Ply[fd].ply->fd = fd;


                                                init_ply(Ply[fd].ply);
                                                init_alias(Ply[fd].ply);


             if(last_login[fd]) {
                 login_tt=localtime(&last_login[fd]);
                 print(fd,"\n¸¶Áö¸· Á¢¼Ó½Ã°£: %d¿ù %dÀÏ(%s) %d½Ã %dºĞÀÔ´Ï´Ù.\n",
                 login_tt->tm_mon+1, login_tt->tm_mday, wday[login_tt->tm_wday],
                 login_tt->tm_hour, login_tt->tm_min);
            }

						RETURN(fd, command, 1);
				}
		}
}

/**********************************************************************/
/*                              create_ply                            */
/**********************************************************************/

/* This function allows a new player to create his or her character. */

void create_ply(fd, param, str)
int     fd;
int     param;
char    *str;
{
		int     i, k, l, n, sum;
		int     num[5];

		switch(param) {
		case 1:
				print(fd,"\n\n");
				Ply[fd].ply = (creature *)malloc(sizeof(creature));
				if(!Ply[fd].ply)
						merror("create_ply", FATAL);
				zero(Ply[fd].ply, sizeof(creature));
				Ply[fd].ply->fd = -1;
				Ply[fd].ply->rom_num = 1;
				print(fd, "´ç½ÅÀº ³²ÀÚÀÔ´Ï±î, ¿©ÀÚÀÔ´Ï±î(³²ÀÚ/¿©ÀÚ)? ");
				RETURN(fd, create_ply, 2);
		case 2:
				if(strncmp(str,"³²", 2) && strncmp(str,"¿©",2)) {
						print(fd, "ÀÔ·ÂÀÌ Àß¸øµÇ¾ú½À´Ï´Ù.\n\n´ç½ÅÀº ³²ÀÚÀÔ´Ï±î, ¿©ÀÚÀÔ´Ï±î(³²ÀÚ/¿©ÀÚ)? ");
						RETURN(fd, create_ply, 2);
				}
				if(!strncmp(str,"³²",2))
						F_SET(Ply[fd].ply, PMALES);
				print(fd, "\n´ÙÀ½°ú °°Àº Á÷¾÷ÀÌ ÀÖ½À´Ï´Ù.\n");
				print(fd, "1.ÀÚ  °´  2.±Ç¹ı°¡  3.ºÒÁ¦ÀÚ  4.°Ë  »ç\n");
				print(fd, "5.µµ¼ú»ç  6.¹«  »ç  7.Æ÷  Á¹  8.µµ  µÏ\n");
				print(fd, "Á÷¾÷À» °í¸£¼¼¿ä: ");
				RETURN(fd, create_ply, 3);
		case 3:
				switch(low(str[0])) {
						case '1': Ply[fd].ply->class = ASSASSIN; break;
						case '2': Ply[fd].ply->class = BARBARIAN; break;
						case '3': Ply[fd].ply->class = CLERIC; break;
						case '4': Ply[fd].ply->class = FIGHTER; break;
						case '5': Ply[fd].ply->class = MAGE; break;
						case '6': Ply[fd].ply->class = PALADIN; break;
						case '7': Ply[fd].ply->class = RANGER; break;
						case '8': Ply[fd].ply->class = THIEF; break;
						default: print(fd, "Á÷¾÷À» °í¸£¼¼¿ä: ");
								 RETURN(fd, create_ply, 3);
				}
				print(fd, "\n´ç½ÅÀº 54Á¡À¸·Î ´ÙÀ½ 5°¡Áö ´É·ÂÄ¡¸¦ ±¸¼ºÇÒ¼ö ÀÖ½À´Ï´Ù.\n\
3ÀÌ»ó 18ÀÌÇÏÀÇ ¼öÄ¡·Î ## ## ## ## ##ÀÇ Çü½ÄÀ¸·Î 5°¡Áö ´É·ÂÄ¡¸¦ Àû¾îÁÖ½Ê½Ã¿ä.\n\
´É·Â: Èû ¹ÎÃ¸ ¸ËÁı Áö½Ä ½Å¾Ó½É\n¿¹: 12 10 12 10 10\n\n");
/* Strength, Dexterity, Constitution, Intelligence, Piety. */

				print(fd, ": ");
				RETURN(fd, create_ply, 4);
		case 4:
				n = strlen(str); l = 0; k = 0;
				for(i=0; i<=n; i++) {
						if(str[i]==' ' || str[i]==0) {
								str[i] = 0;
								num[k++] = atoi(&str[l]);
								l = i+1;
						}
						if(k>4) break;
				}
				if(k<5) {
						print(fd, "5°¡Áö ´É·ÂÄ¡ ¸ğµÎ¸¦ À§ÀÇ Çü½Ä´ë·Î Àû¾î ÁÖ½Ê½Ã¿ä.\n");
						print(fd, ": ");
						RETURN(fd, create_ply, 4);
				}
				sum = 0;
				for(i=0; i<5; i++) {
						if(num[i] < 3 || num[i] > 18) {
								print(fd, "°¢ ´É·ÂÄ¡´Â 3ÀÌ»ó 18ÀÌÇÏ·Î ¼³Á¤ÇØ¾ß ÇÕ´Ï´Ù.\n");
								print(fd, ": ");
								RETURN(fd, create_ply, 4);
						}
						sum += num[i];
				}
				if(sum > 54) {
						print(fd, "°¢ ´É·ÂÄ¡ÀÇ ÇÕÀÌ 54Á¡À» ÃÊ°úÇÒ¼ö ¾ø½À´Ï´Ù.\n");
						print(fd, ": ");
						RETURN(fd, create_ply, 4);
				}
				Ply[fd].ply->strength = num[0];
				Ply[fd].ply->dexterity = num[1];
				Ply[fd].ply->constitution = num[2];
				Ply[fd].ply->intelligence = num[3];
				Ply[fd].ply->piety = num[4];
				print(fd, "\n´ç½Å¿¡°Ô ÀÍ¼÷ÇÑ ¹«±â¸¦ °í¸£½Ê½Ã¿ä.\n");
				print(fd, "1.µµ   2.°Ë   3.ºÀ   4.Ã¢   5.±Ã.\n");
				print(fd, ": ");
				RETURN(fd, create_ply, 5);
		case 5:
				switch(low(str[0])) {
						case '1': Ply[fd].ply->proficiency[0]=1024; break;
						case '2': Ply[fd].ply->proficiency[1]=1024; break;
						case '3': Ply[fd].ply->proficiency[2]=1024; break;
						case '4': Ply[fd].ply->proficiency[3]=1024; break;
						case '5': Ply[fd].ply->proficiency[4]=1024; break;
						default: print(fd, "´Ù½Ã °í¸£¼¼¿ä.\n: ");
								 RETURN(fd, create_ply, 5);
				}
				print(fd, "\n¼±ÇÑ ±¸¼º¿øÀº ´Ù¸¥»ç¶÷À» °ø°İÇÏÁö ¸øÇÏ°í °ø°İ ¹ŞÀ»¼öµµ ¾øÀ¸¸ç");
				print(fd, "\n±× ±¸¼º¿ø¿¡°Ô¼­ ¹°°ÇÀ» ÈÉÄ¥¼öµµ ¾ø½À´Ï´Ù.");
				print(fd, "\n±×·¯³ª ¾ÇÇÑ ±¸¼º¿øÀº °ø°İÇÒ¼öµµ ÀÖ°í ¹°°ÇÀ» ÈÉÄ¥¼öµµ ÀÖÀ¸¸ç");
				print(fd, "\n´Ù¸¥ ¾ÇÇÑ ±¸¼º¿øµé¿¡°Ô °ø°İÀ» ¹ŞÀ» ¼öµµ ÀÖ½À´Ï´Ù.\n");
				print(fd, "\n¼ºÇâÀ» °í¸£½Ê½Ã¿ä(¼±ÇÔ/¾ÇÇÔ): ");
				RETURN(fd, create_ply, 6);
		case 6:
				if(!strncmp(str,"¾Ç",2))
						F_SET(Ply[fd].ply, PCHAOS);
				else if(!strncmp(str,"¼±",2))
						F_CLR(Ply[fd].ply, PCHAOS);
				else {
						print(fd, "¼ºÇâÀ» °í¸£½Ê½Ã¿ä(¼±ÇÔ/¾ÇÇÔ): ");
						RETURN(fd, create_ply, 6);
				}
				print(fd, "\n´ÙÀ½°ú °°Àº Á¾Á·µéÀÌ ÀÖ½À´Ï´Ù.");
				print(fd, "\n1.³­ÀåÀÌÁ·  2.¿ë ½Å Á·  3.¶¥±Í½ÅÁ· 4.¿ä ±« Á·");
				print(fd, "\n5.°Å ÀÎ Á·  6.Åä ½Å Á·  7.ÀÎ °£ Á· 8.µµ±úºñÁ·");
				print(fd, "\nÁ¾Á·À» °í¸£½Ê½Ã¿ä: ");
				RETURN(fd, create_ply, 7);
		case 7:
				switch(low(str[0])) {
				case '1': Ply[fd].ply->race = DWARF; break;
				case '2': Ply[fd].ply->race = ELF; break;
				case '3': Ply[fd].ply->race = GNOME; break;
				case '8': Ply[fd].ply->race = ORC; break;
				case '4': Ply[fd].ply->race = HALFELF; break;
				case '5': Ply[fd].ply->race = HALFGIANT; break;
				case '6': Ply[fd].ply->race = HOBBIT; break;
				case '7': Ply[fd].ply->race = HUMAN; break;
				}
				if(!Ply[fd].ply->race) {
						print(fd, "\nÁ¾Á·À» °í¸£½Ê½Ã¿ä: ");
						RETURN(fd, create_ply, 7);
				}

				switch(Ply[fd].ply->race) {
				case DWARF:
						Ply[fd].ply->strength++;
						Ply[fd].ply->piety--;
						break;
				case ELF:
						Ply[fd].ply->intelligence+=2;
						Ply[fd].ply->constitution--;
						Ply[fd].ply->strength--;
						break;
				case GNOME:
						Ply[fd].ply->piety++;
						Ply[fd].ply->strength--;
						break;
				case HALFELF:
						Ply[fd].ply->intelligence++;
						Ply[fd].ply->constitution--;
						break;
				case HOBBIT:
						Ply[fd].ply->dexterity++;
						Ply[fd].ply->strength--;
						break;
				case HUMAN:
						Ply[fd].ply->constitution++;
						break;
				case ORC:
						Ply[fd].ply->strength++;
						Ply[fd].ply->constitution++;
						Ply[fd].ply->dexterity--;
						Ply[fd].ply->intelligence--;
						break;
				case HALFGIANT:
						Ply[fd].ply->strength+=2;
						Ply[fd].ply->intelligence--;
						Ply[fd].ply->piety--;
						break;
				}

				print(fd, "\n»õ ¾ÏÈ£¸¦ ³ÖÀ¸½Ê½Ã¿ä(3ÀÚÀÌ»ó 14ÀÚÀÌÇÏ): ");
                                print(fd,"%c%c%c",255,251,1);
				RETURN(fd, create_ply, 8);
		case 8:
				if(strlen(str) > 14) {
						print(fd, "ÀÔ·ÂµÈ ¾ÏÈ£°¡ ³Ê¹« ±é´Ï´Ù.\n¾ÏÈ£¸¦ ´Ù½Ã ³ÖÀ¸½Ê½Ã¿ä(3ÀÚÀÌ»ó 14ÀÚÀÌÇÏ): ");
						RETURN(fd, create_ply, 8);
				}
				if(strlen(str) < 3) {
						print(fd, "ÀÔ·ÂµÈ ¾ÏÈ£°¡ ³Ê¹« Âª½À´Ï´Ù.\n¾ÏÈ£¸¦ ´Ù½Ã ³ÖÀ¸½Ê½Ã¿ä(3ÀÚÀÌ»ó 14ÀÚÀÌÇÏ): ");
						RETURN(fd, create_ply, 8);
				}
				strncpy(Ply[fd].ply->password, str, 14);
				strcpy(Ply[fd].ply->name, Ply[fd].extr->tempstr[0]);
				up_level(Ply[fd].ply);
				Ply[fd].ply->fd = fd;
				init_ply(Ply[fd].ply);
                               init_alias(Ply[fd].ply);
		F_SET(Ply[fd].ply,PLECHO);
		F_SET(Ply[fd].ply,PPROMP);
                Ply[fd].ply->gold = 500;
				save_ply(Ply[fd].ply->name, Ply[fd].ply);
				print(fd, "%c%c%c\n",255,252,1);

				print(fd, "[È¯¿µ]ÀÌ¶ó°í Ä¡½Ã¸é ÃÊº¸ÀÚ ºĞµé¿¡°Ô µµ¿òÀÌ µÇ´Â ¸¹Àº Á¤º¸¸¦ ¾òÀ»¼ö ÀÖ½À´Ï´Ù.\n");
				print(fd, "·¹º§ 5 °¡ µÇÁö ¾ÊÀ¸¸é ¾ÆÀÌµğ°¡ »èÁ¦µÉ ¼öµµ ÀÖ½À´Ï´Ù.\n");

				RETURN(fd, command, 1);
		}
}

/**********************************************************************/
/*                              command                               */
/**********************************************************************/

/* This function handles the main prompt commands, and calls the        */
/* appropriate function, depending on what service is requested by the  */
/* player.                                                              */

void command(fd, param, str)
int     fd;
int     param;
char    *str;
{
		cmd     cmnd;
		int     n;
		unsigned char ch;
		int     i;
		char buf[256];

#ifdef RECORD_ALL
/*
this logn commands wil print out all the commands entered by players.
It should be used in extreme case hen trying to isolate a players
input which causes a crash.
*/

logn("all_cmd","\n%s-%d (%d): %s\n",Ply[fd].ply->name,fd,Ply[fd].ply->rom_num,str);
#endif RECORD_ALL

		switch(param) {
		case 1:

				if(F_ISSET(Ply[fd].ply, PHEXLN)) {
						for(n=0;n<strlen(str);n++) {
								ch = str[n];
								print(fd, "%02X", ch);
						}
						print(fd, "\n");
				}

				if(!strcmp(str, "!"))
						strncpy(str, Ply[fd].extr->lastcommand, 79);
				else if(str[0]=='!') {
						strncpy(buf, Ply[fd].extr->lastcommand, 79);
						strncat(buf,&str[1],79);
						strncpy(str,buf,79);
				}

				if(str[0]) {
						for(n=0; str[n] && str[n] == ' '; n++) ;
						strncpy(Ply[fd].extr->lastcommand, &str[n], 79);
				}

				strncpy(cmnd.fullstr, str, 255);
				lowercize(str, 0);
				parse(str, &cmnd); n = 0;

				/* ÆÄ¼­ ºĞ¼® ½ÃÇè¿ë.. */
/*      print(fd,"ÆÄ¼­ ºĞ¼®....\n");
		for(i=0;i<cmnd.num;i++) print(fd,"%d: %s %d\n",i+1,cmnd.str[i],cmnd.val[i]);
*/
				if(cmnd.num){
					if((n=alias_cmd(Ply[fd].ply,&cmnd))==4000) n = process_cmd(fd, &cmnd);
				} else
						n = PROMPT;

				if(n == DISCONNECT) {
						write(fd, "¾È³çÈ÷ °¡½Ê½Ã¿ä!\n\n\n", 19);
						disconnect(fd);
						return;
				}
				else if(n == PROMPT) {
						if(F_ISSET(Ply[fd].ply, PPROMP))
								sprintf(str, "(%d Ã¼·Â %d µµ·Â): ",
										Ply[fd].ply->hpcur, Ply[fd].ply->mpcur);
						else
								strcpy(str, ": ");
						write(fd, str, strlen(str));
						if(Spy[fd] > -1) write(Spy[fd], str, strlen(str));
				}

				if(n != DOPROMPT) {
						RETURN(fd, command, 1);
				}
				else
						return;
		}
}

/**********************************************************************/
/*                              parse                                 */
/**********************************************************************/

/* This function takes the string in the first parameter and breaks it */
/* up into its component words, stripping out useless words.  The      */
/* resulting words are stored in a command structure pointed to by the */
/* second argument.                                                    */

void parse(str, cmnd)
char    *str;
cmd     *cmnd;
{
		int     i, j, l, m, n, o, art;
		unsigned char   tempstr[25];

		l = m = n = 0;
		j = strlen(str);

		/* include for processing hangul command */
		if(j!=0)
		if(str[j-1]==' ' || str[j-1]=='.' || str[j-1]=='!' || str[j-1]=='?') {
				strncpy(cmnd->str[n++],"¸»",20);
				cmnd->val[m] = 1L;
		}
		else {
				i=j;
				while(i>=0 && str[i]!=' ') i--;
				if(i>=0) str[i]=0;
				j=i; i++;
				strncpy(cmnd->str[n++], &str[i], 20);
				cmnd->val[m] = 1L;
		}
		/* --- end --- */

		for(i=0; i<=j; i++) {
				if(str[i] == ' ' || str[i] == '#' || str[i] == 0) {
						str[i] = 0;     /* tokenize */

						/* Strip extra white-space */
						while((str[i+1] == ' ' || str[i] == '#') && i < j+1)
								str[++i] = 0;

						strncpy(tempstr, &str[l], 24); tempstr[24] = 0;
						l = i+1;
						if(!strlen(tempstr)) continue;

						/* Ignore article/useless words */
						/*
						o = art = 0;
						while(article[o][0] != '@') {
								if(!strcmp(article[o++], tempstr)) {
										art = 1;
										break;
								}
						}
						if(art) continue;
						*/

						/* Copy into command structure */
						if(n == m) {
								strncpy(cmnd->str[n++], tempstr, 20);
								cmnd->val[m] = 1L;
						}
						else if(is_number(tempstr) || (tempstr[0] == '-' &&
								is_number(tempstr+1))) {
								cmnd->val[m++] = atol(tempstr);
						}
						else {
								strncpy(cmnd->str[n++], tempstr, 20);
								cmnd->val[m++] = 1L;
						}

				}
				if(m >= COMMANDMAX) {
						n = 5;
						break;
				}
		}

		if(n > m)
				cmnd->val[m++] = 1L;
		cmnd->num = n;

}

/**********************************************************************/
/*                              process_cmd                           */
/**********************************************************************/

/* This function takes the command structure of the person at the socket */
/* in the first parameter and interprets the person's command.           */

int process_cmd(fd, cmnd)
int     fd;
cmd     *cmnd;
{
		int     match=0, cmdno=0, c=0, n;
		int current_deep,match_deep=0;

		do {
				if(!strcmp(cmnd->str[0], cmdlist[c].cmdstr)) {
						match = 1;
						cmdno = c;
						break;
				}
				else if((current_deep=str_compare(cmnd->str[0], cmdlist[c].cmdstr))!=0) {
						match = 1;
						if(match_deep==0 || match_deep>current_deep) {
								cmdno = c;
								match_deep = current_deep;
						}
				}
				c++;
		} while(cmdlist[c].cmdno);

		if(match == 0 || (cmnd->str[0][0]=='*' &&
                  (Ply[fd].ply->class<CARETAKER &&
                   Ply[fd].ply->class!=ZONEMAKER))) {
				print(fd, "\"%s\": ÀÌ·± ¸í·É¾î´Â ¾ø³×¿ä.",
					  cmnd->str[0]);
				RETURN(fd, command, 1);
		}
/* °¡Àå ºñ½ÁÇÑ ¸í·É¾î¸¦ ¾²±â À§ÇØ ¼öÁ¤.. ¿©·¯ ³¹¸»ÀÌ ³ª¿Ã°æ¿ì ¿¡·¯Ã³¸®=>»ç¿ë
		else if(match > 1) {
				print(fd, "¸í·ÉÀ» Àß ¸ğ¸£°Ú³×¿ä.");
				RETURN(fd, command, 1);
		}
*/
		if(cmdlist[cmdno].cmdno < 0)
				return(special_cmd(Ply[fd].ply, 0-cmdlist[cmdno].cmdno, cmnd));

		/* DM command log */
		if(cmnd->str[0][0]=='*') {
		    log_dmcmd("%s : %s :\n",Ply[fd].ply->name,cmnd->fullstr);
		}

		return((*cmdlist[cmdno].cmdfn)(Ply[fd].ply, cmnd));

}

#ifdef CHECKDOUBLE

int checkdouble(name)
char *name;
{
		char    path[128], tempname[80];
		FILE    *fp;
		int     rtn=0;

		sprintf(path, "%s/simul/%s", PLAYERPATH, name);
		fp = fopen(path, "r");
		if(!fp)
				return(0);

		while(!feof(fp)) {
				fgets(tempname, 80, fp);
				tempname[strlen(tempname)-1] = 0;
				if(!strcmp(tempname, name))
						continue;
				if(find_who(tempname)) {
						rtn = 1;
						break;
				}
		}

		fclose(fp);
		return(rtn);
}

#endif

char cut_paste_chr;

int cut_command(str)
char *str;
{
	int i;
	
	for(i=strlen(str)-1;i>=0;i--) {
		if(str[i]==' ') {
			break;
		}
	}
	if(i<0) i=0;
	cut_paste_chr=str[i];
	str[i]=0;

	return i;
}

void paste_command(str,index)
char *str; int index;
{
	str[index]=cut_paste_chr;
}

int ishan(str)
unsigned char *str;
{
	int i;
	for(i=0;i<strlen(str);i+=2) {
		if(!is_hangul(str+i)) return 0;
	}
	return 1;
}

int is_hangul(str)
unsigned char *str;    /* one character */
{
	/* ¼ø¼öÇÑ ÇÑ±ÛÀÎÁö °Ë»ç */
	if(str[0]>=0xb0 && str[0]<=0xc8 && str[1]>=0xa1 && str[1]<=0xfe) return 1;
	return 0;
}

int under_han(str)
unsigned char *str;
{
	unsigned char high,low;
	int len;

	len=strlen(str);
	if(len<2) return 0;
	if(str[len-1]==')') while(str[len]!='(') len--;

	high=str[len-2];
	low=str[len-1];

	if(!is_hangul(&str[len-2])) return 0;

	high=KStbl[(high-0xb0)*94+low-0xa1] & 0x1f;
	if(high<2 || high>28) return 0;
	return 1;
}


char *first_han(str)
unsigned char *str;
{
    unsigned char high,low;
    int len,i;
    char *p = "temp";
    static unsigned char *exam[]={
        "°¡", "°¡", "³ª", "´Ù", "´Ù",
        "¶ó", "¸¶", "¹Ù", "¹Ù", "»ç",
        "»ç", "¾Æ", "ÀÚ", "ÀÚ", "Â÷", 
        "Ä«", "Å¸", "ÆÄ", "ÇÏ", "" };
    static unsigned char *johab_exam[]={
        "ˆa", "Œa", "a", "”a", "˜a",
        "œa", " a", "¤a", "¨a", "¬a",
        "°a", "´a", "¸a", "¼a", "Àa",
        "Äa", "Èa", "Ìa", "Ğa", "" };

    len=strlen(str);
    if(len<2) return p;

    high=str[0];
    low=str[1];

    if(!is_hangul(&str[0])) return p;
    high=(KStbl[(high-0xb0)*94+low-0xa1] >> 8) & 0x7c;
    for(i=0;johab_exam[i][0];i++) {
        low= (johab_exam[i][0] & 0x7f);
        if(low==high) return exam[i];
    }
    return p;
}

int is_number(str)
unsigned char *str;
{
	int i;
	for(i=0;i<strlen(str);i++) {
		if(!isdigit(str[i])) return 0;
	}
	return 1;
}

int return_square(ply_ptr,cmnd)
creature    *ply_ptr;
cmd     *cmnd;
{
	room    *rom_ply;
	int fd;
        ctag    *cp;

	rom_ply=ply_ptr->parent_rom;
	fd=ply_ptr->fd;

	if(ply_is_attacking(ply_ptr,cmnd)) {
		print(fd,"´ç½ÅÀº ½Î¿ì°í ÀÖ´Â ÁßÀÔ´Ï´Ù!!");
		return 0;
	}

	if(rom_ply->rom_num==1001) {
		print(fd,"´ç½ÅÀº ÀÌ¹Ì ±¤Àå¿¡ ¿Í ÀÖ½À´Ï´Ù!");
		return 0;
	}

        if(ply_ptr->following) {
            cp = ply_ptr->following->first_fol;
        }
        else {
            cp = ply_ptr->first_fol;
        }
        if(cp){
            print(fd,"¸ÕÀú ±×·ì¿¡¼­ ³ª¿À¼¼¿ä.");
            return(0);
        }

	if(ply_ptr->level>20 && ply_ptr->class<INVINCIBLE) {
		print(fd, "´ç½ÅÀÌ ±ÍÈ¯ÇÏ·ÁÇÏÀÚ Èæ¾ÏÀÇ ¼¼·ÂÀÌ ´ç½ÅÀÇ µµ·ÂÀ» »¯½À´Ï´Ù.\n");
		ply_ptr->mpcur = 0;
		/*
		ply_ptr->experience -= (ply_ptr->experience)/100000;
		*/
	}


	print(fd, "´ç½ÅÀÌ \"±ÍÈ¯!\"ÀÌ¶ó°í ¿ÜÄ¡ÀÚ ÀÌ»óÇÑ Èû¿¡ ÀÇÇØ ¾îµò°¡·Î »¡·Áµé¾î°©´Ï´Ù.");
	if(!F_ISSET(ply_ptr,PDMINV)) broadcast_rom(fd,ply_ptr->rom_num,"\n%m´ÔÀÌ °©ÀÚ±â »ç¶óÁı´Ï´Ù!",ply_ptr);

	del_ply_rom(ply_ptr,rom_ply);
	if(!F_ISSET(ply_ptr,PFRTUN))
		load_rom(1001,&rom_ply);
	else	
		load_rom(3300 + ply_ptr->daily[DL_EXPND].max, &rom_ply);
	add_ply_rom(ply_ptr,rom_ply);
	if(!F_ISSET(ply_ptr,PDMINV)) broadcast_rom(fd,ply_ptr->rom_num, "\n%m´ÔÀÌ °©ÀÚ±â ÀÚ¿íÇÑ ¿¬±â¿Í ÇÔ²² ³ªÅ¸³µ½À´Ï´Ù!",ply_ptr);
	return 0;
}

int ply_is_attacking(ply_ptr,cmnd)
creature        *ply_ptr;
cmd             *cmnd;
{
    room            *rom_ptr;
    ctag            *cp;
    creature        *crt_ptr;

    rom_ptr=ply_ptr->parent_rom;

    cp = rom_ptr->first_mon;
    while(cp) {
        if(find_enm_crt(ply_ptr->name,cp->crt)>-1) return 1;
        cp = cp->next_tag;
    }
    return 0;
}

int str_compare(str1,str2)
unsigned char *str1,*str2;
{
	int i=0;

	while(str1[i]!=0 && str2[i]!=0) {
		if(str1[i]==str2[i] && str1[i+1]==str2[i+1]) i+=2;
	else if(str1[i]==str2[i] && str1[i]<128) i++;
		else break;
	}
	if(str1[i]!=0) return 0;
	return i;
}

char *cut_space(str)
char *str;
{
	static char buf[512];
	int i;

	strcpy(buf,str);
	for(i=strlen(buf)-1;i>=0;i--) {
		if(buf[i]!=' ') break;
	}
	buf[i+1]=0;

	return buf;
}






