.global sys_mmap 
sys_mmap:
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %ebx
  movl $90, %eax
  int $128
  popl %ebp
  ret

.global sys_munmap
sys_munmap:
  pushl %ebp
  movl %esp, %ebp
  movl 8(%ebp), %ecx
  movl 12(%ebp), %ebx
  movl $91, %eax
  int $128
  popl %ebp
  ret

