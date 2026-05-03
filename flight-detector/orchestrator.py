import json
import os
import time
from datetime import datetime

from dotenv import load_dotenv

from clients import OpenSkyClient
from detection.step_prediction import step_predict_intersection
from utils.logger import logger
from config import (
    SEARCH_LAT_MIN,
    SEARCH_LAT_MAX,
    SEARCH_LNG_MIN,
    SEARCH_LNG_MAX,
    AOI_LAT_MIN,
    AOI_LAT_MAX,
    AOI_LNG_MIN,
    AOI_LNG_MAX,
    DETECTION_INTERVAL_SECONDS,
)


def detect(results: dict, opensky_client: OpenSkyClient) -> dict:
    state_vectors = opensky_client.search_box(
        lat_min=SEARCH_LAT_MIN,
        lng_min=SEARCH_LNG_MIN,
        lat_max=SEARCH_LAT_MAX,
        lng_max=SEARCH_LNG_MAX,
    )

    result_key = datetime.now().strftime("%Y-%m-%d %H")
    for sv in state_vectors:
        intersect_time = step_predict_intersection(
            sv=sv,
            lat_min=AOI_LAT_MIN,
            lat_max=AOI_LAT_MAX,
            lng_min=AOI_LNG_MIN,
            lng_max=AOI_LNG_MAX,
        )

        if intersect_time is not None:
            logger.info(f"{sv.callsign.strip()} is predicted to cross into your field of view in {intersect_time} seconds")
            result_record = {sv.callsign.strip(): intersect_time}
            if result_key not in results:
                results[result_key] = [result_record]
            else:
                results[result_key].append(result_record)

    return results


def orchestrate_detection():
    load_dotenv(dotenv_path="config/.env")
    OPENSKY_CLIENT_ID = os.getenv("OPENSKY_CLIENT_ID")
    OPENSKY_CLIENT_SECRET = os.getenv("OPENSKY_CLIENT_SECRET")

    if not all([OPENSKY_CLIENT_ID, OPENSKY_CLIENT_SECRET]):
        logger.error("MISSING ENV VAR")
        return

    opensky_client = OpenSkyClient(
        client_id=OPENSKY_CLIENT_ID,
        client_secret=OPENSKY_CLIENT_SECRET
    )

    results = {}
    prev_hour = datetime.now().hour
    while True:
        logger.info("Running detection...")

        # high-level exception catching for the time being
        try:
            results = detect(results=results, opensky_client=opensky_client)
        except Exception as e:
            logger.error(f"Exception occurred during detection: {e}")

        curr_hour = datetime.now().hour
        if curr_hour > prev_hour or (curr_hour == 0 and prev_hour != 0):
            prev_hour = curr_hour
            logger.info(f"Results: {json.dumps(results)}")

        logger.info("Detection complete, sleeping...")
        time.sleep(DETECTION_INTERVAL_SECONDS)


if __name__ == "__main__":
    orchestrate_detection()
