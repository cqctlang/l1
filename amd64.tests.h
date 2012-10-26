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
	test(                          MOV(p, indirecti(ECX, EDX, 0), EBX),   "\x89\x1c\x11"                                     );	// 143
	test(                         MOV(p, indirect2i(ECX, EDX, 0), EBX),   "\x89\x1c\x51"                                     );	// 144
	test(                         MOV(p, indirect4i(ECX, EDX, 0), EBX),   "\x89\x1c\x91"                                     );	// 145
	test(                         MOV(p, indirect8i(ECX, EDX, 0), EBX),   "\x89\x1c\xd1"                                     );	// 146
	test(                          MOV(p, indirecti(RCX, RDX, 0), RBX),   "\x48\x89\x1c\x11"                                 );	// 148
	test(                         MOV(p, indirect2i(RCX, RDX, 0), RBX),   "\x48\x89\x1c\x51"                                 );	// 149
	test(                         MOV(p, indirect4i(RCX, RDX, 0), RBX),   "\x48\x89\x1c\x91"                                 );	// 150
	test(                         MOV(p, indirect8i(RCX, RDX, 0), RBX),   "\x48\x89\x1c\xd1"                                 );	// 151
	test(                                  MOV(p, indirect1(RAX), EBX),   "\x89\x18"                                         );	// 172
	test(                                             MOV(p, RDX, R12),   "\x4c\x89\xe2"                                     );	// 181
	test(                                             MOV(p, R12, RDX),   "\x49\x89\xd4"                                     );	// 182
	test(                                             MOV(p, EDX, ECX),   "\x89\xca"                                         );	// 183
	test(                                          MOV(p, ECX, imm(1)),   "\xb9\x01\x00\x00\x00\x00\x00\x00\x00"             );	// 187
	test(                                               MOV(p, BL, CH),   "\x88\xeb"                                         );	// 188
	test(                                         MOV(p, AL, byte(12)),   "\xb0\x0c"                                         );	// 190
	test(                                         MOV(p, AH, byte(12)),   "\xb4\x0c"                                         );	// 191
