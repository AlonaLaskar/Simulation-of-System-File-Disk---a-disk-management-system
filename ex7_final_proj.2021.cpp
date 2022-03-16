/*
Authored by:Alona Laskar
Id:316559624
This program is a simulation of the  Disk File System:
Which is the way that the files, their names, data etc. are stored on the disk.
The program knows how to manage the data given by the user and write it correctly to the disk, the program knows
how to assert block Allocation index method.
index allocation The method says that every existing file has an index block that keeps the list of occupied blocks.
This simulation is of a small Disk File System of a small computer with a single Main Directory
 */
#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define DISK_SIZE 256


void  decToBinary(int n , char &c)
{
    // array to store binary number
    int binaryNum[8];

    // counter for binary array
    int i = 0;
    while (n > 0) {
        // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }

    // printing binary array in reverse order
    for (int j = i - 1; j >= 0; j--) {
        if (binaryNum[j]==1)
            c = c | 1u << j;
    }
}


// ============================================================================

class FsFile {

    int file_size;
    int block_in_use;
    int index_block;
    int block_size;

public:

    FsFile(int _block_size) {

        file_size = 0;
        block_in_use = 0;
        block_size = _block_size;
        index_block = -1;

    }

    int  getFileSize(){
        return file_size;
    }

    void setFileSize(int fileSize) {
        file_size = fileSize;
    }

    int getBlockInUse()  {
        return block_in_use;
    }

    void setBlockInUse(int blockInUse) {
        block_in_use = blockInUse;
    }

    int getIndexBlock() {
        return index_block;
    }

    void setIndexBlock(int indexBlock) {
        index_block = indexBlock;
    }



};

// ============================================================================

class FileDescriptor {
    string file_name;
    FsFile* fs_file;
    bool inUse;



public:

    FileDescriptor(string FileName, FsFile* fsi) {
        file_name = FileName;
        fs_file = fsi;
        inUse = true;

    }

    bool getisInUse() {
        return inUse;
    }

    void setInUse(bool inUse) {
        FileDescriptor::inUse = inUse;
    }


    string getFileName() {

        return file_name;

    }

    FsFile *getFsFile(){
        return fs_file;
    }


};

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
// ============================================================================

class fsDisk {
    FILE *sim_disk_fd;
    bool is_formated;
    // BitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if BitVector[0] == 1 , means that the
    //             first block is occupied.
    int BitVectorSize;
    int *BitVector;
    // filename and one fsFile.
    // MainDir;
    // OpenFileDescriptors --  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.
    // OpenFileDescriptors;
    vector<FileDescriptor*> MainDir ;
    vector<FileDescriptor*>OpenFileDescriptors;
    int direct_enteris;
    int block_size;
    int maxSize;
    int freeBlocks;



    // ------------------------------------------------------------------------
public:
    fsDisk() {

        sim_disk_fd = fopen(DISK_SIM_FILE , "r+");
        assert(sim_disk_fd);
        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" ,  1 , 1, sim_disk_fd);
            assert(ret_val == 1);
        }

        fflush(sim_disk_fd);
        direct_enteris = 0;
        block_size = 0;
        is_formated = false;

    }
    ~fsDisk(){
        for (int x = 0; x < MainDir.size(); ++x) {
            delete MainDir[x]->getFsFile();
            delete MainDir[x];
        }
        delete BitVector;
        fclose(sim_disk_fd);
    }

    // ------------------------------------------------------------------------
    void listAll() {//Print function
        int i ;
        for (i=0;i<MainDir.size();) {
            cout << "index: " << i << ": FileName: " <<MainDir[i]->getFileName()<<  " , isInUse: " << MainDir[i]->getisInUse()<< endl;
            i++;
        }
        char bufy;
        cout << "Disk content: '" ;
        for (i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fread(  &bufy , 1 , 1, sim_disk_fd );
            cout << bufy;

        }
        cout << "'" << endl;
    }



    // ------------------------------------------------------------------------
    void fsFormat(int blockSize){
        this->block_size=blockSize;
        this->BitVectorSize=DISK_SIZE/this->block_size;// // Caclculate the amount of blocks according to the given data.
        this->BitVector=new int[this->BitVectorSize];
        this->maxSize= this->block_size*this->block_size; // A calculation of the maximum file size according to the given data.
        //Reset each bit vector
        fill(BitVector,BitVector+BitVectorSize,0);
        this->is_formated= true;
        for(FileDescriptor* file1:MainDir){// Goes through the entire maindirectory
            delete file1->getFsFile();
            delete file1;
        }
        MainDir.clear();//delete null
        OpenFileDescriptors.clear();
        freeBlocks=BitVectorSize;// Update the number of available blocks
    }


    // ------------------------------------------------------------------------
    int CreateFile(string fileName) {
        if (this->is_formated== false){//check is the disk is formatted
            return -1;
        }

        int j=0;
        bool flag=true;
        while (j<BitVectorSize){
            if(BitVector[j]==0){//Checks if space is available
                flag= false;
                BitVector[j]=1;
                freeBlocks--;
                break;
            }
            j++;
        }
        if(flag){//error there is no free space
            return -1;
        }


        FsFile *fs_file=new FsFile(this->block_size);//create FsFile
        FileDescriptor* desc =new FileDescriptor(fileName,fs_file);
        MainDir.push_back(desc);//Data update in MainDir
        int index=MainDir.size()-1;
        MainDir[index]->getFsFile()->setIndexBlock(j);
        for (int i = 0; i < OpenFileDescriptors.size(); i++) {//Check if the cell in place i is null
            if (OpenFileDescriptors[i]==NULL){
                OpenFileDescriptors[i]=desc;
                return i;
            }

        }
        OpenFileDescriptors.push_back(desc);//Data update in OpenFileDescriptors
        return (int)OpenFileDescriptors.size()-1;

    }
    // ------------------------------------------------------------------------
    int OpenFile(string fileName) {
        int flag=0,i;
        for( i=0;i<MainDir.size();i++){
            if(MainDir[i]->getFileName()==fileName){//Checks if the file already exists
                if (MainDir[i]->getisInUse()== true){//check if the file is already open
                    return -1;
                }
                MainDir[i]->setInUse(true);
                break;
            }
        }
        for (int  k= 0; k < MainDir.size();k++) {//
            if(MainDir[k]->getFileName()==fileName){
                flag=1;
            }
        }
        if(flag==0){//check if the file isn't found
            return -1;
        }
        for(int h=0;h<OpenFileDescriptors.size();h++){
            if(OpenFileDescriptors[h]==NULL){
                OpenFileDescriptors[h]=MainDir[i];
                return h;
            }
        }
        OpenFileDescriptors.push_back(MainDir[i]);//Updating data
        return OpenFileDescriptors.size()-1;
    }
    // ------------------------------------------------------------------------
    string CloseFile(int fd) {

        if((fd>=0 && fd<=OpenFileDescriptors.size()-1)&&  OpenFileDescriptors[fd]==NULL){//Not in range or null

            return "-1";
        }
        OpenFileDescriptors[fd]->setInUse(false);
        string nameFile=OpenFileDescriptors[fd]->getFileName();
        OpenFileDescriptors[fd]=NULL;
        return nameFile;
    }

    // ------------------------------------------------------------------------
    int WriteToFile(int fd, char *buf, int len ) {
        if(is_formated== false){//Check whether it has been formated
            return -1;
        }

        if((OpenFileDescriptors[fd]->getFsFile()->getFileSize()+len)>maxSize)
        {
            return-1;
        }
        if (OpenFileDescriptors[fd]==NULL){//Check if the instead is OpenFileDescriptors[fd]is null
            return -1;
        }
        int off=(OpenFileDescriptors[fd]->getFsFile()->getFileSize())%this->block_size;
        int neededBlock=((len+off-this->block_size)/this->block_size);

        if(off==0){
            neededBlock=len/this->block_size;
        }
        if (neededBlock>this->freeBlocks){//check if there is enough match place
            return -1;
        }
        char temp [this->block_size];
        fseek(sim_disk_fd,OpenFileDescriptors[fd]->getFsFile()->getIndexBlock()*block_size,SEEK_SET);//Sets the position indicator associated with the stream to a new position
        fread(temp,sizeof(char ) ,this->block_size,sim_disk_fd);//Read block of data from sim_disk_fd
        int thisIndex=(OpenFileDescriptors[fd]->getFsFile()->getFileSize())/block_size;
        int freeBlocki;
        for (int i = 0; i < len; i++) {
            if(off==0)
            {
                int j=0;
                bool flag=true;
                while (j<BitVectorSize){
                    if(BitVector[j]==0){
                        flag= false;
                        BitVector[j]=1;
                        freeBlocks--;
                        break;
                    }
                    j++;
                }
                if (flag){
                    freeBlocki=-1 ;
                    return -1;
                }
                else{
                    freeBlocki= j;
                }

                decToBinary(freeBlocki,temp[thisIndex]);
                int temp2=OpenFileDescriptors[fd]->getFsFile()->getBlockInUse()+1;
                OpenFileDescriptors[fd]->getFsFile()->setBlockInUse(temp2);

            }
            fseek(sim_disk_fd,temp[thisIndex]*block_size+off,SEEK_SET);
            fwrite(&buf[i],sizeof(char),1,sim_disk_fd);//write block of data from sim_disk_fd
            off++;
            if(off==block_size){
                thisIndex++;
                off=0;
            }

        }
        OpenFileDescriptors[fd]->getFsFile()->setFileSize(OpenFileDescriptors[fd]->getFsFile()->getFileSize()+len);
        fseek(sim_disk_fd,OpenFileDescriptors[fd]->getFsFile()->getIndexBlock()*block_size,SEEK_SET);// Goes to the place of a block index
        fwrite(temp,sizeof (char),block_size,sim_disk_fd);//write block of data from sim_disk_fd
        return 0;

    }
    //---------------------------------------------------
    int findFileDiscriptor(string FileName){//The method looks for a file name in MainDir
        int temp=0, i=0;
        while (i<MainDir.size()){
            if(MainDir[i]->getFileName()==FileName){
                temp=1;
                return i;
            }
            i++;
        }
        return -1;
    }

    // ------------------------------------------------------------------------
    int DelFile( string FileName ) {
        if (is_formated== false){//check if formated
            return -1;
        }
        int fd= findFileDiscriptor(FileName);
        char temp [this->block_size];
        fseek(sim_disk_fd,MainDir[fd]->getFsFile()->getIndexBlock()*this->block_size,SEEK_SET);
        fread(temp,sizeof(char ) ,this->block_size,sim_disk_fd);//read block of data from sim_disk_fd
        char emptyString[this->block_size];
        for (int i = 0; i < this->block_size; i++) {
            emptyString[i]='\0';
        }
        for (int j = 0; j < MainDir[fd]->getFsFile()->getBlockInUse(); j++) {
            fseek(sim_disk_fd,temp[j]*this->block_size,SEEK_SET);
            fwrite(emptyString,1,this->block_size,sim_disk_fd);//write block of data from sim_disk_fd
            this->freeBlocks++;//Increases by 1 the freeBlocks
            BitVector[temp[j]]=0;
        }

        int location= MainDir[fd]->getFsFile()->getIndexBlock();
        fseek(sim_disk_fd,this->block_size*location,SEEK_SET);
        fwrite(emptyString,1,this->block_size,sim_disk_fd);//write block of data from sim_disk_fd
        this->freeBlocks++;//Increases by 1 the freeBlocks
        BitVector[location]=0;//Initializes the array BitVector[location]in 0
        delete MainDir[fd]->getFsFile();
        delete MainDir[fd];
        MainDir.erase(MainDir.begin()+fd);
        return 0;
    }
    // ------------------------------------------------------------------------
    int ReadFromFile(int fd, char *buf, int len ) {
        if(is_formated== false){//Check whether it has been formated
            return -1;
        }

        if(OpenFileDescriptors[fd]->getFsFile()->getFileSize()<len)
        {
            return-1;
        }
        if ((fd<0 || fd>OpenFileDescriptors.size()) ||OpenFileDescriptors[fd]==NULL){//Check whether fd is in the range or equal to null in OpenFileDescriptors[fd]
            return -1;
        }
        int off=0,thisIndex=0;
        char temp[this->block_size];
        int location=OpenFileDescriptors[fd]->getFsFile()->getIndexBlock();
        fseek(sim_disk_fd,location*this->block_size,SEEK_SET);
        fread(temp,1,this->block_size,sim_disk_fd);//read block of data from sim_disk_fd

        for (int a = 0; a < len; a++) {
            fseek(sim_disk_fd,(temp[thisIndex]* this->block_size)+off,SEEK_SET);
            fread(&buf[a],1,1,sim_disk_fd);//read block of data from sim_disk_fd
            off++;
            if(off==this->block_size){
                off=0;
                thisIndex++;
            }

        }
        buf[len]='\0';
        return 0;

    }


};

int main() {
    int blockSize;
    int direct_entries;
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
            delete fs;
            exit(0);
            break;

            case 1:  // list-file
            fs->listAll();
            break;

            case 2:    // format
            cin >> blockSize;
            fs->fsFormat(blockSize);
            break;

            case 3:    // creat-file
            cin >> fileName;
            _fd = fs->CreateFile(fileName);
            cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 4:  // open-file
            cin >> fileName;
            _fd = fs->OpenFile(fileName);
            cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 5:  // close-file
            cin >> _fd;
            fileName = fs->CloseFile(_fd);
            cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;

            case 6:   // write-file
            cin >> _fd;
            cin >> str_to_write;
            fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) );
            break;

            case 7:    // read-file
            cin >> _fd;
            cin >> size_to_read ;
            fs->ReadFromFile( _fd , str_to_read , size_to_read );
            cout << "ReadFromFile: " << str_to_read << endl;
            break;

            case 8:   // delete file
            cin >> fileName;
            _fd = fs->DelFile(fileName);
            cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
            break;
            default:
                break;
        }
    }

}