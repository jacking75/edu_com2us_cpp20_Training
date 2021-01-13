/*
 * MUDDLE.C:
 *
 *	A multi-function client program for playing Multi-User
 *	Dungeon (MUD) games.
 *
 *	Copyright (C) 1992 Brett J. Vickers
 *
 */

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/signal.h>
};
#include "muddle.h"

macro 	*First_macro;
site	*First_site;
auton	*First_auton;
conn	*Conn[10];
int	Numconnections;
int	Target;
int	Tablesize;
fd_set	Mask;

int main(int argc, char *argv[])
{
	printf("Muddle v1.0 by Brett J. Vickers (bvickers@ics.uci.edu)\n");
	load_macros();

	signal(SIGINT, sigint);
	signal(SIGTSTP, sigstop);

	FD_SET(0, &Mask);
	Conn[0] = new conn(0);
	Conn[0]->fd = 0;

	Tablesize = getdtablesize();

	if(argc == 3) {
		open_connection(argv[1], argv[2], 0);
		client();
	}

	else if(argc == 2) {
		open_connection(argv[1]);
		client();
	}

	else client();
}

void client(void)
{
	char str[512];
	int n;

	setbuf(stdout, 0);
	setbuf(stdin, 0);

	while(1) {
		if(!Numconnections || !Target) {
			printf("Muddle> "); fflush(stdout);
			n = (int)fgets(str, 512, stdin);
			str[strlen(str)-1] = 0;
			handle_command(str);
		}
		else {
			io_check();
			Conn[Target]->output_buf();
		}
	}
}

void handle_command(char *str)
{
	int n;
	char *array[3];
	int len[3];

	if(!str[0]) return;

	n = parse(str, array, len, 3);

	if(array[0][0] == '?') {
		printf("(c)onnect      Connect to a site\n");
		printf("(d)isconnect   Disconnect from a connection\n");
		printf("(l)ist         List connections\n");
		printf("(m)acros       List macros\n");
		printf("(q)uit         Quit from Muddle\n");
		printf("(r)eload       Reload macro file\n");
		printf("(s)ites        List sites\n");
		printf("(in=...)       Define a macro on the fly\n");
		printf("(site=...)     Define a site macro on the fly\n");
		printf("<number>       Set current connection\n");
	}
		
	if(array[0][0] == 0 || str[0] == 0)
		Target = 0;

	else if(array[0][0] == 'q')
		sigint(0);

	else if(array[0][0] == 'r') {
		while(First_macro) {
			macro *m = First_macro;
			First_macro = First_macro->next;
			delete m;
		}
		for(site *s=First_site; s; ) {
			site *temp = s;
			s = s->next;
			delete temp;
		}
		for(auton *a=First_auton; a; ) {
			auton *temp = a;
			a = a->next;
			delete temp;
		}
		First_site = (site *)0;
		First_macro = (macro *)0;
		First_auton = (auton *)0;
		load_macros();
	}

	else if(array[0][0] == 'c') {
		if(n==2)
			open_connection(array[1]);
		if(n==3)
			open_connection(array[1], array[2], 0);
	}

	else if(array[0][0] == 'd' && n > 1) {
		int d = array[1][0] - '0';
		if(d < 1 || d > 9 || !Conn[d]) {
			printf("Illegal disconnect request.\n");
			return;
		}
		FD_CLR(Conn[d]->fd, &Mask);
		Numconnections--;
		delete Conn[d];
		Conn[d] = 0;
		if(d == Target) Target = 0;
		printf("Connection #%d disconnected.\n", d);
	}

	else if(array[0][0] == 'm') {
		printf("Stored macros:\n");
		for(macro *m = First_macro; m; m = m->next)
			printf("   Key: %-10.10s  Output: %-10.50s\n",
				m->key, m->output);
	}

	else if(array[0][0] == 's') {
		printf("Stored sites:\n");
		for(site *s = First_site; s; s = s->next)
			printf("   Key: %-10.10s  Addr: %-20.20s  Port: %4s\n",
				s->key, s->address, s->port);
	}

	else if(isdigit(array[0][0]) && n==1) {
		n = atoi(array[0]);
		if(n > 0 && n < 10)
			if(Conn[n]) {
				Target = n;
				printf("Reconnecting to %s, port %s.\n",
					Conn[n]->addr, Conn[n]->port);
			}
	}

	else if(isdigit(array[0][0])) {
		n = atoi(array[0]);
		if(n > 0 && n < 10)
			if(Conn[n]) {
				Conn[n]->send(array[1]);
				Conn[n]->send("\n");
			}
	}

	else if(array[0][0] == 'l') {
		printf("Current connections:\n");
		for(int i=1; i<10; i++) 
			if(Conn[i])
				printf(
				"   Connection #%d:  %20.20s, port %04s %s\n",
					i, Conn[i]->addr, Conn[i]->port,
					Target == i ? "(*)":"");
	}
			
	else if(!strncmp(array[0], "in", 2) || !strncmp(array[0], "site", 4)) {
		char *key, *str1, *str2, *str3;
		n = read_parse_macro(str, &key, &str1, &str2, &str3);
		if(n<0) printf("Parse error.\n");
		else if(n == MACRO) {
			printf("Macro defined.\n");
			First_macro = new macro(key, str1, First_macro);
		}
		else if(n == SITE) {
			printf("Site defined.\n");
			First_site = new site(key, str1, str2, 
			    str3, First_site);
		}
	}

}

void io_check(void)
{
	fd_set mask;
	int n;
	static timeval t = { 0, 75000 };
	static char str[DEFSIZE], last[DEFSIZE];

	mask = Mask;
	if(select(Tablesize, &mask, 0, 0, &t) > 0)
		for(int i=0; i<10; i++)
			if(Conn[i] && FD_ISSET(Conn[i]->fd, &mask)) {
				n = Conn[i]->update_buf();
				if(n < 0) {
					FD_CLR(Conn[i]->fd, &Mask);
					delete Conn[i];
					if(Target == i) Target = 0;
					Numconnections--;
					Conn[i] = 0;
				}
			}

	if(Conn[0]->check_command(str) && Target) {
		if(str[0] == '`' && last[0])
			strcpy(str, last);
		else
			strcpy(last, str);
		if(str[0] == '#' && str[1] != '#') {
			str[strlen(str)-1] = 0;
			handle_command(&str[1]);
		}
		else if(str[0] == '#')
			Conn[Target]->send(&str[1]);
		else {
			str[strlen(str)-1] = 0;
			n = write_parse_macro(str);
			if(!n) {
				Conn[Target]->send(str);
				Conn[Target]->send("\n");
			}
		}
	}

}

void open_connection(char *addr, char *port, char slot)
{
	long netaddr;

	if(isdigit(addr[0])) {
		if((netaddr = inet_addr(addr)) == -1) {
			fprintf(stderr, "Unable to compute address.\n");
			return;
		}
	}
	else {
		struct hostent *host;
		host = gethostbyname(addr);
		if(!host) {
			fprintf(stderr, "Unknown host address.\n");
			return;
		}
		netaddr = *(long *)(host->h_addr);
	}

	if(!atoi(port) || atoi(port) < 0) {
		fprintf(stderr, "Illegal port address: %s.\n", port);
		return;
	}

	struct sockaddr_in sin;
	sin.sin_addr.s_addr = netaddr;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(port));

	printf("Trying %s, port %s ...\n", addr, port);

	int i;

	if(slot > 0 && slot < 10) {
		i = slot;
		if(Conn[i]) i = 10;
	}

	else {
		for(i=0; i<10; i++)
			if(!Conn[i]) break;
	}

	if(i==10) {
		fprintf(stderr, "Invalid connection request.\n");
		return;
	}

	Conn[i] = new conn(i);
	Conn[i]->fd = socket(AF_INET, SOCK_STREAM, 0);

	if(connect(Conn[i]->fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		fprintf(stderr, "Unable to connect.\n");
		delete Conn[i]; Conn[i] = 0;
		return;
	}

	printf("Connected to %s, port %s.\n", addr, port);

	Conn[i]->addr = new char[strlen(addr)+1];
	strcpy(Conn[i]->addr, addr);
	Conn[i]->port = new char[strlen(port)+1];
	strcpy(Conn[i]->port, port);

	Numconnections++;
	Target = i;
	FD_SET(Conn[i]->fd, &Mask);

	int temp = 1;
	ioctl(Conn[i]->fd, FIONBIO, &temp);

}

void open_connection(char *key)
{
	int n = strlen(key), k;

	for(site *sp = First_site; sp; sp = sp->next)
		if(!strncmp(key, sp->key, n)) {
			k = strlen(sp->key);
			if(isdigit(sp->key[k-1]))
				open_connection(sp->address, sp->port, 
					sp->key[k-1]-'0');
			else
				open_connection(sp->address, sp->port, 0);
			if(sp->init) write_parse_macro(sp->init);
			return;
		}

	fprintf(stderr, "There exists no site alias named %s.\n", key);
}

void load_macros(void)
{
	First_macro = NULL;

	char filename[256];
	strcpy(filename, ".muddle");

	FILE *fp;
	int i=0;
	fp = fopen(filename, "r");

	if(!fp) {
		sprintf(filename, "%s/.muddle", getenv("HOME"));
		fp = fopen(filename, "r");
	}

	if(fp) {
		int n;
		char str[512], *key, *str1, *str2, *str3;
		for(;;i++) {
			n = (int)fgets(str, 512, fp);
			if(!n) break;
			n = read_parse_macro(str, &key, &str1, &str2, &str3);
			if(n < 0) {
				i--;
				fprintf(stderr, 
				    "Parse error in macro file:\n  %s", str);
				continue;
			}
			else if(n == COMMENT)
				i--;
			else if(n == MACRO)
				First_macro = 
				    new macro(key, str1, First_macro);
			else if(n == SITE)
				First_site =
				    new site(key, str1, str2, str3,
					     First_site);
			else if(n == AUTON)
				First_auton =
				    new auton(key, str1, First_auton);
				
		}
		fclose(fp);
	}

	printf("%d macros and sites loaded.\n", i);
}

int read_parse_macro(char *str, char **key, char **str1, 
		     char **str2, char **str3)
{
	int i=0, k;
	char tempstr[512];


	int len = strlen(str);

	while(iswhite(str[i])) i++;

	if(str[i] == ';' || str[i] == '\n')
		return COMMENT;

	if(str[i] == 's' && str[i+1] == 'i' && str[i+2] == 't' &&
		str[i+3] == 'e') {
	
		i+=4;
		while(iswhite(str[i])) i++;
		if(str[i] != '=')
			return -1;

		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"')
			return -1;

		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}

		tempstr[k] = 0; i++;
		*key = new char[k+1];
		strcpy(*key, tempstr);

		while(iswhite(str[i])) i++;
		if(str[i] != 'a' || str[i+1] != 'd' || 
		    str[i+2] != 'd' || str[i+3] != 'r')
			return -1;
	
		i+=4;
		while(iswhite(str[i])) i++;
		if(str[i] != '=')
			return -1;
	
		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"')
			return -1;

		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}

		tempstr[k] = 0; i++;
		*str1 = new char[k+1];
		strcpy(*str1, tempstr);
	
		while(iswhite(str[i])) i++;
		if(str[i] != 'p' || str[i+1] != 'o' ||
		    str[i+2] != 'r' || str[i+3] != 't') {
			delete *str1; 
			return -1;
		}
	
		i+=4;
		while(iswhite(str[i])) i++;
		if(str[i] != '=') {
			delete *str1;
			return -1;
		}
	
		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"') {
			delete *str1;
			return -1;
		}

		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}

		tempstr[k] = 0; i++;
		*str2 = new char[k+1];
		strcpy(*str2, tempstr);

		*str3 = 0;
		while(iswhite(str[i])) i++;
		if(str[i] == 0 || str[i] == '\n') return SITE;

		if(str[i] != 'i' || str[i+1] != 'n' ||
		    str[i+2] != 'i' || str[i+3] != 't') {
			delete *str1; 
			delete *str2;
			return -1;
		}
	
		i+=4;
		while(iswhite(str[i])) i++;
		if(str[i] != '=') {
			delete *str1;
			delete *str2;
			return -1;
		}
	
		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"') {
			delete *str1;
			delete *str2;
			return -1;
		}

		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}

		tempstr[k] = 0; i++;
		*str3 = new char[k+1];
		strcpy(*str3, tempstr);

		return SITE;
	}
	
	else if(str[i] == 'i' && str[i+1] == 'n') {

		i+=2;
		while(iswhite(str[i])) i++;
		if(str[i] != '=')
			return -1;

		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"')
			return -1;

		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}
	
		tempstr[k] = 0; i++;
		*key = new char[k+1];
		strcpy(*key, tempstr);
	
		while(iswhite(str[i])) i++;
		if(str[i] != 'o' || str[i+1] != 'u' || str[i+2] != 't')
			return -1;
	
		i+=3;
		while(iswhite(str[i])) i++;
		if(str[i] != '=')
			return -1;
	
		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"')
			return -1;
	
		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}
	
		tempstr[k] = 0;
		*str1 = new char[k+1];
		strcpy(*str1, tempstr);

		return MACRO;
	}

	else if(str[i] == 'a' && str[i+1] == 'u' && str[i+2] == 't' &&
		str[i+3] == 'o') {

		i+=4;
		while(iswhite(str[i])) i++;
		if(str[i] != '=')
			return -1;

		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"')
			return -1;

		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}
	
		tempstr[k] = 0; i++;
		*key = new char[k+1];
		strcpy(*key, tempstr);
	
		while(iswhite(str[i])) i++;
		if(str[i] != 'o' || str[i+1] != 'u' || str[i+2] != 't')
			return -1;
	
		i+=3;
		while(iswhite(str[i])) i++;
		if(str[i] != '=')
			return -1;
	
		i++;
		while(iswhite(str[i])) i++;
		if(str[i] != '"')
			return -1;
	
		i++; k=0;
		while(str[i] != '"' || (str[i] == '"' && str[i+1] == '"')) {
			if(i >= len) return -1;
			tempstr[k++] = str[i++];
			if(str[i-1] == '"' && str[i] == '"') i++;
		}
	
		tempstr[k] = 0;
		*str1 = new char[k+1];
		strcpy(*str1, tempstr);

		return AUTON;
	}

	else return -1;
}

int write_parse_macro(char *str)
{
	char *wild[10];
	int len[10];
	for(macro *m=First_macro; m; m=m->next)
		if(m->match(str, wild, len)) {
			m->out(Target, wild, len);
			return 1;
		}
	return 0;
}

int iswhite(char ch)
{
	return(ch == ' ' || ch == 9);
}

int parse(char *str, char **array, int *len, int n)
{
	int i = 0, word = 0, last = 0;
	int l = strlen(str);

	while(i < l && word < n) {
		while(str[i] == ' ' && i < l) i++;
		last = i; array[word] = &str[i];
		while(str[i] != ' ' && i < l) i++;
		len[word] = i-last;
		word++;
	}

	return word;
}

void usleep(long usec)
{
	int j=0;
	timeval t;

	t.tv_sec = 0; t.tv_usec = usec;

	select(8, 0, 0, 0, &t);
}

void sigstop(int i)
{
	int temp = Target;
	Target = 0;
	kill(getpid(), SIGSTOP);
	Target = temp;
}

void sigint(int i)
{
	if(!Numconnections) exit(0);
	char str[80];
	int temp = Target;
	Target = 0;
	printf("\nReally quit? [y/n]: "); fflush(stdout);
	int n = (int)fgets(str, 80, stdin);
	if(n && str[0] == 'y') exit(0);
	Target = temp;
}
