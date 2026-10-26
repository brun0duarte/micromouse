// Os VL53L0X ligam todos em 0x29; o XSHUT desliga um sensor enquanto o endereco de outro e trocado.
// So 2 precisam de XSHUT: o da frente (sem XSHUT) recebe endereco novo enquanto os outros estao desligados.
#pragma once

#include <VL53L0X.h>

// 0 frente, 1 esquerda, 2 direita
#define TOF_FRENTE 0x30
#define TOF_ESQ 0x31
#define TOF_DIR 0x29

extern VL53L0X tof[3];
extern bool tofOk[3];
extern int dist[3];  // mm; -1 sem parede ao alcance, -2 sensor nao respondeu

// iniciarI2C() antes
void iniciarTofs();
// nao bloqueia: so le o sensor que tem medida nova
void lerDistancias();
