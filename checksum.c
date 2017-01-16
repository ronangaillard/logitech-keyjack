#include <stdio.h>
#define PAYLOAD_SIZE 22

int main()
{
    int i =0;
    printf("Hello, World!\n");
    
    unsigned char payload[] ={0xa2, 0x5F, 0x02, 0x01, 0x02, 0x03, 0x04, 0x58, 0x8A, 0x51, 0xEA, 0x1E, 0x40, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
    int sum = 0;
    
    for(i = 0; i < PAYLOAD_SIZE; i++)
        sum += payload[i];
        
    printf("Sum : %x\n", sum);
    printf("Checksum : %02x\n", - sum);

    return 0;
}
