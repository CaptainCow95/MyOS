#include "MemoryManager.h"
#include "TaskManager.h"

Task* TaskManager::_currentTask;
Task* TaskManager::_taskQueueStart;
uint32_t TaskManager::_nextId;

extern "C" uint32_t ReadEip();

void TaskManager::Init()
{
	_currentTask = _taskQueueStart = (Task*)MemoryManager::Allocate(sizeof(Task));
	_currentTask->Id = _nextId++;
	_currentTask->Esp = _currentTask->Ebp = 0;
	_currentTask->NextTask = 0;
	_currentTask->SwitchedTasks = false;
}

void TaskManager::SwitchTask()
{
	asm volatile("cli");
	
	uint32_t esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	eip = ReadEip();
	
	if(_currentTask->SwitchedTasks)
	{
		_currentTask->SwitchedTasks = false;
		return;
	}
	
	_currentTask->Esp = esp;
	_currentTask->Ebp = ebp;
	_currentTask->Eip = eip;
	_currentTask->SwitchedTasks = true;
	
	_currentTask = _currentTask->NextTask;
	if(_currentTask == 0)
	{
		_currentTask = _taskQueueStart;
	}
	
	esp = _currentTask->Esp;
	ebp = _currentTask->Ebp;
	eip = _currentTask->Eip;
	
	asm volatile("mov %0, %%eax\n\tmov %1, %%esp\n\tmov %2, %%ebp\n\tsti\n\tjmp *%%eax" :: "r"(eip), "r"(esp), "r"(ebp));
}

void TaskManager::CreateThread(void(*newEip)())
{
	Task* newTask = (Task*)MemoryManager::Allocate(sizeof(Task));
	newTask->Id = _nextId++;
	uint32_t stack = (uint32_t)MemoryManager::AllocateAligned(0x4000);
	newTask->Stack = stack;
	newTask->Esp = stack + 0x4000;
	newTask->Esp = newTask->Ebp = newTask->Esp - sizeof(uint32_t);
	*((uint32_t*)newTask->Esp) = (uint32_t)&ThreadFinished;
	newTask->Eip = (uint32_t)newEip;
	newTask->NextTask = 0;
	newTask->SwitchedTasks = false;
	
	asm volatile("cli");
	// Use _currentTask instead of _taskQueueStart to hopefully skip the beginning
	Task* lastTask = _currentTask;
	while(lastTask->NextTask != 0)
	{
		lastTask = lastTask->NextTask;
	}
	
	lastTask->NextTask = newTask;
	
	asm volatile("sti");
}

void TaskManager::ThreadFinished()
{
	asm volatile("cli");
	Task* task = _taskQueueStart;
	while(task->NextTask != _currentTask)
	{
		task = task->NextTask;
	}
	
	task->NextTask = task->NextTask->NextTask;
	
	MemoryManager::Free((void*)_currentTask->Stack);
	asm volatile("sti");
	
	for(;;);
}
