#include <Kernel/Process.hh>

namespace Kernel{
    Process::Process(u32 pid){
        this->pid = pid;
        this->status = ProcessStatus::CREATED;
    }

    Process::~Process(){
    }

    u32 Process::getPid(){
        return pid;
    }

    ProcessStatus Process::getStatus(){
        return status;
    }

    void Process::setStatus(ProcessStatus status){
        this->status = status;
    }
};