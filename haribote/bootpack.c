/* bootpack������ */
#include "bootpack.h"
#include <stdio.h>

#define KEYCMD_LED		0xed

int rand(void);
void keywin_off(struct SHEET *key_win);
void keywin_on(struct SHEET *key_win);
void close_console(struct SHEET *sht);
void close_constask(struct TASK *task);

void HariMain(void) {
	struct BOOTINFO *binfo = (struct BOOTINFO *) ADR_BOOTINFO;
	struct SHTCTL *shtctl;
	char s[40];
	struct FIFO32 fifo, keycmd;
	int fifobuf[128], keycmd_buf[32];
	int mx, my, i, new_mx = -1, new_my = 0, new_wx = 0x7fffffff, new_wy = 0;
	unsigned int memtotal;
	struct MOUSE_DEC mdec;
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	unsigned char *buf_back, buf_mouse[256];
	struct SHEET *sht_back, *sht_mouse;
	struct TASK *task_a, *task;
	//;---------------------------------------�޸�----------------------------------------------
	unsigned int start_status = 0;//start_status��ֵ��ʾstart��ť��״̬   0:δ����   1:����
	unsigned char *buf_menu;//�˵�ͼ�� ���ļ�����ͼ��
	struct SHEET *sht_menu, *sht_fm;//sht_backΪ����ͼ��        sht_menuΪ������start��ť�󵯳��Ŀ�ʼ�˵�
	struct SHEET *sht_taskmgr;

	static char keytable0[0x80] =
			{ 0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^',
					0x08, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
					'@', '[', 0x0a, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K',
					'L', ';', ':', 0, 0, ']', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
					',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
					'3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0x5c, 0, 0 };
	static char keytable1[0x80] = { 0, 0, '!', 0x22, '#', '$', '%', '&', 0x27,
			'(', ')', '~', '=', '~', 0x08, 0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
			'I', 'O', 'P', '`', '{', 0x0a, 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
			'K', 'L', '+', '*', 0, 0, '}', 'Z', 'X', 'C', 'V', 'B', 'N', 'M',
			'<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, '_', 0, 0, 0, 0, 0, 0, 0, 0, 0, '|', 0, 0 };

	int key_shift = 0, key_leds = (binfo->leds >> 4) & 7, keycmd_wait = -1;
	int j, x, y, mmx = -1, mmy = -1, mmx2 = 0;
	struct SHEET *sht = 0, *key_win, *sht2;
	int *fat;
	unsigned char *nihongo;
	struct FILEINFO *finfo;
	extern char hankaku[4096];

	init_gdtidt();
	init_pic();
	io_sti(); /* IDT/PIC�ĳ�ʼ���Ѿ����������Խ��CPU���жϽ�ֹ   */
	fifo32_init(&fifo, 128, fifobuf, 0);
	*((int *) 0x0fec) = (int) &fifo;
	init_pit();
	init_keyboard(&fifo, 256);
	enable_mouse(&fifo, 512, &mdec);
	io_out8(PIC0_IMR, 0xf8); /* PIT��PIC1�ͼ�������Ϊ���(11111000) */
	io_out8(PIC1_IMR, 0xef); /* �������Ϊ���(11101111) */
	fifo32_init(&keycmd, 32, keycmd_buf, 0);

	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman, 0x00001000, 0x0009e000); /* 0x00001000 - 0x0009efff */
	memman_free(memman, 0x00400000, memtotal - 0x00400000);

	init_palette();
	shtctl = shtctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	task_a = task_init(memman);
	fifo.task = task_a;
	task_run(task_a, 1, 2);
	*((int *) 0x0fe4) = (int) shtctl;
	task_a->langmode = 3;
	task_a->langbyte1 = 0;

	/* HZK16.fnt������������ */
	fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));

	finfo = file_search("HZK16.fnt",(struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo != 0) {
		i = finfo->size;
		nihongo = file_loadfile2(finfo->clustno, &i, fat);
	} else {
		nihongo = (unsigned char *) memman_alloc_4k(memman,16 * 256 + 32 * 94 * 47);
		for (i = 0; i < 16 * 256; i++) {
			nihongo[i] = hankaku[i]; /* �t�H���g���Ȃ������̂Ŕ��p�������R�s�[ */
		}
		for (i = 16 * 256; i < 16 * 256 + 32 * 94 * 47; i++) {
			nihongo[i] = 0xff; /* �t�H���g���Ȃ������̂őS�p������0xff�Ŗ��ߐs���� */
		}
	}
	*((int *) 0x0fe8) = (int) nihongo;

	/* sht_back */
	sht_back = sheet_alloc(shtctl);
	buf_back = (unsigned char *) memman_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	sheet_setbuf(sht_back, buf_back, binfo->scrnx, binfo->scrny, -1); /* �����F�Ȃ� */
	init_screen8(buf_back, binfo->scrnx, binfo->scrny);

	/* sht_cons */
	key_win = open_console(shtctl, memtotal);

	/* sht_mouse */
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse, buf_mouse, 16, 16, 99);
	init_mouse_cursor8(buf_mouse, 99);
	mx = (binfo->scrnx - 16) / 2; /* �������ʾ�ڻ������� */
	my = (binfo->scrny - 28 - 16) / 2;

	/* file manager */
	sht_fm = init_fileManager(shtctl);


	/*sht_menu*/
	sht_menu = sheet_alloc(shtctl);
	buf_menu = (unsigned char *) memman_alloc(memman, 120 * 180);
	sheet_setbuf(sht_menu, buf_menu, 120, 180, -1);
	init_menu(sht_menu);

	sheet_slide(sht_back, 0, 0);
	sheet_slide(sht_menu, 0, binfo->scrny - 29 -180);
	sheet_slide(key_win, 32, 4);
	sheet_slide(sht_mouse, mx, my);
	sheet_slide(sht_fm, 10, 40);
	//sheet_slide(sht_info, 360, 275);
	sheet_updown(sht_menu, -1);
	sheet_updown(sht_fm, -1);
	//sheet_updown(sht_info, -1);
	sheet_updown(sht_back, 0);
	sheet_updown(key_win, 1);
	sheet_updown(sht_mouse, 2);
	keywin_on(key_win);

	struct TASK *clock = task_alloc("sysclk", 6);
	int *clock_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	clock->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;
	clock->tss.eip = (int) &sysclock_task;
	clock->tss.es = 1 * 8;
	clock->tss.cs = 2 * 8;
	clock->tss.ss = 1 * 8;
	clock->tss.ds = 1 * 8;
	clock->tss.fs = 1 * 8;
	clock->tss.gs = 1 * 8;
	task_run(clock, 1, 2); /* level=1, priority=2 */
	fifo32_init(&clock->fifo, 128, clock_fifo, clock);
	/* Ϊ�˱���ͼ��̵�ǰ״̬��ͻ����һ��ʼ�Ƚ������� */
	fifo32_put(&keycmd, KEYCMD_LED);
	fifo32_put(&keycmd, key_leds);


	//memman_free_4k(memman, (int) fat, 4 * 2880);
	for (;;) {
		if (fifo32_status(&keycmd) > 0 && keycmd_wait < 0) {
			/* �����������̿��������͵����ݣ�������  */
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT, keycmd_wait);
		}
		io_cli();
		if (fifo32_status(&fifo) == 0) {
			/* FIFOΪ�գ������ڸ��õĻ�ͼ����ʱ����ִ�� */
			if (new_mx >= 0) {
				io_sti();
				sheet_slide(sht_mouse, new_mx, new_my);
				new_mx = -1;
			} else if (new_wx != 0x7fffffff) {
				io_sti();
				sheet_slide(sht, new_wx, new_wy);
				new_wx = 0x7fffffff;
			} else {
				task_sleep(task_a);
				io_sti();
			}
		} else {
			i = fifo32_get(&fifo);
			io_sti();
			if (key_win != 0 && key_win->flags == 0) { /* ���ڱ��ر� */
				if (shtctl->top == 1) { /* ���б���ͼ������ͼ�� */
					key_win = 0;
				} else {
					key_win = shtctl->sheets[shtctl->top - 1];
					keywin_on(key_win);
				}
			}
			if (256 <= i && i <= 511) { /* �������� */
				if (i < 0x80 + 256) { /* ����������ת��Ϊ�ַ�����  */
					if (key_shift == 0) {
						s[0] = keytable0[i - 256];
					} else {
						s[0] = keytable1[i - 256];
					}
				} else {
					s[0] = 0;
				}
				if ('A' <= s[0] && s[0] <= 'Z') { /* A-Z */
					if (((key_leds & 4) == 0 && key_shift == 0)
							|| ((key_leds & 4) != 0 && key_shift != 0)) {
						s[0] += 0x20; /*��д��ĸת��ΪСд��ĸ */
					}
				}
				if (s[0] != 0 && key_win != 0) { /* һ���ַ����˸����Enter */
					fifo32_put(&key_win->task->fifo, s[0] + 256);
				}
				if (i == 256 + 0x0f && key_win != 0) { /* Tab */
					keywin_off(key_win);
					j = key_win->height - 1;
					if (j == 0) {
						j = shtctl->top - 1;
					}
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
				}
				if (i == 256 + 0x2a) { /* ��shift ON */
					key_shift |= 1;
				}
				if (i == 256 + 0x36) { /* ��shift ON */
					key_shift |= 2;
				}
				if (i == 256 + 0xaa) { /* ��shift OFF */
					key_shift &= ~1;
				}
				if (i == 256 + 0xb6) { /* ��shift OFF */
					key_shift &= ~2;
				}
				if (i == 256 + 0x3a) { /* CapsLock */
					key_leds ^= 4;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x45) { /* NumLock */
					key_leds ^= 2;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x46) { /* ScrollLock */
					key_leds ^= 1;
					fifo32_put(&keycmd, KEYCMD_LED);
					fifo32_put(&keycmd, key_leds);
				}
				if (i == 256 + 0x3b && key_shift != 0 && key_win != 0) { /* Shift+F1 */
					task = key_win->task;
					if (task != 0 && task->tss.ss0 != 0) {
						cons_putstr0(task->cons, "\nBreak(key) :\n");
						io_cli(); /* �����ڸı�Ĵ���ֵʱ�л����������� */
						task->tss.eax = (int) &(task->tss.esp0);
						task->tss.eip = (int) asm_end_app;
						io_sti();
						task_run(task, -1, 0);
					}
				}
				if (i == 256 + 0x3c && key_shift != 0) { /* Shift+F2 */
					/* �Զ������뽹���л����´򿪵������д��� */
					if (key_win != 0) {
						keywin_off(key_win);
					}
					key_win = open_console(shtctl, memtotal);
					sheet_slide(key_win, 32, 4);
					sheet_updown(key_win, shtctl->top);
					keywin_on(key_win);
				}
				if (i == 256 + 0x57) { /* F11 */
					sheet_updown(shtctl->sheets[1], shtctl->top - 1);
				}
				if (i == 256 + 0xfa) { /* ���̳ɹ����յ����� */
					keycmd_wait = -1;
				}
				if (i == 256 + 0xfe) { /* ����û�гɹ����ܵ����� */
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT, keycmd_wait);
				}
			} else if (512 <= i && i <= 767) { /* ������� */
				if (mouse_decode(&mdec, i - 512) != 0) {
					/* ����ƶ� */
					mx += mdec.x;
					my += mdec.y;
					if (mx < 0) {
						mx = 0;
					}
					if (my < 0) {
						my = 0;
					}
					if (mx > binfo->scrnx - 1) {
						mx = binfo->scrnx - 1;
					}
					if (my > binfo->scrny - 1) {
						my = binfo->scrny - 1;
					}
					new_mx = mx;
					new_my = my;
					if ((mdec.btn & 0x01) != 0) {
						/* �������*/
						if (mmx < 0) {
							/* ����ͨ��ģʽ */
							/* ���մ��ϵ��µ�˳��Ѱ�ұ������ͼ��*/
							for (j = shtctl->top - 1; j >= 0; j--) {	//����ͼ��sht_back�� height = 0, topΪ���ͼ��ĸ߶�;
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if (0 <= x && x < sht->bxsize && 0 <= y && y < sht->bysize) {	//�ҵ��������ͼ��
									if (sht == sht_back) {		//������˱���ͼ��
										if (3 < x && x < 60 && sht->bysize - 24 < y && y < sht->bysize - 5) {//�����ʼ��ť
											keywin_off(key_win);
											key_win = 0;
											//keywin_on(key_win);

											if (start_status == 0) {	//���start��ť״̬����δ������״̬    ���л���������״̬
												start_status = 1;
												//���´���������ʾstart��ť�����µ�Ч��
												start_status_switch(start_status,sht_back);
												sheet_updown(sht_menu, shtctl->top);//��ʾ�˵�
											}
										} else {		//������� ����ͼ���� start��ť����ĵط�
											if (start_status == 1) {	//���start��ť���ڱ�����״̬   ���л���  δ�����µ�״̬
												start_status = 0;
												start_status_switch(start_status,sht_back);
												sheet_updown(sht_menu, -1);
											}
										}
									} else if (sht == sht_menu) {//�������menuͼ��
										if (y < 30) {			//���������Ϣ
											if (key_win != 0) {
												keywin_off(key_win);
											}
											key_win = open_console(shtctl, memtotal);
//											sheet_slide(key_win,rand() % 200 + 160, rand() % 300 + 50);
//											sheet_updown(key_win, shtctl->top);
//											keywin_on(key_win);

											fifo32_put(&(key_win->task->fifo),'i' + 256);
											fifo32_put(&(key_win->task->fifo),'n' + 256);
											fifo32_put(&(key_win->task->fifo),'f' + 256);
											fifo32_put(&(key_win->task->fifo),'o' + 256);
											fifo32_put(&(key_win->task->fifo),'\n' + 256);
										} else if (y < 60) {	//���Consoleѡ��
											if (key_win != 0) {
												keywin_off(key_win);
											}
											key_win = open_console(shtctl, memtotal);
											sheet_slide(key_win,rand() % 200 + 160, rand() % 300 + 50);
											sheet_updown(key_win, shtctl->top);
											keywin_on(key_win);
										} else if (y < 90) {//���Programs  >ѡ��
											if (key_win != 0) {
												keywin_off(key_win);
											}
											key_win = open_console(shtctl, memtotal);
//											sheet_slide(key_win,rand() % 200 + 160, rand() % 300 + 50);
//											sheet_updown(key_win, shtctl->top);
//											keywin_on(key_win);

											fifo32_put(&(key_win->task->fifo),'m' + 256);
											fifo32_put(&(key_win->task->fifo),'m' + 256);
											fifo32_put(&(key_win->task->fifo),'l' + 256);
											fifo32_put(&(key_win->task->fifo),'g' + 256);
											fifo32_put(&(key_win->task->fifo),'a' + 256);
											fifo32_put(&(key_win->task->fifo),'m' + 256);
											fifo32_put(&(key_win->task->fifo),'e' + 256);
											fifo32_put(&(key_win->task->fifo),' ' + 256);
											fifo32_put(&(key_win->task->fifo),'d' + 256);
											fifo32_put(&(key_win->task->fifo),'a' + 256);
											fifo32_put(&(key_win->task->fifo),'i' + 256);
											fifo32_put(&(key_win->task->fifo),'g' + 256);
											fifo32_put(&(key_win->task->fifo),'o' + 256);
											fifo32_put(&(key_win->task->fifo),'.' + 256);
											fifo32_put(&(key_win->task->fifo),'m' + 256);
											fifo32_put(&(key_win->task->fifo),'m' + 256);
											fifo32_put(&(key_win->task->fifo),'l' + 256);
											fifo32_put(&(key_win->task->fifo),'\n' + 256);
										} else if (y < 120) {//���File Managerѡ��
											if (key_win != sht_fm) {
												if (key_win != 0) {
													keywin_off(key_win);
												}
												key_win = sht_fm;
												sheet_updown(sht_fm, shtctl->top);
												keywin_on(key_win);
											}
										} else if (y < 150) {//���Task Managerѡ��
//											sht_taskmgr = open_taskmgr(shtctl, memtotal);
											//open_taskmgrtask(shtctl, memtotal);
										} else {				//���Shutdownѡ��
											key_win = open_console(shtctl, memtotal);
											fifo32_put(&(key_win->task->fifo),'s' + 256);
											fifo32_put(&(key_win->task->fifo),'h' + 256);
											fifo32_put(&(key_win->task->fifo),'u' + 256);
											fifo32_put(&(key_win->task->fifo),'t' + 256);
											fifo32_put(&(key_win->task->fifo),'d' + 256);
											fifo32_put(&(key_win->task->fifo),'o' + 256);
											fifo32_put(&(key_win->task->fifo),'w' + 256);
											fifo32_put(&(key_win->task->fifo),'n' + 256);
											fifo32_put(&(key_win->task->fifo),'\n' + 256);
										}
									} else {		//�����������ͼ��(�� ����ͼ�� ��  menu ͼ��)
									//	���start��ť���ڱ�����״̬ �ұ������ͼ�㲻��menu�ͱ���ͼ��, ���л��� δ�����µ�״̬
										if (start_status == 1) {
											start_status = 0;
											start_status_switch(start_status, sht_back);
											sheet_updown(sht_menu, -1);
										}
										if (sht->buf[y * sht->bxsize + x] != sht->col_inv) {
											sheet_updown(sht, shtctl->top - 1);
											if (sht != key_win) { //�����ǰͼ��   ���л�
												keywin_off(key_win);
												key_win = sht;
												keywin_on(key_win);
											}
											if (3 <= x && x < sht->bxsize - 3 && 3 <= y && y < 21) {
												mmx = mx; /* �����ڱ����������봰���ƶ�ģʽ*/
												mmy = my;
												mmx2 = sht->vx0;
												new_wy = sht->vy0;
											}
											if (sht->bxsize - 21 <= x && x < sht->bxsize - 5 && 5 <= y && y < 19) {
												/* ���X"��ť */

												if ((sht->flags & 0x10) != 0) { /* �ô����Ƿ�ΪӦ�ó��򴰿ڣ� */
													task = sht->task;	//��
													cons_putstr0(task->cons, "\nBreak(mouse) :\n");
													io_cli(); /* ǿ�ƽ��������н�ֹ�л�����*/
													task->tss.eax = (int) &(task->tss.esp0);
													task->tss.eip = (int) asm_end_app;
													io_sti();
													task_run(task, -1, 0);
												}
												else if((sht->flags & 0x40) != 0){	//�ǲ˵�ѡ��ͼ��
													sheet_updown(sht, -1);
													keywin_off(key_win);
													key_win = shtctl->sheets[shtctl->top - 1];
													keywin_on(key_win);
												}
												else { /* ����Ӧ�ó��򴰿� */
													task = sht->task;
													sheet_updown(sht, -1); /* ���ô���������һ��E*/
													keywin_off(key_win);
													key_win = shtctl->sheets[shtctl->top - 1];
													keywin_on(key_win);
													io_cli();
													fifo32_put(&task->fifo, 4);
													io_sti();
												}
											}
										}
									}
									break;
								}
							}
						} else {
							/* ������ڴ����ƶ�ģʽ */
							x = mx - mmx; /* ���������ƶ�����*/
							y = my - mmy;
							new_wx = (mmx2 + x + 2) & ~3;
							new_wy = new_wy + y;
							mmy = my; /*����Ϊ�ƶ��������*/
						}
					} else {
						/* û�а������*/
						mmx = -1; /* ����ͨ��ģʽ */
						if (new_wx != 0x7fffffff) {
							sheet_slide(sht, new_wx, new_wy); /* �̶�ͼ��λ�� */
							new_wx = 0x7fffffff;
						}
					}
				}
			} else if (768 <= i && i <= 1023) { /* �����д��ڹرմ��� */
				close_console(shtctl->sheets0 + (i - 768));
			} else if (1024 <= i && i <= 2023) {
				close_constask(taskctl->tasks0 + (i - 1024));
			} else if (2024 <= i && i <= 2279) { /* �R���\�[����������� */
				sht2 = shtctl->sheets0 + (i - 2024);
				memman_free_4k(memman, (int) sht2->buf, 256 * 165);
				sheet_free(sht2);
			}
		}
	}
}

void keywin_off(struct SHEET *key_win) {
	change_wtitle8(key_win, 0);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 3); /* �����д��ڹ��OFF */
	}
	return;
}

void keywin_on(struct SHEET *key_win) {
	change_wtitle8(key_win, 1);
	if ((key_win->flags & 0x20) != 0) {
		fifo32_put(&key_win->task->fifo, 2); /* �����д��ڹ��ON */
	}
	return;
}

struct TASK *open_constask(struct SHEET *sht, unsigned int memtotal) {
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = task_alloc("console", 7);
	int *cons_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
	task->cons_stack = memman_alloc_4k(memman, 64 * 1024);
	task->tss.esp = task->cons_stack + 64 * 1024 - 12;
	task->tss.eip = (int) &console_task;
	task->tss.es = 1 * 8;
	task->tss.cs = 2 * 8;
	task->tss.ss = 1 * 8;
	task->tss.ds = 1 * 8;
	task->tss.fs = 1 * 8;
	task->tss.gs = 1 * 8;
	*((int *) (task->tss.esp + 4)) = (int) sht;
	*((int *) (task->tss.esp + 8)) = memtotal;
	task_run(task, 2, 2); /* level=2, priority=2 */
	fifo32_init(&task->fifo, 128, cons_fifo, task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl, unsigned int memtotal) {
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *) memman_alloc_4k(memman, 256 * 165);
	sheet_setbuf(sht, buf, 256, 165, -1); /* ��͸��ɫ */
	make_window8(buf, 256, 165, "����̨", 0);
	make_textbox8(sht, 8, 28, 240, 128, COL8_000000);
	sht->task = open_constask(sht, memtotal);
	sht->flags |= 0x20; /* �й�� */
	return sht;
}

void close_constask(struct TASK *task) {
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman, task->cons_stack, 64 * 1024);
	memman_free_4k(memman, (int) task->fifo.buf, 128 * 4);
	task->flags = 0; /* �������task_free(task);  */
	return;
}

void close_console(struct SHEET *sht) {
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman, (int) sht->buf, 256 * 165);
	sheet_free(sht);
	close_constask(task);
	return;
}

//struct SHEET *open_taskmgr(struct SHTCTL *shtctl, unsigned int memtotal)
//{
//	int i;
//	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
//	struct SHEET *sht = sheet_alloc(shtctl);
//	unsigned short *buf = (unsigned short *) memman_alloc_4k(memman, 296 * 290 * 2);
//	sheet_setbuf(sht, buf, 296, 290, -1); /* �����F�Ȃ� */
//	make_window8(buf, 296, 290, "�������", 0);
//	make_textbox8(sht, 5, 24, 291, 284, COL8_000000);
//
//	struct TASK *taskmgr = task_alloc("���������", 10);
//	int *tmgr_fifo = (int *) memman_alloc_4k(memman, 128 * 4);
//	taskmgr->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024 - 12;
//	taskmgr->tss.eip = (int) &taskmgr_task;
//	taskmgr->tss.es = 1 * 8;
//	taskmgr->tss.cs = 2 * 8;
//	taskmgr->tss.ss = 1 * 8;
//	taskmgr->tss.ds = 1 * 8;
//	taskmgr->tss.fs = 1 * 8;
//	taskmgr->tss.gs = 1 * 8;
//	taskmgr->times = 0;
//	*((int *) (taskmgr->tss.esp + 4)) = shtctl;
//	*((int *) (taskmgr->tss.esp + 8)) = memtotal;
//	task_run(taskmgr, 2, 2); /* level=2, priority=2 */
//	fifo32_init(&taskmgr->fifo, 128, tmgr_fifo, taskmgr);
//	return sht;
//}
