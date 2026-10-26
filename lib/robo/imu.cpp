#include "imu.h"

#include <MPU6050_6Axis_MotionApps20.h>

static MPU6050 mpu;
static uint8_t fifo[64];
static float acelLsb, giroLsb;  // leitura bruta por g e por grau/s
bool mpuOk = false;

bool iniciarImu() {
  mpu.initialize();
  mpuOk = mpu.dmpInitialize() == 0;
  if (!mpuOk) return false;

  // so o giro: o CalibrateAccel da biblioteca 1.4.5 erra o alvo e satura o Z em 2 g
  Serial.println("calibrando: deixe o sensor parado...");
  mpu.CalibrateGyro(6);
  Serial.println();
  mpu.setDMPEnabled(true);

  acelLsb = 16384 >> mpu.getFullScaleAccelRange();
  giroLsb = 131.0 / (1 << mpu.getFullScaleGyroRange());
  return true;
}

bool lerImu(LeituraImu &l) {
  if (!mpu.dmpGetCurrentFIFOPacket(fifo)) return false;
  Quaternion q;
  mpu.dmpGetQuaternion(&q, fifo);
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  l.w = q.w, l.x = q.x, l.y = q.y, l.z = q.z;
  l.ax = ax / acelLsb, l.ay = ay / acelLsb, l.az = az / acelLsb;
  l.gx = gx / giroLsb, l.gy = gy / giroLsb, l.gz = gz / giroLsb;
  return true;
}
