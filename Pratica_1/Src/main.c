#include "stm32f1xx.h"

void delay_debounce(volatile uint32_t count) {
    while(count--) {}
}

int main(void) {

    RCC->APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN);

    GPIOA->CRL &= ~(0xFFF0000);
    GPIOA->CRL |=  (0x2220000);
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |=  (0x2 << 20);
    GPIOA->ODR &= ~((1 << 4) | (1 << 6));
    GPIOC->ODR &= ~(1 << 13);

    GPIOA->CRL &= ~(0xF00 | 0xF0 | 0xF0000000);
    GPIOA->CRL |=  (0x800 | 0x80 | 0x80000000);
    GPIOA->ODR |=  (1 << 1) | (1 << 2) | (1 << 7);

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
        GPIOC->ODR ^= (1 << 13);
        EXTI->PR |= (1 << 1);
    }
}

void EXTI2_IRQHandler(void) {
    if (EXTI->PR & (1 << 2)) {
        delay_debounce(200000);
        GPIOA->ODR ^= (1 << 6);
        EXTI->PR |= (1 << 2);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1 << 7)) {
        delay_debounce(200000);
        GPIOA->ODR ^= (1 << 4);
        EXTI->PR |= (1 << 7);
    }
}
