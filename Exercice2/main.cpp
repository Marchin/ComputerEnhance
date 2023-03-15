#include <stdio.h>

typedef unsigned char u8;

// Based on 8086 spec: https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf

int main(int argc, char** argv) {
    // These intructions are displaced to move out the configurable bits.
    const u8 RmRMov = 0b00100010;
    FILE* pFileRead;
    
    if (argc < 2) {
        printf("File not specified.");
        return -1;
    }
    
    fopen_s(&pFileRead, argv[1], "rb");
    
    if (pFileRead && pFileRead) {
        u8 buffer;
        printf("bits 16\n");
        while (!feof(pFileRead)) {
            fread_s(&buffer, sizeof(buffer), sizeof(u8), 1, pFileRead);
            if ((buffer >> 2) == RmRMov) {
                printf("\nmov ");
                bool isWide = buffer & 1;
                fread_s(&buffer, sizeof(buffer), sizeof(u8), 1, pFileRead);
                char* reg = "";
                if (isWide) {
                    for (int i = 0; i < 2; i++, buffer >>= 3) {
                        if (i > 0) {
                            printf(", ");
                        }
                        u8 regb = buffer & 0b00000111;
                        switch (regb) {
                            case 0b00000000: {
                                reg = "ax";
                            } break;
                            case 0b00000001: {
                                reg = "cx";
                            } break;
                            case 0b00000010: {
                                reg = "dx";
                            } break;
                            case 0b00000011: {
                                reg = "bx";
                            } break;
                            case 0b00000100: {
                                reg = "sp";
                            } break;
                            case 0b00000101: {
                                reg = "bp";
                            } break;
                            case 0b00000110: {
                                reg = "si";
                            } break;
                            case 0b00000111: {
                                reg = "di";
                            } break;
                        }
                        printf(reg);
                    }
                } else {
                    for (int i = 0; i < 2; i++, buffer >>= 3) {
                        if (i > 0) {
                            printf(", ");
                        }
                        u8 regb = buffer & 0b00000111;
                        switch (regb) {
                            case 0b00000000: {
                                reg = "al";
                            } break;
                            case 0b00000001: {
                                reg = "cl";
                            } break;
                            case 0b00000010: {
                                reg = "dl";
                            } break;
                            case 0b00000011: {
                                reg = "bl";
                            } break;
                            case 0b00000100: {
                                reg = "ah";
                            } break;
                            case 0b00000101: {
                                reg = "ch";
                            } break;
                            case 0b00000110: {
                                reg = "dh";
                            } break;
                            case 0b00000111: {
                                reg = "bh";
                            } break;
                        }
                        printf(reg);
                    }
                }
            }
        }
    }
    
    if (pFileRead) {
        fclose(pFileRead);
    }
    
    return 0;
}