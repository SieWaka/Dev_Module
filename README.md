# Dev_Module
Intrucciones paso a paso para poder replicar la fabricacion de uno de los modulos sensor de Sie Waka
# Sie_Waka_Dev
Repositorio para el desarrollo del proyecto SIE WAKA
El proyecto consiste en el desarrollo de un modulo inhalambrico para sensar la calidad del agua.

El proyecto utiliza la tarjeta de desarrollo ESP32, el protocolo MQTT y el servicio de AWS IoT Core.
Para más información esta la docuementación de AWS: https://catalog.us-east-1.prod.workshops.aws/workshops/5b127b2f-f879-48b9-9dd0-35aff98c7bbc/en-US/module1/getting-started

# Instrucciones
## 1. Materiales:
        * Modulo de desarrollo ESP 32.
        * Sensor Analogo de Electroconductividad ECMeter V2 k=1 DFRobots
        * Sensor Analogo de pH phMeter V2 DFRobots
        * Sensor de Temperarura
        * Cada sensor con su respectivo amplificador.
        * Cables, jumpers.
## 2. Servicio AWS - IoT Core
        * Para esto es necesario crear una cuenta en AWS.
        * Una vez finalizado este proceso, nos dirigimos al servicio de AWS IoT Core.
        * Connect-> Connect one device-> Create a new thing
        * Asignamos el nombre a nuestro objeto, para este caso "MyNewESP32".
        * Seleccionamos la plataforma de nuestro SO, para este caso "Windows".
        * Seleccionamis el SDK, para este caso seleccionamos Python.
        * Descargamos el "connection kit" en donde se encuentran los certificados de conexión. Estos luego seran copiados en el archivo "secret.h".
        * Crear un "Policy" con los permisos requeridos. Click en Security-> Policies-> Create
        * Policy name: “MyNewESP32-Policy”
        * Editamos en formato JSON y reemplazamos todo su contenido por el siguiente:
                {
                         "Version": "2012-10-17",
                         "Statement": [
                                {
                                        "Effect": "Allow",
                                        "Action":"iot:Connect,
                                        "Resource": "arn:aws:iot:us-west-2:824254770658:client/MyNewESP32"
                                        },
                                {
                                        "Effect": "Allow",
                                        "Action": "iot:Subscribe",
                                        "Resource": "arn:aws:iot:us-west-2:824254770658:topicfilter/esp32/sub"
                                        },
                                {
                                        "Effect": "Allow",
                                        "Action": "iot:Receive",
                                        "Resource": "arn:aws:iot:us-west-2:824254770658:topic/esp32/sub"
                                        },
                                {
                                        "Effect": "Allow",
                                        "Action": "iot:Publish",
                                        "Resource": "arn:aws:iot:us-west-2:824254770658:topic/esp32/pub"
                                        }
                                ]
                        } 
        * "us-west-2:824254770658" s la region donde este oprando nuestro servidor de AWS y "client/MyNewESP32" el nombre de nuestro objeto. Reemplazar segùn el caso.
        * Por ultimo agregamos la "Policy" personalizada a los certificados de nuestro objeto
        * Manage -> All devices -> Things
        * Seleccionamos nuestro objeto, luego en la seccion de "Certificates" seleccionamos el certificado actual.
        * En Policies, damos click en "Attach policies" y leccionamos nuestra "Policy" perzonalizada, ya que la que aparece ahi es una "Policy" que se genera automaticamente al crear nuestro objeto.
## 5. Conexión ESP32 al Pc
       * Seguir las instrucciones del siguiente link para configurar el IDE de Arduino y lograr programar y esstablecer comunicación serial entre el Pc y la tarjeta ESP32. 
       https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
## 4. Archivo .ino
        * Contamos con dos archivos .ino que se pueden cargar a la tarjeta ESP32 mediante el ide de Arduino. Para lograr esto primero hay que hacer una pequeña configuración en el IDE de Arduino, siguiendolos pasos de este tutorial: https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
        * Uno de los archivos .ino es para poder probar los sesnores unicamente sin establecer la conexión con el servidor de AWS, mientras el otro ya es desarrollo completo donde contamos con el proceso de conexión a internet via wi-fi y conexión al servidor de AWS.
        En el archivo "secrets.h" reemplazamos la siguiente información:
                THINGNAME: MyNewESP32
                WIFI_SSID: El nombre de la red Wi-Fi a conectarse, no deberia incluir espacios o caracteres especiales.
                WIFI_PASSWORD: La contraseña de la red Wi-Fi a la cual nos vamos a conectar.
                AWS_IOT_ENDPOINT: El endpoint asociado a nuestra cuenta de AWS IoT Core.
                AWS_CERT_CA: Amazon Root CA 1 -> https://www.amazontrust.com/repository/AmazonRootCA1.pem
                AWS_CERT_CRT: contenido de ESP32.cert.pem
                AWS_CERT_PRIVATE: contenido de ESP32.private.key
        Estos ultimos dos se encunetran en el "connection kit" que se descargoal final del procesode creación de nuestro objeto.

        * Fnalmete cargamos el archivo .ino correspondiente "ESP32_Water_monitor.ino", seleccionamos el puerto donde esta conectada nuestra tarjeta, mantenmos precionado el botón Boot y esperamosque cargue nuestro programa. Despues desde el Serial Monitor podemos visualizar el proceso de conexión y la información enviada o recibida desde el servidor de AWS.

