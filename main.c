// U0Rx connected to PA0
// U0Tx connected to PA1

#include <inttypes.h>
#include <tm4c123gh6pm.h>

#define UART_FR_TXFF            0x00000020          // UART Transmit FIFO Full

uint32_t leituraAtual;

void UART0_Init(void){

    SYSCTL_RCGCUART_R |= 0x01;                        // activate UART0
    SYSCTL_RCGCGPIO_R |= 0x01;                        // activate port A
    GPIO_PORTA_AFSEL_R |= 0x03;                       // enable alt funct on PA1-0
    GPIO_PORTA_PCTL_R |= 0x11;                        // enable UART funct on PA1-0

    UART0_CTL_R = 0x00;                               // disable UART
    UART0_IBRD_R = 8;                                 // IBRD = int(16.000.000 / (16 * 115.200)) = int(8,6805)
    UART0_FBRD_R = 44;                                // FBRD = int(0,6805 * 64 + 0.5) = 44
    UART0_LCRH_R = 0x70;                              // 8 bit word length (no parity bits, one stop bit, FIFOs)
    UART0_CC_R = 0x00;
    UART0_CTL_R = 0x301;                              // enable UART

    GPIO_PORTA_DEN_R |= 0x03;                         // enable digital I/O on PA1-0

}

void UART0_OutChar(unsigned char data){
    while ((UART0_FR_R & UART_FR_TXFF) != 0) { }
    UART0_DR_R = data;
}

void VerificaUart()
{

    if ((GPIO_PORTF_RIS_R & 0x10) == 0x10) //Verificação de qual pino gerou a inten
    {
        GPIO_PORTF_ICR_R = 0x10; //Limpa flag de int do pino que gerou a inten
        UART0_OutChar(35);
        while ((GPIO_PORTF_DATA_R & 0x10) == 0x00) { }
        GPIO_PORTF_DATA_R ^= 0x0E;
    }
}

void main()
{

    SYSCTL_RCGCGPIO_R = 0x28;
    GPIO_PORTF_DIR_R = 0x0E;
    GPIO_PORTF_PUR_R = 0x10;
    GPIO_PORTF_DATA_R = 0x00;
    GPIO_PORTF_DEN_R = 0x1E;

    //ADC no PortD

    SYSCTL_RCGCADC_R = 0x01;
    GPIO_PORTD_AFSEL_R = 0x01;
    GPIO_PORTD_AMSEL_R = 0x01;

    ADC0_ACTSS_R = 0x00;
    ADC0_EMUX_R = 0x0F;
    ADC0_SSMUX0_R = 0x07;
    ADC0_SSCTL0_R = 0x02;
    ADC0_ACTSS_R = 0x01;
    ADC0_PSSI_R = 0x01;

    //Interrupcao

    GPIO_PORTF_IS_R = 0x00; //Define sensibilidade do botão - 0 borda - 1 sinal continuo
    GPIO_PORTF_IEV_R = 0x00; //Define evento - 0 descida | baixo - 1 subida | alto
    GPIO_PORTF_IBE_R = 0x00; //Define inten por ambas as bordas
    GPIO_PORTF_IM_R = 0x10; //Habilita inten no pino
    NVIC_EN0_R = 1 << 30; //habilita inten do portF

    UART0_Init();

    while(1){leituraAtual = ADC0_SSFIFO0_R;}
}
