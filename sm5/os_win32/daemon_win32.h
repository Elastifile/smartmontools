/*
 * os_win32/daemon_win32.h
 *
 * Home page of code is: http://smartmontools.sourceforge.net
 *
 * Copyright (C) 2004 Christian Franke <smartmontools-support@lists.sourceforge.net>
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
 */

#ifndef DAEMON_WIN32_H
#define DAEMON_WIN32_H

#define DAEMON_WIN32_H_CVSID "$Id: daemon_win32.h,v 1.1 2004/03/24 22:37:37 chrfranke Exp $\n"

#include <signal.h>

// Additional non-ANSI signals
#define SIGHUP  (NSIG+1)
#define SIGUSR1 (NSIG+2)
#define SIGUSR2 (NSIG+3)

// This function must be called from main()
int daemon_main(const char * ident, int argc, char **argv);

// Simulate signal()
void (*daemon_signal(int sig, void (*func)(int)))(int);
const char * daemon_strsignal(int sig);

// Simulate sleep()
void daemon_sleep(int seconds);

// Disable/Enable console
void daemon_disable_console(void);
int daemon_enable_console(const char * title);

// Detach from console
int daemon_detach(void);

#endif // DAEMON_WIN32_H