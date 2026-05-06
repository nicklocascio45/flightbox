import json
import os
import time
import sys
from datetime import datetime

from dotenv import load_dotenv

from clients import OpenSkyClient, FlightAwareClient
from utils.logger import logger
from config import (
    AOI_LAT_MIN,
    AOI_LAT_MAX,
    AOI_LNG_MIN,
    AOI_LNG_MAX,
    DETECTION_INTERVAL_SECONDS,
)


def detect(results: dict, opensky_client: OpenSkyClient, flightaware_client: FlightAwareClient) -> dict:
    state_vectors = opensky_client.search_box(
        lat_min=AOI_LAT_MIN,
        lng_min=AOI_LNG_MIN,
        lat_max=AOI_LAT_MAX,
        lng_max=AOI_LNG_MAX,
    )

    result_key = datetime.now().strftime("%Y-%m-%d %H")
    for sv in state_vectors:
        logger.info(f"{sv.callsign.strip()} should be in your field of view")

        # flightaware_client.get_flight_details(sv=sv)

        # push detections to a results dictionary that we can inspect in logs for analysis
        if result_key not in results:
            results[result_key] = [sv.callsign.strip()]
        else:
            results[result_key].append(sv.callsign.strip())

    return results


def orchestrate_detection():
    load_dotenv(dotenv_path="config/.env")
    OPENSKY_CLIENT_ID = os.getenv("OPENSKY_CLIENT_ID")
    OPENSKY_CLIENT_SECRET = os.getenv("OPENSKY_CLIENT_SECRET")
    FLIGHTAWARE_API_KEY = os.getenv("FLIGHTAWARE_API_KEY")

    if not all([OPENSKY_CLIENT_ID, OPENSKY_CLIENT_SECRET, FLIGHTAWARE_API_KEY]):
        logger.error("MISSING ENV VAR")
        return

    opensky_client = OpenSkyClient(
        client_id=OPENSKY_CLIENT_ID,
        client_secret=OPENSKY_CLIENT_SECRET,
    )
    flighaware_client = FlightAwareClient(
        api_key=FLIGHTAWARE_API_KEY,
    )

    results = {}
    prev_hour = datetime.now().hour
    while True:
        logger.info("Running detection...")

        # high-level exception catching for the time being
        try:
            results = detect(
                results=results,
                opensky_client=opensky_client,
                flightaware_client=flighaware_client
            )
        except Exception as e:
            logger.error(f"Exception occurred during detection: {e}")

        curr_hour = datetime.now().hour
        if curr_hour > prev_hour or (curr_hour == 0 and prev_hour != 0):
            prev_hour = curr_hour
            logger.info(f"Results: {json.dumps(results)}")

        logger.info("Detection complete, sleeping...")
        time.sleep(DETECTION_INTERVAL_SECONDS)


def _test_functionality():
    """
    Temp function just to try out new things
    """
    from models import FlightAwareFlight

    with open("clients/temp.json") as f:
        data = json.loads(f.read())["flights"][0]

    flight = FlightAwareFlight(**data)
    print(flight.origin.code)
    print(flight.destination.code)

    import csv
    with open("data/planes.dat", "r") as csvfile:
        plane_reader = csv.reader(csvfile, delimiter=",")

    with open("data/airlines.dat", "r") as csvfile:
        airline_reader = csv.reader(csvfile, delimiter=",")

    """
    Airline DB:
        id
        name
        alias
        iata - CAN MATCH (operator_iata)
        icao - CAN MATCH (operator_icao)
        callsign
        country
        active ("Y" or "N")

    Plane DB:
        name
        iata
        icao - CAN MATCH (aircraft_type)
    """


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "test":
        _test_functionality()
    else:
        orchestrate_detection()
