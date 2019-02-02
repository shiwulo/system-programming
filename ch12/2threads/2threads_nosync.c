atomic_int turn=0;
atomic_int flag[2] = {0, 0};

void p0(void) {
    printf("start p0\n");
    while (1) {
        atomic_store(&flag[0], 1);
        atomic_thread_fence(memory_order_seq_cst);
        atomic_store(&turn, 1);
        while (atomic_load(&flag[1]) && atomic_load(&turn)==1)
            ;   //waiting
        //底下程式碼用於模擬在critical section
        in_cs++;
        nanosleep(&ts, NULL);
        if (in_cs == 2) fprintf(stderr, "p0及p1都在critical section\n");
        p0_in_cs++;
        nanosleep(&ts, NULL);
        in_cs--;
        //離開critical section
        atomic_store(&flag[0], 0);
    }
}

atomic_store(&flag[0], 1);
atomic_thread_fence(memory_order_seq_cst);
atomic_store(&turn, 1);

struct semaphore {
    raw_spinlock_t        lock;
    unsigned int        count;
    struct list_head    wait_list;
};

//在這個driver中，需要先鎖定write_buffer，如果鎖定不成功會cotext-switch
down_interruptible(&dev->write_buffer)
//鎖住spinlock，將這個task放到這個device的waiting queue
spin_lock_irqsave(&device->lock, flags);
//設定：如果收到signal，kernel會喚醒這一個task
set_current_state(TASK_INTERRUPTIBLE);
add_wait_queue(&device->wq, &wait);
spin_unlock_irqrestore(&device->lock, flags);
//task已經進入到waiting queue，因此呼叫scheduler，準備切換到下一個task
schedule_timeout(msecs_to_jiffies(max_waiting_time));
//能執行到底下這一行，表示這一個task已經被喚醒，因此這一個task已經在run-queue。將自己從wating queue移除
remove_wait_queue(&device->wq, &wait);
//檢查一下被喚醒的原因，是真的等到I/O或者是有人送signal給這個task
if (signal_pending(current)) {
    /*被signal喚醒，因此就不等這個I/O了，釋放掉之前鎖定的write buffer*/
    up(&dev->write_buffer); /*被signal喚醒，因此就不等這個I/O了，釋放掉之前鎖定的write buffer*/
    return -ERESTARTSYS;
}
up(&dev->write_buffer); /*等到I/O，要釋放掉之前佔用的write_buffer，如果剛好有人在等，喚醒他*/
return success;
