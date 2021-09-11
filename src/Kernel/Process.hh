#pragma once
#include <Utils/Types.hh>
namespace Kernel{
    enum class ProcessStatus{
        CREATED
    };
    class Process{
    private:
        u32 pid;
        ProcessStatus status;
    public:
        Process(u32);
        ~Process();
        u32 getPid();
        ProcessStatus getStatus();
        void setStatus(ProcessStatus status);
    };
};