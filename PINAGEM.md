# Pinagem

ESP32-C3 Super Mini, vista de cima com o USB-C para cima.

```
                      ┌──[USB-C]──┐
  (livre)    GPIO5    │ ●       ● │ 5V       power bank (+), DRV8833 VCC
  SDA        GPIO6    │ ●       ● │ GND      GND comum
  SCL        GPIO7    │ ●       ● │ 3V3      MPU, VL53L0X, encoder
  XSHUT dir  GPIO8    │ ●       ● │ GPIO4    ENC esq B
  XSHUT esq  GPIO9    │ ●       ● │ GPIO3    ENC esq A
  IN3        GPIO10   │ ●       ● │ GPIO2    (não usar: strapping)
  (livre)    GPIO20   │ ●       ● │ GPIO1    IN2
  IN4        GPIO21   │ ●       ● │ GPIO0    IN1
                      └───────────┘
```

Livres: GPIO5 e GPIO20, reservados para o encoder do motor direito.

## Barramento I2C (MPU6050 + 3 VL53L0X)

Todos em paralelo: SDA no GPIO6, SCL no GPIO7, alimentação 3V3.

| Sensor | VCC/VIN | GND | SDA | SCL | XSHUT | Endereço |
|---|---|---|---|---|---|---|
| MPU6050 | 3V3 | GND | GPIO6 | GPIO7 | — (INT e AD0 soltos) | 0x68 |
| VL53L0X frente | 3V3 | GND | GPIO6 | GPIO7 | solto | 0x30 |
| VL53L0X esquerda | 3V3 | GND | GPIO6 | GPIO7 | GPIO9 | 0x31 |
| VL53L0X direita | 3V3 | GND | GPIO6 | GPIO7 | GPIO8 | 0x29 |

GPIO8 e GPIO9 são pinos de strapping: GPIO9 em nível baixo no boot entra em modo de gravação.
O pull-up do XSHUT na placa do VL53L0X mantém os dois altos, então não ligue nada que puxe esses pinos para GND.

## DRV8833

| DRV8833 | Liga em |
|---|---|
| IN1 | GPIO0 |
| IN2 | GPIO1 |
| IN3 | GPIO10 |
| IN4 | GPIO21 |
| OUT1 / OUT2 | motor esquerdo (M1 / M2 do N20) |
| OUT3 / OUT4 | motor direito |
| VCC | power bank 5 V (+) |
| GND | power bank (−) e GND da ESP32-C3 |
| EEP / SLEEP | VCC (se a placa não tiver pull-up próprio) |

## Motor N20 com encoder (motor esquerdo)

O conector de 6 fios do N20 costuma vir assim (confira a serigrafia da sua placa):

| Fio do N20 | Nome comum | Liga em |
|---|---|---|
| 1 | M1 / M+ | DRV8833 OUT1 |
| 2 | GND | GND |
| 3 | C1 / A | GPIO3 |
| 4 | C2 / B | GPIO4 |
| 5 | VCC / 3.3V | **3V3** (não 5 V: a saída do encoder segue a alimentação) |
| 6 | M2 / M− | DRV8833 OUT2 |

- Encoder de 7 pulsos por volta do eixo do motor por canal. Contando as duas bordas dos dois canais (x4):
  pulsos por volta da roda = 7 × 4 × redução (ex.: 1:30 → 840).
- Se o contador diminuir com o motor indo para frente, troque `ENC_ESQ_A` e `ENC_ESQ_B` no `main.cpp`
  (ou inverta C1 e C2).
- Encoder do motor direito (futuro): A no GPIO5, B no GPIO20.
