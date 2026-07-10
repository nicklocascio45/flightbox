from datetime import datetime, timedelta

import requests

from models import StateVector, Area
from config import (
    OPENSKY_BASE_URL,
    OPENSKY_TOKEN_URL,
    OPENSKY_TOKEN_REFRESH_MARGIN,
    SEARCH_AREAS,
)


class OpenSkyClient:
    def __init__(self, client_id: str, client_secret: str):
        self.client_id = client_id
        self.client_secret = client_secret

        self.token_url = OPENSKY_TOKEN_URL
        self.base_url = OPENSKY_BASE_URL
        self.token: str = None
        self.expires_at: datetime = None

    def _get_token(self) -> str:
        """
        Return a valid access token, refreshing automatically if needed
        """
        if self.token and self.expires_at and datetime.now() < self.expires_at:
            return self.token
        
        return self._refresh_token()

    def _refresh_token(self) -> str:
        """
        Fetch new access token from OpenSky auth server
        """
        response = requests.post(
            url=self.token_url,
            data={
                "grant_type": "client_credentials",
                "client_id": self.client_id,
                "client_secret": self.client_secret,
            },
        )
        response.raise_for_status()

        data = response.json()
        self.token = data["access_token"]
        expires_in = data.get("expires_in", 1800)
        self.expires_at = datetime.now() + timedelta(seconds=expires_in - OPENSKY_TOKEN_REFRESH_MARGIN)
        return self.token

    def search_box(self, lat_min: float, lng_min: float, lat_max: float, lng_max: float) -> list[StateVector]:
        """
        Search specified box for flights and return state vectors
        for all found flights that are in the air within that box
        """
        response = requests.get(
            url=f"{self.base_url}/states/all",
            params={
                "lamin": lat_min,
                "lomin": lng_min,
                "lamax": lat_max,
                "lomax": lng_max,
                "extended": 1,
            },
            headers={"Authorization": f"Bearer {self._get_token()}"}
        )
        self.daily_remaining = response.headers.get("X-Rate-Limit-Remaining")
        response.raise_for_status()
        response = response.json()

        states = response["states"]
        if states:
            # NOTE: investigate why only one * here, but two ** for flightaware?
            # TODO: add more filtering on altitude / type (maybe not possible)
            state_vectors = [StateVector(*state) for state in states]
            return [sv for sv in state_vectors if not sv.on_ground]
        else:
            return []

    def _filter_own_box(self, state_vectors: list[StateVector]) -> list[StateVector]:
        filtered_vectors = []
        # TODO: Clean this up, want to focus on firmware right now though
        for sv in state_vectors:
            track = sv.true_track
            lat = sv.latitude
            lng = sv.longitude

            for area, coords in SEARCH_AREAS.items():
                if area == Area.NORTH and (track > coords.track_max_deg or track < coords.track_min_deg):
                    if lat > coords.lat_min and lat < coords.lat_max and lng > coords.lng_min and lng < coords.lng_max:
                        sv.area = area
                        filtered_vectors.append(sv)
                elif track > coords.track_min_deg and track < coords.track_max_deg:
                    if lat > coords.lat_min and lat < coords.lat_max and lng > coords.lng_min and lng < coords.lng_max:
                        sv.area = area
                        filtered_vectors.append(sv)

        return filtered_vectors

    def search_own_box(self) -> list[StateVector]:
        """
        Search own receiver for flights and return all state vectors
        """
        response = requests.get(
            url=f"{self.base_url}/states/own",
            headers={"Authorization": f"Bearer {self._get_token()}"}
        )
        self.daily_remaining = response.headers.get("X-Rate-Limit-Remaining")
        response.raise_for_status()
        response = response.json()

        states = response["states"]
        if states:
            state_vectors = [StateVector(*state, category=0) for state in states]
            return self._filter_own_box(state_vectors)
        else:
            return []
