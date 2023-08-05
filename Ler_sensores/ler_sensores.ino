#include <SPI.h>
#include <SD.h>

const int chipSelect = 10;
File dataFile;

float vazao;
int contagem_pulsos;
const int SENSOR = A0;

void incremento_pulsos() {
  contagem_pulsos++;
}

void setup() {
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), incremento_pulsos, RISING);
  Serial.begin(9600);

  if (!SD.begin(chipSelect)) {
    Serial.println("Falha ao inicializar o cartão SD");
    return;
  }
}

void loop() {
  contagem_pulsos = 0;
  sei();
  delay(1000);
  cli();

  vazao = contagem_pulsos / 7.5;

  Serial.print("Vazao: ");
  Serial.print(vazao);
  Serial.println(" L/min");

  Serial.print("Pressao: ");
  float pressao_MPa = (analogRead(SENSOR) / 1024.0 - 0.1) / 0.75;
  float pressao_Bar = pressao_MPa * 10;
  Serial.print(pressao_Bar);
  Serial.println(" Bar");

  // Abre o arquivo para escrita
  dataFile = SD.open("dados.txt", FILE_WRITE);

  if (dataFile) {
    // Escreve os dados no arquivo
    dataFile.print("Vazao: ");
    dataFile.print(vazao);
    dataFile.println(" L/min");
    dataFile.print("Pressao: ");
    dataFile.print(pressao_Bar);
    dataFile.println(" Bar");

    // Fecha o arquivo
    dataFile.close();

    Serial.println("Dados salvos no cartão SD.");
  } else {
    Serial.println("Erro ao abrir o arquivo.");
  }

  delay(500);
}
