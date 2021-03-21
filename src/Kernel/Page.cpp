#include <Kernel/Page.hh>

namespace Kernel{
    Page::Page(){}

    Page::Page(PageBits flag){
        this->flags = flag;
    }

    bool Page::isLast(){
        if(flags & PageBits::LAST != 0){
            return true;
        }else{
            return false;
        }
    }

    bool Page::isTaken(){
        if(flags & PageBits::TAKEN != 0){
            return true;
        }else{
            return false;
        }
    }

    bool Page::isFree(){
        return !Page::isTaken();
    }

    void Page::clear(){
        flags = PageBits::EMPTY;
    }

    void Page::setFlag(PageBits flag){
        flags |= flag;
    }

}