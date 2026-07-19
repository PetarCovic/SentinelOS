#include <sentinel/string.hpp>
#include <sentinel/types.hpp>

namespace sentinel::string
{
    static char to_lower_ascii(char c);

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

        int i=0;

        while(str1[i]!='\0' && str2[i]!='\0')
        {
            if(str1[i]!=str2[i])
            {
                return false;
            }

            i++;
        }

        return str1[i]=='\0' && str2[i]=='\0';
    }

    bool equals_ignore_case(const char* str1, const char* str2)
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

        int i=0;

        while(str1[i]!='\0' && str2[i]!='\0')
        {
            if(to_lower_ascii(str1[i])!=to_lower_ascii(str2[i]))
            {
                return false;
            }

            i++;
        }

        return str1[i]=='\0' && str2[i]=='\0';
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

    bool is_empty(const char* str)
    {
        if(length(str)>0)
        {
            return false;
        }

        return true;
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

    bool starts_with_ignore_case(const char* str, const char* substr)
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
            if(to_lower_ascii(str[i])!=to_lower_ascii(substr[i]))
            {
                return false;
            }
        }
        return true;
    }

    void substr(const char* src, char* dst, sentinel::u32 beginIndex)
    {
        if(dst==nullptr || src==nullptr)
        {
            return;
        }
        
        if(beginIndex>length(src))
        {
            return;
        }

        clear(dst);

        sentinel::u32 i;
        for(i=beginIndex; src[i]!='\0'; i++)
        {
            dst[i-beginIndex]=src[i];
        }

        dst[i-beginIndex]='\0';
    }

    void substr(const char* src, char* dst, sentinel::u32 beginIndex, sentinel::u32 endIndex)
    {
        if(dst==nullptr || src==nullptr)
        {
            return;
        }
        
        if(beginIndex>length(src) || beginIndex>endIndex || endIndex>length(src))
        {
            return;
        }

        clear(dst);

        sentinel::u32 i;
        for(i=beginIndex; i<endIndex; i++)
        {
            dst[i-beginIndex]=src[i];
        }
        
        dst[i-beginIndex]='\0';
    }

    bool parse_u64(const char* str, sentinel::u64* out_value)
    {
        if(str == nullptr || out_value == nullptr)
        {
            return false;
        }

        sentinel::u64 i = 0;

        while(str[i] == ' ' || str[i] == '\t')
        {
            i++;
        }

        sentinel::u64 base = 10;

        if(str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
        {
            base = 16;
            i += 2;
        }

        sentinel::u64 value = 0;
        bool found_digit = false;

        const sentinel::u64 max_value = ~static_cast<sentinel::u64>(0);

        while(str[i] != '\0')
        {
            char c = str[i];

            if(c == ' ' || c == '\t')
            {
                break;
            }

            sentinel::u64 digit;

            if(c >= '0' && c <= '9')
            {
                digit = static_cast<sentinel::u64>(c - '0');
            }
            else if(base == 16 && c >= 'a' && c <= 'f')
            {
                digit = static_cast<sentinel::u64>(c - 'a' + 10);
            }
            else if(base == 16 && c >= 'A' && c <= 'F')
            {
                digit = static_cast<sentinel::u64>(c - 'A' + 10);
            }
            else
            {
                return false;
            }

            if(digit >= base)
            {
                return false;
            }

            if(value > (max_value - digit) / base)
            {
                return false;
            }

            value = (value * base) + digit;
            found_digit = true;
            i++;
        }

        while(str[i] == ' ' || str[i] == '\t')
        {
            i++;
        }

        if(str[i] != '\0')
        {
            return false;
        }   

        if(!found_digit)
        {
            return false;
        }

        *out_value = value;
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

    static char to_lower_ascii(char c)
    {
        if(c>='A' && c<='Z')
        {
            return static_cast<char>(c-'A'+'a');
        }

        return c;
    }
}