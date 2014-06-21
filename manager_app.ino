#include <MemoryFree.h>
#include <TimerOne.h>
#include <applink.h>
#include <appvar.h>
#include "sysvar.h"
#include "manager.h"

//size of the global string buffer
#define BUFFERSIZE 256


//Global Variables
//Manager m -> the manager
//applink_found -> flag for determining a valid AppLink packet is found
//str -> temporary storage for valid AppLink packets
//ss -> the SysVar object

Manager m = Manager(3);
byte applink_found = 0;
byte *str;
SysVar ss;


void setup(){
  byte i;
  Serial.begin(9600);
  while(!Serial);
  Serial.println("MCU READY!");
  
  //initalize the SysVar
  ss = SysVar();
  
  //initialize timer1 and attach a callback function when it overflows every 1mS (1000uS)  
  Timer1.initialize();
  Timer1.attachInterrupt(t_callback,1000);
  
  //add CheckForApplink task at the manager and call it every 250mS with an offset of 1s
  m.AddTask(CheckForApplink,1000,250);
  //add ReadSerial task at the manager and call it every 80mS with an offset of 2s
  m.AddTask(ReadSerial,2000,80);
  //add UpdateMemory task to the manager and call it every 2s with an offset of 2.5s
  m.AddTask(UpdateMemory,2500,2000);
  
  //initalize global buffer with buffersize
  str = new byte[BUFFERSIZE];
}

void loop(){
  m.DispatchTasks();
}

/*------------------------------------------------------------------------------*-
* Timer1 overflow callback function. At each timer1 overflow interrupt, manager's
* 'Update Function' is called.
-*------------------------------------------------------------------------------*/

void t_callback(){
  m.Update();
}

/*------------------------------------------------------------------------------*-
* Task set to determine and report remaining SRAM resource.
-*------------------------------------------------------------------------------*/

void UpdateMemory(){
  //AppVar tmp_appvar = AppVar(MEM_G,READ | MGVAR | T_U_INT,S_U_INT);
  //tmp_appvar.SetData(UIntToArray(freeMemory()));
  ss.Set(MEM_G,UIntToArray(freeMemory()));
  //AppLinkPacket tmp_packet = AppLinkPacket(ss.Get(MEM_G));
  //tmp_packet.Write();
  //tmp_appvar.free();
  //tmp_packet.free();
}

/*------------------------------------------------------------------------------*-
* Task set to parse valid AppLink Packet. Corresponding actions are taken to
* each of message class types. Check 'applink.h' for the types of message classes.
-*------------------------------------------------------------------------------*/


void CheckForApplink(){
   if(applink_found){
     //Serial.println(F("APLNK FOUND"));
     if((str[0] & 0x80) == P_S_STREAM){
       //message is of a single stream type
       if((str[0] & 0x40) == P_S_APPVAR){
         //message has only one AppVar variable carried
         AppLinkPacket tmp_packet;
         byte tmp_bt1[1],tmp_bt2[2],tmp_bt4[4];
         switch(str[0] & 0x30){
           case P_BROADCAST:
             //do nothing for now
             break;
             
           case P_QUERY:
             //if query
             if(str[1] < MAX_SV_ELEMENTS){
               //make sure the index is not out of bounds
                if((ss.app_vars_g[str[1]].data_class & 0x80) == READ){
                  //ensures the variable is readable
                  tmp_packet = AppLinkPacket(ss.Get(str[1]));
                  tmp_packet.Write();
                }else{
                  ss.SetRetError(ERR_SYS_VAR_CANT_BE_READ);
                  tmp_packet = AppLinkPacket(ss.Get(RET_ERR_G));
                  tmp_packet.Write();
                }                
             }else{
                ss.SetRetError(ERR_SYS_APPVAR_NOT_DEFINE);
                tmp_packet = AppLinkPacket(ss.Get(RET_ERR_G));
                tmp_packet.Write();
             }            
             break;
           
           case P_COMMAND:
             
             //if it is a command
             if(str[1] < MAX_SV_ELEMENTS){
               if((ss.app_vars_g[str[1]].data_class & 0x40) == WRITE){
                 switch(str[3]){
                   case 0x01:
                     tmp_bt1[0] = str[4]; 
                     ss.Set(str[1],tmp_bt1);
                     tmp_packet = AppLinkPacket(ss.Get(str[1]));
                     tmp_packet.Write();                     
                     break;
                   case 0x02:
                     tmp_bt2[0] = str[4];
                     tmp_bt2[1] = str[5];
                     ss.Set(str[1],tmp_bt2);
                     tmp_packet = AppLinkPacket(ss.Get(str[1]));
                     tmp_packet.Write();
                     break;
                   case 0x04:
                     tmp_bt4[0] = str[4];
                     tmp_bt4[1] = str[5];
                     tmp_bt4[2] = str[6];
                     tmp_bt4[3] = str[7];                     
                     ss.Set(str[1],tmp_bt4);
                     tmp_packet = AppLinkPacket(ss.Get(str[1]));
                     tmp_packet.Write();
                     break;                     
                 }
               }else{
                  ss.SetRetError(ERR_SYS_VAR_CANT_BE_WRITTEN);
                  tmp_packet = AppLinkPacket(ss.Get(RET_ERR_G));
                  tmp_packet.Write();
               }
             }else{
                ss.SetRetError(ERR_SYS_APPVAR_NOT_DEFINE);
                tmp_packet = AppLinkPacket(ss.Get(RET_ERR_G));
                tmp_packet.Write();
             }
             
             break;
                     
         }
         //free temporary variable to prevent heap defragmentation
         tmp_packet.free();           
       }
     }
     //clear flag
     applink_found = 0;
   }
}


/*------------------------------------------------------------------------------*-
* Task set to read incoming stream from the Serial port and checks if it is a valid
* AppLink Packet by checking its start-of-sequence , end-of-sequence , and checksum
* bytes. This task is called periodically by the manager at a designated period as
* initialized.
-*------------------------------------------------------------------------------*/


void ReadSerial(){
  byte s,cs,csb,i=0;
  byte *tmp;
  if(Serial.available() > 0){
    //if there exist character from the serial port
    if(Serial.read() == SS0){
      if(Serial.read() == SS1){
          //if packet has valid start of sequence, read payload length
          s = Serial.read() - 1;
          //create temporary storage for the packet
          tmp = new byte[s];
          //read the rest of the payload and compute the checksum along the way 
          while(i<s){
            tmp[i] = Serial.read();
            if(i==0){
              //first payload value is the base checksum
              cs = tmp[i];
            }else{
              //update checksum value spontaneously
              cs = cs ^ tmp[i];
            }
            i++;
          }
          //read the packet checksum
          csb = Serial.read();
          if(Serial.read() == ES){
            //if packet has valid end of sequence
            if(csb == cs){
              //if the packet satisfies the checksum, copy tmp to the global buffer str
              //update application that a valid AppLink Packet has been recieved
              memcpy(str,tmp,s);
              applink_found = 1;
            }
          }
          //free tmp to reduce memory usage
          delete [] tmp;
      }
    }
    while(Serial.available() > 0){
      Serial.read();
    }
  }
}

/*--------------------------------------------------------------------------------*-
* Miscellaneous conversion routines
-*--------------------------------------------------------------------------------*/

byte* ULongToArray(unsigned long ut){
  byte t[4];
  t[3] = (ut & 0x000000FF) >> 0;
  t[2] = (ut & 0x0000FF00) >> 8;
  t[1] = (ut & 0x00FF0000) >> 16;
  t[0] = (ut & 0xFF000000) >> 24;
  return t;
}

byte* UIntToArray(unsigned int it){
  byte t[2];
  t[1] = (it & 0x00FF) >> 0;
  t[0] = (it & 0xFF00) >> 8;
  return t;
}

/*
byte* ByteArrayToArray(byte* bt,byte b_size){
  byte *t = new byte[b_size];
  switch(b_size){
    case 0x01:
      t[0] = bt[0];
      break;
    case 0x02:
      t[0] = bt[0];
      t[1] = bt[1];
      break;
    case 0x04:
      t[0] = bt[0];
      t[1] = bt[1];
      t[2] = bt[2];
      t[3] = bt[3];
      break; 
  }
  return t;
}
*/

