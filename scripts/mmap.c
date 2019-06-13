// this is the way the x86 Linux interface
// specifies the arguments to mmap
struct mmap_arg_struct {
  unsigned long addr;
  unsigned long len;
  unsigned long prot;
  unsigned long flags;
  unsigned long fd;
  unsigned long offset;
};

// these functions refer to our assembly subroutines
extern void *sys_mmap(struct mmap_arg_struct *args);
extern int sys_munmap(void *addr, int len);

// this is the ultimate mmap wrapper
void *mmap(void *addr, int len, int prot, int flags, int fd, int offset) {
  struct mmap_arg_struct args;
  args.addr = (unsigned long) addr;
  args.len = len;
  args.prot = prot;
  args.flags = flags;
  args.fd = fd;
  args.offset = offset;

  return sys_mmap(&args);
}

// this is the ultimate munmap wrapper
int munmap(void *addr, int len) {
  return sys_munmap(addr, len);
}

