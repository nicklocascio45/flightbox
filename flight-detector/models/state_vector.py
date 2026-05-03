from dataclasses import dataclass

@dataclass
class StateVector:
    icao24: str
    callsign: str
    origin_country: str
    time_position: int
    last_contact: int
    longitude: float
    latitude: float
    baro_altitude: float
    on_ground: bool
    velocity: float
    true_track: float
    vertical_rate: float
    sensors: list[int]
    geo_altitude: float
    squawk: str
    spi: bool
    position_source: int
    category: int
