// Motor esquerdo e seu encoder na placa. A roda gira sozinha: deixe o robo suspenso.
#include <Arduino.h>
#include <unity.h>

#include "encoder.h"
#include "motores.h"

#define POTENCIA 150
#define MIN_PULSOS 50  // em 500 ms; o N20 da centenas de pulsos nesse tempo

// variacao do encoder com o motor esquerdo na potencia dada por 'ms'
long andar(int potencia, int ms) {
  long antes = pulsosEsq;
  mover(potencia, 0);
  delay(ms);
  mover(0, 0);
  delay(200);  // a roda para antes do proximo teste
  return pulsosEsq - antes;
}

void test_parado_nao_conta() {
  long antes = pulsosEsq;
  delay(500);
  TEST_ASSERT_EQUAL_MESSAGE(antes, pulsosEsq, "contou com o motor parado: ruido no A ou B");
}

void test_frente_aumenta() {
  long delta = andar(POTENCIA, 500);
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, delta, "encoder nao contou: motor parado ou encoder sem 3V3/fios");
  TEST_ASSERT_GREATER_THAN_MESSAGE(MIN_PULSOS, delta, "contou para tras: troque ENC_ESQ_A e ENC_ESQ_B");
}

void test_re_diminui() {
  long delta = andar(-POTENCIA, 500);
  TEST_ASSERT_LESS_THAN_MESSAGE(-MIN_PULSOS, delta, "na re o contador nao diminuiu");
}

void test_ida_e_volta_simetricas() {
  long ida = andar(POTENCIA, 500);
  long volta = andar(-POTENCIA, 500);
  TEST_ASSERT_GREATER_THAN_MESSAGE(MIN_PULSOS, ida, "nao andou na ida");
  // mesmo tempo e potencia: as contagens nao devem diferir mais que 30%
  TEST_ASSERT_INT_WITHIN(abs(ida) * 3 / 10, abs(ida), abs(volta));
}

void test_velocidade_positiva_na_frente() {
  mover(POTENCIA, 0);
  for (unsigned long inicio = millis(); millis() - inicio < 300;) medirVelocidade();
  int vel = velEsq;
  mover(0, 0);
  TEST_ASSERT_GREATER_THAN(0, vel);
}

void setUp() {}
void tearDown() { mover(0, 0); }

void setup() {
  iniciarMotores();
  iniciarEncoder();
  delay(2000);  // tempo para o pio test abrir o serial
  UNITY_BEGIN();
  RUN_TEST(test_parado_nao_conta);
  RUN_TEST(test_frente_aumenta);
  RUN_TEST(test_re_diminui);
  RUN_TEST(test_ida_e_volta_simetricas);
  RUN_TEST(test_velocidade_positiva_na_frente);
  UNITY_END();
}

void loop() {}
