/***********************************************************************
Copyright 2003-2006 Raza Microelectronics, Inc.(RMI). All rights
reserved.
Use of this software shall be governed in all respects by the terms and
conditions of the RMI software license agreement ("SLA") that was
accepted by the user as a condition to opening the attached files.
Without limiting the foregoing, use of this software in source and
binary code forms, with or without modification, and subject to all
other SLA terms and conditions, is permitted.
Any transfer or redistribution of the source code, with or without
modification, IS PROHIBITED,unless specifically allowed by the SLA.
Any transfer or redistribution of the binary code, with or without
modification, is permitted, provided that the following condition is
met:
Redistributions in binary form must reproduce the above copyright
notice, the SLA, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution:
THIS SOFTWARE IS PROVIDED BY Raza Microelectronics, Inc. `AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL RMI OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*****************************#RMI_3#***********************************/
#ifndef _PSB_H
#define _PSB_H

#include "rmi/types.h"

#include "rmi/shared_structs.h"
#include "rmi/shared_structs_func.h"

extern int register_shell_cmd(const char *name,
			      void (*func) (int argc, char *argv[]), const char *shorthelp);
extern void print_args(int argc, char *argv[]);
extern struct boot1_info *boot1_info;
extern void print_boot1_info(int argc, char *argv[]);
extern struct net_device *PhoenixMacNetDevice(void);

extern void *smp_malloc(size_t size);
extern void smp_free(void *ptr);
extern void *smp_calloc(size_t nmemb, size_t size);
extern void *smp_realloc(void *src, size_t size);

#endif