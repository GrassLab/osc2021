# Lab 4: Exception and Interrupt

Exception 是一種能夠讓目前執行於 CPU 的程式放棄 CPU 執行權的事件，藉由 Exception 可以達成以下目標:
1. 在執行時若發生錯誤，則可以及時處理。
2. User program 可以藉由 Exception 取得需要的作業系統服務。
3. 周端裝置可以藉由 Exception 來讓 CPU 執行其 Handler。

### Requirements
1. Exception Level Switch
2. Exception Handling
3. Core Timer Interrupt

### Elective
1. Enable Interrupt in EL1
2. Rpi3’s Peripheral Interrupt
3. Timer Multiplexing
4. Concurrent I/O Devices Handling

## Background
### Official Reference
Exceptions 與 CPU 的設計關聯性極高，接下來只會簡單描述所需要的 components，其餘的資訊可以查詢官方文件:
1. [ARM’s official introduction](https://developer.arm.com/documentation/102412/0100)
2. [ARM Architecture Reference Manual ARMv8](https://developer.arm.com/documentation/ddi0487/aa/?lang=en)

### Exception Levels
藉由 Priciple of least privilege，program 需要盡量限制其存取的權利，這種限制可以降低當錯誤或是惡意攻擊發生時所造成的影響，如此，可以增加系統的安全性與穩定性。

Armv8-A CPUs 藉由 Exception Levels 來達成以上的限制。

![](https://i.imgur.com/lGnjChb.png)

Armv8-A 有四層 Exception Levels (ELs)。通常，所有的 user program 都執行於 EL0，而作業系統執行於 EL1。

作業系統可以將其的 Exception Level 並藉由設定 system register 與 Exception Return Instruction 來跳到 User program 執行。

當 User program 發生 Exception 時，Exception Level 提升然後 CPU 會開始執行 Exception Handler。

### Exception Handling
當 CPU 要處理 Exception 時，會進行以下操作:
1. 將目前處理器狀態 (PSTATE) 存放於 SPSR_ELx (x 為 target Exception Level)
2. 將 Exception return address 存放於 ELR_ELx
3. 暫停 Interrupt (將 PSTATE.{D, A, I, F} 設為 1)
4. 若 Exception 為 synchronous exception 或是 SError interrupt，則將造成 Exception 的原因存放於 ESR_ELx
5. 切換至 target Exception Level 並執行 vector address 的 handler。

當 Exception handler 執行完畢，它會發起 ``eret`` 以從 Exception 中返回，接下來 CPU 要進行以下操作:
1. 藉由 ELR_ELx 回復 Program counter
2. 藉由 SPSR_ELx 回復 PSTATE
3. 藉由 SPSR_ELx 切換 Exception Level

#### Vector Table
Vector table 裡面存放各種 Exception 對應的 Handler 位址，table 的 base address 存放於 VBAR_ELx。

![](https://i.imgur.com/VzPs4SX.png)

table 中左半部分為 Exception 的 cause，在 Lab 中，我們只需要處理 Synchronous Exceptions 與 IRQ Exceptions。table 的右半部為發生 Exception 的 EL 與目標 EL 的關係，在 Lab 中，我們只需要處理 kernel 發生的 Exception (EL1 -> EL1) 與 64 bit user program 發生的 Exception (EL0 -> EL1)。此外，我們也希望 kernel 和 user program 使用不同的 stacks (使用 SP_ELx)，因此，對應到 table 的 ``Exception from the currentEL while using SP_ELx`` 與 ``Exception from a lower EL and at least one lower EL is AARCH64``。

### Interrupts
CPU 也需要控制 I/O Devices，由於要確認 I/O Devices 是否準備好，CPU 需要在處理 I/O data 前先確認 device 的狀態。

若使用 busy polling 來確認 device 的狀態，會浪費大量的 CPU Time，此外，CPU 可能無法馬上在 I/O Device 準備好時來處理 I/O data，這會導致 I/O Device 的效率低落以及 data loss。

為了解決以上問題，可以讓 I/O Deivce 在準備好時發起 Exception 以通知 CPU，接著，CPU 可以執行 Device handler 來立刻處理資料。

#### Interrupt Controllers
Rpi3 有兩層的 Interrupt Controllers，第一層的 Controller 會路由 Interrupt 到不同的 CPU core，因此每個 CPU core 都會有自己的 timer interrupt 並可向其他 processor 送出 Interrupts。

文件: https://github.com/raspberrypi/documentation/blob/master/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf

第二層的 Controller 可以路由其他周圍設備的 Interrupt，像是 UART 與 System timer 等等，會將第二層的 Interrupts 彙整後向第一層的 Controller 送出 GPU IRQ。

文件: https://cs140e.sergio.bz/docs/BCM2837-ARM-Peripherals.pdf (第 109 頁)

#### Critical Sections
critical section 為不同讓不同 core 同時執行的程式碼片段，當 Interrupt 被開啟時，CPU 可能因為 Interrupt 去存取一些共享資料，若 Interrupt handler 也去存取這些共享資料，可能會發生 Race condition。因此 kernel 需要保護這些共享資料。

在 Requirement，只需要啟動 user program (EL0) 的 Interrupt，因此，kernel 不需要處理 critical sections 的問題。

在 Elective 中，kernel 也需要啟動 Interrupt，因此需要以下操作:
1. 在執行 critical section 時短暫停止 CPU 的 Interrupt 以避免 Interrupt Handlers 的 Race Condition。
2. 限制 Interrupt Handler 使用存取共享資料的 APIs。

## Exception Level Switch
### EL2 to EL1
在樹梅派 booting 後，CPU 預設會跑在 EL2，但是我們的 kernel 需要執行在 EL1，因此，kernel 需要進行 EL2 到 EL1 的轉換。

可以使用以下的程式碼來將 EL2 轉換到 EL1，它藉由 ``hcr_el2`` 使得 EL1 執行於 AARCH64。接著，它設定 ``spsr_el2`` 與 ``elr_el2`` 使得 CPU 在 ``eret`` 後可以由 PSTATE 回到 target address。

```
    ...
    bl from_el2_to_el1
    # the next instruction runs in EL1
    ...
from_el2_to_el1:
    mov x0, (1 << 31) // EL1 uses aarch64
    msr hcr_el2, x0
    mov x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
    msr spsr_el2, x0
    msr elr_el2, lr
    eret // return to EL1
```

### EL1 to EL0
在 kernel 初始化完後，kernel 可以載入 user program 並藉由 ``eret`` 來執行 user program。

你需要在 shell 中加入一個命令來進行以下操作:
1. 載入位於 initramfs 的 user program 於特定的位址
2. 設定 ``spsr_el1`` 為 ``0x3c0``，``elr_el1`` 為 user program 的起始位址
3. 藉由設定 ``sp_el0`` 來將 user program 的 stack pointer 設定到合適的位址
4. 發起 eret 以進入 user code

### EL0 to EL1
user program 可以藉由 Exception 來回到 EL1，但是必須先建立好 Exception Vector Table，可以利用以下的 vector table 並設定 ``vbar_el1`` 為 table's address。

```
exception_handler:
  ...
.align 11 // vector table should be aligned to 0x800
.global exception_vector_table
exception_vector_table:
  b exception_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7
  b exception_handler
  .align 7

set_exception_vector_table:
  adr x0, exception_vector_table
  msr vbar_el1, x0
```

*Note: The vector table’s base address should be aligned to 0x800*

## Exception Handling
在設置好 vector table 後，載入以下 user program，該程式會藉由 ``svc`` 指令來觸發 Exception。

而 kernel 的 Exception Handler 僅需印出 ``spsr_el1``、``elr_el1`` 與 ``esr_el1`` 的內容。

```
.section ".text"
.global _start
_start:
    mov x0, 0
1:
    add x0, x0, 1
    svc 0
    cmp x0, 5
    blt 1b
1:
    b 1b
```

### Context saving
由於 user program 與 exception handler 共享相同的 general purpose registers bank，因此，必須在進入 kernel function 前儲存 user program 的狀態。

可以藉由以下的程式碼在進入 kernel 前儲存 registers，並在離開 kernel 後恢復 registers。

```
// save general registers to stack
.macro save_all
    sub sp, sp, 32 * 8
    stp x0, x1, [sp ,16 * 0]
    stp x2, x3, [sp ,16 * 1]
    stp x4, x5, [sp ,16 * 2]
    stp x6, x7, [sp ,16 * 3]
    stp x8, x9, [sp ,16 * 4]
    stp x10, x11, [sp ,16 * 5]
    stp x12, x13, [sp ,16 * 6]
    stp x14, x15, [sp ,16 * 7]
    stp x16, x17, [sp ,16 * 8]
    stp x18, x19, [sp ,16 * 9]
    stp x20, x21, [sp ,16 * 10]
    stp x22, x23, [sp ,16 * 11]
    stp x24, x25, [sp ,16 * 12]
    stp x26, x27, [sp ,16 * 13]
    stp x28, x29, [sp ,16 * 14]
    str x30, [sp, 16 * 15]
.endm

// load general registers from stack
.macro load_all
    ldp x0, x1, [sp ,16 * 0]
    ldp x2, x3, [sp ,16 * 1]
    ldp x4, x5, [sp ,16 * 2]
    ldp x6, x7, [sp ,16 * 3]
    ldp x8, x9, [sp ,16 * 4]
    ldp x10, x11, [sp ,16 * 5]
    ldp x12, x13, [sp ,16 * 6]
    ldp x14, x15, [sp ,16 * 7]
    ldp x16, x17, [sp ,16 * 8]
    ldp x18, x19, [sp ,16 * 9]
    ldp x20, x21, [sp ,16 * 10]
    ldp x22, x23, [sp ,16 * 11]
    ldp x24, x25, [sp ,16 * 12]
    ldp x26, x27, [sp ,16 * 13]
    ldp x28, x29, [sp ,16 * 14]
    ldr x30, [sp, 16 * 15]
    add sp, sp, 32 * 8
.endm

exception_handler:
    save_all
    bl exception_entry
    load_all
    eret
```

requirement 1 中的需求為:
+ required 1-1 Switch from EL2 to EL1
+ required 1-2 Add a command that can load a user program in the initramfs. Then, use eret to jump to the start address
+ required 1-3 Set the vector table and implement the exception handler
+ required 1-4 Save the user program’s context before executing the exception handler

## Core Timer Interrupt
Rpi3 的每個 CPU core 都有自己的 core timer，可以藉由以下的 system registers 來控制。
+ ``cntpct_el0``: timer's current count。
+ ``cntp_cval_el0``: compared timer count，若 ``cntpct_el0 >= cntp_cval_el0``，則中斷 CPU core。
+ ``cntp_tval_el0``: (``cntp_cval_el0 - cntpct_el0``)，可以利用它來設置 current count 之後的 expired counter。

為了開啟 timer's interrput，必須進行以下操作:
1. 設定 ``cntp_ctl_el0`` 為 1。
2. unmask 第一層 Interrupt Controller 的 timer interrupt
3. 開啟 CPU core's interrupt。

在 Requirements 中，只開啟 EL0 中的 interrupt，可以藉由在返回到 EL0 時設定 ``spsr-_el1`` 為 0 來達成。

可以由以下的程式碼來啟動 core timer's interrupt。

```
#define CORE0_TIMER_IRQ_CTRL 0x40000040

core_timer_enable:
  mov x0, 1
  msr cntp_ctl_el0, x0 // enable
  mrs x0, cntfrq_el0
  msr cntp_tval_el0, x0 // set expired time
  mov x0, 2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str w0, [x1] // unmask timer interrupt

core_timer_handler:
  mrs x0, cntfrq_el0
  msr cntp_tval_el0, x0
```

requirement 2 中的需求為開啟 core timer's interrupt，interrupt handler 需要印出從 booting 後的時間並設定 timeout 為 2 秒鐘。

*Hint: You can get the seconds after booting from the count of the timer(cntpct_el0) and the frequency of the timer(cntfrq_el0).*

## Enable Interrupt in EL1

## Rpi3’s Peripheral Interrupt

## Timer Multiplexing

## Concurrent I/O Devices Handling
