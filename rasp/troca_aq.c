#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

int i2c_fd;
void ctrl_c(int sig)
{
  close(i2c_fd);
  exit(-1);
}

int main(void)
{
  unsigned char user_input=1,input1, msp430_ret,msp430_ret1, slave_addr=0x0F;
  signal(SIGINT, ctrl_c);
  i2c_fd = open("/dev/i2c-1", O_RDWR);
  ioctl(i2c_fd, I2C_SLAVE, slave_addr);

  while(user_input!=0)
  {
    puts("Digite:\n");
    puts("1-para dados da temperatura\n");
    puts("2 - para dados de pH \n");
    puts("3 - Para nivel d'agua\n");
    puts("4 - lara luminosidade do ambiete\n");
    puts("5 - para ativar rele1\n");
    puts("6 - para ativar rele2\n");
    puts("7 - para ativar rele3\n");
    puts("8 - para ativar rele4\n");
    puts("8 - para ativar rele4\n");
    scanf("%d", &user_input);
    
    if((user_input<0) || (user_input>30))
      puts("Valor invalido");
    else if(user_input>0)
    {
      write(i2c_fd, &user_input, 1);
      sleep(1.5);
      read(i2c_fd, &msp430_ret, 1);
      float msp_ret;
      switch(user_input)
       {
            case 1:
              msp_ret = ((float) msp430_ret);
              printf("o valor da luminosidade num range de 0 a 140 é é: %.2f", msp_ret);
            break;
            case 2:
              msp_ret = (((float) msp430_ret)/10);
              printf("o valor do pH é: %.2f", msp_ret);
            break;
            case 3:
               msp_ret = ((float) msp430_ret);
               printf("A distancia da coluna de água é: %.2f", msp_ret);
            break;
            case 4:
               msp_ret = ((float) msp430_ret);
               printf("A temperatura da aqua é: %.2f", msp_ret);
            break;
            case 9:
             
               msp_ret = ((float) msp430_ret);
               if(msp_ret==10){
                   input1=3;
                   write(i2c_fd,&input1, 1);
                   sleep(1.5);
                   read(i2c_fd,&msp430_ret1,1);
                   
                   //lê ultrassônico
                   while(msp430_ret1<20)
                   {
                      write(i2c_fd,&input1, 1);
                      sleep(1.5);
                      read(i2c_fd,&msp430_ret1,1);
                      sleep(1.5);
                   }
                   input1=10;      
                   write(i2c_fd,&input1, 1);
                   sleep(1.5);

                    //le novamente ultrassônico
                   input1=3;
                   write(i2c_fd,&input1, 1);
                   sleep(1.5);
                   read(i2c_fd,&msp430_ret1,1);
                   
                   while(msp430_ret1>8)
                   {
                      write(i2c_fd,&input1, 1);
                      sleep(1.5);
                      read(i2c_fd,&msp430_ret1,1);
                      sleep(1.5);
                   }
                   
                   input1=11;
                   write(i2c_fd,&input1, 1);
                   sleep(1.5);               
                  }         
            break;
       }}
    puts("");
  }
  close(i2c_fd);
}
