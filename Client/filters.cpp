//#include "header.h"

//#define     IPv4_MAX_SIZE    15


//#define     mcr_isDigit(ch)                 ((c >= '0') && (c <= '9'))
//#define     mcr_isSeparator(ch)             (ch == '.')
//#define     mcr_isValidIPv4Number(number)   (number >= 0 && number <= 255)


//enum ErrorCode {
//    NO_ERROR,
//    INCORRECT_FIRST_SYMBOL,
//    BAD_NUMBER,
//    INVALID_SYMBOL,
//    TOO_LONG_ADRESS,

//    MAX_ERROR_CODE
//};

//ErrorCode gl_errorCode {NO_ERROR};



//struct IPv4Error {
//    ErrorCode       errorCode       {NO_ERROR};
//    const char*     errorMessage    {"NO ERROR"};
//};


//IPv4Error IPv4ErrorArray[] = {
//    NO_ERROR,                   "NO ERROR.",
//    INCORRECT_FIRST_SYMBOL,     "ERROR: IPv4 adress has to start with the digit.",
//    BAD_NUMBER,                 "ERROR: Invalid IPv4-number between separators.",
//    INVALID_SYMBOL,             "ERROR: IPv4 adress has to include ONLY digits(from '0' to '9') \
//                                 and '.' as a separator.",
//    TOO_LONG_ADRESS,            "ERROR: IPv4 adress has to have 15 symbols max (including dots)."
//};

////==================================================================================================
//// WHAT: Global function-filter
////  WHY: It returns "true" if input [ipv4String] sutisfies the conditions for IPv4 adress format.
////       Otherwise returns "false".
////==================================================================================================
//bool checkIPv4(const char *ipv4String)
//{
//    const char* ipv4StringPtr {ipv4String};


//    int number  {0};
//    char        ch      {'\0'};

//    // IPv4 требования:
//    //
//    // 1) Адрес допускает только символы '.' и цифры от '0' до '9'.
//    // 2) Фомат адреса должен состоять из 4-ёх полей, разделённых символами-сепараторами.
//    // 3) В каждом поле должно стоять число в десятичном формате в диапазоне от 0 до 255.
//    // 4) Максимальная длина поля IPv4 составляет 15 символов (плюс '\0')


//    // #### Each IPv4-field has to start from the arabic numeral.
//    if (!mcr_isDigit(*ipv4StringPtr))
//    {
//        gl_errorCode = INCORRECT_FIRST_SYMBOL;
//        return false;
//    }
//    else {} // Nothing to do


//    // #### This loop handle the syntax of the IPv4 field.
//    for (int ii {0}; ii <= IPv4_MAX_SIZE; ++ii)
//    {
//        ch = *ipv4StringPtr;

//        // ## Handle the number
//        if (mcr_isDigit(ch))
//        {
//            number = number + (ch - '0');
//        }
//        // ## Handle the separator
//        else if (mcr_isSeparator(ch))
//        {
//            // # Check the number's validity
//            if (mcr_isValidIPv4Number(number))
//            {
//                number = 0;
//            }
//            else
//            {
//                gl_errorCode = BAD_NUMBER;
//                return false;
//            }
//        }
//        // ## Handle other symbols
//        else
//        {
//            gl_errorCode = INVALID_SYMBOL;
//            return false;
//        }

//        ++ipv4StringPtr;
//    }


//    if (*ipv4StringPtr != '\0') {
//        gl_errorCode = TOO_LONG_ADRESS;
//        return TOO_LONG_false;
//    }
//    else {} // Nothing to do


//    return true;


//}

//const char ipv4Test[] = {
//    "",
//    ".111.222.4",
//    "1.11.111.123",
//    "0.0.0.0",
//    "256.45.44.1",
//    "0.1.1.1",
//    "1111.2.3.4",
//    "...",
//    "122.122.122.122.122"
//};


//int main()
//{

//}










