//
//  main.cpp
//  AESEncrytionSequential
//
//  Copyright (c) 2016 TusharaSankaranArakkan. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <omp.h>


//key length (128 bit or 192 bit or 256 bit)
int keyLength = 0;
//total word count
int totalWords = 0;
//total rounds
int totalRounds = 0;
//total columns fixed to be 4 for 128 bit, 192 bit, 256 bit
int totalColumn = 4;

//input - text to be encrypted
unsigned char input[16];

//output - encrypted text
unsigned char* result;


//key - 128 bit (4x4), 192 bit(6x4), 256 bit (8x4)

//stores round key - 128 bit 44 words
unsigned char roundKey[240];

unsigned char key[16] = {0x00  ,0x01  ,0x02  ,0x03  ,0x04  ,0x05  ,0x06  ,0x07  ,0x08  ,0x09  ,0x0a  ,0x0b  ,0x0c  ,0x0d  ,0x0e  ,0x0f};





int getSBoxValue(int num)
{
    int sbox[256] =   {
        //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F
    return sbox[num];
}


// The round constant word array, Rcon[i], contains the values given by
// x to th e power (i-1) being powers of x (x is denoted as {02}) in the field GF(28)
// Note that i starts at 1, not 0).
int roundConstant[255] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };


void keyExpansion() {
    int bytePos=0;
    unsigned char compute[4];
    //first four words in round key will be same as key input
    for(;bytePos<totalWords*4;bytePos++) {
        roundKey[bytePos] = key[bytePos];
    }
    bytePos = totalWords;
    
    //computing the remaining 40 words
    //for(;i<totalColumn * (1+totalRounds);i++) {
    for(;bytePos<44;bytePos++) {
        for(int j=0;j<4;j++) {
            compute[j] = roundKey[(bytePos-1) * 4 + j];
        }
        if(bytePos%totalWords==0) {
            
            //performing one byte left circular rotation
            int temp = compute[0];
            compute[0] = compute[1];
            compute[1] = compute[2];
            compute[2] = compute[3];
            compute[3] = temp;
            
            //performing byte sustitution for each byte
            compute[0]=getSBoxValue(compute[0]);
            compute[1]=getSBoxValue(compute[1]);
            compute[2]=getSBoxValue(compute[2]);
            compute[3]=getSBoxValue(compute[3]);
            
            //xor the leftmost byte with round constant
            compute[0] = compute[0] ^ roundConstant[bytePos/totalWords];
        }
        roundKey[bytePos*4+0] = roundKey[(bytePos-totalWords)*4+0] ^ compute[0];
        roundKey[bytePos*4+1] = roundKey[(bytePos-totalWords)*4+1] ^ compute[1];
        roundKey[bytePos*4+2] = roundKey[(bytePos-totalWords)*4+2] ^ compute[2];
        roundKey[bytePos*4+3] = roundKey[(bytePos-totalWords)*4+3] ^ compute[3];
    }
}

void addRoundKey(int roundNo, unsigned char inputStateArray[4][4]) {
    int val = roundNo * totalColumn * 4;

    for(int i=0;i<4;i++)
    {
        for(int j=0;j<4;j++)
        {
            int indexVal = val + i * totalColumn + j;
            inputStateArray[j][i] = inputStateArray[j][i] ^ roundKey[indexVal] ;
        }
    }
    
}
void byteSubstitution(unsigned char inputStateArray[4][4]) {
    for(int i=0;i<totalWords;i++) {
        for(int j=0;j<4;j++) {
            inputStateArray[i][j] = getSBoxValue(inputStateArray[i][j]);
        }
    }
    
}
void rowShifting(unsigned char inputStateArray[4][4]) {
    
    //no changes to first row
    
    //circularly shifting second row by 1 bytes to left
    char temp=inputStateArray[1][0];
    inputStateArray[1][0]=inputStateArray[1][1];
    inputStateArray[1][1]=inputStateArray[1][2];
    inputStateArray[1][2]=inputStateArray[1][3];
    inputStateArray[1][3]=temp;
    
    //circularly shifting third row by 2 bytes to left
    temp=inputStateArray[2][0];
    inputStateArray[2][0]=inputStateArray[2][2];
    inputStateArray[2][2]=temp;
    temp=inputStateArray[2][1];
    inputStateArray[2][1]=inputStateArray[2][3];
    inputStateArray[2][3]=temp;
    
    //circularly shifting fourth row by 3 bytes to left
    temp=inputStateArray[3][0];
    inputStateArray[3][0]=inputStateArray[3][3];
    inputStateArray[3][3]=inputStateArray[3][2];
    inputStateArray[3][2]=inputStateArray[3][1];
    inputStateArray[3][1]=temp;
}

// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

void columnMixing(unsigned char inputStateArray[4][4]) {
    int i;
    unsigned char Tmp,Tm,t;
    for(i=0;i<4;i++)
    {
        t=inputStateArray[0][i];
        Tmp = inputStateArray[0][i] ^ inputStateArray[1][i] ^ inputStateArray[2][i] ^ inputStateArray[3][i] ;
        Tm = inputStateArray[0][i] ^ inputStateArray[1][i] ; Tm = xtime(Tm); inputStateArray[0][i] ^= Tm ^ Tmp ;
        Tm = inputStateArray[1][i] ^ inputStateArray[2][i] ; Tm = xtime(Tm); inputStateArray[1][i] ^= Tm ^ Tmp ;
        Tm = inputStateArray[2][i] ^ inputStateArray[3][i] ; Tm = xtime(Tm); inputStateArray[2][i] ^= Tm ^ Tmp ;
        Tm = inputStateArray[3][i] ^ t ; Tm = xtime(Tm); inputStateArray[3][i] ^= Tm ^ Tmp ;
    }
    
}

void encrypt(unsigned char inputStateArray[4][4]) {
    //first step is to perform key expansion
    keyExpansion();
    //second step is the xor the input and first 4 words of the key
    addRoundKey(0, inputStateArray);
    //third step is to start 10 rounds
    for(int i=1;i<=totalRounds;i++) {
        byteSubstitution(inputStateArray);
        rowShifting(inputStateArray);
        if(i!=totalRounds)
        columnMixing(inputStateArray);
        addRoundKey(i, inputStateArray);
    }
    
  /*  for(int i=0;i<4;i++) {
        for(int j=0;j<4;j++) {
            
            printf("%02x ",  inputStateArray[j][i] );
            
        }
        
        printf("\n");
    }*/
}

void createStateArray(int blockcount, int filesize, unsigned char textTemp[],
                      unsigned char inputStateArray[4][4]){
    int blockcounter = blockcount * 16;
    //convert the 1D input to 2D input state array
  //  printf("Block counter %d", blockcounter);
    for(int i=0;i<totalWords;i++) {
        for(int j=0;j<4;j++) {
            if(blockcounter < filesize) {
                inputStateArray[j][i] = textTemp[blockcounter];
                blockcounter++;
            }
        }
    }
  /*
    printf("\nNormal Text U -------------------------\n");
    
    for(int i=0;i<totalWords;i++) {
        for(int j=0;j<4;j++) {
            
               printf("%02x ",  inputStateArray[j][i] );
            
        }
        
        printf("\n");
    }
    */
    
}

void createOutputArray(int blockcount, int filesize, unsigned char output[][16],  unsigned char inputStateArray[4][4]){
    int blockcounter = blockcount * 16;
  //  printf("Block counter %d", blockcounter);
    //converting the result into 1D
    for(int i=0;i<totalWords;i++) {
        for(int j=0;j<4;j++) {
            if(blockcounter < filesize) {
                output[blockcount][4* i + j] = inputStateArray[j][i] ;
               // printf("%02x %02x ", output[blockcounter] ,inputStateArray[j][i] );
                
                blockcounter ++;
            }
        }
    }
    
   //  result = output;
 /*
    printf("\nENCRYPT Text U -------------------------\n");
    
    for(int i=0;i<totalWords;i++) {
        for(int j=0;j<4;j++) {
            
            printf("%02x ",  inputStateArray[j][i] );
            
        }
        
        printf("\n");
    }
    
    
    result = output;*/
    
  /*  printf("\nResult Text -------------------------\n %d", blockcounter);
    for(int i=0;i< 16 ;i++)
    {
        printf("%02x ",output[blockcount][i]);
    }
    printf("\n");*/
    
}
#include "../AESDecryptionSequential/aesdecryption.cpp"

int main(int argc, const char * argv[]) {
    FILE *fw = fopen("decrypted.txt","w");
    double time_initial = omp_get_wtime();
    unsigned char* encrypt_result;
    keyLength=128;
    //calculating the number of words from key length
    totalWords = keyLength/32;
    //calculating the number of rounds
    totalRounds = totalWords + 6;
    //get the plain text from user (for testing purpose taking readymade values) - 128 bit

    int filesize;
    FILE *ft = fopen("data.txt","r"); //Input file data
    fseek(ft,0L,SEEK_END);
    filesize = ftell(ft) - 1;
    rewind(ft);
    printf("FileSize %d", filesize);
    unsigned char textTemp[filesize];
  
    for(int fcount = 0;fcount < filesize; fcount += 1) {
        unsigned char c=0x00000000;
        void *t = &c;
        int sz;
        sz = fread(t,1,1,ft);
        unsigned char temp = sz?c:0x00;
        textTemp[fcount] = temp;
       // printf(" %c ", temp);
        c=0x00000000;
        
    }
  
    int total_blocks = (filesize / 16);
    int block_count = 0;
    int blockcounter;
    int tid, tids;
     omp_set_num_threads(1);
    printf("Number of threads %d", omp_get_num_threads);
    unsigned char output[total_blocks][16];
    //input state array
    unsigned char inputStateArray[4][4];
    int j;
    #pragma omp parallel private(inputStateArray, block_count, tid,tids, output) shared(textTemp, filesize, total_blocks)
    {
        block_count = tid = omp_get_thread_num();
        tids = omp_get_num_threads();
        
        printf("Number of thread %d %d", tids, tid);
        double begin_time = omp_get_wtime();
      //  printf("\nTime taken is: (%f)\n" , time_final);
       
        for(block_count = omp_get_thread_num(); block_count < total_blocks; block_count+=tids) {
            //blockcounter  = block_count * 16;
            
            createStateArray(block_count, filesize, textTemp, inputStateArray);
            encrypt(inputStateArray);
            createOutputArray(block_count, filesize, output, inputStateArray);
           // block_count += tids;
        }
        
        double time_final = omp_get_wtime();
        printf("\nTime taken before(%f) after: (%f) sub: (%f)\n" , begin_time, time_final, time_final - begin_time);
    }
    fclose(ft);
    double over_time_final = omp_get_wtime();
    printf("\nTime taken is: (%f)\n" , over_time_final - time_initial);
    
  /*
    printf("\nCipher Text -------------------------\n");
    for(int i=0;i< filesize ;i++)
    {
        printf("%02x ",result[i]);
    }
    printf("\n");
    
    printf("\nCipher Text -------------------------\n");
    for(int i=16;i< 32 ;i++)
    {
        printf("%02x ",result[i]);
    }
    printf("\n");

    fclose(ft);
    double time_final = omp_get_wtime();
    printf("\nTime taken is: (%f)\n" , time_final - time_initial);
    //    result = decrypt_block(keyTemp, encrypt_result, output);
  /*  unsigned char decrypt_output[filesize];
    unsigned char* decrypt_result;
    block_count = 0;
    while(block_count < total_blocks) {
        //blockcounter  = block_count * 16;
        decrypt_result = decrypt_block(key, result, decrypt_output, block_count);
        block_count ++;
    }
    
    printf("\nNormal Text -------------------------\n");
    for(int i=0;i< filesize ;i++)
    {
        printf("%02x ",decrypt_result[i]);
    }
    printf("\n");
    
    void* l;
    for(int i=0;i<filesize;i++) {
        l = &decrypt_result[i];
        fwrite(l,1,1,fw);
    }
    printf("Out of the for loop"); */
    fclose(fw);

    return 0;
}



