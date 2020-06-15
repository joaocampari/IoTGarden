#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Declaração de parametros de entradas
#define RELAY_PIN D2
#define HYGROMETER_PIN A0

const char* ssid = "";
const char* password = "";
const char* mqtt_server = "mqtt.eclipse.org";
int mqtt_port = 1883;
long lastVerification = 0;

// Declaração de variaveis globais para parametros de irrigação
int IRRIGATION_MILLISECONDS;
int SUITABLE_MOISTURE;
int VERIFICATION_TIME;

WiFiClient espClient;
PubSubClient client(espClient);

/**
 * Metódo responsável por retornar a umidade
 */
float getMoisture(void)
{
    int directValue;
    float moisturePercentage;
 
    directValue = analogRead(HYGROMETER_PIN);
    moisturePercentage = ((978- (float)directValue) / 978) * 100;
     
    Serial.print("Umidade atual: ");
    Serial.println(moisturePercentage);
 
    return moisturePercentage;
}

/**
 * Inicialização
 */
void setup()
{  
    Serial.begin(9600);
    Serial.println("Inicializando");
    
    setupWifi();
    
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
  

    // Declaração de valores padrões para irrigação do solo ( caso não sete no broker MQTT)
    setIrrigationVariables(1, 30, 1);

    
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
}

/**
 * Metódo responsavel por definir as variaveis do projeto
 * Tempo de irrigação, Umidade adequada, Tempo de verificação
 */
void setIrrigationVariables(int irrigationSeconds, int suitableMoisture, int verificationTime) 
{
    Serial.print("Setando variaveis de irrigação: ");
    Serial.print("[ Segundos de irrigação:");
    Serial.print(irrigationSeconds);
    Serial.print("], [ Umidade adequada:");
    Serial.print(suitableMoisture);
    Serial.print("], [ Tempo de verificação:");
    Serial.print(verificationTime);
    Serial.println("]");
    
    IRRIGATION_MILLISECONDS = irrigationSeconds * 1000;
    SUITABLE_MOISTURE = suitableMoisture;
    VERIFICATION_TIME = verificationTime * 1000;
}

/**
 * Loop default
 */
void loop()
{
    if (!client.connected())
    {
      reconnect();
    }
    
    client.loop();
    long now = millis();

    if (now - lastVerification > VERIFICATION_TIME)
    {
      lastVerification = now;
      Serial.println("Efetuando leitura do solo e necessidade de irrigamento");
      int moisture = (int)getMoisture();
      verifyIrrigate(moisture);

      char cstr[16];
      itoa(moisture, cstr, 10);
      Serial.println("Enviando ao MQTT");   
      client.publish("hortainteligentedanijoao/humidity", cstr);
    }
}

/**
 * Metodo que efetua o irrigamento
 */
void irrigate()
{
    reconnect();
    Serial.print("Irrigamento iniciado, quantidade de segundos de irrigação: ");
    Serial.println(IRRIGATION_MILLISECONDS);
      
    digitalWrite(RELAY_PIN, HIGH);
    delay(IRRIGATION_MILLISECONDS);
    digitalWrite(RELAY_PIN, LOW);
    reconnect(); 
    Serial.println("Irrigamento finalizado");    
}

/**
 * Metodo que verifica a necessidade de irrigação de acordo com os parametros definidos.
 */
void verifyIrrigate(int moisturePercentage)
{
    if (moisturePercentage > 0 && moisturePercentage < SUITABLE_MOISTURE) 
    {
      irrigate();
      verifyIrrigate((int)getMoisture());
    } 
}

/**
 * Inicializa o WIFI
 */
void setupWifi() 
{
  //---- Conecta ao Wi-Fi ----
    WiFi.begin(ssid, password);
    //---- Enquanto estiver tentando conectar ----
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.println("Conectando ao WiFi..");
    }
    Serial.println("Conectado");
    Serial.print("Endereço de IP: ");
    Serial.println(WiFi.localIP());
}

/**
 * Metodo que efetua a leitura do sub. (callback)
 */
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Mensagem recebida[");
  Serial.print(topic);
  Serial.println("] ");
  
  if (strcmp(topic,"hortainteligentedanijoao/verificationseconds") == 0) 
  {
     payload[length] = '\0';
      setIrrigationVariables(IRRIGATION_MILLISECONDS/1000, SUITABLE_MOISTURE, atoi((char *)payload)); 
  }

  if (strcmp(topic,"hortainteligentedanijoao/irrigationmilli") == 0) 
  {
      payload[length] = '\0';
      setIrrigationVariables(atoi((char *)payload), SUITABLE_MOISTURE, VERIFICATION_TIME/1000); 
  }

  if (strcmp(topic,"hortainteligentedanijoao/suitablemoisture") == 0) 
  {
      payload[length] = '\0';
      setIrrigationVariables(IRRIGATION_MILLISECONDS/1000, atoi((char *)payload), VERIFICATION_TIME/1000); 
  }

  if (strcmp(topic,"hortainteligentedanijoao/solenoid") == 0) {
      payload[length] = '\0';
      if (atoi((char *)payload) == 1)
      {
        irrigate();
      } 
  }

}

/**
 * Metodo responsavel pela conectividade com MQTT.
 */
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("joaomackenzieiot"))
    {
      Serial.println("Conectado");
      
      // Se inscreve nos tópicos
      client.subscribe("hortainteligentedanijoao/verificationseconds");
      client.subscribe("hortainteligentedanijoao/irrigationmilli");
      client.subscribe("hortainteligentedanijoao/suitablemoisture");
      client.subscribe("hortainteligentedanijoao/solenoid");
    } 
    else
    {
      Serial.print("Erro:");
      Serial.print(client.state());
      Serial.println(" reconectando em 5 segundos");
      delay(5000);
    }
  }
}
