from datetime import datetime, timedelta, timezone

from .api_client import ApiClient
from models import StateVector, FlightAwareFlight
from config import (
    FLIGHTAWARE_BASE_URL,
)

class FlightAwareClient(ApiClient):
    def __init__(self, api_key: str):
        super().__init__(base_url=FLIGHTAWARE_BASE_URL)

        self.api_key = api_key

    def get_headers(self):
        return {"x-apikey": self.api_key}

    def get_flight_details(self, sv: StateVector):
        """
        
        """
        start = (datetime.now(timezone.utc).replace(microsecond=0) - timedelta(hours=24)).isoformat().replace("+00:00", "Z")
        end = (datetime.now(timezone.utc).replace(microsecond=0)).isoformat().replace("+00:00", "Z")

        response = self.make_request(
            path=f"flights/{sv.callsign.strip()}",
            query_params={
                "ident_type": "designator",
                "start": start,
                "end": end,
            }
        )
        
        # TODO: convert to flight objects and validate enroute / timing
