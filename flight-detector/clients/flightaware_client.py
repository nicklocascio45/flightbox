import csv
from datetime import datetime, timedelta, timezone

import requests

from models import StateVector, FlightAwareFlight, Plane, Airline
from utils import logger
from config import (
    FLIGHTAWARE_BASE_URL,
    FLIGHTAWARE_DAILY_LIMIT,
)


class FlightAwareClient:
    def __init__(self, api_key: str):
        self.api_key = api_key

        self.base_url = FLIGHTAWARE_BASE_URL
        self.planes = self._load_planes()
        self.airlines = self._load_airlines()
        self.callsign_cache = []

        self.daily_request_count = 0
        self.request_tracking_day = datetime.today().date()

    def _load_planes(self) -> list[Plane]:
        """
        Helper to read in detailed plane data for
        more human-readable reporting
        """
        with open("data/planes.dat", "r") as f:
            reader = csv.DictReader(f, delimiter=",")
            return [Plane(**row) for row in reader]

    def _load_airlines(self) -> list[Airline]:
        """
        Helper to read in detailed airline data for
        more human-readable reporting
        """
        with open("data/airlines.dat", "r") as f:
            reader = csv.DictReader(f, delimiter=",")
            return [Airline(**row) for row in reader]

    def _check_limit_reached(self) -> bool:
        """
        Helper responsible for resetting daily request limit and
        checking that we're good on requests for the day

        NOTE: This is limited to a per-run basis. If we kill a run
        at 29 requests and restart, our request count for the day 
        also resets. API doesn't readily provide daily request count
        in the response unfortunately
        """
        if datetime.today().date() > self.request_tracking_day:
            logger.info("Resetting daily FlightAware request count")
            self.request_tracking_day = datetime.today().date()
            self.daily_request_count = 0
            return False

        if self.daily_request_count == FLIGHTAWARE_DAILY_LIMIT:
            logger.warning("Maximum number of daily FlightAware requests have been used, skipping")
            return True
        elif self.daily_request_count >= FLIGHTAWARE_DAILY_LIMIT * 0.8:
            logger.warning("80% or more of daily FlightAware requests have been used")
            return False

    def get_flight_details(self, sv: StateVector) -> FlightAwareFlight | None:
        """
        TODO
        """
        if self._check_limit_reached():
            return None

        start = (datetime.now(timezone.utc).replace(microsecond=0) - timedelta(hours=24)).isoformat().replace("+00:00", "Z")
        end = (datetime.now(timezone.utc).replace(microsecond=0)).isoformat().replace("+00:00", "Z")

        response = requests.get(
            url=f"{self.base_url}/flights/{sv.callsign}",
            params={
                "ident_type": "designator",
                "start": start,
                "end": end,
            },
            headers={"x-apikey": self.api_key},
        )
        response.raise_for_status()
        response = response.json()
        self.daily_request_count += 1

        # Validate response and ensure flight is en route
        flights = response["flights"]
        if len(flights) == 0:
            logger.warning(f"No flights found for {sv.callsign}")
            return None

        fa_flight = None
        for flight in flights:
            if "En Route" not in flight["status"]:
                continue
            else:
                fa_flight = FlightAwareFlight(**flight)

        if fa_flight is None:
            logger.warning(f"No en route flights found for {sv.callsign}")
            return None

        # Convert plane and airline to friendly names
        for plane in self.planes:
            if fa_flight.aircraft_type == plane.icao:
                # TODO: better way to handle? idk if notification details dataclass is best approach here
                fa_flight.aircraft_type = plane.name
                fa_flight.notification_details.aircraft_type = plane.name

        for airline in self.airlines:
            if fa_flight.operator_icao == airline.icao:
                # TODO: better way to handle? idk if notification details dataclass is best approach here
                fa_flight.operator = airline.name
                fa_flight.notification_details.operator = airline.name

        return fa_flight
