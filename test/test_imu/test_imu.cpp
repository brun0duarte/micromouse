// MPU6050 na placa. Deixe o robo parado e na horizontal durante o teste.
#include <Arduino.h>
#include <unity.h>

#include "i2c.h"
#include "imu.h"

// media de n pacotes do DMP; false se o DMP parar de mandar
bool mediaImu(LeituraImu &media, int n) {
  media = LeituraImu();
  media.w = 0;
  for (int i = 0; i < n; i++) {
    LeituraImu l;
    unsigned long inicio = millis();
    while (!lerImu(l))
      if (millis() - inicio > 200) return false;
    media.ax += l.ax / n, media.ay += l.ay / n, media.az += l.az / n;
    media.gx += l.gx / n, media.gy += l.gy / n, media.gz += l.gz / n;
  }
  return true;
}

void test_responde_no_i2c() {
  TEST_ASSERT_TRUE_MESSAGE(respondeEm(MPU_ENDERECO), "nada em 0x68: confira SDA, SCL e VCC");
}

void test_dmp_inicia() {
  TEST_ASSERT_TRUE_MESSAGE(iniciarImu(), "dmpInitialize falhou");
}

void test_dmp_manda_pacotes() {
  int pacotes = 0;
  LeituraImu l;
  for (unsigned long inicio = millis(); millis() - inicio < 1000;)
    if (lerImu(l)) pacotes++;
  TEST_ASSERT_GREATER_THAN_MESSAGE(20, pacotes, "menos de 20 pacotes por segundo");
}

void test_quaternion_unitario() {
  LeituraImu l;
  unsigned long inicio = millis();
  while (!lerImu(l)) TEST_ASSERT_LESS_THAN(200, millis() - inicio);
  TEST_ASSERT_FLOAT_WITHIN(0.02, 1.0, sqrt(l.w * l.w + l.x * l.x + l.y * l.y + l.z * l.z));
}

void test_parado_mede_1g() {
  LeituraImu m;
  TEST_ASSERT_TRUE(mediaImu(m, 50));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 1.0, sqrt(m.ax * m.ax + m.ay * m.ay + m.az * m.az));
}

void test_na_horizontal_gravidade_em_z() {
  LeituraImu m;
  TEST_ASSERT_TRUE(mediaImu(m, 50));
  TEST_ASSERT_GREATER_THAN_MESSAGE(0.9, fabs(m.az), "gravidade fora do Z: o robo esta inclinado?");
}

void test_parado_giro_perto_de_zero() {
  LeituraImu m;
  TEST_ASSERT_TRUE(mediaImu(m, 50));
  TEST_ASSERT_FLOAT_WITHIN(2.0, 0, m.gx);
  TEST_ASSERT_FLOAT_WITHIN(2.0, 0, m.gy);
  TEST_ASSERT_FLOAT_WITHIN(2.0, 0, m.gz);
}

void setUp() {}
void tearDown() {}

void setup() {
  delay(2000);  // tempo para o pio test abrir o serial
  iniciarI2C();
  UNITY_BEGIN();
  RUN_TEST(test_responde_no_i2c);
  RUN_TEST(test_dmp_inicia);
  if (mpuOk) {
    RUN_TEST(test_dmp_manda_pacotes);
    RUN_TEST(test_quaternion_unitario);
    RUN_TEST(test_parado_mede_1g);
    RUN_TEST(test_na_horizontal_gravidade_em_z);
    RUN_TEST(test_parado_giro_perto_de_zero);
  }
  UNITY_END();
}

void loop() {}
