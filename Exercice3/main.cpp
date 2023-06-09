#include "main.h"

// Based on 8086 spec: https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

void pushText(Buffer* pBuffer, char* text, size_t textLength) {
    char* ptr = (char*)pBuffer->ptr + pBuffer->currSize;
    size_t bufferSize = pBuffer->maxSize - pBuffer->currSize;
    // Remove 0 end
    pBuffer->currSize += textLength;
    strncpy_s(ptr, bufferSize, text, bufferSize);
}

void getOp(Buffer* pBuffer, int op) {
    switch (op) {
        case 0b000: {
            char instrBase[] = "\nadd ";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b101: {
            char instrBase[] = "\nsub ";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b111: {
            char instrBase[] = "\ncmp ";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
    }
}

void getWideRegName(Buffer* pBuffer, int rm) {
    switch (rm) {
        case 0b00000000: {
            char instrBase[] = "ax";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000001: {
            char instrBase[] = "cx";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000010: {
            char instrBase[] = "dx";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000011: {
            char instrBase[] = "bx";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000100: {
            char instrBase[] = "sp";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000101: {
            char instrBase[] = "bp";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000110: {
            char instrBase[] = "si";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000111: {
            char instrBase[] = "di";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
    }
}

void getShortRegName(Buffer* pBuffer, int rm) {
    int rmv0 = (pBuffer->currSize) > 0 ? 1 : 0;
    char* ptr = (char*)pBuffer->ptr + pBuffer->currSize - rmv0;
    
    switch (rm) {
        case 0b00000000: {
            char instrBase[] = "al";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000001: {
            char instrBase[] = "cl";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000010: {
            char instrBase[] = "dl";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000011: {
            char instrBase[] = "bl";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000100: {
            char instrBase[] = "ah";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000101: {
            char instrBase[] = "ch";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000110: {
            char instrBase[] = "dh";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b00000111: {
            char instrBase[] = "bh";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
    }
}

void getEffectiveAddress(Buffer* pBuffer, int rm, int mod, u8 lowDisp, u8 highDisp) {
    char* ptr = (char*)pBuffer->ptr + pBuffer->currSize;
    
    switch (rm) {
        case 0b000: {
            char instrBase[] = "[bx + si";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b001: {
            char instrBase[] = "[bx + di";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b010: {
            char instrBase[] = "[bp + si";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b011: {
            char instrBase[] = "[bp + di";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b100: {
            char instrBase[] = "[si";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b101: {
            char instrBase[] = "[di";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
        case 0b110: {
            if (mod == 0) {
                s16 value = (highDisp << 8) + lowDisp;
                char instrBase[MAX_CHAR_16 + 1];
                instrBase[0] = '[';
                _itoa_s(value, instrBase + 1, sizeof(instrBase) - 1, 10);
                pushText(pBuffer, instrBase, strlen(instrBase));
            } else {
                char instrBase[] = "[bp";
                pushText(pBuffer, instrBase, strlen(instrBase));
            }
        } break;
        case 0b111: {
            char instrBase[] = "[bx";
            pushText(pBuffer, instrBase, strlen(instrBase));
        } break;
    }
    
    if (lowDisp || highDisp) {
        switch (mod) {
            case 0b01: {
                if ((lowDisp >> 7) & 1) {
                    char op[] = " - ";
                    pushText(pBuffer, op, strlen(op));
                    char instrBase[MAX_CHAR_16];
                    _itoa_s(~(s8)lowDisp + 1, instrBase, 10);
                    pushText(pBuffer, instrBase, strlen(instrBase));
                } else {
                    char op[] = " + ";
                    pushText(pBuffer, op, strlen(op));
                    char instrBase[MAX_CHAR_16];
                    _itoa_s(lowDisp, instrBase, 10);
                    pushText(pBuffer, instrBase, strlen(instrBase));
                }
            } break;
            case 0b10: {
                s16 value = (highDisp << 8) + (s16)lowDisp;
                if ((value >> 15) & 1) {
                    char op[] = " - ";
                    pushText(pBuffer, op, strlen(op));
                    char instrBase[MAX_CHAR_16];
                    _itoa_s(~value + 1, instrBase, 10);
                    pushText(pBuffer, instrBase, strlen(instrBase));
                } else {
                    char op[] = " + ";
                    pushText(pBuffer, op, strlen(op));
                    char instrBase[MAX_CHAR_16];
                    _itoa_s(value, instrBase, 10);
                    pushText(pBuffer, instrBase, strlen(instrBase));
                }
            } break;
        }
    }
    
    pushText(pBuffer, "]", 1);
}

bool getConditionalJump(Buffer* pBuffer, u8 instr) {
    bool result = false;
    
    switch (instr) {
        case 0b01110100: {
            char instrBase[] = "\njz ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111100: {
            char instrBase[] = "\njl ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111110: {
            char instrBase[] = "\njle ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01110010: {
            char instrBase[] = "\njb ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01110110: {
            char instrBase[] = "\njbe ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111010: {
            char instrBase[] = "\njp ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01110000: {
            char instrBase[] = "\njo ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111000: {
            char instrBase[] = "\njs ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01110101: {
            char instrBase[] = "\njne ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111101: {
            char instrBase[] = "\njnl ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111111: {
            char instrBase[] = "\njnle ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01110011: {
            char instrBase[] = "\njnb ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01110111: {
            char instrBase[] = "\njnbe ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111011: {
            char instrBase[] = "\njnp ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01110001: {
            char instrBase[] = "\njno ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b01111001: {
            char instrBase[] = "\njns ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b11100010: {
            char instrBase[] = "\nloop ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b11100001: {
            char instrBase[] = "\nloopz ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b11100000: {
            char instrBase[] = "\nloopnz ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
        case 0b11100011: {
            char instrBase[] = "\njcxz ";
            pushText(pBuffer, instrBase, strlen(instrBase));
            result = true;
        } break;
    }
    
    return result;
}

int main(int argc, char** argv) {
    // These intructions are displaced to move out the configurable bits.
    const int RmRMov = 0b00100010;
    const int IRmMov = 0b01100011;
    const int IRMov = 0b00001011;
    const int MemAccMov = 0b01010000;
    const int AccMemMov = 0b01010001;
    const int RmROp = 0b00000000;
    const int IRmOp = 0b00100000;
    const int IAccOp = 0b00000010;
    const int OpAdd = 0b000;
    const int OpSub = 0b101;
    const int OpCmp = 0b111;
    const int OpMaskClear = 0b11000111;
    const int rmMask = 0b00000111;
    FILE* pFileRead;
    
    if (argc < 2) {
        printf("File not specified.");
        return -1;
    }
    
    fopen_s(&pFileRead, argv[1], "rb");
    
    if (pFileRead) {
        Buffer buffer = {};
        buffer.maxSize = 32;
        buffer.ptr = malloc(buffer.maxSize);
        int instr = 0;
        printf("bits 16\n");
        u8 tempBuffer;
        
        while (fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead) > 0) {
            instr++;
            
            if (getConditionalJump(&buffer, tempBuffer)) {
                s8 value;
                fread_s(&value, sizeof(value), sizeof(s8), 1, pFileRead);
                char instrBase[MAX_CHAR_16];
                _itoa_s(value, instrBase, 10);
                pushText(&buffer, instrBase, strlen(instrBase));
                
                ((char*)buffer.ptr)[buffer.currSize] = 0;
                printf((char*)buffer.ptr);
                buffer.currSize = 0;
                continue;
            }
            
            int opMaskedBufferOffset2 = (tempBuffer & OpMaskClear) >> 2;
            int opMaskedBufferOffset1 = (tempBuffer & OpMaskClear) >> 1;
            bool isRmRMov = tempBuffer >> 2 == RmRMov;
            int op = (tempBuffer & ~OpMaskClear) >> 3;
            bool isOP = (op == 0b000) || (op == 0b101) || (op == 0b111);
            if (isRmRMov || (isOP && (opMaskedBufferOffset2 == RmROp))) {
                if (isOP) {
                    getOp(&buffer, (tempBuffer & ~OpMaskClear) >> 3);
                } else {
                    char mov[] = "\nmov ";
                    pushText(&buffer, mov, strlen(mov));
                }
                
                bool wBit = tempBuffer & 0b1;
                bool dBit = tempBuffer & 0b10;
                fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                int mod = tempBuffer >> 6;
                switch (mod) {
                    case 0b00: {
                        const int bpRm = 0b00000110;
                        u8 disp[2] = {};
                        if (dBit) {
                            if (wBit) {
                                getWideRegName(&buffer, (tempBuffer >> 3) & rmMask);
                            } else {
                                getShortRegName(&buffer, (tempBuffer >> 3) & rmMask);
                            }
                            char comma[] = ", ";
                            pushText(&buffer, comma, strlen(comma));
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
                            pushText(&buffer, comma, strlen(comma));
                            tempBuffer >>= 3;
                            if (wBit) {
                                getWideRegName(&buffer, tempBuffer & rmMask);
                            } else {
                                getShortRegName(&buffer, tempBuffer & rmMask);
                            }
                        }
                    } break;
                    case 0b01: {
                        u8 disp;
                        fread_s(&disp, sizeof(disp), sizeof(u8), 1, pFileRead);
                        if (dBit) {
                            if (wBit) {
                                getWideRegName(&buffer, (tempBuffer >> 3) & rmMask);
                            } else {
                                getShortRegName(&buffer, (tempBuffer >> 3) & rmMask);
                            }
                            char comma[] = ", ";
                            pushText(&buffer, comma, strlen(comma));
                            getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp, 0);
                        } else {
                            getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp, 0);
                            char comma[] = ", ";
                            pushText(&buffer, comma, strlen(comma));
                            tempBuffer >>= 3;
                            if (wBit) {
                                getWideRegName(&buffer, tempBuffer & rmMask);
                            } else {
                                getShortRegName(&buffer, tempBuffer & rmMask);
                            }
                        }
                    } break;
                    case 0b10: {
                        u8 disp[2];
                        fread_s(&disp, sizeof(disp), sizeof(u8), 2, pFileRead);
                        if (dBit) {
                            if (wBit) {
                                getWideRegName(&buffer, (tempBuffer >> 3) & rmMask);
                            } else {
                                getShortRegName(&buffer, (tempBuffer >> 3) & rmMask);
                            }
                            char comma[] = ", ";
                            pushText(&buffer, comma, strlen(comma));
                            getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp[0], disp[1]);
                        } else {
                            getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp[0], disp[1]);
                            char comma[] = ", ";
                            pushText(&buffer, comma, strlen(comma));
                            tempBuffer >>= 3;
                            if (wBit) {
                                getWideRegName(&buffer, tempBuffer & rmMask);
                            } else {
                                getShortRegName(&buffer, tempBuffer & rmMask);
                            }
                        }
                    } break;
                    case 0b11: {
                        if (wBit) {
                            getWideRegName(&buffer, tempBuffer & rmMask);
                        } else {
                            getShortRegName(&buffer, tempBuffer & rmMask);
                        }
                        char comma[] = ", ";
                        pushText(&buffer, comma, strlen(comma));
                        tempBuffer >>= 3;
                        if (wBit) {
                            getWideRegName(&buffer, tempBuffer & rmMask);
                        } else {
                            getShortRegName(&buffer, tempBuffer & rmMask);
                        }
                    } break;
                }
            } else if (((tempBuffer >> 4) == IRMov) || (isOP && (opMaskedBufferOffset1 == IAccOp))) {
                if (isOP) {
                    getOp(&buffer, (tempBuffer & ~OpMaskClear) >> 3);
                } else {
                    char mov[] = "\nmov ";
                    pushText(&buffer, mov, strlen(mov));
                }
                
                bool wBit = isOP ? (tempBuffer & 1) : ((tempBuffer >> 3) & 0b1);
                int reg = isOP ? 0 : (tempBuffer & 0b111);
                
                if (wBit) {
                    getWideRegName(&buffer, reg);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, strlen(comma));
                    
                    u8 data[2];
                    fread_s(&data, sizeof(data), sizeof(u8), 2, pFileRead);
                    int value = (data[1] << 8) + data[0];
                    char instrBase[MAX_CHAR_16];
                    _itoa_s((s16)value, instrBase, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                } else {
                    getShortRegName(&buffer, reg);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, strlen(comma));
                    
                    fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                    char instrBase[MAX_CHAR_16];
                    _itoa_s((s8)tempBuffer, instrBase, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                }
            } else if (((tempBuffer >> 1) == IRmMov) || ((isOP && (opMaskedBufferOffset2 == IRmOp)))) {
                bool wBit = tempBuffer & 1;
                bool sBit = (opMaskedBufferOffset2 == IRmOp) && (tempBuffer & 0b10);
                
                fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                int mod = tempBuffer >> 6;
                
                if (isOP) {
                    getOp(&buffer, (tempBuffer & 0b00111000) >> 3);
                    
                    if (mod != 0b11) {
                        if (wBit) {
                            char wordText[] = "word ";
                            pushText(&buffer, wordText, strlen(wordText));
                        } else {
                            char byteText[] = "byte ";
                            pushText(&buffer, byteText, strlen(byteText));
                        }
                    }
                } else {
                    char mov[] = "\nmov ";
                    pushText(&buffer, mov, strlen(mov));
                }
                
                switch (mod) {
                    case 0b00: {
                        u8 disp[2] = {};
                        int rm = tempBuffer & rmMask;
                        if (rm == 0b110) {
                            fread_s(&disp, sizeof(disp), sizeof(u8), 2, pFileRead);
                        }
                        getEffectiveAddress(&buffer, rm, mod, disp[0], disp[1]);
                    } break;
                    case 0b01: {
                        u8 disp;
                        fread_s(&disp, sizeof(disp), sizeof(u8), 1, pFileRead);
                        getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp, 0);
                    } break;
                    case 0b10: {
                        u8 disp[2];
                        fread_s(&disp, sizeof(disp), sizeof(u8), 2, pFileRead);
                        getEffectiveAddress(&buffer, tempBuffer & rmMask, mod, disp[0], disp[1]);
                    } break;
                    case 0b11: {
                        if (wBit) {
                            getWideRegName(&buffer, tempBuffer & rmMask);
                        } else {
                            getShortRegName(&buffer, tempBuffer & rmMask);
                        }
                    } break;
                }
                
                char comma[] = ", ";
                pushText(&buffer, comma, strlen(comma));
                
                if (wBit && !sBit) {
                    u8 data[2];
                    fread_s(&data, sizeof(data), sizeof(u8), 2, pFileRead);
                    
                    if (!isOP) {
                        char wordText[] = "word ";
                        pushText(&buffer, wordText, strlen(wordText));
                    }
                    
                    s16 value = (data[1] << 8) + data[0];
                    char instrBase[MAX_CHAR_16];
                    _itoa_s(value, instrBase, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                } else {
                    if (!sBit && !isOP) {
                        char byteText[] = "byte ";
                        pushText(&buffer, byteText, strlen(byteText));
                    }
                    fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                    char instrBase[MAX_CHAR_16];
                    _itoa_s(tempBuffer, instrBase, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                }
            } else if ((tempBuffer >> 1) == MemAccMov) {
                char mov[] = "\nmov ";
                pushText(&buffer, mov, strlen(mov));
                
                bool wBit = tempBuffer & 1;
                
                if (wBit) {
                    getWideRegName(&buffer, 0);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, strlen(comma));
                    
                    u8 data[2];
                    fread_s(&data, sizeof(data), sizeof(u8), 2, pFileRead);
                    s16 value = (data[1] << 8) + data[0];
                    char instrBase[MAX_CHAR_16 + 1];
                    instrBase[0] = '[';
                    _itoa_s(value, instrBase + 1, sizeof(instrBase) - 1, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                } else {
                    getShortRegName(&buffer, 0);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, strlen(comma));
                    
                    fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                    char instrBase[MAX_CHAR_16 + 1];
                    instrBase[0] = '[';
                    _itoa_s(tempBuffer, instrBase + 1, sizeof(instrBase) - 1, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                }
                
                pushText(&buffer, "]", 1);
            } else if ((tempBuffer >> 1) == AccMemMov) {
                char mov[] = "\nmov ";
                pushText(&buffer, mov, strlen(mov));
                
                bool wBit = tempBuffer & 1;
                
                if (wBit) {
                    u8 data[2];
                    size_t a = fread_s(&data, sizeof(data), sizeof(u8), 2, pFileRead);
                    s16 value = (data[1] << 8) + data[0];
                    char instrBase[MAX_CHAR_16 + 1];
                    instrBase[0] = '[';
                    _itoa_s(value, instrBase + 1, sizeof(instrBase) - 1, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                    pushText(&buffer, "]", 1);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, strlen(comma));
                    
                    getWideRegName(&buffer, 0);
                } else {
                    fread_s(&tempBuffer, sizeof(tempBuffer), sizeof(u8), 1, pFileRead);
                    char instrBase[MAX_CHAR_16 + 1];
                    instrBase[0] = '[';
                    _itoa_s(tempBuffer, instrBase + 1, sizeof(instrBase) - 1, 10);
                    pushText(&buffer, instrBase, strlen(instrBase));
                    pushText(&buffer, "]", 1);
                    
                    char comma[] = ", ";
                    pushText(&buffer, comma, strlen(comma));
                    
                    getShortRegName(&buffer, 0);
                }
            }
            
            ((char*)buffer.ptr)[buffer.currSize] = 0;
            printf((char*)buffer.ptr);
            buffer.currSize = 0;
        }
        
        fclose(pFileRead);
    }
    
    return 0;
}