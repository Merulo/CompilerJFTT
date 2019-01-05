#include "Register.hpp"

bool Register::shouldSave(Register one)
{
    //check if both are unkown
    if (one.state == RegisterState::UNKNOWN)
    {
        return false;
    }
    if (one.state == this->state)
    {
        if (one.state == RegisterState::VARIABLE)
        {
            return one.variableName != this->variableName;
        }
        if (one.state == RegisterState::TABLE)
        {
            return one.variableName != this->variableName;
        }
        if (one.state == RegisterState::CONST)
        {
            return false;
        }
        if (one.state == RegisterState::CONSTVARIABLE)
        {
            return one.variableName != this->variableName || one.constValue != this->constValue;
        }
    }
    return true;
}

// bool Register::shouldSave(Register one)
// {
//     //check if both are unkown
//     if (one.state == this->state && one.state == RegisterState::UNKNOWN)
//     {
//         return false;
//     }
//     if (one.state == RegisterState::UNKNOWN || this->state == RegisterState::UNKNOWN)
//     {
//         return true;
//     }
//     if (one.state == this->state)
//     {
//         if (one.state == RegisterState::VARIABLE)
//         {
//             return one.variableName != this->variableName;
//         }
//         if (one.state == RegisterState::TABLE)
//         {
//             return one.variableName != this->variableName;
//         }
//         if (one.state == RegisterState::CONST)
//         {
//             return false;
//         }
//         if (one.state == RegisterState::CONSTVARIABLE)
//         {
//             return one.variableName != this->variableName || one.constValue != this->constValue;
//         }
//     }
//     return true;
// }