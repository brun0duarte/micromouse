// A ESP32-C3 nao tem PCNT: a contagem e feita por interrupcao nas duas bordas dos dois canais.
#include "encoder.h"

#include <Arduino.h>

#include "pinos.h"

volatile long pulsosEsq = 0;
int velEsq = 0;
static volatile uint8_t estadoEsq = 0;
static long pulsosAnterior = 0;
static unsigned long ultimaVelocidade = 0;

// cada mudanca de A ou B anda um passo; transicao invalida (pulo de 2 estados) conta 0
static void IRAM_ATTR encoderEsq() {
  static const int8_t passo[16] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
  uint8_t agora = digitalRead(ENC_ESQ_A) << 1 | digitalRead(ENC_ESQ_B);
  pulsosEsq += passo[estadoEsq << 2 | agora];
  estadoEsq = agora;
}

void iniciarEncoder() {
  pinMode(ENC_ESQ_A, INPUT_PULLUP);  // algumas placas de encoder tem saida em coletor aberto
  pinMode(ENC_ESQ_B, INPUT_PULLUP);
  estadoEsq = digitalRead(ENC_ESQ_A) << 1 | digitalRead(ENC_ESQ_B);
  attachInterrupt(ENC_ESQ_A, encoderEsq, CHANGE);
  attachInterrupt(ENC_ESQ_B, encoderEsq, CHANGE);
}

void medirVelocidade() {
  unsigned long agora = millis();
  if (agora - ultimaVelocidade < 50) return;
  long pulsos = pulsosEsq;
  velEsq = (pulsos - pulsosAnterior) * 1000 / (long)(agora - ultimaVelocidade);
  pulsosAnterior = pulsos;
  ultimaVelocidade = agora;
}
