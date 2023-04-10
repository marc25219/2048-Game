section .data               
;Canviar Nom i Cognom per les vostres dades.
developer db "_Marc _Ciruelos_",0

;Constants que també estan definides en C.
DimMatrix    equ 4
SizeMatrix   equ DimMatrix*DimMatrix ;=16
rowScreenAux equ 10
colScreenAux equ 17

section .text        
;Variables definides en Assemblador.
global developer                        

;Subrutines d'assemblador que es criden des de C.
global showNumberP1, updateBoardP1, copyMatrixP1, rotateMatrixRP1
global shiftNumbersRP1, addPairsRP1
global readKeyP1, playP1

;Variables definides en C.
extern rowScreen, colScreen, charac, number
extern m, mRotated, score, state

;Funcions de C que es criden des de assemblador
extern clearScreen_C, printBoardP1_C, gotoxyP1_C, getchP1_C, printchP1_C
extern insertTileP1_C, printMessageP1_C

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ATENCIÓ: Recordeu que en assemblador les variables i els paràmetres 
;;   de tipus 'char' s'han d'assignar a registres de tipus  
;;   BYTE (1 byte): al, ah, bl, bh, cl, ch, dl, dh, sil, dil, ..., r15b
;;   les de tipus 'short' s'han d'assignar a registres de tipus 
;;   WORD (2 bytes): ax, bx, cx, dx, si, di, ...., r15w
;;   les de tipus 'int' s'han d'assignar a registres de tipus 
;;   DWORD (4 bytes): eax, ebx, ecx, edx, esi, edi, ...., r15d
;;   les de tipus 'long' s'han d'assignar a registres de tipus 
;;   QWORD (8 bytes): rax, rbx, rcx, rdx, rsi, rdi, ...., r15
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Les subrutines en assemblador que heu d'implementar són:
;;   showNumberP1, updateBoardP1, copyMatrixP1,  
;;   rotateMatrixRP1, shiftNumbersRP1, addPairsRP1.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Aquesta subrutina es dóna feta. NO LA PODEU MODIFICAR.
; Situar el cursor a la fila indicada per la variable (rowScreen) i a 
; la columna indicada per la variable (colScreen) de la pantalla,
; cridant la funció gotoxyP1_C.
; 
; Variables globals utilitzades:   
; (rowScreen): Fila de la pantalla on posicionem el cursor.
; (colScreen): Columna de la pantalla on posicionem el cursor.
; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
gotoxyP1:
   push rbp
   mov  rbp, rsp
   ;guardem l'estat dels registres del processador perquè
   ;les funcions de C no mantenen l'estat dels registres.
   push rax
   push rbx
   push rcx
   push rdx
   push rsi
   push rdi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15

   call gotoxyP1_C
 
   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rdi
   pop rsi
   pop rdx
   pop rcx
   pop rbx
   pop rax

   mov rsp, rbp
   pop rbp
   ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Aquesta subrutina es dóna feta. NO LA PODEU MODIFICAR.
; Mostrar un caràcter guardat a la variable (charac) a la pantalla, 
; en la posició on està el cursor, cridant la funció printchP1_C
; 
; Variables globals utilitzades:   
; (charac): Caràcter que volem mostrar.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
printchP1:
   push rbp
   mov  rbp, rsp
   ;guardem l'estat dels registres del processador perquè
   ;les funcions de C no mantenen l'estat dels registres.
   push rax
   push rbx
   push rcx
   push rdx
   push rsi
   push rdi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15

   call printchP1_C
 
   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rdi
   pop rsi
   pop rdx
   pop rcx
   pop rbx
   pop rax

   mov rsp, rbp
   pop rbp
   ret
   

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Aquesta subrutina es dóna feta. NO LA PODEU MODIFICAR.
; Llegir una tecla i guarda el caràcter associat a la variable (charac)
; sense mostrar-la per pantalla, cridant la funció getchP1_C. 
; 
; Variables globals utilitzades:   
; (charac): Caràcter que llegim de teclat.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
getchP1:
   push rbp
   mov  rbp, rsp
   ;guardem l'estat dels registres del processador perquè
   ;les funcions de C no mantenen l'estat dels registres.
   push rax
   push rbx
   push rcx
   push rdx
   push rsi
   push rdi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15
   push rbp

   call getchP1_C
 
   pop rbp
   pop r15
   pop r14
   pop r13
   pop r12
   pop r11
   pop r10
   pop r9
   pop r8
   pop rdi
   pop rsi
   pop rdx
   pop rcx
   pop rbx
   pop rax
   
   mov rsp, rbp
   pop rbp
   ret 
   

;;;;;
; Convertir el número de la variable (number) de tipus int (DWORD) de 6
; dígits (number <= 999999) a caràcters ASCII que representen el seu valor.
; Si (number) és més gran que 999999 canviarem el valor a 999999.
; S'ha de dividir el valor entre 10, de forma iterativa, fins 
; obtenir els 6 dígits.
; A cada iteració, el residu de la divisió que és un valor entre (0-9) 
; indica el valor del dígit que s'ha de convertir a ASCII ('0' - '9') 
; sumant '0' (48 decimal) per a poder-lo mostrar.
; Quan el quocient sigui 0 mostrarem espais a la part no significativa.
; Per exemple, si number=103 mostrarem "   103" i no "000103".
; S'han de mostrar els dígits (caràcter ASCII) des de la posició 
; indicada per les variables (rowScreen) i (colScreen), posició de les 
; unitats, cap a l'esquerra.
; El primer dígit que obtenim són les unitats, després les desenes,
; ..., per a mostrar el valor s'ha de desplaçar el cursor una posició
; a l'esquerra a cada iteració.
; Per a posicionar el cursor cridar a la subrutina gotoxyP1 i per a 
; mostrar els caràcters a la subrutina printchP1.
;
; Variables globals utilitzades:   
; (number)   : Número que volem mostrar.
; (rowScreen): Fila de la pantalla on posicionem el cursor.
; (colScreen): Columna de la pantalla on posicionem el cursor.
; (charac)   : Caràcter a escriure a pantalla.
;;;;;
showNumberP1:
   push rbp
   mov  rbp, rsp
   
   push rax
   push rbx
   push rcx
   push rdx
   push r8
  
   ; Assignació del valor de colScreen i number als registres ecx, eax
   ; de 32 bits (int). A més, R8 correspon al iterador (i).
   mov ecx, DWORD[colScreen]		
   mov eax, DWORD[number] 			
   mov r8d, 1 						
   
   ; Es compara el valor de la variable number. Si es major de 999999, 
   ; s'assigna 999999 com a valor per defecte.
   cmp eax, 999999					
   jle showNumberP1_for										
   mov eax, 999999					
   
   ; Aquest bucle converteix de digit en digit al format ASCII, per 
   ; finalment mostrar per pantalla el valor corresponent
   showNumberP1_for:
   
		; S'assigna ' ' al caracter (espai vuit = ALT + 32)
		mov BYTE[charac], ' '
		
		; Es compara el valor del dígit amb 0, i llavors:  
		;
		;	+ Si és més gran  -> Es procedeix a l'operació de conversió
		;					     del dígit a ASCII
		;
		; 	+ Si és més petit 
		;			  o igual -> Vol dir que el dígit és 0, i 
		;                        per tant, no s'ha de fer la conversió
		;                        a ASCII, ja que 0 és mostra com un espai
		cmp eax, 0
		jle not_operate

		; Divisió del dígit entre 10
		mov edx, 0								;Residu = 0
		mov ebx, 10								;Divisor = 10
		div ebx									;Divisió (EAX/EBX)
		
		; Conversió del residu de l'anterior divisió per obtenir
		; el digit principal en format ASCII
		mov BYTE[charac], dl		
		add BYTE[charac], '0'
		
		; Posicionar el cursor i mostrar el nombre per pantalla
		not_operate:
		call gotoxyP1
		call printchP1
		dec DWORD[colScreen]	
	  
   ; Comparació del bucle per iterar fins a 6 vegades.
   showNumberP1_for_end:
   		inc r8d
		cmp r8d, 6
		jle	showNumberP1_for
   
   ; Es restaura el cursor al punt inicial
   mov DWORD[colScreen], ecx
   	
   pop r8 
   pop rdx 
   pop rcx
   pop rbx 
   pop rax
   
   mov rsp, rbp
   pop rbp
   ret


;;;;;
; Actualitzar el contingut del Tauler de Joc amb les dades de la matriu 
; (m) i els punts del marcador (score) que s'han fet.  
; S'ha de recórrer tota la matriu (m), i per a cada element de la matriu
; posicionar el cursor a la pantalla i mostrar el número d'aquella 
; posició de la matriu.
; Per recórrer la matriu en assemblador l'índex va de 0 (posició [0][0])
; a 30 (posició [3][3]) amb increments de 2 perquè les dades son de 
; tipus short(WORD) 2 bytes.
; Després, mostrar el marcador (score) a la part inferior del tauler,
; fila 18, columna 26 cridant la subrutina showNumberP1.
; Finalment posicionar el cursor a la fila 18, columna 28 cridant la 
; subrutina goroxyP1.
;
; Variables globals utilitzades:   
; (rowScreen): Fila de la pantalla on posicionem el cursor.
; (colScreen): Columna de la pantalla on posicionem el cursor.
; (m)        : Matriu on guardem els nombres del joc.
; (score)    : Punts acumulats al marcador.
; (number)   : Número que volem mostrar.
;;;;;  
updateBoardP1:
   push rbp
   mov  rbp, rsp

   push rax
   push rbx
   push rcx
   push rdx
   
   ; Assignació de la posició auxiliar a les posicions del cursor     
   mov DWORD[rowScreen], rowScreenAux
   mov DWORD[colScreen], colScreenAux
   
   ; Inicialització dels registres que serviran per iterar
   mov eax, 0												; i
   mov ebx, 0												; j
   mov ecx, 0												; Index
   
   ; Aquest bucle actualitza cadascuna de les posicions de la matriu
   updateBoardP1_for:
		
		; Es mostra el numero que está en al posició referenciada per 
		; l'index. Es crida la subrutina showNumberP1
		movsx edx, WORD[m+ecx]        	
		mov DWORD[number], edx			
		call showNumberP1				
		
		; S'incrementa en dos l'index degut al short(WORD)
		add ecx, 2						
		; S'incrementa la posicioó horitzontal
		add DWORD[colScreen], 9			
		
		; S'incrementa en 1 l'iterador corresponent a j i es compara
		; si ha arribat a l'última columna d'una fila en concret. 
		; En cas contrari, comença de nou a l'etiqueta updateBoardP1_for
		inc eax							
		cmp eax, DimMatrix				
		jl updateBoardP1_for			
		
		; Es comença a iterar en la següent fila, i s'augmenta l'iterador
		; i. A més, s'augmenta la posició de la fila. També es reseteja 
		; la posició de la columna a 0.
		mov eax, 0						
		mov DWORD[colScreen], colScreenAux	
		add DWORD[rowScreen], 2				
		inc ebx								
		
		; Si s'ha iterat per tota la matriu, el bucle termina i, no salta
		; i segueix amb la següent instrucció
		cmp ebx, DimMatrix					
		jl updateBoardP1_for				
	
		; S'assigna el valor de score al la variable number
		mov edx, DWORD[score]				
		mov DWORD[number], edx				
		
		; Es situa el cursor a la posició de la puntuació i es mostra
		mov DWORD[colScreen], 26			
		call showNumberP1					
		
		; Es situa el cursor a la posició 28
		mov DWORD[colScreen], 28			
		call gotoxyP1						

   pop rdx 
   pop rcx
   pop rbx 
   pop rax
   
   mov rsp, rbp
   pop rbp
   ret



;;;;;  
; Copiar els valors de la matriu (mRotated) a la matriu (m).
; La matriu (mRotated) no s'ha de modificar, 
; els canvis s'han de fer a la matriu (m).
; Per recórrer la matriu en assemblador l'índex va de 0 (posició [0][0])
; a 30 (posició [3][3]) amb increments de 2 perquè les dades son de 
; tipus short(WORD) 2 bytes.
; No es mostrar la matriu.
;
; Variables globals utilitzades:   
; (m)       : Matriu on guardem els nombres del joc.
; (mRotated): Matriu amb els nombres rotats a la dreta.
;;;;;  
copyMatrixP1:
   push rbp
   mov  rbp, rsp
   
   push rax
   push rbx
   push rcx
   push rdx

   ; Inicialització dels registres que serviran per iterar
   mov eax, 0							; i
   mov ebx, 0							; j
   mov ecx, 0							; Index
   
   ; Copia el valor de la matriu rotada en la matriu final mitjançant
   ; un bucle
   copyMatrixP1_for:				
		mov dx, WORD[mRotated+ecx]		
		mov WORD[m+ecx], dx				
		
		; S'incrementa l'index de les matrius
		add ecx, 2						
		inc eax							
		
		; Es compara si les columnes de la primera fila han sigut
		; iterades. Si no, comença de nou a l'etiqueta copyMatrixP1_for
		cmp eax, DimMatrix				
		jl copyMatrixP1_for
		
		; La fila ha sigut completada, i per tant, es pasa a iterar la 
		; següent fila. Per això, s'assigna a 0 l'iterador j i s'incrementa
		; el corresponent a la fila.
		mov eax, 0						
		inc ebx							
		
		; El bucle termina quan finalment s'ha iterat tota la matriu
		cmp ebx, DimMatrix				
		jl copyMatrixP1_for
		

   pop rdx 
   pop rcx
   pop rbx 
   pop rax
     
   mov rsp, rbp
   pop rbp
   ret


;;;;;      
; Rotar a la dreta la matriu (m), sobre la matriu (mRotated). 
; La primera fila passa a ser la quarta columna, la segona fila passa 
; a ser la tercera columna, la tercera fila passa a ser la segona 
; columna i la quarta fila passa a ser la primer columna.
; A l'enunciat s'explica en més detall com fer la rotació.
; NOTA: NO és el mateix que fer la matriu transposada.
; La matriu (m) no s'ha de modificar, 
; els canvis s'han de fer a la matriu (mRotated).
; Per recórrer la matriu en assemblador l'índex va de 0 (posició [0][0])
; a 30 (posició [3][3]) amb increments de 2 perquè les dades son de 
; tipus short(WORD) 2 bytes.
; Per a accedir a una posició concreta de la matriu des d'assemblador 
; cal tindre en compte que l'índex és:(index=(fila*DimMatrix+columna)*2),
; multipliquem per 2 perquè les dades son de tipus short(WORD) 2 bytes.
; No s'ha de mostrar la matriu.
; Un cop s'ha fet la rotació, copiar la matriu (mRotated) a la matriu (m)
; cridant la subrutina copyMatrixP1.
; 
; Variables globals utilitzades:   
; (m)       : Matriu on guardem els nombres del joc.
; (mRotated): Matriu amb els nombres rotats a la dreta.
;;;;;  
rotateMatrixRP1:
   push rbp
   mov  rbp, rsp
   
   push rax
   push rbx
   push rcx
   push rdx
   push r8

   ; Inicialització dels registres que serviran per iterar
   mov ebx, 0							; j
   mov ecx, 3							; i = d'abaix a dalt
   mov r8, 0							; Index

   ; Bucle que itera per tota la matriu fins invertirla 90º a la dreta. 
   ; Els valors dels iteradors són diferents perque serveixen per calcular 
   ; posteriorment la posició de destí.
   rotateMatrixRP1_for:
   		
   		; S'assigna el valor del nombre a rotar a la nova possició
		mov dx, WORD[m+r8]		        
		
		; Es calcula la posició de destinació 
		mov eax, DimMatrix				
		imul eax, ebx					
		add eax, ecx					
		imul eax, 2						
		
		;S'assigna el valor del nomber a la nova possició
		mov WORD[mRotated+eax], dx		
		
		;S'incrementa la posició de la matriu origen
		add r8, 2						
		
		; S'incrementa la posició de les columnes fins recorrer 
		; totes les columnes d'una fila	
		inc ebx							
		cmp ebx, DimMatrix				
		jl rotateMatrixRP1_for
		
		; Es decrementa la fila fins que s'arriba a la fila inicial
		mov ebx, 0						
		dec ecx							
		
		; Si es recorre tota la matriu, termina el bucle
		cmp ecx, 0						
		jge rotateMatrixRP1_for

   ; Es fa la crida per a copiar la matriu
   call copyMatrixP1

   pop r8
   pop rdx 
   pop rcx
   pop rbx 
   pop rax
   
   mov rsp, rbp
   pop rbp
   ret


;;;;;  
; Desplaça a la dreta els números de cada fila de la matriu (m),
; mantenint l'ordre dels números i posant els zeros a l'esquerra.
; Recórrer la matriu per files de dreta a esquerra i de baix a dalt.
; Per recórrer la matriu en assemblador, en aquest cas, l'índex va de la
; posició 30 (posició [3][3]) a la 0 (posició [0][0]) amb decrements de
; 2 perquè les dades son de tipus short(WORD) 2 bytes.
; Para recorrer la matriz en ensamblador el índice va de 0 (posición [0][0])
; a 30 (posiciones [3][3]) con incrementos de 2 porquè los datos son de
; tipo short(WORD) 2 bytes.
; Si es desplaça un número (NO ELS ZEROS), posarem la variable 
; (state) a '2'.
; A cada fila, si troba un 0, mira si hi ha un número diferent de zero,
; a la mateixa fila per a posar-lo en aquella posició.
; Si una fila de la matriu és: [2,0,4,0] i state = '1', quedarà [0,0,2,4] 
; i state = '2'.
; Els canvis s'han de fer sobre la mateixa matriu.
; No s'ha de mostrar la matriu.
;
; Variables globals utilitzades:   
; (m)    : Matriu on guardem els nombres del joc.
; (state): Estat del joc. ('2': S'han fet moviments).
;;;;;  
shiftNumbersRP1:
   push rbp
   mov  rbp, rsp

   push rax
   push rbx
   push rcx
   push rdx
   push r8
   push r9
   push r10
	
   ; Inicialització dels registres que serviran per iterar
   mov r8d, 3								; j (de dreta a esquerra)
   mov r9d, 3								; i (baix a dalt)
   mov ecx, 30								; Index
   
    ; Aquest bucle recorre tota la matriu per a comprobar i desplaçar
    ; els numeros a la dreta i els espais (0) a l'esquerra
	shiftNumbersRP1_start:
		
		; Comproba que la posició és igual a 0
		; 	Si no és igual -> Continua a la següent posició
		; 	Si és igual    -> S'assigna a un registre la posició anterior
		cmp WORD[m+ecx], 0					
		jne shiftNumbersRP1_next_position
		mov r10d , r8d						
		dec r10d							
		
		; Aquest bucle itera totes les posicions de la fila fins a 
		; trobar un valor diferent de 0 o arribar al final. 
		; Tanmateix, calcula la posició de cada moviment previament
		; per a poder realitzar els desplazaments i les comparacions
		shiftNumbersRP1_while: 		
			mov eax, DimMatrix
		   imul eax, r9d
			add eax, r10d
		   imul eax, 2							
			
			; Es compara el valor de la posició amb 0
			; 	Si es igual --> continua 
			; 	Si no és igual --> finalitza el bucle
			cmp WORD[m+eax], 0					
			jne shiftNumbersRP1_end_while
			
			; Compara la posició anterior amb 0
			; 	Si és més petit de 0   -> arriba al final de la fila
		    ;							  i termina el bucle
			; 	Si és més gran o igual -> decrementa una posició la 
			;                             la posició de la columna
			cmp r10d, 0							
			jl  shiftNumbersRP1_end_while			
			dec r10d							
			
			; Es torna a fer una iteració fins recorrer tota la fila
			jmp shiftNumbersRP1_while
			
		; En aquesta condició s'avalua que la posició anterior guardada
		; no sigui inferior a zero. En aquest cas, es fa un intercanvi 
		; amb la seguent posició, deixant el zero a la posició de l'esquerra
		; i el numero a la posició de la dreta. 	
		shiftNumbersRP1_end_while:
		cmp r10d, 0							
		jge shiftNumbersRP1_replace
		mov r8d, 0							
		mov ecx, DimMatrix					
	   imul ecx, r9d
	   imul ecx, 2
		
		; Continua mirant la següent posició    
		jmp shiftNumbersRP1_next_position	
	
		; Es fa l'intercambi d'una posició amb 0 per una posició amb 
		; un valor diferent de cero posicionat més a l'esquerra. 
		shiftNumbersRP1_replace: 				
			; Es calcula la posició
			mov dx, WORD[m+eax]					
			mov ebx, DimMatrix					
			imul ebx, r9d
			add ebx, r8d
			imul ebx, 2							
			
			; Es realitza l'intercanvi dels valors
			mov WORD[m+ebx], dx					
			; S'assigna 0 el valor de l'esquerra
			mov WORD[m+eax], 0					
			; S'assigna un 2 a la variable state
			mov DWORD[state], '2'				
					
		; Es decrementa la posició de la matriu per a seguir fent el bucle
		shiftNumbersRP1_next_position:
			sub ecx, 2							
			
			; Es decrementa en 1 el valor de la posició de les columnes
			; fins arribar a la primera posició de la fila.
			dec r8d								
			cmp r8d, 0							
			jge shiftNumbersRP1_start
			
			; Es decrementa en 1 el valor de la posició de les files
			; fins arribar a recorrer tota la matriu sencera
			mov r8d, 3							
			dec r9d								
			cmp r9d, 0							
			jge shiftNumbersRP1_start

   pop r8
   pop r9
   pop r10
   pop rdx 
   pop rcx
   pop rbx 
   pop rax
   
   mov rsp, rbp
   pop rbp
   ret
      

;;;;;  
; Aparellar nombres iguals des de la dreta de la matriu (m) i acumular 
; els punts al marcador sumant el punts de les parelles que s'hagin fet.
; Recórrer la matriu per files de dreta a esquerra i de baix a dalt. 
; Quan es trobi una parella, dos caselles consecutives amb el mateix 
; número, ajuntem la parella posant la suma de la parella a la casella 
; de la dreta, un 0 a la casella de l'esquerra i 
; acumularem aquesta suma (punts que es guanyen).
; Si una fila de la matriu és: [8,4,4,2] i state = 1'', quedarà [8,0,8,2], 
; p = p + (4+4) i state = '2'.
; Si al final s'ha ajuntat alguna parella (punts>0), posarem la variable 
; (state) a '2' per a indicar que s'ha mogut algun nombre i actualitzarem 
; la variable (score) amb els punts obtinguts de fer les parelles.
; Per recórrer la matriu en assemblador, en aquest cas, l'índex va de la
; posició 30 (posició [3][3]) a la 0 (posició [0][0]) amb increments de 
; 2 perquè les dades son de tipus short(WORD) 2 bytes.
; Els canvis s'han de fer sobre la mateixa matriu.
; No s'ha de mostrar la matriu.
;
; Variables globals utilitzades:   
; (m)    : Matriu on guardem els nombres del joc.
; (score): Punts acumulats al marcador.
; (state): Estat del joc. ('2': S'han fet moviments).
;;;;;  
addPairsRP1:
   push rbp
   mov  rbp, rsp

   push rax
   push rbx
   push rcx
   push rdx
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
	

   ; Inicialització dels registres que serviran per iterar
   mov r8d, 3								; j
   mov r9d, 3								; i
   mov r11d, 0								; p
   mov r12d, 0								; Nou index (j-1)
   mov ecx, 30								; Index (m) 
  
	; Aquest bucle comproba si posició per posició si el valor de la posicio
	; de la matriu i la seva anterior son iguals. 
	addPairsRP1_start:
	    movsx r13d, WORD[m+ecx]				
	    
	    ; Si el valor de la posició es 0, continua buscant la següent posició
		cmp r13d, 0
		je addPairsRP1_next_position
		
		; En cas contrari, calcula la posició anterior i comproba si son
		; iguals:
		;	Si son iguals -> Fa la suma dels nombres
		; 	Si no són iguals -> Continua buscant la següent posició
		mov r10d , ecx						; J-1 = R10  // R8D = J
		sub r10d , 2						; J-1 = R10  // R8D = J
	   movsx r12d, WORD[m+r10d]				; m[i][j-1]
	    cmp r12d, r13d
	    jne addPairsRP1_next_position	
		
	; Realitza la suma dels nombres i l'hi assigna 0 a la posició de l'esquerra
	addPairsRP1_join:
	   shl r13d, 1
	   mov WORD[m+ecx], r13w 
	   mov r12d, 0
	   mov WORD[m+r10d], r12w
	   add r11d, r13d  
   
   ; Calcula l'anterior posició per a realitzar la comparació
   addPairsRP1_next_position:
		sub ecx, 2							; decrementar posición indice
		
		; Es decrementa en 1 el valor de la posició de les columnes
		; fins arribar a la primera posició de la fila.
		dec r8d								; j--
		cmp r8d, 0							; if j >= 0
		jg addPairsRP1_start
		
		; S'assegura de que al terminar la fila, la posició és la 
		; següent fila. A més, reinicia el comptador de la columna a 3
		sub ecx, 2
		mov r8d, 3							;j = 3
		
		; Es decrementa en 1 el valor de la posició de les files
		; fins arribar a recorrer tota la matriu sencera
		dec r9d								;i--
		cmp r9d, 0							;if i >= 0
		jge addPairsRP1_start
		
   ; Compara la puntuació sumada amb 0:
   ; 	Si és més petita o igual -> finalitza 
   ; 	Si és més gran de 0, significa que s'ha generat puntuació i 
   ;	s'assigna a la variable score i s'assigna 2 a la variable state
   cmp r11d, 0								; if (p > 0)
   jle addPairsRP1_end
   mov DWORD[state], '2'
   add DWORD[score], r11d
   
   addPairsRP1_end:
	
   pop r8
   pop r9
   pop r10
   pop r11
   pop r12
   pop r13
   pop rdx 
   pop rcx
   pop rbx 
   pop rax
   

   mov rsp, rbp
   pop rbp
   ret
   

;;;;;; 
; Aquesta subrutina es dóna feta. NO LA PODEU MODIFICAR.
; Llegir una tecla cridant la subrutina getchP1 
; i quedarà guarda a la variable (charac).
; Segons la tecla llegida cridarem a les subrutines corresponents.
;    ['i' (amunt),'j'(esquerra),'k' (avall) o 'l'(dreta)] 
; Desplaçar els números i fer les parelles segons la direcció triada.
; Segons la tecla premuda, rotar la matriu cridant (rotateMatrixRP1), per
; a poder fer els desplaçaments dels números cap a la dreta 
; (shiftNumbersRP1), fer les parelles cap a la dreta (addPairsRP1) i 
; tornar a desplaçar els nombres cap a la dreta (shiftNumbersRP1) 
; amb les parelles fetes, després seguir rotant cridant (rotateMatrixRP1) 
; fins deixar la matriu en la posició inicial. 
; Per a la tecla 'l' (dreta) no cal fer rotacions, per a la resta 
; s'han de fer 4 rotacions.
;    '<ESC>' (ASCII 27)  posar (state = '0') per a sortir del joc.
; Si no és cap d'aquestes tecles no fer res.
; Els canvis produïts per aquestes subrutines no s'han de mostrar a la 
; pantalla, per tant, caldrà actualitzar després el tauler cridant la 
; subrutina UpdateBoardP1.
;
; Variables globals utilitzades:
; (charac)   : Caràcter llegit de teclat.
; (state)    : Indica l'estat del joc. '0':sortir (ESC premut), '1':jugar.
;;;;;  
readKeyP1:
   push rbp
   mov  rbp, rsp

   push rax 
      
   call getchP1    ; Llegir una tecla i deixar-la a charac.
   mov  al, BYTE[charac]
      
   readKeyP1_i:
   cmp al, 'i'      ; amunt
   jne  readKeyP1_j
      call rotateMatrixRP1
      
      call shiftNumbersRP1
      call addPairsRP1
      call shiftNumbersRP1  
      
      call rotateMatrixRP1
      call rotateMatrixRP1
      call rotateMatrixRP1
      jmp  readKeyP1_End
      
   readKeyP1_j:
   cmp al, 'j'      ; esquerra
   jne  readKeyP1_k
      call rotateMatrixRP1
      call rotateMatrixRP1
      
      call shiftNumbersRP1
      call addPairsRP1
      call shiftNumbersRP1  
      
      call rotateMatrixRP1
      call rotateMatrixRP1
      jmp  readKeyP1_End
      
   readKeyP1_k:
   cmp al, 'k'      ; abajo
   jne  readKeyP1_l
      call rotateMatrixRP1
      call rotateMatrixRP1
      call rotateMatrixRP1
      
      call shiftNumbersRP1
      call addPairsRP1
      call shiftNumbersRP1  
      
      call rotateMatrixRP1
      jmp  readKeyP1_End

   readKeyP1_l:
   cmp al, 'l'      ; dreta
   jne  readKeyP1_ESC
      call shiftNumbersRP1
      call addPairsRP1
      call shiftNumbersRP1  
      jmp  readKeyP1_End

   readKeyP1_ESC:
   cmp al, 27      ; Sortir del programa
   jne  readKeyP1_End
      mov BYTE[state], '0'

   readKeyP1_End:
   pop rax
   
   mov rsp, rbp
   pop rbp
   ret


;;;;;
; Joc del 2048
; Subrutina principal del joc
; Permet jugar al joc del 2048 cridant totes les funcionalitats.
; Aquesta subrutina es dóna feta. NO LA PODEU MODIFICAR.
;
; Pseudo codi:
; Inicialitzar estat del joc, (state='1')
; Esborrar pantalla (cridar la funció clearScreen_C).
; Mostrar el tauler de joc (cridar la funció PrintBoardP1_C).
; Actualitza el contingut del Tauler de Joc i els punts que s'han fet
; (cridar la subrutina updateBoardP1).
; Mentre (state=='1') fer
;   Llegir una tecla (cridar la subrutina readKeyP1) i cridar les subrutines corresponents.
;   Si hem mogut algun número al fer els desplaçaments o al fer les 
;   parelles (state=='2'), generar una nova fitxa (cridant la función 
;   insertTileP1_C) i posar la variable state a '1' (state='1').
;   Actualitza el contingut del Tauler de Joc i els punts que s'han fet
;   (cridar la subrutina updateBoardP1).
; Fi mentre.
; Mostra un missatge a sota del tauler segons el valor de la variable 
; (state). (cridar la funció printMessageP1_C).
; Sortir: 
; S'acabat el joc.
;
; Variables globals utilitzades:
; (state)    : Estat del joc.
;              '0': Sortir, hem premut la tecla 'ESC').
;              '1': Continuem jugant.
;              '2': Continuem jugant però s'han fet canvis a la matriu.
;;;;;  
playP1:
   push rbp
   mov  rbp, rsp
   
   mov BYTE[state], '1'       ;state = '1';    
   
   call clearScreen_C
   call printBoardP1_C
   call updateBoardP1

   playP1_Loop:               ;while  {     //Bucle principal.
   cmp  BYTE[state], '1'      ;(state == '1')
   jne  playP1_End
      
      call readKeyP1          ;readKeyP1_C();
      cmp BYTE[state], '2'    ;state == '2';
      jne playP1_Next 
         call insertTileP1_C  ;insertTileP1_C(); 
         mov BYTE[state],'1'  ;state = '1';
      playP1_Next
      call updateBoardP1      ;updateBoardP1_C();
      
   jmp playP1_Loop

   playP1_End:
   call printMessageP1_C      ;printMessageP1_C();
   
   mov rsp, rbp
   pop rbp
   ret
