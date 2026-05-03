# API information
OPENSKY_BASE_URL = "https://opensky-network.org/api"
OPENSKY_TOKEN_URL = "https://auth.opensky-network.org/auth/realms/opensky-network/protocol/openid-connect/token"
OPENSKY_TOKEN_REFRESH_MARGIN = 30

AERODATA_BASE_URL = "https://aerodatabox.p.rapidapi.com/flights/Icao24"
AERODATA_HOST_HEADER = "aerodatabox.p.rapidapi.com"

FLIGHTAWARE_BASE_URL = "https://aeroapi.flightaware.com/aeroapi"

PLANESPOTTERS_BASE_HEX_URL = "https://api.planespotters.net/pub/photos/hex"
PLANESPOTTERS_BASE_REG_URL = "https://api.planespotters.net/pub/photos/reg"

# Search coordinate information
SEARCH_LAT_MIN = 47.461        # lower bound
SEARCH_LNG_MIN = -122.460      # left bound
SEARCH_LAT_MAX = 47.738        # upper bound
SEARCH_LNG_MAX = -122.264      # right bound

AOI_LAT_MIN = 47.588           # lower bound
AOI_LNG_MIN = -122.380         # left bound
AOI_LAT_MAX = 47.612           # upper bound
AOI_LNG_MAX = -122.344         # right bound

# Prediction logic constants
STEP_SIZE_SECONDS = 20                          # calculated from shortest possible time across AOI
FURTHEST_DISTANCE_SECONDS = 152.3               # calculated from size of search box
DEG_LAT_METERS = 111.32 * 1000                  # one degree of latitude in meters
DEG_LNG_METERS_MULTIPLIER = 40075 * 1000        # longitude degree meters requires current location, this is a constant in the calculation
DEG_LNG_METERS_DIVISOR = 360                    # longitude degree meters requires current location, this is a constant in the calculation

# Orchestration constants
DETECTION_INTERVAL_SECONDS = 30
