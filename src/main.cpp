#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Keypad.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

// Pantalla
// U8G2_R0 = Horizontal
// U8G2_R1 o R3 = Vertical

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R1,
                                         OLED_SCL, OLED_SDA, OLED_RST);

const byte ROWS = 4;
const byte COLS = 4;


char keys[ROWS][COLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 26, 5, 18};
byte colPins[COLS] = {19, 13, 12, 14};

Keypad keypad = Keypad(
        makeKeymap(keys),
        rowPins,
        colPins,
        ROWS,
        COLS);

// -------------------PERSONAJE

void dibujaCaha(int x,int y){
    x++;
    u8g2.drawFrame(x,y,4,4);
    u8g2.drawFrame(x+1,y+4,2,1);
    u8g2.drawFrame(x,y+5,1,1);
    u8g2.drawFrame(x+3,y+5,1,1);
    u8g2.drawFrame(x+5,y+2,1,2);  //pistola
    u8g2.drawFrame(x,y-1,3,1);
    u8g2.drawFrame(x+4,y,1,1);
}


// -------------------PLATAFORMA
void dibujaPlataforma(int x, int y){
    u8g2.drawFrame(x,y,6,2);
}


//---------------------MONEDA
void dibujaMoneda(int x, int y){
    u8g2.drawFrame(x+2,y+1,2,3);
}



int tiempo=0;

int salto = 0;
int contador = 0;
int vidas=3;

int posX = 4, posXOriginal= 4;
int posY = 19, posYOriginal= 19;

// matrix (tablero)

int matrix[10][20]={};


// Puede moverse? Separado para que la gravedad funcione.--------

int puedeMoverseVertical(int x, int y,int matricilla[10][20]){
    if (matricilla[x][y] > 0 || y < 0 || y > 19) return 0;
    else return 1;
}

int puedeMoverseHorizontal(int x, int y,int matricilla[10][20]) {
    if (matricilla[x][y] > 0 || x < 0 || x > 9) return 0;
    else return 1;
}
//-------------------------------------------


// Dibujar pantalla -----------------------------

void dibujaPantalla(int mapa[10][20], int vidas){

    for (int i = 0; i<10;i++){
        for (int j  = 0; j < 20; ++j) {

            if  (mapa[i][j]==1) {dibujaPlataforma((i*6+2),(j*6+6));
            }

            else if (mapa[i][j]==2) {

                if (vidas)  {dibujaCaha(i*6+2,j*6+6);
                }

                //else    muerte();
            }

            else if (mapa[i][j]==-1) dibujaMoneda(i*6+2,j*6+6);
            /*
            else if (mapa[i][j]==4) dibujaEnemigo();*/
        }
    }
}
//--------------------------------------------------





void setup() {

    u8g2.begin();
    u8g2.clearBuffer();
    randomSeed(analogRead(0));
    int matrix[10][20]={};

    posX=0;
    posY=19;
    contador=0;


    vidas=3;
}



void loop() {

    matrix[posX][posY]=2;

    matrix[5][18]=1;matrix[6][18]=1;matrix[4][18]=1;
    matrix[2][12]=1;matrix[3][12]=1;matrix[4][12]=1;  matrix[8][19]=-1;
    matrix[3][10]=1;matrix[2][10]=1;matrix[1][10]=1;  matrix[2][9]=-1;
    matrix[4][19]=1;
    matrix[5][17]=1;

    matrix[4][15]=1;

    matrix[5][13]=1;

    while (true){
        posX++;
        keypad.getKeys();
        u8g2.clearBuffer();

        posXOriginal=posX;
        posYOriginal=posY;

        if (keypad.key[keypad.findInList('4')].kstate == PRESSED ||
            keypad.key[keypad.findInList('4')].kstate == HOLD) {
            posX-=1;
        }

        if (keypad.key[keypad.findInList('6')].kstate == PRESSED ||
            keypad.key[keypad.findInList('6')].kstate == HOLD) {
            posX+=1;
        }

        if (keypad.key[keypad.findInList('8')].kstate == PRESSED && salto==0) {
            salto=1;

        }

        // Gravedad (posición necesaria debajo de activar salto)


        if (salto==0 && (tiempo+220)<millis()){
            posY+=1;
            tiempo=0;
        }
        if (tiempo == 0){
            tiempo=millis();
        }


/*
  if (keypad.key[keypad.findInList('2')].kstate == PRESSED ||
      keypad.key[keypad.findInList('2')].kstate == HOLD) {
    posY+=2;
  }

  if (keypad.key[keypad.findInList('5')].kstate == PRESSED) {

  }
*/
        //Primero se compila la bajada y luego la subida, para evitar que suba y baje en el mismo tick de tiempo

        if (salto==1 && contador>3){
            posY+=1;
            contador++;
            if (contador==8){
                salto=0;
                contador=0;
            }
        }

        if (salto==1 && contador<4){
            posY-=1;
            contador++;
        }



//----------------------¿Se mueve?------------------------------

        if (matrix[posX][posY] == -1) matrix[posX][posY]=0;

        //si hay un enemigo, perderemos una vida, pero el enemigo desaparece.
        //La muerte ocurrirá una vez se actualize el mapa, para mayor realidad.
/*
    if (matrix[posX][posY] == 4) {
        matrix[posX][posY]=0;
        vidas-=1;
    }
*/
        //que cancele el movimiento, para mantenerse encima, si hay una plataforma


        if (!puedeMoverseHorizontal(posX,posY,matrix)){posX=posXOriginal;}

        if (!puedeMoverseVertical(posX,posY,matrix)) {posY=posYOriginal;}

        //intercambio de posiciones para su dibujo en pantalla
        matrix[posXOriginal][posYOriginal]=0;
        matrix[posX][posY]=2;

//------------------------------------------------------------

        dibujaPantalla(matrix, vidas);

        // marco pantalla

        u8g2.drawFrame(0, 6, 64, 121);

        u8g2.setFont(u8g2_font_u8glib_4_hf);

        //serán funcionales

        u8g2.drawStr(0, 4, "Vidas: 5 ");

        u8g2.drawStr(0, 12, "Puntos: 000");

        u8g2.sendBuffer();

        delay(100);
    }
}











