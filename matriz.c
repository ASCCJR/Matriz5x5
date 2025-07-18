/**
 * @file matriz.c
 * @brief Implementação básica do driver para controle da matriz de LEDs WS2812B (Neopixel).
 * Inclui funções para inicialização, limpeza e acender um pixel específico.
 */

#include "matriz.h"
#include "configura_geral.h" // Para MATRIZ_PIN (que deve estar definido como 7)
#include "hardware/pio.h"
#include "ws2812.pio.h"     // Para o programa PIO do WS2812
#include <string.h>         // Para memset

// --- Definições Internas ---
#define LED_COUNT 25 // Total de LEDs na matriz 5x5 (5x5)

// --- Variáveis Estáticas Globais ---
static uint32_t matriz_buffer[LED_COUNT] = {0}; // Buffer de pixels da matriz (formato GRB)

// --- Protótipos de Funções Estáticas (Auxiliares Internas) ---
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b); // Converte RGB para GRB (uint32_t)
static inline void put_pixel_raw(uint32_t pixel_grb);          // Envia um pixel para o PIO
static uint xy_to_index(uint x, uint y);                     // Converte coordenadas (x,y) para índice linear

// --- Implementações de Funções Estáticas ---

/**
 * @brief Converte componentes de cor RGB em um formato GRB de 32 bits para o WS2812B.
 * @param r Componente vermelho (0-255).
 * @param g Componente verde (0-255).
 * @param b Componente azul (0-255).
 * @return Valor GRB de 32 bits.
 */
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(g) << 16) | ((uint32_t)(r) << 8) | (uint32_t)(b);
}

/**
 * @brief Envia um único pixel formatado (GRB) para o hardware PIO.
 * Esta função é interna e não deve ser chamada diretamente. Use matriz_renderizar().
 * @param pixel_grb O valor GRB de 32 bits do pixel.
 */
static inline void put_pixel_raw(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

/**
 * @brief Converte coordenadas (x, y) para um índice linear no buffer da matriz.
 * Leva em consideração o layout "zig-zag" dos LEDs na matriz 5x5.
 * Este é o layout corrigido para a sua placa.
 * @param x Coordenada X (coluna, 0-4).
 * @param y Coordenada Y (linha, 0-4).
 * @return O índice linear correspondente no buffer.
 */
static uint xy_to_index(uint x, uint y) {
    // Inverte o eixo Y para corrigir a orientação da matriz (confirmado por teste)
    uint inverted_y = 4 - y; // Para uma matriz 5x5 (índices 0-4)

    if (inverted_y % 2 == 0) { // Linhas pares (agora invertidas): da esquerda para a direita
        return inverted_y * 5 + x;
    } else { // Linhas ímpares (agora invertidas): da direita para a esquerda
        return inverted_y * 5 + (4 - x);
    }
}

// --- Funções Públicas (API do Módulo) ---

/**
 * @brief Inicializa o hardware PIO para controlar a matriz de LEDs.
 * Deve ser chamada uma vez na inicialização do sistema.
 */
void matriz_init(void) {
    uint offset = pio_add_program(pio0, &ws2812_program);
    ws2812_program_init(pio0, 0, offset, MATRIZ_PIN, 800000, false);
}

/**
 * @brief Limpa todos os LEDs da matriz (desliga todos os pixels no buffer).
 * Para ver a mudança, chame matriz_renderizar() em seguida.
 */
void matriz_limpar(void) {
    memset(matriz_buffer, 0, sizeof(matriz_buffer)); // Preenche o buffer com zeros
    // Não renderiza automaticamente, é preciso chamar matriz_renderizar()
}

/**
 * @brief Acende um pixel específico na matriz com a cor desejada no buffer.
 * A matriz tem 5x5 LEDs, então x e y devem estar entre 0 e 4.
 * Para ver a mudança na matriz física, chame matriz_renderizar() em seguida.
 * @param x Coordenada X (coluna, 0-4).
 * @param y Coordenada Y (linha, 0-4).
 * @param r Componente vermelho (0-255).
 * @param g Componente verde (0-255).
 * @param b Componente azul (0-255).
 */
void matriz_set_pixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b) {
    if (x < 5 && y < 5) { // Garante que as coordenadas são válidas para 5x5
        matriz_buffer[xy_to_index(x, y)] = urgb_u32(r, g, b);
    }
}

/**
 * @brief Envia o conteúdo atual do buffer de pixels para a matriz física de LEDs.
 * Deve ser chamado após modificar pixels com matriz_set_pixel.
 */
void matriz_renderizar(void) {
    for (int i = 0; i < LED_COUNT; ++i) {
        put_pixel_raw(matriz_buffer[i]);
    }
}