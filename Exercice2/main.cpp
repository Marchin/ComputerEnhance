#include "main.h"

// Based on 8086 spec: https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

void pushText(Buffer* pBuffer, char* text, int textLength) {
    char* ptr = (char*)pBuffer->ptr + pBuffer->currSize;
    int bufferSize = pBuffer->maxSize - pBuffer->currSize;
    // Remove 0 end
    pBuffer->currSize += textLength - 1;
    strncpy_s(ptr, bufferSize, text, bufferSize);
}

void getWideRegName(Buffer* pBuffer, int rm) {
    
    switch (rm) {
        case 0b00000000: {
            char instrBase[] = "ax";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000001: {
            char instrBase[] = "cx";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000010: {
            char instrBase[] = "dx";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000011: {
            char instrBase[] = "bx";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000100: {
            char instrBase[] = "sp";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000101: {
            char instrBase[] = "bp";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000110: {
            char instrBase[] = "si";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000111: {
            char instrBase[] = "di";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
    }
}

void getShortRegName(Buffer* pBuffer, int rm) {
    int rmv0 = (pBuffer->currSize) > 0 ? 1 : 0;
    char* ptr = (char*)pBuffer->ptr + pBuffer->currSize - rmv0;
    int bufferSize = pBuffer->maxSize - pBuffer->currSize;
    
    switch (rm) {
        case 0b00000000: {
            char instrBase[] = "al";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000001: {
            char instrBase[] = "cl";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000010: {
            char instrBase[] = "dl";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000011: {
            char instrBase[] = "bl";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000100: {
            char instrBase[] = "ah";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000101: {
            char instrBase[] = "ch";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000110: {
            char instrBase[] = "dh";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000111: {
            char instrBase[] = "bh";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
    }
}

void getEffectiveAddress(Buffer* pBuffer, int rm, int mod, int lowDisp, int highDisp) {
    char* ptr = (char*)pBuffer->ptr + pBuffer->currSize;
    int bufferSize = pBuffer->maxSize;
    
    switch (rm) {
        case 0b00000000: {
            char instrBase[] = "[bx + si";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000001: {
            char instrBase[] = "[bx + di";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000010: {
            char instrBase[] = "[bp + si";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000011: {
            char instrBase[] = "[bp + di";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000100: {
            char instrBase[] = "[si";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000101: {
            char instrBase[] = "[di";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b00000110: {
            if (mod == 0) {
                int value = (highDisp << 8) + lowDisp;
                char instrBase[6];
                _itoa_s(value, instrBase, 10);
                pushText(pBuffer, instrBase, sizeof(instrBase));
            } else {
                char instrBase[] = "[bp";
                pushText(pBuffer, instrBase, sizeof(instrBase));
            }
        } break;
        case 0b00000111: {
            char instrBase[] = "[bx";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
    }
    
    if (mod == 0b01) {
        
    }
}

int main(int argc, char** argv) {
    // These intructions are displaced to move out the configurable bits.
    const int RmRMov = 0b00100010;
    FILE* pFileRead;
    
    if (argc < 2) {
        printf("File not specified.");
        return -1;
    }
    
    fopen_s(&pFileRead, argv[1], "rb");
    
    if (pFileRead && pFileRead) {
        printf("bits 16\n");
        while (!feof(pFileRead)) {
            u8 tempBuffer;
            Buffer buffer = {};
            buffer.maxSize = 24;
            buffer.ptr = malloc(buffer.maxSize);
            
            fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
            if ((tempBuffer >> 2) == RmRMov) {
                char mov[] = "\nmov ";
                pushText(&buffer, mov, sizeof(mov));
                bool wBit = tempBuffer & 0b1;
                bool dBit = tempBuffer & 0b10;
                fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                int mod = tempBuffer >> 6;
                const int regMask = 0b00000111;
                if (wBit) {
                    switch (mod) {
                        case 0b00: {
                        } break;
                        case 0b01: {
                            
                        } break;
                        case 0b10: {
                            
                        } break;
                        case 0b11: {
                            getWideRegName(&buffer, tempBuffer & regMask);
                            char comma[] = ", ";
                            pushText(&buffer, comma, sizeof(comma));
                            tempBuffer >>= 3;
                            getWideRegName(&buffer, tempBuffer & regMask);
                        } break;
                    }
                } else {
                    getShortRegName(&buffer, tempBuffer & regMask);
                    char comma[] = ", ";
                    pushText(&buffer, comma, sizeof(comma));
                    tempBuffer >>= 3;
                    getShortRegName(&buffer, tempBuffer & regMask);
                }
            }
            ((char*)buffer.ptr)[buffer.currSize] = 0;
            printf((char*)buffer.ptr);
            buffer.currSize = 0;
        }
        
    }
    
    if (pFileRead) {
        fclose(pFileRead);
    }
    
    return 0;
}