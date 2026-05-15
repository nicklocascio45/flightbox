#!/bin/bash
# Create server certificate and self sign using our CA
cd /etc/mosquitto
mkdir -p certs
cd certs/

openssl genrsa -out server.key 4096

openssl req -new -key server.key \
    -out /etc/mosquitto/certs/server.csr \
    -subj "/C=US/ST=Washington/L=Seattle/O=custom-ca/CN=$HOST_IP" \
    -addext "subjectAltName=IP:$HOST_IP"

openssl x509 -req -CAcreateserial -days 36500 \
    -in server.csr \
    -CA ../ca_certificates/ca.crt \
    -CAkey ../ca_certificates/ca.key \
    -out server.crt \
    -copy_extensions copy

cd ..
chown -R mosquitto:mosquitto certs
chmod 755 certs
chmod 600 certs/server.key
chmod 644 certs/server.crt

# Set password from runtime env var
mosquitto_passwd -b -c /etc/mosquitto/passwd iot $MQ_PASSWORD
chmod 700 /etc/mosquitto/passwd
chown mosquitto:mosquitto /etc/mosquitto/passwd

# Run in non-daemon mode
mosquitto -c /etc/mosquitto/mosquitto.conf
