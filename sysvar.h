/*----------------------------------------------------------------------------------*-
*  'sysvar.h'
*  SYSVAR library header for APPLINK PROTOCOL
*  Part of the iit-avionics project
*  All rights reserve. MSU-IIT Iligan City
-*----------------------------------------------------------------------------------*/

#ifndef SYSVAR_H
#define SYSVAR_H

#include<Arduino.h>
#include <appvar.h>
#include "error.h"

#define MAX_SV_ELEMENTS 5

//message id's
#define SYS_STATE_G  0x00
#define TIME_G       0x01
#define ERROR_G      0x02
#define MEM_G        0x03
#define RET_ERR_G    0x04


class SysVar{
  public:
    SysVar(void);
    AppVar Get(const byte index);
    AppVar *GetBulk(const byte *indices,const byte sz);
    //byte Set(const AppVar v);
    byte Set(const byte id,byte* data);
    void SetError(const byte err);
    void SetRetError(const byte er);
    AppVar app_vars_g[MAX_SV_ELEMENTS];
};

#endif

