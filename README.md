### DESCRIPTION  
Mini file system stored in `fs.bin` file  
Mini file system can store files of unlimited capacity due to nested inodes  
Mini fs has some capacity, which is easily increased by changing FS_BLOCK_SIZE constant  
### BUILD & EXECUTE
Use CMakeLists.txt to build project  
In build directory you will find server, client binaries and kernel module:  
- `./fs/mini_fs_server port` - to run server  
- `./client/mini_fs_client address:port` - to run client  
- `./fs/module/mini-fs-module.ko` - module  
To insert module use `insmod`, to remove `rmmod`  
To checkout kernel logs use `dmesg`,  
Before running server make sure you have create character file `/dev/mini-fs`  
To create character file use `mknod /dev/mini-fs c major_number_from_logs 0`  
### COMMANDS  
`quit` - to quit mini-fs client  
`init` - to initialise mini-fs  
`ls /path/to/dir` - to show content of directory  
`mkdir /path/to/new_dir` - to create new directory  
`touch /path/to/new_file` - to create new file  
`open /path/to/file` - to open file  
`close file_descriptor` - to close file  
`seek file_descriptor pos` - to reposition read/write file offset  
`write file_descriptor /path/to/real/file size` - to write to mini-fs file data from another file  
`read file_descriptor /path/to/real/file size` - to read from mini-fs file data to another file  
### TESTS  
to launch tests run `./test/run.sh`  
### USAGE LIMITS  
approximate max fs capacity = 500 kB  
max number of files or subdirs in dir = 32  
max dir or file name length = 14 symbols  
max path length = 256 symbols  
max number of descriptors = 16  
block size = 1024 bytes  
max number of inodes = 256  
max number of blocks = 512  
