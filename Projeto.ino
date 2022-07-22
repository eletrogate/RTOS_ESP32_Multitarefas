#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
LiquidCrystal lcd(22,23,5,18,19,21);

TaskHandle_t Task1;
TaskHandle_t Task2;

// O fio de dados é conectado no pino digital 33 no ESP32
#define ONE_WIRE_BUS 33
 
// Prepara uma instância oneWire para comunicar com qualquer outro dispositivo oneWire
OneWire oneWire(ONE_WIRE_BUS);  
 
// Passa uma referência oneWire para a biblioteca DallasTemperature
DallasTemperature sensors(&oneWire);

// Pinos dos componentes do projeto
const int led = 2;
const int buzzer = 4;
const int PushButton = 15;
const int rele = 13;

void setup() {
  Serial.begin(115200); 
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(PushButton, INPUT);
  pinMode(rele, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();

  // Cria uma tarefa que será executada na função Task1code(), com  prioridade 1 e execução no núcleo 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Função da tarefa */
                    "Lembrete",  /* nome da tarefa */
                    10000,       /* Tamanho (bytes) */
                    NULL,        /* parâmetro da tarefa */
                    1,           /* prioridade da tarefa */
                    &Task1,      /* observa a tarefa criada */
                    0);          /* tarefa alocada ao núcleo 0 */                  
  delay(500); 

  // Cria uma tarefa que será executada na função Task2code(), com  prioridade 1 e execução no núcleo 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Função da tarefa */
                    "Ventilador",/* nome da tarefa */
                    10000,       /* Tamanho (bytes) */
                    NULL,        /* parâmetro da tarefa */
                    1,           /* prioridade da tarefa */
                    &Task2,      /* observa a tarefa criada */
                    1);          /* tarefa alocada ao núcleo 1 */
    delay(500); 
}

//Task1code: Lembrete para beber água
void Task1code( void * pvParameters ){
  for(;;){
    vTaskDelay(5000/portTICK_PERIOD_MS); // Tempo de espera do lembrete (recomendado: 1800000 (30 min))
    lcd.setCursor(0,0);
    lcd.print("HIDRATE-SE!!!!"); // Escreve no Display

    lcd.setCursor(0,1); 
    lcd.print ("BEBA AGUA AGORA!");

    // Botão para indicar se a pessoa já bebeu a água
    int botao = digitalRead(PushButton);

    // Toca o buzzer
    digitalWrite(buzzer, HIGH);
    vTaskDelay(500/portTICK_PERIOD_MS);        // ...for 1 sec
    digitalWrite(buzzer, LOW);     // Stop sound...
    vTaskDelay(500/portTICK_PERIOD_MS); 
    digitalWrite(buzzer, HIGH); // Send 1KHz sound signal...
    vTaskDelay(500/portTICK_PERIOD_MS);        // ...for 1 sec
    digitalWrite(buzzer, LOW);     // Stop sound...

    for(;;){ // Loop infinito
      botao = digitalRead(PushButton);
      if(botao == HIGH){ // Se a pessoa já bebeu a água...
        break; // Sai do loop
      }
      else{ // Se não...
        // Led pisca
        digitalWrite(led, HIGH);
        vTaskDelay(700/portTICK_PERIOD_MS);
        digitalWrite(led, LOW);
        vTaskDelay(700/portTICK_PERIOD_MS);
      }
    }

    lcd.clear(); // Limpa Display
    lcd.setCursor(0,0);
    lcd.print("PEDRA NOS RINS, "); // Escreve no Display
    lcd.setCursor(0,1); 
    lcd.print ("VAO EMBORA!!!");

    vTaskDelay(5000/portTICK_PERIOD_MS);

    lcd.clear();
  } 
}

//Task2code: Controle do ventilador
void Task2code( void * pvParameters ){
    for(;;){ // Loop infinito
      // Manda comando para ler temperaturas
      sensors.requestTemperatures(); 
     
      // Escreve a temperatura em Celsius
      Serial.print("Temperatura: ");
      Serial.print(sensors.getTempCByIndex(0));
      Serial.println(" graus");

      float temp = sensors.getTempCByIndex(0);

      if(temp>30){ // Se temperatura for maior que 30 graus...
        digitalWrite(rele, HIGH); // Liga ventilador
      }

      if(temp<29){ // Se temperatura for menor que 29 graus...
        digitalWrite(rele, LOW); // Desliga ventilador
      }
  }
}

void loop() {
  
}