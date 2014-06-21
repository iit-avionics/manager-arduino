/*----------------------------------------------------------------------------------*-
* 'sysvar.cpp'
*  SYSVAR library implementation for APPLINK PROTOCOL
*  Part of the iit-avionics project
*  All rights reserve. MSU-IIT Iligan City
-*----------------------------------------------------------------------------------*/

#include "sysvar.h"

SysVar::SysVar(void){
  byte i,none[] = {0x00,0x00,0x00,0x00};
  app_vars_g[SYS_STATE_G] = AppVar(SYS_STATE_G,READ | WRITE | MGVAR | T_BYTE,S_BYTE);
  app_vars_g[TIME_G] = AppVar(TIME_G,READ | WRITE | MGVAR | T_BYTE,S_U_LONG);
  app_vars_g[ERROR_G] = AppVar(ERROR_G,READ | WRITE | MGVAR | T_BYTE,S_BYTE);
  app_vars_g[MEM_G] = AppVar(MEM_G,READ | MGVAR | T_U_INT,S_U_INT);
  app_vars_g[RET_ERR_G] = AppVar(RET_ERR_G,READ | MGVAR | T_BYTE,S_BYTE);
  //initialize values to 0x00
  for(i=0;i<MAX_SV_ELEMENTS;i++)
    app_vars_g[i].SetData(none);
}

void SysVar::SetError(const byte err){
  byte e[] = {err};
  app_vars_g[ERROR_G].SetData(e);
}

void SysVar::SetRetError(const byte err){
  byte e[] = {err};
  app_vars_g[RET_ERR_G].SetData(e);
}

AppVar SysVar::Get(const byte index){
  return app_vars_g[index];
}

AppVar *SysVar::GetBulk(const byte *indices,const byte sz){
    byte i;
    AppVar *var_array = new AppVar[sz];
    for(i = 0;i < sz; i++){
      var_array[i] = app_vars_g[indices[i]];
    }
    return var_array;     
}

/*
byte SysVar::Set(const AppVar v){
  byte i = 0;
  while(i != MAX_SV_ELEMENTS){
    if (app_vars_g[i].unique_id == v.unique_id ) break;
    i++;
  }
  if(i == MAX_SV_ELEMENTS){
    SetError(ERR_SYS_APPVAR_NOT_DEFINE);
  }else{
    app_vars_g[i].SetData(v.data);
  }
  return i;
}
*/

byte SysVar::Set(const byte id,byte* data){
  byte i = 0;
    while(i != MAX_SV_ELEMENTS){
    if (app_vars_g[i].unique_id == id ) break;
    i++;
  }
  if(i == MAX_SV_ELEMENTS){
    SetError(ERR_SYS_APPVAR_NOT_DEFINE);
  }else{
    app_vars_g[i].SetData(data);
  }
  return i;
}





