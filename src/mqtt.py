import random
from paho.mqtt import client as mqtt_client
import os
import time

broker = '127.0.0.1'
port = 1883
topic = "cmd"
# generate client ID with pub prefix randomly
client_id = f'python-mqtt-{random.randint(0, 100)}'

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def publish(client, msg):
    result = client.publish(topic, msg)
    status = result[0]
    if status == 0:
        print(f"Py: Sent `{msg}` to topic `{topic}`")
    else:
        print(f"Py: Failed to send message to topic {topic}")

def run():
    # run mosquitto broker in terminal
    os.system("start cmd /c D:/Coding/CPP/Alphabot/lib/mosquitto/build/src/Debug/mosquitto.exe")
    time.sleep(2)

    client = connect_mqtt()
    client.loop_start()

    publish(client, "IDLE")

    message = ""
    prevMessage = "IDLE"

    while True:
        with open("D:/Coding/CPP/Alphabot/docs/message.txt", "r") as f:
            message = f.read()
        # если прищло сообщение или пришло сообщение IDLE и предыдущее тоже IDLE
        if message == "" or (message == "IDLE" and prevMessage == "IDLE"):
            continue
        # если пришло сообщение EXIT то выходим из цикла
        if message == "EXIT":
            break
        # Если полезное сообщение
        else:
            publish(client, message) # отправляем его
            prevMessage = message    # запоминаем его
            message = ""             # обнуляем текущее сообщение
            time.sleep(0.1) # немного задержки чтобы не засорять канал
            
    client.loop_stop()

if __name__ == '__main__':    
    run()
