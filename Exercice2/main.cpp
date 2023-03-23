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
        case 0b000: {
            char instrBase[] = "[bx + si";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b001: {
            char instrBase[] = "[bx + di";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b010: {
            char instrBase[] = "[bp + si";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b011: {
            char instrBase[] = "[bp + di";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b100: {
            char instrBase[] = "[si";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b101: {
            char instrBase[] = "[di";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b110: {
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
        case 0b111: {
            char instrBase[] = "[bx";
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
    }
    
    switch (mod) {
        case 0b01: {
            char add[] = " + ";
            pushText(pBuffer, add, sizeof(add));
            char instrBase[6];
            _itoa_s(lowDisp, instrBase, 10);
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
        case 0b10: {
            char add[] = " + ";
            pushText(pBuffer, add, sizeof(add));
            int value = (highDisp << 8) + lowDisp;
            char instrBase[6];
            _itoa_s(value, instrBase, 10);
            pushText(pBuffer, instrBase, sizeof(instrBase));
        } break;
    }
    
    pushText(pBuffer, "]", 2);
}

int main(int argc, char** argv) {
    // These intructions are displaced to move out the configurable bits.
    const int RmRMov = 0b00100010;
    const int IRmMov = 0b01100011;
    const int IRMov = 0b00001011;
    const int rmMask = 0b00000111;
    // TODO(Marchin): immediate mode
    FILE* pFileRead;
    
    if (argc < 2) {
        printf("File not specified.");
        return -1;
    }
    
    fopen_s(&pFileRead, argv[1], "rb");
    
    if (pFileRead && pFileRead) {
        Buffer buffer = {};
        buffer.maxSize = 32;
        buffer.ptr = malloc(buffer.maxSize);
        int instr = 0;
        printf("bits 16\n");
        while (!feof(pFileRead)) {
            u8 tempBuffer;
            instr++;
            fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
            if ((tempBuffer >> 2) == RmRMov) {
                char mov[] = "\nmov ";
                pushText(&buffer, mov, sizeof(mov));
                bool wBit = tempBuffer & 0b1;
                bool dBit = tempBuffer & 0b10;
                fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                int mod = tempBuffer >> 6;
                if (wBit) {
                    switch (mod) {
                        case 0b00: {
                            const int bpRm = 0b00000110;
                            u8 disp[2] = {};
                            if (dBit) {
                                getWideRegName(&buffer, tempBuffer & rmMask);
                                char comma[] = ", ";
                                pushText(&buffer, comma, sizeof(comma));
                                tempBuffer >>= 3;
                                int rm = tempBuffer & rmMask;
                                if (rm == bpRm) {
                                    fread_s(&disp, sizeof(disp), sizeof(u8), 2, pFileRead);
                                }
                                getEffectiveAddress(&buffer, rm, mod, disp[0], disp[1]);
                            } else {
                                int rm = tempBuffer & rmMask;
                                if (rm == bpRm) {
                                    fread_s(&disp, sizeof(disp), sizeof(u8), 2, pFileRead);
                                }
                                getEffectiveAddress(&buffer, rm, mod, disp[0], disp[1]);
                                char comma[] = ", ";
                                pushText(&buffer, comma, sizeof(comma));
                                tempBuffer >>= 3;
                                getWideRegName(&buffer, tempBuffer & rmMask);
                            }
                        } break;
                        case 0b01: {
                            u8 disp;
                            fread_s(&disp, sizeof(disp), sizeof(u8), 1, pFileRead);
                            if (dBit) {
                                getWideRegName(&buffer, tempBuffer & rmMask);
                                char comma[] = ", ";
                                pushText(&buffer, comma, sizeof(comma));
                                tempBuffer >>= 3;
                                getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp, 0);
                            } else {
                                getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp, 0);
                                char comma[] = ", ";
                                pushText(&buffer, comma, sizeof(comma));
                                tempBuffer >>= 3;
                                getWideRegName(&buffer, tempBuffer & rmMask);
                            }
                        } break;
                        case 0b10: {
                            u8 disp[2];
                            fread_s(&disp, sizeof(disp), sizeof(u8), 2, pFileRead);
                            if (dBit) {
                                getWideRegName(&buffer, tempBuffer & rmMask);
                                char comma[] = ", ";
                                pushText(&buffer, comma, sizeof(comma));
                                tempBuffer >>= 3;
                                getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp[0], disp[1]);
                            } else {
                                getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp[0], disp[1]);
                                char comma[] = ", ";
                                pushText(&buffer, comma, sizeof(comma));
                                tempBuffer >>= 3;
                                getWideRegName(&buffer, tempBuffer & rmMask);
                            }
                        } break;
                        case 0b11: {
                            getWideRegName(&buffer, tempBuffer & rmMask);
                            char comma[] = ", ";
                            pushText(&buffer, comma, sizeof(comma));
                            tempBuffer >>= 3;
                            getWideRegName(&buffer, tempBuffer & rmMask);
                        } break;
                    }
                } else {
                    getShortRegName(&buffer, tempBuffer & rmMask);
                    char comma[] = ", ";
                    pushText(&buffer, comma, sizeof(comma));
                    tempBuffer >>= 3;
                    getShortRegName(&buffer, tempBuffer & rmMask);
                }
            } else if ((tempBuffer >> 4) == IRMov) {
                char mov[] = "\nmov ";
                pushText(&buffer, mov, sizeof(mov));
                bool wBit = (tempBuffer >> 3) & 0b1;
                int reg = tempBuffer & 0b111;
                
                if (wBit) {
                    getWideRegName(&buffer, reg);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, sizeof(comma));
                    
                    u8 data[2];
                    fread_s(&data, sizeof(data), sizeof(u8), 2, pFileRead);
                    int value = (data[1] << 8) + data[0];
                    char instrBase[6];
                    _itoa_s(value, instrBase, 10);
                    pushText(&buffer, instrBase, sizeof(instrBase));
                } else {
                    getShortRegName(&buffer, reg);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, sizeof(comma));
                    
                    fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                    char instrBase[6];
                    _itoa_s(tempBuffer, instrBase, 10);
                    pushText(&buffer, instrBase, sizeof(instrBase));
                }
            } else if ((tempBuffer >> 1) == IRmMov) {
                bool wBit = (tempBuffer >> 1) & 0b1;
                
                fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                int mod = tempBuffer >> 6;
                
                if (wBit) {
                    u8 data[2];
                    fread_s(&data, sizeof(data), sizeof(u8), 2, pFileRead);
                    getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, data[0], data[1]);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, sizeof(comma));
                    
                    fread_s(&data, sizeof(data), sizeof(u8), 2, pFileRead);
                    int value = (data[1] << 8) + data[0];
                    char instrBase[6];
                    _itoa_s(value, instrBase, 10);
                    pushText(&buffer, instrBase, sizeof(instrBase));
                } else {
                    u8 data;
                    fread_s(&data, sizeof(data), sizeof(u8), 1, pFileRead);
                    getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, data, 0);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, sizeof(comma));
                    
                    fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                    char instrBase[6];
                    _itoa_s(tempBuffer, instrBase, 10);
                    pushText(&buffer, instrBase, sizeof(instrBase));
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