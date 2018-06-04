//
// Created by razk on 02/06/18.
//

#ifndef ERRORCODES_H
#define ERRORCODES_H

#define PRINT_ERR(ErrorCode) printf("ERR [%d] : LINE [%d] FILE [%s]", ErrorCode, __LINE__, __FILE__);

typedef enum _ErrorCode{
    UNINITIALIZED = -2,
    FAIL = -1,
    SUCCESS = 0
} ErrorCode;

#endif //ERRORCODES_H
