# API information
OPENSKY_BASE_URL = "https://opensky-network.org/api"
OPENSKY_TOKEN_URL = "https://auth.opensky-network.org/auth/realms/opensky-network/protocol/openid-connect/token"
OPENSKY_TOKEN_REFRESH_MARGIN = 30

AERODATA_BASE_URL = "https://aerodatabox.p.rapidapi.com/flights/Icao24"
AERODATA_HOST_HEADER = "aerodatabox.p.rapidapi.com"

FLIGHTAWARE_BASE_URL = "https://aeroapi.flightaware.com/aeroapi"
FLIGHTAWARE_DAILY_LIMIT = 30

PLANESPOTTERS_BASE_HEX_URL = "https://api.planespotters.net/pub/photos/hex"
PLANESPOTTERS_BASE_REG_URL = "https://api.planespotters.net/pub/photos/reg"

# Search coordinate information - larger box for dead reckoning
SEARCH_LAT_MIN = 47.461        # lower bound
SEARCH_LNG_MIN = -122.460      # left bound
SEARCH_LAT_MAX = 47.738        # upper bound
SEARCH_LNG_MAX = -122.264      # right bound

# Search coordinate information - actual visible AOI from couch
AOI_LAT_MIN = 47.588
AOI_LNG_MIN = -122.380
AOI_LAT_MAX = 47.612
AOI_LNG_MAX = -122.344

# Search coordinate information - secondary AOI more commonly traveled
ALT_AOI_LAT_MIN = 47.600
ALT_AOI_LNG_MIN = -122.319
ALT_AOI_LAT_MAX = 47.620
ALT_AOI_LNG_MAX = -122.305

# Depart AOI
DEP_AOI_LAT_MIN = 47.590305
DEP_AOI_LNG_MIN = -122.298431
DEP_AOI_LAT_MAX = 47.607205
DEP_AOI_LNG_MAX = -122.279377

# Search coordinate information - secondary AOI more commonly traveled
ALT_SEARCH_AOI_LAT_MIN = 47.560
ALT_SEARCH_AOI_LNG_MIN = -122.319
ALT_SEARCH_AOI_LAT_MAX = 47.660
ALT_SEARCH_AOI_LNG_MAX = -122.305

# Prediction logic constants
STEP_SIZE_SECONDS = 20                          # calculated from shortest possible time across AOI
FURTHEST_DISTANCE_SECONDS = 152.3               # calculated from size of search box
DEG_LAT_METERS = 111.32 * 1000                  # one degree of latitude in meters
DEG_LNG_METERS_MULTIPLIER = 40075 * 1000        # longitude degree meters requires current location, this is a constant in the calculation
DEG_LNG_METERS_DIVISOR = 360                    # longitude degree meters requires current location, this is a constant in the calculation

# Orchestration constants
DETECTION_INTERVAL_SECONDS = 2

# MQTT Details
NOTIFY_TOPIC = "/flightbox/flights/visible"
