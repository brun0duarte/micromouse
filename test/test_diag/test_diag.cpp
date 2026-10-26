// Diagnostico temporario: motor esquerdo + encoder, camada por camada.
#include <Arduino.h>
#include <driver/gpio.h>
#include <unity.h>

#include "encoder.h"
#include "motores.h"
#include "pinos.h"

// fracao do tempo em HIGH no pad do pino (le o nivel real, mesmo sendo saida)
float dutyNoPad(int pino) {
  gpio_input_enable((gpio_num_t)pino);
  int altos = 0;
  for (int i = 0; i < 20000; i++) altos += gpio_get_level((gpio_num_t)pino);
  return altos / 20000.0;
}

// transicoes vistas por polling nos pinos do encoder durante ms
void sondar(const char *nome, int esq, int ms) {
  long isrAntes = pulsosEsq;
  mover(esq, 0);
  delay(50);
  float d1 = dutyNoPad(ESQ_IN1), d2 = dutyNoPad(ESQ_IN2);
  int a0 = digitalRead(ENC_ESQ_A), b0 = digitalRead(ENC_ESQ_B);
  int ta = 0, tb = 0, a = a0, b = b0;
  for (unsigned long t = millis(); millis() - t < (unsigned long)ms;) {
    int na = digitalRead(ENC_ESQ_A), nb = digitalRead(ENC_ESQ_B);
    ta += na != a, tb += nb != b, a = na, b = nb;
  }
  mover(0, 0);
  delay(300);
  Serial.printf("[%s] cmd=%d duty IN1=%.2f IN2=%.2f | A inicio=%d trans=%d | B inicio=%d trans=%d | ISR=%ld\n",
                nome, esq, d1, d2, a0, ta, b0, tb, pulsosEsq - isrAntes);
}

void test_diag() {
  // com pull-down: 1 = algo liga o pino em 3V3; 0 = pino solto ou encoder sem VCC
  for (int pull : {INPUT_PULLUP, INPUT_PULLDOWN, INPUT}) {
    pinMode(ENC_ESQ_A, pull);
    pinMode(ENC_ESQ_B, pull);
    delay(5);
    int a = 0, b = 0;
    for (int i = 0; i < 1000; i++) a += digitalRead(ENC_ESQ_A), b += digitalRead(ENC_ESQ_B);
    Serial.printf("[%s] A=%d/1000 B=%d/1000\n", pull == INPUT_PULLUP ? "pullup" : pull == INPUT_PULLDOWN ? "pulldown" : "solto", a, b);
  }
  iniciarEncoder();
  sondar("parado", 0, 500);
  sondar("frente150", 150, 800);
  sondar("re150", -150, 800);
  sondar("frente255", 255, 800);
  sondar("re255", -255, 800);
}

void setUp() {}
void tearDown() { mover(0, 0); }

void setup() {
  iniciarMotores();
  iniciarEncoder();
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_diag);
  UNITY_END();
}

void loop() {}
