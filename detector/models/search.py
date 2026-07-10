from dataclasses import dataclass
from enum import Enum


class Area(Enum):
    EAST = "EAST"
    NORTH = "NORTH"
    WEST = "WEST"
    SOUTH = "SOUTH"


@dataclass
class SearchCoords:
    lat_min: float
    lat_max: float
    lng_min: float
    lng_max: float
    track_min_deg: float
    track_max_deg: float
