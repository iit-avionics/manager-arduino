/*----------------------------------------------------------------------------------*-
*  'error.h'
*  Holds the different system errors that could be used by AppLink
*  Part of the iit-avionics project
*  All rights reserve. MSU-IIT Iligan City
-*----------------------------------------------------------------------------------*/

#ifndef ERROR_H
#define ERROR_H

//SysVar Global Error Definition

//System Errors
#define ERR_NORMAL                  0x00
#define ERR_MEM_CRITICAL            0x01
#define ERR_ERROR                   0xFF

//Operation Errors
#define ERR_SYS_APPVAR_NOT_DEFINE   0x10
#define ERR_SYS_TOO_MANY_TASKS      0x11
#define ERR_SYS_CANT_DELETE_TASK    0x12
#define ERR_SYS_PCKT_INCONSISTENT   0x13
#define ERR_SYS_VAR_CANT_BE_READ    0x14
#define ERR_SYS_VAR_CANT_BE_WRITTEN 0x15

#endif
