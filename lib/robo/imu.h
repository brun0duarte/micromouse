// MPU6050 com a orientacao calculada pelo DMP. INT nao e usado.
// So o imu.cpp inclui o MotionApps20: o header define funcoes e quebra o link se entrar em 2 arquivos.
#pragma once

#define MPU_ENDERECO 0x68

struct LeituraImu {
  float w = 1, x = 0, y = 0, z = 0;  // quaternion
  float ax = 0, ay = 0, az = 0;      // g
  float gx = 0, gy = 0, gz = 0;      // graus/s
};

extern bool mpuOk;

// iniciarI2C() antes. Calibra o giro: o sensor precisa estar parado.
bool iniciarImu();
// false enquanto o DMP nao tem pacote novo
bool lerImu(LeituraImu &leitura);
