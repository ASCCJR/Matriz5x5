/**
 * @file matriz.h
 * @brief Arquivo de cabeçalho básico para o driver da matriz de LEDs WS2812B (Neopixel).
 * Declara funções para inicialização, limpeza e acender um pixel específico.
 */

#ifndef MATRIZ_H
#define MATRIZ_H

#include "pico/stdlib.h" // Para tipos básicos como uint8_t

// --- Funções de Inicialização e Controle Básico ---

/**
 * @brief Inicializa o hardware PIO para controlar a matriz de LEDs.
 * Deve ser chamada uma vez na inicialização do sistema.
 */
void matriz_init(void);

/**
 * @brief Limpa todos os LEDs da matriz (desliga todos os pixels).
 */
void matriz_limpar(void);

/**
 * @brief Acende um pixel específico na matriz com a cor desejada.
 * A matriz tem 5x5 LEDs, então x e y devem estar entre 0 e 4.
 * @param x Coordenada X (coluna, 0-4).
 * @param y Coordenada Y (linha, 0-4).
 * @param r Componente vermelho (0-255).
 * @param g Componente verde (0-255).
 * @param b Componente azul (0-255).
 */
void matriz_set_pixel(uint x, uint y, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Envia o buffer atual de pixels para a matriz física de LEDs.
 * Deve ser chamado após modificar pixels com matriz_set_pixel.
 */
void matriz_renderizar(void);


#endif // MATRIZ_H