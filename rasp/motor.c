#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>

#define in1 0     // pino fisico 11
#define in2 2     // pino fisico 13
#define in3 3     // pino fisico 15
#define in4 4     // pino fisico 16
int var1,var2,var3,var4;
// Velocidade do Motor
int velocMotor = 1048;    
//Variavel de pulsos
int cont = 0;     
// Numero de passos nescessaria para dar uma volta completa
int passosPorRotacao = 512;
//Definiçao da sequencia de ativação das bobinas do motor
int Sequencia[8] = {0x08,0x0C,0x04,0x06,0x02,0x03,0x01,0x09};
void sentidoHorario();
void setOutput(int out);
void sentidoAntiHorario();


int main(){
  
  wiringPiSetup();
  
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  
  while(1){
     if(cont < passosPorRotacao)
     sentidoHorario(); 
     else if (cont == passosPorRotacao * 2)
     cont = 0;
     
     else
     sentidoAntiHorario(); 
     cont++;
    }
    return 0;
}


void sentidoHorario(){
      for(int i=7;i >=0;i--)
      {
        setOutput(i);
        usleep(velocMotor);
      }
 }
  
void sentidoAntiHorario()
{
 for(int i=0;i<8;i++)
 {
 setOutput(i);
 usleep(velocMotor);
 }
}

void setOutput(int out) 
{
var1=((Sequencia[out])&0x01);
var2=((Sequencia[out]>>1)&0x01);
var3=((Sequencia[out]>>2)&0x01);
var4=((Sequencia[out]>>3)&0x01);

 digitalWrite(in1,var1);
 digitalWrite(in2, var2);
 digitalWrite(in3, var3);
 digitalWrite(in4, var4);
}
