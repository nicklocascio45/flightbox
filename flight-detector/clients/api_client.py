import abc

import requests


class ApiClient(metaclass=abc.ABCMeta):
    def __init__(self, base_url):
        self.base_url = base_url

    def make_request(self, path: str, query_params: dict) -> dict:
        response = requests.get(
            url=f"{self.base_url}/{path}",
            params=query_params,
            headers=self.get_headers(),
        )
        response.raise_for_status()
        return response.json()

    @abc.abstractmethod
    def get_headers(self) -> dict:
        pass
