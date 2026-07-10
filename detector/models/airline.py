from dataclasses import dataclass


@dataclass
class Airline:
    id: str
    name: str
    alias: str
    iata: str
    icao: str
    callsign: str
    country: str
    active: str
