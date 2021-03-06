//extern InterruptTest; 
int dis_pos;
typedef void (*int_handle) ();
void disp_str(char *str);
// void disp_str_colour(char *str, int colour);
void disp_str_colour2(int str, int colour);
void disp_str_colour(char *str, int colour);

// 内部中断 start
void divide_zero_fault();
void single_step_fault();
void non_maskable_interrupt();
void breakpoint_trap();
void overflow_trap();
void bound_range_exceeded_fault();
void invalid_opcode_fault();
void coprocessor_not_available_fault();
void double_fault_exception_abort();
void coprocessor_segment_overrun();
void invalid_task_state_segment_fault();
void segment_not_present_fault();
void stack_exception_fault();
void general_protection_exception_fault();
void page_fault();
void coprocessor_error_fault();
void align_check_fault();
void simd_float_exception_fault();
// 内部中断 end


// 外部中断 start
void hwint0();
void hwint1();
// 外部中断 end

void atoi(char *str, int num);
void disp_int(int num);
void InterruptTest();
void exception_handler(int vec_no, int error_no, int eip, int cs, int eflags);
unsigned char gdt_ptr[6];
void Memcpy(void *dst, void *src, int size);
//void InitInterruptDesc(int vec_no, int_handle offset); 
void InitInterruptDesc(int vec_no, int_handle offset, int privilege, int type);
// 初始化内部中断
void init_internal_interrupt();

void test();

void spurious_irq(int irq);
void init_propt();

const int INIT_MASTER_VEC_NO = 0x20;
const int INIT_SLAVE_VEC_NO = 0x28;


typedef struct{
	unsigned short seg_limit_below;
	unsigned short seg_base_below;
	unsigned char  seg_base_middle;
	unsigned char seg_attr1;
	unsigned char seg_limit_high_and_attr2;
	unsigned char seg_base_high;
}Descriptor;

Descriptor gdt[128];

unsigned char idt_ptr[6];
//门描述符
typedef struct{
	unsigned short offset_below;
	unsigned short selector;
	unsigned char paramCount;
	unsigned char type_other_attribute;
	unsigned short offset_high;
}Gate;

Gate idt[256];

void ReloadGDT()
{
	//disp_str_colour("AAAA", 0x0C);
	//disp_str_colour("AAAA", 0x0A);
	//disp_str_colour("bbbb", 0x0D);
	//disp_int(0x3);
	//disp_str_colour("Hello,World\n", 0x0C);
	//return;
	Memcpy(&gdt,
		(void *)(*((int *)(&gdt_ptr[2]))),
		*((short *)(&gdt_ptr[0]))
	);
	short *pm_gdt_limit = (short *)(&gdt_ptr[0]);
	int *pm_gdt_base = (int *)(&gdt_ptr[2]);
	
	//*pm_gdt_limit = 128 * sizeof(Descriptor) * 64 - 1;
	*pm_gdt_limit = 128 * sizeof(Descriptor) - 1;
	*pm_gdt_base = (int)&gdt;
	
	// 修改idt_ptr
	short *pm_idt_limit = (short *)(&idt_ptr[0]);
	int *pm_idt_base = (int *)(&idt_ptr[2]);
	*pm_idt_limit = 256 * sizeof(Gate) - 1;
	*pm_idt_base = (int)&idt;

	//for(int i = 0; i < 10; i++){
	//	for(int j = 0; j < 160; j++){
	//		disp_str(" ");
	//	}

	//}
	//for(int i = 0; i < 80 * 2 * 5; i++){
	//dis_pos = 0;
	for(int i = 0; i < 80 * 2 * 25; i++){
		//disp_str(" ");
	}
	//dis_pos = 0;
	// disp_int(0x8);
	// return;
	//disp_str_colour("Hello, World!", 0x74);
	//disp_str_colour("Hello, World!===========I am successful!", 0x0B);
	//disp_str("\n=================\n");
	//disp_str("Hello, World!\n");
	//disp_int(23);
	//disp_int(0x020A);
	//disp_str("\n");
	//return;
	// 向idt中添加中断门 InterruptTest
	 // InitInterruptDesc(1, InterruptTest);	
	 //InitInterruptDesc(0x0, InterruptTest);	
	// 内部中断
	init_internal_interrupt();
	// 外部中断
	init_propt();	
	return;
}

void InitInterruptDesc(int vec_no, int_handle offset, int privilege, int type)
{
	Gate *gate = &idt[vec_no];
	//idt[vec_no].paramCount = 0;
	//// idt[vec_no].offset_below = offset;
	//int base = (int)offset;
	//idt[vec_no].offset_below = base & 0xffff;
	//idt[vec_no].selector = 1;
	//idt[vec_no].offset_high = base >> 16;
	//// 先这样，以后再拆分成TYPE和特权级
	//idt[vec_no].type_other_attribute = 0x08E;
	

	gate->paramCount = 0;
	// gate->offset_below = offset;
	int base = (int)offset;
	gate->offset_below = base & 0xffff;
	gate->selector = 0x8;
	gate->offset_high = base >> 16;
	// 先这样，以后再拆分成TYPE和特权级
	// gate->type_other_attribute = 0x08E;
	gate->type_other_attribute = (privilege << 4) | type;
}


void atoi(char *str, int num)
{
	char *p = str;
	*p++ = '0';
	*p++ = 'x';
	char ch;
	char flag = 0;	

	if(num == 0){
		*p++ = '0';
	}else{
		for(int i = 28; i >= 0; i-=4){
			ch = (num >> i) & 0xF;
			if(flag == 0 && ch == 0) continue;
			flag = 1;
			ch = ch + '0';
			if(ch > '9'){
				ch += 7;	
			}	
			*p++ = ch;
		}
	}	
	
	*p = 0;

	return;
}

void disp_int(int num)
{
	//char *str = "";
	char str[16];
	atoi(str, num);
	//disp_str_colour("ABC", 0x0A);
	// disp_str(str);
	disp_str_colour(str, 0x0B);
	//return;

}

void exception_handler(int vec_no, int error_no, int eip, int cs, int eflags)
{
	char *msg[] = {
		"# Divide by zero:",
		"# Single step:",
		"# Non-maskable  (NMI):",
		"# Breakpoint:",
		"# Overflow trap:",
		"# BOUND range exceeded (186,286,386):",
		"# Invalid opcode (186,286,386):",
		"# Coprocessor not available (286,386):",
		"# Double fault exception (286,386):",
		"# Coprocessor segment overrun (286,386):",
		"# Invalid task state segment (286,386):",
		"# Segment not present (286,386):",
		"# Stack exception (286,386):",
		"# General protection exception (286,386):",
		"# Page fault (286,386):",
		//"# Reserved:",
		"# Coprocessor error (286,386):",

		"#AC :",
		"#MC :",
		"#XF :",
	};
	dis_pos = 0;
	// 清屏
	for(int i = 0; i < 80 * 25 * 2; i++){
		disp_str(" ");
	}	

	dis_pos = 0;
	// int colour = 0x74;
	int colour = 0x0A;
	char *error_msg = msg[vec_no];
	disp_str_colour(error_msg, colour);
	disp_str("\n\n");
	disp_str_colour("vec_no:", colour);
	disp_int(vec_no);
	disp_str("\n\n");
	
	if(error_no != 0xFFFFFFFF){
		disp_str_colour("error_no:", colour);
        	disp_int(error_no);
        	disp_str("\n\n");
	}

	disp_str_colour("cs:", colour);
	disp_int(cs);
	disp_str("\n\n");

	disp_str_colour("eip:", colour);
        disp_int(eip);
        disp_str("\n\n");

	disp_str_colour("eflags:", colour);
        disp_int(eflags);
        disp_str("\n\n");	

	return;
}

void init_internal_interrupt()
{
	InitInterruptDesc(0,divide_zero_fault,0x08,0x0E);
	InitInterruptDesc(1,single_step_fault,0x08,0x0E);
	InitInterruptDesc(2,non_maskable_interrupt,0x08,0x0E);
	InitInterruptDesc(3,breakpoint_trap,0x08,0x0E);
	InitInterruptDesc(4,overflow_trap,0x08,0x0E);
	InitInterruptDesc(5,bound_range_exceeded_fault,0x08,0x0E);
	InitInterruptDesc(6,invalid_opcode_fault,0x08,0x0E);
	InitInterruptDesc(7,coprocessor_not_available_fault,0x08,0x0E);
	InitInterruptDesc(8,double_fault_exception_abort,0x08,0x0E);
	InitInterruptDesc(9,coprocessor_segment_overrun,0x08,0x0E);
	InitInterruptDesc(10,invalid_task_state_segment_fault,0x08,0x0E);
	InitInterruptDesc(11,segment_not_present_fault,0x08,0x0E);
	InitInterruptDesc(12,stack_exception_fault,0x08,0x0E);
	InitInterruptDesc(13,general_protection_exception_fault,0x08,0x0E);
	InitInterruptDesc(14,page_fault,0x08,0x0E);
	InitInterruptDesc(16,coprocessor_error_fault,0x08,0x0E);
	InitInterruptDesc(17,align_check_fault,0x08,0x0E);
	InitInterruptDesc(18,simd_float_exception_fault,0x08,0x0E);	
}

void test()
{
	disp_str("A");
	disp_int(0x6);
	disp_str("\n");
	//return;
	//disp_str_colour2(0x9988, 0x74);
	dis_pos = 0;
	for(int i = 0; i < 80 * 25 * 2; i++){
                disp_str(" ");
        }
        dis_pos = 0;
	//return;
        disp_str_colour("Hello, World!", 0x0F);
	disp_str("\n");
        disp_int(0x89);
	disp_str("\n");
        disp_str_colour("Hello, World!", 0x0F);
        disp_str_colour("Hello, World!", 0x74);
	disp_str("\n");
        disp_str_colour("Hello, World!===========I am successful!", 0x0F);
        disp_str("\n=================\n");
        disp_str("Hello, World!\n");
        disp_int(23);
	disp_str("\n");
        disp_int(0x020A);
        disp_str("\n");
}


void disp_str_colour3(char *str, int colour)
{

}

void spurious_irq(int irq)
{
	disp_str_colour("\n------------irq start---------------\n", 0x0B);
	disp_int(irq);
	disp_str_colour("\n------------irq end---------------\n", 0x0C);
}

void init_propt()
{
	InitInterruptDesc(INIT_MASTER_VEC_NO + 0, hwint0 ,0x08,0x0E);	
	InitInterruptDesc(INIT_MASTER_VEC_NO + 1, hwint1 ,0x08,0x0E);	
}
