# 30OS
30天自制操作系统
一、内容
1.添加中文字库
2.界面优化
要求：桌面背景；在桌面的左下角做一个开始菜单，点击这个按钮之后就会弹出一个开始菜单的列表。
3.开始菜单应用响应
（1）个人信息显示：点击此菜单，将自己的个人信息使用中文显示在窗口中。
（2）控制台：点击此菜单，在桌面上显示控制台窗口，并能实现关机命令和硬件信息获取命令
（2）游戏：点击此菜单，在游戏正常进行的同时有背景音乐在播放。
（3）关机：点击此菜单，实现关机功能。
（4）文件管理
4.重启命令
5.系统时间显示
二、目的
通过学习操作系统及自己实现相关部分并通过此次实验来巩固和加深理解。

三、实验设计思想和流程
1.添加中文字库：主要是添加显示中文的函数putfont8_ch
1)下载中文GB2312的二进制点阵文件；并将HZK16.fnt文件放入nihongo文件夹中；
2)修改主makefile文件和app_make.txt文件，将原来装载nihongo.fnt的语句替换成装载HZK16.fnt即可；
3)修改bootpack.c文件，将之前分配的装载日语字体的内存扩大，载入字库的文件名；
4)在haribote/graphic.c中添加支持汉字的代码，增加一个函数putfont8_ch用于显示汉字；
5)修改putfonts8_asc函数里if (task->langmode == 3)语句块；
2.界面优化
1）在背景图层左下角描画菜单按钮，并在支持中文模式下描画“菜单”字符串；
2）单独做一个弹出菜单的图层sht_menu，初始时图层高度为-1；
3）在bootpack.c中得到鼠标数据，并进行按下左键判断后，从上往下遍历图层，若鼠标按到背景图层且为菜单按钮，则显示出菜单图层,（即将菜单图层高度上移至最上层）；
4）背景图层加载图片参考图片阅览器的实现，编写set_picture函数，将图片文件读入，若为jpg文件，则进行相应的解码成RGB，并在相应的显存中写入每个像素；在初始化屏幕背景时init_srceen8中加载图片；
3.开始菜单应用响应
1）个人信息显示：在bootpack.c中，按下鼠标左键按到菜单图层并且按到个人信息学按钮，则打开一个新的命令行窗口，并在命令行的缓冲区中放入“info\n”，则命令行任务读到\n则执行infoAPP，info应用程序实现了打开一个“个人信息”的窗口，并读取info.txt文件的内容并逐条显示；
2）控制台：这个在作者已实现代码的基础上，每次左键点击菜单图层上的控制台按钮，就调用open_console函数打开一个新的命令行窗口；
3）游戏：每次左键点击菜单图层上的外星人游戏按钮，则打开一个新的命令行窗口，并在命令行的缓冲区中放入“mmlgame daigo.mml\n”，则命令行任务读到\n则执行mmlgam应用程序，并播放音乐daigo.mml，将外星人游戏与音乐播放器两个应用程序进行合并；
4）关机：左键点击菜单图层上的关机按钮，则打开一个新的命令行窗口，并在命令行的缓冲区中放入“shutdown\n”，则命令行任务读到\n则执行shutdown命令，用汇编实现shutdown，在网上找的关机代码实现从16位保护模式跳到实模式实行关机；
5）文件信息显示
4.重启命令：
在命令行窗口中输入“reboot”然后回车执行reboot命令，用汇编实现，从16位保护模式跳到实模式后，与关机不同的是，将ECS,EIP指向ffff:0实现重启；
5.系统时间显示
在Harimain开头创建一个clock任务并进行相应初始化，并运行task_run将clock任务加入任务链表中每个0.02秒进行任务切换；单独实现了一个sysclock_task函数实现clock任务，通过io_in8,io_out8实现从设备中获取系统时间并进行显示，定时器设置每隔1秒重新获取时间并刷新显示；

四、主要数据结构及符号说明
set_picture函数，将图片文件读入，若为jpg文件，则进行相应的解码成RGB，并在相应的显存中写入每个像素；
sht_menu:菜单图层
info:应用程序，显示个人信息；
Mmlgame：应用程序，将游戏与音乐结合；
void sysclock_task(void)：实现系统时间显示任务；

五、程序初值及运行结果




六、实验体会和思考题
1.背景图片设置时图片文件大小最多一百多KB，太大了会影响系统其他内容的显示；
2.个人信息时给窗口分配的内存和文件分配的内存通过系统自动内配，若通过API自行分配可能因为分配空间太大，造成显示有问题；
3.未解决的问题：显示系统时间时编写的clock任务，应该只要task_run超时到了之后就会自动运行，但有时候打开并没有运行clock任务，显示不出时间；