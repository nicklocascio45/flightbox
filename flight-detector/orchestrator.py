import os
import time
import sys

from dotenv import load_dotenv

from clients import OpenSkyClient, FlightAwareClient, PlanespottersClient
from comms import Mqtt
from utils import logger, FlightCache
from models import NotificationDetails
from config import (
    AOI_LAT_MIN,
    AOI_LAT_MAX,
    AOI_LNG_MIN,
    AOI_LNG_MAX,
    ALT_AOI_LAT_MIN, # temp
    ALT_AOI_LAT_MAX, # temp
    ALT_AOI_LNG_MIN, # temp
    ALT_AOI_LNG_MAX, # temp
    DETECTION_INTERVAL_SECONDS,
)

flight_cache = FlightCache()

"""
We should at least start thinking about this in terms of a stateful
object. I think ideal state would be to have this thing always-on, but
with modifiable behavior based on device inputs. For example, we don't 
want to burn through API calls when I'm not around. So when I get home I
can press a button on my device and that will start firing API calls
"""

def _show_details(details: NotificationDetails, photo_url: str):
    logger.info(f"{details.callsign} should be in your field of view")
    logger.info(f"{details.operator} {details.aircraft_type}: {details.origin} -> {details.destination}")
    logger.info(f"Image: {photo_url}")


def detect(
        opensky_client: OpenSkyClient, 
        flightaware_client: FlightAwareClient, 
        planespotters_client: PlanespottersClient, 
        mqtt: Mqtt
    ):
    """
    Run the detection process
    """
    state_vectors = opensky_client.search_box(
        lat_min=ALT_AOI_LAT_MIN,
        lng_min=ALT_AOI_LNG_MIN,
        lat_max=ALT_AOI_LAT_MAX,
        lng_max=ALT_AOI_LNG_MAX,
    )

    for sv in state_vectors:
        if sv.callsign in flight_cache.detected:
            continue

        flight_cache.detected.append(sv.callsign)
        flight = flightaware_client.get_flight_details(sv=sv)
        if flight is not None:
            photo_url = planespotters_client.get_plane_photo(sv=sv)
            _show_details(flight.notification_details, photo_url)
            mqtt.flight_notify(flight.notification_details)


def orchestrate_detection():
    load_dotenv(dotenv_path="config/.env")

    # TODO: make this whole initialization piece a little less disgusting?
    OPENSKY_CLIENT_ID = os.getenv("OPENSKY_CLIENT_ID")
    OPENSKY_CLIENT_SECRET = os.getenv("OPENSKY_CLIENT_SECRET")
    FLIGHTAWARE_API_KEY = os.getenv("FLIGHTAWARE_API_KEY")
    MQTT_USERNAME = os.getenv("MQTT_USERNAME")
    MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")
    MQTT_HOST = os.getenv("MQTT_HOST")

    if not all([OPENSKY_CLIENT_ID, OPENSKY_CLIENT_SECRET, FLIGHTAWARE_API_KEY, MQTT_USERNAME, MQTT_PASSWORD, MQTT_HOST]):
        logger.error("MISSING ENV VAR")
        return

    opensky_client = OpenSkyClient(
        client_id=OPENSKY_CLIENT_ID,
        client_secret=OPENSKY_CLIENT_SECRET,
    )
    flightaware_client = FlightAwareClient(
        api_key=FLIGHTAWARE_API_KEY,
    )
    planespotters_client = PlanespottersClient()
    mqtt = Mqtt(
        username=MQTT_USERNAME,
        password=MQTT_PASSWORD,
        host=MQTT_HOST,
    )

    while True:
        # High-level exception catching for the time being
        try:
            detect(
                opensky_client=opensky_client,
                flightaware_client=flightaware_client,
                planespotters_client=planespotters_client,
                mqtt=mqtt,
            )
        except Exception as e:
            logger.error(f"Exception occurred during detection: {e}")

        time.sleep(DETECTION_INTERVAL_SECONDS)


def _test_functionality():
    """
    Temp function just to try out new things
    """
    pass


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        _test_functionality()
    else:
        orchestrate_detection()
