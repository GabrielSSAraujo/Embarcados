#include <msp430g2553.h>
#include <legacymsp430.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define LDR BIT1
#define IN_AD_CH INCH_1
#define pH BIT0
#define SCL BIT6
#define SDA BIT7
#define DLY 0x6000
//rele
#define REL1  BIT0
#define REL2  BIT1
#define REL3  BIT2
#define REL4  BIT3
#define REL (REL1|REL2|REL3|REL4)

//ultrasonico
#define TRIG BIT3
#define ECHO BIT4
int miliseconds;
int distance;
long sensor;

unsigned int valor[2];

void init_AD(void){
    ADC10CTL1 = INCH_1 + CONSEQ_1;                                       //sequencia simples
    ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + ADC10IE;      
    ADC10AE0 = pH + LDR;                                                //pinos selecionados
    ADC10DTC1 = 0x2;                                                     //2 conversões
}

void init_I2C(volatile unsigned int slave_address){
    UCB0CTL1 |= UCSWRST;                       // Enable SW reset
    UCB0CTL1 &= ~UCTR;
    UCB0CTL0 = UCMODE_3 + UCSYNC;              // I2C Master, modo sincrono
    UCB0I2COA = slave_address;                 //|UCGCEN;
    P1SEL |= SCL | SDA;                        // Assign I2C pins to USCI_B0
    P1SEL2|= SCL | SDA;                        // Assign I2C pins to USCI_B0
    UCB0STAT &= ~(UCSTPIFG | UCSTTIFG);
    UCB0CTL1 &= ~UCSWRST;                      // Limpa SW reset
    IE2 |= UCB0RXIE;                           //Ativa interrupção em RX e TX                                                                    //UCB0I2CIE |= UCSTPIE | UCSTTIE;
}

void Transmit(char data[], int len){
    volatile unsigned int i;
    for(i=0; i<len; i++)
    {
        while((IFG2 & UCB0TXIFG)==0);
        UCB0TXBUF = data[i];
    }
}

int leitor_AD(){
    ADC10CTL0 &= ~ENC;
    while (ADC10CTL1 & BUSY);
    ADC10SA = (unsigned int)valor;
    ADC10CTL0 |= ENC + ADC10SC;              //inicia a conversão AD
    __bis_SR_register(CPUOFF + GIE);
  }

int leitor_Ultra(){
     P1OUT |= BIT3;                          // genera pulso
     __delay_cycles(10);                     // delay de 10us
     P1OUT &= ~BIT3;                         // fim do pulso
     P1IES &= ~BIT4;                         // rising edge no pino ECHO
     P1IFG = 0x00;                           // limpa flag de interrupcao
     __delay_cycles(30000);                  // delay de 30ms (LDRo para a onda voltar no echo)
     distance = sensor/58;                   // converter a distancia para cm (ECHO)
     return(distance);
}

int ativa_rele(int rel){
    switch(rel)
    {
        case 5:
            P2OUT ^=REL1;
        break;
        case 6:
            P2OUT ^=REL2;
        break;
        case 7:
            P2OUT ^=REL3;
        break;
        case 8:
            P2OUT ^=REL4;
        break;
    }
}  

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    //ultra
    CCTL0 = CCIE;                             // CCR0 desabilita a interrupçao
    CCR0 = 1000;                              // 1ms - 1mhz
    TACTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
    P1IFG  = 0x00;                            // limpa as flags de interrupcoes
    P1IE |= ECHO;                             // habilita interupt no pino ECHO
    P1DIR &= ~ECHO;                           // make pin P1.2 entrada (ECHO)
    P1DIR |= TRIG;                            // trigger pin como saida
    //rele
    P2OUT |=(REL2|REL3|REL4);
    P2OUT &=~REL1;
    P2DIR |= REL;
    //Inicializando i2C e ADC
    init_I2C(0xF);
    init_AD();
    
    
    _BIS_SR(LPM0_bits + GIE);
    return 0;
}

interrupt(USCIAB0TX_VECTOR) USCIAB0TX_ISR(void){
    int ret, d, ultra;
    char d_send[2];
    
    if(IFG2 & UCB0RXIFG)
    {
        d = UCB0RXBUF;
        
        ativa_rele(d);       
        leitor_AD();
        ultra = leitor_Ultra();
     
        if(d==1){
            ret = valor[d-1]/7.307;
        }
        else if(d==2){
            ret = valor[d-1]/34.1;
        }
        else if(d==3){
            ret = ultra;
        }
        else if(d==9){
          //desliga Filtro1-rele1
           ativa_rele(5); 
          //Fecah valvula 1-RELE2
           ativa_rele(6);
          //LIga bomba de descarte- rel 4
           ativa_rele(8);
           ret=10;
        }    
        else if(d==10)
        {
          //fecha desliga bomba descarte - rel4
          ativa_rele(8);
          //abre Val2 - Rele3
          ativa_rele(7);
          ativa_rele(5);
        }
        else if(d==11)
        {
          ativa_rele(5);
          __delay_cycles(200000);
          ativa_rele(7);
          __delay_cycles(200000);
          ativa_rele(6);
          __delay_cycles(200000);
          ativa_rele(5); 
        }
        
        while( (UCB0STAT & UCSTTIFG)==0);
        d_send[0] = ret & 0xff;
        d_send[1] = ret>>8;
        Transmit(d_send, 1);
        UCB0STAT &= ~(UCSTPIFG | UCSTTIFG);
    } 
    IFG2 &= ~(UCB0TXIFG|UCB0RXIFG);
}
//interrupção do conversor AD
#pragma vector=ADC10_VECTOR
    __interrupt void ADC10_ISR(void)
    {
        __bic_SR_register_on_exit(CPUOFF);            // Clear CPUOFF bit from 0(SR)
    }

// interrupção sensor ultrassônico ele usa interrupção do timerA
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if(P1IFG&ECHO)                                   //se houver interrupcap
    {
        if(!(P1IES&ECHO))                            //rising edge?
        {
            TACTL|=TACLR;                            //limpa o timer A
            miliseconds = 0;
            P1IES |= ECHO;                           //borda de descida
        }
        else
        {
            sensor = (long)miliseconds*1000 + (long)TAR;    //calcula distancia ECHO
        }
        P1IFG &= ~ECHO;                             //limpa a flag
    }
}
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
 miliseconds++;
}
