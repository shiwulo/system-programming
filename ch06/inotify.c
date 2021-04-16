#include <sys/inotify.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//設定每次read最多讀取1000個物件，這裡如果設定太小，可能會有「漏失」某些事件
#define BUF_LEN (1000 * (sizeof(struct inotify_event) + NAME_MAX + 1))

void printInotifyEvent(struct inotify_event *event);

//key-value的映射，陣列的形式，key就是第一個index
//最多可以映射1000個，value最多是10000個字母
char wd[1000][4000];

void printInotifyEvent(struct inotify_event *event)
{
	int eatRet;
	char buf[8192] = "";
	//printf("@event = %p\n", event);
	sprintf(buf, "來自[%s]的事件 ", wd[event->wd]);

	//底下是將所有的事件做檢查，照理說應該只會有一個事件
	strncat(buf+strlen(buf), "{", 4095);
	if (event->mask & IN_ACCESS) {
		strncat(buf + strlen(buf), "ACCESS, ", 4095);
	}
	if (event->mask & IN_ATTRIB) {
		strncat(buf + strlen(buf), "ATTRIB, ", 4095);
	}
	if (event->mask & IN_CLOSE_WRITE)
	{
		strncat(buf + strlen(buf), "CLOSE_WRITE, ", 4095);
	}
	if (event->mask & IN_CLOSE_NOWRITE)
	{
		strncat(buf + strlen(buf), "IN_CLOSE_NOWRITE, ", 4095);
	}
	if (event->mask & IN_CREATE)
	{
		strncat(buf + strlen(buf), "IN_CREATE, ", 4095);
	}
	if (event->mask & IN_DELETE)
	{
		strncat(buf + strlen(buf), "IN_DELETE, ", 4095);
	}
	if (event->mask & IN_DELETE_SELF)
	{
		strncat(buf + strlen(buf), "IN_DELETE_SELF, ", 4095);
	}
	if (event->mask & IN_MODIFY)
	{
		strncat(buf + strlen(buf), "IN_MODIFY, ", 4095);
	}
	if (event->mask & IN_MOVE_SELF)
	{
		strncat(buf + strlen(buf), "IN_MOVE_SELF, ", 4095);
	}
	if (event->mask & IN_MOVED_FROM)
	{
		strncat(buf + strlen(buf), "IN_MOVED_FROM, ", 4095);
	}
	if (event->mask & IN_MOVED_TO)
	{
		strncat(buf + strlen(buf), "IN_MOVED_TO, ", 4095);
	}
	if (event->mask & IN_OPEN)
	{
		strncat(buf + strlen(buf), "IN_OPEN", 4095);
	}
	if (event->mask & IN_IGNORED)
	{
		strncat(buf + strlen(buf), "IN_IGNORED, ", 4095);
	}
	if (event->mask & IN_ISDIR)
	{
		strncat(buf + strlen(buf), "IN_ISDIR, ", 4095);
	}
	if (event->mask & IN_Q_OVERFLOW)
	{
		strncat(buf + strlen(buf), "IN_Q_OVERFLOW, ", 4095);
	}
	strncat(buf, "}, ", 4095);
	eatRet = snprintf(buf + strlen(buf), 4095, "cookie=%d, ", event->cookie);
	if (event->len > 0)
		eatRet = snprintf(buf + strlen(buf), 4095, "name = %s\n", event->name);
	else
		eatRet = snprintf(buf + strlen(buf), 4095, "name = null\n");
	printf("%s\n", buf);
}

int main(int argc, char **argv)
{
	//監聽的頻道
	int fd;
	int nRead, ret, i;
	char *eventPtr;
	char *inotify_entity = (char *)malloc(BUF_LEN);
	//跟作業系統要一個監聽專用的『頻道』，作業系統會幫我們建立一個檔案，
	//用這個檔案「送」資料給我們，並且自動開啟該「檔案/頻道」，並給它的fd
	fd = inotify_init();
	//設定在哪些檔案監聽哪些事件
	for (i = 1; i < argc; i++)
	{
		//inotify_add_watch，對檔案其路徑是『argv[i]』，監聽所有事件
		ret = inotify_add_watch(fd, argv[i], IN_ALL_EVENTS);
		if (ret == -1) {
			printf("argv[%d]=%s\n", i, argv[i]);
			perror("inotify_add_watch");
		} else {
			printf("監聽檔案 %s \n", argv[i]);
		}
		//這裡構成一個簡單的 key-value 的結構
		//key是 「watch  descriptor」， value是檔案名稱
		strcpy(wd[ret], argv[i]);
	}
	//使用一個while loop不斷地讀取 inotify_init() 所開啟的檔案 fd
	//fd 裏面就是我們要監聽的訊息
	while (1)
	{
		//一直讀取，作業系統開給我們的頻道，nRead是這次頻道中的資料量大小
		nRead = read(fd, inotify_entity, BUF_LEN);
		printf("🎄 🌲 🌳 🌴 🪵 🌱 🌿 ☘️  🍀 🎍 🪴 🎋\n");
		printf("從與作業系統的秘密檔案通道讀到『%d』個字元\n", nRead);
		//底下的 for loop 不斷地將收進來的資料切割成『不定長度的』的 inotify_event
		printf("這些字元的解析如下\n");
		for (eventPtr = inotify_entity; eventPtr < inotify_entity + nRead;)
		{
			printInotifyEvent((struct inotify_event *)eventPtr);
			/*
            struct inotify_event {
                int      wd;       // Watch descriptor 
                uint32_t mask;     // Mask describing event 
                uint32_t cookie;   // Unique cookie associating related
                                   //     events (for rename(2))
                uint32_t len;      // Size of name field 🦐 🦞 🦀 在這裏OS會告訴我們name的長度
                char     name[];   // Optional null-terminated name
                };
            */
			//目前這個物件的長度是 基本的inotiry_event的長度 ＋ name字串的長度
			//將eventPtr加上物件長度，就是下一個物件的開始位置
			eventPtr += sizeof(struct inotify_event) + ((struct inotify_event *)eventPtr)->len;
		}
	}
}
