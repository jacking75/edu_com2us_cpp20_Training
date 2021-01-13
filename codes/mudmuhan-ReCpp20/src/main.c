/*
 * MAIN.C:
 *
 *	This files contains the main() function which initiates the
 *	game.
 *
 *	Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */

#include "mstruct.h"
#include "mextern.h"
#include <time.h>
#define SCHEDPORT  4000

int Port;
char report =0;

main(argc, argv)
int	argc;
char	*argv[];
{
	int i;
	FILE *fp;
	char file[80];
	static void mvc_log();
	static int schedule_g();

	Port = PORTNUM;

	for(i=0; i<PMAX; i++) Spy[i] = -1;
#ifdef SCHED
	if(argc == 1)
		Port = schedule_g();
#endif SCHED
	
#ifndef DEBUG
	if(fork()) exit(0);		/* go into background */
	/*close(0); close(1); close(2);	 close stdio */
#endif

	if (argc == 2)
 	    if(isnumber(argv[1]))
		Port = atoi(argv[1]);
	    else if (!strcmp(argv[1],"-r"))
		report = 1;

	if (argc == 3){
 	    if(isnumber(argv[1]))
		Port = atoi(argv[1]);
	      else if (!strcmp(argv[1],"-r"))
                report = 1;
 	    if(isnumber(argv[2]))
		Port = atoi(argv[2]);
	      else if (!strcmp(argv[2],"-r"))
                report = 1;
	}

#ifdef AUTOSHUTDOWN
	if (!Shutdown.interval){
	    Shutdown.ltime = time(0);
	    Shutdown.interval = 21600L;
	}
#endif


	umask(000);
	srand(getpid() + time(0));
	load_lockouts();
	load_family();
	
#ifndef DEBUG
 	sock_init(Port,0);
#else
 	sock_init(Port,1);
#endif

#ifdef RECORD_ALL
	mvc_log();
#endif RECORD_ALL
	{
	 long c;
	 c = time(0);
	logf("--- 머드 시작: %d --- (%.24s)\n", Port, ctime(&c));
    //log_overwrite("command.log","--- 재시작 ---");
	}

	sock_loop();
}

/*================================================================*/

#define DAYSEC 86400L
#define MORNT 27900L
#define MIDSEC(x) (((x->tm_hour)*3600)+((x->tm_min)*60)+(x->tm_sec))

/*================================================================*/
int schedule_g()
/* The schedule_g function keeps the game on a time  schedule */
{
    struct tm   *time_m;
    long	t = 0;
    int		mport = SCHEDPORT, port = PORTNUM; 
    enum {Sun, Mon, Tue, Wed, Thu, Fri, Sat};

    t = time(0);
    time_m = localtime(&t);
    t = 0;

    switch(time_m->tm_wday){
	case Sun:
 		t = DAYSEC - MIDSEC(time_m) + MORNT;
		break;
	case Mon: 
	case Tue: 
	case Wed: 
	case Thu:
		if (time_m->tm_hour >= 18)
			t = MORNT + DAYSEC - MIDSEC(time_m);
		else if (MIDSEC(time_m) < MORNT)
			t =  MORNT - MIDSEC(time_m);
		else{
			t = (18L*3600L) - MIDSEC(time_m);
			port =  SCHEDPORT;
			mport  = PORTNUM;
		}
		break;
	case Fri: 
		if (time_m->tm_hour >= 18)
			t = DAYSEC*3 - MIDSEC(time_m) + MORNT;
		else if (MIDSEC(time_m) < MORNT) 
			t =  MORNT - MIDSEC(time_m);
		else{
			t = (18L*3600L) - MIDSEC(time_m);
			port =  SCHEDPORT;
			mport = PORTNUM;
		}
		break;
	case Sat:
		t = DAYSEC*2 - MIDSEC(time_m) + MORNT;
		break;
		}

	if (!port)
		exit(0);


#ifdef AUTOSHUTDOWN
	t = MIN(21600L,t);
#endif
	Shutdown.ltime = time(0);
	Shutdown.interval =  t;
Port = mport;
	return (port);
}

/*================================================================*/
void mvc_log(){
/*  This function renames the list of all command file to include  *
 *  the date.  This is done to preserve the list commands that     *
 *  occured before a crash. */

	long t;
	char *date, mfile[128], rfile[128];
	int i;

	t = time(0);
	date = ctime(&t);

   for(i=0;i<24;i++)
	if(date[i] == ' ')
		date[i] = '_';
   date[24] = 0;
   sprintf(rfile,"%s/%s",LOGPATH,"all_cmd");
   sprintf(mfile,"%s/%s_%s",LOGPATH,"all_cmd",date);
	link(rfile,mfile);
	unlink(rfile);

}
