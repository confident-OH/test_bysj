#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x96af5c1, "module_layout" },
	{ 0x9a353ae, "__x86_indirect_alt_call_rax" },
	{ 0xdcedb544, "sock_release" },
	{ 0x2e31761, "kernel_sock_shutdown" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xf9a482f9, "msleep" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0x2021e9e3, "sock_create" },
	{ 0xa916b694, "strnlen" },
	{ 0x2a23ed9e, "kmem_cache_alloc_trace" },
	{ 0x5e08a474, "kmalloc_caches" },
	{ 0x3f13a12b, "kernel_recvmsg" },
	{ 0xc5850110, "printk" },
	{ 0xa1320b70, "kernel_sendmsg" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "3AEDB058218FA123A251376");
