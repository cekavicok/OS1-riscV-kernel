# OS1 RISC-V Kernel

A small operating system kernel for **RISC-V (RV64IMA)**, written in **C++** and assembly and running on the **QEMU** emulator. Built as part of the *Operating Systems 1* course at the School of Electrical Engineering, University of Belgrade (Project 2026).

The kernel provides preemptive time-sliced threads, semaphores, thread sleeping, a custom memory allocator, and an interrupt-driven console, exposed through both a C and a C++ API.

---

## Project Structure



```
.
├── h/                        # Headers
│   ├── syscall_c.h           # C API (mem_alloc, thread_create, sem_open, ...)
│   ├── syscall_cpp.h         # C++ API (Thread, Semaphore, PeriodicThread, Console)
│   ├── riscV.h               # CSR access, trap routine, shutdown
│   ├── PCB.h                 # Thread control block
│   ├── scheduler.h           # Scheduler (ready + sleep queues)
│   ├── list.h                # Templated linked list
│   ├── Ksemaphore.h          # Kernel semaphore
│   ├── KBuffer.h             # Circular buffer used by the console
│   ├── Kconsole.h            # Kernel console
│   └── MemoryAllocator.h     # Memory allocator
├── src/                      # Implementation
│   ├── main.cpp              # Kernel initialization, starts userMain
│   ├── riscV.cpp             # handleTrap: system calls and interrupts
│   ├── PCB.cpp               # Threads, dispatch, exit, cleanup of finished threads
│   ├── scheduler.cpp         # FIFO scheduling, sleep, tick
│   ├── Ksemaphore.cpp
│   ├── KBuffer.cpp
│   ├── Kconsole.cpp
│   ├── MemoryAllocator.cpp
│   ├── syscall_c.cpp         # ecall wrappers
│   ├── syscall_cpp.cpp       # C++ classes on top of the C API + new/delete
│   ├── trapRoutine.s         # Trap entry (saves x0–x31)
│   ├── contextSwitch.s       # Context switch (ra, sp)
│   └── registerUtil.S        # pushRegisters / popRegisters (x3–x31)
├── lib/                      # Provided libraries (hw.h, console.h, hw.lib, mem.lib, console.lib)
├── test/                     # Public tests (userMain, Threads, ConsumerProducer, Sleep, ...)
├── kernel.ld                 # Linker script (entry _entry at 0x80000000)
└── Makefile
```

---

## Getting Started

### Requirements

- `riscv64-linux-gnu` toolchain (`gcc`, `g++`, `ld`)
- `qemu-system-riscv64`
- `make`
- (optional) `gdb-multiarch` for debugging

### Build and Run

```bash
make clean      # required after header changes (avoids stale .o files)
make qemu       # builds and runs the kernel in QEMU
```

The kernel is compiled with `-nostdlib -ffreestanding -fno-rtti -fno-threadsafe-statics` and linked against `hw.lib`, `mem.lib` and `console.lib`.

### Selecting Tests

1. In the test code, set the `LEVEL_N_IMPLEMENTED` macros to `1` for every completed task (N is the task number from the assignment).
2. `userMain()` is called from `main` after kernel initialization.
3. After startup, a test is selected by entering the corresponding digit in the console.

> Note: for test 7 it is expected that the process does **not** terminate regularly.

### Debugging

Start QEMU with a GDB server (e.g. `make qemu-gdb`, if your Makefile has such a target) and connect to `localhost:26000` from GDB or CLion (remote GDB).

---

## Architecture

The kernel is organized in four layers:

| Layer | Files | Role |
|---|---|---|
| C API | `syscall_c.h/.cpp` | `ecall` wrappers passing the call code and arguments through `a0–a4` |
| C++ API | `syscall_cpp.h/.cpp` | `Thread`, `Semaphore`, `PeriodicThread`, `Console` classes |
| Kernel | `PCB`, `scheduler`, `riscV`, `Ksemaphore`, `KBuffer`, `Kconsole`, `MemoryAllocator` | The actual kernel logic |
| Assembly | `trapRoutine.s`, `contextSwitch.s`, `registerUtil.S` | Trap entry, context switching |

### System Calls

| Code | Call |
|---|---|
| `0x01` / `0x02` | `mem_alloc` / `mem_free` |
| `0x11` / `0x12` / `0x13` | `thread_create` / `thread_exit` / `thread_dispatch` |
| `0x21`–`0x26` | `sem_open`, `sem_close`, `sem_wait`, `sem_signal`, `sem_wait_n`, `sem_signal_n` |
| `0x31` | `time_sleep` |
| `0x41` / `0x42` | `getc` / `putc` |

---

## Implementation

### Threads and Context Switching

- **Individual stack per thread** (variant A): every thread owns a stack of `DEFAULT_STACK_SIZE` bytes.
- The `PCB` stores only a minimal context `{ra, sp}`; the remaining registers are saved on the thread's stack in `trapRoutine.s` (256 B frame, x0–x31).
- `contextSwitch.s` saves `ra` and `sp` of the old thread and loads them for the new one.
- Thread startup: `contextSwitch` → `wrapper()` → `setSppSpie()` (sets SPP/SPIE and `sepc`) → `sret` → `userBodyWrapper()` → `body(arg)` → `thread_exit()`.
- Finished threads are put on a `done` list and deleted in `clean_done()` (never the running thread).
- An **idle thread** runs when no other thread is ready.

### Scheduling

- **FIFO (FCFS)** ready queue with **time slicing** (`DEFAULT_TIME_SLICE`) driven by the timer interrupt.
- Privileged (kernel) threads have an effectively infinite time slice.
- `time_sleep` places a thread on a separate sleep queue; `Scheduler::tick()` decrements counters on every timer interrupt and wakes threads when they expire.

### Semaphores

- `Ksemaphore` supports `wait(n)` / `signal(n)` and closing (`close()` wakes all blocked threads with return value `-1`).
- Blocked threads are kept in a FIFO list of `Waiter` records (thread + requested token count).
- Critical sections are protected by masking interrupts (saving and restoring `sstatus`).

### Console

- `Kconsole` uses two circular buffers (`KBuffer`): output and input.
- **Output**: `putc` enqueues a character, and a dedicated kernel *writer* thread drains it to the console device.
- **Input**: the external console interrupt (PLIC `claim`/`complete`) fills the input buffer via `try_put`, with a cap on characters handled per interrupt.
- `flush()` waits for the output buffer to drain on shutdown.

### Memory Allocator

- **First-fit** over a doubly linked list of free blocks.
- Blocks are split on allocation and **coalesced with both neighbours** on free.
- Sizes are rounded up to `MEM_BLOCK_SIZE`.
- The internal `List<T>` allocates its nodes through this allocator, with no dependency on a standard library.

### Trap and Exception Handling (`riscV::handleTrap`)

| `scause` | Handling |
|---|---|
| `ecall` (user / supervisor) | Dispatches the system call, return value written to `a0` of the saved frame |
| Software interrupt (timer) | `tick()`, time-slice accounting, possible context switch |
| External interrupt | Console via PLIC |
| Illegal instruction / read / write | Prints a message and shuts the system down |

In branches that call `dispatch()`, `sepc` and `sstatus` are kept in `volatile` locals so they survive the context switch.

---

## Tests

The `test/` directory contains the public tests:

- `Threads_C_API_test`, `Threads_CPP_API_test` – thread creation and switching (C and C++ API)
- `ConsumerProducer_C_API_test`, `ConsumerProducer_CPP_API_test`, `ConsumerProducer_CPP_Sync_API_test` – producer/consumer over a bounded buffer using semaphores
- `ThreadSleep_C_API_test` – thread sleeping
- `System_Mode_test` – register preservation across system calls in system mode
- `printing.cpp/.hpp` – print helpers guarded by a simple spinlock

---

## Known Limitations

- Scheduling is plain FIFO without priorities (unless the priority modification is enabled).
- PCBs and semaphores are allocated dynamically through the custom allocator, so limits depend only on heap size.
- The kernel targets QEMU only (single core, no virtual memory).

---

## Author

**Gordan Djordjevic** – Computer Engineering and Informatics, School of Electrical Engineering, University of Belgrade.

## License

This project was built for educational purposes as part of a university course. Provided files (`lib/`, public tests) belong to the course authors.
