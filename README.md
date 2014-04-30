MLP
===

Machine Language Program

Reads in Hex Instructions and executes them

Based on Simple Simulator instructions

OpCode/
Operands  Instruction       Operation
  2  RXY    load  R,XY        register[R]:=XY
  
  1  RXY    load  R,[XY]      register[R]:=memory[XY]
  
  3  RXY    store R,[XY]      memory[XY]:=register[R]
  
  D  0RS    load  R,[S]       register[R]:=memory[register[S]]
  
  E  0RS    store R,[S]       memory[register[S]]:=register[R]
  
  4  0RS    move  S,R         register[S]:=register[R]
  
  5  RST    addi  R,S,T       register[R]:=register[S]+register[T]
                                 integer add
                                 
  6  0      Input             register[E]:= Input from Keyboard
                                 
  7  RST    or    R,S,T       register[R]:=register[S] OR register[T]
                                 bitwise OR
                                 
  8  RST    and   R,S,T       register[R]:=register[S] AND register[T]
                                 bitwise AND
                                 
  9  RST    xor   R,S,T       register[R]:=register[S] XOR register[T]
                                 bitwise eXclusive OR
                                 
  A  R0X    ror   R,X         register[R]:=register[R] ROR X
                                 ROtate Right register R for X times
                                 
  B  RXY    jmpEQ R=R0,XY     PC:=XY, if R=R0
  
     0XY    jmp   XY          PC:=XY
     
  F  RXY    jmpLE R<=R0,XY    PC:=XY, if R<=R0
  
  C  000    halt              halt program

