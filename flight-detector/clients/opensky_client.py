from datetime import datetime, timedelta

import requests

from models import StateVector
from config import (
    OPENSKY_BASE_URL,
    OPENSKY_TOKEN_URL,
    OPENSKY_TOKEN_REFRESH_MARGIN,
)


class OpenSkyClient:
    def __init__(self, client_id: str, client_secret: str):
        self.client_id = client_id
        self.client_secret = client_secret

        self.token_url = OPENSKY_TOKEN_URL
        self.base_url = OPENSKY_BASE_URL
        self.token: str = None
        self.expires_at: datetime = None

        # TODO: want some sort of watcher on this so we don't go over?
        self.daily_remaining = 4000

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
