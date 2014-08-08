#include "includes.h"
#include "../../Xmodem/xmodem.h"

#define AUTO_UPDATE_FILE_NAME "auto_update_auto_update.bin"

#define CNTLQ       0x11
#define CNTLS       0x13
#define DEL         0x7F
#define BACKSPACE   0x08
#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

#define   GPIOB_BASE      0x40005000
#define   GPIOC_BASE      0x40006000
#define   GPIOBLOCK   (*((volatile unsigned long *)(GPIOB_BASE+0x520)))
#define   GPIOBCR     (*((volatile unsigned long *)(GPIOB_BASE+0x524)))
#define   GPIOBAFSEL  (*((volatile unsigned long *)(GPIOB_BASE+0x420))) 
#define   GPIOCLOCK   (*((volatile unsigned long *)(GPIOC_BASE+0x520)))
#define   GPIOCCR     (*((volatile unsigned long *)(GPIOC_BASE+0x524))) 
#define   GPIOCAFSEL  (*((volatile unsigned long *)(GPIOC_BASE+0x420))) 

/* Command definitions structure. */
typedef struct scmd {
  char val[8];
  void (*func)(char *par);
} SCMD;

/* External functions */
extern void init_serial (void);
extern int  getkey (void);

extern int CallApplication (int);
extern int unpend_getkey (void);
extern void serial_disable (void);

/* Command Functions */
static void cmd_format (char *par);
static void cmd_type (char *par);
static void cmd_dir (char *par);
static void cmd_run (char*par);
static void cmd_update (char *par);
static void cmd_backup (char*par);
static void cmd_lock (char*par);
static void cmd_unlock (char*par);
static void cmd_xmdr (char*par);

/* Local constants */
static const char intro[] = "\r\n\r\n+++++++++++++++++++++++++++++++\r\n          BOOTLOADER\r\n       MEILIN_2010.11.08\r\n+++++++++++++++++++++++++++++++\r\n";

static const SCMD cmd[] = {
	"format", cmd_format,
    "cat",   cmd_type,
    "ls",    cmd_dir,
    "app",   cmd_run,
    "update", cmd_update,
    "backup", cmd_backup,
    "lock", cmd_lock,
    "unlock", cmd_unlock,
	"xmdr", cmd_xmdr,
};

static int flag = 0;
static unsigned char  update_buf[1024];

#define CMD_COUNT   (sizeof (cmd) / sizeof (cmd[0]))

/* Local variables */
static char in_line[80];

/* Local Function Prototypes */
static void dot_format (U64 val, char *sp);
static char *get_entry (char *cp, char **pNext);
static void fs_init (void);

/*----------------------------------------------------------------------------
 *      Line Editor
 *---------------------------------------------------------------------------*/
BOOL getline (char *lp, U32 n) {
   U32 cnt = 0;
   int c;
   U32 timeout = 0;

   do {
      c = unpend_getkey ();
	  if (c == -1)
	  {
		 timeout++;
		 if (timeout == 100000000)
		 {
		 	CallApplication (0xc000);
		 }
	  }
	  else
	  {
	      timeout = 0;
		  switch (c) {
	         case CNTLQ:                       /* ignore Control S/Q             */
	         case CNTLS:
	            break;;
	         case BACKSPACE:
	         case DEL:
	            if (cnt == 0) {
	               break;
	            }
	            cnt--;                         /* decrement count                */
	            lp--;                          /* and line pointer               */
	            putchar (0x08);                /* echo backspace                 */
	            putchar (' ');
	            putchar (0x08);
	            fflush (stdout);
	            break;
	         case ESC:
	            *lp = 0;                       /* ESC - stop editing line        */
	            return (__FALSE);
	         case CR:                          /* CR - done, stop editing line   */
	            *lp = c;
	            lp++;                          /* increment line pointer         */
	            cnt++;                         /* and count                      */
	            c = LF;		
	         default:
	            putchar (*lp = c);             /* echo and store character       */
	            fflush (stdout);
	            lp++;                          /* increment line pointer         */
	            cnt++;                         /* and count                      */
	            break;
      	}
	  }
   } while (cnt < n - 2  &&  c != LF);     /* check limit and CR             */
   *lp = 0;                                /* mark end of string             */
   return (__TRUE);
}

/*----------------------------------------------------------------------------
 *        Process input string for an entry
 *---------------------------------------------------------------------------*/
static char *get_entry (char *cp, char **pNext) {
    char *sp;

    if (cp == NULL)
    {                          /* skip NULL pointers          */
        *pNext = cp;
        return(cp);
    }

    for ( ; *cp == ' '; cp++)
    {                /* skip blanks                 */
        *cp = 0;
    }

    for (sp = cp; *sp != ' ' && *sp != CR && *sp != LF; sp++)
    {
        //*sp = (char)toupper (*sp);              /* convert entry to uppercase  */
    }
    for ( ; *sp == ' ' || *sp == CR || *sp == LF; sp++)
    {
        *sp = 0;
    }

    *pNext = (*sp) ? sp : NULL;                /* next entry                  */
    return(cp);
}

/*----------------------------------------------------------------------------
 *        Print size in dotted fomat
 *---------------------------------------------------------------------------*/
static void dot_format (U64 val, char *sp) {

  if (val >= (U64)1e9) {
    sp += sprintf (sp,"%d.",(U32)(val/(U64)1e9));
    val %= (U64)1e9;
    sp += sprintf (sp,"%03d.",(U32)(val/(U64)1e6));
    val %= (U64)1e6;
    sprintf (sp,"%03d.%03d",(U32)(val/1000),(U32)(val%1000));
    return;
  }
  if (val >= (U64)1e6) {
    sp += sprintf (sp,"%d.",(U32)(val/(U64)1e6));
    val %= (U64)1e6;
    sprintf (sp,"%03d.%03d",(U32)(val/1000),(U32)(val%1000));
    return;
  }
  if (val >= 1000) {
    sprintf (sp,"%d.%03d",(U32)(val/1000),(U32)(val%1000));
    return;
  }
  sprintf (sp,"%d",(U32)(val));
}

/*----------------------------------------------------------------------------
 *        Read file and dump it to serial window
 *---------------------------------------------------------------------------*/
static void cmd_type (char *par) {
    char *fname,*next;
    FILE *f;
    int ch;

    fname = get_entry (par, &next);
    if (fname == NULL)
    {
        printf ("\nFilename missing.\n");
        return;
    }
    printf("\nRead data from file %s\n",fname);
    f = fopen (fname,"r");             /* open the file for reading           */
    if (f == NULL)
    {
        printf ("\nFile not found!\n");
        return;
    }

    while ((ch = fgetc (f)) != EOF)
    {  /* read the characters from the file   */
        putchar (ch);                   /* and write them on the screen        */
    }
    fclose (f);                        /* close the input file when done      */
    printf ("\nFile closed.\n");
}


/*----------------------------------------------------------------------------
 *        Print a Flash Memory Card Directory
 *---------------------------------------------------------------------------*/
static void cmd_dir (char *par) {
    U32 fsize,files;
    char temp[32],*mask,*next;
    FINFO info;

    mask = get_entry (par, &next);
    if (mask == NULL)
    {
        mask = "*.*";
    }

    printf ("\nFile System Directory...");
    files = 0;
    fsize = 0;
    info.fileID = 0;
    while (ffind (mask,&info) == 0)
    {
        dot_format (info.size, &temp[0]);
        printf ("\n%-20s %14s ", info.name, temp);
        printf ("  %02d.%02d.%04d  %02d:%02d  ID[%04d]",
                info.time.day, info.time.mon, info.time.year,
                info.time.hr, info.time.min, info.fileID);
        fsize += info.size;
        files++;
    }
    if (info.fileID == 0)
    {
        printf ("\nNo files...");
    }
    else
    {
        dot_format (fsize, &temp[0]);
        printf ("\n%9d File(s)    %14s bytes", files, temp);
    }
    dot_format (ffree(""), &temp[0]);
    printf ("\n%35s bytes free.\n",temp);
}

/*----------------------------------------------------------------------------
 *        Format a Flash Memory Card
 *---------------------------------------------------------------------------*/
static void cmd_format (char *par) {
  U32 retv;

  printf ("\nFormat SPI Flash Memory? [Y/N]\n");
  retv = getkey();
  if (retv == 'y' || retv == 'Y') {
    /* Format SPI Flash Memory. "*/
    if (fformat ("S:") == 0) {
      printf ("SPI Flash Memory Formatted.\n");
    }
    else {
      printf ("Formatting failed.\n");
    }
  }
}

/*----------------------------------------------------------------------------
 *        Main: 
 *---------------------------------------------------------------------------*/
static void cmd_run (char*par)
{
    CallApplication(0xC000);
}

void Tmr_TickISR_Handler (void)
{
    static unsigned char cnt = 15;

	io_reverse ("pd5", 0);

    if (cnt == 0)
    {
        SysTickIntDisable ();
        flag = 1;
    }

    cnt--;
}

static void cmd_update (char *par)
{
    char *fname,*next;
    FILE *f;
    int j = 0;

    fname = get_entry (par, &next);
    if (fname == NULL)
    {
        printf ("\nFilename missing.\n");
        return;
    }
    f = fopen (fname,"r");             /* open the file for reading           */
    if (f == NULL)
    {
        printf ("\nFile not found!\n");
        return;
    }

    printf("\nupdating"); 
    while ((fread (&update_buf, 1, 1024, f)) != 0)
    {
        IntMasterDisable();     
        FlashErase(j*0x400 + 0xc000);
        FlashProgram((unsigned long*)update_buf, j*0x400 + 0xc000, 1024);
        j++;
        printf(".");
        IntMasterEnable();
    }

    fclose (f); 
    /* close the input file when done      */
    printf ("\nupdate success!\n");
}

static void cmd_backup (char*par)
{
   char *fname, *next;
   FILE *f;
   int i,cnt = 100;

   fname = get_entry (par, &next);
   if (fname == NULL) {
      printf ("\r\nFilename missing.\r\n");
      return;
   }
   if (next) {
      par = get_entry (next, &next);
      if (sscanf (par,"%d", &cnt) == 0) {
         printf ("\r\nCommand error.\r\n");
         return;
      }
   }

   f = fopen (fname, "w");              /* open a file for writing           */
   if (f == NULL) {
      printf ("\r\nCan not open file!\r\n");/* error when trying to open file    */
      return;
   }
   printf ("\r\nbackuping"); 
   for (i = 0; i < cnt; i++)  {
	  fwrite ((void*)(0xc000 + i * 1024), 1, 1024, f);
	  printf ("."); 	
   }
   fclose (f);                        /* close the output file               */
   printf ("\r\nbackup success!\r\n");
}

static void jtag_lock (void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOBLOCK = 0x1ACCE551;
    GPIOBCR   = 0xFF;
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_DIR_MODE_IN);
    GPIOBLOCK  = 0x00;   
    GPIOCLOCK = 0x1ACCE551;
    GPIOCCR   = 0xFF;
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_DIR_MODE_OUT);
    GPIOCLOCK = 0x00;   

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_DIR_MODE_IN);    
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
    GPIOPinIntEnable(GPIO_PORTB_BASE, GPIO_PIN_5); 

    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_0, ~GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_0, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
}

static void cmd_lock (char*par)
{
	jtag_lock ();
}

static void cmd_unlock (char*par)
{
	jtag_lock ();

    GPIOBLOCK = 0x1ACCE551;
    GPIOBCR   = 0xFF;
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_7,GPIO_DIR_MODE_HW);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOBCR   = 0x7F;
    GPIOBLOCK = 0x00;

    GPIOCLOCK = 0x1ACCE551;
    GPIOCCR   = 0xFF;
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3,GPIO_DIR_MODE_HW);
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOCCR   = 0xF0;
    GPIOCLOCK = 0x00;
}

static void cmd_xmdr (char*par)
{
	char *fname,*next;

	fname = get_entry (par, &next);
    if (fname == NULL)
    {
        printf ("\nFilename missing.\n");
        return;
    }
	if (next) {
		printf ("\r\nCommand error.\r\n");
		return;
    }
   
	XmodemRun (fname);
}

static void fs_init (void) {
   U32 retv;

   retv = finit ();

   spi_hi_speed (__TRUE); 

   if (retv == 0) {
      retv = fcheck ("S:");
   }
   else
   {
   	  //格式化开始红灯亮
      io_write ("pd5", 0);

	  retv = fformat ("S:");

	  //格式化完成红灯灭
	  io_write ("pd5", 1);

      if (retv == 0)
      {
         //printf ("\r\nSPI Flash Memory Formatted.\r\n");
      }
      else
      {
         //printf ("\r\nFormatting failed.\r\n");
      }
   }
}

static void Tmr_TickInit (void)
{
    unsigned long  cnts;	

    cnts = (unsigned long)SysCtlClockGet() / 16;
    SysTickPeriodSet(cnts);
    SysTickEnable();
    SysTickIntEnable();
}

int main (void)
{
    char *sp,*next;
    U32 i;
    int j = 0;
    FILE* f;

	SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
	SysCtlLDOSet(SYSCTL_LDO_2_75V);	

//	jtag_lock ();
	
	init_serial ();

	io_init ("pd5", "out");
	io_init ("pd6", "out");

    fs_init (); 

    f = fopen (AUTO_UPDATE_FILE_NAME, "r");

    if (f != NULL)
    {	
		//升级开始绿灯亮
        io_write ("pd6", 0);

        while ((fread (&update_buf, 1, 1024, f)) != 0)
        {
            IntMasterDisable();     
            FlashErase(j*0x400 + 0xc000);
            FlashProgram((unsigned long*)update_buf, j*0x400 + 0xc000, 1024);
            j++;
            IntMasterEnable();
        }

        fclose (f); 
        fdelete (AUTO_UPDATE_FILE_NAME);

        //升级完成绿灯灭
		io_write ("pd6", 1);
    }

	Tmr_TickInit();			

    while (unpend_getkey () != CR)
    {
        if (flag == 1)
        {
			serial_disable ();//不关闭串口会有乱码出现
			CallApplication(0xC000);
        }
    }
    SysTickIntDisable ();
	io_write ("pd5", 1);
    printf (intro);
	printf ("\n");
    while (1)
    {
        printf ("BOOT\\> ");                     /* display prompt              */
        fflush (stdout);
        /* get command line input      */
        if (getline (in_line, sizeof (in_line)) == __FALSE)
        {
            continue;
        }
        sp = get_entry (&in_line[0], &next);
        if (*sp == 0)
        {
            continue;
        }

        for (i = 0; i < CMD_COUNT; i++)
        {
            if (strcmp (sp, (const char *)&cmd[i].val))
            {
                continue;
            }
            cmd[i].func (next);                  /* execute command function    */
			printf ("\n");
            break;
        }
        if (i == CMD_COUNT)
        {
            printf ("\nCommand error\n\n");
        }
    }
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
