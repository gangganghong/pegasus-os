// 进程表 start
typedef struct{
        // 中断处理程序压栈，手工压栈
        //unsigned short gs;
        //unsigned short fs;
        //unsigned short es;
        //unsigned short ds;

        unsigned int gs;
        unsigned int fs;
        unsigned int es;
        unsigned int ds;
        // pushad压栈，顺序固定
        unsigned int edi;
        unsigned int esi;
        unsigned int ebp;
        // 在这里消耗了很多时间。为啥需要在这里补上一个值？这是因为popad依次出栈的数据中有这么个值，
        // 如果不补上这一位，出栈时数据不能依次正确更新到寄存器中。
        unsigned int kernel_esp;
        unsigned int ebx;
        unsigned int edx;
        unsigned int ecx;
        unsigned int eax;
        // 中断发生时压栈
        unsigned int eip;
        unsigned int cs;
        //unsigned short cs;
        unsigned int eflags;
        //unsigned short ss;
        unsigned int esp;       // 漏掉了这个。iretd会出栈更新esp。
        unsigned int ss;
}Regs;


// todo 根据目前的需求，下面的成员都是必需的。
typedef struct{
        int source;             // 谁发送的消息
        int type;               // 这条消息要求接收者做什么。例如，获取时钟中断次数。
        int val;                // 消息中的数据。
}Message;

// 发送消息的进程队列的成员的数据类型
struct MsgSender
{
        //Proc *sender;
        unsigned int sender_pid;
        //struct MsgSender next;
        struct MsgSender *next;
};

typedef struct{
        Regs s_reg;
        // ldt选择子
        unsigned short ldt_selector;
        // ldt
        Descriptor ldts[2];
        unsigned int pid;
        // 进程剩余可用的时钟中断次数
        unsigned int ticks;
        // 进程优先级
        unsigned int priority;
        // 本进程使用哪个tty，tty_table的索引
        unsigned int tty_index;
        // 进程名称
        char name[20];

        // ipc start
        unsigned char p_flag;           // 进程的状态：RUNNING等
        Message *p_msg; // 指向消息体
        int p_send_to;  // 要向哪个进程发送消息，目标进程ID
        int p_receive_from;     // 要从哪个进程接收消息，目标进程ID
        struct MsgSender *header;       // 要给本进程发送消息的进程的队列的头指针
        //struct MsgSender header;      // 要给本进程发送消息的进程的队列的头指针

        // ipc end
}Proc;


// 用户进程的数量
#define USER_PROC_NUM 3
// 系统任务的数量
#define TASK_PROC_NUM 4
// 消息收发对象是任意进程时，目标进程的pid是这个值
#define ANY (USER_PROC_NUM + TASK_PROC_NUM + 10)
// 消息收发对象是无对象时，目标进程的pid是这个值
#define NO_TASK (USER_PROC_NUM + TASK_PROC_NUM + 25)
// 系统任何和用户进程的进程表都存储在这个数组中
Proc proc_table[TASK_PROC_NUM + USER_PROC_NUM];

// 用户进程元数据
Task user_task_table[USER_PROC_NUM] = {
        {TestA, A_STACK_SIZE},
        {TestB, B_STACK_SIZE},
        {TestC, C_STACK_SIZE},
};
// 系统任务元数据
Task sys_task_table[TASK_PROC_NUM] = {
        {TaskTTY, TaskTTY_STACK_SIZE},
        {TaskSys, TASK_SYS_SIZE},
        {TaskHD, TASK_HD_SIZE},
        {task_fs, TASK_FS_SIZE},
};
