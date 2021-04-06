# lab2 note

## Goal of this lab
* implement a bootloader that loads kernel images through UART
* Understand what's initial ramdisk
* Understand what's devicetree (I do not finish this part)

### Scoring
* require1: implement UART bootloader that loads kernel images through UART
* require2: Parse New ASCII Format Cpio archive, and read file's content given file's pathname.
* elective1:
* elective2:

## Loads kernel images through UART
傳統上，當我們撰寫出新的kernel image，我們需要將他裝到SD卡之上
這樣就要做滿多實體的動作（插卡讀卡之類的），這樣不僅麻煩，也可能會讓記憶卡有所損壞
因此，我們希望能賺寫出一個bootloader和kernel，而這組合的目的是從UART讀取由電腦傳來的，真正要用來運行的kernel

### /dev/ttyUSB0
寫到這邊就回想到之前在修 Advance UNIX programming時，在UNIX系統下，terminal之類的裝置都會被註冊在/dev（各subsystem會有實作上的不同）
例如我們可以在程式中直接寫入/dev/tty0，然後內容就會直接show在terminal之上
而/dev/ttyUSB0也是。我們可以將/dev/ttyUSB0直接當成檔案來進行讀寫
所以可以用這個機制來傳輸新的kernel至板子上

### notes
傳輸的protocol沒有硬性限制，可以自己衡量要不要做錯誤偵測，例如checksum的動作
傳輸錯誤的機率其實沒有很大，但有做有保佑囉XD
基本上就是將kernel讀成binary file，然後從電腦一個byte一個byte傳輸，板子就一byte一byte接收
請注意要將收來的資料放置正確位置，然後在接收完所有資料後，branch到該address
理論上就可以正常運行了

## Initial Ramdisk
一般的OS在開機後會mount filesystem，讓user去使用
不過作業到目前為止還沒有此功能，所以需要時做簡單的Initial ramdisk

### CPIO format
[reference](https://www.systutorials.com/docs/linux/man/5-cpio/)
這次使用的架構是Cpio，簡而言之就是能夠將directories and files 包成一個架構（包成一個header）的概念
若是使用linux開發，那很容易可以將自己的檔案包成cpio格式

### note
要讓板子知道你需要mount Cpio filesystem，需要在config.txt註冊（config.txt也會被放入SD card），並assign好address
之後在撰寫kernel時，直接去該address就能access到這個filesystem了

content that you have to write to config.txt
> initramfs initramfs.cpio 0x20000000


## Bootloader Self Relocation
傳統上，在Rpi3，若kernel不放置於0x80000時，需要去config.txt特別宣告，不然可能會有問題
例如，這次作業中，我們希望first kernel放置在0x70000，然後真實的kernel放在0x80000
但假如板子幫我們預設的把first kernel放在0x80000，這樣真實kernel在填寫時就會蓋掉原本的kernel
這種情況可能造成當機
所以我們希望說，不用特別在config.txt宣告kernel擺放位置，只需要在kernel code和bootloader裡有做好機制，就能夠讓這份kernel出現在該出現的位置

### note
機制其實不難，就是bootloader一進去會先確認PC跟我期望的位置合不合
在這作業，PC（板子的機制）是0x80000，我們期望的位置是0x70000，這兩者存在差異
所以bootloader就會手動把kernel 的東西搬到0x70000中
這樣就是self relocation bootloader的精髓