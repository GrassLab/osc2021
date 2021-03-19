# My OSC 2021

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`309552002`| `jackhong12` | `黃振宏` | jackhuang1205@gmail.com |


## Support Feature
- bootloader will relocate itself
- load kernel image via mini-uart
- parse and print all flattened device tree nodes
- a python script to interact with raspberry pi trough tty

## How to build
Due to some data are stored in different address in Qemu and Raspberry Pi, we need to specify different macros when compiling.
- for Qemu
```
$ make
```

- for Raspberry Pi 3B+
```
$ make tar=raspi3
```

## How to interact with raspberry pi
- If using Raspberry Pi, [tty] will be something like `/dev/ttyUSB0` and [baud rate] would be 115200.
- If using Qemu, [tty] will be something like `/dev/pts/0`. tty of Qemu don't need baud rate, so [buad rate] should be blank.
```
$ ./screen.py [tty] [baud rate]
```

- load kernel image
```
$ load
```

- If loads kernel image successfully, it will show content below.
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
