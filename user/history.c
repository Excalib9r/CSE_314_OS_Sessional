#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/myStruct.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if(argc == 2){
        int syscall_no = atoi(argv[1]);
        struct myStruct strct;
        history(syscall_no, &strct);
        printf("%d:\tsyscall: %s, #: %d, time: %d\n", syscall_no, strct.syscall_name, strct.count, strct.accum_time);
    }
    else if(argc == 1){
        int i;
        for(i = 1; i < 24; i++){
            struct myStruct strct;
            history(i, &strct);
            printf("%d:\tsyscall: %s, #: %d, time: %d\n", i, strct.syscall_name, strct.count, strct.accum_time);
        }
    }
    return 0;
}
