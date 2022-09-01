#include <Kernel/Memory/Page.h>

namespace Kernel::Memory
{
    Page::Page(){
        status = PageStatus::FREE;
    }

    Page::~Page(){

    }

    bool Page::isFree(){
        return status == PageStatus::FREE; 
    }

    bool Page::isTaken(){
        return status == PageStatus::TAKEN;
    }

    bool Page::isLast(){
        return status == PageStatus::LAST;
    }

    void Page::clear(){
        status = PageStatus::FREE;
    }

    void Page::setStatus(PageStatus status){
        this->status = status;
    }
};
