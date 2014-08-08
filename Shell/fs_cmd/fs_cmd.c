#include "includes.h"
#include "fs_cmd.h" 

#define CNTLQ       0x11
#define CNTLS       0x13
#define DEL         0x7F
#define BACKSPACE   0x08
#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

/* Command Functions */
static void cmd_capture (char *par);
static void cmd_type (char *par);
static void cmd_rename (char *par);
static void cmd_copy (char *par);
static void cmd_delete (char *par);
static void cmd_dir (char *par);
static void cmd_format (char *par);
static void cmd_fill (char *par);

/* Local Function Prototypes */
static void dot_format (U32 val, char *sp);
static char *get_entry (char *cp, char **pNext);
static void init_file (void);

static char getkey (void)
{
    INT8U key;
    
    shell_read (&key,1);
    
    return key;   
}

static char in_line[80];

/*----------------------------------------------------------------------------
 *        Process input string for an entry
 *---------------------------------------------------------------------------*/
static char *get_entry (char *cp, char **pNext) {
   char *sp;

   if (cp == NULL) {                          /* skip NULL pointers          */
      *pNext = cp;
      return (cp);
   }

   for ( ; *cp == ' '; cp++) {                /* skip blanks                 */
      *cp = 0;
   }

   for (sp = cp; *sp != ' ' && *sp != CR && *sp != LF; sp++) {
   //   *sp = (char)toupper (*sp);              /* convert entry to uppercase  */
   }

   for ( ; *sp == ' ' || *sp == CR || *sp == LF; sp++) {
      *sp = 0;
   }

   *pNext = (*sp) ? sp : NULL;                /* next entry                  */
   return (cp);
}

/*----------------------------------------------------------------------------
 *        Print size in dotted fomat
 *---------------------------------------------------------------------------*/
static void dot_format (U32 val, char *sp) {

   if (val >= (U32)1e9) {
      sp += sprintf (sp,"%d.",val/(U32)1e9);
      val %= (U32)1e9;
      sp += sprintf (sp,"%03d.",val/(U32)1e6);
      val %= (U32)1e6;
      sprintf (sp,"%03d.%03d",val/1000,val%1000);
      return;
   }
   if (val >= (U32)1e6) {
      sp += sprintf (sp,"%d.",val/(U32)1e6);
      val %= (U32)1e6;
      sprintf (sp,"%03d.%03d",val/1000,val%1000);
      return;
   }
   if (val >= 1000) {
      sprintf (sp,"%d.%03d",val/1000,val%1000);
      return;
   }
   sprintf (sp,"%d",val);
}

/*----------------------------------------------------------------------------
 *        Capture serial data to file
 *---------------------------------------------------------------------------*/
static void cmd_capture (char *par) {
   char *fname,*next;
   BOOL append,retv;
   FILE *f;

   fname = get_entry (par, &next);
   if (fname == NULL) {
      shell_printf ("\r\nFilename missing.\r\n");
      return;
   }
   append = __FALSE;
   if (next) {
      par = get_entry (next, &next);
      if (strcmp (par, "/A") == 0) {
         append = __TRUE;
      }
      else {
         shell_printf ("\r\nCommand error.\r\n");
         return;
      }
   }
   shell_printf ((append) ? "\r\nAppend data to file %s" :
                      "\r\nCapture data to file %s", fname);
   shell_printf("\r\nPress ESC to stop.\r\n");
   f = fopen (fname,append ? "a" : "w");/* open a file for writing           */
   if (f == NULL) {
      shell_printf ("\r\nCan not open file!\r\n");/* error when trying to open file    */
      return;
   } 
   do {
      retv = shell_read ((INT8U*)in_line, sizeof (in_line));
      fputs (in_line, f);
   } while (retv == __TRUE);
   fclose (f);                        /* close the output file               */
   shell_printf ("\r\nFile closed.\r\n");
}

/*----------------------------------------------------------------------------
 *        Create a file and fill it with some text
 *---------------------------------------------------------------------------*/
static void cmd_fill (char *par) {
   char *fname, *next;
   FILE *f;
   int i,cnt = 1000;

   fname = get_entry (par, &next);
   if (fname == NULL) {
      shell_printf ("\r\nFilename missing.\r\n");
      return;
   }
   if (next) {
      par = get_entry (next, &next);
      if (sscanf (par,"%d", &cnt) == 0) {
         shell_printf ("\r\nCommand error.\r\n");
         return;
      }
   }

   f = fopen (fname, "w");              /* open a file for writing           */
   if (f == NULL) {
      shell_printf ("\r\nCan not open file!\r\n");/* error when trying to open file    */
      return;
   } 
   for (i = 0; i < cnt; i++)  {
      fprintf (f, "This is line # %d in file %s\r\n", i, fname);
   }
   fclose (f);                        /* close the output file               */
   shell_printf ("\r\nFile closed.\r\n");
}


/*----------------------------------------------------------------------------
 *        Read file and dump it to serial window
 *---------------------------------------------------------------------------*/
static void cmd_type (char *par) {
   char *fname,*next;
   FILE *f;
   int ch;

   fname = get_entry (par, &next);
   if (fname == NULL) {
      shell_printf ("\r\nFilename missing.\r\n");
      return;
   }
   shell_printf("\r\nRead data from file %s\r\n",fname);
   f = fopen (fname,"r");             /* open the file for reading           */
   if (f == NULL) {
      shell_printf ("\r\nFile not found!\r\n");
      return;
   }
  
   while ((ch = fgetc (f)) != EOF) {  /* read the characters from the file   */
      shell_putchar (ch);                   /* and write them on the screen        */
   }
   fclose (f);                        /* close the input file when done      */
   shell_printf ("\r\nFile closed.\r\n");
}

/*----------------------------------------------------------------------------
 *        Rename a File
 *---------------------------------------------------------------------------*/
static void cmd_rename (char *par) {
   char *fname,*fnew,*next;

   fname = get_entry (par, &next);
   if (fname == NULL) {
      shell_printf ("\r\nFilename missing.\r\n");
      return;
   }
   fnew = get_entry (next, &next);
   if (fnew == NULL) {
      shell_printf ("\r\nNew Filename missing.\r\n");
      return;
   }
   if (strcmp (fname,fnew) == 0) {
      shell_printf ("\r\nNew name is the same.\r\n");
      return;
   }
   if (frename (fname, fnew) == 0) {
      shell_printf ("\r\nFile %s renamed to %s\r\n",fname,fnew);
   }
   else {
      shell_printf ("\r\nFile rename error.\r\n");
   }
}

/*----------------------------------------------------------------------------
 *        Copy a File
 *---------------------------------------------------------------------------*/
static void cmd_copy (char *par) {
   char *fname,*fnew,*fmer,*next;
   FILE *fin,*fout;
   U32 cnt,total;
   static char buf[512];
   BOOL merge;

   fname = get_entry (par, &next);
   if (fname == NULL) {
      shell_printf ("\r\nFilename missing.\r\n");
      return;
   }
   fmer = get_entry (next, &next);
   if (fmer == NULL) {
      shell_printf ("\r\nNew Filename missing.\r\n");
      return;
   }
   fnew = get_entry (next, &next);
   if (fnew != NULL) {
      merge = __TRUE;
   }
   else {
      merge = __FALSE;
      fnew = fmer;
   }
   if ((strcmp (fname,fnew) == 0)        ||
       (merge && strcmp (fmer,fnew) == 0)) {
      shell_printf ("\r\nNew name is the same.\r\n");
      return;
   }

   fin = fopen (fname,"r");           /* open the file for reading           */
   if (fin == NULL) {
      shell_printf ("\r\nFile %s not found!\r\n",fname);
      return;
   }

   if (merge == __FALSE) {
      shell_printf ("\r\nCopy file %s to %s\r\n",fname,fnew);
   }
   else {
      shell_printf ("\r\nCopy file %s, %s to %s\r\n",fname,fmer,fnew);
   }
   fout = fopen (fnew,"w");           /* open the file for writing           */
   if (fout == NULL) {
      shell_printf ("\r\nFailed to open %s for writing!\r\n",fnew);
      fclose (fin);
      return;
   }

   total = 0;
   while ((cnt = fread (&buf, 1, 512, fin)) != 0) {
      fwrite (&buf, 1, cnt, fout);
      total += cnt;
   }
   fclose (fin);                      /* close input file when done          */

   if (merge == __TRUE) {
      fin = fopen (fmer,"r");         /* open the file for reading           */
      if (fin == NULL) {
         shell_printf ("\r\nFile %s not found!\r\n",fmer);
      }
      else {
         while ((cnt = fread (&buf, 1, 512, fin)) != 0) {
            fwrite (&buf, 1, cnt, fout);
            total += cnt;
         }
         fclose (fin);
      }
   }
   fclose (fout);
   dot_format (total, &buf[0]);
   shell_printf ("\r\n%s bytes copied.\r\n", &buf[0]);
}

/*----------------------------------------------------------------------------
 *        Delete a File
 *---------------------------------------------------------------------------*/
static void cmd_delete (char *par) {
   char *fname,*next;

   fname = get_entry (par, &next);
   if (fname == NULL) {
      shell_printf ("\r\nFilename missing.\r\n");
      return;
   }

   if (fdelete (fname) == 0) {
      shell_printf ("\r\nFile %s deleted.\r\n",fname);
   }
   else {
      shell_printf ("\r\nFile %s not found.\r\n",fname);
   }
}

/*----------------------------------------------------------------------------
 *        Print a Flash Memory Card Directory
 *---------------------------------------------------------------------------*/
static void cmd_dir (char *par) {
   U32 fsize,files;
   static char temp[32],*mask,*next;
   static FINFO info;

   mask = get_entry (par, &next);
   if (mask == NULL) {
      mask = "*.*";
   }

   shell_printf ("\r\nFile System Directory...");
   files = 0;
   fsize = 0;
   info.fileID = 0;
   while (ffind (mask,&info) == 0) {
      dot_format (info.size, &temp[0]);
      shell_printf ("\r\n%-20s %14s ", info.name, temp);
      shell_printf ("  %02d.%02d.%04d  %02d:%02d  ID[%04d]",
               info.time.day, info.time.mon, info.time.year,
               info.time.hr, info.time.min, info.fileID);
      fsize += info.size;
      files++;
   }
   if (info.fileID == 0) {
      shell_printf ("\r\nNo files...");
   }
   else {
      dot_format (fsize, &temp[0]);
      shell_printf ("\r\n%9d File(s)    %14s bytes", files, temp);
   }
   dot_format (ffree(""), &temp[0]);
   shell_printf ("\r\n%35s bytes free.\r\n",temp);
}

/*----------------------------------------------------------------------------
 *        Format a Flash Memory Card
 *---------------------------------------------------------------------------*/
static void cmd_format (char *par) {
   U32 retv;

   shell_printf ("\r\nFormat SPI Flash Memory? [Y/N]\r\n");
   retv = getkey();
   if (retv == 'y' || retv == 'Y') {
        shell_printf ("\r\nFormatting...");
      /* Format SPI Flash Memory. "*/

	  shell_enter_critical ();
	  retv = fformat ("S:");
	  shell_exit_critical ();

      if (retv == 0) {
         shell_printf ("\r\nSPI Flash Memory Formatted.\r\n");
      }
      else {
         shell_printf ("\r\nFormatting failed.\r\n");
      }
	  
   }
}

/*----------------------------------------------------------------------------
 *        Initialize a Flash File System
 *---------------------------------------------------------------------------*/
static void init_file (void) {
   U32 retv;

   retv = finit ();

   spi_hi_speed (__TRUE); 

   if (retv == 0) {
      retv = fcheck ("S:");
   }
   if (retv != 0) {
      shell_printf ("\r\nFlash File System needs Formatting.");
      in_line[0] = 0;
      cmd_format (&in_line[0]);
   }
}

static void fsinit (void* pdat)
{
   init_file ();
   shell_return ();    
}

static char buf[100];

static void ls (void* pdat)
{
	INT8U argc;
	char** argv;
	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));

	if (argc == 1)
    {
     	cmd_dir (0);
        shell_return ();  
    }
	
	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}		
 	cmd_dir (buf);

    shell_return ();    
}

static void cat (void* pdat)
{
	INT8U argc;
	char** argv;
	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));

	if (argc == 1)
    {
     	cmd_type (0);
        shell_return ();  
    }
	
	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}

	shell_enter_critical ();
	cmd_type (buf);
	shell_exit_critical ();
	 
    shell_return ();  
}

static void del (void* pdat)
{
	INT8U argc;
	char** argv;
	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));

	if (argc == 1)
    {
     	cmd_delete (0);
        shell_return ();  
    }

	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}		
 	cmd_delete (buf);
 
    shell_return ();  
}

static void cp (void* pdat)
{
	INT8U argc;
	char** argv;

	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));

	if (argc == 1)
    {
     	cmd_copy (0);
        shell_return ();  
    }
	
	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}		
 	cmd_copy (buf);
 
    shell_return ();  
}

static void ren (void* pdat)
{
	INT8U argc;
	char** argv;
	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));

	if (argc == 1)
    {
     	cmd_rename (0);
        shell_return ();  
    }
	
	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}		
 	cmd_rename (buf);
 
    shell_return ();  
}

static void format (void* pdat)
{
	INT8U argc;
	char** argv;
	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));

	if (argc == 1)
    {
     	cmd_format (0);
        shell_return ();  
    }
	
	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}		
 	cmd_format (buf); 
	 
    shell_return ();  
}

static void fill (void* pdat)
{
	INT8U argc;
	char** argv;
	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));
    
    if (argc == 1)
    {
     	cmd_fill (0);
        shell_return ();  
    }

	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}		
 	cmd_fill (buf);
 
    shell_return ();  
}

static void cap (void* pdat)
{
	INT8U argc;
	char** argv;
	INT8U i;	

	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv; 
	
	memset (buf, 0, sizeof (buf));

    if (argc == 1)
    {
     	cmd_capture (0);
        shell_return ();  
    }
	
	for (i = 1; i < argc; i++)
	{
	    strcat (buf, argv[i]); 
	    strcat (buf, " ");  
	}		
 	cmd_capture (buf);
 
    shell_return ();  
}

void fs_cmd_register (void)
{
    shell_cmd_register ("ls", ls, "list files");
    shell_cmd_register ("cat", cat, "view file");
    shell_cmd_register ("del", del, "delete file");		
    shell_cmd_register ("cp", cp, "copy file");
	shell_cmd_register ("ren", ren, "rename file");
	shell_cmd_register ("fill", fill, "fill file");
	shell_cmd_register ("cap", cap, "captrue user input to a file");
	shell_cmd_register ("format", format, "format file system");
    shell_cmd_register ("fsinit", fsinit, "initial file system");
}
