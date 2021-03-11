## DESCRIPTION  
Mini file system stored in `fs.bin` file  
## COMMANDS  
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
## TESTS  
to launch tests run `./test/run.sh`  
## USAGE LIMITS  
approximate max fs capacity = 320 kB  
max number of files or subdirs in dir = 32  
max dir or file name length = 14 symbols  
max path length = 256 symbols  
max number of descriptors = 16  
block size = 512 bytes  
max number of inodes = 128  
max number of blocks = 256  
