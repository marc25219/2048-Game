/**
 * Implementació en C de la pràctica, per a què tingueu una
 * versió funcional en alt nivell de totes les funcions que heu 
 * d'implementar en assemblador.
 * Des d'aquest codi es fan les crides a les subrutines de assemblador. 
 * AQUEST CODI NO ES POT MODIFICAR I NO S'HA DE LLIURAR.
 **/

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>    //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>     //STDIN_FILENO

extern  int developer;	     //Variable declarada en assemblador que indica el nom del programador

/**
 * Constants
 */
#define DimMatrix  4     //dimensió de la matriu
#define SizeMatrix DimMatrix*DimMatrix //=16


/**
 * Definició de variables globals
 */
// Matriu 9x9 on guardarem els números del joc
// Accés a les matrius en C: utilitzem fila (0..[DimMatrix-1]) i 
// columna(0..[DimMatrix-1]) (m[fila][columna]).
// Accés a les matrius en assemblador: S'hi accedeix com si fos un vector 
// on indexMat (0..[DimMatrix*DimMatrix-1]). 
// indexMat=((fila*DimMatrix)+(columna))*2 (2 perquè la matriu és de tipus short).
// WORD[M+indexMat] (WORD perquè és de tipus short) 
// (indexMat ha de ser un registre de tipus long/QWORD:RAX,RBX,..,RSI,RDI,..,R15).
short m[DimMatrix][DimMatrix]        = { {    8,    8,    32,    32},
                                         {    4,   32,   128,    64},
                                         {    0,    0,   256,   128},
                                         {    0,    4,   512,  1024} };

short mRotated[DimMatrix][DimMatrix] = { {    2,    0,     2,     0},
                                         {    2,    2,     4,     4},
                                         {    4,    4,     0,     4},
                                         {    4,    2,     2,     4} };

short mAux[DimMatrix][DimMatrix]     = { {    0,    0,     0,     0},
                                         {    0,    0,     0,     0},
                                         {    0,    0,     0,     0},
                                         {    0,    0,     0,     0} };
                                    
short mUndo[DimMatrix][DimMatrix]    = { {    0,    1,     3,     7},
                                         {   15,   31,    63,   127},
                                         {  255,  511,  1023,  2047},
                                         { 4095, 8191, 16383, 32767},};

char state = '1';   //'0': Sortir, hem premut la tecla 'ESC' per a sortir.
			        //'1': Continuem jugant.
			        //'2': Continuem jugant però hi han hagut canvis a la matriu.
			        //'3': Desfer últim moviment.
				    //'4': Guanyat, s'han arribat a la casella 2048.
				    //'5': Perdut, no es poden fer moviments.

/**
 * Definició de les funcions de C
 */
void clearscreen_C();
void gotoxyP2_C(int, int);
void printchP2_C(char);
char getchP2_C();

char printMenuP2_C();
void printBoardP2_C();

void  showNumberP2_C(int, int, int);
void  updateBoardP2_C(int);
void  copyMatrixP2_C(short [DimMatrix][DimMatrix], short [DimMatrix][DimMatrix]);
void  rotateMatrixRP2_C(short [DimMatrix][DimMatrix]);
int   shiftNumbersRP2_C(short [DimMatrix][DimMatrix]);
int   addPairsRP2_C(short [DimMatrix][DimMatrix]);

int   readKeyP2_C(int);
void  insertTileP2_C();
void  checkEndP2_C();
void  printMessageP2_C();
void  playP2_C();


/**
 * Definició de les subrutines d'assemblador que es criden des de C.
 */
extern void  showNumberP2(int, int, int);
extern void  updateBoardP2(int);
extern void  copyMatrixP2(short [DimMatrix][DimMatrix], short [DimMatrix][DimMatrix]);
extern void  rotateMatrixRP2(short [DimMatrix][DimMatrix]);
extern int   shiftNumbersRP2(short [DimMatrix][DimMatrix]);
extern int   addPairsRP2(short [DimMatrix][DimMatrix]);
extern int   readKeyP2(int);
extern void  checkEndP2();
extern void  playP2();


/**
 * Esborrar la pantalla
 * 
 * Variables globals utilitzades:	
 * Cap
 * 
 * Paràmetres d'entrada : 
 * Cap
 *   
 * Paràmetres de sortida: 
 * Cap
 * 
 * Aquesta funció no es crida des d'assemblador
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void clearScreen_C(){
	
    printf("\x1B[2J");
    
}


/**
 * Situar el cursor en una fila i una columna de la pantalla
 * en funció de la fila (rowScreen) i de la columna (colScreen) 
 * rebuts com a paràmetre.
 * 
 * Variables globals utilitzades:	
 * Cap
 * 
 * Paràmetres d'entrada : 
 * rdi(edi): (rowScreen): Fila
 * rsi(esi): (colScreen): Columna
 * 
 * Paràmetres de sortida: 
 * Cap
 * 
 * S'ha definit un subrutina en assemblador equivalent 'gotoxyP2' 
 * per a poder cridar aquesta funció guardant l'estat dels registres 
 * del processador. Això es fa perquè les funcions de C no mantenen 
 * l'estat dels registres.
 * El pas de paràmetres és equivalent.
 */
void gotoxyP2_C(int rowScreen, int colScreen){
	
   printf("\x1B[%d;%dH",rowScreen,colScreen);
   
}


/**
 * Mostrar un caràcter (c) a la pantalla, rebut com a paràmetre, 
 * en la posició on està el cursor.
 * 
 * Variables globals utilitzades:	
 * Cap
 * 
 * Paràmetres d'entrada : 
 * rdi(dil): (c): Caràcter que volem mostrar
 * 
 * Paràmetres de sortida: 
 * Cap
 * 
 * S'ha definit un subrutina en assemblador equivalent 'printchP2' 
 * per a cridar aquesta funció guardant l'estat dels registres del 
 * processador. Això es fa perquè les funcions de C no mantenen 
 * l'estat dels registres.
 * El pas de paràmetres és equivalent.
 */
void printchP2_C(char c){
	
   printf("%c",c);
   
}


/**
 * Llegir una tecla i retornar el caràcter associat 
 * sense mostrar-lo per pantalla. 
 * 
 * Variables globals utilitzades:	
 * Cap
 * 
 * Paràmetres d'entrada : 
 * Cap
 * 
 * Paràmetres de sortida: 
 * rax(al): (c): Caràcter llegit de teclat
 * 
 * S'ha definit un subrutina en assemblador equivalent 'getchP2' per a
 * cridar aquesta funció guardant l'estat dels registres del processador.
 * Això es fa perquè les funcions de C no mantenen l'estat dels 
 * registres.
 * El pas de paràmetres és equivalent.
 */
char getchP2_C(){

   int c;   

   static struct termios oldt, newt;

   /*tcgetattr obtenir els paràmetres del terminal
   STDIN_FILENO indica que s'escriguin els paràmetres de l'entrada estàndard (STDIN) sobre oldt*/
   tcgetattr( STDIN_FILENO, &oldt);
   /*es copien els paràmetres*/
   newt = oldt;

   /* ~ICANON per a tractar l'entrada de teclat caràcter a caràcter no com a línia sencera acabada amb /n
      ~ECHO per a què no mostri el caràcter llegit*/
   newt.c_lflag &= ~(ICANON | ECHO);          

   /*Fixar els nous paràmetres del terminal per a l'entrada estàndard (STDIN)
   TCSANOW indica a tcsetattr que canvii els paràmetres immediatament. */
   tcsetattr( STDIN_FILENO, TCSANOW, &newt);

   /*Llegir un caràcter*/
   c=getchar();                 
    
   /*restaurar els paràmetres originals*/
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

   /*Retornar el caràcter llegit*/
   return (char)c;
   
}


/**
 * Mostrar a la pantalla el menú del joc i demanar una opció.
 * Només accepta una de les opcions correctes del menú ('0'-'9')
 * 
 * Variables globals utilitzades:	
 * developer:((char *)&developer): variable definida en el codi assemblador.
 * 
 * Paràmetres d'entrada : 
 * Cap
 * 
 * Paràmetres de sortida: 
 * rax(al): (charac) Opció triada del menú, llegida de teclat.
 * 
 * Aquesta funció no es crida des d'assemblador
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
char printMenuP2_C(){

 	clearScreen_C();
    gotoxyP2_C(1,1);
    printf("                                    \n");
    printf("           Developed by:            \n");
	printf("        ( %s )   \n",(char *)&developer);
    printf(" __________________________________ \n");
    printf("|                                  |\n");
    printf("|            MAIN MENU             |\n");
    printf("|__________________________________|\n");
    printf("|                                  |\n");
    printf("|         1. ShowNumber            |\n");
    printf("|         2. UpdateBoard           |\n");
    printf("|         3. CopyMatrix            |\n");
    printf("|         4. RotateMatrix          |\n");
    printf("|         5. ShiftNumbers          |\n");
    printf("|         6. AddPairs              |\n");
    printf("|         7. CheckEnd              |\n");
    printf("|         8. Play Game             |\n");
    printf("|         9. Play Game C           |\n");
    printf("|         0. Exit                  |\n");
    printf("|__________________________________|\n");
    printf("|                                  |\n");
    printf("|            OPTION:               |\n");
    printf("|__________________________________|\n"); 

    char charac =' ';
    while (charac < '0' || charac > '9') {
      gotoxyP2_C(21,22);
	  charac = getchP2_C();
	  printchP2_C(charac);
	}
	return charac;
   
}


/**
 * Mostrar el tauler de joc a la pantalla. Les línies del tauler.
 * 
 * Variables globals utilitzades:	
 * Cap
 * 
 * Paràmetres d'entrada : 
 * Cap
 *   
 * Paràmetres de sortida: 
 * Cap
 * 
 * Aquesta funció es crida des de C i des d'assemblador,
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void printBoardP2_C(){

   gotoxyP2_C(1,1);
   printf(" _________________________________________________  \n"); //01
   printf("|                                                  |\n"); //02
   printf("|                  2048 PUZZLE  v2.0               |\n"); //03
   printf("|                                                  |\n"); //04
   printf("|     Join the numbers and get to the 2048 tile!   |\n"); //05   
   printf("|__________________________________________________|\n"); //06
   printf("|                                                  |\n"); //07
   printf("|            0        1        2        3          |\n"); //08
   printf("|        +--------+--------+--------+--------+     |\n"); //09
   printf("|      0 |        |        |        |        |     |\n"); //10
   printf("|        +--------+--------+--------+--------+     |\n"); //11
   printf("|      1 |        |        |        |        |     |\n"); //12
   printf("|        +--------+--------+--------+--------+-    |\n"); //13
   printf("|      2 |        |        |        |        |     |\n"); //14
   printf("|        +--------+--------+--------+--------+     |\n"); //15
   printf("|      3 |        |        |        |        |     |\n"); //16
   printf("|        +--------+--------+--------+--------+     |\n"); //17
   printf("|          Score:   ______                         |\n"); //18
   printf("|__________________________________________________|\n"); //19
   printf("|                                                  |\n"); //20
   printf("| (ESC)Exit (u)Undo (i)Up (j)Left (k)Down (l)Right |\n"); //21
   printf("|__________________________________________________|\n"); //22
   
}


/**
 * Converteix el número de la variable (n) de tipus int de 6 dígits
 * (n <= 999999), rebuda com a paràmetre,  
 * a caràcters ASCII que representen el seu valor.
 * Si (n) és més gran que 999999 canviarem el valor a 999999.
 * S'ha de dividir(/) el valor entre 10,  de forma iterativa, 
 * fins obtenir el 6 digits.
 * A cada iteració, el residu de la divisió (%) que és un valor
 * entre (0-9) indica el valor del dígit que s'han de convertir
 * a ASCII ('0' - '9') sumant '0' (48 decimal) per a poder-lo mostrar.
 * Quan el quocient sigui 0 mostrarem espais a la part no significativa.
 * Per exemple, si n=103 mostrarem "   103" i no "000103".
 * S'han de mostrar els dígits (caràcter ASCII) des de  la posició 
 * indicada per les variables (rScreen) i (cScreen), rebudes com a 
 * paràmetre, posició de les unitats, cap a l'esquerra.
 * El primer dígit que obtenim són les unitats, després les desenes,
 * ..., per a mostrar el valor s'ha de desplaçar el cursor una posició
 * a l'esquerra a cada iteració.
 * Per a posicionar el cursor es crida a la funció gotoxyP2_C i per a 
 * mostrar els caràcters a la funció printchP2_C.
 * 
 * Variables globals utilitzades:	
 * Cap
 * 
 * Paràmetres d'entrada : 
 * rdi(edi): (rScreen): Fila de la pantalla on posicionem el cursor.
 * rsi(esi): (cScreen): Columna de la pantalla on posicionem el cursor.
 * rdx(edx): (n)      : Número que volem mostrar.
 * 
 * Paràmetres de sortida: 
 * Cap
 * 
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'showNumberP2',  
 * el pas de paràmetres és equivalent.
 */
 void showNumberP2_C(int rScreen, int cScreen, int n) {
	
	char charac;
	int  i;

    if (n > 999999) n = 999999;
	for (i=0;i<6;i++){
	  charac = ' ';
	  if (n > 0) {
		charac = n%10;	//residu
		n = n/10;		//quocient
		charac = charac + '0';
	  } 
	  gotoxyP2_C(rScreen, cScreen);
	  printchP2_C(charac);
	  cScreen--;
	}
		
}


/**
 * Actualitzar el contingut del Tauler de Joc amb les dades de la 
 * matriu (m) de 4x4 de tipus short i els punts del marcador 
 * (scr), rebut com a paràmetre, que s'han fet.  
 * S'ha de recórrer tota la matriu (m), i per a cada element de la matriu
 * posicionar el cursor a la pantalla i mostrar el número 
 * d'aquella posició de la matriu.
 * Recorrer tota la matriu per files d'esquerra a dreta i de dalt a baix.
 * Per recorrer la matriu en assemblador l'index va de 0 (posició [0][0])
 * a 30 (posició [3][3]) amb increments de 2 perquè les dades son de
 * tipus short(WORD) 2 bytes.
 * Després, mostrar el marcador (scr) a la part inferior del tauler,
 * fila 18, columna 26 cridant la funció showNumberP2_C.
 * Finalment posicionar el cursor a la fila 18, columna 28 cridant la 
 * funció gotoxyP2_C().
 * 
 * Variables globals utilitzades:
 * (m): Matriu on guardem els nombres del joc.
 * 
 * Paràmetres d'entrada : 
 * rdi(edi): (scr): Punts acumulats al marcador.
 * 
 * Paràmetres de sortida: 
 * Cap
 *  
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'updateBoardP2', 
 * el pas de paràmetres és equivalent.
 */
void updateBoardP2_C(int scr){

   int i,j;
   int rScreen, cScreen;
   
   rScreen = 10;
   for (i=0;i<DimMatrix;i++){
	  cScreen = 17;
      for (j=0;j<DimMatrix;j++){
         showNumberP2_C(rScreen,cScreen, m[i][j]);
         cScreen = cScreen + 9;
      }
      rScreen = rScreen + 2;
   }
   showNumberP2_C(18, 26, scr);   
   gotoxyP2_C(18,28);
   
}


/**
 * Copiar la matriu origen (mOrigin) -segon paràmetre- sobre la matriu 
 * destinació (mDest) -primer paràmetre-.
 * La matriu origen (mOrigin) no s'ha de modificar
 * els canvis s'ha de fer sobre la matriu destinació (mDest).
 * Recorrer tota la matriu per files d'esquerra a dreta i de dalt a baix.
 * Per recorrer la matriu en assemblador l'index va de 0 (posició [0][0])
 * a 30 (posició [3][3]) amb increments de 2 perquè les dades son de
 * tipus short(WORD) 2 bytes.
 * Això permetrà copiar dues matrius després d'una rotació 
 * i gestionar l'opció '(u)Undo' del joc.
 * No es mostrar la matriu.
 * 
 * Variables globals utilitzades:
 * Cap
 * 
 * Paràmetres d'entrada : 
 * rdi(rdi): (mOrigin): Adreça de la matriu on guardem els nombres del joc que volem copiar.
 * rsi(rsi): (mDest)  : Adreça de la matriu on guardem els nombres del joc que volem sobreescriure.
 * 
 * Paràmetres de sortida: 
 * Cap.
 *  
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'copyMatrixP2', 
 * el pas paràmetres és equivalent
 */
void copyMatrixP2_C(short mDest[DimMatrix][DimMatrix], short mOrig[DimMatrix][DimMatrix]) {
	
	int i,j;
	
	for (i=0; i<DimMatrix; i++) {
		for (j=0; j<DimMatrix; j++) {
			mDest[i][j]=mOrig[i][j];
		}
	}

}


/**
 * Rotar a la dreta la matriu (mToRotate), rebuda com a pràmetre
 * sobre la matriu (mRotated). 
 * La primera fila passa a ser la quarta columna, la segona fila passa
 * a ser la tercera columna, la tercera fila passa a ser la segona 
 * columna i la quarta fila passa a ser la primera columna.
 * A l'enunciat s'explica en més detall com fer la rotació.
 * NOTA: NO és el mateix que fer la matriu transposada.
 * La matriu (mToRotate) no s'ha de modificar, 
 * els canvis s'han de fer a la matriu (mRotated).
 * Per recórrer la matriu en assemblador l'índex va de 0 (posició [0][0])
 * a 30 (posició [3][3]) amb increments de 2 perquè les dades son de 
 * tipus short(WORD) 2 bytes.
 * Per a accedir a una posició concreta de la matriu des d'assemblador 
 * cal tindre en compte que l'índex és:(index=(fila*DimMatrix+columna)*2),
 * multipliquem per 2 perquè les dades son de tipus short(WORD) 2 bytes.
 * Un cop s'ha fet la rotació, copiar la matriu (mRotated) a la matriu 
 * rebuda com a paràmetre cridant la funció copyMatrixP2_C().
 * No s'ha de mostrar la matriu.
 * 
 * Variables globals utilitzades:
 * (mRotated) : Matriu on guardem els nombres ja rotats
 * 
 * Paràmetres d'entrada : 
 * rdi(rdi): (mToRotate): Adreça de la matriu on guardem els nombres del joc que volem rotar.
 * 
 * Paràmetres de sortida: 
 * Cap
 *  
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'rotateMatrixRP2', 
 * el pas de paràmetres és equivalent.
 */
void rotateMatrixRP2_C(short mToRotate[DimMatrix][DimMatrix]) {
	
	int i,j;
	
	for (i=0; i<DimMatrix; i++) {
		for (j=0; j<DimMatrix; j++) {	
			mRotated[j][DimMatrix-1-i] = mToRotate[i][j];
		}
	}
	
	copyMatrixP2_C(mToRotate, mRotated);
	
}


/**
 * Desplaça a la dreta els número de cada fila de la matriu (mShift) 
 * rebuda com a paràmetre, mantenint l'ordre dels nombres i posant 
 * els zeros a l'esquerra.
 * Recórrer la matriu per files de dreta a esquerra i de baix a dalt.  
 * Per recórrer la matriu en assemblador, en aquest cas, l'índex va de la
 * posició 30 (posició [3][3]) a la 0 (posició [0][0]) amb decrements de
 * 2 perquè les dades son de tipus short(WORD) 2 bytes.
 * Per a accedir a una posició concreta de la matriu des d'assemblador 
 * cal tindre en compte que l'índex és:(index=(fila*DimMatrix+columna)*2),
 * multipliquem per 2 perquè les dades son de tipus short(WORD) 2 bytes. 
 * Si es desplaça un número (NO ELS ZEROS), s'han de comptar els
 * desplaçament incrementant la varaible (shifts).
 * A cada fila, si troba un 0, mira si hi ha un número diferent de zero,
 * a la mateiza fila per posar-lo en aquella posició.
 * Si una fila de la matriu és: [0,2,0,4] i (shifts = 0), quedarà [0,0,2,4] 
 * i (shifts = 1).
 * Retornem el valor de la variable (shifts) per a indicar el número
 * de desplazamientos que s'han fet.
 * Els canvis s'han de fer sobre la mateixa matriu.
 * No s'ha de mostrar la matriu.
 * 
 * Variables globals utilitzades:
 * Cap
 * 
 * Paràmetres d'entrada : 
 * rdi(edi): (mShift): Adreça de la matriu on guardem els nombres del joc que volem desplaçar.
 * 
 * Paràmetres de sortida: 
 * rax(eax): (shifts): Desplaçaments que s'han fet.
 *  
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'shiftNumbersRP2', 
 * el pas de paràmetres és equivalent.
 */
int shiftNumbersRP2_C(short mShift[DimMatrix][DimMatrix]) {
	
	int i,j,k;
	int shifts=0;
	
	for (i=DimMatrix-1; i>=0; i--) {
      for (j=DimMatrix-1; j>0; j--) {
        if (mShift[i][j] == 0) {
          k = j-1;           
          while (k>=0 && mShift[i][k]==0) k--;
          if (k==-1) {
             j=0;                
          } else {
              mShift[i][j]=mShift[i][k];
              mShift[i][k]= 0; 
              shifts++;         
          }
        }      
      }
    }

    return shifts;
	
}
	

/**
 * Aparellar nombres iguals des la dreta de la matriu (mPairs), 
 * rebuda com a paràmetre, i acumular els punts al marcador sumant els 
 * punts parelles que s'hagin fet.
 * Recórrer la matriu per files de dreta a esquerra i de baix a dalt. 
 * Quan es trobi una parella, dos caselles consecutives de la mateixa 
 * fila amb el mateix número, ajuntem la parella posant la suma de la 
 * parella a la casella de la dreta, un 0 a la casella de l'esquerra i 
 * acumularem aquesta suma a la variable (p) (punts guanyats).
 * Si una fila de la matriu és: [8,4,4,2], quedarà [8,0,8,2] i 
 * p = p + (4+4).
 * Retornarem els punts obtinguts de fer les parelles.
 * Per recórrer la matriu en assemblador, en aquest cas, l'índex va de la
 * posició 30 (posició [3][3]) a la 0 (posició [0][0]) amb increments de 
 * 2 perquè les dades son de tipus short(WORD) 2 bytes.
 * Els canvis s'han de fer sobre la mateixa matriu.
 * No s'ha de mostrar la matriu.
 * 
 * Variables globals utilitzades:
 * Cap
 * 
 * Paràmetres d'entrada : 
 * rdi(edi): (mPairs): Adreça de la matriu on guardem els nombres del joc que volem fer parelles.
 * 
 * Paràmetres de sortida: 
 * rax(eax): (p): punts  de les parelles que s'hagin fet.
 *  
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'addPairsRP2', 
 * el pas de paràmetres és equivalent.
 */
int addPairsRP2_C(short mPairs[DimMatrix][DimMatrix]) {
	
	int i,j;
	int p = 0;
	
	for (i=DimMatrix-1; i>=0; i--) {
      for (j=DimMatrix-1; j>0; j--) {
			if ((mPairs[i][j]!=0) && (mPairs[i][j]==mPairs[i][j-1])) {
				mPairs[i][j]  = mPairs[i][j]*2;
				mPairs[i][j-1]= 0;
				p = p + mPairs[i][j];
			}		
		}
	}
	
	return p;
	
}


/**
 * Verificar si s'ha arribat a 2048 o si no es pot fer algun moviment.
 * Si hi ha el nombre 2048 a la matriu (m), canviar 
 * l'estat a 4 (status='4') per indicar que s'ha guanyat (WIN!).
 * Si no hem guanyat, mirar si es pot fer algun moviment, 
 * Si no es pot fer cap moviment canviar  l'estat a 5 (status='5') per a
 * indicar que s'ha perdut (GAME OVER!!!).
 * Recórrer la matriu (m) per files de dreta a esquerra i de baix a dalt
 * comptant les caselles buides i mirant si hi ha el nombre 2048. 
 * Si hi ha el número 2048 posar (status='4') i acabar.
 * Si no hi ha el número 2048 i no hi ha caselles buides mirar si es pot
 * fer algun aparellament en horitzontal o en vertical. Per fer-ho, cal 
 * copiar la matriu (m) sobre la matriu (mAux) cridant (copyMatrixP2_C),
 * fer parelles a la matriu (mAux) per a mirar si es poden fer parelles 
 * en horitzontal cridant (addPairsRP2_C) i guardar els punts obtinguts,
 * rotar la matriu (mAux) cridant (rotateMatrixRP2_C) i tornar a fer 
 * parelles a la matriu (mAux) per mirar si es poden fer  parelles en 
 * vertical cridant (addPairsRP2_C) i acumular els punts obtinguts amb
 * els punt obtinguts abans, si els punts acumulats són 0,
 * vol dir que no es poden fer parelles i s'ha de posar l'estat del
 * joc a 5 (status='5').
 * No es pot modificar ni la matriu (m), ni la matriu (mUndo).
 * 
 * Variables globals utilitzades:
 * (m)       : Matriu on guardem els nombres del joc.
 * (mRotated): Matriu on guardem els nombres ja rotats.
 * (mAux)    : Matriu on copiem la matriu (m) per a comprovar-la.
 * (state)   : Estat del joc.
 * 
 * Paràmetres d'entrada : 
 * Cap
 * 
 * Paràmetres de sortida: 
 * Cap
 *  
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'checkEndP2', 
 * el pas de paràmetres és equivalent.
 */
void checkEndP2_C() {
	
	int i,j;
	int zeros=0;
	int pairs=0;
		
	i=DimMatrix;
	do {
		i--;
		j=DimMatrix;
		do {
			j--;
			if (m[i][j] == 0 ) zeros++;
			if (m[i][j] == 2048) state='4';
		} while ((j>0) && (m[i][j]!=2048));
		
	} while ((i>0) && (m[i][j]!=2048));

	if ((state!= '4') && (zeros == 0)) {
		copyMatrixP2_C(mAux,m);
		pairs = addPairsRP2_C(mAux);
		rotateMatrixRP2_C(mAux);
		pairs = pairs + addPairsRP2_C(mAux);
		if (pairs==0) state = '5';	
	}
	
} 


/**
 * Llegir una tecla cridant la funció getchP2_C. 
 * Guardar la tecla llegida a la variable (c).
 * Segons la tecla llegida cridarem a les funcions corresponents.
 *    ['i' (amunt),'j'(esquerra),'k' (avall) o 'l'(dreta)] 
 * Desplaçar els números i fer les parelles segons la direcció triada.
 * Segons la tecla premuda, rotar la matriu (m) cridant (rotateMatrixRP2_C),  
 * per a poder fer els desplaçaments dels números cap a la dreta cridant
 * (shiftNumbersRP2_C), fer les parelles cap a la dreta (addPairsRP2_C)  
 * que retorna els punts que hem sumar al marcador (actualScore) rebut
 * com a paràmetre i que retornarem per actualitzar el marcador (score). 
 * Tornar a desplaçar els nombres cap a la dreta (shiftNumbersRP2_C)
 * amb les parelles fetes. 
 * Si s'ha fet algun desplaçament o alguna parella
 * indicar-ho posant (state='2').
 * Després seguir rotant cridant (rotateMatrixRP2_C) 
 * fins deixar la matriu en la posició inicial. 
 * Per a la tecla 'l' (dreta) no cal fer rotacions, per a la resta 
 * s'han de fer 4 rotacions.
 *    'u'                Posar (state = '3') per a recuperar l'estat anterior.
 *    '<ESC>' (ASCII 27) posar (state = '0') per a sortir del joc.
 * Si no és cap d'aquestes tecles no fer res.
 * Els canvis produïts per aquesta funció no es mostren a la pantalla.
 * 
 * Variables globals utilitzades:
 * (mRotated): Matriu on guardem els nombres ja rotats.
 * (m)       : Matriu on guardem els nombres del joc.
 * (state)   : Estat del joc.
 * 
 * Paràmetres d'entrada : 
 * rdi(edi): (actualScore): Punts acumulats al marcador.
 * 
 * Paràmetres de sortida: 
 * rax(eax): (actualScore): Punts acumulats al marcador actualitzats.
 *  
 * Aquesta funció no es crida des d'assemblador.
 * A la subrutina d'assemblador equivalent 'readKeyP2', 
 * el pas de paràmetres és equivalent.
 */
int readKeyP2_C(int actualScore){

   int s1, s2;
   int p;
   
   char c;
   c = getchP2_C();	
 
   switch(c){
      case 'i': //i:(105) amunt
         rotateMatrixRP2_C(m);
		 
         s1 = shiftNumbersRP2_C(m);
         p  = addPairsRP2_C(m);
         s2 = shiftNumbersRP2_C(m);
         actualScore = actualScore + p;
         if ( (s1>0) || (p>0) || (s2>0) ) state = '2';
         
         rotateMatrixRP2_C(m);
         rotateMatrixRP2_C(m);
         rotateMatrixRP2_C(m);
		 
      break;
      case 'j': //j:(106) esquerra
         rotateMatrixRP2_C(m);
         rotateMatrixRP2_C(m);
         
         s1 = shiftNumbersRP2_C(m);
         p  = addPairsRP2_C(m);
         s2 = shiftNumbersRP2_C(m);
         actualScore = actualScore + p;
         if ( (s1>0) || (p>0) || (s2>0) ) state = '2';
         
         rotateMatrixRP2_C(m);
         rotateMatrixRP2_C(m);
      break;
      case 'k': //k:(107) avall
         rotateMatrixRP2_C(m);
         rotateMatrixRP2_C(m);
         rotateMatrixRP2_C(m);
             
         s1 = shiftNumbersRP2_C(m);
         p  = addPairsRP2_C(m);
         s2 = shiftNumbersRP2_C(m);
         actualScore = actualScore + p;
         if ( (s1>0) || (p>0) || (s2>0) ) state = '2';
         
		 rotateMatrixRP2_C(m);
      break;
      case 'l': //l:(108) dreta
         s1 = shiftNumbersRP2_C(m);
         p  = addPairsRP2_C(m);
         s2 = shiftNumbersRP2_C(m);
         actualScore = actualScore + p;
         if ( (s1>0) || (p>0) || (s2>0) ) state = '2';
      break;  
	  case 'u': //u:(117) Desfer jugada
         state = '3';
	  break;
	  case 27: //ESC:(27) Sortir del programa
		 state = '0';
	  break;
	}
   
   return actualScore;
   
}


/**
 * Generar nova fitxa de forma aleatòria.
 * Si hi ha com a mínim una casella buida a la matriu (m) genera una 
 * fila i una columna de forma aleatòria fins que és una de les caselles 
 * buides. 
 * A continuació generar un nombre aleatori per decidir si la nova fitxa 
 * ha de ser un 2 (90% dels casos) o un 4 (10% dels casos).
 * 
 * Variables globals utilitzades:
 * (m): Matriu on guardem els nombres del joc.
 * 
 * Paràmetres d'entrada : 
 * Cap
 * 
 * Paràmetres de sortida: 
 * Cap
 *  
 * Aquesta funció es crida des de C i des d'assemblador,
 * i no hi ha definida una subrutina d'assemblador equivalent.
 * No hi ha pas de paràmetres.
 */
void insertTileP2_C() {
	
	int i,j,k,l,r;
	
	i=DimMatrix; // Mirem si hi ha una casella buida.
	do {
		i--;
		j=DimMatrix;
		do {
			j--;	
		} while ((j>=0) && (m[i][j]!=0));
	} while ((i>=0) && (m[i][j]!=0));
	
	//Inserim el 2 o el 4 si hi ha com a mínim una casella buida.
	if (m[i][j]==0) { 
		do { // Genera files i columnes aleatòriament fins que troba
			 // una posició buida
			k = rand() % 4; l = rand() % 4; }
		while( m[k][l] != 0 );
		
		//Decidim si posem un 2 o un 4
		r = rand() % 100;
		if( r < 90 ) {
			m[k][l] = 2;
		} else {
			m[k][l] = 4;
		}
	}

}


/**
 * Mostrar un missatge a sota del tauler segons el valor de la variable 
 * (state).
 * state: '0': Hem premut la tecla 'ESC' per a sortir del joc.
 * 		  '1': Continuem jugant.
 *        '2': Continuem jugant però hi han hagut canvis a la matriu.
 *        '3': Desfer últim moviment
 * 		  '4': Guanyat, ja tenim la fitxa 2048.
 * 		  '5': Perdut, no es pot fer cap moviment.
 * S'espera que es premi una tecla per a continuar.
 * 
 * Si s'ha perdut (state='5') es dona l'opció de fer un darrer 'Undo'
 * prement la tecla 'u' per a recuperar l'estat anterior i provar de 
 * continuar jugant (state='3'). Si es prem una altra tecla es surt del joc.
 * 
 * Variables globals utilitzades:
 * (state):  Estat del joc.
 * 
 * Paràmetres d'entrada : 
 * Cap
 * 
 * Paràmetres de sortida: 
 * Cap
 * 
 * Aquesta funció es crida des de C i des d'assemblador,
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void printMessageP2_C() {

   switch(state){
      case '0':
		 gotoxyP2_C(23,12);
         printf("<<<<<< EXIT: (ESC) Pressed >>>>>>");
         getchP2_C();
        break;
      case '4':
		 gotoxyP2_C(23,12);
         printf("++++++ 2048!!!  YOU  W I N ++++++");
         getchP2_C();
      break;
      case '5':
		 gotoxyP2_C(23,12);
         printf("---- G A M E   O V E R ! ! ! ----");
         gotoxyP2_C(24,12);
         printf("---- (u)Undo  (Any key) EXIT ----");
         char c;
		 c = getchP2_C();
		 if (c == 'u') {
			gotoxyP2_C(23,12);
			printf("                                  ");
			gotoxyP2_C(24,12);
            printf("                                  ");
            state='3';
		 }
      break;
   }

}
 

/**
 * Joc del 2048
 * Funció principal del joc
 * Permet jugar al joc del 2048 cridant totes les funcionalitats.
 *
 * Pseudo codi:
 * Inicialitzar estat del joc, (state='1').
 * Esborrar pantalla (cridar la funció clearScreen_C).
 * Mostrar el tauler de joc (cridar la funció PrintBoardP2_C).
 * Actualitza el contingut del Tauler de Joc i els punts que s'han fet
 * (cridar la funció updateBoardP2_C).
 * Mentre (state=='1') fer:
 *   Copiar la matriu (m) sobre la matriu (mAux) (cridant la funció 
 *   copyMatrixP2_C) i copiar els punts (score) sobre (scoreAux).
 *   Llegir una tecla (cridar la funció readKeyP2_C)
 *   i cridar a les funcions corresponents.
 *   Si hem mogut algun número al fer els desplaçaments o al fer les 
 *   parelles (state=='2'), copiar l'estat del joc que hem guardat abans
 *   (mAux i scoreAux) a (mUndo i scoreUndo) per a poder fer l'Undo 
 *   (recuperar estat anterior) copiant (mAux) sobre (mUndo) (cridant a 
 *   la funció copyMatrixP2_C) i copiant (scoreAux) sobre (scoreUndo). 
 *   Generar una nova fitxa (cridant la funció insertTileP2_C) i posar 
 *   la variable state a '1' (state='1').
 *   Si hem de recuperar l'estat anterior (state='3'), copiar l'estat 
 *   anterior del joc que tenim a (mUndu i scoreUndu) sobre (m i score)
 *   (cridant a la funció copyMatrixP2_C) i copiant (scoreUndu) sobre 
 *   (score) i posar la variable state a '1' (state='1').
 *   Actualitza el contingut del Tauler de Joc i els punts que s'han fet
 *   (cridar la funció updateBoardP2_C).
 *   Verificar si s'ha arribat a 2048 o si no es pot fer cap moviment
 *   (cridar la funció CheckEndP2_C).
 *   Mostrar un missatge a sota del tauler segons el valor de la variable 
 *   (state). (cridar la funció printMessageP2_C()).
 * Fi mentre.
 * Sortir: 
 * S'acabat el joc.
 * 
 * Variables globals utilitzades:
 * (m)       : Matriu on guardem els nombres del joc.
 * (mRotated): Matriu on guardem els nombres ja rotats
 * (mAux)    : Matriu on copiem la matriu (m) per a comprovar-la.
 * (mUndu)   : Matriu on guardem el darrer moviment per poder tornar enrera.
 * (state)   : Estat del joc.
 * 
 * Paràmetres d'entrada : 
 * Cap
 * 
 * Paràmetres de sortida: 
 * Cap
 */
void playP2_C(){
   		     
   int score     = 290500;
   int scoreAux  = 0;
   int scoreUndo = 1;     
   
   state = '1';	   			   
   clearScreen_C();
   printBoardP2_C();
   updateBoardP2_C(score);
          
   while (state == '1') {  	  //Bucle principal.
	 copyMatrixP2_C(mAux,m);  
	 scoreAux = score;
	 score = readKeyP2_C(score);
	 if (state == '2') {	  
		copyMatrixP2_C(mUndo,mAux);
		scoreUndo = scoreAux;
		insertTileP2_C();	  
		state = '1';
	 }
	 if (state == '3') {       
		 copyMatrixP2_C(m,mUndo);
         score = scoreUndo;
         state = '1';
     }
     updateBoardP2_C(score);
	 checkEndP2_C();
	 printMessageP2_C();       
	 if (state == '3') {       
		 copyMatrixP2_C(m,mUndo);
         score = scoreUndo;
         state = '1';
         updateBoardP2_C(score);
     } 		
  }

}


/**
 * Programa Principal
 * 
 * ATENCIÓ: A cada opció es crida a una subrutina d'assemblador.
 * A sota hi ha comentada la funció en C equivalent que us donem feta 
 * per si voleu veure com funciona, per al joc complet hi ha una opció 
 * per cada cas.
 */
int main(){   

   char op=' ';
   char c;
   int score = 123456;
   
   while (op!='0') {
	  op = printMenuP2_C();
      
      switch(op){
         case '1':// Mostrar punts
            clearScreen_C();  
            printBoardP2_C();   
            gotoxyP2_C(18, 30);
            printf(" Press any key ");
            //=======================================================
            showNumberP2(18, 26, score);       
            //showNumberP2_C(18, 26, score);   
            //=======================================================
            getchP2_C();
         break;
         case '2': //Actualitzar el contingut del tauler.
            clearScreen_C();  
            printBoardP2_C(); 
            //=======================================================
            updateBoardP2(score);
            //updateBoardP2_C(score); 
            //=======================================================
            gotoxyP2_C(18, 30);
            printf("Press any key ");
            getchP2_C();
         break;
         case '3': //Copiar matrius.
            clearScreen_C();  
            printBoardP2_C(); 
            int scoreUndu = 500;
            //=======================================================
            copyMatrixP2(m, mUndo);
            //copyMatrixP2_C(m, mUndo);
            //=======================================================
            updateBoardP2_C(scoreUndu);
            gotoxyP2_C(18, 30);
			printf("Press any key ");
	        getchP2_C();
         break;
         case '4': //Rotar matriu a la dreta
            clearScreen_C();  
            printBoardP2_C(); 
            //===================================================
            rotateMatrixRP2(m);
			//rotateMatrixRP2_C(m);
			//===================================================
            updateBoardP2_C(score);
            gotoxyP2_C(18, 30);
			printf("Press any key ");
	        getchP2_C();
          break;
          case '5': //Desplaça números a la dreta
            clearScreen_C();  
            printBoardP2_C(); 
            //===================================================
            shiftNumbersRP2(m);
			//shiftNumbersRP2_C(m);
			//===================================================
            updateBoardP2_C(score);
            gotoxyP2_C(18, 30);
			printf("Press any key ");
	        getchP2_C();
          break;
          case '6': //Fer parelles i sumar punts
            clearScreen_C();  
            printBoardP2_C();   
            score = 1000;
            //===================================================
			score = score + addPairsRP2(m);
			//score = score + addPairsRP2_C(m);
			//===================================================
			updateBoardP2_C(score);
			gotoxyP2_C(18, 30);
			printf("Press any key ");
	        getchP2_C();
         break;
         case '7': //Verificar si hi ha el 2048 o si es pot fer algun moviment.
            clearScreen_C();        
            printBoardP2_C();       
            updateBoardP2_C(score); 
            //===================================================
			checkEndP2();
			//checkEndP2_C();
			//===================================================
			printMessageP2_C();
			if ((state!='4') && (state!='5')) {
			   gotoxyP2_C(18, 30);
			   printf("Press any key ");
	           getchP2_C();
	        }
         break;
         case '8': //Joc complet Assemblador
            //=======================================================
            playP2();
            //=======================================================
         break;
         case '9': //Joc complet C 
            //=======================================================
            playP2_C();
            //=======================================================
         break;
      }
   }
   printf("\n\n");
   
   return 0;
   
}
