/*
 * smartctl.c
 *
 * Home page of code is: http://smartmontools.sourceforge.net
 *
 * Copyright (C) 2002 Bruce Allen <smartmontools-support@lists.sourceforge.net>
 * Copyright (C) 2000 Michael Cornwell <cornwell@acm.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * You should have received a copy of the GNU General Public License
 * (for example COPYING); if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This code was originally developed as a Senior Thesis by Michael Cornwell
 * at the Concurrent Systems Laboratory (now part of the Storage Systems
 * Research Center), Jack Baskin School of Engineering, University of
 * California, Santa Cruz. http://ssrc.soe.ucsc.edu/
 *
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdarg.h>
#include "smartctl.h"
#include "atacmds.h"
#include "ataprint.h"
#include "scsicmds.h"
#include "scsiprint.h"

extern const char *CVSid1, *CVSid2, *CVSid4, *CVSid5; 
const char* CVSid6="$Id: smartctl.c,v 1.21 2002/10/26 19:33:40 ballen4705 Exp $"
CVSID1 CVSID2 CVSID4 CVSID5 CVSID6;

unsigned char driveinfo               = FALSE;
unsigned char checksmart              = FALSE;
unsigned char smartvendorattrib       = FALSE;
unsigned char generalsmartvalues      = FALSE;
unsigned char smartselftestlog        = FALSE;
unsigned char smarterrorlog           = FALSE;
unsigned char smartdisable            = FALSE;
unsigned char smartenable             = FALSE;
unsigned char smartstatus             = FALSE;
unsigned char smartexeoffimmediate    = FALSE;
unsigned char smartshortselftest      = FALSE;
unsigned char smartextendselftest     = FALSE;
unsigned char smartshortcapselftest   = FALSE;
unsigned char smartextendcapselftest  = FALSE;
unsigned char smartselftestabort      = FALSE;
unsigned char smartautoofflineenable  = FALSE;
unsigned char smartautoofflinedisable = FALSE;
unsigned char smartautosaveenable     = FALSE;
unsigned char smartautosavedisable    = FALSE;
unsigned char printcopyleft           = FALSE;
unsigned char smart009minutes         = FALSE;
unsigned char quietmode               = FALSE;
unsigned char veryquietmode           = FALSE;
int           testcase                = -1;


void printslogan(){
  pout("smartctl version %d.%d-%d Copyright (C) 2002 Bruce Allen\n",RELEASE_MAJOR,RELEASE_MINOR,SMARTMONTOOLS_VERSION);
  pout("Home page is %s\n\n",PROJECTHOME);
  return;
}


void printcopy(){
  char out[CVSMAXLEN];
  pout("smartctl comes with ABSOLUTELY NO WARRANTY. This\n");
  pout("is free software, and you are welcome to redistribute it\n");
  pout("under the terms of the GNU General Public License Version 2.\n");
  pout("See http://www.gnu.org for further details.\n\n");
  pout("CVS version IDs of files used to build this code are:\n");
  printone(out,CVSid6);
  pout("%s",out);
  printone(out,CVSid1);
  pout("%s",out);
  printone(out,CVSid2);
  pout("%s",out);
  printone(out,CVSid4);
  pout("%s",out);
  printone(out,CVSid5);
  pout("%s",out);
  return;
}

/*  void prints help information for command syntax */
void Usage ( void){
  printf("Usage: smartctl -[options] [device]\n");
  printf("\nRead Only Options:\n");
  printf("  %c  Show version, copyright and license info\n", PRINTCOPYLEFT);
  printf("  %c  Show all SMART Information              (ATA/SCSI)\n",SMARTVERBOSEALL);
  printf("  %c  Show SMART Drive Info                   (ATA/SCSI)\n",DRIVEINFO);
  printf("  %c  Show SMART Status                       (ATA/SCSI)\n",CHECKSMART);
  printf("  %c  Show SMART General Attributes           (ATA Only)\n",    GENERALSMARTVALUES);
  printf("  %c  Show SMART Vendor Attributes            (ATA Only)\n",    SMARTVENDORATTRIB);
  printf("  %c  Show SMART Drive Error Log              (ATA Only\n",     SMARTERRORLOG);
  printf("  %c  Show SMART Drive Self Test Log          (ATA Only)\n",    SMARTSELFTESTLOG);
  printf("  %c  Quiet: only show SMART drive errors     (ATA Only)\n",    QUIETMODE);
  printf("  %c  Very Quiet: no display, use exit status (ATA Only)\n",    VERYQUIETMODE);
  printf("\nVendor-specific Display Options:\n");
  printf("  %c  Raw Attribute 009 is minutes            (ATA Only)\n",    SMART009MINUTES);
  printf("\nEnable/Disable Options:\n");
  printf("  %c  Enable  SMART data collection           (ATA/SCSI)\n",SMARTENABLE);
  printf("  %c  Disable SMART data collection           (ATA/SCSI)\n",SMARTDISABLE);
  printf("  %c  Enable  SMART Automatic Offline Test    (ATA Only)\n",    SMARTAUTOOFFLINEENABLE);
  printf("  %c  Disable SMART Automatic Offline Test    (ATA Only)\n",    SMARTAUTOOFFLINEDISABLE);
  printf("  %c  Enable  SMART Attribute Autosave        (ATA Only)\n",    SMARTAUTOSAVEENABLE);
  printf("  %c  Disable SMART Attribute Autosave        (ATA Only)\n",    SMARTAUTOSAVEDISABLE);
  printf("\nTest Options (no more than one):\n");
  printf("  %c  Execute Off-line data collection        (ATA Only)\n",    SMARTEXEOFFIMMEDIATE);
  printf("  %c  Execute Short Self Test                 (ATA Only)\n",    SMARTSHORTSELFTEST );
  printf("  %c  Execute Short Self Test (Captive Mode)  (ATA Only)\n",    SMARTSHORTCAPSELFTEST );
  printf("  %c  Execute Extended Self Test              (ATA Only)\n",    SMARTEXTENDSELFTEST );
  printf("  %c  Execute Extended Self Test (Captive)    (ATA Only)\n",    SMARTEXTENDCAPSELFTEST );
  printf("  %c  Execute Self Test Abort                 (ATA Only)\n",    SMARTSELFTESTABORT );
  printf("\nExamples:\n");
  printf("  smartctl -etf /dev/hda  (Enables SMART on first disk)\n");
  printf("  smartctl -a   /dev/hda  (Prints all SMART information)\n");
  printf("  smartctl -X   /dev/hda  (Executes extended disk self-test)\n");
  printf("  smartctl -qvL /dev/hda  (Prints Self-Test & Attribute errors.)\n");
}

const char opts[] = { 
  DRIVEINFO, CHECKSMART, SMARTVERBOSEALL, SMARTVENDORATTRIB,
  GENERALSMARTVALUES, SMARTERRORLOG, SMARTSELFTESTLOG, SMARTDISABLE,
  SMARTENABLE, SMARTAUTOOFFLINEENABLE, SMARTAUTOOFFLINEDISABLE,
  SMARTEXEOFFIMMEDIATE, SMARTSHORTSELFTEST, SMARTEXTENDSELFTEST, 
  SMARTSHORTCAPSELFTEST, SMARTEXTENDCAPSELFTEST, SMARTSELFTESTABORT,
  SMARTAUTOSAVEENABLE,SMARTAUTOSAVEDISABLE,PRINTCOPYLEFT,SMART009MINUTES,QUIETMODE,VERYQUIETMODE,'h','?','\0'
};

/*      Takes command options and sets features to be run */	
void ParseOpts (int argc, char** argv){
  int optchar;
  extern char *optarg;
  extern int optopt, optind, opterr;
  
  opterr=optopt=0;
  while (-1 != (optchar = getopt(argc, argv, opts))) {
    switch (optchar){
    case QUIETMODE:
      quietmode=TRUE;
      break;
    case VERYQUIETMODE:
      veryquietmode=TRUE;
      break;
    case SMART009MINUTES:
      smart009minutes=TRUE;
      break;
    case PRINTCOPYLEFT :
      printcopyleft=TRUE;
      break;
    case DRIVEINFO :
      driveinfo  = TRUE;
      break;		
    case CHECKSMART :
      checksmart = TRUE;		
      break;
    case SMARTVERBOSEALL :
      driveinfo = TRUE;
      checksmart = TRUE;
      generalsmartvalues = TRUE;
      smartvendorattrib = TRUE;
      smarterrorlog = TRUE;
      smartselftestlog = TRUE;
      break;
    case SMARTVENDORATTRIB :
      smartvendorattrib = TRUE;
      break;
    case GENERALSMARTVALUES :
      generalsmartvalues = TRUE;
      break;
    case SMARTERRORLOG :
      smarterrorlog = TRUE;
      break;
    case SMARTSELFTESTLOG :
      smartselftestlog = TRUE;
      break;
    case SMARTDISABLE :
      smartdisable = TRUE;
      break;
    case SMARTENABLE :
      smartenable   = TRUE;
      break;
    case SMARTAUTOSAVEENABLE:
      smartautosaveenable = TRUE;
      break;
    case SMARTAUTOSAVEDISABLE:
      smartautosavedisable = TRUE;
      break;
    case SMARTAUTOOFFLINEENABLE: 
      smartautoofflineenable = TRUE;
      break;
    case SMARTAUTOOFFLINEDISABLE:
      smartautoofflinedisable = TRUE;
      break;
    case SMARTEXEOFFIMMEDIATE:
      smartexeoffimmediate = TRUE;
      testcase=OFFLINE_FULL_SCAN;
      break;
    case SMARTSHORTSELFTEST :
      smartshortselftest = TRUE;
      testcase=SHORT_SELF_TEST;
      break;
    case SMARTEXTENDSELFTEST :
      smartextendselftest = TRUE;
      testcase=EXTEND_SELF_TEST;
      break;
    case SMARTSHORTCAPSELFTEST:
      smartshortcapselftest = TRUE;
      testcase=SHORT_CAPTIVE_SELF_TEST;
      break;
    case SMARTEXTENDCAPSELFTEST:
      smartextendcapselftest = TRUE;
      testcase=EXTEND_CAPTIVE_SELF_TEST;
      break;
    case SMARTSELFTESTABORT:
      smartselftestabort = TRUE;
      testcase=ABORT_SELF_TEST;
      break;
    case 'h':
    case '?':
    default:
      veryquietmode=FALSE;
      printslogan();
      if (optopt){
	pout("=======> UNRECOGNIZED OPTION: %c <=======\n\n",optopt);
	Usage();
	exit(FAILCMD);
      }
      Usage();
      exit(0);	
    }
  }
  // Do this here, so results are independent of argument order	
  if (quietmode)
    veryquietmode=TRUE;
  
  // error message if user has asked for more than one test
  if (1<(smartexeoffimmediate+smartshortselftest+smartextendselftest+
	 smartshortcapselftest+smartextendcapselftest+smartselftestabort)){
    veryquietmode=FALSE;
    printslogan();
    Usage();
    printf ("\nERROR: smartctl can only run a single test (or abort) at a time.\n\n");
    exit(FAILCMD);
  }

  // From here on, normal operations...
  printslogan();
  
  // Print Copyright/License info if needed
  if (printcopyleft){
    printcopy();
    if (argc==2)
      exit(0);
  }   
}


// Printing function (controlled by global veryquietmode)
void pout(char *fmt, ...){
  va_list ap;

  // initialize variable argument list 
  va_start(ap,fmt);
  if (veryquietmode){
    va_end(ap);
    return;
  }

  // print out
  vprintf(fmt,ap);
  va_end(ap);
  return;
}


/* Main Program */
int main (int argc, char **argv){
  int fd,retval=0;
  char *device;
  
  // Part input arguments
  ParseOpts(argc,argv);
    
  // Further argument checking
  if (argc != 3){
    Usage();
    return FAILCMD;
  }
  
  // open device - read-only mode is enough to issue needed commands
  fd = open(device=argv[2], O_RDONLY);
  
  if (fd<0) {
    perror("Smartctl device open failed");
    return FAILDEV;
  }
  
  if (device[5] == 'h')
    retval=ataPrintMain(fd);
  else if (device[5] == 's')
    scsiPrintMain (fd);
  else {
    Usage();
    return FAILCMD;
  }

  return retval;
}
