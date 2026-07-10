from models import SearchCoords, Area

# API information
OPENSKY_BASE_URL = "https://opensky-network.org/api"
OPENSKY_TOKEN_URL = "https://auth.opensky-network.org/auth/realms/opensky-network/protocol/openid-connect/token"
OPENSKY_TOKEN_REFRESH_MARGIN = 30

FLIGHTAWARE_BASE_URL = "https://aeroapi.flightaware.com/aeroapi"
FLIGHTAWARE_DAILY_LIMIT = 30

PLANESPOTTERS_BASE_HEX_URL = "https://api.planespotters.net/pub/photos/hex"
PLANESPOTTERS_BASE_REG_URL = "https://api.planespotters.net/pub/photos/reg"

# Search Areas
SEARCH_AREAS = {
    Area.EAST: SearchCoords(
        lat_min=47.591,
        lat_max=47.609,
        lng_min=-122.304,
        lng_max=-122.280,
        track_min_deg=0,
        track_max_deg=90,
    ),
    Area.NORTH: SearchCoords(
        lat_min=47.600,
        lat_max=47.616,
        lng_min=-122.317,
        lng_max=-122.307,
        track_min_deg=5,
        track_max_deg=355,
    ),
    Area.WEST: SearchCoords(
        lat_min=47.298,
        lat_max=47.612,
        lng_min=-122.380,
        lng_max=-122.353,
        track_min_deg=260,
        track_max_deg=350,
    ),
    Area.SOUTH: SearchCoords(
        lat_min=47.600,
        lat_max=47.620,
        lng_min=-122.319,
        lng_max=-122.305,
        track_min_deg=170,
        track_max_deg=185,
    ),
}

# Orchestration constants
DETECTION_INTERVAL_SECONDS = 1

# MQTT Details
NOTIFY_TOPIC = "/flightbox/flights/visible"

# ========== CURRENTLY UNUSED ==========
# Prediction logic constants
STEP_SIZE_SECONDS = 20                          # calculated from shortest possible time across AOI
FURTHEST_DISTANCE_SECONDS = 152.3               # calculated from size of search box
DEG_LAT_METERS = 111.32 * 1000                  # one degree of latitude in meters
DEG_LNG_METERS_MULTIPLIER = 40075 * 1000        # longitude degree meters requires current location, this is a constant in the calculation
DEG_LNG_METERS_DIVISOR = 360                    # longitude degree meters requires current location, this is a constant in the calculation

AERODATA_BASE_URL = "https://aerodatabox.p.rapidapi.com/flights/Icao24"
AERODATA_HOST_HEADER = "aerodatabox.p.rapidapi.com"

# Search coordinate information - east AOI
EAST_LAT_MIN = 47.591
EAST_LNG_MIN = -122.297
EAST_LAT_MAX = 47.604
EAST_LNG_MAX = -122.290

# Search coordinate information - north AOI 
NORTH_LAT_MIN = 47.600
NORTH_LNG_MIN = 122.317
NORTH_LAT_MAX = 47.616
NORTH_LNG_MAX = -122.307

# Search coordinate information - west AOI
WEST_LAT_MIN = 47.568
WEST_LNG_MIN = -122.353
WEST_LAT_MAX = 47.612
WEST_LNG_MAX = -122.380

# Search coordinate information - south AOI
SOUTH_LAT_MIN = 47.620
SOUTH_LNG_MIN = -122.319
SOUTH_LAT_MAX = 47.600
SOUTH_LNG_MAX = -122.305
