#include "distancia.h"

#include <Arduino.h>

#include "i2c.h"
#include "pinos.h"

VL53L0X tof[3];
bool tofOk[3];
int dist[3] = {-2, -2, -2};
static unsigned long ultimaConsulta = 0;

static bool iniciarTof(int i, uint8_t endereco) {
  tof[i] = VL53L0X();  // o XSHUT voltou o sensor para 0x29: esquece o endereco de uma chamada anterior
  tof[i].setTimeout(100);
  if (i == 0 && respondeEm(TOF_FRENTE)) tof[0].setAddress(TOF_FRENTE);
  tofOk[i] = tof[i].init();
  if (!tofOk[i]) return false;
  if (endereco != 0x29) tof[i].setAddress(endereco);
  tof[i].startContinuous();
  return true;
}

void iniciarTofs() {
  // esquerda e direita desligadas: so a frente responde
  pinMode(XSHUT_ESQ, OUTPUT);
  pinMode(XSHUT_DIR, OUTPUT);
  digitalWrite(XSHUT_ESQ, LOW);
  digitalWrite(XSHUT_DIR, LOW);
  delay(10);

  // a frente nao tem XSHUT: se a ESP reiniciou sem cortar a energia, ela continua em 0x30
  iniciarTof(0, TOF_FRENTE);

  pinMode(XSHUT_ESQ, INPUT_PULLUP);  // solta o XSHUT em vez de forcar 3.3V no pino do sensor
  delay(10);
  iniciarTof(1, TOF_ESQ);

  pinMode(XSHUT_DIR, INPUT_PULLUP);
  delay(10);
  iniciarTof(2, TOF_DIR);
}

void lerDistancias() {
  // a cada 5 ms basta: cada sensor gera uma medida a cada ~33 ms
  if (millis() - ultimaConsulta < 5) return;
  ultimaConsulta = millis();
  for (int i = 0; i < 3; i++) {
    if (!tofOk[i]) continue;
    // so le quando ha medida nova, para nao travar o loop esperando o sensor
    if ((tof[i].readReg(VL53L0X::RESULT_INTERRUPT_STATUS) & 0x07) == 0) continue;
    int mm = tof[i].readRangeContinuousMillimeters();
    dist[i] = mm > 2000 ? -1 : mm;
  }
}
