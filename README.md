### DMA Memory Allocator

A custom dynamic memory allocator written in C using memory-mapped space (`mmap`) and bitmap tracking. This project demonstrates allocation, deallocation, and internal fragmentation tracking using multithreading.

---

### 📁 Files

- `dma.c`: Implementation of the dynamic memory allocator.
- `dma.h`: Header file declaring public DMA functions.
- `app.c`: Multithreaded test program using the allocator.
- `libdma.a`: Precompiled static library version of `dma.c`.
- `dma.o`: Compiled object file for `dma.c`.
- `Makefile`: Compilation instructions.

---

### 🧠 Features

- Custom memory allocator with bitmap-based tracking
- Allocation and deallocation with thread safety
- Tracks internal fragmentation
- Page-level memory inspection
- Allocation block reporting
- Uses `mmap()` for memory management
- Thread-safe with `pthread` mutexes

---

### ⚙️ Compilation

Use the provided `Makefile` to compile everything.

```bash
make
```

This will:

- Compile dma.c into libdma.a

- Compile and link app.c with the library

- Generate the app executable

### 🚀 Running the Program
To run the multithreaded memory allocator demo:

```bash
./app
```
This will:

- Initialize the memory segment

- Launch 3 threads

- Perform various memory allocations/frees

- Print bitmap and block status

- Report internal fragmentation

### 🧵 Multithreading
Each thread in app.c does the following:

- Allocates blocks of various sizes

- Frees some of them

- Writes values to allocated blocks (for testing)

- Prints bitmap and block status

- Reports internal fragmentation in that thread

### 🧪 Diagnostic Tools
- dma_print_bitmap(): Shows the current allocation bitmap.

- dma_print_blocks(): Prints all blocks as Free (F) or Allocated (A).

- dma_print_page(int pno): Prints the hexadecimal content of a page.

- dma_give_intfrag(): Returns the current internal fragmentation value.

### 🔢 Parameters
- m in dma_init(m) must be between 14 and 22 (inclusive).

- size in dma_alloc(size) is the number of bytes to allocate.

- All allocation sizes are rounded and aligned to match internal block sizes.

### 🧼 Cleanup
To clean all compiled files:
```bash
make clean
```

