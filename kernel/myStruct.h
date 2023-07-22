struct myStruct{
    struct spinlock myLock;
    char syscall_name[16];
    int count;
    int accum_time;
};