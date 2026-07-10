import math

from models import StateVector
from config import (
    STEP_SIZE_SECONDS,
    FURTHEST_DISTANCE_SECONDS,
    DEG_LAT_METERS,
    DEG_LNG_METERS_MULTIPLIER,
    DEG_LNG_METERS_DIVISOR,
)


def _calculate_position(sv: StateVector, t: int):
    """
    Given a state vector and time in seconds, calculate latitude
    and longitude of the plane in t seconds    
    """
    track_radians = sv.true_track * (math.pi / 180)
    pred_lat = sv.latitude + (sv.velocity * t * math.cos(track_radians) / DEG_LAT_METERS)
    pred_lng = sv.longitude + (sv.velocity * t * math.sin(track_radians) / (DEG_LNG_METERS_MULTIPLIER * math.cos(sv.latitude) / DEG_LNG_METERS_DIVISOR))

    return pred_lat, pred_lng


def step_predict_intersection(sv: StateVector, lat_min: float, lng_min: float, lat_max: float, lng_max: float) -> int | None:
    """
    Based on furthest possible distance from AOI and constant step size
    in seconds, calculate plane positions at each time step. If any of
    the predicted positions intersect with AOI, that is a flight to alert on

    NOTE: I decided to stop using this for the time being. The flight path
    that I am on is predictable enough and right on a curve, so this dead
    reckoning prediction does not perform as well as simply checking AOI
    at a higher rate. For future implementations this could come in handy so
    I'm keeping it. There are also more sophisticated ways that we can go
    about prediction as well
    """
    step_count = math.ceil(FURTHEST_DISTANCE_SECONDS / STEP_SIZE_SECONDS)
    for i in range(step_count + 1):
        t = i * STEP_SIZE_SECONDS
        pred_lat, pred_lng = _calculate_position(sv=sv, t=t)

        if pred_lat > lat_min and pred_lat < lat_max and pred_lng > lng_min and pred_lng < lng_max:
            return t

    return None
