// Desarrollado por Lucas Capiz
// Proyecto final para la materia Laboratorio de Microprocesadores de la Universidad Tecnológica Nacional-INSPT
// Smart Indoor: Control de temperatura || Control de humedad || Control lumínico

#include <hidef.h>      /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include <mc9s08sh8.h>
#include "lcd_AyC.h"
#include "lib_time.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Luces PTBD_PTBD7
#define BotonPantalla PTCD_PTCD3
#define BotonModo PTCD_PTCD2
#define LedHumedad PTBD_PTBD6
#define BotonLuces PTBD_PTBD1

// Declaracion de variables

char texto[16];
char decimales[][6] = {".00 C\0", ".25 C\0", ".50 C\0", ".75 C\0"}; //(Se debe poner el EOF \0 al final de cada cadena)
char TemperaturaSpi;
char str_num[9];
char str[9];
char TextoAscii[9];
int Tiempo[7];
int dec;
int Enteros;
int Pantalla = 1;
int Humedad;
int Valor;
int ModoActual = 0;
int ModoPlanta = 0;
int ModoLuces  = 0;
int i;

// Declaracion de funciones

void Bienvenida();
char temperaturaSpi();
int temperaturaLcd(char);
int humedad();
void controlDeBomba(int);
int setModo();
void tiempo(int *);
void tiempoAscii(int *, char *);
void intToAscii(int, char *);
void mostrarLcd(int, int, char *);

void main(void)
{

    // Configuracion del SH08

    EnableInterrupts;
    SOPT1 = 0x02; // SOPT1: COPT=0,STOPE=0,IICPS=0,BKGDPE=1,RSTPE=0
    PTBDD = 0xC8; // PTB7 como salida el resto como entrada
    PTCDD = 0xC;  // PTC 2 y 3 como salida lo demas como entrada

    // Inicializo LCD

    initLCD();
    delay(200);

    // Registros SPI

    SPIBR = 0b00000011; // Calculo la frecuencia: 500 Khz
    SPIC1 = 0b01010110; // Lo configuro como maestro y el tipo de clock.
    SPIC2 = 0b10000;    // SPIC2 ---> MODFEN=1,BIDIROE=0,SPISWAI=0,SPC0=0 (0b10000)

    // Configuracion entrada analogica

    ADCCFG = 0b00000000; // HIGH power | Short sample
    APCTL1 = 0b00010000; // Habilito AD4 como entrada analogica || Sensor humedad

    // Mensaje de presentación del proyecto en el LCD

    Bienvenida();
    delay(600);

    // Inicializo el tiempo en 12:00:00 para floración
    Tiempo[0] = 1;
    Tiempo[1] = 2;
    Tiempo[2] = 0;
    Tiempo[3] = 0;
    Tiempo[4] = 0;
    Tiempo[5] = 0;

    while (1)
    {

        switch (Pantalla)
        {
        case 1:

            TemperaturaSpi = temperaturaSpi();        // Devuelve la temperatura en formato SPI
            Enteros = temperaturaLcd(TemperaturaSpi); // Enteros para la temperatura en formato para el LCD
            dec = TemperaturaSpi & 3;
            Valor = 20;                             // Valor random
            intToAscii(Valor, TextoAscii);        // Creo un valor ascii con variables reutilizadas para poder usar mostrarLcd
            mostrarLcd(Enteros, dec, TextoAscii); // Muestra la temperatura

            break;

        case 2:

            Valor = 1; // Valor necesario para usar mostrarLcd
            Humedad = humedad();
            intToAscii(Humedad, TextoAscii); // Convierte la humedad en ascii+
            mostrarLcd(Valor, Humedad, TextoAscii); // Muestra la humedad
            controlDeBomba(Humedad);

            break;

        case 3:

            ModoActual = setModo(); // Alterna entre Floracion y vegetacion

            tiempo(Tiempo); // Funcion de contador descendente

            tiempoAscii(Tiempo, TextoAscii); // Convierte el tiempo en formato Ascii

            Valor = 2;
            mostrarLcd(Valor, ModoActual, TextoAscii); // Muestra el tiempo y el modo

            if (BotonPantalla == 1)
            {
                Pantalla = 0;
            }
            break;

        default:

            // Limitamos el rango de pantallas que existen (3)
            if (BotonPantalla == 1)
            {
                Pantalla = 1;
            }
            break;
        }

        if (BotonPantalla == 1)
        {
            Pantalla++;
        }

        if (Pantalla != 3)
        {

            tiempo(Tiempo); // Funcion de contador descendente
        }

        if (Pantalla != 2)
        {

            Humedad = humedad();
            controlDeBomba(Humedad);
        }
    }
    // Fin del WHILE

    for (;;)
    {
        //__RESET_WATCHDOG();	/* feeds the dog */
    } /* loop forever */
      /* please make sure that you never leave main */
}
// FIN DE MAIN

void Bienvenida() // Mensaje de presentacion
{

    lcdGoto(1, 1);
    strcpy(texto, "Final Lab.micros"); // Escribimos en un array "texto"
    printLCD(texto);
    delay(300);

    lcdGoto(1, 2);
    strcpy(texto, "   INSPT-UTN    "); // Escribimos en un array "texto"
    printLCD(texto);
    delay(300);

    lcdGoto(1, 1);
    strcpy(texto, "                "); // Escribimos en un array "texto"
    printLCD(texto);
    delay(300);

    lcdGoto(1, 1);
    strcpy(texto, "  Smart Indoor  "); // Escribimos en un array "texto"
    printLCD(texto);
    delay(300);
}

/////////// Funciones temperatura

char temperaturaSpi() // Transferencia de datos SPI y muestreo en el LCD
{

    while (SPIS_SPTEF == 0) // Espera flag de TX
        ;
    SPID = 0x0; // Envia un byte cualquiera para iniciar TX

    while (SPIS_SPTEF == 0) // Espera flag de TX
        ;
    SPID = 0x0; // Envia un segundo byte cualquiera continuar TX

    while (SPIS_SPRF == 0) // Espera flag de RX
        ;
    Enteros = SPID; // SPID = Registro de datos de SPI

    while (SPIS_SPRF == 0) // Espera flag de RX
        ;
    TemperaturaSpi = SPID;

    // Arrlegos para enviar el valor de temperatura y decimales al LCD

    TemperaturaSpi = TemperaturaSpi >> 3; // Descarta los ultimos 3 bits.

    return TemperaturaSpi;
}

int temperaturaLcd(char TemperaturaSpi) // Arreglamos la temperatura SPI para poder visualizarla en enteros
{

    TemperaturaSpi = TemperaturaSpi >> 2; // Descarta los decimales.
    Enteros = Enteros << 3;               // Abre lugar para el byte2.
    Enteros = Enteros & 0b111111000;      // Pone en cero los ultimos 3 bits.
    Enteros = Enteros | TemperaturaSpi;   // Concatena los dos bytes.

    return Enteros;
}

////////////////////////////// Funciones humedad

int humedad()
{

    ADCSC1 = 0b00100100;
    // Limitamos la humedad entre 0 y 99%
    Humedad = ADCRL;
    Humedad = ((255 - Humedad) * (99 - 0) / (255 - 0));
    return Humedad;
}

void controlDeBomba(int Humedad) // Prendemos y apagamos la bomba
{
    if (Humedad < 50 || Humedad > 75)
    {

        LedHumedad = 1;
    }
    else
    {

        LedHumedad = 0;
    }
}

////////////////// Funciones control luminico

void tiempo(int *Tiempo) // Contador descendente
{
    // Tiempo[0] = 1; --> Decena de horas
    // Tiempo[1] = 2; --> Unidad de horas
    // Tiempo[2] = 0; --> Decena de minutos
    // Tiempo[3] = 0; --> Unidad de minutos
    // Tiempo[4] = 0; --> Decena de segundos
    // Tiempo[5] = 0; --> Unidad de segundos


    if (BotonLuces == 1) // Si se presiona el botón, cambiamos de estado
    {
        ModoLuces = !ModoLuces; // Cambiamos el estado
        
    }
        
        if (ModoLuces == 1) // Modo floracion
        {

        Luces = 1;



    Tiempo[5]--; // Resta un segundo a las unidades de segundos

    if (Tiempo[5] < 0)
    {
        Tiempo[5] = 9;
        Tiempo[4]--;
    }

    if (Tiempo[4] < 0)
    {
        Tiempo[3]--;
        Tiempo[4] = 5;
    }

    if (Tiempo[3] < 0)
    {
        Tiempo[2]--;
        Tiempo[3] = 9;
    }
    if (Tiempo[2] < 0)
    {

        Tiempo[1]--;
        Tiempo[2] = 5;
    }

    if (Tiempo[1] < 0)
    {
        Tiempo[1] = 9;
        Tiempo[0] = 0;
    }
        } else if (ModoLuces == 0){
        Luces = 0;
        delay(200);
        }
    
}

int setModo() // Obtenemos el set del control luminico
{

    if (BotonModo == 1) // Si se presiona el botón, cambiamos de estado
    {
        ModoActual = !ModoActual; // Cambiamos el estado
        ModoPlanta = ModoActual;   // Modificamos el tiempo de las luces
        ModoLuces  = 0;
        if (ModoPlanta == 1) // Modo vegetacion
        {
            Tiempo[0] = 1;
            Tiempo[1] = 8;
            Tiempo[2] = 0;
            Tiempo[3] = 0;
            Tiempo[4] = 0;
            Tiempo[5] = 0;
        }
        else if (ModoPlanta == 0) // Modo floracion
        {

            Tiempo[0] = 1;
            Tiempo[1] = 2;
            Tiempo[2] = 0;
            Tiempo[3] = 0;
            Tiempo[4] = 0;
            Tiempo[5] = 0;
        }
    }

    return ModoActual;
}

void tiempoAscii(int *Tiempo, char *TextoAscii)
{

    // El caracter '0' (Lo trato como un char, no como un int) tiene como valor numérico 48
    // 1 + 48 = 49 --> Es el valor numerico del 1 en formato ASCII
    // X + '0' = X en ascii || Con X entre 0 y 9

    TextoAscii[0] = Tiempo[0] + '0';
    TextoAscii[1] = Tiempo[1] + '0';
    TextoAscii[3] = Tiempo[2] + '0';
    TextoAscii[4] = Tiempo[3] + '0';
    TextoAscii[6] = Tiempo[4] + '0';
    TextoAscii[7] = Tiempo[5] + '0';
    TextoAscii[2] = ':';
    TextoAscii[5] = ':';
}

///////////////// Funciones globales

void intToAscii(int num, char *str)
{
    int decena = num / 10;
    int unidad = num % 10;
    str[0] = decena + '0';
    str[1] = unidad + '0';
}

void mostrarLcd(int Int0, int Int, char *str_num)
{

    switch (Int0)
    {

    default:

        lcdGoto(1, 1);
        sprintf(texto, "Temp: %d%s   ", Int0, decimales[Int]); // Sprintf se puede modificar a futuro
        printLCD(texto);

        if (Int0 >= 26 && Int0 <= 28)
        {

            lcdGoto(1, 2);
            strcpy(texto, "Clima optimo");
            // strcpy(texto + 6, str_num);
            strcat(texto, " 1/3");
            printLCD(texto);
        }
        else if (Int0 > 30 || Int0 <= 23)
        {

            lcdGoto(1, 2);
            strcpy(texto, "Alerta roja");
            // strcpy(texto + 6, str_num);
            strcat(texto, "  1/3");
            printLCD(texto);
        }
        else if ((Int0 >= 24 && Int0 <= 25) || (Int0 >= 29 && Int0 <= 30))
        {
            lcdGoto(1, 2);
            strcpy(texto, "Moderada");
            // strcpy(texto + 6, str_num);
            strcat(texto, "     1/3");
            printLCD(texto);
        }

        break;

    case 1:

        lcdGoto(1, 1);
        strcpy(texto, "Humedad : ");
        strcpy(texto + 10, str_num);
        strcat(texto, " %  ");
        printLCD(texto);

        if (Int < 50 || Int > 75)
        {

            lcdGoto(1, 2);
            strcpy(texto, "Bomba: ON    2"); // Guarda en array lo que se quiere mostrar en pantalla.
            printLCD(texto);
            delay(100);
        }
        else
        {

            lcdGoto(1, 2);
            strcpy(texto, "Bomba: OFF   2"); // Guarda en array lo que se quiere mostrar en pantalla.
            printLCD(texto);
            delay(100);
        }

        break;

    case 2:

        if (Int == 1)
        {
            lcdGoto(1, 1);
            strcpy(texto, "Modo: Vegetacion");
            printLCD(texto);
        }

        if (Int == 0)
        {
            lcdGoto(1, 1);
            strcpy(texto, "Modo: Floracion ");
            printLCD(texto);
        }

        lcdGoto(1, 2);

        strcpy(texto, str_num);
        strcat(texto, " HS  3");

        printLCD(texto);
        break;
    }
}
