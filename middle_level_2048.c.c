/**
 * Implementació en C de la pràctica, per a què tingueu una
 * versió funcional en alt nivell de totes les funcions que heu 
 * d'implementar en assemblador.
 * Des d'aquest codi es fan les crides a les subrutines de assemblador. 
 * AQUEST CODI NO ES POT MODIFICAR I NO S'HA DE LLIURAR.
 **/

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>     //termios, TCSANOW, ECHO, ICANON
#include <unistd.h>      //STDIN_FILENO

extern int developer;    //Variable declarada en assemblador que indica el nom del programador

/**
 * Constants
 */
#define DimMatrix  4     //dimensió de la matriu
#define SizeMatrix DimMatrix*DimMatrix //=16


/**
 * Definició de variables globals
 */
int  rowScreen;//Fila per a posicionar el cursor a la pantalla.
int  colScreen;//Columna per a posicionar el cursor a la pantalla.
char charac;   //Caràcter llegit de teclat i per a escriure a pantalla.

   
// Matriu 9x9 on guardem els números del joc.
// Accés a les matrius en C: utilitzem fila (0..[DimMatrix-1]) i 
// columna(0..[DimMatrix-1]) (m[fila][columna]).
// Accés a les matrius en assemblador: S'hi accedeix com si fos un vector 
// on indexMat (0..[DimMatrix*DimMatrix-1]). 
// indexMat=((fila*DimMatrix)+(columna))*2 (2 perquè la matriu és de tipus short).
// WORD[m+indexMat] (WORD perquè és de tipus short) 
// (indexMat ha de ser un registre de tipus long/QWORD:RAX,RBX,..,RSI,RDI,..,R15).
short m[DimMatrix][DimMatrix]        = { {    8,    8,    32,    32},
                                         {    4,   32,   128,    64},
                                         {    0,    0,   256,   128},
                                         {    0,    4,   512,  1024} };

short mRotated[DimMatrix][DimMatrix] = { {    2,    0,     2,     0},
                                         {    2,    2,     4,     4},
                                         {    4,    4,     0,     4},
                                         {    4,    2,     2,     4} };

char  state  = '1';    // '0': Sortir, hem premut la tecla 'ESC' per a sortir.
                       // '1': Continuem jugant.
                       // '2': Continuem jugant però s'han fet canvis a la matriu.

int   number;          //Numero que volem mostrar.
int   score  = 290500; // Punts acumulats al marcador.                    


/**
 * Definició de les funcions de C
 */
void clearscreen_C();
void gotoxyP1_C();
void printchP1_C();
void getchP1_C();

void printMenuP1_C();
void printBoardP1_C();

void showNumberP1_C();
void updateBoardP1_C();
void copyMatrixP1_C();
void rotateMatrixRP1_C();
void shiftNumbersRP1_C();
void addPairsRP1_C();

void readKeyP1_C();
void insertTileP1_C();

void printMessageP1_C();
void playP1_C();

/**
 * Definició de les subrutines d'assemblador que es criden des de C.
 */
extern void showNumberP1();
extern void updateBoardP1();
extern void copyMatrixP1();
extern void rotateMatrixRP1();
extern void shiftNumbersRP1();
extern void addPairsRP1();
extern void readKeyP1();
extern void playP1();


/**
 * Esborrar la pantalla
 * 
 * Variables globals utilitzades:   
 * Cap
 * 
 * Aquesta funció no es crida des d'assemblador
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void clearScreen_C(){
   
    printf("\x1B[2J");
    
}


/**
 * Situar el cursor a la fila indicada per la variable (rowScreen) i a 
 * la columna indicada per la variable (colScreen) de la pantalla.
 * 
 * Variables globals utilitzades:   
 * (rowScreen): Fila de la pantalla on posicionem el cursor.
 * (colScreen): Columna de la pantalla on posicionem el cursor.
 * 
 * S'ha definit un subrutina en assemblador equivalent 'gotoxyP1' per a 
 * poder cridar aquesta funció guardant l'estat dels registres del 
 * processador. Això es fa perquè les funcions de C no mantenen 
 * l'estat dels registres.
 */
void gotoxyP1_C(){
   
   printf("\x1B[%d;%dH",rowScreen,colScreen);
   
}


/**
 * Mostrar un caràcter guardat a la variable (charac) a la pantalla, 
 * en la posició on està el cursor.
 * 
 * Variables globals utilitzades:   
 * (charac): Caràcter que volem mostrar.
 * 
 * S'ha definit un subrutina en assemblador equivalent 'printchP1' per a
 * cridar aquesta funció guardant l'estat dels registres del processador.
 * Això es fa perquè les funcions de C no mantenen l'estat dels registres.
 */
void printchP1_C(){

   printf("%c",charac);
   
}


/**
 * Llegir una tecla i guardar el caràcter associat a la variable (charac)
 * sense mostrar-lo per pantalla. 
 * 
 * Variables globals utilitzades:   
 * (charac): Caràcter que llegim de teclat.
 * 
 * S'ha definit un subrutina en assemblador equivalent 'getchP1' per a
 * cridar aquesta funció guardant l'estat dels registres del processador.
 * Això es fa perquè les funcions de C no mantenen l'estat dels 
 * registres.
 */
void getchP1_C(){

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
   charac = (char) getchar();                 
    
   /*restaurar els paràmetres originals*/
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
   
}


/**
 * Mostrar a la pantalla el menú del joc i demana una opció.
 * Només accepta una de les opcions correctes del menú ('0'-'9')
 * 
 * Variables globals utilitzades:   
 * (rowScreen): Fila de la pantalla on posicionem el cursor.
 * (colScreen): Columna de la pantalla on posicionem el cursor.
 * (charac)   : Caràcter que llegim de teclat.
 * (developer): ((char *)&developer): Variable definida en el codi assemblador.
 * 
 * Aquesta funció no es crida des d'assemblador
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void printMenuP1_C(){
    clearScreen_C();
    rowScreen = 1;
    colScreen = 1;
    gotoxyP1_C();
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
    printf("|                                  |\n");
    printf("|         8. Play Game             |\n");
    printf("|         9. Play Game C           |\n");
    printf("|         0. Exit                  |\n");
    printf("|__________________________________|\n");
    printf("|                                  |\n");
    printf("|            OPTION:               |\n");
    printf("|__________________________________|\n"); 

    charac=' ';
    while (charac < '0' || charac > '9') {
      rowScreen = 21;
      colScreen = 22;
      gotoxyP1_C();
      getchP1_C();
      printchP1_C();
   }
   
}


/**
 * Mostrar el tauler de joc a la pantalla. Les línies del tauler.
 * 
 * Variables globals utilitzades:   
 * (rowScreen): Fila de la pantalla on posicionem el cursor.
 * (colScreen): Columna de la pantalla on posicionem el cursor.
 *  
 * Aquesta funció es crida des de C i des d'assemblador,
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void printBoardP1_C(){

   rowScreen = 1;
   colScreen = 1;
   gotoxyP1_C();
   printf(" _________________________________________________  \n"); //01
   printf("|                                                  |\n"); //02
   printf("|                  2048 PUZZLE  v1.0               |\n"); //03
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
   printf("|  (ESC)Exit  (i)Up   (j)Left  (k)Down  (l)Right   |\n"); //21
   printf("|__________________________________________________|\n"); //22
   
}


/**
 * Converteix el número de la variable (number) de tipus short de 6 dígits
 * (number <= 999999) a caràcters ASCII que representen el seu valor.
 * Si (number) és més gran que 999999 canviarem el valor a 999999.
 * S'ha de dividir(/) el valor entre 10,  de forma iterativa, 
 * fins obtenir el 6 digits.
 * A cada iteració, el residu de la divisió (%) que és un valor
 * entre (0-9) indica el valor del dígit que s'han de convertir
 * a ASCII ('0' - '9') sumant '0' (48 decimal) per a poder-lo mostrar.
 * Quan el quocient sigui 0 mostrarem espais a la part no significativa.
 * Per exemple, si number=103 mostrarem "   103" i no "000103".
 * S'han de mostrar els dígits (caràcter ASCII) des de la posició 
 * indicada per les variables (rowScreen) i (colScreen), posició de les 
 * unitats, cap a l'esquerra.
 * El primer dígit que obtenim són les unitats, després les desenes,
 * ..., per a mostrar el valor s'ha de desplaçar el cursor una posició
 * a l'esquerra a cada iteració.
 * Per a posicionar el cursor es crida a la funció gotoxyP1_C i per a 
 * mostrar els caràcters a la funció printchP1_C.
 * 
 * Variables globals utilitzades:   
 * (number)   : Número que volem mostrar.
 * (rowScreen): Fila de la pantalla on posicionem el cursor.
 * (colScreen): Columna de la pantalla on posicionem el cursor.
 * (charac)   : Caràcter a escriure a pantalla.
 * 
 * Aquesta funció no es crida des d'assemblador.
 * Hi ha una subrutina en assemblador equivalent 'showNumberP1'. 
 */
 void showNumberP1_C() {
   
   int n = number;
   int i;

   if (n > 999999) n = 999999;
   for (i=0;i<6;i++){
     charac = ' ';
     if (n > 0) {
	   charac = n%10;     //residu
	   n = n/10;          //quocient
	   charac = charac + '0';
	 }
     gotoxyP1_C();
     printchP1_C();
     colScreen--;
   }

}


/**
 * Actualitzar el contingut del Tauler de Joc amb les dades de la matriu 
 * (m) de 4x4 de tipus short i els punts del marcador (score) que s'han fet.  
 * S'ha de recórrer tota la matriu (m), i per a cada element de la matriu
 * posicionar el cursor a la pantalla i mostrar el nombre d'aquella
 * posició de la matriu.
 * Recorrer tota la matriu per files d'esquerra a dreta i de dalt a baix.
 * Per recorrer la matriu en assemblador l'index va de 0 (posició [0][0])
 * a 30 (posició [3][3]) amb increments de 2 perquè les dades son de
 * tipus short(WORD) 2 bytes.
 * Després, mostrar el marcador (score) a la part inferior del tauler,
 * fila 18, columna 26 cridant la funció showNumberP1_C.
 * Finalment posicionar el cursor a la fila 18, columna 28 cridant la 
 * funció gotoxyP1_C().
 *  
 * Variables globals utilitzades:   
 * (rowScreen): Fila de la pantalla on posicionem el cursor.
 * (colScreen): Columna de la pantalla on posicionem el cursor.
 * (m)        : Matriu on guardem els nombres del joc.
 * (score)    : Punts acumulats al marcador.
 * (number)   : Número que volem mostrar.
 * 
 * Aquesta funció no es crida des d'assemblador.
 * Hi ha una subrutina en assemblador equivalent 'updateBoardP1'.
 */
void updateBoardP1_C(){

   int i,j;
   int rowScreenAux;
   int colScreenAux;
   
   rowScreenAux = 10;
   for (i=0;i<DimMatrix;i++){
     colScreenAux = 17;
      for (j=0;j<DimMatrix;j++){
         number = m[i][j];
         rowScreen = rowScreenAux;
         colScreen = colScreenAux;
         showNumberP1_C();
         colScreenAux = colScreenAux + 9;
      }
      rowScreenAux = rowScreenAux + 2;
   }
   
   number = score;
   rowScreen = 18;
   colScreen = 26;
   showNumberP1_C();   
   rowScreen = 18;
   colScreen = 28;
   gotoxyP1_C();
   
}


/**
 * Copiar els valors de la matriu (mRotated) a la matriu (m).
 * La matriu (mRotated) no s'ha de modificar, 
 * els canvis s'han de fer a la matriu (m).
 * Recorrer tota la matriu per files d'esquerra a dreta i de dalt a baix.
 * Per recorrer la matriu en assemblador l'index va de 0 (posició [0][0])
 * a 30 (posició [3][3]) amb increments de 2 perquè les dades son de
 * tipus short(WORD) 2 bytes.
 * No es mostrar la matriu.
 * 
 * Variables globals utilitzades:   
 * (m)       : Matriu on guardem els nombres del joc.
 * (mRotated): Matriu amb els nombres rotats a la dreta.
 *
 * Aquesta funció no es crida des d'assemblador.
 * Hi ha una subrutina en assemblador equivalent 'copyMatrixP1'.
 */
void copyMatrixP1_C() {

   int i,j;
   
   for (i=0; i<DimMatrix; i++) {
      for (j=0; j<DimMatrix; j++) {   
         m[i][j] = mRotated[i][j];
      }
   }
   
}


/**
 * Rotar a la la dreta la matriu (m), sobre la matriu (mRotated).
 * La primera fila passa a ser la primera columna, la segona fila passa
 * a ser la segona columna, la tercera fila passa a ser la tercera columna
 * i la quarta fila passa a ser la quarta columna.
 * A l'enunciat s'explica en més detall com fer la rotació.
 * NOTA: NO és el mateix que fer la matriu transposada.
 * La matriu (m) no s'ha de modificar, 
 * els canvis s'han de fer a la matriu (mRotated).
 * Per recórrer la matriu en assemblador l'índex va de 0 (posició [0][0])
 * a 30 (posició [3][3]) amb increments de 2 perquè les dades son de 
 * tipus short(WORD) 2 bytes.
 * Per a accedir a una posició concreta de la matriu des d'assemblador 
 * cal tindre en compte que l'índex és:(index=(fila*DimMatrix+columna)*2),
 * multipliquem per 2 perquè les dades son de tipus short(WORD) 2 bytes.
 * No s'ha de mostrar la matriu.
 * Un cop s'ha fet la rotació, copiar la matriu (mRotated) a la matriu
 * (m) cridant la funció copyMatrixP1_C().
 * 
 * Variables globals utilitzades:   
 * (m)       : Matriu on guardem els nombres del joc.
 * (mRotated): Matriu amb els nombres rotats a la dreta.
 * 
 * Aquesta funció no es crida des d'assemblador.
 * Hi ha una subrutina en assemblador equivalent 'rotateMatrixRP1'.
 */
void rotateMatrixRP1_C() {
   
   int i,j;
   
   for (i=0; i<DimMatrix; i++) {
      for (j=0; j<DimMatrix; j++) {   
         mRotated[j][DimMatrix-1-i] = m[i][j];
      }
   }
   
   copyMatrixP1_C();
   
}



/**
 * Desplaça a la dreta els números de cada fila de la matriu (m), 
 * mantenint l'ordre dels números i posant els zeros a l'esquerra.
 * Recórrer la matriu per files de dreta a esquerra i baix a dalt.
 * Per recórrer la matriu en assemblador, en aquest cas, l'índex va de la
 * posició 30 (posició [3][3]) a la 0 (posició [0][0]) amb decrements de
 * 2 perquè les dades son de tipus short(WORD) 2 bytes.
 * Per a accedir a una posició concreta de la matriu des d'assemblador 
 * cal tindre en compte que l'índex és:(index=(fila*DimMatrix+columna)*2),
 * multipliquem per 2 perquè les dades son de tipus short(WORD) 2 bytes. 
 * Si es desplaça un número (NO ELS ZEROS) posarem la variable 
 * (state) a '2'.
 * A cada fila, si troba un 0, mira si hi ha un número diferent de zero,
 * a la mateixa fila per a posar-lo en aquella posició.
 * Si una fila de la matriu és: [0,2,0,4] i state = '1', quedarà [0,0,2,4] 
 * i  state = '2'.
 * Els canvis s'han de fer sobre la mateixa matriu.
 * No s'ha de mostrar la matriu.
 * 
 * Variables globals utilitzades:   
 * (m)    : Matriu on guardem els nombres del joc.
 * (state): Estat del joc. ('2': S'han fet moviments).
 *
 * Aquesta funció no es crida des d'assemblador.
 * Hi ha una subrutina en assemblador equivalent 'shiftNumbersRP1', 
 */
void shiftNumbersRP1_C() {
   
   int i,j,k;
   
   for (i=DimMatrix-1; i>=0; i--) {
      for (j=DimMatrix-1; j>0; j--) {
        if (m[i][j] == 0) {
          k = j-1;           
          while (k>=0 && m[i][k]==0) k--;
          if (k==-1) {
             j=0;                
          } else {
            m[i][j]=m[i][k];
             m[i][k]= 0; 
             state='2';        
          }
        }      
      }
    }
    
}
   

/**
 * Aparellar nombres iguals des la dreta de la matriu (m) i acumular 
 * els punts al marcador sumant els punts de les parelles que s'hagin fet.
 * Recórrer la matriu per files de dreta a esquerra i de baix a dalt. 
 * Quan es trobi una parella, dos caselles consecutives de la mateixa 
 * fila amb el mateix número, ajuntem la parella posant la suma de la 
 * parella a la casella de la dreta, un 0 a la casella de l'esquerra i 
 * acumularem aquesta suma a la variable (p) (punts guanyats).
 * Si una fila de la matriu és: [8,4,4,2] i state = '1', quedarà [8,0,8,2], 
 * p = p + (4+4) i state = '2'.
 * Si al final s'ha ajuntat alguna parella (p>0), posarem la variable 
 * (state) a '2' per indicar que s'ha mogut algun nombre i actualitzarem
 * la variable (score) amb els punts obtinguts de fer les parelles.
 * Per recórrer la matriu en assemblador, en aquest cas, l'índex va de la
 * posició 30 (posició [3][3]) a la 0 (posició [0][0]) amb increments de 
 * 2 perquè les dades son de tipus short(WORD) 2 bytes.
 * Els canvis s'han de fer sobre la mateixa matriu.
 * No s'ha de mostrar la matriu.
 * 
 * Variables globals utilitzades:   
 * (m)    : Matriu on guardem els nombres del joc.
 * (score): Punts acumulats al marcador.
 * (state): Estat del joc. ('2': S'han fet moviments).
 *
 * Aquesta funció no es crida des d'assemblador.
 * Hi ha una subrutina en assemblador equivalent 'addPairsRP1'.
 */
void addPairsRP1_C() {
   
   int i,j;
   short p = 0;
   
   for (i=DimMatrix-1; i>=0; i--) {
      for (j=DimMatrix-1; j>0; j--) {
         if ((m[i][j]!=0) && (m[i][j]==m[i][j-1])) {
            m[i][j]  = m[i][j]*2;
            m[i][j-1]= 0;
            p = p + m[i][j];
         }      
      }
   }
   
   if (p > 0) {
      state = '2';
      score = score + p;
   }
   
}


/**
 * Llegir una tecla cridant la funció getchP1_C i quedarà guardada 
 * a la variable (charac).
 * Segons la tecla llegida cridarem a les funcions corresponents.
 *    ['i' (amunt),'j'(esquerra),'k' (avall) o 'l'(dreta)] 
 * Desplaçar els números i fer les parelles segons la direcció triada.
 * Segons la tecla premuda, rotar la matriu cridant (rotateMatrixRP1_C),
 * per a poder fer els desplaçaments dels nombres cap a la dreta 
 * (shiftNumbersRP1_C), fer les parelles cap a la dreta (addPairsRP1_C) 
 * i tornar a desplaçar els nombres cap a la dreta (shiftNumbersRP1_C) 
 * amb les parelles fetes, després seguir rotant la matriu cridant 
 * (rotateMatrixRP1_C) fins deixar la matriu en la posició inicial. 
 * Per a la tecla 'l' (dreta) no cal fer rotacions, per a la resta 
 * s'han de fer 4 rotacions.
 *    '<ESC>' (ASCII 27)  posar (state = '0') per a sortir del joc.
 * Si no és cap d'aquestes tecles no fer res.
 * Els canvis produïts per aquestes funcions no s'han de mostrar a la 
 * pantalla, per tant, caldrà actualitzar després el tauler cridant la 
 * funció updateBoardP1_C.
 * 
 * Variables globals utilitzades:   
 * (charac)   : Caràcter llegit de teclat.
 * (state)    : Indica l'estat del joc. '0':sortir (ESC premut), '1':jugar.
 *
 * Aquesta funció no es crida des d'assemblador.
 * Hi ha una subrutina en assemblador equivalent 'readKeyP1'.
 */
void readKeyP1_C(){
   
   getchP1_C();   
   
   switch(charac){
      case 'i': //i:(105) amunt
         rotateMatrixRP1_C();
                 
         shiftNumbersRP1_C();
         addPairsRP1_C();
         shiftNumbersRP1_C();  
         
         rotateMatrixRP1_C();
         rotateMatrixRP1_C();
         rotateMatrixRP1_C();
      break;
      case 'j': //j:(106) esquerra
         rotateMatrixRP1_C();
         rotateMatrixRP1_C();
         
         shiftNumbersRP1_C();
         addPairsRP1_C();
         shiftNumbersRP1_C();
         
         rotateMatrixRP1_C();
         rotateMatrixRP1_C();
      break;
      case 'k': //k:(107) avall
         rotateMatrixRP1_C();
         rotateMatrixRP1_C();
         rotateMatrixRP1_C(); 
              
         shiftNumbersRP1_C();
         addPairsRP1_C();
         shiftNumbersRP1_C();
         
         rotateMatrixRP1_C();
      break;
      case 'l': //l:(108) dreta
         shiftNumbersRP1_C();
         addPairsRP1_C();
         shiftNumbersRP1_C();
      break;  
     case 27: //ESC:(27) Sortir del programa
       state = '0';
     break;
   }
   
}


/**
 * Generar nova fitxa de forma aleatòria.
 * Si hi ha com a mínim una casella buida a la matriu (m) genera una  
 * fila i una columna de forma aleatòria fins que és una de les caselles 
 * buides. A continuació posa un 2 a aquella casella.
 * 
 * Variables globals utilitzades:   
 * (m): Matriu on guardem els nombres del joc.
 * 
 * Aquesta funció es crida des de C i des d'assemblador,
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void insertTileP1_C() {
   
   int i,j,k,l;
   
   i=DimMatrix; // Mirem si hi ha una casella buida.
   do {
      i--;
      j=DimMatrix;
      do {
         j--;   
      } while ((j>=0) && (m[i][j]!=0));
   } while ((i>=0) && (m[i][j]!=0));
   
   //Inserim el 2 si hi ha com a mínim una casella buida.
   if (m[i][j]==0) { 
      do { // Genera files i columnes aleatòriament fins que troba
          // una posició buida
         k = rand() % 4; l = rand() % 4; }
      while( m[k][l] != 0 );
      
      m[k][l] = 2; //Posem un 2
      
   }
      
}


/**
 * Mostra un missatge a sota del tauler, al costat del marcador, segons
 * el valor de la variable (state).
 * state: '0': Hem premut la tecla 'ESC' per a sortir del joc.
 *             S'espera que es premi una tecla per a continuar.
 * 
 * Variables globals utilitzades:   
 * (state)    : Estat del joc. ('0': Sortir, hem premut la tecla 'ESC').
 * (rowScreen): Fila de la pantalla on posicionem el cursor.
 * (colScreen): Columna de la pantalla on posicionem el cursor. 
 *  
 * Aquesta funció es crida des de C i des d'assemblador,
 * i no hi ha definida una subrutina d'assemblador equivalent.
 */
void printMessageP1_C() {

   switch(state){
      case '0':
         rowScreen = 23;
         colScreen = 12;
         gotoxyP1_C();
         printf("<<<<<< EXIT: (ESC) Pressed >>>>>>");
      break;
   }
   getchP1_C();
   
}
 

/**
 * Joc del 2048
 * Funció principal del joc
 * Permet jugar al joc del 2048 cridant totes les funcionalitats.
 *
 * Pseudo codi:
 * Inicialitzar estat del joc, (state='1')
 * Esborrar pantalla (cridar la funció clearScreen_C).
 * Mostrar el tauler de joc (cridar la funció printBoardP1_C).
 * Actualitza el contingut del Tauler de Joc i els punts que s'han fet
 * (cridar la funció updateBoardP1_C).
 * Mentre (state=='1') fer
 *   Llegir una tecla (cridar la funció readKeyP1_C) i cridar les funcions corresponents.
 *   Si hem mogut algun número al fer els desplaçaments o al fer les 
 *   parelles (state=='2'). Generar una nova fitxa (cridant la funció 
 *   insertTileP1_C) i posar la variable state a '1' (state='1').
 *   Actualitza el contingut del Tauler de Joc i els punts que s'han fet
 *   (cridar la funció updateBoardP1_C).
 * Fi mentre.
 * Mostrar un missatge a sota del tauler segons el valor de la variable 
 * (state). (cridar la funció printMessageP1_C).
 * Sortir: 
 * S'acabat el joc.
 * 
 * Variables globals utilitzades:   
 * (state)    : Estat del joc.
 *              '0': Sortir, hem premut la tecla 'ESC').
 *              '1': Continuem jugant.
                '2': Continuem jugant però s'han fet canvis a la matriu.
 */
void playP1_C(){
   
   state = '1';               
   
   clearScreen_C();
   printBoardP1_C();
   updateBoardP1_C();       
   while (state == '1') {     //Bucle principal.
      readKeyP1_C();
      if (state == '2') {     
         insertTileP1_C();    
         state = '1';
      }
      updateBoardP1_C();
   }
   printMessageP1_C();        
   
}


/**
 * Programa Principal
 * 
 * ATENCIÓ: Podeu provar la funcionalitat de les subrutines que s'han de
 * desenvolupar treient els comentaris de la crida a la funció 
 * equivalent implementada en C que hi ha sota a cada opció.
 * Per al joc complet hi ha una opció per la versió en assemblador i 
 * una opció pel joc en C.
 */
int main(void){
   
   while (charac!='0') {
     clearScreen_C();
     printMenuP1_C();    
      
      switch(charac){
         case '1':// Mostrar punts
            clearScreen_C();  
            printBoardP1_C(); 
            rowScreen = 18;
            colScreen = 30;
            gotoxyP1_C();
            printf(" Press any key ");
            //=======================================================
            rowScreen = 18;
            colScreen = 26;
            number = score;
            showNumberP1(); 
            //showNumberP1_C();   
            //=======================================================
            getchP1_C();
         break;
         case '2': //Actualitzar el contingut del tauler.
            clearScreen_C();  
            printBoardP1_C(); 
            //=======================================================
            updateBoardP1(); 
            //updateBoardP1_C();  
            //=======================================================
            rowScreen = 18;
            colScreen = 30;
            gotoxyP1_C();
            printf(" Press any key ");
            getchP1_C();
         break;
         case '3': //Copiar matriu mRotated a m
            clearScreen_C(); 
            printBoardP1_C();
            //===================================================
            copyMatrixP1();
            //copyMatrixP1_C();
            //===================================================
            updateBoardP1_C();
            rowScreen = 18;
            colScreen = 30;
            gotoxyP1_C();
            printf(" Press any key ");
            getchP1_C();
         break;
         case '4': //Rotar matriu a la dreta
            clearScreen_C();  
            printBoardP1_C(); 
            //===================================================
            rotateMatrixRP1();
            //rotateMatrixRP1_C();
            //===================================================
            updateBoardP1_C();
            rowScreen = 18;
            colScreen = 30;
            gotoxyP1_C();
            printf(" Press any key ");
            getchP1_C();
         break;
         case '5': //Desplaça números a la dreta
            clearScreen_C();  
            printBoardP1_C();
            //===================================================
            shiftNumbersRP1(); 
            //shiftNumbersRP1_C(); 
            //===================================================
            updateBoardP1_C(); 
            rowScreen = 18;
            colScreen = 30;
            gotoxyP1_C();
            printf(" Press any key ");
            getchP1_C();
         break;
         case '6': //Fer parelles i sumar punts
            clearScreen_C();  
            printBoardP1_C(); 
            //===================================================
            addPairsRP1();
            //addPairsRP1_C();
            //===================================================
            updateBoardP1_C();
            rowScreen = 20;
            colScreen = 30;
            gotoxyP1_C();
            printf(" Press any key ");
            getchP1_C();
         break;
         case '8': //Joc complet Assemblador    
            //=======================================================
            playP1();
            //=======================================================
         break;
         case '9': //Joc complet C    
            //=======================================================
            playP1_C();
            //=======================================================
         break;
      }
   }
   printf("\n\n");
   
   return 0;
}
