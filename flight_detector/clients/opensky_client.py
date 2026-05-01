import json
from datetime import datetime, timedelta

import requests

from .api_client import ApiClient
from config import (
    OPENSKY_BASE_URL,
    OPENSKY_TOKEN_URL,
    OPENSKY_TOKEN_REFRESH_MARGIN,
)


class OpenSkyClient(ApiClient):
    def __init__(self, client_id: str, client_secret: str):
        super().__init__(base_url=OPENSKY_BASE_URL)

        self.client_id = client_id
        self.client_secret = client_secret
        self.token: str = None
        self.expires_at: datetime = None

    def get_headers(self) -> dict:
        return {"Authorization": f"Bearer {self._get_token()}"}

    def _get_token(self) -> str:
        """
        
        """
        if self.token and self.expires_at and datetime.now() < self.expires_at:
            return self.token
        
        return self._refresh_token()

    def _refresh_token(self) -> str:
        """
        
        """
        response = requests.post(
            url=OPENSKY_TOKEN_URL,
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

    def search_box(self, lat_min, lng_min, lat_max, lng_max):
        response = self.make_request(
            path="states/all",
            query_params={
                "lamin": lat_min,
                "lomin": lng_min,
                "lamax": lat_max,
                "lomax": lng_max,
                "extended": 1,
            }
        )
        print(json.dumps(response, indent=2, default=str))
