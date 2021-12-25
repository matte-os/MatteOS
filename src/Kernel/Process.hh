#pragma once
#include <Kernel/TrapFrame.hh>
#include <Kernel/PageTable.hh>
#include <Utils/Types.hh>
namespace Kernel{
    enum class ProcessStatus{
        RUNNING,
        SLEEPING,
        WAITING,
        DEAD
    };
    class Process{
    private:
        u64 pid;
        ProcessStatus status;
        TrapFrame frame;
        PageTable* table;
        u64 programCounter;
        u64* stack;
    public:
        Process();
        Process(u64, ProcessStatus, TrapFrame, PageTable*, u64, u64*);
        ~Process();
        u64 getPid();
        ProcessStatus getStatus();
        TrapFrame& getFrame();
        PageTable& getPageTable();
        u64* getStack();
        void setStatus(ProcessStatus status);
    };
};