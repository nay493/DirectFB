#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xa6d31cde, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0x851fffe3, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x12da5bb2, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x45b6eea6, __VMLINUX_SYMBOL_STR(single_open) },
	{ 0x8dd0d517, __VMLINUX_SYMBOL_STR(seq_open) },
	{ 0xc8b57c27, __VMLINUX_SYMBOL_STR(autoremove_wake_function) },
	{ 0x34184afe, __VMLINUX_SYMBOL_STR(current_kernel_time) },
	{ 0x46608fa0, __VMLINUX_SYMBOL_STR(getnstimeofday) },
	{ 0x85ac2e59, __VMLINUX_SYMBOL_STR(sock_release) },
	{ 0xa478c298, __VMLINUX_SYMBOL_STR(sock_recvmsg) },
	{ 0xfd485ec8, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xe1d3c08b, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x4861b141, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x9eddb535, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x5fb5407c, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0xb07a58e0, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x68dfc59f, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0xad1a9d4c, __VMLINUX_SYMBOL_STR(PDE_DATA) },
	{ 0x2bc95bd4, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x385e7f8, __VMLINUX_SYMBOL_STR(proc_mkdir) },
	{ 0x8122056e, __VMLINUX_SYMBOL_STR(current_task) },
	{ 0x50eedeb8, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x7069c3f1, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0xd06d2d3b, __VMLINUX_SYMBOL_STR(sock_sendmsg) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
	{ 0xdd1a2871, __VMLINUX_SYMBOL_STR(down) },
	{ 0xf6c5699a, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xbfdfc895, __VMLINUX_SYMBOL_STR(__task_pid_nr_ns) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{        0, __VMLINUX_SYMBOL_STR(schedule_timeout) },
	{ 0x4292364c, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0x86a4889a, __VMLINUX_SYMBOL_STR(kmalloc_order_trace) },
	{ 0xca8e24be, __VMLINUX_SYMBOL_STR(force_sig) },
	{ 0x6627cbd9, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xd19662f0, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x7afa89fc, __VMLINUX_SYMBOL_STR(vsnprintf) },
	{ 0xfe35e622, __VMLINUX_SYMBOL_STR(sched_setscheduler) },
	{ 0xe45f60d8, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0x4f68e5c9, __VMLINUX_SYMBOL_STR(do_gettimeofday) },
	{ 0xe174796d, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x7dc9c622, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x2e60bace, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x622fa02a, __VMLINUX_SYMBOL_STR(prepare_to_wait) },
	{ 0xc55e500e, __VMLINUX_SYMBOL_STR(sock_create) },
	{ 0xc4554217, __VMLINUX_SYMBOL_STR(up) },
	{ 0x430b40dd, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x75bb675a, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0xb81960ca, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0xd53cd06d, __VMLINUX_SYMBOL_STR(seq_release) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x5944b486, __VMLINUX_SYMBOL_STR(__class_create) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "517D22798BF4FD7C1D7BA70");
