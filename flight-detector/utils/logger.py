import logging
from logging.handlers import TimedRotatingFileHandler

def get_logger(logger_name: str, log_level: str = logging.INFO, file_handler: bool = False, log_file_path: str = "logs")-> logging.Logger:
    """
    
    """
    logger = logging.getLogger(logger_name)
    logger.setLevel(log_level)

    formatter = logging.Formatter(
        fmt="%(asctime)s - %(levelname)s - %(name)s - %(lineno)d - %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S"
    )

    stream = logging.StreamHandler()
    stream.setFormatter(formatter)
    logger.addHandler(stream)

    if file_handler:
        file_log = TimedRotatingFileHandler(
            filename=log_file_path,
            when="H",
            interval=1,
        )
        file_log.setFormatter(formatter)
        logger.addHandler(file_log)

    return logger

logger = get_logger(logger_name="flight-detector", file_handler=True, log_file_path="logs/flight_detector.log")
