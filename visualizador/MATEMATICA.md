# A matemática do visualizador

Uma introdução ao que o script do [index.html](index.html) faz com os números que chegam da placa.

## 1. O que chega da placa

Cada linha do serial tem esta forma:

```
w,x,y,z, ax,ay,az, gx,gy,gz, frente,esquerda,direita, motorEsq,motorDir
```

- `w,x,y,z`: a **orientação** do sensor, como um quaternion (seção 2). Quem calcula é o DMP, um processador dentro do próprio MPU6050 que junta acelerômetro e giroscópio.
- `ax,ay,az`: **aceleração** em g. Parado e deitado, mede cerca de `0, 0, 1`: é a gravidade puxando para baixo.
- `gx,gy,gz`: **velocidade de giro** em graus por segundo. Parado, fica perto de 0.
- O resto são as distâncias em mm e a potência dos motores.

A função `processarLinha` separa os números por vírgula e guarda cada grupo.

## 2. Quaternions: o que são

Para descrever "para onde o robô está virado", o jeito mais intuitivo são três ângulos:

- **yaw**: virar para os lados (em torno do eixo vertical);
- **pitch**: empinar ou abaixar o nariz;
- **roll**: tombar para um lado.

O problema é que, em certas posições (com o nariz apontando reto para cima, por exemplo), dois desses ângulos passam a girar em torno do mesmo eixo e um grau de liberdade se perde. Isso se chama **gimbal lock**, e as contas ficam instáveis.

O **quaternion** evita isso. Ele usa quatro números para dizer duas coisas: em torno de qual eixo girar e quanto girar.

```
q = (w, x, y, z)
w = cos(θ/2)
(x, y, z) = eixo de rotação × sin(θ/2)
```

Exemplo: girar 90° em torno do eixo Z vertical dá `θ = 90°`, e portanto
`q = (cos 45°, 0, 0, sin 45°) ≈ (0.707, 0, 0, 0.707)`.
Sem rotação nenhuma: `q = (1, 0, 0, 0)`. É o que a placa manda quando está parada na posição inicial.

Um quaternion de rotação sempre tem tamanho 1 (`w² + x² + y² + z² = 1`).

## 3. Combinar e desfazer rotações

Duas operações aparecem no script:

- **Multiplicação** (`mult(a, b)`): aplicar uma rotação depois da outra. A ordem importa: `a·b` em geral é diferente de `b·a`, assim como girar um livro primeiro para a frente e depois para o lado dá um resultado diferente da ordem inversa.
- **Conjugado** (`conj(q)`): a rotação inversa. Basta trocar o sinal de `x, y, z`.

O botão **Zerar orientação** usa as duas:

```js
qRef = qAtual;                      // guarda a posição de agora
const q = mult(conj(qRef), qAtual); // "desfaz" qRef antes de mostrar
```

Logo depois de clicar, `qAtual = qRef`, e o resultado é `(1, 0, 0, 0)`: o robô aparece reto. Daí em diante, a tela mostra só o quanto ele girou a partir daquele ponto.

## 4. Trocar de sistema de eixos

O sensor e o three.js (a biblioteca 3D) não concordam sobre qual eixo aponta para cima:

| | para cima | para a frente |
|---|---|---|
| MPU6050 | Z | Y |
| three.js | Y | −Z |

Por isso o script reorganiza os componentes antes de girar o modelo:

```js
robo.quaternion.set(q.x, q.z, -q.y, q.w);
```

Essa linha diz que o X do sensor é o X da tela, que o Z do sensor vira o Y da tela e que o Y do sensor vira o −Z da tela.
Se a placa for montada em outra posição no chassi, é só esta linha que muda.

## 5. De quaternion para yaw, pitch e roll

Os ângulos do painel são calculados a partir do quaternion com as fórmulas padrão:

```js
roll  = atan2(2(w·x + y·z), 1 − 2(x² + y²))
pitch = asin (2(w·y − z·x))
yaw   = atan2(2(w·z + x·y), 1 − 2(y² + z²))
```

O `atan2` devolve ângulos de −180° a 180°, sem ambiguidade de quadrante. O `asin` do pitch só vai de −90° a 90°: é exatamente o ponto em que aparece o gimbal lock. Por isso os ângulos servem só para **mostrar** a orientação, e quem gira o modelo 3D é o quaternion.

## 6. Os gráficos

`desenharGrafico` guarda as últimas 300 amostras (~3 s) e ajusta a escala sozinha: procura o maior valor absoluto (no mínimo 1,2 g ou 20 °/s) e o usa como topo do gráfico. A linha do meio é o zero.

## Para se aprofundar

- [Visualizing quaternions](https://eater.net/quaternions), de Ben Eater e 3Blue1Brown: vídeos interativos, o melhor ponto de partida.
- [Quaternions and spatial rotation](https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation) (Wikipédia): as contas completas.
- [Conversion between quaternions and Euler angles](https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles) (Wikipédia): de onde vêm as fórmulas da seção 5.
- [Gimbal lock](https://en.wikipedia.org/wiki/Gimbal_lock) (Wikipédia): o problema que os quaternions resolvem.
- [three.js: Quaternion](https://threejs.org/docs/#api/en/math/Quaternion): como a biblioteca 3D usa quaternions.
- [MPU-6000/6050 Register Map](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf) (InvenSense): de onde vêm as escalas do acelerômetro e do giroscópio.
