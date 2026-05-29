import requests

from models import StateVector
from utils import logger
from config import (
    PLANESPOTTERS_BASE_HEX_URL
)


class PlanespottersClient:
    def __init__(self):
        self.base_url = PLANESPOTTERS_BASE_HEX_URL

    def get_plane_photo(self, sv: StateVector) -> str | None:
        response = requests.get(
            url=f"{self.base_url}/{sv.icao24}",
            headers={
                "User-Agent": "MyFlightTracker/1.0 (nicoloc4545@gmail.com)",
            }
        )
        response.raise_for_status()
        response = response.json()

        error = response.get("erorr")
        if error is not None:
            logger.warning(f"Error while getting photo for {sv.icao24}: {error}")
            return

        photos = response["photos"]
        if len(photos) > 0:
            photo = photos[0]["thumbnail_large"]["src"]
            return photo
        else:
            logger.warning(f"No photos found for {sv.icao24}")
            return None
