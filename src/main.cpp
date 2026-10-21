// Envia a orientacao do MPU6050 (calculada pelo DMP) e as leituras brutas pelo serial.
// Uma linha por amostra: w,x,y,z,ax,ay,az,gx,gy,gz  (quaternion, g, graus/s)
// Para ver em 3D: abra visualizador/index.html no Chrome.
// MPU6050: SDA no GPIO6, SCL no GPIO7, 3.3V. O pino INT nao e usado (le a FIFO por polling).
#include <Arduino.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Wire.h>

#define SDA_PIN 6
#define SCL_PIN 7

MPU6050 mpu;
uint8_t fifo[64];
float acelLsb, giroLsb;  // leitura bruta por g e por grau/s

void setup() {
  Serial.begin(115200);
  delay(1000);  // tempo para o monitor serial conectar
  Wire.begin(SDA_PIN, SCL_PIN, 400000);

  mpu.initialize();
  if (mpu.dmpInitialize() != 0) {
    Serial.println("MPU6050 nao iniciou: confira os fios");
    while (true) delay(1000);
  }

  // so o giro: o CalibrateAccel da biblioteca 1.4.5 erra o alvo e satura o Z em 2 g
  Serial.println("calibrando: deixe o sensor parado...");
  mpu.CalibrateGyro(6);
  Serial.println();
  mpu.setDMPEnabled(true);

  acelLsb = 16384 >> mpu.getFullScaleAccelRange();
  giroLsb = 131.0 / (1 << mpu.getFullScaleGyroRange());
}

void loop() {
  if (!mpu.dmpGetCurrentFIFOPacket(fifo)) return;

  Quaternion q;
  mpu.dmpGetQuaternion(&q, fifo);
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.printf("%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f,%.1f,%.1f,%.1f\n",
                q.w, q.x, q.y, q.z,
                ax / acelLsb, ay / acelLsb, az / acelLsb,
                gx / giroLsb, gy / giroLsb, gz / giroLsb);
}
