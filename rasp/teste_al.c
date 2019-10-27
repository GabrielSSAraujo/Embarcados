#include <stdio.h>
#include <wiringPi.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//Motor principal
#define in1 0     // pino fisico 11
#define in2 2     // pino fisico 13
#define in3 3     // pino fisico 15
#define in4 4     // pino fisico 16

//motor dispensa fuso1
#define fs11 21     // pino fisico 11
#define fs12 22     // pino fisico 13
#define fs13 23     // pino fisico 15
#define fs14 24     // pino fisico 16

//motor dispensa fuso2
#define fs21 26     // pino fisico 11
#define fs22 27     // pino fisico 13
#define fs23 28     // pino fisico 15
#define fs24 29     // pino fisico 16

#define FDC1 6
#define FDC2 25


// Velocidade do Motor(max)
int velocMotor = 1048;    
//Variavel de pulsos
int conta = 0,number_aqua,number_tip1,number_tip2=0;     
// Numero de passos nescessaria para dar uma volta completa
int passosPorRotacao = 512;
//Definiçao da sequencia de ativação das bobinas do motor
int Sequencia[8]={0x08,0x0C,0x04,0x06,0x02,0x03,0x01,0x09};

void* alimentar();
void sentidoHorario();
void setOutput(int out);
void setOutputFuso1(int out);
void setOutputFuso2(int out);
void sentidoAntiHorario();
void gira_fuso1();
void gira_fuso2();
void* fuso();

int main(){


  wiringPiSetup();
  pthread_t thread_1;
  pthread_t thread_2;

  pinMode(FDC1,INPUT);
  pinMode(FDC2,INPUT);
  pullUpDnControl(FDC1,PUD_DOWN);
  pullUpDnControl(FDC2,PUD_DOWN);
 
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  
  pinMode(fs11,OUTPUT);
  pinMode(fs12,OUTPUT);
  pinMode(fs13,OUTPUT);
  pinMode(fs14,OUTPUT);
  
  pinMode(fs21,OUTPUT);
  pinMode(fs22,OUTPUT);
  pinMode(fs23,OUTPUT);
  pinMode(fs24,OUTPUT);
  

  puts("Qual aquario alimentar 1, 2 ou os dois(3)?\n");
  scanf("%d", &number_aqua);
  puts("Qual ração 1, 2 do aquario 1?\n");
  scanf("%d", &number_tip1);
 if(number_aqua==3){
  puts("Qual ração 1, 2 do aquario 2?\n");
  scanf("%d", &number_tip2);
  }

  //abre processos para motores funcionarem simultaneamente
  pthread_create(&thread_1,NULL,&alimentar,NULL);
  sleep(1);
  pthread_create(&thread_2,NULL,&fuso,NULL);
  
  pthread_join (thread_1, NULL);
 // pthread_join (thread_2, NULL);
  printf("aquario %d alimentado\n A(s) rações fornecida(s) %d", number_aqua,number_tip1);    
  
 return 0;
}

void* fuso()
{
      if(number_tip1==1 && number_tip2==0){
        if(number_aqua==1 || number_aqua==2){
    	  while(conta>0 && conta<=2*passosPorRotacao);
	  while(conta>=2*passosPorRotacao && conta<=4*passosPorRotacao)
          {
            gira_fuso1();
          }
       }}
      else if(number_tip1==2 && number_tip2==0){
            if(number_aqua==1 || number_aqua==2){
       	        while(conta>0 && conta<=2*passosPorRotacao);
                while(conta>=2*passosPorRotacao && conta<=4*passosPorRotacao)
                {
                    gira_fuso2();
                }
            }
      }
      else if(number_aqua==3)
      {
    	  while(conta>0 && conta<=2*passosPorRotacao);
	  while(conta>=2*passosPorRotacao && conta<=4*passosPorRotacao)
	  {
		if(number_tip1==1){gira_fuso1();}
		if(number_tip1==2){gira_fuso2();}
	  }
	//espera motor voltar p/ posição inicial
	  while(conta!=0);
	  sleep(1);
	//espera passar vão entre aquarios
     	  while(conta>0 && conta<=2*passosPorRotacao);
	//dispensando raçao 1 no aquario 2
	  while(conta>=2*passosPorRotacao && conta<=4*passosPorRotacao)
	  {
		if(number_tip2==1){gira_fuso1();}
		if(number_tip2==2){gira_fuso2();}
	  }
      }
}

void* alimentar()
{
    if(number_aqua==1)
    {
  while(digitalRead(FDC1)!=0)
  {
      sentidoHorario();
      conta++;  
  }
  while(conta>=0)
  {
      sentidoAntiHorario(); 
      conta--;
  }
    }
    
    else if(number_aqua==2)
    {
      while(digitalRead(FDC2)!=0)
  {
      sentidoAntiHorario(); 
      conta++;  
  }
      
      while(conta>=0)
  {
      sentidoHorario(); 
      conta--;
  }
    }
    
    else if(number_aqua==3)
    {
	while(digitalRead(FDC1)!=0)
    	{
      	   sentidoHorario();
           conta++;  
        }
	while(conta>=0){
	   sentidoAntiHorario();
	   conta--;
	}
        while(digitalRead(FDC2)!=0)
        {
           sentidoAntiHorario(); 
           conta++;  
        }
 	 while(conta>=0)
  	{
      	   sentidoHorario(); 
           conta--;
  	}
    }
}
void gira_fuso1(){
      for(int i=7;i >=0;i--)
      {
        setOutputFuso1(i);
        usleep(velocMotor);
      }}
 void gira_fuso2(){
      for(int i=7;i >=0;i--)
      {
        setOutputFuso2(i);
        usleep(velocMotor);
      }}
void sentidoHorario(){
      for(int i=7;i >=0;i--)
      {
        setOutput(i);
        usleep(velocMotor);
      }}
  
void sentidoAntiHorario()
{
 for(int i=0;i<8;i++)
 {
 setOutput(i);
 usleep(velocMotor);
}}

//ativa bobinas do motor principal na sequencia tabelada no datasheet
void setOutput(int out) 
{
  int var1,var2,var3,var4;
  
  var1=((Sequencia[out])&0x01);
  var2=((Sequencia[out]>>1)&0x01);
  var3=((Sequencia[out]>>2)&0x01);
  var4=((Sequencia[out]>>3)&0x01);

  digitalWrite(in1,var1);
  digitalWrite(in2, var2);
  digitalWrite(in3, var3);
  digitalWrite(in4, var4);
}
//ativa bobinas do motor de dispensa 
void setOutputFuso1(int out) 
{
  int var1,var2,var3,var4;
  
  var1=((Sequencia[out])&0x01);
  var2=((Sequencia[out]>>1)&0x01);
  var3=((Sequencia[out]>>2)&0x01);
  var4=((Sequencia[out]>>3)&0x01);

  digitalWrite(fs11, var1);
  digitalWrite(fs12, var2);
  digitalWrite(fs13, var3);
  digitalWrite(fs14, var4);
}
void setOutputFuso2(int out) 
{
  int var1,var2,var3,var4;
  
  var1=((Sequencia[out])&0x01);
  var2=((Sequencia[out]>>1)&0x01);
  var3=((Sequencia[out]>>2)&0x01);
  var4=((Sequencia[out]>>3)&0x01);

  digitalWrite(fs21, var1);
  digitalWrite(fs22, var2);
  digitalWrite(fs23, var3);
  digitalWrite(fs24, var4);
}
