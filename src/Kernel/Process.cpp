#include <Kernel/Process.hh>

namespace Kernel{
    Process::Process(u64 pid, ProcessStatus status, TrapFrame trapFrame, PageTable* table, u64 pc, u64* stack){
        this->pid = pid;
        this->status = status;
        this->frame = trapFrame;
        this->table = table;
        this->programCounter = pc;
        this->stack = stack;
    }

    Process::Process(){
    }

    Process::~Process(){
    }

    u64 Process::getPid(){
        return pid;
    }

    ProcessStatus Process::getStatus(){
        return status;
    }

    TrapFrame& Process::getFrame(){
        return frame;
    }

    PageTable& Process::getPageTable(){
        return *table;
    }

    u64* Process::getStack(){
        return stack;
    }

    void Process::setStatus(ProcessStatus status){
        this->status = status;
    }
};