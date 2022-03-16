Simulation of System File Disk - a disk management system
==Description==

This program is a simulation of the  Disk File System:
Which is the way that the files, their names, data etc. are stored on the disk.
The program knows how to manage the data given by the user and write it correctly to the disk, the program knows
how to assert block Allocation index method.
index allocation The method says that every existing file has an index block that keeps the list of occupied blocks.
This simulation is of a small Disk File System of a small computer with a single Main Directory.
The size of the disk is pre-defined to be 256 characters (And is basically a text file).
All of the files created during the program are all store in the Main Directory.
functions====
0) Exit - Close terminal and finish.
1) listAll - Print all the data of the disk, including the files and their open / closed status.
2) FsFormat - Format the disk: You will need to enter one value:
A) Size of a block.
NOTICE: !! YOU MUST FORMAT THE DISK BEFORE OPERATING ON IT !!
3) CreateFile - Create a file: You will need to enter the name of the file you wish to create.
4) OpenFile - If a file is closed, you can open it with this function by entering the file name you want to open.
5) CloseFile - Close an opened file, enter the fd of the file you wish to close.
6) WriteToFile - Write to a file: You will need to  A) FD of a file.
  B) String to write.
7) ReadFromFile - Read from a file: You will need to enter additional data:
A) FD of a file.
    B) Length to read.
8) DeleteFile - Delete a file, enter the name of the file you wish to delete.
9)findFileDiscriptor-looks for a file name in MainDir
==Program Files==
main.cpp
==How to compile?==
compile:g++ ex7_final_proj.2021.cpp -o ex7_final_proj.2021
run: ./ex7_final_proj.2021
