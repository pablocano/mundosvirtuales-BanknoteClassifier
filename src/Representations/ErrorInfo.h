#pragma once
#include "Tools/Streamable.h"

class ErrorInfo : public Streamable
{
public:

    ErrorInfo():error(0),lastbanknote(0){};

    //Error type
    int error;

    //Last banknote
    int lastbanknote;


};
