# Error Message

```
Unable to handle kernel NULL pointer dereference at virtual address 0000000000000000
Mem abort info:
ESR = 0x96000045
EC = 0x25: DABT (current EL), IL = 32 bits
SET = 0, FnV = 0
EA = 0, S1PTW = 0
FSC = 0x05: level 1 translation fault
Data abort info:
ISV = 0, ISS = 0x00000045
CM = 0, WnR = 1
user pgtable: 4k pages, 39-bit VAs, pgdp=0000000042622000
[0000000000000000] pgd=0000000000000000, p4d=0000000000000000, pud=0000000000000000
Internal error: Oops: 96000045 [#1] SMP
Modules linked in: hello(O) scull(O) faulty(O)
CPU: 0 PID: 160 Comm: sh Tainted: G           O      5.15.18 #1
Hardware name: linux,dummy-virt (DT)
pstate: 80000005 (Nzcv daif -PAN -UAO -TCO -DIT -SSBS BTYPE=--)
pc : faulty_write+0x14/0x20 [faulty]
lr : vfs_write+0xa8/0x2b0
sp : ffffffc008d23d80
x29: ffffffc008d23d80 x28: ffffff8002693300 x27: 0000000000000000
x26: 0000000000000000 x25: 0000000000000000 x24: 0000000000000000
x23: 0000000040000000 x22: 000000000000000d x21: 000000558bb92a70
x20: 000000558bb92a70 x19: ffffff8002623e00 x18: 0000000000000000
x17: 0000000000000000 x16: 0000000000000000 x15: 0000000000000000
x14: 0000000000000000 x13: 0000000000000000 x12: 0000000000000000
x11: 0000000000000000 x10: 0000000000000000 x9 : 0000000000000000
x8 : 0000000000000000 x7 : 0000000000000000 x6 : 0000000000000000
x5 : 0000000000000001 x4 : ffffffc0006f0000 x3 : ffffffc008d23df0
x2 : 000000000000000d x1 : 0000000000000000 x0 : 0000000000000000
Call trace:
faulty_write+0x14/0x20 [faulty]
ksys_write+0x68/0x100
__arm64_sys_write+0x20/0x30
invoke_syscall+0x54/0x130
el0_svc_common.constprop.0+0x44/0xf0
do_el0_svc+0x40/0xa0
el0_svc+0x20/0x60
el0t_64_sync_handler+0xe8/0xf0
el0t_64_sync+0x1a0/0x1a4
Code: d2800001 d2800000 d503233f d50323bf (b900003f) 
---[ end trace 98d31923b8775a8f ]---
```

# Analysis

- From the first line, you can see it's NULL pointer issue.
- 'Modlues linked': tells what modules are related to this error
- 'CPU: 0 PID: 160 Comm: sh Tainted: G' --> tells a shell command triggered the error
- 'pc : faulty_write+0x14' --> the problemtic function is 'faulty_write' at location, 0x40
- 'Call trace:' --> we can use this call trace to see more detail of function call chain
