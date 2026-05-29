FROM debian:stable-slim

COPY . .
RUN chmod +x setup.sh entrypoint.sh
RUN ./setup.sh
RUN mv mosquitto.conf /etc/mosquitto/

ENTRYPOINT [ "./entrypoint.sh" ]
