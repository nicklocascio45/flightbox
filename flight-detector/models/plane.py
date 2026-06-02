from dataclasses import dataclass


@dataclass
class Plane:
    name: str
    iata: str
    icao: str
    widebody: str
