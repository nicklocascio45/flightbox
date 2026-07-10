from dataclasses import dataclass, field

from .search import Area


@dataclass
class Airport:
    code: str
    code_icao: str
    code_iata: str
    code_lid: str
    timezone: str
    name: str
    city: str
    airport_info_url: str


@dataclass
class FlightAwareFlight:
    ident: str
    ident_icao: str
    ident_iata: str
    actual_runway_off: str
    actual_runway_on: str
    fa_flight_id: str
    operator: str
    operator_icao: str
    operator_iata: str
    flight_number: str
    registration: str
    atc_ident: str
    inbound_fa_flight_id: str
    codeshares: list[str]
    codeshares_iata: list[str]
    blocked: bool
    diverted: bool
    cancelled: bool
    position_only: bool
    origin: Airport
    destination: Airport
    departure_delay: int
    arrival_delay: int
    filed_ete: int
    foresight_predictions_available: bool
    scheduled_out: str
    estimated_out: str
    actual_out: str
    scheduled_off: str
    estimated_off: str
    actual_off: str
    scheduled_on: str
    estimated_on: str
    actual_on: str
    scheduled_in: str
    estimated_in: str
    actual_in: str
    progress_percent: int
    status: str
    aircraft_type: str
    route_distance: int
    filed_airspeed: int
    filed_altitude: int
    route: str
    baggage_claim: str
    seats_cabin_business: int
    seats_cabin_coach: int
    seats_cabin_first: int
    gate_origin: str
    gate_destination: str
    terminal_origin: str
    terminal_destination: str
    type: str
    area: Area = field(init=False)
    widebody: bool = field(init=False)

    def __post_init__(self):
        if isinstance(self.origin, dict):
            self.origin = Airport(**self.origin)

        if isinstance(self.destination, dict):
            self.destination = Airport(**self.destination)
