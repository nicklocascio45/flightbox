
class FlightCache:
    def __init__(self):
        """
        NOTE: this is unnecessary in its current state, but I'm envisioning 
        adding more functionality here such as a TTL on cache objects and 
        perhaps some more analytical features around what we're detecting
        """
        self.detected = []
