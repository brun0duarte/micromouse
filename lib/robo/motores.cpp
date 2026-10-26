#include "motores.h"

#include <Arduino.h>

#include "pinos.h"

// N20 de 6 V alimentados pelo power bank (5 V): potencia cheia e segura.
// Ao trocar para a LiPo 2S (ate 8.4 V), use 180 para nao passar de ~6 V nos motores.
#define PWM_MAX 255

int motorEsq = 0, motorDir = 0;

static void motor(int in1, int in2, int valor) {
  int pwm = map(constrain(abs(valor), 0, 255), 0, 255, 0, PWM_MAX);
  analogWrite(in1, valor > 0 ? pwm : 0);
  analogWrite(in2, valor < 0 ? pwm : 0);
}

void iniciarMotores() {
  for (int pino : {ESQ_IN1, ESQ_IN2, DIR_IN1, DIR_IN2}) analogWriteFrequency(pino, 20000);  // acima do audivel
  mover(0, 0);
}

void mover(int esq, int dir) {
  motorEsq = constrain(esq, -255, 255);
  motorDir = constrain(dir, -255, 255);
  motor(ESQ_IN1, ESQ_IN2, motorEsq);
  motor(DIR_IN1, DIR_IN2, motorDir);
}
