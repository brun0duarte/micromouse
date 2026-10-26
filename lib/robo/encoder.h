#pragma once

extern volatile long pulsosEsq;  // quadratura x4: 7 x 4 x reducao por volta da roda
extern int velEsq;               // pulsos/s, atualizado por medirVelocidade()

void iniciarEncoder();
// chamar sempre no loop: recalcula a velocidade a cada 50 ms
void medirVelocidade();
