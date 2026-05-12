```bash
anpanman17@HP-Zorin18:~/Escritorio/SistDeComp/Practico/Sudo-Make-Me-a-Sandwich-TP-s-SdC/TP3-a$ objdump -h aplicacion.so | grep .text
  3 .text         00000300  0000000000002000  0000000000002000  00003000  2**4
anpanman17@HP-Zorin18:~/Escritorio/SistDeComp/Practico/Sudo-Make-Me-a-Sandwich-TP-s-SdC/TP3-a$ objdump -d aplicacion.efi

aplicacion.efi:     formato del fichero pei-x86-64


Desensamblado de la sección .text:

0000000000002000 <_start>:
    2000:	48 83 ec 08          	sub    $0x8,%rsp
    2004:	51                   	push   %rcx
    2005:	52                   	push   %rdx
    2006:	48 8d 3d f3 df ff ff 	lea    -0x200d(%rip),%rdi        # 0 <_start-0x2000>
    200d:	48 8d 35 ec 2f 00 00 	lea    0x2fec(%rip),%rsi        # 5000 <_DYNAMIC>
    2014:	59                   	pop    %rcx
    2015:	5a                   	pop    %rdx
    2016:	51                   	push   %rcx
    2017:	52                   	push   %rdx
    2018:	e8 33 02 00 00       	call   2250 <_relocate>
    201d:	5f                   	pop    %rdi
    201e:	5e                   	pop    %rsi
    201f:	e8 05 00 00 00       	call   2029 <efi_main>
    2024:	48 83 c4 08          	add    $0x8,%rsp

0000000000002028 <.exit>:
    2028:	c3                   	ret

0000000000002029 <efi_main>:
    2029:	f3 0f 1e fa          	endbr64
    202d:	55                   	push   %rbp
    202e:	48 89 e5             	mov    %rsp,%rbp
    2031:	48 83 ec 20          	sub    $0x20,%rsp
    2035:	48 89 7d e8          	mov    %rdi,-0x18(%rbp)
    2039:	48 89 75 e0          	mov    %rsi,-0x20(%rbp)
    203d:	c7 45 fc 01 00 00 00 	movl   $0x1,-0x4(%rbp)
    2044:	90                   	nop
    2045:	8b 45 fc             	mov    -0x4(%rbp),%eax
    2048:	85 c0                	test   %eax,%eax
    204a:	75 f9                	jne    2045 <efi_main+0x1c>
    204c:	48 8d 15 ad 1f 00 00 	lea    0x1fad(%rip),%rdx        # 4000 <_data>
    2053:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
    2057:	48 8b 40 40          	mov    0x40(%rax),%rax
    205b:	48 89 c1             	mov    %rax,%rcx
    205e:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
    2062:	48 8b 40 40          	mov    0x40(%rax),%rax
    2066:	48 8b 40 08          	mov    0x8(%rax),%rax
    206a:	48 89 ce             	mov    %rcx,%rsi
    206d:	48 89 c7             	mov    %rax,%rdi
    2070:	e8 71 00 00 00       	call   20e6 <efi_call2>
    2075:	c6 45 fb cc          	movb   $0xcc,-0x5(%rbp)
    2079:	0f b6 45 fb          	movzbl -0x5(%rbp),%eax
    207d:	3c cc                	cmp    $0xcc,%al
    207f:	75 29                	jne    20aa <efi_main+0x81>
    2081:	48 8d 15 c8 1f 00 00 	lea    0x1fc8(%rip),%rdx        # 4050 <_data+0x50>
    2088:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
    208c:	48 8b 40 40          	mov    0x40(%rax),%rax
    2090:	48 89 c1             	mov    %rax,%rcx
    2093:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
    2097:	48 8b 40 40          	mov    0x40(%rax),%rax
    209b:	48 8b 40 08          	mov    0x8(%rax),%rax
    209f:	48 89 ce             	mov    %rcx,%rsi
    20a2:	48 89 c7             	mov    %rax,%rdi
    20a5:	e8 3c 00 00 00       	call   20e6 <efi_call2>
    20aa:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
    20ae:	48 8b 40 60          	mov    0x60(%rax),%rax
    20b2:	48 8b 80 f8 00 00 00 	mov    0xf8(%rax),%rax
    20b9:	be c0 c6 2d 00       	mov    $0x2dc6c0,%esi
    20be:	48 89 c7             	mov    %rax,%rdi
    20c1:	e8 12 00 00 00       	call   20d8 <efi_call1>
    20c6:	b8 00 00 00 00       	mov    $0x0,%eax
    20cb:	c9                   	leave
    20cc:	c3                   	ret

00000000000020cd <efi_call0>:
    20cd:	48 83 ec 28          	sub    $0x28,%rsp
    20d1:	ff d7                	call   *%rdi
    20d3:	48 83 c4 28          	add    $0x28,%rsp
    20d7:	c3                   	ret

00000000000020d8 <efi_call1>:
    20d8:	48 83 ec 28          	sub    $0x28,%rsp
    20dc:	48 89 f1             	mov    %rsi,%rcx
    20df:	ff d7                	call   *%rdi
    20e1:	48 83 c4 28          	add    $0x28,%rsp
    20e5:	c3                   	ret

00000000000020e6 <efi_call2>:
    20e6:	48 83 ec 28          	sub    $0x28,%rsp
    20ea:	48 89 f1             	mov    %rsi,%rcx
    20ed:	ff d7                	call   *%rdi
    20ef:	48 83 c4 28          	add    $0x28,%rsp
    20f3:	c3                   	ret

00000000000020f4 <efi_call3>:
    20f4:	48 83 ec 28          	sub    $0x28,%rsp
    20f8:	49 89 c8             	mov    %rcx,%r8
    20fb:	48 89 f1             	mov    %rsi,%rcx
    20fe:	ff d7                	call   *%rdi
    2100:	48 83 c4 28          	add    $0x28,%rsp
    2104:	c3                   	ret

0000000000002105 <efi_call4>:
    2105:	48 83 ec 28          	sub    $0x28,%rsp
    2109:	4d 89 c1             	mov    %r8,%r9
    210c:	49 89 c8             	mov    %rcx,%r8
    210f:	48 89 f1             	mov    %rsi,%rcx
    2112:	ff d7                	call   *%rdi
    2114:	48 83 c4 28          	add    $0x28,%rsp
    2118:	c3                   	ret

0000000000002119 <efi_call5>:
    2119:	48 83 ec 28          	sub    $0x28,%rsp
    211d:	4c 89 4c 24 20       	mov    %r9,0x20(%rsp)
    2122:	4d 89 c1             	mov    %r8,%r9
    2125:	49 89 c8             	mov    %rcx,%r8
    2128:	48 89 f1             	mov    %rsi,%rcx
    212b:	ff d7                	call   *%rdi
    212d:	48 83 c4 28          	add    $0x28,%rsp
    2131:	c3                   	ret

0000000000002132 <efi_call6>:
    2132:	48 83 ec 38          	sub    $0x38,%rsp
    2136:	48 8b 44 24 40       	mov    0x40(%rsp),%rax
    213b:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
    2140:	4c 89 4c 24 20       	mov    %r9,0x20(%rsp)
    2145:	4d 89 c1             	mov    %r8,%r9
    2148:	49 89 c8             	mov    %rcx,%r8
    214b:	48 89 f1             	mov    %rsi,%rcx
    214e:	ff d7                	call   *%rdi
    2150:	48 83 c4 38          	add    $0x38,%rsp
    2154:	c3                   	ret

0000000000002155 <efi_call7>:
    2155:	48 83 ec 38          	sub    $0x38,%rsp
    2159:	48 8b 44 24 48       	mov    0x48(%rsp),%rax
    215e:	48 89 44 24 30       	mov    %rax,0x30(%rsp)
    2163:	48 8b 44 24 40       	mov    0x40(%rsp),%rax
    2168:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
    216d:	4c 89 4c 24 20       	mov    %r9,0x20(%rsp)
    2172:	4d 89 c1             	mov    %r8,%r9
    2175:	49 89 c8             	mov    %rcx,%r8
    2178:	48 89 f1             	mov    %rsi,%rcx
    217b:	ff d7                	call   *%rdi
    217d:	48 83 c4 38          	add    $0x38,%rsp
    2181:	c3                   	ret

0000000000002182 <efi_call8>:
    2182:	48 83 ec 48          	sub    $0x48,%rsp
    2186:	48 8b 44 24 60       	mov    0x60(%rsp),%rax
    218b:	48 89 44 24 38       	mov    %rax,0x38(%rsp)
    2190:	48 8b 44 24 58       	mov    0x58(%rsp),%rax
    2195:	48 89 44 24 30       	mov    %rax,0x30(%rsp)
    219a:	48 8b 44 24 50       	mov    0x50(%rsp),%rax
    219f:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
    21a4:	4c 89 4c 24 20       	mov    %r9,0x20(%rsp)
    21a9:	4d 89 c1             	mov    %r8,%r9
    21ac:	49 89 c8             	mov    %rcx,%r8
    21af:	48 89 f1             	mov    %rsi,%rcx
    21b2:	ff d7                	call   *%rdi
    21b4:	48 83 c4 48          	add    $0x48,%rsp
    21b8:	c3                   	ret

00000000000021b9 <efi_call9>:
    21b9:	48 83 ec 48          	sub    $0x48,%rsp
    21bd:	48 8b 44 24 68       	mov    0x68(%rsp),%rax
    21c2:	48 89 44 24 40       	mov    %rax,0x40(%rsp)
    21c7:	48 8b 44 24 60       	mov    0x60(%rsp),%rax
    21cc:	48 89 44 24 38       	mov    %rax,0x38(%rsp)
    21d1:	48 8b 44 24 58       	mov    0x58(%rsp),%rax
    21d6:	48 89 44 24 30       	mov    %rax,0x30(%rsp)
    21db:	48 8b 44 24 50       	mov    0x50(%rsp),%rax
    21e0:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
    21e5:	4c 89 4c 24 20       	mov    %r9,0x20(%rsp)
    21ea:	4d 89 c1             	mov    %r8,%r9
    21ed:	49 89 c8             	mov    %rcx,%r8
    21f0:	48 89 f1             	mov    %rsi,%rcx
    21f3:	ff d7                	call   *%rdi
    21f5:	48 83 c4 48          	add    $0x48,%rsp
    21f9:	c3                   	ret

00000000000021fa <efi_call10>:
    21fa:	48 83 ec 58          	sub    $0x58,%rsp
    21fe:	48 8b 84 24 80 00 00 	mov    0x80(%rsp),%rax
    2205:	00 
    2206:	48 89 44 24 48       	mov    %rax,0x48(%rsp)
    220b:	48 8b 44 24 78       	mov    0x78(%rsp),%rax
    2210:	48 89 44 24 40       	mov    %rax,0x40(%rsp)
    2215:	48 8b 44 24 70       	mov    0x70(%rsp),%rax
    221a:	48 89 44 24 38       	mov    %rax,0x38(%rsp)
    221f:	48 8b 44 24 68       	mov    0x68(%rsp),%rax
    2224:	48 89 44 24 30       	mov    %rax,0x30(%rsp)
    2229:	48 8b 44 24 60       	mov    0x60(%rsp),%rax
    222e:	48 89 44 24 28       	mov    %rax,0x28(%rsp)
    2233:	4c 89 4c 24 20       	mov    %r9,0x20(%rsp)
    2238:	4d 89 c1             	mov    %r8,%r9
    223b:	49 89 c8             	mov    %rcx,%r8
    223e:	48 89 f1             	mov    %rsi,%rcx
    2241:	ff d7                	call   *%rdi
    2243:	48 83 c4 58          	add    $0x58,%rsp
    2247:	c3                   	ret
    2248:	0f 1f 84 00 00 00 00 	nopl   0x0(%rax,%rax,1)
    224f:	00 

0000000000002250 <_relocate>:
    2250:	f3 0f 1e fa          	endbr64
    2254:	48 8b 06             	mov    (%rsi),%rax
    2257:	48 85 c0             	test   %rax,%rax
    225a:	74 7e                	je     22da <_relocate+0x8a>
    225c:	48 83 c6 08          	add    $0x8,%rsi
    2260:	31 d2                	xor    %edx,%edx
    2262:	45 31 c0             	xor    %r8d,%r8d
    2265:	31 c9                	xor    %ecx,%ecx
    2267:	eb 20                	jmp    2289 <_relocate+0x39>
    2269:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
    2270:	48 83 f8 07          	cmp    $0x7,%rax
    2274:	75 06                	jne    227c <_relocate+0x2c>
    2276:	48 8b 16             	mov    (%rsi),%rdx
    2279:	48 01 fa             	add    %rdi,%rdx
    227c:	48 8b 46 08          	mov    0x8(%rsi),%rax
    2280:	48 83 c6 10          	add    $0x10,%rsi
    2284:	48 85 c0             	test   %rax,%rax
    2287:	74 1c                	je     22a5 <_relocate+0x55>
    2289:	48 83 f8 08          	cmp    $0x8,%rax
    228d:	74 51                	je     22e0 <_relocate+0x90>
    228f:	48 83 f8 09          	cmp    $0x9,%rax
    2293:	75 db                	jne    2270 <_relocate+0x20>
    2295:	4c 8b 06             	mov    (%rsi),%r8
    2298:	48 8b 46 08          	mov    0x8(%rsi),%rax
    229c:	48 83 c6 10          	add    $0x10,%rsi
    22a0:	48 85 c0             	test   %rax,%rax
    22a3:	75 e4                	jne    2289 <_relocate+0x39>
    22a5:	48 89 d0             	mov    %rdx,%rax
    22a8:	4c 09 c0             	or     %r8,%rax
    22ab:	74 2d                	je     22da <_relocate+0x8a>
    22ad:	48 85 d2             	test   %rdx,%rdx
    22b0:	74 36                	je     22e8 <_relocate+0x98>
    22b2:	4d 85 c0             	test   %r8,%r8
    22b5:	74 31                	je     22e8 <_relocate+0x98>
    22b7:	48 85 c9             	test   %rcx,%rcx
    22ba:	7e 1e                	jle    22da <_relocate+0x8a>
    22bc:	0f 1f 40 00          	nopl   0x0(%rax)
    22c0:	83 7a 08 08          	cmpl   $0x8,0x8(%rdx)
    22c4:	75 09                	jne    22cf <_relocate+0x7f>
    22c6:	48 8b 02             	mov    (%rdx),%rax
    22c9:	48 01 f8             	add    %rdi,%rax
    22cc:	48 01 38             	add    %rdi,(%rax)
    22cf:	4c 29 c1             	sub    %r8,%rcx
    22d2:	4c 01 c2             	add    %r8,%rdx
    22d5:	48 85 c9             	test   %rcx,%rcx
    22d8:	7f e6                	jg     22c0 <_relocate+0x70>
    22da:	31 c0                	xor    %eax,%eax
    22dc:	c3                   	ret
    22dd:	0f 1f 00             	nopl   (%rax)
    22e0:	48 8b 0e             	mov    (%rsi),%rcx
    22e3:	eb 97                	jmp    227c <_relocate+0x2c>
    22e5:	0f 1f 00             	nopl   (%rax)
    22e8:	48 b8 01 00 00 00 00 	movabs $0x8000000000000001,%rax
    22ef:	00 00 80 
    22f2:	c3                   	ret
    22f3:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
    22fa:	00 00 00 
    22fd:	0f 1f 00             	nopl   (%rax)
```