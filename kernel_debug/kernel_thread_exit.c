kernel/kernel/exit.c

void do_exit(long code)
{
        struct task_struct *tsk = current;
        int group_dead;

        profile_task_exit(tsk);
#ifdef CONFIG_SCHEDSTATS
        /* mt shceduler profiling*/
        printk(KERN_DEBUG "[%d:%s] exit\n", tsk->pid, tsk->comm);
        end_mtproc_info(tsk);
#endif

#ifdef CONFIG_MT_PRIO_TRACER
        delete_prio_tracer(tsk->pid);
#endif

        WARN_ON(blk_needs_flush_plug(tsk));

        if (unlikely(in_interrupt()))
                panic("Aiee, killing interrupt handler!");
        if (unlikely(!tsk->pid))
                panic("Attempted to kill the idle task!");

        /*
         * If do_exit is called because this processes oopsed, it's possible
         * that get_fs() was left as KERNEL_DS, so reset it to USER_DS before
         * continuing. Amongst other possible reasons, this is to prevent
         * mm_release()->clear_child_tid() from writing to a user-controlled
         * kernel address.
         */
        set_fs(USER_DS);

        ptrace_event(PTRACE_EVENT_EXIT, code);

        validate_creds_for_do_exit(tsk);

...

