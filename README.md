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
`seek file_descriptor start/end` - to reposition read/write file offset  
`write file_descriptor /path/to/real/file size` - to write to mini-fs file data from another file  
`read file_descriptor /path/to/real/file size` - to read from mini-fs file data to another file  
