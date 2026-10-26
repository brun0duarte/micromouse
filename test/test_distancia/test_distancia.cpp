// Os 3 VL53L0X na placa. Qualquer cenario serve: o teste aceita parede ou espaco livre.
#include <Arduino.h>
#include <unity.h>

#include "distancia.h"
#include "i2c.h"
#include "pinos.h"

const char *nomes[3] = {"frente", "esquerda", "direita"};

// espera ate todos os sensores que iniciaram terem uma medida nova
void esperarLeituras() {
  for (int i = 0; i < 3; i++) dist[i] = -2;
  for (unsigned long inicio = millis(); millis() - inicio < 200;) {
    lerDistancias();
    if ((!tofOk[0] || dist[0] != -2) && (!tofOk[1] || dist[1] != -2) && (!tofOk[2] || dist[2] != -2)) return;
  }
}

void test_xshut_desliga_esquerda_e_direita() {
  pinMode(XSHUT_ESQ, OUTPUT);
  pinMode(XSHUT_DIR, OUTPUT);
  digitalWrite(XSHUT_ESQ, LOW);
  digitalWrite(XSHUT_DIR, LOW);
  delay(10);
  // so a frente pode estar ligada: em 0x29 (energia recem ligada) ou em 0x30 (ja configurada)
  TEST_ASSERT_FALSE_MESSAGE(respondeEm(TOF_ESQ), "algo ainda responde em 0x31 com os XSHUT em LOW");
  TEST_ASSERT_TRUE_MESSAGE(respondeEm(0x29) != respondeEm(TOF_FRENTE),
                           "esperava so a frente, em 0x29 ou 0x30: confira o XSHUT dos laterais");
}

void test_tres_sensores_iniciam() {
  iniciarTofs();
  for (int i = 0; i < 3; i++) TEST_ASSERT_TRUE_MESSAGE(tofOk[i], nomes[i]);
}

void test_enderecos_distintos() {
  TEST_ASSERT_TRUE_MESSAGE(respondeEm(TOF_FRENTE), "frente fora de 0x30");
  TEST_ASSERT_TRUE_MESSAGE(respondeEm(TOF_ESQ), "esquerda fora de 0x31");
  TEST_ASSERT_TRUE_MESSAGE(respondeEm(TOF_DIR), "direita fora de 0x29");
}

void test_leituras_no_alcance() {
  for (int n = 0; n < 10; n++) {
    esperarLeituras();
    for (int i = 0; i < 3; i++) {
      TEST_ASSERT_NOT_EQUAL_MESSAGE(-2, dist[i], nomes[i]);
      // -1 = livre; abaixo de 20 mm o VL53L0X nao mede direito
      if (dist[i] != -1) TEST_ASSERT_INT_WITHIN_MESSAGE(990, 1010, dist[i], nomes[i]);
    }
  }
}

void test_sem_timeout() {
  for (int i = 0; i < 3; i++) TEST_ASSERT_FALSE_MESSAGE(tof[i].timeoutOccurred(), nomes[i]);
}

// a ESP reinicia (upload, botao RESET) sem cortar a energia dos sensores
void test_reinicia_sem_cortar_energia() {
  iniciarTofs();
  for (int i = 0; i < 3; i++) TEST_ASSERT_TRUE_MESSAGE(tofOk[i], nomes[i]);
  esperarLeituras();
  for (int i = 0; i < 3; i++) TEST_ASSERT_NOT_EQUAL_MESSAGE(-2, dist[i], nomes[i]);
}

void setUp() {}
void tearDown() {}

void setup() {
  delay(2000);  // tempo para o pio test abrir o serial
  iniciarI2C();
  UNITY_BEGIN();
  RUN_TEST(test_xshut_desliga_esquerda_e_direita);
  RUN_TEST(test_tres_sensores_iniciam);
  RUN_TEST(test_enderecos_distintos);
  RUN_TEST(test_leituras_no_alcance);
  RUN_TEST(test_sem_timeout);
  RUN_TEST(test_reinicia_sem_cortar_energia);
  UNITY_END();
}

void loop() {}
