#ifndef MANAGER_H
#define MANAGER_H

#include<Arduino.h>


typedef struct{
  void (*funcptr)();
  word Delay;
  word Period;
  byte RunMe;
} sTask;

class Manager{
  public:
    Manager(const byte MAX_TASK);
    void DispatchTasks();
    byte AddTask(void (*)(void),const word,const word);
    byte DeleteTask(const byte);
    void Update(void);
    //void ReportStatus(void);
    byte MAX_TASK;
    sTask *Tasks_G;    
};

#endif
