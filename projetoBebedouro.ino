/******************************************************************
//  Arquivo: projetoBebedouro.ino
//  Autores: Michel Lima, Altamir Junio, Carlos Vinicius, Keucilene Coelho e Shangella Candido.
//
//  Projeto: Bebedouro Inteligente
//
//  Data: Maio de 2021
//
//  Descricao:  Este projeto e` um prototipo desenvolvido para a disciplina
//              de Internet das Coisas do Curso Superior de Analise e Desenvolvimento
                de Sistemas oferecido pelo IFRO - Campus Ji-Parana.
                
                Este prototipo objetiva exemplicar/provar o potencial da solucao propostas
                pelo grupo. Para tal, foram desenvolvidas as funcoes de conexao wifi para configuracao do
                sensor ultrassonico e a parametrizacao dos componentes do sistema.

*********************************************************************/

#include <WiFi.h>
#include <Ultrasonic.h>

// Define oS pinoS do sensor ultrassonico
#define PINO_SENSOR_TRIGGER 12 
#define PINO_SENSOR_ECHO 13

// Define o pino da valvula
#define PINO_VALVULA 6

#define NIVEL_MINIMO 400 //Nivel minimo em cm (distancia do sensor para o nivel da agua)
#define NIVEL_MAXIMO 20 // Nivel minimo em cm (distancia do sensor para o nivel da agua)

const char* ssid     = "ESP32_IFRO_IOT"; // Constante que define o nome da rede
const char* password = "ifro2021"; //  Constante que define a senha da rede

int statusSensor = 0;
int nivelHidraulico = 0;

HC_SR04 sensor1(PINO_SENSOR_TRIGGER, PINO_SENSOR_ECHO)
WiFiServer server(80);

void setup()
{
    WiFi.begin(ssid, password); // Inicia a rede Wifi

    while (WiFi.status() != WL_CONNECTED) { // Enquanto o status for diferente de WL_CONNECTED, ou seja, enquanto nao haver alguem connectado a placa...
        delay(500); // ... esperamos 500ms e checamos novamente
    }

    server.begin(); // Inicia o "servidor" wifi

}

int value = 0;

void loop(){

/*
Esta sessao do codigo se refere a ativacao e a desativacao de um sensor via HTTP requests.

 http://meuIP/sensor/ativar ativa o sensor
 http://meuIP/sensor/desativar desativa o sensor

Bibliotecas usadas desenvolvidas por Tom Igoe e Jan Hendrik Berlin
 
 */
 WiFiClient client = server.available();   // O servidor wi-fi aguarda por clientes

  if (client) {                             // Se um cliente connecta...
    
    while (client.connected()) {            // Enquanto um cliente estiver connectado...
      if (client.available()) {             // Comecamos a esperar por requests, sabemos que existe Se houver um byte para ler desse client...
        // Para mais detalhes sobre HTTP Requests e Responses, checar documento do trabalho
        String currentLine = ""; // String para receber os dados do http.request enviado pelo cliente
        char c = client.read();             // ... lemos o byte
        if (c == '\n') {                    // se esse byte for um new line (ascii 10)..

          // cheacamos se a linha atual eh branca, pois caso seja, teremops dois \n em seguida,
          // consequentemente o fim de uma HTTP request, entao enviamos a HTTP response:
          if (currentLine.length() == 0) {
            // HTTP responses sempre comecam com o response code e o content-type para que o client entenda o que esta vindo
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println(); // E uma linha em branco logo em seguida (lembrar de explicar como funciona a comunicacao cliente-servidor no documento do trabalho)

            // O conteudo do response dando as opcoes para o usuario de desativar ou ativar o sensor
            client.print("Clique <a href=\"/sensor/ativar">aqui</a> para ativar o sensor.<br>");
            client.print("Click <a href=\"/sensor/desativar">aqui</a> para desativar o sensor.<br>");

            // O response terminar com uma linha em branco:
            client.println();
            
            break;// break pra fora do loop
          } else {    // Se a linha recebida apos \n nao for branca...
            currentLine = ""; // ...resetamos currentLine
          }
          
        } else if (c != '\r') {  // Se nao recebermos um \n, nem um carriage return (tecla enter/paragrafo)
          currentLine += c;      // Adicionamos aos dados enviados pelo client para que possamos...
        }

        // ...checar para onde foi o request:
        if (currentLine.endsWith("GET /sensor/ativar")) { // Se o currentLine (que neste momento eh o cabecalho do request) terminar em ativar
         statusSensor = 1;
        }
        if (currentLine.endsWith("GET /sensor/desativar")) {
         statusSensor = 0;
        }
      }
    }
    
    client.stop(); // fecha a conexao:
  }

  if(sensorStatus = 1){ // Se o sensor esta ativado
    
    nivelHidraulico = sensor1.distance(); // Lemos o nivel hidraulico
    nivelHidraulico = nivelHidraulico/58; // Convertemos para cm
    
    if (nivelHidraulico < NIVEL_MAXIMO) { //Pode parecer o contrario, mas aqui estamos checando se o nivel atual eh maior que o nivel maximo
      digitalWrite(PINO_VALVULA, LOW); //Desativamos a valvula
    } else if (nivelHidraulico > NIVEL_MINIMO){ //Pode parecer o contrario, mas aqui estamos checando se o nivel atual eh menor que o nivel minimo
      digitalWrite(PINO_VALVULA, HIGH); //Ativamos a valvula
    }
  }
}
