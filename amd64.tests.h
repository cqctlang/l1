	test(                                           CMOVO(p, RAX, RDX),   "\x48\x0f\x40\xc2"                                 );	//   0
	test(                                          CMOVNO(p, RAX, RDX),   "\x48\x0f\x41\xc2"                                 );	//   1
	test(                                           CMOVB(p, RAX, RDX),   "\x48\x0f\x42\xc2"                                 );	//   2
	test(                                          CMOVAE(p, RAX, RDX),   "\x48\x0f\x43\xc2"                                 );	//   3
	test(                                           CMOVE(p, RAX, RDX),   "\x48\x0f\x44\xc2"                                 );	//   4
	test(                                          CMOVNE(p, RAX, RDX),   "\x48\x0f\x45\xc2"                                 );	//   5
	test(                                          CMOVBE(p, RAX, RDX),   "\x48\x0f\x46\xc2"                                 );	//   6
	test(                                           CMOVA(p, RAX, RDX),   "\x48\x0f\x47\xc2"                                 );	//   7
	test(                                           CMOVS(p, RAX, RDX),   "\x48\x0f\x48\xc2"                                 );	//   8
	test(                                          CMOVNS(p, RAX, RDX),   "\x48\x0f\x49\xc2"                                 );	//   9
	test(                                           CMOVP(p, RAX, RDX),   "\x48\x0f\x4a\xc2"                                 );	//  10
	test(                                          CMOVNP(p, RAX, RDX),   "\x48\x0f\x4b\xc2"                                 );	//  11
	test(                                           CMOVL(p, RAX, RDX),   "\x48\x0f\x4c\xc2"                                 );	//  12
	test(                                          CMOVGE(p, RAX, RDX),   "\x48\x0f\x4d\xc2"                                 );	//  13
	test(                                          CMOVLE(p, RAX, RDX),   "\x48\x0f\x4e\xc2"                                 );	//  14
	test(                                           CMOVG(p, RAX, RDX),   "\x48\x0f\x4f\xc2"                                 );	//  15
	test(                                           IMUL2(p, RAX, RDX),   "\x48\x0f\xaf\xc2"                                 );	//  16
	test(                                   IMUL3(p, RAX, RDX, imm(3)),   "\x48\x6b\xc2\x03"                                 );	//  17
	test(               IMUL2(p, RAX, indirecti(RCX, RDX, 0x12345678)),   "\x48\x0f\xaf\x84\x11\x78\x56\x34\x12"             );	//  18
	test(       IMUL3(p, RAX, indirecti(RCX, RDX, 0x12345678), imm(3)),   "\x48\x6b\x84\x11\x78\x56\x34\x12\x03"             );	//  19
	test( IMUL3(p, RAX, indirecti(RCX, RDX, 0x12345678), imm(0x12345678)),   "\x48\x69\x84\x11\x78\x56\x34\x12\x78\x56\x34\x12" );	//  20
	test(                                              ROL(p, EDX, CL),   "\xd3\xc2"                                         );	//  21
	test(                                         ROL(p, EDX, imm(31)),   "\xc1\xc2\x1f"                                     );	//  22
	test(                                              ROR(p, EDX, CL),   "\xd3\xca"                                         );	//  23
	test(                                         ROR(p, EDX, imm(31)),   "\xc1\xca\x1f"                                     );	//  24
	test(                                              ROL(p, RDX, CL),   "\x48\xd3\xc2"                                     );	//  25
	test(                                         ROL(p, RDX, imm(31)),   "\x48\xc1\xc2\x1f"                                 );	//  26
	test(                  ROL(p, indirecti(RCX, RDX, 0x12345678), CL),   "\x48\xd3\x84\x11\x78\x56\x34\x12"                 );	//  27
	test(             ROL(p, indirecti(RCX, RDX, 0x12345678), imm(31)),   "\x48\xc1\x84\x11\x78\x56\x34\x12\x1f"             );	//  28
	test(                                              ROR(p, RDX, CL),   "\x48\xd3\xca"                                     );	//  29
	test(                                         ROR(p, RDX, imm(31)),   "\x48\xc1\xca\x1f"                                 );	//  30
	test(                  ROR(p, indirecti(RCX, RDX, 0x12345678), CL),   "\x48\xd3\x8c\x11\x78\x56\x34\x12"                 );	//  31
	test(             ROR(p, indirecti(RCX, RDX, 0x12345678), imm(31)),   "\x48\xc1\x8c\x11\x78\x56\x34\x12\x1f"             );	//  32
	test(                                         RCL(p, RDX, imm(31)),   "\x48\xc1\xd2\x1f"                                 );	//  33
	test(                                         RCR(p, RDX, imm(31)),   "\x48\xc1\xda\x1f"                                 );	//  34
	test(                                         SHL(p, RDX, imm(31)),   "\x48\xc1\xe2\x1f"                                 );	//  35
	test(                                         SHR(p, RDX, imm(31)),   "\x48\xc1\xea\x1f"                                 );	//  36
	test(                                         SAR(p, RDX, imm(31)),   "\x48\xc1\xfa\x1f"                                 );	//  37
	test(                                                     LEAVE(p),   "\xc9"                                             );	//  38
	test(                                                       RET(p),   "\xc3"                                             );	//  39
	test(                                       RETPOP(p, imm(0x1234)),   "\xc2\x34\x12"                                     );	//  40
	test(                                                     CPUID(p),   "\x0f\xa2"                                         );	//  41
	test(                                                       NOP(p),   "\x90"                                             );	//  42
	test(                                                     RDTSC(p),   "\x0f\x31"                                         );	//  43
	test(                                                     RDPMC(p),   "\x0f\x33"                                         );	//  44
	test(                                      JO(p, disp(0x12345678)),   "\x0f\x80\x78\x56\x34\x12"                         );	//  45
	test(                                     JNO(p, disp(0x12345678)),   "\x0f\x81\x78\x56\x34\x12"                         );	//  46
	test(                                      JB(p, disp(0x12345678)),   "\x0f\x82\x78\x56\x34\x12"                         );	//  47
	test(                                     JAE(p, disp(0x12345678)),   "\x0f\x83\x78\x56\x34\x12"                         );	//  48
	test(                                      JE(p, disp(0x12345678)),   "\x0f\x84\x78\x56\x34\x12"                         );	//  49
	test(                                     JNE(p, disp(0x12345678)),   "\x0f\x85\x78\x56\x34\x12"                         );	//  50
	test(                                     JBE(p, disp(0x12345678)),   "\x0f\x86\x78\x56\x34\x12"                         );	//  51
	test(                                      JA(p, disp(0x12345678)),   "\x0f\x87\x78\x56\x34\x12"                         );	//  52
	test(                                      JS(p, disp(0x12345678)),   "\x0f\x88\x78\x56\x34\x12"                         );	//  53
	test(                                     JNS(p, disp(0x12345678)),   "\x0f\x89\x78\x56\x34\x12"                         );	//  54
	test(                                      JP(p, disp(0x12345678)),   "\x0f\x8a\x78\x56\x34\x12"                         );	//  55
	test(                                     JNP(p, disp(0x12345678)),   "\x0f\x8b\x78\x56\x34\x12"                         );	//  56
	test(                                      JL(p, disp(0x12345678)),   "\x0f\x8c\x78\x56\x34\x12"                         );	//  57
	test(                                     JGE(p, disp(0x12345678)),   "\x0f\x8d\x78\x56\x34\x12"                         );	//  58
	test(                                     JLE(p, disp(0x12345678)),   "\x0f\x8e\x78\x56\x34\x12"                         );	//  59
	test(                                      JG(p, disp(0x12345678)),   "\x0f\x8f\x78\x56\x34\x12"                         );	//  60
	test(                                                 CALL(p, ECX),   "\xff\xd1"                                         );	//  61
	test(                                                 CALL(p, RCX),   "\x48\xff\xd1"                                     );	//  62
	test(                                    CALL(p, disp(0x12345678)),   "\xe8\x78\x56\x34\x12"                             );	//  63
	test(                     CALL(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xff\x94\x11\x78\x56\x34\x12"                 );	//  64
	test(                     CALL(p, indirecti(ECX, EDX, 0x12345678)),   "\xff\x94\x11\x78\x56\x34\x12"                     );	//  65
	test(                                                  JMP(p, ECX),   "\xff\xe1"                                         );	//  66
	test(                                                  JMP(p, RCX),   "\x48\xff\xe1"                                     );	//  67
	test(                                     JMP(p, disp(0x12345678)),   "\xe9\x78\x56\x34\x12"                             );	//  68
	test(                      JMP(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xff\xa4\x11\x78\x56\x34\x12"                 );	//  69
	test(                      JMP(p, indirecti(ECX, EDX, 0x12345678)),   "\xff\xa4\x11\x78\x56\x34\x12"                     );	//  70
	test(                                  LEA(p, ECX, indirect1(EDX)),   "\x8d\x0a"                                         );	//  71
	test(                 LEA(p, RCX, indirecti(RCX, RDX, 0x12345678)),   "\x48\x8d\x8c\x11\x78\x56\x34\x12"                 );	//  72
	test(                                                 PUSH(p, RAX),   "\x50"                                             );	//  73
	test(                                                 PUSH(p, EAX),   "\x50"                                             );	//  74
	test(                                     PUSH(p, imm(0x12345678)),   "\x68\x78\x56\x34\x12"                             );	//  75
	test(                                             PUSH(p, byte(1)),   "\x68\x01\x00\x00\x00"                             );	//  76
	test(                     PUSH(p, indirecti(RCX, RDX, 0x12345678)),   "\xff\xb4\x11\x78\x56\x34\x12"                     );	//  77
	test(                     PUSH(p, indirecti(ECX, EDX, 0x12345678)),   "\xff\xb4\x11\x78\x56\x34\x12"                     );	//  78
	test(                                                  POP(p, RAX),   "\x58"                                             );	//  79
	test(                                                  POP(p, EAX),   "\x58"                                             );	//  80
	test(                      POP(p, indirecti(RCX, RDX, 0x12345678)),   "\x8f\x84\x11\x78\x56\x34\x12"                     );	//  81
	test(                      POP(p, indirecti(ECX, EDX, 0x12345678)),   "\x8f\x84\x11\x78\x56\x34\x12"                     );	//  82
	test(                                                  NOT(p, RAX),   "\x48\xf7\xd0"                                     );	//  83
	test(                      NOT(p, indirecti(ECX, EDX, 0x12345678)),   "\xf7\x94\x11\x78\x56\x34\x12"                     );	//  84
	test(                      NOT(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xf7\x94\x11\x78\x56\x34\x12"                 );	//  85
	test(                                                  NEG(p, RAX),   "\x48\xf7\xd8"                                     );	//  86
	test(                      NEG(p, indirecti(ECX, EDX, 0x12345678)),   "\xf7\x9c\x11\x78\x56\x34\x12"                     );	//  87
	test(                      NEG(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xf7\x9c\x11\x78\x56\x34\x12"                 );	//  88
	test(                                                  MUL(p, RAX),   "\x48\xf7\xe0"                                     );	//  89
	test(                      MUL(p, indirecti(ECX, EDX, 0x12345678)),   "\xf7\xa4\x11\x78\x56\x34\x12"                     );	//  90
	test(                      MUL(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xf7\xa4\x11\x78\x56\x34\x12"                 );	//  91
	test(                                                IMUL1(p, RAX),   "\x48\xf7\xe8"                                     );	//  92
	test(                    IMUL1(p, indirecti(ECX, EDX, 0x12345678)),   "\xf7\xac\x11\x78\x56\x34\x12"                     );	//  93
	test(                    IMUL1(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xf7\xac\x11\x78\x56\x34\x12"                 );	//  94
	test(                                                  DIV(p, RAX),   "\x48\xf7\xf0"                                     );	//  95
	test(                      DIV(p, indirecti(ECX, EDX, 0x12345678)),   "\xf7\xb4\x11\x78\x56\x34\x12"                     );	//  96
	test(                      DIV(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xf7\xb4\x11\x78\x56\x34\x12"                 );	//  97
	test(                                                 IDIV(p, RAX),   "\x48\xf7\xf8"                                     );	//  98
	test(                     IDIV(p, indirecti(ECX, EDX, 0x12345678)),   "\xf7\xbc\x11\x78\x56\x34\x12"                     );	//  99
	test(                     IDIV(p, indirecti(RCX, RDX, 0x12345678)),   "\x48\xf7\xbc\x11\x78\x56\x34\x12"                 );	// 100
	test(                                           OR(p, RAX, imm(5)),   "\x48\x83\xc8\x05"                                 );	// 101
	test(                                              OR(p, RAX, RBX),   "\x48\x09\xd8"                                     );	// 102
	test(                                   OR(p, RAX, indirect1(RBX)),   "\x48\x0b\x03"                                     );	// 103
	test(                                          ADC(p, RAX, imm(5)),   "\x48\x83\xd0\x05"                                 );	// 104
	test(                                             ADC(p, RAX, RBX),   "\x48\x11\xd8"                                     );	// 105
	test(                                  ADC(p, RAX, indirect1(RBX)),   "\x48\x13\x03"                                     );	// 106
	test(                                          SBB(p, RAX, imm(5)),   "\x48\x83\xd8\x05"                                 );	// 107
	test(                                             SBB(p, RAX, RBX),   "\x48\x19\xd8"                                     );	// 108
	test(                                  SBB(p, RAX, indirect1(RBX)),   "\x48\x1b\x03"                                     );	// 109
	test(                                          AND(p, RAX, imm(5)),   "\x48\x83\xe0\x05"                                 );	// 110
	test(                                             AND(p, RAX, RBX),   "\x48\x21\xd8"                                     );	// 111
	test(                                  AND(p, RAX, indirect1(RBX)),   "\x48\x23\x03"                                     );	// 112
	test(                                          SUB(p, RAX, imm(5)),   "\x48\x83\xe8\x05"                                 );	// 113
	test(                                             SUB(p, RAX, RBX),   "\x48\x29\xd8"                                     );	// 114
	test(                                  SUB(p, RAX, indirect1(RBX)),   "\x48\x2b\x03"                                     );	// 115
	test(                                          XOR(p, RAX, imm(5)),   "\x48\x83\xf0\x05"                                 );	// 116
	test(                                             XOR(p, RAX, RBX),   "\x48\x31\xd8"                                     );	// 117
	test(                                  XOR(p, RAX, indirect1(RBX)),   "\x48\x33\x03"                                     );	// 118
	test(                                          CMP(p, RAX, imm(5)),   "\x48\x83\xf8\x05"                                 );	// 119
	test(                                             CMP(p, RAX, RBX),   "\x48\x39\xd8"                                     );	// 120
	test(                                  CMP(p, RAX, indirect1(RBX)),   "\x48\x3b\x03"                                     );	// 121
	test(                                         TEST(p, RAX, imm(5)),   "\x48\xf7\xc0\x05\x00\x00\x00"                     );	// 122
	test(                                            TEST(p, RAX, RBX),   "\x48\x85\xd8"                                     );	// 123
	test(                                 TEST(p, indirect1(RBX), RAX),   "\x48\x85\x03"                                     );	// 124
	test(                                 TEST(p, RAX, indirect1(RBX)),   "\x48\x85\x03"                                     );	// 125
	test(                 MOV(p, indirecti(ECX, EDX, 0x12345678), EBX),   "\x89\x9c\x11\x78\x56\x34\x12"                     );	// 126
	test(                MOV(p, indirect2i(ECX, EDX, 0x12345678), EBX),   "\x89\x9c\x51\x78\x56\x34\x12"                     );	// 127
	test(                MOV(p, indirect4i(ECX, EDX, 0x12345678), EBX),   "\x89\x9c\x91\x78\x56\x34\x12"                     );	// 128
	test(                MOV(p, indirect8i(ECX, EDX, 0x12345678), EBX),   "\x89\x9c\xd1\x78\x56\x34\x12"                     );	// 129
	test(                 MOV(p, indirecti(RCX, RDX, 0x12345678), RBX),   "\x48\x89\x9c\x11\x78\x56\x34\x12"                 );	// 130
	test(                MOV(p, indirect2i(RCX, RDX, 0x12345678), RBX),   "\x48\x89\x9c\x51\x78\x56\x34\x12"                 );	// 131
	test(                MOV(p, indirect4i(RCX, RDX, 0x12345678), RBX),   "\x48\x89\x9c\x91\x78\x56\x34\x12"                 );	// 132
	test(                MOV(p, indirect8i(RCX, RDX, 0x12345678), RBX),   "\x48\x89\x9c\xd1\x78\x56\x34\x12"                 );	// 133
	test(                        MOV(p, indirecti(ECX, EDX, 100), EBX),   "\x89\x5c\x11\x64"                                 );	// 134
	test(                       MOV(p, indirect2i(ECX, EDX, 100), EBX),   "\x89\x5c\x51\x64"                                 );	// 135
	test(                       MOV(p, indirect4i(ECX, EDX, 100), EBX),   "\x89\x5c\x91\x64"                                 );	// 136
	test(                       MOV(p, indirect8i(ECX, EDX, 100), EBX),   "\x89\x5c\xd1\x64"                                 );	// 137
	test(                        MOV(p, indirecti(RCX, RDX, 100), RBX),   "\x48\x89\x5c\x11\x64"                             );	// 138
	test(                       MOV(p, indirect2i(RCX, RDX, 100), RBX),   "\x48\x89\x5c\x51\x64"                             );	// 139
	test(                       MOV(p, indirect4i(RCX, RDX, 100), RBX),   "\x48\x89\x5c\x91\x64"                             );	// 140
	test(                       MOV(p, indirect8i(RCX, RDX, 100), RBX),   "\x48\x89\x5c\xd1\x64"                             );	// 141
	test(                             MOV(p, indirect2(ECX, EDX), EBX),   "\x89\x1c\x11"                                     );	// 142
	test(                          MOV(p, indirecti(ECX, EDX, 0), EBX),   "\x89\x1c\x11"                                     );	// 143
	test(                         MOV(p, indirect2i(ECX, EDX, 0), EBX),   "\x89\x1c\x51"                                     );	// 144
	test(                         MOV(p, indirect4i(ECX, EDX, 0), EBX),   "\x89\x1c\x91"                                     );	// 145
	test(                         MOV(p, indirect8i(ECX, EDX, 0), EBX),   "\x89\x1c\xd1"                                     );	// 146
	test(                             MOV(p, indirect2(RCX, RDX), RBX),   "\x48\x89\x1c\x11"                                 );	// 147
	test(                          MOV(p, indirecti(RCX, RDX, 0), RBX),   "\x48\x89\x1c\x11"                                 );	// 148
	test(                         MOV(p, indirect2i(RCX, RDX, 0), RBX),   "\x48\x89\x1c\x51"                                 );	// 149
	test(                         MOV(p, indirect4i(RCX, RDX, 0), RBX),   "\x48\x89\x1c\x91"                                 );	// 150
	test(                         MOV(p, indirect8i(RCX, RDX, 0), RBX),   "\x48\x89\x1c\xd1"                                 );	// 151
	test(                             MOV(p, RAX, indirect2(R13, RBX)),   "\x49\x8b\x44\x1d\x00"                             );	// 152
	test(                             MOV(p, RAX, indirect2(RBP, RBX)),   "\x48\x8b\x44\x1d\x00"                             );	// 153
	test(                             MOV(p, RAX, indirect2(RBX, R12)),   "\x4a\x8b\x04\x23"                                 );	// 154
	test(                             MOV(p, RAX, indirect2(RBX, R13)),   "\x4a\x8b\x04\x2b"                                 );	// 155
	test(                             MOV(p, RAX, indirect2(R13, R12)),   "\x4b\x8b\x44\x25\x00"                             );	// 156
	test(                             MOV(p, RAX, indirect2(R12, R13)),   "\x4b\x8b\x04\x2c"                                 );	// 157
	test(                             MOV(p, RAX, indirect2(R12, RBP)),   "\x49\x8b\x04\x2c"                                 );	// 158
	test(                             MOV(p, indirect2(R13, RBX), RAX),   "\x49\x89\x44\x1d\x00"                             );	// 159
	test(                             MOV(p, indirect2(RBP, RBX), RAX),   "\x48\x89\x44\x1d\x00"                             );	// 160
	test(                             MOV(p, indirect2(RBX, R12), RAX),   "\x4a\x89\x04\x23"                                 );	// 161
	test(                             MOV(p, indirect2(RBX, R13), RAX),   "\x4a\x89\x04\x2b"                                 );	// 162
	test(                             MOV(p, indirect2(R13, R12), RAX),   "\x4b\x89\x44\x25\x00"                             );	// 163
	test(                             MOV(p, indirect2(R12, R13), RAX),   "\x4b\x89\x04\x2c"                                 );	// 164
	test(                             MOV(p, indirect2(R12, RBP), RAX),   "\x49\x89\x04\x2c"                                 );	// 165
	test(                             MOV(p, indirect2(R10, RBX), RAX),   "\x49\x89\x04\x1a"                                 );	// 166
	test(                             MOV(p, indirect2(R11, RBX), RAX),   "\x49\x89\x04\x1b"                                 );	// 167
	test(                             MOV(p, indirect2(R12, RBX), RAX),   "\x49\x89\x04\x1c"                                 );	// 168
	test(                             MOV(p, indirect2(RSP, RBX), RAX),   "\x48\x89\x04\x1c"                                 );	// 169
	test(                               OR(p, indirect1(EAX), byte(5)),   "\x80\x08\x05"                                     );	// 170
	test(                              MOV(p, indirect1(EAX), byte(5)),   "\xc6\x00\x05"                                     );	// 171
	test(                                  MOV(p, indirect1(RAX), EBX),   "\x89\x18"                                         );	// 172
	test(                                  MOV(p, indirect1(RAX), RBX),   "\x48\x89\x18"                                     );	// 173
	test(                                   MOV(p, indirect1(RAX), BL),   "\x88\x18"                                         );	// 174
	test(                                   MOV(p, indirect1(RAX), BX),   "\x66\x89\x18"                                     );	// 175
	test(                                  MOV(p, EBX, indirect1(RAX)),   "\x8b\x18"                                         );	// 176
	test(                                  MOV(p, RBX, indirect1(RAX)),   "\x48\x8b\x18"                                     );	// 177
	test(                                   MOV(p, BL, indirect1(RAX)),   "\x8a\x18"                                         );	// 178
	test(                                   MOV(p, BX, indirect1(RAX)),   "\x66\x8b\x18"                                     );	// 179
	test(                                  MOV(p, BPL, indirect1(RDX)),   "\x40\x8a\x2a"                                     );	// 180
	test(                                             MOV(p, RDX, R12),   "\x4c\x89\xe2"                                     );	// 181
	test(                                             MOV(p, R12, RDX),   "\x49\x89\xd4"                                     );	// 182
	test(                                             MOV(p, EDX, ECX),   "\x89\xca"                                         );	// 183
	test(                                  MOV(p, indirect1(R12), RAX),   "\x49\x89\x04\x24"                                 );	// 184
	test(                                  ADD(p, RAX, indirect1(R14)),   "\x49\x03\x06"                                     );	// 185
	test(                                  MOV(p, RAX, indirect1(R14)),   "\x49\x8b\x06"                                     );	// 186
	test(                                          MOV(p, ECX, imm(1)),   "\xb9\x01\x00\x00\x00"                             );	// 187
	test(                                          MOV(p, RCX, imm(1)),   "\x48\xb9\x01\x00\x00\x00\x00\x00\x00\x00"         );	// 188
	test(                                               MOV(p, BL, CH),   "\x88\xeb"                                         );	// 189
	test(                             MOV(p, indirect1(EAX), byte(12)),   "\xc6\x00\x0c"                                     );	// 190
	test(                                         MOV(p, AL, byte(12)),   "\xb0\x0c"                                         );	// 191
	test(                                         MOV(p, AH, byte(12)),   "\xb4\x0c"                                         );	// 192
	test(                                         ADD(p, AL, byte(12)),   "\x80\xc0\x0c"                                     );	// 193
	test(                                         ADD(p, AH, byte(12)),   "\x80\xc4\x0c"                                     );	// 194
	test(                                               ADD(p, BL, CH),   "\x00\xeb"                                         );	// 195
	test(                                          ADD(p, ECX, imm(1)),   "\x83\xc1\x01"                                     );	// 196
	test(                                          ADD(p, ECX, imm(1)),   "\x83\xc1\x01"                                     );	// 197
	test(                                        ADD(p, ECX, imm(150)),   "\x81\xc1\x96\x00\x00\x00"                         );	// 198
	test(                                             ADD(p, RAX, RBX),   "\x48\x01\xd8"                                     );	// 199
	test(                                  ADD(p, RAX, indirect1(RBX)),   "\x48\x03\x03"                                     );	// 200
	test(                                  ADD(p, indirect1(RBX), RAX),   "\x48\x01\x03"                                     );	// 201
	test(                                                    PUSHAD(p),   "\x60"                                             );	// 202
	test(                                                     POPAD(p),   "\x61"                                             );	// 203
