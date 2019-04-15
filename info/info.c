#include "apilib.h"

void HariMain(void){
	char win_buf[1024 * 757], text_buf[240 * 1024], *q, *p, c;
	int fh, win, x, y, size;
	int w = 30, h = 10, t = 4, xskip = 0;
	int lang = api_getlang();
	api_initmalloc();
//	win_buf = api_malloc(w * h);

	win = api_openwin(win_buf, w * 8 + 16, h * 16 + 37, -1, "个人信息");
	api_boxfilwin(win, 6, 27, w * 8 + 10, h * 16 + 30, 7);

	fh = api_fopen("info.txt");		//打开个人信息文件
	if (fh == 0) {
		api_putstr0("File not found.\n");
		api_end();
	}
	size = api_fsize(fh, 0);
//	text_buf = api_malloc(size + 10);
	api_fread(text_buf, size, fh);	//读取个人信息
	api_fclose(fh);
	text_buf[size + 1] = 0;

	q = text_buf;
	x = 0, y = 0;
	for(p = text_buf; *p != 0; p++){
		if(*p == 0x0d){
			*p = 0x00;
			api_putstrwin(win, 8, 29 + y * 16, 0, p - q, q);//显示一条语句
			y++;
			q = p+2;
		}
	}
	for (;;) {
		if (api_getkey(1) == 0x0a) {
			break; /* 按下Enter则break; */
		}
	}
	api_end();
}
