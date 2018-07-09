/* PINAGEM
 *
 * UART0 RX : PA0
 * UART0 TX : PA1
 *
 * Analógico Esq. Y : AIN7 - PD0 [ADC0]
 * Analógico Esq. X : AIN8 - PE5 [ADC0]
 * Analógico Dir. Y : AIN11 - PB5 [ADC1]
 * Analógico Dir. X : AIN6 - PD1 [ADC1]
 *
 * Digital 1  (UP)    : PE4
 * Digital 2  (DOWN)  : PA5
 * Digital 3  (LEFT)  : PB2
 * Digital 4  (RIGHT) : PE0
 * Digital 5  (A)     : PA4
 * Digital 6  (B)     : PA3
 * Digital 7  (C)     : PA2
 * Digital 8  (D)     : PB3
 * Digital 9  (E)     : PD6
 * Digital 10 (F)     : PD7
 * Digital 11 (Start) : PF4
 *
 * PORTS ATIVOS
 * A B D E F
 */

#include <inttypes.h>
#include <tm4c123gh6pm.h>

#define UART_FR_TXFF 0x00000020 // UART Transmit FIFO Full
uint32_t tempo = 200000;
int i;

volatile uint32_t AnalogEsq[] = {0, 0}; // X, Y
volatile uint32_t AnalogDir[] = {0, 0};

InitSysTick(){

    NVIC_ST_RELOAD_R = tempo;
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_INTEN | NVIC_ST_CTRL_ENABLE;
}

void InitDigitals(){
    // PortA
    GPIO_PORTA_DIR_R = 0x00; // Todos são inputs
    GPIO_PORTA_DATA_R = 0x00;
    GPIO_PORTA_IS_R = 0x00; // Define sensibilidade do botão - 0 borda - 1 sinal contínuo
    GPIO_PORTA_IEV_R = 0x00; // Define evento - [0 descida/baixo] ou [1 subida/alto]
    GPIO_PORTA_IBE_R = 0x00; // Desabilita interrupt por ambas as bordas
    GPIO_PORTA_DEN_R |= 0x3C; // Habilita {2, 3, 4, 5}
    GPIO_PORTA_IM_R = 0x3C; // Habilita interrupt no pino
    GPIO_PORTA_PUR_R = 0x3C;
    NVIC_EN0_R = 1 << 0; // Habilita interrupt no PortA

    // PortB
    GPIO_PORTB_DIR_R = 0x00;
    GPIO_PORTB_DATA_R = 0x00;
    GPIO_PORTB_IS_R = 0x00;
    GPIO_PORTB_IEV_R = 0x00;
    GPIO_PORTB_IBE_R = 0x00;
    GPIO_PORTB_DEN_R |= 0x0C; // Habilita {2, 3}
    GPIO_PORTB_IM_R = 0x0C;
    GPIO_PORTB_PUR_R = 0x0C;
    NVIC_EN0_R = 1 << 1;

    // PortD
    GPIO_PORTD_DIR_R = 0x00;
    GPIO_PORTD_DATA_R = 0x00;
    GPIO_PORTD_IS_R = 0x00;
    GPIO_PORTD_IEV_R = 0x00;
    GPIO_PORTD_IBE_R = 0x00;
    GPIO_PORTD_DEN_R |= 0xC0; // Habilita {6, 7}
    GPIO_PORTD_IM_R = 0xC0;
    GPIO_PORTD_PUR_R = 0xC0;
    NVIC_EN0_R = 1 << 3;

    // PortE
    GPIO_PORTE_DIR_R = 0x00;
    GPIO_PORTE_DATA_R = 0x00;
    GPIO_PORTE_IS_R = 0x00;
    GPIO_PORTE_IEV_R = 0x00;
    GPIO_PORTE_IBE_R = 0x00;
    GPIO_PORTE_DEN_R |= 0x11; // Habilita {0, 4}
    GPIO_PORTE_IM_R = 0x11;
    GPIO_PORTE_PUR_R = 0x11;
    NVIC_EN0_R = 1 << 4;

    // PortF
    GPIO_PORTF_DIR_R = 0x00;
    GPIO_PORTF_DATA_R = 0x00;
    GPIO_PORTF_IS_R = 0x00;
    GPIO_PORTF_IEV_R = 0x00;
    GPIO_PORTF_IBE_R = 0x00;
    GPIO_PORTF_DEN_R |= 0x10; // Habilita {4}
    GPIO_PORTF_IM_R = 0x10;
    GPIO_PORTF_PUR_R = 0x10;
    NVIC_EN0_R = 1 << 30;
}

void UART0_Init(){
    SYSCTL_RCGCUART_R |= 0x01; // Ativa a UART0
    SYSCTL_RCGCGPIO_R |= 0x01;
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_PCTL_R |= 0x11; // Ativa a função UART em PA {0, 1}

    UART0_CTL_R = 0x00; // Desativa a UART
    UART0_IBRD_R = 8; // IBRD = int(16.000.000 / (16 * 115.200)) = int(8,6805)
    UART0_FBRD_R = 44; // FBRD = int(0,6805 * 64 + 0.5) = 44
    UART0_LCRH_R = 0x70; // Palavra de 8 bit (Sem bit de paridade, um bit de stop, FIFOs)
    UART0_CC_R = 0x00;
    UART0_CTL_R = 0x301; // Ativa a UART

    GPIO_PORTA_DEN_R |= 0x03; // Ativa digitalmente o I/O em PA {0, 1}
}

void UART0_OutChar(unsigned char data){
    while((UART0_FR_R & UART_FR_TXFF) != 0);
    UART0_DR_R = data;
}

void InterruptPortA(){

    for(i = 0;i<100;i++);
    if ((GPIO_PORTA_RIS_R & 0x04) == 0x04){ // Verificação de qual pino gerou a interrupt
        GPIO_PORTA_ICR_R = 0x04; // Limpa flag de int do pino que gerou a interrupt
        UART0_OutChar(67); // C
    }
    for(i = 0;i<100;i++);
    if ((GPIO_PORTA_RIS_R & 0x08) == 0x08){
        GPIO_PORTA_ICR_R = 0x08;
        UART0_OutChar(66); // B
    }
    for(i = 0;i<100;i++);
    if ((GPIO_PORTA_RIS_R & 0x10) == 0x10){
        GPIO_PORTA_ICR_R = 0x10;
        UART0_OutChar(65); // A
    }
    for(i = 0;i<100;i++);
    if ((GPIO_PORTA_RIS_R & 0x20) == 0x20){
        GPIO_PORTA_ICR_R = 0x20;
        UART0_OutChar(86); // DOWN
    }
}
void InterruptPortB(){

    for(i = 0;i<100;i++);
    if ((GPIO_PORTB_RIS_R & 0x04) == 0x04){
        GPIO_PORTB_ICR_R = 0x04;
        UART0_OutChar(60); // LEFT
    }
    for(i = 0;i<100;i++);
    if ((GPIO_PORTB_RIS_R & 0x08) == 0x08){
        GPIO_PORTB_ICR_R = 0x08;
        UART0_OutChar(68); // D
    }
}
void InterruptPortD(){
    for(i = 0;i<100;i++);
    if ((GPIO_PORTD_RIS_R & 0x40) == 0x40){
        GPIO_PORTD_ICR_R = 0x40;
        UART0_OutChar(69); //E
    }
    for(i = 0;i<100;i++);
    if ((GPIO_PORTB_RIS_R & 0x80) == 0x80){
        GPIO_PORTB_ICR_R = 0x80;
        UART0_OutChar(70); // F
    }
    for(i = 0;i<100;i++);
    GPIO_PORTD_ICR_R = 0x40;
    GPIO_PORTF_DATA_R ^= 0x0A;
}
void InterruptPortE(){
    for(i = 0;i<100;i++);
    if ((GPIO_PORTE_RIS_R & 0x01) == 0x01){
        GPIO_PORTE_ICR_R = 0x01;
        UART0_OutChar(62); // RIGHT
    }
    for(i = 0;i<100;i++);
    if ((GPIO_PORTE_RIS_R & 0x10) == 0x10){
        GPIO_PORTE_ICR_R = 0x10;
        UART0_OutChar(94); // UP
    }
}
void InterruptPortF(){
    for(i = 0;i<100;i++);
    if ((GPIO_PORTF_RIS_R & 0x10) == 0x10){
        GPIO_PORTF_ICR_R = 0x10;
        UART0_OutChar(83); // START (S)
    }
}

void InitADC(){
    // Habilita ADC0 e ADC1
    SYSCTL_RCGCADC_R = 0x03;

    // Configura pinos
    GPIO_PORTB_AFSEL_R = 0x20;
    GPIO_PORTB_AMSEL_R = 0x20;

    GPIO_PORTD_AFSEL_R = 0x03;
    GPIO_PORTD_AMSEL_R = 0x03;

    GPIO_PORTE_AFSEL_R = 0x20;
    GPIO_PORTE_AMSEL_R = 0x20;


    // ADC0 - Analógico Esq.
    ADC0_ACTSS_R = 0x00;
    ADC0_EMUX_R = 0x00;
    ADC0_SSMUX0_R = 0x07; // Y
    ADC0_SSCTL0_R = 0x06; // Habilita interrupt + 1 sample

    ADC0_SSMUX1_R = 0x08; // X
    ADC0_SSCTL1_R = 0x06;

    ADC0_ACTSS_R = 0x03;


    // ADC1 - Analógico Dir.
    ADC1_ACTSS_R = 0x00;
    ADC1_EMUX_R = 0x00;
    ADC1_SSMUX0_R = 0x0B; // Y
    ADC1_SSCTL0_R = 0x06;

    ADC1_SSMUX1_R = 0x06; // X
    ADC1_SSCTL1_R = 0x06;

    ADC1_ACTSS_R = 0x03;
}

void trataSystick()
{
    if(AnalogEsq[0] > 2200)
        UART0_OutChar(94);
    if(AnalogEsq[0] < 1800)
        UART0_OutChar(86);
    if(AnalogDir[1] < 1800)
        UART0_OutChar(60);
    if(AnalogDir[1] > 2200)
        UART0_OutChar(62);
    if(AnalogEsq[1] > 2200)
        UART0_OutChar(49);
    if(AnalogEsq[1] < 1800)
        UART0_OutChar(50);
    if(AnalogDir[0] < 1800)
        UART0_OutChar(51);
    if(AnalogDir[0] > 2200)
        UART0_OutChar(52);

}

void main(){

    SYSCTL_RCGCGPIO_R = 0x3B; // Habilita os Ports {A, B, D, E, F}

    InitSysTick();
    InitDigitals();
    InitADC();
    UART0_Init();

    while(1){
        ADC0_PSSI_R = 0x03;
        while((ADC0_RIS_R & 0x01) == 0); // Espera a conversão terminar
        AnalogEsq[1] = ADC0_SSFIFO0_R; // Lê resultado
        ADC0_ISC_R = 0x01; // Limpa flag de interrupt de conclusão da conversão

        while((ADC0_RIS_R & 0x02) == 0);
        AnalogEsq[0] = ADC0_SSFIFO1_R;
        ADC0_ISC_R = 0x02;

        ADC1_PSSI_R = 0x03;
        while((ADC1_RIS_R & 0x01) == 0);
        AnalogDir[1] = ADC1_SSFIFO0_R;
        ADC1_ISC_R = 0x01;

        while((ADC1_RIS_R & 0x02) == 0);
        AnalogDir[0] = ADC1_SSFIFO1_R;
        ADC1_ISC_R = 0x02;
    }
}
