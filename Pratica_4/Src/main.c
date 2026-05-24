#include "stm32f1xx.h"

#define FREQ_CPU 8000000

volatile uint32_t msTicks = 0;
volatile uint8_t estado_pisca = 0;
volatile uint8_t cor_atual = 1;

void SysTick_Handler(void) {
    msTicks++;
}

void aplicar_cor(uint8_t cor, uint8_t ligar) {
    GPIOA->BSRR = (GPIO_BSRR_BS4 | GPIO_BSRR_BS5 | GPIO_BSRR_BS6);

    if (ligar) {
        switch (cor) {
            case 1: GPIOA->BRR = GPIO_BRR_BR4; break;
            case 2: GPIOA->BRR = GPIO_BRR_BR5; break;
            case 3: GPIOA->BRR = GPIO_BRR_BR6; break;
            case 4: GPIOA->BRR = (GPIO_BRR_BR4 | GPIO_BRR_BR5); break;
            case 5: GPIOA->BRR = (GPIO_BRR_BR5 | GPIO_BRR_BR6); break;
            case 6: GPIOA->BRR = (GPIO_BRR_BR4 | GPIO_BRR_BR6); break;
            case 7: GPIOA->BRR = (GPIO_BRR_BR4 | GPIO_BRR_BR5 | GPIO_BRR_BR6); break;
        }
    }
}

int main(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4 |
                     GPIO_CRL_MODE5 | GPIO_CRL_CNF5 |
                     GPIO_CRL_MODE6 | GPIO_CRL_CNF6);

    GPIOA->CRL |= (GPIO_CRL_MODE4_1 |
                   GPIO_CRL_MODE5_1 |
                   GPIO_CRL_MODE6_1);

    GPIOA->ODR |= (GPIO_ODR_ODR4 |
                   GPIO_ODR_ODR5 |
                   GPIO_ODR_ODR6);

    SysTick_Config(FREQ_CPU / 1000);

    while (1) {
        if (msTicks >= 500) {
            msTicks = 0;
            estado_pisca++;

            if (estado_pisca == 1 || estado_pisca == 3) {
                aplicar_cor(cor_atual, 0);
            } else if (estado_pisca == 2) {
                aplicar_cor(cor_atual, 1);
            } else if (estado_pisca >= 4) {
                estado_pisca = 0;
                cor_atual++;

                if (cor_atual > 7)
                    cor_atual = 1;

                aplicar_cor(cor_atual, 1);
            }
        }
    }
}
