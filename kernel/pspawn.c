/*
 pfork system call
*/

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>

int sys_pspawn(const char *filename,
	const char __user *const __user *__argv,
	const char __user *const __user *__envp,
	struct pt_regs *regs)
{
	struct user_arg_ptr argv = { .ptr.native = __argv };
	struct user_arg_ptr envp = { .ptr.native = __envp };
	int pid;

	pid = do_fork(SIGCHLD, regs->sp, regs, 0, NULL, NULL);
	if (pid) {
	  return do_execve_common(filename, argv, envp, regs);
	}

	return pid;
}
