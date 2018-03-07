ONE Bee: WONBE, WonderWitch BASIC Environment

(Warning: this translation to english was done using google translate, some part of the document may mean nothing, proper translation will come later)

  Provisional manual Ver 0.06 
  January 29, 2001 
  People 
  Kawamata Akira 
  This software is distributed under GNU General Public License 

  ● What is this? 
  One way is a small BASIC interpreter that you can use on WonderWitch. 
  All functions operate on WonderWitch and operate with ordinary communication software connected via serial cable.  When executing a debugged program, neither a serial cable nor communication software is necessary. 
  You can write simple mini games with minimum statement (cls, locate, print) handling WonderWitch's text screen and minimum button input function (wait, scan). 
  When WonderSwan Color is used, 4 color mode is possible. 
  There is also a binary that runs on Win32 for the operation test, but please divide this for the operation check of BASIC body. 
  The document may be a little difficult to see, but I hope that volunteers will make things that are easy to see. 

  ● What you need 
  ・ During development 
  WonderWitch cartridge and serial cable.  Personal computers with communication software capable of non-procedural communication at 38,400 bps. 
  ・runtime 
  Only the cartridge of WonderWitch containing the executed program with the one way 

  ● Preparation 
  ・ Transfer one side (fonbe.fx) to WonderWitch with TransMagic etc. 
  ・ Disconnect the TransMagic etc. and start the non-procedure communication at 38400 bps from the communication software 
  ・ Run one-speed on WonderWitch 
  ・ Press Y2 on the entrance menu screen 
  ・ The name of the one-piece is displayed in the communication software and "* Ready" appears 
  ・ You are now ready 
  ・ Print "hello!" Return and hit the WonderWitch screen.  It is normal if letters appear. 

  ● Execution of developed programs 
  ・ Run one-speed on WonderWitch 
  ・ On the entrance menu screen, select the file you want to execute and press the A button 

  ● Overview of development environment 
  You can use a normal line number based editor. 
  After entering one line following the line number and pressing return, it is recorded in the program. 
  If you enter only the line number and press Return, the line will be deleted. 
  If you enter an instruction without a line number, it will be executed immediately. 
  Editing can only use one character deletion by backspace.  COPY & PASTE from the communication software to the editor on the host, edit it on the editor, paste it on the communication software and send it to the one side is convenient.  However, there is no flow control, so if you paste multiple lines at once, character dropout will occur.  Please do sticking on a line-by-line basis.  It is also effective to edit the program on the host and send it by TransMagic etc. beforehand. 
  Even if you create a program of 2 KB or more at present, you can not save with the save command.  Execute the list command, COPY & PASTE on the communication software and save it on the host. 
  During execution, you can return to the interactive mode at any time by pressing the START button.  You can then continue with the cont statement. 
  Since it is processed after translating it into an intermediate language, if it can not be translated into an intermediate language, an error will be issued immediately after input. 

  ● Language overview 
  It is a very light BASIC language. 
  I have a syntax similar to Tiny BASIC that used to be. 
  The data type is only a signed 16 bit integer. 
  Constants can be described as decimal numbers from -32767 to 32767 or hexadecimal numbers from 0x0000 to 0xffff.  - 32768 can be stored in variables, but it can not be written in source.  (Please write like -32767-1) 
  Variables have 26 alphabetic capital letters of A to Z as global variables. 
  As local variables, there are 26 alphabet lowercase letters a through z.  A local variable is allocated a new area when the gosub statement is executed and the area before gosub is returned when the return statement is executed. 
  Array variables can only be one-dimensional arrays of the form @ (index).  The available size is equal to the remaining size of the program storage area (48 Kbytes).  The index starts at 0.  (Eg for i = 0 to 9: print @ (i): next) 
  Operator precedence is "unary operator> * /> + -> comparison operator> and, or, xor".  (Comparison operator is all operators described with <and> and =) 
  The calculation order can be changed explicitly with parentheses (). 
  There is no overflow or underflow check.  Overflow is ignored.  However, division by zero results in an error. 
  Enter all keywords such as statements in lowercase alphabet.  Even using uppercase letters will be lowercase. 

  ● Statement list 

  Variable = expression 
  Calculate the expression and assign it to the variable.  The let keyword is not supported. 
  Example: a = b + c 

  if expression then statement column 
  If the expression is not 0, execute the statement column.  If it is 0, execution continues from the next line.  There is no else. 
  For example: if (a = 0) and (b = 1) then goto 200 

  print [character string | expression | chr (expression)] [, |;] ... ... 
  Display characters from WonderWitch's text screen's current cursor position. 
  There is no space character before or after the expression. 
  chr (expression) displays the value of the expression as a character code and displays it. 
  Since control codes are not handled, all characters in WonderWitch's character generator can be displayed. 
  The delimiter "; does nothing.  ,, Advances the cursor to the tab stop position. 
  The end of the print statement ends with; or, it does not break a line. 
  Example: print "calculation result =", chr (34); 1 + 2; chr (34) 

  locate expression, expression 
  Set the current cursor position on the WonderWitch text screen.  The arguments are in the order of x and y. 
  Example: locate 10, 10 

  cls 
  Delete the WonderWitch text screen. 
  Example: cls 

  goto expression 
  Jump to the line number of the formula.  Calculated goto statements are possible.  That is, if you set goto A * 100, A will be on the 100th line when the A is on.  If it is 2, it jumps to line 200. 
  Example: goto 100 

  gosub expression 
  Put the current position and local variable on the stack and jump to the line number of the expression.  Calculated gosub is possible as well as goto. 
  The stack is up to 8 levels in combination with for. 
  Example: gosub 100 

  return 
  It retrieves the information gosub loaded on the stack and returns the processing to it.  Also return local variables. 
  Example: return 

  for variable = expression to expression 
  for variable = expression to expression step expression 
  Repeat until next while varying the value of the variable. 
  If the value of the last variable does not match the closing price and the closing price + step value overflows, there is a possibility of an infinite loop.  Please be careful not to specify such a value. 
  Example: for i = 0 to 10 step 2 

  next 
  Go back to the position of the for statement and repeat the operation.  If the condition indicated in the for statement is satisfied, the operation proceeds next.  Writing the variable name after next will result in an error. 
  Example: next 

  end 
  End program execution. 

  break 
  Temporarily suspend execution at that position.  You can continue execution with the cont statement. 

  rem String 
  'String 
  Write a comment.  You can not put a statement behind the statement: 
  Example: rem This is a test program. 
  Example: 'This expression calculates coordinates 

  new 
  Erases the program in memory. 

  list [line number] [-] [line number] 
  List programs with line numbers in the specified range.  If omitted all the lines are listed. 

  run 
  run expression 
  run String 
  Execute the program.  Prior to that, variables and stacks are cleared. 
  run is from the top of the program in memory.  The run expression is from the line number of the expression.  The run string reads a string file and executes it. 
  Example: run 
  Example: run 100 
  Example: run "test.wb" 

  cont 
  Execution resumes from the position stopped by a break statement or a break by the START button. 

  save String 
  Save the program in memory with the string as the file name.  It is saved in text format, not internal representation. 
  Currently, due to restrictions of WonderWitch's file system, files exceeding 2 K bytes can not be saved.  Please send the list to the terminal with the list command, COPY & PASTE it and save it. 
  Example: save "hello.wb" 

  load string 
  Load a program from a file with a string as a file name.  It reads in text format, and line feed can be either CRLF or LF.  If there are lines that can not be translated into an intermediate language, reading is interrupted there.  Execution is interrupted when used during program execution. 
  Example: load "hello.wb" 

  merge string 
  Mix the program in the file with the program in the memory.  The line with the same line number will be given priority when it is read.  If there are lines that can not be translated into an intermediate language, reading is interrupted there.  Execution is interrupted when used during program execution. 
  Example: load "merge.wb" 

  randomize expression 
  Specify the initial value of the random number.  If you do not use this, random numbers will always occur in the same order.  For the argument, it is best to specify the system variable tick. 
  Example: randomize tick 

  exit 
  I will go back to the OS by going through one page. 

  debug [string | expression | chr (expression)] [, |;] ... ... 
  It is equivalent to the print statement but sends it to the serial port instead of the WonderWitch screen.  It is a function dedicated to debugging that can not be used when executing by itself. 

  waitvb expression 
  Wait for VBLANK for the number of expressions.  While waiting, the START button break will not be accepted, so you can limit the value to 750.  An error will occur if you specify a value greater than 750. 
  Example: waitvb 75 

  files 
  Send the file list of the current directory to the serial port.  It is a function dedicated to debugging that can not be used when executing by itself. 

  play String 
  Play the argument string as MML.  To enable it, see "How to activate the play statement". 
  Example: play "cde" 

  poke expression, expression 
  We regard the first expression as an offset, the second expression as a value, and write 1 byte of memory to the segment of the system variable defseg. 
  It does not work with the Win32 version. 
  Note: Since the segment of cseg is flash memory, it can not be written by poke. 

  call expression 
  Assume the expression as an offset and do a machine language level subroutine call (FAR) on the segment of the system variable defseg.  Before calling, the system variables AX, BX, CX, DX, SI, DI, DS, ES are set in their respective registers.  The register state after return is saved in the system variables AX, BX, CX, DX, SI, DI, ES.  (Note: DS will not return) 
  It does not work with the Win32 version. 

  int expression 
  We regard the expression as an interrupt number and perform a system interrupt (int) at machine language level.  You can call WonderWitch's BIOS.  Before calling, the system variables AX, BX, CX, DX, SI, DI, DS, ES are set in their respective registers.  The register state after return is saved in the system variables AX, BX, CX, DX, SI, DI, ES.  (Note: DS will not return) 
  It does not work with the Win32 version. 

  tron 
  Turn on trace mode.  When entering trace mode, each time you start executing a new line, it sends the line number to the debug console. 

  troff 
  Turn off trace mode. 

  colormode expression 
  Set the color mode.  Can be used only with WonderSwan Color. 
  The values ??of the expressions are as follows. 
  0x00 monochrome mode 
  0x80 4 color mode 
  0xC0 16 color mode (It can be set but it can not be drawn from the function of the one side) 
  0xE0 16 color PACKED mode (It can be set but it can not be drawn from the function of the one side) 
  It does not work with the Win32 version. 

  color expression, expression 
  Specify the color of the character to be drawn.  The first expression is the index of the foreground color, and the second expression is the index of the background color.  Only 0 to 3 can be specified, respectively. 
  The initial state at the start of execution is the state corresponding to color 3,0. 
  It does not work with the Win32 version. 

  palette expression, expression, expression, expression 
  Specify the palette.  Specify the index (0 to 3) of the palette set by the first expression.  The second to fourth expressions indicate the values ??of R, G, B respectively.  The values ??of R, G, and B are each in the range of 0 to 15. 
  It does not work with the Win32 version. 
  Example: palette 0, 15, 0, 0 

  ● Binary operator 
  + - * / <> = <=> = <> and or xor 

  ● Unary operator 
  not 

  ● Function 

  rnd (expression) 
  Returns a random number in the range from 0 to expression -1. 

  abs (expression) 
  Returns the absolute value of an expression. 

  varptr (variable) 
  Returns the offset of the specified variable. 
  It does not work with the Win32 version. 

  varseg (variable) 
  Returns the segment of the specified variable. 
  It does not work with the Win32 version. 

  peek (expression) 
  We regard the expression as an offset and read 1 byte of memory from the segment of the system variable defseg. 
  It does not work with the Win32 version. 

  ● System variables (readable and writable) 

  ax, bx, cx, dx, si, di, ds, es 
  It is used to specify the register value when call and int statements are used and to save the value of the register after return. 

  defseg 
  It holds segment value used in peek function, poke, call statement.  The initial value is the value of dseg. 

  ● System variables (read only) 

  scan 
  It examines the state of WonderSwan's button and returns it as one integer value.  You can determine which button is pressed by taking a system variable beginning with SCAN_ and and.  Since the START button is suspended, this can not be handled. 

  wait 
  Wait until the button of WonderSwan is pressed and return its state as one integer value.  By taking the system variable beginning with SCAN_ and and, you can determine which button was pressed.  Since the START button is suspended, this can not be handled. 

  scan_a 
  scan_b 
  scan_x1 
  scan_x2 
  scan_x 3 
  scan_x 4 
  scan_y1 
  scan_y 2 
  scan_y 3 
  scan_y 4 
  Returns a constant indicating the mask value of each button. 
  To check whether the A button is pressed, write it as in the example below. 
  Example: if scan and scna_a then print "A button is pressed" 

  tick 
  Returns the system timer tick.  However, it is important to note that the original tick value is an unsigned 32 bit integer, and that obtained here is a 16 bit signed integer.  The overflowing bits are thrown away. 

  dseg 
  Returns the value of the DS segment register being executed.  Variables and intermediate language code of the program are all in this segment. 
  It does not work with the Win32 version. 

  cseg 
  Returns the value of the CS segment register being executed.  One program's main program is in this segment. 
  It does not work with the Win32 version. 

  sseg 
  Returns the value of the running SS segment register.  There is a machine language stack in this segment. 
  It does not work with the Win32 version. 

  hardarch 
  Returns whether or not WonderSwan currently running is a Color model.  0 for monochrome WonderSwan.  WonderSwan Color will be 1. 
  It does not work with the Win32 version. 

  ● File extension 
  Please make sure that the source fillet of one bee is a file name ending with ". Wb".  You can load with commands such as load regardless of the extension, but only the filename that ends with ".wb" is displayed in the entrance menu. 

  ● How to activate the play statement 
  Transfer sound.il to / rom0.  Without this file, no sound will be emitted even if you use the play statement.  If there is no sound.il, the sound is not produced, and the program is executed. 
  In the Win32 version, you can play MML using dsakura.dll included in "text music" Sakura "by whale flying machine.This software is available from http://www.text2music.com/ You can put dsakura.dll anywhere you go through the environment variable path.The only file you need is this DLL.If you do not have dsakura.dll, the program will just run with no sounds However, please note that the specification of MML is not exactly the same.In Win32 version, a file called $$$. Mid is generated in the current directory when the play statement is executed.Delete it after the execution does not matter. 

  Distribution rules 
  From this version, we assume that this software follows the GNU General Public License (GPL).  Details are at http://www.gnu.org/. 
  In short, anyone can use it for any purpose.  However, you should not sell this software.  In addition, source code must be released.  It is possible to record it in the product, but you should not prevent the user from freely copying and distributing this software. 
  As of December 2000, the source code is managed by PD Kawamata Akira (autumn@piedey.co.jp).  If you feed back useful source code fixes, it will be reflected in this source tree. 

  Digression
  My name is Mr. Kawamata made by gratitude and discretion for the 20th century.  In other words, for me, the incident that I encountered a microcomputer / personal computer was the biggest event of the 20th century, and the very small interpreter languages ??Tiny BASIC, VTL, GAME, TL / 1 played a big role there, have a big impression I left it inside of me.  And, at that time, I also wanted to make programming languages ??by themselves, and it was also true that I worked hard.  After all, I developed several toy languages.  I also made a practical language if necessary.  When porting TOKYO Nanpa Street of ENIX from FM - 7 to PC - 8801, I also made an integer type compact interpreter to increase memory efficiency.  However, with the assembler development system AZAS of my own Z-80 which finally contained the odor of structured assembler finely, the world of my own language came to the terminal station.  Clearly, at this point there was no point in developing a language like Tiny BASIC.  Assembler level development was indispensable to obtain the necessary performance.  I could make it, but I had no meaning to make.  And, as I moved to the era of 16 bit era, I stopped using my own language and started to use a pretty good existing processing system.  MASM 1.27 ~, Turbo Pascal 3.0, Turbo C 1.5 ~ 2.0, MS - C 6.0 ~, Visual BASIC 1.0 ~, JDK 1.02 ~ etc etc etc.  However, when I look back at the end of the 20th century, I remembered that simple Tiny BASIC that I admired in a boyhood when I do not know right or left.  That's why I made it with the feeling that this is the 20th century for me. 
  That is why there are certain generations.  To make games with BASIC, there seem to be generations such as MSX and Baumaga, but clearly speaking, I think that one generation can not understand this generation.  BASIC on MSX is an ultra-high-performance huge BASIC, and the generation is completely different from Tiny BASIC.  Even in generations back to PC - 8801 and FM - 7, you will not know yet.  After all, in order to understand this, before the PC is called a personal computer, it is called a microcomputer, it is necessary to go back to the era when the board was sold in a bare state without fitting in the case.  In other words, in such a period, what you sacrifice for a romantic era when you try to gain something, a handmade element enters and it is said to be this one.  And, as a foundation suitable for reproducing that romance, I think that a product named WonderWitch appeared in this era also is a mysterious edge. 
  So, there is one thing I would like to say to a user.  Share this romance.  In other words, if you do not like it, remodel it quickly.  In order to guarantee that it can be used by remodeling freely as you like, the source is GPL. 

  ● Included one-page program 
  ▼ Demo 
  colordemo.wb 
  Demo of color function (only for WonderSwanColor) 
  ▼ Game 
  ctb.wb 
  Fall game 
  janken.wb 
  Janken game 
  shoot.wb 
  shooting game 
  star.wb 
  STAR TREK-like game "STAR WITCH" (see bundled star.txt) 
  Please refer to how to write the program 
  ▼ Test 
  test.wb 
  Simple interpreter operation test 
  testcall.wb 
  Testing the operation of the call statement 
  testcont.wb 
  Testing the behavior of the cont statement 
  testfor.wb 
  Test operation of for statement 
  testgosub.wb 
  Testing the operation of the gosub statement 
  testif.wb 
  Operation test of if statement 
  testint.wb 
  Testing the operation of int statements 
  testlocate.wb 
  Testing the operation of the locate statement 
  testlongloop.wb 
  Test of interruption by START key 
  testpeekpoke.wb 
  peek function test poke statement 

  ● Change history 

  January 29, 2001 Ver 0.06 
  ・ Transfer to WonderSwan Color development kit 
  ・ Add colormode, palette, color statement, hardarch system variable 
  ・ Add a digression to the document 

  December 10, 2000 Ver 0.05 
  ・ Hirotaka JOE Ohkubo's difference (0.04j4) can be merged with play statement (sound.il required) 
  ・ Consider? As the keyword debug 
  ・ Allow goto to be omitted if immediately after it is a decimal integer value 
  ・ Addition of machine language related functions 
  ・ Add tron ??/ troff command (display execution line number) 

  November 12, 2000 Ver 0.04 
  ・ When trying to delete a line number including 0x0d such as 13, the program took a broken bug 
  ・ Change processing system from Turbo C2.0 to Visual C ++ 1.51 
  ・ Significantly improved loading speed 
  ・ Only the .wb entrance menu is displayed 
  ・ Sample game STAR WITCH (star.wb, star.txt) attached 

  November 7, 2000 Ver 0.03 
  ・ Operator precedence order Unary operator> * /> + -> Comparison operator> and, or, xor 
  ・ Change the value when the comparison operator is established from 1 to -1 
  ・ Change prompt to OK 
  - Fixed a bug that negative numbers were output as positive numbers by print / debug 
  ・ Changed so that loop can escape in next even if for closing price does not match exactly 
  November 7, 2000 Ver 0.02 scan_XX took a bug that does not work 
  November 6, 2000 Ver 0.01 First version 

  ● Contact Information 
  Paiday Co., Ltd. http://www.piedey.co.jp/ 
  Kawamata Akira autumn@piedey.co.jp 

  that's all 