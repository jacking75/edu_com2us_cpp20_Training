/*
 * MTYPE.H:
 *
 *      #defines required by the rest of the program
 *
 *      Copyright (C) 1991, 1992, 1993 Brett J. Vickers
 *
 */
int Port;

/* socket port number */
#define PORTNUM         4000
 
/* DM's name */
#define DMNAME          "테스토스"
#define DMNAME2         "꽃"
#define DMNAME3         "뽀뽀"
#define DMNAME4         "" 
#define DMNAME5         ""
#define DMNAME6         ""
#define DMNAME7		    ""
 
/* I/O buffer sizes */
#define IBUFSIZE        1024
#define OBUFSIZE        8192   /* 원래 8192 */
 
/* File permissions */
/*#define S_IWRITE      00660 */
/*#define S_IREAD       00006 */
#define O_BINARY        0
#define ACC             00660
 
/* merror() error types */
#define FATAL           1
#define NONFATAL        0
 
/* pathnames */
#define MUDHOME			"/home/mordor"
#define ROOMPATH        MUDHOME"/rooms"
#define MONPATH         MUDHOME"/objmon"
#define OBJPATH         MUDHOME"/objmon"
#define PLAYERPATH     	MUDHOME"/player"
#define DOCPATH         MUDHOME"/help"
#define POSTPATH        MUDHOME"/post"
#define BINPATH         MUDHOME"/bin"
#define LOGPATH         MUDHOME"/log" 
        
#define COMMANDMAX      7 

/* Monster and object files sizes (in terms of monsters or objects) */
#define MFILESIZE       100
#define OFILESIZE       100

/* memory limits */
#define RMAX            9000    /* Max number of these allowed to be created */
#define CMAX            1000
#define OMAX            1000
#define PMAX            256

#define RQMAX           300     /* Max number of these allowed in memory */
#define CQMAX           100     /* at any one time                       */
#define OQMAX           100

/* how often (in seconds) players get saved */
#define SAVEINTERVAL    600 

/* Save flags */
#define PERMONLY        1
#define ALLITEMS        0

/* Command status returns */
#define DISCONNECT      1
#define PROMPT          2
#define DOPROMPT        3

/* Warring Kingdoms */
/* #define AT_WAR 1                         princes at war =1 / 0=peace */
#define REWARD 1000                     /* base value for pledge and rescind */
#define MAXALVL 128			/*max size of exp level array */

/* Creature stats */
#define STR             1
#define DEX             2
#define CON             3
#define INT             4
#define PTY             5

/* Character classes */
#define ZONEMAKER       0
#define ASSASSIN        1
#define BARBARIAN       2
#define CLERIC          3
#define FIGHTER         4
#define MAGE            5
#define PALADIN         6
#define RANGER          7
#define THIEF           8
#define INVINCIBLE      9
#define CARETAKER       10
#define SUB_DM		    11
#define DM              12 

/* Character races */
#define DWARF           1
#define ELF             2
#define HALFELF         3
#define HOBBIT          4
#define HUMAN           5
#define ORC             6
#define HALFGIANT       7
#define GNOME           8

/* creature types */
#define PLAYER          0
#define MONSTER         1
#define NPC             2

/* object types */
#define ARMOR           5
#define POTION          6
#define SCROLL          7
#define WAND            8
#define CONTAINER       9
#define MONEY           10
#define KEY             11
#define LIGHTSOURCE     12
#define MISC            13
#define CONTAINER2		14

/* Proficiencies */
#define SHARP           0
#define THRUST          1
#define BLUNT           2
#define POLE            3
#define MISSILE         4

/* Spell Realms */
#define EARTH           1
#define WIND            2
#define FIRE            3
#define WATER           4

/* Daily use variables */
#define DL_BROAD        0       /* Daily broadcasts */
#define DL_ENCHA        1       /* Daily enchants */
#define DL_FHEAL        2       /* Daily heals */
#define DL_TRACK        3       /* Daily track spells */
#define DL_DEFEC        4       /* Daily defecations */
#define DL_MARRI	8	/* 결혼식 번호 */
#define DL_EXPND 	9	/* 확장용(패거리에서 사용) */

#define MAXFAMILY	16	/* 최대 패거리의 숫자 */

/* Last-time specifications */
#define LT_INVIS        0
#define LT_PROTE        1
#define LT_BLESS        2
#define LT_ATTCK        3 
#define LT_TRACK        4
#define LT_MSCAV        4
#define LT_STEAL        5
#define LT_PICKL        6
#define LT_MWAND        6
#define LT_SERCH        7
#define LT_HEALS        8
#define LT_SPELL        9
#define LT_PEEKS        10
#define LT_PLYKL        11
#define LT_READS        12
#define LT_LIGHT        13
#define LT_HIDES        14
#define LT_TURNS        15
#define LT_HASTE        16
#define LT_DINVI        17
#define LT_DMAGI        18
#define LT_PRAYD        19
#define LT_PREPA        20
#define LT_LEVIT        21
#define LT_PSAVE        22
#define LT_RFIRE        23
#define LT_FLYSP        24
#define LT_RMAGI        25
#define LT_MOVED        26
#define LT_KNOWA        27
#define LT_HOURS        28
#define LT_RCOLD        29
#define LT_BRWAT        30
#define LT_SSHLD        31
#define LT_FEARS	33
#define LT_SILNC	34
#define LT_CHARM	35
#define LT_CHRMD	36
#define LT_POWER        37
#define LT_SLAYE        38
#define LT_MEDIT        39
#define LT_BEFUD        40
#define LT_SAYTLK       41 /* 말하는 몹의 lasttime */
#define LT_UPDMG	42
#define LT_KICK		43

/* Maximum number of items that can be worn/readied */
#define MAXWEAR         20

/* Wear locations */
#define BODY            1
#define ARMS            2
#define LEGS            3
#define NECK            4
#define NECK1           4
#define NECK2           5
#define HANDS           6
#define HEAD            7
#define FEET            8
#define FINGER          9
#define FINGER1         9
#define FINGER2         10
#define FINGER3         11
#define FINGER4         12
#define FINGER5         13
#define FINGER6         14
#define FINGER7         15
#define FINGER8         16
#define HELD            17
#define SHIELD          18
#define FACE            19
#define WIELD           20

/* Spell flags */
#define SVIGOR          0       /* 회복 */
#define SHURTS          1       /* 삭풍 */
#define SLIGHT          2       /* 발광 */
#define SCUREP          3       /* 해독 */
#define SBLESS          4       /* 성현진 */
#define SPROTE          5       /* 수호진 */
#define SFIREB          6       /* 화궁 */
#define SINVIS          7       /* 은둔법 */
#define SRESTO          8       /* 도력반 */
#define SDINVI          9       /* 은둔감지 */
#define SDMAGI          10      /* 주문감지 */
#define STELEP          11      /* 축지법 */
#define SBEFUD          12      /* 혼동 */
#define SLGHTN          13      /* 뇌전 */
#define SICEBL          14      /* 동설주 */
#define SENCHA          15      /* 빙의 */
#define SRECAL          16      /* 귀환 */
#define SSUMMO          17      /* 소환 */
#define SMENDW          18      /* 원기회복 */
#define SFHEAL          19      /* 완치 */
#define STRACK          20      /* 추적 */
#define SLEVIT          21      /* 부양술 */
#define SRFIRE          22      /* 방열진 */
#define SFLYSP          23      /* 비행 */
#define SRMAGI          24      /* 보마진 */
#define SSHOCK          25      /* 권풍술 */
#define SRUMBL          26      /* 지동술 */
#define SBURNS          27      /* 화선도 */
#define SBLIST          28      /* 탄수공 */
#define SDUSTG          29      /* 풍마현 */
#define SWBOLT          30      /* 파초식 */
#define SCRUSH          31      /* 폭진 */
#define SENGUL          32      /* 낙석 */
#define SBURST          33      /* 화풍술 */
#define SSTEAM          34      /* 화룡대천 */
#define SSHATT          35      /* 토합술 */
#define SIMMOL          36      /* 주작현 */
#define SBLOOD          37      /* 열사천 */
#define STHUND          38      /* 파천풍 */
#define SEQUAK          39      /* 지옥패 */
#define SFLFIL          40      /* 태양안 */
#define SKNOWA          41      /* 선악감지 */
#define SREMOV          42      /* 저주해소 */
#define SRCOLD          43      /* 방한진 */
#define SBRWAT          44      /* 수생술 */
#define SSSHLD          45      /* 지방호 */
#define SLOCAT          46      /* 천리안 */
#define SDREXP          47      /* 백치술 */
#define SRMDIS          48      /* 치료 */
#define SRMBLD          49      /* 개안술 */
#define SFEARS          50      /* 공포 */
#define SRVIGO		51	/* room vigor */
#define STRANO		52	/* item transport */
#define SBLIND		53	/* cause blindness */
#define SSILNC		54	/* cause silence */
#define SCHARM		55	/* charm person */
#define SCURSE			56	/* 저주 */
/* #define SNAHAN          56   */    /* 변수나한권 */
/* Trap types */
#define TRAP_PIT        1       /* Pit trap */
#define TRAP_DART       2       /* Poison dart trap */
#define TRAP_BLOCK      3       /* Falling block */
#define TRAP_MPDAM      4       /* Mp damaging trap */
#define TRAP_RMSPL      5       /* Spell loss trap */
#define TRAP_NAKED      6       /* player loses all items */
#define TRAP_ALARM      7       /* monster alarm trap */

/* Spell casting types */
#define CAST            0

/* Room flags */
#define RSHOPP          0       /* Shoppe */
#define RDUMPR          1       /* Dump */
#define RPAWNS          2       /* Pawn Shoppe */
#define RTRAIN          3       /* Training class bits (3-6) */
#define RREPAI          7       /* Repair Shoppe */
#define RDARKR          8       /* Room is dark always */
#define RDARKN          9       /* Room is dark at night */
#define RPOSTO          10      /* Post office */
#define RNOKIL          11      /* Safe room, no playerkilling */
#define RNOTEL          12      /* Cannot teleport to this room */
#define RHEALR          13      /* Heal faster in this room */
#define RONEPL          14      /* 1-player only inside */
#define RTWOPL          15      /* 2-players only inside */
#define RTHREE          16      /* 3-players only inside */
#define RNOMAG          17      /* No magic allowed in room */
#define RPTRAK          18      /* Permanent tracks in room */
#define REARTH          19      /* Earth realm */
#define RWINDR          20      /* Wind realm */
#define RFIRER          21      /* Fire realm */
#define RWATER          22      /* Water realm */
#define RPLWAN          23      /* Player-dependent monster wanders */
#define RPHARM          24      /* player harming room */
#define RPPOIS          25      /* player poison room */
#define RPMPDR          26      /* player mp drain room */
#define RPBEFU          27      /* player befuddle room */
#define RNOLEA          28      /* player can not be summon out */
#define RPLDGK          29      /* player can pledge in room */
#define RRSCND          30      /* player can rescind in room */
#define RNOPOT          31      /*  no potion room */
#define RPMEXT          32      /* Player magic spell extend */
#define RNOLOG          33      /* No player login */
#define RELECT          34      /* Election Booth */ 
#define RFORGE          35      /* 무기 제련소  */
#define RSUVIV		36	/* 대련장(서바이벌 존) */
#define RFAMIL		37	/* 패거리존 */
#define RONFML		38	/* Only Family */
#define RBANK		39		/* 은행 */
#define RONMAR		40	/* 결혼한 사람만 갈수 있음 */
#define RMARRI      41      /* 결혼식장 */

/* Player flags */
#define PBLESS          0       /* Bless spell */
#define PHIDDN          1       /* Hidden */
#define PINVIS          2       /* Invisibility */
#define PNOBRD          3       /* Don't show broadcasts */
#define PNOLDS          4       /* Don't show long description */
#define PNOSDS          5       /* Don't show short description */
#define PNORNM          6       /* Don't show room name */
#define PNOEXT          7       /* Don't show exits */
#define PPROTE          8       /* Protection spell */
#define PNOAAT          9       /* no auto attack for players */
#define PDMINV          10      /* DM Invisibility */
#define PNOCMP          11      /* Non-compact */
#define PMALES          12      /* Sex == male */
#define PHEXLN          13      /* Hexline */
#define PWIMPY          14      /* Wimpy mode */
#define PEAVES          15      /* Eavesdropping mode */
#define PPOISN          16      /* Poisoned */
#define PLIGHT          17      /* Light spell cast */
#define PPROMP          18      /* Display status prompt */
#define PHASTE          19      /* Haste flag (for rangers) */
#define PDMAGI          20      /* Detect magic */
#define PDINVI          21      /* Detect invisible */
#define PPRAYD          22      /* Prayer activated */
#define PROBOT          23      /* Robot mode */
#define PPREPA          24      /* Prepared for trap */
#define PLEVIT          25      /* Levitation */
#define PANSIC          26      /* Ansi Color */
#define PSPYON          27      /* Spying on someone */
#define PCHAOS          28      /* Chaotic/!Lawful */
#define PREADI          29      /* Reading a file */
#define PRFIRE          30      /* Resisting fire */
#define PFLYSP          31      /* Flying */
#define PRMAGI          32      /* Resist magic */
#define PKNOWA          33      /* Know alignment */
#define PNOSUM          34      /* Nosummon flag */
#define PIGNOR          35      /* Ignore all send */ 
#define PRCOLD          36      /* Resist-cold flag */
#define PBRWAT          37      /* Breathe wateR flag */
#define PSSHLD          38      /* Resist-cold flag */
#define PPLDGK          39      /* player pledge to a prince */
#define PRDDIV          40      /* 이혼 신청. */
#define PDISEA          41      /* Player is diseased */
#define PBLIND          42      /* Player is blind */
#define PFEARS		43	/* Player is fearful */
#define	PSILNC		44	/* Player has been silenced */
#define PCHARM		45	/* Player is charmed */
#define PLECHO		46	/* Local Echo mode enable */
#define PKNDM1          47      /* kingdome 1 */
#define PKNDM2          48      /* kingdome 2 */
#define PBRSND			49      /* 잡담 횟수 제한자. */
#define PNOBR2			50      /* 경매 안듣기 */
#define PBRIGH          51      /* 밝은색 사용 */
#define PPOWER          52      /* 기공집결 power +3 */
#define PSLAYE          53      /* 살기충전 thac0 +3 */
#define PMEDIT          54      /* 참선 intelligence +3 */
#define PFAMIL		55	/* 패거리 가입자 */
#define PRDFML		56	/* 패거리 가입 대기자 */
#define PFMBOS		57 	/* 패거리의 두목 */
#define PFRTUN		58 	/* 패거리귀환 설정 */
#define PUPDMG		59	/* 잠력격발 사용 */	
#define PMARRI			60		/* 결혼한 사람 */
#define PRDMAR			61		/* 결혼 신청 */
#define SUICD       62  /* 자살신청 */
#define PDSCRP			63		/* 묘사보기 설정 */

/* Monster flags */
#define MPERMT          0       /* Permanent monster */
#define MHIDDN          1       /* Hidden */
#define MINVIS          2       /* Invisible */
#define MTOMEN          3       /* Man to men on plural */
#define MDROPS          4       /* Don't add s on plural */
#define MNOPRE          5       /* No prefix */
#define MAGGRE          6       /* Aggressive */
#define MGUARD          7       /* Guards treasure */
#define MBLOCK          8       /* Blocks exits */
#define MFOLLO          9       /* Monster follows attacker */
#define MFLEER          10      /* Monster flees */
#define MSCAVE          11      /* Monster is a scavenger */
#define MMALES          12      /* Sex == male */
#define MPOISS          13      /* Poisoner */
#define MUNDED          14      /* Undead */
#define MUNSTL          15      /* Cannot be stolen from */
#define MPOISN          16      /* Poisoned */
#define MMAGIC          17      /* Can cast spells */
#define MHASSC          18      /* Has already scavenged something */
#define MBRETH          19      /* Breath weapon */
#define MMGONL          20      /* Can only be harmed by magic */
#define MDINVI          21      /* Detect invisibility */
#define MENONL          22      /* Can only be harmed by magic/ench.weapon */
#define MTALKS          23      /* Monster can talk interactively */
#define MUNKIL          24      /* Monster cannot be harmed */
#define MNRGLD          25      /* Monster has fixed amt of gold */
#define MTLKAG          26      /* Becomes aggressive after talking */
#define MRMAGI          27      /* Resist magic */
#define MBRWP1          28      /* MBRWP1 & MBRWP2 type of breath */
#define MBRWP2          29      /* 00 =fire, 01= ice, 10 =gas, 11= acid */
#define MENEDR          30      /* energy (exp) drain */
#define MKNGDM          31      /* monster belongs to kingdom 0/1 */
#define MPLDGK          32      /* players can pledge to monster */
#define MRSCND          33      /* players can rescind to monster */
#define MDISEA          34      /* Monster causes disease */
#define MDISIT          35      /* Monster can dissolve items */
#define MPURIT          36      /* player can purchase from monster */
#define MTRADE          37      /* monster will giuve items */
#define MPGUAR          38      /* passive exit guard */
#define MGAGGR          39      /* Monster aggro to good players */
#define MEAGGR          40      /* Monster aggro to evil players */
#define MDEATH          41      /* Monster has additon desc after death */
#define MMAGIO		42	/* Monster cast magic percent flag (prof 1) */
#define MRBEFD		43	/* Monster resists stun only */
#define MNOCIR		44	/* Monster cannot be circled */
#define MBLNDR		45	/* Monster blinds */
#define MDMFOL		46	/* Monster will follow DM */
#define MFEARS		47	/* Monster is fearful */
#define MSILNC		48	/* Monster is silenced */
#define MBLIND		49	/* Monster is blind */
#define MCHARM		50	/* Monster is charmed */
#define MBEFUD		51	/* 혼동되있음 */
#define MKNDM1           52           /* kingdom 1 */
#define MKNDM2           53           /* kingdome 2 */
#define MKNDM3           54           /* kingdome 3 */
#define MKNDM4           55           /* dingdome 4 */
#define MKING1		56		/* 왕 1 */
#define MKING2		57		/* 왕 2 */
#define MKING3		58		/* 왕 3 */
#define MKING4		59		/* 왕 4 */
#define MSAYTLK               60              /* 말하는 몹 */
#define MSUMMO          61              /* 몹을 소환함 */
#define MNOCHA		62	/* Monster cannot be charmed */

/* Object flags */
#define OPERMT          0       /* Permanent item (not yet taken) */
#define OHIDDN          1       /* Hidden */
#define OINVIS          2       /* Invisible */
#define OSOMEA          3       /* "some" prefix */
#define ODROPS          4       /* Don't add s on plural */
#define ONOPRE          5       /* No prefix */
#define OCONTN          6       /* Container object */
#define OWTLES          7       /* Container of weightless holding */
#define OTEMPP          8       /* Temporarily permanent */
#define OPERM2          9       /* Permanent INVENTORY item */
#define ONOMAG          10      /* Mages cannot wear/use it */
#define OLIGHT          11      /* Object serves as a light */
#define OGOODO          12      /* Usable only by good players */
#define OEVILO          13      /* Usable only by evil players */
#define OENCHA          14      /* Object enchanted */
#define ONOFIX          15      /* Cannot be repaired */
#define OCLIMB          16      /* Climbing gear */
#define ONOTAK          17      /* Cannot be taken */
#define OSCENE          18      /* Part of room description/scenery */
#define OSIZE1          19      /* OSIZE: 00=all wear, 01=small wear, */
#define OSIZE2          20      /* 10=medium wear, 11=large wear */
#define ORENCH          21      /* Random enchantment flag */
#define OCURSE          22      /* The item is cursed */
#define OWEARS          23      /* The item is being worn */
#define OUSEFL          24      /* Can be used from the floor */
#define OCNDES          25      /* Container devours items */
#define ONOMAL          26      /* Usable by only females */
#define ONOFEM          27      /* Usable by only males */
#define ODDICE          28      /* damage based on object nds */
#define OPLDGK          29      /* pledge players may only use */
#define OKNGDM          30      /* object belongs to kingdom 0/1 */
#define OCLSEL          31      /* class selective weapon */
#define OASSNO          32      /* class selective: assassin */
#define OBARBO          33      /* class selective: barbarian */
#define OCLERO          34      /* class selective: cleric */
#define OFIGHO          35      /* class selective: fighter */
#define OMAGEO          36      /* class selective: mage */
#define OPALAO          37      /* class selective: paladin */
#define ORNGRO          38      /* class selective: ranger */
#define OTHIEO          39      /* class selective: thief */
#define OVBEFD		40	/* stun length based on ndice */
#define ONSHAT		41	/* weapon will never shatter */
#define OALCRT		42	/* weapon will always critical */
#define OCNAME          43      /* 물건 이름 바꿀수 있음 */
#define OSPECI		44	/* special object (pdice 이용) */
#define OMARRI		45	/* 결혼한 사람만 사용가능 */
#define OEVENT		46	/* 이벤트용 아이템 */
#define ONAMED      47  /* 명명한거 */
#define ONOBUN		48	/* can not burn */
#define OWHELD		49	/* 쥐고 있다는 플래그 . 외부 사용안함. */
#define ONEWEV      50  /* 새로운 이벤트 플래그 */

/* Exit flags */
#define XSECRT          0       /* Secret */
#define XINVIS          1       /* Invisible */
#define XLOCKD          2       /* Locked */
#define XCLOSD          3       /* Closed */
#define XLOCKS          4       /* Lockable */
#define XCLOSS          5       /* Closable */
#define XUNPCK          6       /* Un-pickable lock */
#define XNAKED          7       /* Naked exit */
#define XCLIMB          8       /* Climbing gear required to go up */
#define XREPEL          9       /* Climbing gear require to repel */
#define XDCLIM          10      /* Very difficult climb */
#define XFLYSP          11      /* Must fly to go that way */
#define XFEMAL          12      /* female only exit */
#define XMALES          13      /* male only exit */
#define XPLDGK          14      /* pledge player exit only */
#define XKNGDM          15      /* exit for kingdom 0/1 */
#define XNGHTO          16      /* only open at night */
#define XDAYON          17      /* only open during day */
#define XPGUAR          18      /* passive guarded exit */
#define XNOSEE          19      /* Can not use / see exit */
#define XKNDM1          20      /* 왕국 1번 길 */
#define XKNDM2          21      /* 왕국 2번 길 */

/* specials */
#define SP_MAPSC        1       /* Map or scroll */
#define SP_COMBO        2       /* Combination lock */
#define SP_WAR          3       /* 전쟁 아이템 */
#define SP_BOARD	4       /* 게시판 */

/* obj_str and crt_str flags */
#define CAP             1
#define INV             2
#define MAG             4
#define DMF             8

#define RETURN(a,b,c)   Ply[a].io->fn = b; Ply[a].io->fnparam = c; return;

#define F_ISSET(p,f)    ((p)->flags[(f)/8] & (1<<((f)%8)))
#define F_SET(p,f)      ((p)->flags[(f)/8] |= 1<<((f)%8))
#define F_CLR(p,f)      ((p)->flags[(f)/8] &= ~(1<<((f)%8)))

#define S_ISSET(p,f)    ((p)->spells[(f)/8] & 1<<((f)%8))
#define S_SET(p,f)      ((p)->spells[(f)/8] |= 1<<((f)%8))
#define S_CLR(p,f)      ((p)->spells[(f)/8] &= ~(1<<((f)%8)))

#define Q_ISSET(p,f)    ((p)->quests[(f)/8] & 1<<((f)%8))
#define Q_SET(p,f)      ((p)->quests[(f)/8] |= 1<<((f)%8))
#define Q_CLR(p,f)      ((p)->quests[(f)/8] &= ~(1<<((f)%8)))

#define BOOL(a)         ((a) ? 1 : 0)
#define EQUAL(a,b)      ((a) && (b) && \
                         (!strncmp((a)->name,(b),strlen(b)) || \
                          !strncmp((a)->key[0],(b),strlen(b)) || \
                          !strncmp((a)->key[1],(b),strlen(b)) || \
                          !strncmp((a)->key[2],(b),strlen(b))))       

#define LT(a,b)         ((a)->lasttime[(b)].ltime + (a)->lasttime[(b)].interval)

#define mrand(a,b)      (((a)>(b)) ? (a) : ((a)+(rand()%((b)*10-(a)*10+10))/10))
#define mdice(a)        (dice((a)->ndice,(a)->sdice,(a)->pdice))
#define MIN(a,b)        (((a)<(b)) ? (a):(b))
#define MAX(a,b)        (((a)>(b)) ? (a):(b))

 
#define BLACK           30
#define RED             31
#define GREEN           32
#define YELLOW          33
#define BLUE            34
#define MAGENTA         35
#define CYAN            36
#define WHITE           37
#define BOLD            1
#define BLINK           5
#define NORMAL          0  
#define ANSI(a,b)       if(F_ISSET(Ply[(a)].ply, PANSIC)) \
                        print((a), "%c[%d;%dm", 27, (F_ISSET(Ply[(a)].ply,PBRIGH)&&(b)!=WHITE)?1:0,(b));

