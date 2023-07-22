#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main()
{
   printf("hello world\n");
   char buf[10];
   gets(buf, 9);
   int num = atoi(buf);


   printf("%d^2 = %d\n", num, num * num);


   return 0;
}
