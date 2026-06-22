Aqui está a versão do **README.md** totalmente personalizada para a sua cadeira de **MICROCONTROLADORES - 01A - 2026.1**, adaptada para o padrão do seu repositório `praticas_micro` (focado em desenvolvimento bare-metal, C, Assembly, interrupções EXTI e a placa STM32F103 Blue Pill em ambiente Linux).

# Práticas de Microcontroladores (2026.1)

[![Licença](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Plataforma](https://img.shields.io/badge/platform-Linux-orange.svg)]()
[![Hardware](https://img.shields.io/badge/hardware-STM32F103%20(Blue%20Pill)-blue.svg)]()
[![Linguagem](https://img.shields.io/badge/language-C%20%2F%20Assembly-lightgrey.svg)]()

Este repositório foi criado para armazenar, organizar e documentar as atividades de laboratório e códigos desenvolvidos na disciplina **MICROCONTROLADORES - 01A - 2026.1**. O foco das práticas é o aprendizado de sistemas embarcados, programação de baixo nível (bare-metal) e manipulação direta de registradores e periféricos do microcontrolador ARM Cortex-M3.


## 📂 Estrutura do Repositório

O repositório está estruturado de forma a separar cada prática, contendo os arquivos de configuração, códigos-fonte e os arquivos de compilação


## 🛠️ Tecnologias e Ferramentas Utilizadas

* **Microcontrolador:** STM32F103C8T6 (Blue Pill)
* **Linguagens:** `C` e `Assembly` (ARM Cortex-M3)
* **Sistema Operacional de Desenvolvimento:** Linux
* **Toolchain:** `gcc-arm-none-eabi` (Compilador cruzado)
* **Gravação e Depuração:** `openocd` / `st-link`

---

## Como Compilar e Gravar os Códigos

### 1. Instalar os Pré-requisitos (Ubuntu/Debian)

Para preparar seu ambiente Linux para compilar e descarregar os programas na Blue Pill, instale as ferramentas necessárias:

sudo apt update
sudo apt install build-essential gcc-arm-none-eabi openocd stlink-tools

### 2. Clonar o Repositório

git clone [https://github.com/birabina/praticas_micro.git](https://github.com/birabina/praticas_micro.git)
cd praticas_micro

### 3. Compilação

Navegue até a pasta da prática desejada e utilize o `Makefile` para gerar o arquivo binário:


cd Pratica02_EXTI
make


### 4. Gravação (Flash)

Com a Blue Pill conectada ao ST-Link na sua porta USB, execute:


make flash
# Ou via OpenOCD diretamente se configurado


## 👤 Autores

* **Kaylane Castro** 
* **Sabrina Rodrigues** 

---

## 📄 Licença

Este projeto está sob a licença MIT. Veja o arquivo [LICENSE](https://www.google.com/search?q=LICENSE) para mais detalhes.

```

```
