// Envia a orientacao do MPU6050 (calculada pelo DMP), as leituras brutas, as distancias dos 3 VL53L0X,
// a potencia dos motores e o encoder do motor esquerdo. Recebe comandos dos motores pelo serial.
// Uma linha por amostra: w,x,y,z,ax,ay,az,gx,gy,gz,frente,esquerda,direita,motorEsq,motorDir,encEsq,velEsq
//   (quaternion, g, graus/s, mm, -255..255, pulsos, pulsos/s)
// distancia = -1 sem parede ao alcance, -2 sensor nao respondeu
// Comando: "M,esq,dir" com valores de -255 a 255 (positivo = frente). Sem comando por 500 ms, para.
// Para ver em 3D e dirigir: rode visualizador/servidor.py e abra http://localhost:8000
//
// Todos no mesmo I2C: SDA no GPIO6, SCL no GPIO7, 3.3V. Pinagem em lib/robo/pinos.h e PINAGEM.md.
// Testes de cada sensor na placa: pio test
#include <Arduino.h>

#include "distancia.h"
#include "encoder.h"
#include "i2c.h"
#include "imu.h"
#include "motores.h"

unsigned long ultimoEnvio = 0;
unsigned long ultimoComando = 0;
char comando[32];
int tamComando = 0;

void lerComandos() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c != '\n') {
      if (tamComando < (int)sizeof(comando) - 1) comando[tamComando++] = c;
      continue;
    }
    comando[tamComando] = 0;
    tamComando = 0;
    int esq, dir;
    if (sscanf(comando, "M,%d,%d", &esq, &dir) == 2) {
      mover(esq, dir);
      ultimoComando = millis();
    }
  }
  // pagina fechada ou servidor parado: o robo nao sai andando sozinho
  if ((motorEsq || motorDir) && millis() - ultimoComando > 500) mover(0, 0);
}

void setup() {
  iniciarMotores();
  iniciarEncoder();
  Serial.begin(115200);
  delay(1000);  // tempo para o monitor serial conectar
  iniciarI2C();

  iniciarTofs();
  Serial.printf("VL53L0X frente %s, esquerda %s, direita %s\n",
                tofOk[0] ? "ok" : "FALHOU", tofOk[1] ? "ok" : "FALHOU", tofOk[2] ? "ok" : "FALHOU");

  if (!iniciarImu()) Serial.println("MPU6050 nao iniciou: confira os fios (seguindo sem ele)");
}

void loop() {
  lerComandos();
  lerDistancias();
  medirVelocidade();

  LeituraImu l;  // sem MPU: orientacao parada e leituras zeradas
  if (mpuOk) {
    if (!lerImu(l)) return;
  } else {
    // sem o DMP para marcar o ritmo, envia a cada 20 ms
    if (millis() - ultimoEnvio < 20) return;
    ultimoEnvio = millis();
  }

  Serial.printf("%.4f,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f,%.1f,%.1f,%.1f,%d,%d,%d,%d,%d,%ld,%d\n",
                l.w, l.x, l.y, l.z, l.ax, l.ay, l.az, l.gx, l.gy, l.gz,
                dist[0], dist[1], dist[2], motorEsq, motorDir, (long)pulsosEsq, velEsq);
}
