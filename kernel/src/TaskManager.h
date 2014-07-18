#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "Common.h"

struct Task
{
	uint32_t Id;
	uint32_t Esp, Ebp, Eip;
	Task* NextTask;
	bool SwitchedTasks;
};

class TaskManager
{
public:
	static void Init();
	static void SwitchTask();
	static void CreateThread(void(*)());
private:
	static Task* _currentTask;
	static Task* _taskQueueStart;
	static uint32_t _nextId;
};

#endif
