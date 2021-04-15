# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`309552002`| `jackhong12` | `黃振宏` | jackhuang1205@gmail.com |

## TODO
- [ ] switch from el2 to el1
- [ ] Add a command that can load a user program in the initramfs. Then, use eret to jump to the start address.

## Support Feature
- bootloader will relocate itself
- load kernel image via mini-uart
- parse and print all flattened device tree nodes
- a python script to interact with raspberry pi trough tty
- startup allocator
- dynamic allocator
- buddy system

## How to build
```
$ make
```

## How to interact with raspberry pi
- Load kernel automatically.
```
$ ./screen.py -load /dev/ttyUSB[num]
```

- If load kernel image successfully, it will show content below.
```
+========================+
|       kernel info      |
+========================+
address: 0x80000
size: 0x2730
start sending:
[========================================] 100%

+========================+
|       osdi shell       |
+========================+
$

```

- When type `help`, it will list all support commands.
```
$ help
```

## ttyUSB permission
```
$ sudo usermod -aG dialout $USER
```
