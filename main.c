#include <p18f4550.h>
#include <delays.h>
#include <stdlib.h>

#pragma config FOSC=HS          //HS - HIGH SPEED
#pragma config CPUDIV=OSC1_PLL2 //SEM DIVISOR DE FREQUENCIA
#pragma config PWRT=OFF         //
#pragma config BOR=OFF          //DESABILITAR RESET POR NIVEL DE TENSAO
#pragma config WDT=OFF          //DESLIGAR WATCH DOG TIMER
#pragma config MCLRE=ON         //HABILITA O PINO DE MASTER CLEAR
#pragma config CCP2MX=ON        //
#pragma config LVP=OFF

#define RS PORTEbits.RE0 // configuração do display
#define RW PORTEbits.RE2
#define ENA PORTEbits.RE1

#define ENTRADA PORTB
#define SAIDA PORTA

#define START PORTBbits.RB7
#define YELLOW_IN PORTBbits.RB0
#define GREEN_IN PORTBbits.RB1
#define RED_IN PORTBbits.RB2
#define BLUE_IN PORTBbits.RB3

#define YELLOW_OUT PORTAbits.RA0
#define GREEN_OUT PORTAbits.RA1
#define RED_OUT PORTAbits.RA2
#define BLUE_OUT PORTAbits.RA3

unsigned int i=0;
unsigned int n=0;
unsigned char YELLOW_flag, GREEN_flag, RED_flag, BLUE_flag;
unsigned char sequenciaGerada[50];
unsigned char sequenciaJogada[50];

//PROTOTIPO DE FUNCOES
void delay_ms(unsigned int tempo);
void delay_us(unsigned int tempo);
void escrevelcd (unsigned char valor, char valor_rs);
void inicializarpic (void);
void inicializarlcd (void);
void iniciarjogo (void);
int geravalor();
void mostrarnivellcd(void);
void mostrarSequenciaGerada(void);
void lerSequenciaJogada(void);
void gameover(void);
void acende_yellow(void);
void acende_green(void);
void acende_red(void);
void acende_blue(void);
void som(unsigned float periodoms, unsigned float tempoms);

//*****************************************************
//-----------------------------------------------------

void main()
{
    inicializarpic();

    inicializarlcd();

    iniciarjogo();

    while(1)
    {
        sequenciaGerada[i] = geravalor();

        mostrarnivellcd();

        mostrarSequenciaGerada();

        lerSequenciaJogada();
        
        delay_ms(1000);

        i++;
    }
}

//-----------------------------------------------------
//*****************************************************

void delay_ms(unsigned int tempo)
{
    unsigned int j;
    for(j=0; j<tempo; j++)
    {
        Delay1KTCYx(5);
    }
}

void delay_us (unsigned int tempo)
{
    unsigned int j;
    for (j=0; j< tempo; j++)
	{
            Delay1TCY(); //atrasa 0.2us
            Delay1TCY(); //atrasa 0.2us
            Delay1TCY(); //atrasa 0.2us
            Delay1TCY(); //atrasa 0.2us
            Delay1TCY(); //atrasa 0.2us
	}
}

void escrevelcd (unsigned char valor, char valor_rs)
{
    RS = valor_rs; // pino ligado ao e0 em zero - envio de comando
    RW = 0; // pino ligado ao e2 em zero - escrita no display
    PORTD = valor; // comunicação em 8 bits
    ENA = 1;
    delay_us(1);
    ENA = 0;
    delay_us(40);
}

void inicializarpic (void)
{
    DDRB=0xFF;                 //PINO DOS BOTOES DO PORT B COMO ENTRADA
    DDRAbits.RA0=0;         //PINO DO YELLOW COMO SAIDA
    DDRAbits.RA1=0;         //PINO DO GREEN COMO  SAIDA
    DDRAbits.RA2=0;         //PINO DO RED COMO    SAIDA
    DDRAbits.RA3=0;         //PINO DO BLUE COMO   SAIDA
    DDRC=0x00;              //PORT C COMO SAIDA
    DDRD=0x00;              //PORT D COMO SAIDA
    DDRE=0x00;              //PORT E COMO SAIDA
    ADCON0bits.ADON=0;      //CONVERSOR AD DESLIGADO
    ADCON1=0xFF;            //TODOS OS PINOS COMO DIGITAIS

    //TIMER2
    T2CONbits.TMR2ON=1; //LIGA O TIMER
    TMR2=0X00;//VALOR INICIAL EM ZERO
    PR2=0XFF;//VALOR FINAL EM 255
}


void inicializarlcd (void)
{
    escrevelcd (0x30,0);
    delay_us(40);
    escrevelcd (0x38,0);
    delay_us(40);
    escrevelcd (0x0F,0);
    delay_us(40);
    escrevelcd (0x06,0);
    delay_us(1600);
    escrevelcd (0x01,0);
    delay_ms(2);
}

void iniciarjogo (void)
{
    delay_ms(2);
    escrevelcd ('S', 1);
    escrevelcd ('I', 1);
    escrevelcd ('M', 1);
    escrevelcd ('O', 1);
    escrevelcd ('N', 1);
    
    //SOM INICIO

    while(1)
    {
        if(START == 1)
        {
            escrevelcd (0x01, 0);
            delay_ms(2);
            escrevelcd ('C', 1); escrevelcd ('L', 1);
            escrevelcd ('A', 1); escrevelcd ('S', 1);
            escrevelcd ('S', 1); escrevelcd ('I', 1);
            escrevelcd ('C', 1);
            delay_ms(3000);
            break;
        }
    }
}

int geravalor()
{
    int valor;
    srand(TMR2);
    valor = (rand() % 4);

    if(valor==0)      {valor=1;}
    else if(valor==1) {valor=2;}
    else if(valor==2) {valor=4;}
    else if(valor==3) {valor=8;}

    return valor;
}

void mostrarnivellcd(void)
{
    escrevelcd(0x01, 0);
    delay_ms(2);
    escrevelcd('L', 1); escrevelcd('E', 1);
    escrevelcd('V', 1); escrevelcd('E', 1);
    escrevelcd('L', 1); escrevelcd(' ', 1);
    escrevelcd(((i+1)/10)+48, 1);
    escrevelcd(((i+1)%10)+48, 1);
}

void mostrarSequenciaGerada (void)
{
    unsigned int j=0;
    for(j=0; j<=i; j++)
    {
        if(sequenciaGerada[j]==1)
            acende_yellow();
        else if(sequenciaGerada[j]==2)
            acende_green();
        else if(sequenciaGerada[j]==4)
            acende_red();
        else if(sequenciaGerada[j]==8)
            acende_blue();
    }
    n=0;
}

void lerSequenciaJogada(void)
{
    while(1)
        {
            if(YELLOW_IN == 0) {YELLOW_flag=1;}
            if(GREEN_IN == 0) {GREEN_flag=1;}
            if(RED_IN == 0) {RED_flag=1;}
            if(BLUE_IN == 0) {BLUE_flag=1;}

            if((YELLOW_IN==1 && GREEN_IN==1) || (YELLOW_IN==1 && RED_IN==1) || (YELLOW_IN==1 && BLUE_IN==1)
                    || (GREEN_IN==1 && RED_IN==1) || (GREEN_IN==1 && BLUE_IN==1) || (RED_IN==1 && BLUE_IN==1))
                gameover();

            if(YELLOW_IN==1 && YELLOW_flag==1)
            {
                YELLOW_flag=0;
                sequenciaJogada[n]=1;
                if (sequenciaGerada[n] != sequenciaJogada[n])
                    gameover();
                acende_yellow();
                if (n==i)
                {
                    while(1)
                    {
                        if(YELLOW_IN==0)
                            break;
                    }
                    break;
                }
                n++;
            }

            else if(GREEN_IN==1 && GREEN_flag==1)
            {
                GREEN_flag=0;
                sequenciaJogada[n]=2;
                if (sequenciaGerada[n] != sequenciaJogada[n])
                    gameover();
                acende_green();
                if (n==i)
                {
                    while(1)
                    {
                        if(GREEN_IN==0)
                            break;
                    }
                    break;
                }
                n++;
            }

            else if(RED_IN==1 && RED_flag==1)
            {
                RED_flag=0;
                sequenciaJogada[n]=4;
                if (sequenciaGerada[n] != sequenciaJogada[n])
                    gameover();
                acende_red();
                if (n==i)
                {
                    while(1)
                    {
                        if(RED_IN==0)
                            break;
                    }
                    break;
                }
                n++;
            }

            else if(BLUE_IN==1 && BLUE_flag==1)
            {
                BLUE_flag=0;
                sequenciaJogada[n]=8;
                if (sequenciaGerada[n] != sequenciaJogada[n])
                    gameover();
                acende_blue();
                if (n==i)
                {
                    while(1)
                    {
                        if(BLUE_IN==0)
                            break;
                    }
                    break;
                }
                n++;
            }
        }
}

void gameover(void)
{
    unsigned int j;
    escrevelcd(0x01, 0); delay_ms(2);
    escrevelcd('G', 1); escrevelcd('A', 1);
    escrevelcd('M', 1); escrevelcd('E', 1);
    escrevelcd(' ', 1);
    escrevelcd('O', 1); escrevelcd('V', 1);
    escrevelcd('E', 1); escrevelcd('R', 1);

    //SOM GAME OVER


    for(j=0; j<5; j++)
    {
        YELLOW_OUT=1; GREEN_OUT=1; RED_OUT=1; BLUE_OUT=1; delay_ms(500);
        YELLOW_OUT=0; GREEN_OUT=0; RED_OUT=0; BLUE_OUT=0; delay_ms(500);
    }
    
    i=0;
    n=0;
    main();
}

void acende_yellow(void)
{
    SAIDA = 1;
    som(5,1000);
    SAIDA = 0;
    delay_ms(100);
}

void acende_green(void)
{
    SAIDA = 2;
    som(10,1000);
    SAIDA = 0;
    delay_ms(100);
}

void acende_red(void)
{
    SAIDA = 4;
    som(15,1000);
    SAIDA = 0;
    delay_ms(100);
}

void acende_blue(void)
{
    SAIDA = 8;
    som(20,1000);
    SAIDA = 0;
    delay_ms(100);
}

void som(unsigned float periodoms, unsigned float tempoms)
{
    unsigned float i;
    for(i=0; i<(tempoms/periodoms); i++)
    {
        PORTCbits.RC2=1;
        delay_ms(periodoms/2);
        PORTCbits.RC2=0;
        delay_ms(periodoms/2);
    }
}