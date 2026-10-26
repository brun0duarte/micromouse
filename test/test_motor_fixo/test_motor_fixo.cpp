// Diagnostico temporario: segura o motor esquerdo em potencia cheia para medir o DRV8833 com multimetro.
#include <Arduino.h>
#include <unity.h>

#include "encoder.h"
#include "motores.h"

// mantem a potencia por 'ms', imprimindo a contagem do encoder a cada segundo
void segurar(const char *nome, int potencia, int ms) {
  Serial.printf("[%s] potencia=%d por %d s\n", nome, potencia, ms / 1000);
  mover(potencia, 0);
  for (int s = 1; s <= ms / 1000; s++) {
    delay(1000);
    Serial.printf("[%s] %d s pulsos=%ld\n", nome, s, pulsosEsq);
  }
  mover(0, 0);
  delay(1000);
}

void test_segurar_motor() {
  segurar("frente", 255, 15000);
  segurar("re", -255, 15000);
}

void setUp() {}
void tearDown() { mover(0, 0); }

void setup() {
  iniciarMotores();
  iniciarEncoder();
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_segurar_motor);
  UNITY_END();
}

void loop() {}
