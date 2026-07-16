#pragma once
#include <string>


class Data_Source
{
    public:
    virtual std::string Get_Data();
    //Call Get_Data() using the >> operator
    void operator >>(std::string &Data);
    void operator >>(int &Data);
    void operator >>(float &Data);
    void operator >>(double &Data);
    void operator >>(bool &Data);
};

