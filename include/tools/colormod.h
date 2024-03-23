#ifndef HDEBUG_H
#define HDEBUG_H
#include <ostream>
namespace Color {

    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };


    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };

}

class hDebug
{

private:
    Color::Modifier m_md;
    Color::Modifier md_bk{Color::BG_DEFAULT};
public:

    hDebug(Color::Modifier md)
        : m_md{md}
    {
    }

    ~hDebug(){
        std::cout << '\n';
    }

    template <typename T>
    hDebug&  operator<<(const T &item)
    {
        // if(std::is_same<T,std::string>::value){
            std::cout << m_md << item << md_bk;
        // }
        // else if(std::is_same<T,int>::value){
        //     std::cout << md << item << md_bk << '\n';
        // }

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const hDebug& debug)
    {
        return os;
    }
};

#endif
