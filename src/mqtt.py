import random
from paho.mqtt import client as mqtt_client
import os
import time
import socket

# Класс для работы с MQTT
class MQTTClient:
    # Конструктор
    def __init__(self, broker, port, topic):
        self.broker = broker
        self.port = port
        self.topic = topic
        self.client_id = f'python-mqtt-{random.randint(0, 100)}'

        absolute_path = os.path.dirname(__file__)
        # Удаляем 4 последних символа в пути, чтобы получить путь к корню проекта
        absolute_path = absolute_path[:-4]
        absolute_path = absolute_path.replace("\\", "/")
        conf_path = absolute_path + "/docs/mosquittoWeb.conf"
        # Открываем файл конфигурации. Если не существует, то создаем его
        with open(conf_path, "a+") as f:
            pass
        # Вписываем listener 1883 {ip}\n allow_anonymous true
        conf = "listener 1883 " + ip + "\n" + "allow_anonymous true"
        # Записываем изменения в файл конфигурации
        with open(conf_path, "w") as f:
            f.write(conf)

        os.system(f"start cmd as admin /c mosquitto -c \"{conf_path}\" -v")
        time.sleep(1)

        self.messagePath = absolute_path + "/.temp/message.txt"

        self.client = self.connect()
        self.client.loop_start()

        self.publish("{\"cmd\" : \"stop\"}")        
    
    # Подключение к брокеру
    def connect(self) -> mqtt_client:
        def on_connect(client, userdata, flags, rc):
            if rc == 0:
                print("Connected to MQTT Broker!")
            else:
                print("Failed to connect, return code %d\n", rc)

        self.client = mqtt_client.Client(self.client_id)
        self.client.on_connect = on_connect
        self.client.connect(self.broker, self.port)
        return self.client

    # Отправка сообщения
    def publish(self, msg):
        result = self.client.publish(self.topic, msg)
        status = result[0]
        if status == 0:
            print(f"Py: Sent `{msg}` to topic `{self.topic}`")
        else:
            print(f"Py: Failed to send message to topic {self.topic}")

    # Запуск клиента и проверка наличия сообщений
    def run(self):
        message = ""
        prevMessage = "{\"cmd\" : \"stop\"}"
        while True:
            # Пробуем открыть файл с сообщением если его нет, то создаем его
            try:
                with open(self.messagePath, "r") as f:
                    message = f.read()
            except:
                with open(self.messagePath, "w") as f:
                    f.write("")
                continue
            # если прищло сообщение или пришло сообщение IDLE и предыдущее тоже IDLE
            if message == "" or (message.find("stop") != -1 and prevMessage.find("stop") != -1):
                continue
            # если пришло сообщение EXIT то выходим из цикла
            if message == "EXIT":
                print("EXIT")
                break
            # Если полезное сообщение
            else:
                self.publish(message) # отправляем его
                prevMessage = message    # запоминаем его
                message = ""             # обнуляем текущее сообщение
                time.sleep(0.1) # немного задержки чтобы не засорять канал

        self.client.loop_stop()

if __name__ == '__main__':
    # получаем IP-адресс компьютера
    ip = socket.gethostbyname(socket.gethostname())
    print("IP: " + ip)
    client = MQTTClient(ip, 1883, 'abot/command/alex')
    client.run()
