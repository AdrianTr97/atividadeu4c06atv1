//Uso da interface I2C para comunicação com o Display OLED
//Estudo da biblioteca ssd1306 com PicoW na Placa BitDogLab.
//Este programa escreve uma mensagem no display OLED.

#include <stdio.h> 
#include <stdlib.h>
#include "pico/stdlib.h"  
// Biblioteca para uso das funções básicas do Raspberry Pi Pico
#include "hardware/pio.h"  
// Biblioteca para PIO (Programmable I/O) do Pico
#include "hardware/clocks.h"  
// Biblioteca para manipulação de relógios
#include "hardware/gpio.h"  
// Biblioteca para controle de GPIOs (usada para os botões e LEDs)
#include "atividadeu4c06atv1.pio.h"  
// Arquivo gerado pelo PIO 

#include "hardware/i2c.h"  
// Biblioteca para comunicação I2C com o OLED
#include "inc/ssd1306.h"  
// Biblioteca para controle do display OLED SSD1306
#include "inc/font.h"  
// Biblioteca para fontes do OLED 


#define IS_RGBW false  
// Define se os LEDs são RGBW 
#define NUM_PIXELS 25  
// Define o número de pixels na matriz de LEDs 5x5
#define WS2812_PIN 7  
// Define o pino de controle dos LEDs WS2812 
#define BTN_A 5  
#define BTN_B 6  
// Define os pino do botão A e B
#define LED_R 13  
// Define o pino do LED vermelho (não utilizado)
#define LED_B 12  
// Define o pino do LED azul
#define LED_G 11  
// Define o pino do LED verde
#define I2C_PORT i2c1  
// Define o barramento I2C a ser usado
#define I2C_SDA 14  
// Define o pino SDA do barramento I2C
#define I2C_SCL 15  
// Define o pino SCL do barramento I2C
#define ENDERECO_OLED 0x3C  
// Define o endereço do OLED (I2C)

ssd1306_t ssd;  // Instancia o objeto para controlar o display SSD1306
volatile bool estado_led_g = false;  // Variável para o estado do LED verde
volatile bool estado_led_b = false;  // Variável para o estado do LED azul
bool led_buffer[NUM_PIXELS] = {0};  // Buffer de LEDs (não utilizado diretamente)

// Tabela que mapeia números de 0 a 9 para padrões de LEDs
void atualizar_matriz(int numero) {
    const bool numeros_matriz_led[10][NUM_PIXELS] = {
        {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1}, // 0
        {1,0,0,0,0, 0,0,0,0,1, 1,0,0,0,0, 0,0,0,0,1, 1,0,0,0,0}, // 1
        {1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1}, // 2
        {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1}, // 3
        {1,0,0,0,0, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1}, // 4
        {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 5
        {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1}, // 6
        {1,0,0,0,0, 0,0,0,0,1, 1,0,0,0,0, 0,0,0,0,1, 1,1,1,1,1}, // 7
        {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}, // 8
        {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}  // 9
    };
    // Atualiza o buffer de LEDs de acordo com o número
    for (int i = 0; i < NUM_PIXELS; i++) {
        led_buffer[i] = numeros_matriz_led[numero][i];
    }
}

void exibir_no_oled(char c) {
    // Preenche a tela com fundo preto
    ssd1306_fill(&ssd, false);
    char texto[2] = {c, '\0'};  // Cria um array de caracteres para exibir o símbolo
    ssd1306_draw_string(&ssd, texto, 20, 30);  // Desenha o texto no display
    ssd1306_send_data(&ssd);  // Envia os dados para o display OLED
}

// Função de callback para os botões
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BTN_A) {
        estado_led_g = !estado_led_g;  // Alterna o estado do LED verde
        gpio_put(LED_G, estado_led_g);  // Atualiza o LED verde
        printf("Botao A pressionado: LED Verde %s\n", estado_led_g ? "Ligado" : "Desligado");
        exibir_no_oled(estado_led_g ? 'G' : 'g');  // Exibe no OLED se o LED verde está ligado ou desligado
    } else if (gpio == BTN_B) {
        estado_led_b = !estado_led_b;  // Alterna o estado do LED azul
        gpio_put(LED_B, estado_led_b);  // Atualiza o LED azul
        printf("Botao B pressionado: LED Azul %s\n", estado_led_b ? "Ligado" : "Desligado");
        exibir_no_oled(estado_led_b ? 'B' : 'b');  // Exibe no OLED se o LED azul está ligado ou desligado
    }
}

int main() {
    stdio_init_all();  // Inicializa a comunicação serial
    i2c_init(I2C_PORT, 400 * 1000);  // Inicializa o barramento I2C com uma taxa de 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Configura o pino SDA para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Configura o pino SCL para I2C
    gpio_pull_up(I2C_SDA);  // Ativa o resistor de pull-up no SDA
    gpio_pull_up(I2C_SCL);  // Ativa o resistor de pull-up no SCL
    ssd1306_init(&ssd, 128, 64, false, ENDERECO_OLED, I2C_PORT);  // Inicializa o display OLED
    ssd1306_fill(&ssd, false);  // Limpa a tela
    ssd1306_send_data(&ssd);  // Atualiza a tela

    gpio_init(BTN_A);  // Inicializa o botão A
    gpio_init(BTN_B);  // Inicializa o botão B
    gpio_set_dir(BTN_A, GPIO_IN);  // Configura o botão A como entrada
    gpio_set_dir(BTN_B, GPIO_IN);  // Configura o botão B como entrada
    gpio_pull_up(BTN_A);  // Ativa o pull-up no botão A
    gpio_pull_up(BTN_B);  // Ativa o pull-up no botão B
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);  // Configura interrupção para o botão A
    gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);  // Configura interrupção para o botão B

    gpio_init(LED_G);  // Inicializa o LED verde
    gpio_init(LED_B);  // Inicializa o LED azul
    gpio_set_dir(LED_G, GPIO_OUT);  // Configura o LED verde como saída
    gpio_set_dir(LED_B, GPIO_OUT);  // Configura o LED azul como saída

    while (true) {
        if (stdio_usb_connected()) {  // Verifica se a conexão USB está ativa
            int c = getchar();  // Lê um caractere da entrada serial
            if (c != EOF) {
                printf("Caractere recebido: %c\n", c);  // Exibe o caractere recebido
                exibir_no_oled(c);  // Exibe o caractere no OLED
                if (c >= '0' && c <= '9') {  // Se o caractere for um número
                    atualizar_matriz(c - '0');  // Atualiza a matriz de LEDs para o número correspondente
                }
            }
        }
        sleep_ms(100);  // Aguarda 100ms antes de verificar novamente
    }
}
