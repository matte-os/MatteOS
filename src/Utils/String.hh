#pragma once

namespace Utils{
    class String{
        char* value;
        public:
            String();
            void operator+=(char* value);
            void operator+=(String& other);
            String operator=(char* value);
    };
};