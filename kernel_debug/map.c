.init.data : {
#ifndef CONFIG_XIP_KERNEL
	INIT_DATA
#endif
		INIT_SETUP(16)
		INIT_CALLS
		CON_INITCALL
		SECURITY_INITCALL
		INIT_RAM_FS
}


#define INIT_CALLS                                                      \
                VMLINUX_SYMBOL(__initcall_start) = .;                   \
                *(.initcallearly.init)                                  \
                INIT_CALLS_LEVEL(0)                                     \
                INIT_CALLS_LEVEL(1)                                     \
                INIT_CALLS_LEVEL(2)                                     \
                INIT_CALLS_LEVEL(3)                                     \
                INIT_CALLS_LEVEL(4)                                     \
                INIT_CALLS_LEVEL(5)                                     \
                INIT_CALLS_LEVEL(rootfs)                                \
                INIT_CALLS_LEVEL(6)                                     \
                INIT_CALLS_LEVEL(7)                                     \
                VMLINUX_SYMBOL(__initcall_end) = .;


#define INIT_CALLS_LEVEL(level)                                         \
                VMLINUX_SYMBOL(__initcall##level##_start) = .;          \
                *(.initcall##level##.init)                              \
                *(.initcall##level##s.init)                             \
#################################################################################

[.init.data]
__initcall_start
__initcall0_start
	.initcall0.init:
		__initcall_##fn##id //__initcall_gpio_test6
		...module_init(fn)
		...
__initcall1_start
	.initcall1.init:
.
.
.
.
__initcall_end

如何在指定的区间放函数指针呢？
core_initcall(fn) --->.initcall1.init 
postcore_initcall(fn) --->.initcall2.init 
arch_initcall(fn) --->.initcall3.init 
subsys_initcall(fn) --->.initcall4.init 
fs_initcall(fn) --->.initcall5.init 
device_initcall(fn) --->.initcall6.init 
late_initcall(fn) --->.initcall7.init


在Linux底下写过driver模块的对这个宏一定不会陌生。module_init宏在MODULE宏有没有定义的情况下展开的内容是不同的，如果这个宏没有定义，基本上表明阁下的模块是要编译进内核的(obj-y)。
1.在MODULE没有定义这种情况下，module_init定义如下：

#define module_init(x)        __initcall(x);
 

因为
#define __initcall(fn)                               device_initcall(fn)
#define device_initcall(fn)                __define_initcall("6",fn,6)
#define __define_initcall(level,fn,id) \
        static initcall_t __initcall_##fn##id __used \
        __attribute__((__section__(".initcall" level ".init"))) = fn
所以，module_init(x)最终展开为：
static initcall_t __initcall_##fn##id __used \
        __attribute__((__section__(".initcall" level ".init"))) = fn

更直白点，假设阁下driver所对应的模块的初始化函数为int gpio_init(void)，那么module_init(gpio_init)实际上等于:
static initcall_t  __initcall_gpio_init_6 __used __attribute__((__section__(".initcall6.init"))) =gpio_init;
就是声明一类型为initcall_t（typedef int (*initcall_t)(void)）函数指针类型的变量__initcall_gpio_init_6并将gpio_init赋值与它。
这里的函数指针变量声明比较特殊的地方在于，将这个变量放在了一名为".initcall6.init"节中。接下来结合vmlinux.lds中的
.initcall.init : AT(ADDR(.initcall.init) - (0xc0000000 -0x00000000)) {
  __initcall_start = .;
  *(.initcallearly.init) __early_initcall_end = .; *(.initcall0.init) *(.initcall0s.init) *(.initcall1.init) *(.initcall1s.init) *(.initcall2.init) *(.initcall2s.init) *(.initcall3.init) *(.initcall3s.init) *(.initcall4.init) *(.initcall4s.init) *(.initcall5.init) *(.initcall5s.init) *(.initcallrootfs.init) *(.initcall6.init) *(.initcall6s.init) *(.initcall7.init) *(.initcall7s.init)
  __initcall_end = .;
  }
以及do_initcalls：
static void __init do_initcalls(void)
{
        initcall_t *call;

        for (call = __initcall_start; call < __initcall_end; call++)
                do_one_initcall(*call);

        /* Make sure there is no pending stuff from the initcall sequence */
        flush_scheduled_work();
}
那么就不难理解阁下模块中的module_init中的初始化函数何时被调用了：在系统启动过程中 start_kernel()->rest_init()->kernel_init()->do_basic_setup()->do_initcalls()。

1.
在看linux核心代码的时候看到/init/main.c 里面的do_initcalls函数
static void __init do_initcalls(void)
{
　　initcall_t *call;

　　call = &__initcall_start;
　　do {
　　　(*call)();
　　　call++;
　　} while (call < &__initcall_end);

　　/* Make sure there is no pending stuff from the initcall sequence */
　　flush_scheduled_tasks();
}
当时就很诧异,在用source insight 查看的时候,没有发现__initcall_start的定义,
然后就很网站搜索,(估计是我搜索水平太菜),没有发现有相关的文章,于是在相关的linux
论坛提问,有人指引我看看/arch/i386/vmlinux.lds
lds是什么?晕菜,不清楚,查了资料得知是ld script.看来的研究ld了

2.
察看/arch/i386/vmlinux.lds,发现一段代码
　__initcall_start = .;
　.initcall.init : { *(.initcall.init) }
　__initcall_end = .;
　
跟我找的东西相关
使用info ld,察看相关资料,（最终发现太麻烦，到网上找了一个ld.pdf).发现有这么一
段介绍关于c++地联结构造器的使用，和这段用法相同
其含义时，是让__initcall_start指向代码节.initcall.init的节首，而__initcall_end
指向.initcall.init的节尾。
那么第一段代码从程序逻辑上得到了解释。

3。
因为do_initcalls所作的是系统中有关于选择的驱动部分的初始化工作，那么具体是这些
函数指针数据怎样放到了.initcall.init节。
想起来了，还没有使用grep哈哈，在
grep -rn .initcall.init *
发现在include/linux/init.h:83:有这样一个定义
#define __init_call　　 __attribute__ ((unused,__section__ (".initcall.init")))
娃哈哈哈
终于让我发现了
然后又发现了
#define __initcall(fn) \
static initcall_t __initcall_##fn __init_call = fn

4。
问题是什么是__attribute__??,查找man gcc,找到关于__attribute__的定义
`section ("section-name")'
　　 Normally, the compiler places the code it generates in the `text'
　　 section.　Sometimes, however, you need additional sections, or you
　　 need certain particular functions to appear in special sections.
　　 The `section' attribute specifies that a function lives in a
　　 particular section.　For example, the declaration:

　　　　　extern void foobar (void) __attribute__ ((section ("bar")));

　　 puts the function `foobar' in the `bar' section.

　　 Some file formats do not support arbitrary sections so the
　　 `section' attribute is not available on all platforms.　If you
　　 need to map the entire contents of a module to a particular
　　 section, consider using the facilities of the linker instead.
　　
他的意思就是使它建造一个在.initcall.init节的指向初始函数的指针

5。
问题是##是什么意思？
查阅gcc的man page得知，她是用在可变参数使用宏定义的时候的
在这里也就是建立一个变量名称为所指向的函数的名称，并且前面加上
__initcall_.

6.
然后看看成果
在/include/linux/init.c中有发现
#define module_init(x) __initcall(x);
看看很多驱动中都有类似
module_init(usb_init);
module_exit(usb_exit);
的代码


