;********************************************************************************************************
;                                               uC/OS-II
;                                         The Real-Time Kernel
;
;                               (c) Copyright 1992-2006, Micrium, Weston, FL
;                                          All Rights Reserved
;
;                                           Generic ARM Port
;
; File      : OS_CPU_A.ASM
; Version   : V2.84
; By        : Jean J. Labrosse
;
; For       : ARMv7M Cortex-M3
; Mode      : Thumb2
; Toolchain : RealView Development Suite
;             RealView Microcontroller Development Kit (MDK)
;             ARM Developer Suite (ADS)
;             Keil uVision
; 版本：V2.85    作者：tangjun  时间：2010/05/04
; 版本描述：修改了任务切换时，R4，R5寄存器没有保存的问题，增加了压栈与出栈的操作。
;********************************************************************************************************

                EXTERN  OSRunning                ; External references
                EXTERN  OSPrioCur
                EXTERN  OSPrioHighRdy
                EXTERN  OSTCBCur
                EXTERN  OSTCBHighRdy
                EXTERN  OSIntNesting
                EXTERN  OSIntExit
                EXTERN  OSTaskSwHook


                EXPORT  OS_CPU_SR_Save           ; Functions declared in this file
                EXPORT  OS_CPU_SR_Restore
                EXPORT  OSStartHighRdy
                EXPORT  OSCtxSw
                EXPORT  OSIntCtxSw
                EXPORT  OSPendSV

NVIC_INT_CTRL   EQU     0xE000ED04               ; interrupt control state register
NVIC_SYSPRI2    EQU     0xE000ED20               ; system priority register (2)
NVIC_PENDSV_PRI EQU     0x00000000               ; PendSV priority value (lowest)
NVIC_PENDSVSET  EQU     0x10000000               ; value to trigger PendSV exception


;*********************************************************************************************************
;                                   CRITICAL SECTION METHOD 3 FUNCTIONS
;
; Description: Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
;              would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
;              disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to
;              disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
;              into the CPU's status register.
;
; Prototypes :     OS_CPU_SR  OS_CPU_SR_Save(void);
;                  void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
;
;
; Note(s)    : 1) These functions are used in general like this:
;
;                 void Task (void *p_arg)
;                 {
;                 #if OS_CRITICAL_METHOD == 3          /* Allocate storage for CPU status register */
;                     OS_CPU_SR  cpu_sr;
;                 #endif
;
;                          :
;                          :
;                     OS_ENTER_CRITICAL();             /* cpu_sr = OS_CPU_SaveSR();                */
;                          :
;                          :
;                     OS_EXIT_CRITICAL();              /* OS_CPU_RestoreSR(cpu_sr);                */
;                          :
;                          :
;                 }
;
;              2) OS_CPU_SaveSR() is implemented as recommended by Atmel's application note:
;
;            (N/A for Cortex-M3)    "Disabling Interrupts at Processor Level"
;*********************************************************************************************************

        AREA |.text|, CODE, READONLY, ALIGN=2
		THUMB
		REQUIRE8
		PRESERVE8


OS_CPU_SR_Save
        MRS     R0, PRIMASK                 ; set prio int mask to mask all (except faults)
        CPSID   I											 
        BX      LR

OS_CPU_SR_Restore
        MSR     PRIMASK, R0
        BX      LR


;*********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; This function uses the context switcher (see below) to cause the first task to start.
; This is because for Cortex-M3 it makes most sense to implement a context switch within
; an exception handler, to take advantage of the processor's auto context save and restore
; features.
;
; Note(s) : 1) OSStartHighRdy() MUST:
;              a) Setup PendSV exception priority to lowest,
;              b) Set initial psp to 0, to tell context switcher this is first run
;              c) Set OSRunning to TRUE,
;              d) Trigger PendSV exception,
;              e) Enable interrupts (tasks will run with ints enabled)
;*********************************************************************************************************

OSStartHighRdy

		PUSH    {R4, R5}
        LDR     R4, =NVIC_SYSPRI2       ; set the PendSV exception priority
        LDR     R5, =NVIC_PENDSV_PRI
        STR     R5, [R4]

        MOV     R4, #0                  ; set the PSP to 0 for initial context switch call
        MSR     PSP, R4

        LDR     R4, __OS_Running        ; OSRunning = TRUE
        MOV     R5, #1
        STRB    R5, [R4]

        LDR     R4, =NVIC_INT_CTRL      ; trigger the PendSV exception (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]

		POP     {R4, R5}
        CPSIE   I                       ; enable interrupts at processor level
OSStartHang
        B       OSStartHang             ; should never get here


;*********************************************************************************************************
;                         PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
;
; Note(s) : 1) OSCtxSw() is called when OS wants to do a task context switch
;              This function triggers the PendSV exception which is where the
;              real work is done.
;*********************************************************************************************************

OSCtxSw                                 ; 由用户手动触发一次PendSV中断，完成任务切换，如使用信号量，延时
		PUSH    {R4, R5}                ; 函数等。
        LDR     R4, =NVIC_INT_CTRL      ; trigger the PendSV exception (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]
		POP     {R4, R5}
        BX      LR

;*********************************************************************************************************
;                   PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
;
; Notes:    1) This function is called by OSIntExit() when it determines a context
;              switch is needed as the result of an interrupt.
;
;           2) This function simply triggers a PendSV exception which will cause the
;              OSPendSV to run whenever there are no more interrupts active
;              and interrupts are enabled.
;*********************************************************************************************************

OSIntCtxSw                              ; 定时器中断后立即触发一次PendSV中断，当定时器中断退出后，立即会进入 
        PUSH    {R4, R5}                ; 到PendSV中断入口程序，进行任务的切换。
        LDR     R4, =NVIC_INT_CTRL      ; trigger the PendSV exception (causes context switch)
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]
		POP     {R4, R5}
        BX      LR

        NOP

;*********************************************************************************************************
;                         PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
;
; Note(s) : 1) OSPendSV is used to cause a context switch.  This is a recommended
;              method for performing context switches with Cortex-M3.  This is because
;              Cortex-M3 auto-saves half of the processor context on any exception, and
;              restores same on return from exception.  So only saving of R4-R11 is required
;              and fixing up the stack pointers.  Using the PendSV exception this way means
;              that context saving and restoring is identical method whether it is
;              initiated from a thread or occurs due to an interrupt or exception.
;              Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch)
;              b) Save remaining regs r4-r11 on process stack
;              c) Save the process SP in it's TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook()
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr
;              h) Restore R4-R11 from new process stack
;              i) Perform exception return which will restore remaining context
;
;           2) On entry into OSPendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           3) Since OSPendSV is set to lowest priority in the system (by OSCtxSw() above),
;              we know that it will only be run when no other exception or interrupt is
;              active, and therefore safe to assume that context being switched out was
;              using the process stack (PSP)
;*********************************************************************************************************

OSPendSV                                ; PendSV中断入口程序，直接定义到vectors.s文件中，不需要被其它函数调用
        MRS     R0, PSP                 ; PSP is process stack pointer
        CBZ     R0, OSPendSV_nosave     ; skip register save the first time

        SUB     R0, R0, #0x20           ; save remaining regs r4-11 on process stack
        STM     R0, {R4-R11}            ; 只对R4-R11进行压栈，因为XPSR,PC,LR,R12,R3,R2,R1,R0 cortex会自行
                                        ; 压栈(进入中断后硬件自行压栈)。

        LDR     R4, __OS_TCBCur         ; OSTCBCur->OSTCBStkPtr = SP; 把压栈后的指针保存回到被切换任务的
        LDR     R4, [R4]                ; OS_TCB中，OSTCBCur首地址的位置就是OSTCBStkPtr
        STR     R0, [R4]                ; R0 is SP of process being switched out

                                        ; at this point, entire context of process has been saved
OSPendSV_nosave
        PUSH    {R14}                   ; need to save LR exc_return value
        LDR     R0, __OS_TaskSwHook     ; OSTaskSwHook(); 调用任务切换回调函数
        BLX     R0                      ; BLX会影响R14的结果，所以需要缓冲起来
        POP     {R14}

        LDR     R4, __OS_PrioCur        ; OSPrioCur = OSPrioHighRdy
        LDR     R5, __OS_PrioHighRdy
        LDRB    R6, [R5]
        STRB    R6, [R4]

        LDR     R4, __OS_TCBCur         ; OSTCBCur  = OSTCBHighRdy;
        LDR     R6, __OS_TCBHighRdy
        LDR     R6, [R6]
        STR     R6, [R4]

        LDR     R0, [R6]                ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
        LDM     R0, {R4-R11}            ; restore r4-11 from new process stack
        ADD     R0, R0, #0x20			; 因为要恢复8个寄存器，所以为0x20(r4-r11)
        MSR     PSP, R0                 ; load PSP with new process SP
		                         
        ORR     LR, LR, #0x04           ; 保证异常返回后使用PSP
		BX      LR                      ; 异常返回

        NOP

;*********************************************************************************************************
;                                     POINTERS TO VARIABLES
;*********************************************************************************************************

;        DATA

__OS_TaskSwHook
        DCD     OSTaskSwHook

__OS_IntExit
        DCD     OSIntExit

__OS_IntNesting
        DCD     OSIntNesting

__OS_PrioCur
        DCD     OSPrioCur

__OS_PrioHighRdy
        DCD     OSPrioHighRdy

__OS_Running
        DCD     OSRunning

__OS_TCBCur
        DCD     OSTCBCur

__OS_TCBHighRdy
        DCD     OSTCBHighRdy

        END
