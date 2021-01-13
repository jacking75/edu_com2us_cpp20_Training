
#include <signal.h>

#define MACRO	1
#define SITE	2
#define COMMENT	3
#define AUTON	4
#define DEFSIZE	4096

struct macro;
struct site;
struct conn;

void 	client(void);
void 	handle_command(char *);
void 	io_check(void);
void 	open_connection(char *, char *, char);
void 	open_connection(char *);
void 	load_macros(void);
int 	read_parse_macro(char *, char **, char **, char **, char **);
int 	write_parse_macro(char *);
int 	iswhite(char);
int 	parse(char *, char **, int *, int);
void 	usleep(long);

void sigstop(int);
void sigint(int);

struct macro {
	char *key;
	char *output;
	macro *next;

	macro(char *k, char *o, macro *n) { key = k; output = o; next = n; }
	~macro() { if(key) delete key; if(output) delete output; }
	int match(char *, char **, int *);
	void out(int, char **, int *);
};

struct auton {
	char *key;
	char *output;
	auton *next;

	auton(char *k, char *o, auton *n) { key = k; output = o; next = n; }
	~auton() { if(key) delete key; if(output) delete output; }
	int match(char *, int);
	void out(int, int);
};

struct site {
	char *key;
	char *address;
	char *port;
	char *init;
	site *next;

	site(char *k, char *a, char *p, char *i, site *n) { key = k; 
		address = a; port = p; init = i; next = n; }
	~site() { if(key) delete key; if(port) delete port; 
		if(address) delete address; if(init) delete init; }
};

struct conn {
	int	fd;
	int	slot;
	int	bufsize;
	char	*buf;
	int	head, tail;
	char	*addr;
	char	*port;

	conn(int i) { addr = 0; port = 0; bufsize = DEFSIZE;
		buf = new char[DEFSIZE]; fd = -1; head = tail = 0; slot = i;}
	~conn() { if(buf) delete buf; if(addr) delete addr;
		if(port) delete port; }
	void send(char *s) { write(fd, s, strlen(s)); }
	void output_buf(void);
	int update_buf(void);
	int check_command(char *);
};

