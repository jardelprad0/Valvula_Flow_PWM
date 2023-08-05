#include <CAN.h> //Inclui a biblioteca CAN

// ----- DEFINE OS IDENTIFICADOS DO módulo -----
#define SECTION 0 // Seção ao qual o módulo diz respeito
#define NODE 0 // Identificador do módulo na seção
// -----------------------------------------

// ----- DEFINE PINOS DOS BICOS -----
#define tip1 27
#define tip2 25
#define tip3 32
// ----------------------------------

// ----- ENUMS -----
enum packetIterator {
  pSECTION = 0, pNODE = 1, TIP_ONE = 2, TIP_TWO = 3, TIP_THREE = 4
};

enum tipsStatus {
  OPEN = 0, CLOSED = 1
};
// -----------------

// ----- Variáveis de uso geral -----
uint8_t received[5];
// ----------------------------------

// ----- Funções -----
void tipsControl(uint8_t *packet);
// -------------------

void setup() {
  Serial.begin(115200); // Inicia a serial para debug
  while (!Serial);

  // Inicia o barramento CAN a 500 kbps
  if (!CAN.begin(125E3)) {
    Serial.println("Falha ao iniciar o controlador CAN"); // Caso não seja possível iniciar o controlador
    while (1);
  }

  // Definição dos pinos como saída
  pinMode(tip1, OUTPUT);
  pinMode(tip2, OUTPUT);
  pinMode(tip3, OUTPUT);

  // Imprime dados de identificação do módulo
  Serial.println("*** Receptor CAN ***");
  Serial.print("-> ");
  Serial.print(NODE + 1);
  Serial.print("º módulo da ");
  Serial.print(SECTION + 1);
  Serial.println("ª seção da barra.");

  // Configura o Timer1 para gerar o sinal PWM com frequência de 15 Hz
  TCCR1A = 0; // Configura o registrador de controle A do Timer1 para o modo normal
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // Configura o registrador de controle B do Timer1 para o modo CTC com prescaler de 64
  OCR1A = 2499; // Valor para gerar uma frequência de 8 Hz com o prescaler de 64 (16 MHz / (64 * 8) - 1)
  TIMSK1 = (1 << OCIE1A); // Habilita a interrupção do Timer1 quando o valor do registrador de comparação A for alcançado
}

void loop() {
  // Tenta verificar o tamanho do pacote recebido
  int packetSize = CAN.parsePacket();

  if (packetSize) {
    if (!CAN.packetRtr()) { // Só faz o processamento se não for um pacote de requsição
      Serial.print("Recebido: ");
      int i = 0;
      while (CAN.available()) { // Montagem e impressão do vetor recebido
        received[i] = CAN.read();
        Serial.print(received[i]);
        i++;
      }
      Serial.println();
      tipsControl(received); // Chamada da função de controle dos bicos
    }
  }
}

void tipsControl(uint8_t *packet) {
  if (packet[pNODE] == NODE && packet[pSECTION] == SECTION) {
    // Controle bico 1
    if (packet[TIP_ONE] == CLOSED) {
      Serial.println("Bico 1 recebeu o comando para ser fechado. Fechando bico 1...");
      analogWrite(tip1, 0); // Define o duty cycle para 0 (bico fechado)
    } else {
      Serial.println("Bico 1 recebeu o comando para ser aberto. Abrindo bico 1...");
      analogWrite(tip1, 128); // Define o duty cycle para 50% (bico aberto)
    }
    // Controle bico 2
    if (packet[TIP_TWO] == CLOSED) {
      Serial.println("Bico 2 recebeu o comando para ser fechado. Fechando bico 2...");
      analogWrite(tip2, 0); // Define o duty cycle para 0 (bico fechado)
    } else {
      Serial.println("Bico 2 recebeu o comando para ser aberto. Abrindo bico 2...");
      analogWrite(tip2, 128); // Define o duty cycle para 50% (bico aberto)
    }
    // Controle bico 3
    if (packet[TIP_THREE] == CLOSED) {
      Serial.println("Bico 3 recebeu o comando para ser fechado. Fechando bico 3...");
      analogWrite(tip3, 0); // Define o duty cycle para 0 (bico fechado)
    } else {
      Serial.println("Bico 3 recebeu o comando para ser aberto. Abrindo bico 3...");
      analogWrite(tip3, 128); // Define o duty cycle para 50% (bico aberto)
    }
  }
}
// Função de interrupção do Timer1 para gerar o sinal PWM com frequência de 15 Hz
ISR(TIMER1_COMPA_vect) {
  // Nada a fazer aqui, apenas para manter a interrupção ativa
}
