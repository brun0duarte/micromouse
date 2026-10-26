// Um VL53L0X sozinho no barramento (SDA GPIO6, SCL GPIO7, XSHUT solto). Aponte para algo entre 3 cm e 1 m.
#include <Arduino.h>
#include <VL53L0X.h>
#include <unity.h>

#include "i2c.h"

VL53L0X sensor;
uint8_t endereco = 0;

// 0x29 de fabrica; 0x30/0x31 se a ESP ja trocou o endereco sem cortar a energia do sensor
void test_sensor_responde() {
  const uint8_t candidatos[3] = {0x29, 0x30, 0x31};
  for (uint8_t e : candidatos)
    if (respondeEm(e)) {
      endereco = e;
      break;
    }
  TEST_ASSERT_NOT_EQUAL_MESSAGE(0, endereco, "nada em 0x29/0x30/0x31: confira VIN em 3V3, GND, SDA e SCL");
  char msg[32];
  snprintf(msg, sizeof msg, "sensor em 0x%02X", endereco);
  TEST_MESSAGE(msg);
}

void test_inicia() {
  sensor.setAddress(endereco);
  sensor.setTimeout(500);
  TEST_ASSERT_TRUE_MESSAGE(sensor.init(), "init falhou: o chip nao e um VL53L0X?");
  sensor.startContinuous(33);
}

void test_leituras() {
  char msg[32];
  for (int n = 0; n < 20; n++) {
    uint16_t mm = sensor.readRangeContinuousMillimeters();
    TEST_ASSERT_FALSE_MESSAGE(sensor.timeoutOccurred(), "timeout na leitura");
    snprintf(msg, sizeof msg, "%u mm", mm);
    TEST_MESSAGE(msg);
    // 8190/8191 = nada no alcance
    if (mm < 8190) TEST_ASSERT_INT_WITHIN_MESSAGE(990, 1010, mm, "fora de 20..2000 mm");
    delay(100);
  }
}

void setUp() {}
void tearDown() {}

void setup() {
  delay(2000);  // tempo para o pio test abrir o serial
  iniciarI2C();
  UNITY_BEGIN();
  RUN_TEST(test_sensor_responde);
  RUN_TEST(test_inicia);
  RUN_TEST(test_leituras);
  UNITY_END();
}

void loop() {}
