/*
 pfork system call
*/

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>

int sys_pfork(struct pt_regs *regs) {
	struct task_struct *p;
	long nr;

	p = copy_process(CLONE_VM | CLONE_VFORK | SIGCHLD, regs->sp, regs, 0,
			 NULL, NULL, 0);

	if (!IS_ERR(p)) {
		struct completion vfork;

		trace_sched_process_fork(current, p);

		nr = task_pid_vnr(p);

		p->vfork_done = &vfork;
		init_completion(&vfork);

		audit_finish_fork(p);

		/*
		 * We set PF_STARTING at creation in case tracing wants to
		 * use this to distinguish a fully live task from one that
		 * hasn't finished SIGSTOP raising yet.  Now we clear it
		 * and set the child going.
		 */
		p->flags &= ~PF_STARTING;

		wake_up_new_task(p);

		/* forking complete and child started to run, tell ptracer */
		if (unlikely(trace))
			ptrace_event(trace, nr);

		freezer_do_not_count();
		wait_for_completion(&vfork);
		freezer_count();
		ptrace_event(PTRACE_EVENT_VFORK_DONE, nr);
	} else {
		nr = PTR_ERR(p);
	}
	return nr;
}
