#pragma once

extern int motorEsq, motorDir;  // ultima potencia aplicada, -255..255

void iniciarMotores();
// valores de -255 a 255; positivo = frente
void mover(int esq, int dir);
