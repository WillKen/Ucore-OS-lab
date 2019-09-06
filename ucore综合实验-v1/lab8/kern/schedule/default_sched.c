#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <default_sched.h>
#define USE_SKEW_HEAP 1

/* You should define the BigMFQ constant here*/
/* LAB6: YOUR CODE */
#define BIG_MFQ    0x7FFFFFFF /* ??? */

/* The compare function for two skew_heap_node_t's and the
 * corresponding procs*/
static int
proc_MFQ_comp_f(void *a, void *b)
{
     struct proc_struct *p = le2proc(a, lab6_run_pool);
     struct proc_struct *q = le2proc(b, lab6_run_pool);
     int32_t c = p->lab6_MFQ - q->lab6_MFQ;
     if (c > 0) return 1;
     else if (c == 0) return 0;
     else return -1;
}

/*
 * MFQ_init initializes the run-queue rq with correct assignment for
 * member variables, including:
 *
 *   - run_list: should be a empty list after initialization.
 *   - lab6_run_pool: NULL
 *   - proc_num: 0
 *   - max_time_slice: no need here, the variable would be assigned by the caller.
 *
 * hint: see proj13.1/libs/list.h for routines of the list structures.
 */
static void
MFQ_init(struct run_queue *rq) {
     int i = 0;
     for(i = 0 ;i < 6 ;i++)
     	list_init(&(rq->run_list[i]));
     rq->proc_num = 0;
}

/*
 * MFQ_enqueue inserts the process ``proc'' into the run-queue
 * ``rq''. The procedure should verify/initialize the relevant members
 * of ``proc'', and then put the ``lab6_run_pool'' node into the
 * queue(since we use priority queue here). The procedure should also
 * update the meta date in ``rq'' structure.
 *
 * proc->time_slice denotes the time slices allocation for the
 * process, which should set to rq->max_time_slice.
 * 
 * hint: see proj13.1/libs/skew_heap.h for routines of the priority
 * queue structures.
 */
static void
MFQ_enqueue(struct run_queue *rq, struct proc_struct *proc) {
    assert(list_empty(&(proc->run_link)));
    if (proc -> time_slice == 0 && proc -> lab6_priority != 5) {
        ++(proc -> lab6_priority);
    }
    list_add_before(&(rq->run_list[proc ->lab6_priority]), &(proc->run_link));
    proc->time_slice = (rq->max_time_slice << proc -> lab6_priority);
    proc->rq = rq;
    rq->proc_num ++;
}

/*
 * MFQ_dequeue removes the process ``proc'' from the run-queue
 * ``rq'', the operation would be finished by the skew_heap_remove
 * operations. Remember to update the ``rq'' structure.
 *
 * hint: see proj13.1/libs/skew_heap.h for routines of the priority
 * queue structures.
 */
static void
MFQ_dequeue(struct run_queue *rq, struct proc_struct *proc) {
     assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
     list_del_init(&(proc->run_link));
     rq->proc_num --;
}
/*
 * MFQ_pick_next pick the element from the ``run-queue'', with the
 * minimum value of MFQ, and returns the corresponding process
 * pointer. The process pointer would be calculated by macro le2proc,
 * see proj13.1/kern/process/proc.h for definition. Return NULL if
 * there is no process in the queue.
 *
 * When one proc structure is selected, remember to update the MFQ
 * property of the proc. (MFQ += BIG_MFQ / priority)
 *
 * hint: see proj13.1/libs/skew_heap.h for routines of the priority
 * queue structures.
 */
static struct proc_struct *
MFQ_pick_next(struct run_queue *rq) {
    int p = rand() % (32 + 16 + 8 + 4 + 2 + 1);
    int priority;
    if (p >= 0 && p < 32) {
        priority = 0;
    } else if (p >= 32 && p < 48) {
        priority = 1;
    } else if (p >= 48 && p < 56) {
        priority = 2;
    } else if (p >= 56 && p < 60) {
	priority = 3;
    } else if (p >= 60 && p < 62) {
	priority = 4;
    } else if (p >= 62 && p < 63) {
	priority = 5;
    }
    list_entry_t *le = list_next(&(rq->run_list[priority]));
    if (le != &(rq->run_list[priority])) {
        return le2proc(le, run_link);
    } else {
	int i = 0;
        for (i = 0; i < 5; ++i) {
            le = list_next(&(rq->run_list[i]));
            if (le != &(rq -> run_list[i])) return le2proc(le, run_link);
        }
    }
    return NULL;
}

/*
 * MFQ_proc_tick works with the tick event of current process. You
 * should check whether the time slices for current process is
 * exhausted and update the proc struct ``proc''. proc->time_slice
 * denotes the time slices left for current
 * process. proc->need_resched is the flag variable for process
 * switching.
 */
static void
MFQ_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
     if (proc->time_slice > 0) {
          proc->time_slice --;
     }
     if (proc->time_slice == 0) {
          proc->need_resched = 1;
     }
}

struct sched_class default_sched_class = {
     .name = "MFQ_scheduler",
     .init = MFQ_init,
     .enqueue = MFQ_enqueue,
     .dequeue = MFQ_dequeue,
     .pick_next = MFQ_pick_next,
     .proc_tick = MFQ_proc_tick,
};

