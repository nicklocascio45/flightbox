import json

import paho.mqtt.client as mqtt

from models import NotificationDetails
from utils import logger
from config import (
    NOTIFY_TOPIC,
)


def on_message(client, userdata, message):
    """
    TODO Callback
    """
    pass


def on_publish(client, userdata, mid, reason_code, properties):
    """
    TODO Callback
    """
    logger.info("Published message")


class Mqtt:
    def __init__(self, username: str, password: str, host: str):
        self.username = username
        self.password = password
        self.host = host

        # TODO: add an SSL flag with some more advanced configuration for SSL throughout class
        self.port = 1883
        self.client = mqtt.Client(
            callback_api_version=mqtt.CallbackAPIVersion.VERSION2
        )
        # Hmmmm I wonder what impact of in class vs out of class is here??
        self.client.on_message = on_message
        self.client.on_publish = on_publish

        if self.setup() < 0:
            raise Exception("Problem setting up MQTT client")

    def setup(self) -> int:
        """
        TODO
        """
        self.client.username_pw_set(username=self.username, password=self.password)

        conn_err = self.client.connect(host=self.host, port=self.port)
        if conn_err != mqtt.MQTT_ERR_SUCCESS:
            logger.error(f"Error connecting to broker: {conn_err}")
            return -1

        loop_err = self.client.loop_start()
        if loop_err != mqtt.MQTT_ERR_SUCCESS:
            logger.error(f"Error starting loop thread: {loop_err}")
            return -1
        
        return 0

    def flight_notify(self, details: NotificationDetails):
        payload = details.__dict__
        self.client.publish(
            topic=NOTIFY_TOPIC,
            payload=json.dumps(payload),
        )

    def tear_down(self):
        """
        TODO
        """
        self.client.disconnect()
        self.client.loop_stop()
