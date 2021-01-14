#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *p;			/*指標未給初始值 */
	int ret;
	printf("輸入數字\n");
	ret = scanf("%d", p);	//這一行會出錯
	printf("ret = %d, %d", ret, *p);	//這一行會出錯
}
