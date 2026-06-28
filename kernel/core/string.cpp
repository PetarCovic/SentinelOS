#include <sentinel/string.hpp>
#include <sentinel/types.hpp>

namespace sentinel::string
{
    sentinel::u64 length(const char* str)
    {
        if(str==nullptr)
        {
            return 0;
        }

        sentinel::u64 count=0;

        while(str[count]!='\0')
        {
            count++;
        }

        return count;
    }

    bool equals(const char* str1, const char* str2)
    {
        if(str1==nullptr && str2==nullptr)
        {
            return true;
        }

        if(str1==nullptr || str2==nullptr)
        {
            return false;
        }

        if(length(str1)!=length(str2))
        {
            return false;
        }

        for(int i=0; str1[i]!='\0'; i++)
        {
            if(str1[i]!=str2[i])
            {
                return false;
            }
        }

        return true;
    }

    void copy(char* dest, const char* src)
    {
        if(dest==nullptr)
        {
            return;
        }

        if(src==nullptr)
        {
            dest[0]='\0';
            return;
        }

        int i=0;
        while(src[i]!='\0')
        {
            dest[i]=src[i];
            i++;
        }

        dest[i]='\0';
    }

    void concat(char* dest, const char* src)
    {
        if(dest==nullptr || src==nullptr)
        {
            return;
        }

        int i=0;

        while(dest[i]!='\0')
        {
            i++;
        }

        int j=0;

        while(src[j]!='\0')
        {
            dest[i]=src[j];
            i++;
            j++;
        }

        dest[i]='\0';
    }

    bool starts_with(const char* str, const char* substr)
    {
        if(str==nullptr)
        {
            return false;
        }

        if(substr==nullptr)
        {
            return false;
        }

        for(int i=0; substr[i]!='\0'; i++)
        {
            if(str[i]!=substr[i])
            {
                return false;
            }
        }
        return true;
    }

    void remove_last(char* str)
    {  
        if(str==nullptr)
        {
            return;
        }

        sentinel::u64 len=length(str);

        if(len==0)
        {
            return;
        }

        str[len-1]='\0';
    }

    void clear(char* str)
    {
        if(str==nullptr)
        {
            return;
        }

        str[0]='\0';
    }
}