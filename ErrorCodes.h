//
// Created by razk on 02/06/18.
//

#ifndef ERRORCODES_H
#define ERRORCODES_H

#define PRINT_ERR(ErrorCode) printf("ERR [%d] : FILE [%s] LINE [%s]", ErrorCode, __FILE__, __LINE__);

typedef enum _ErrorCode{
    FAIL = -1,
    SUCCESS = 0
} ErrorCode;

#endif //ERRORCODES_H
