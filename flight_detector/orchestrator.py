import os

from dotenv import load_dotenv

from clients import OpenSkyClient
from config import (
    SEARCH_LAT_MIN,
    SEARCH_LAT_MAX,
    SEARCH_LNG_MIN,
    SEARCH_LNG_MAX,
)


def detect():
    load_dotenv(dotenv_path="config/.env")
    OPENSKY_CLIENT_ID = os.getenv("OPENSKY_CLIENT_ID")
    OPENSKY_CLIENT_SECRET = os.getenv("OPENSKY_CLIENT_SECRET")

    if not all([OPENSKY_CLIENT_ID, OPENSKY_CLIENT_SECRET]):
        print("MISSING ENV VAR")
        return

    opensky_client = OpenSkyClient(
        client_id=OPENSKY_CLIENT_ID,
        client_secret=OPENSKY_CLIENT_SECRET
    )
    opensky_client.search_box(
        lat_min=SEARCH_LAT_MIN,
        lng_min=SEARCH_LNG_MIN,
        lat_max=SEARCH_LAT_MAX,
        lng_max=SEARCH_LNG_MAX,
    )


if __name__ == "__main__":
    detect()
