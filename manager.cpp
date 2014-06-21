#include "manager.h"
#include "error.h"

Manager::Manager(const byte max_task){
  byte i;
  MAX_TASK = max_task;
  Tasks_G = new sTask[MAX_TASK];
  for(i=0;i<MAX_TASK;i++){
    Tasks_G[i].funcptr = 0;
    Tasks_G[i].Delay = 0;
    Tasks_G[i].Period = 0;
    Tasks_G[i].RunMe = 0;
  }  
}

void Manager::Update(){
  byte index;
  for(index = 0;index < MAX_TASK;index++){
    //for every tasks
    if(Tasks_G[index].funcptr != 0){
      //check if there is a task in this index
      if(Tasks_G[index].Delay == 0){
        //if task is due to run
        Tasks_G[index].RunMe += 1;
        if(Tasks_G[index].Period){
          //run again if task is periodic
          Tasks_G[index].Delay = Tasks_G[index].Period;
        }
     
      }else{
        //Not yet ready to run decrement the delay
        Tasks_G[index].Delay -= 1;        
      }
    }  
  }
}

byte Manager::AddTask(void (*pf)(void),const word DELAY,const word PERIOD){
  byte index = 0;
  while((Tasks_G[index].funcptr != 0)&&(index < MAX_TASK)){
    index++;
  }
  
  if(index == MAX_TASK){
    Serial.println(F("TASK_G FULL"));
    return MAX_TASK;
  }
  
  Tasks_G[index].funcptr = pf;
  Tasks_G[index].Delay = DELAY;
  Tasks_G[index].Period = PERIOD;
  Tasks_G[index].RunMe = 0;
  return index;  
}

byte Manager::DeleteTask(const byte index){
  byte return_code;
  if(Tasks_G[index].funcptr == 0){
    Serial.println(F("CANNOT DELETE TASK"));
    return_code = ERR_ERROR;
  }else{
    return_code = ERR_NORMAL;
  }
  Tasks_G[index].funcptr = 0;
  Tasks_G[index].Delay = 0;
  Tasks_G[index].Period = 0;
  Tasks_G[index].RunMe = 0;
  
  return return_code;
}

void Manager::DispatchTasks(){
  byte index;
  for(index = 0;index < MAX_TASK; index++){
    if(Tasks_G[index].RunMe > 0){
      (*Tasks_G[index].funcptr)();
      Tasks_G[index].RunMe -= 1;
      if(Tasks_G[index].Period == 0){
        DeleteTask(index);
      }
    }
  }
}
