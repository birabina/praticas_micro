#include "stm32f1xx.h"

void delay_debounce(volatile uint32_t count) {
    while(count--) {}
}

int main(void) {
    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN);

    GPIOA->CRL &= ~(0x0FFF0000);
    GPIOA->CRL |=  (0x02220000);

    GPIOA->ODR |= (1 << 4) | (1 << 5) | (1 << 6);

    GPIOA->CRL &= ~(GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
                     GPIO_CRL_MODE2 | GPIO_CRL_CNF2 |
                     GPIO_CRL_MODE7 | GPIO_CRL_CNF7);

    GPIOA->CRL |= (GPIO_CRL_CNF1_1 | GPIO_CRL_CNF2_1 | GPIO_CRL_CNF7_1);

    GPIOA->ODR |= (GPIO_ODR_ODR1 | GPIO_ODR_ODR2 | GPIO_ODR_ODR7);

    AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI1 | AFIO_EXTICR1_EXTI2);
    AFIO->EXTICR[0] |=  (AFIO_EXTICR1_EXTI1_PA | AFIO_EXTICR1_EXTI2_PA);

    AFIO->EXTICR[1] &= ~(AFIO_EXTICR2_EXTI7);
    AFIO->EXTICR[1] |=  (AFIO_EXTICR2_EXTI7_PA);

    EXTI->IMR  |= (1 << 1) | (1 << 2) | (1 << 7);
    EXTI->FTSR |= (1 << 1) | (1 << 2) | (1 << 7);

    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    while(1) {}
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1 << 1)) {
        delay_debounce(200000);
        GPIOA->ODR ^= (1 << 4);
        EXTI->PR |= (1 << 1);
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI->PR & (1 << 2)) {
        delay_debounce(200000);
        GPIOA->ODR ^= (1 << 5);
        EXTI->PR |= (1 << 2);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1 << 7)) {
        delay_debounce(200000);
        GPIOA->ODR ^= (1 << 6);
        EXTI->PR |= (1 << 7);
    }
}
