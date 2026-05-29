#!/bin/sh

# Base installs
apt-get update && apt-get upgrade -y
apt-get install -y \
    vim \
    procps \
    mosquitto \
    mosquitto-clients \
    openssl

# Run pid directory setup
mkdir -p /run/mosquitto
chown mosquitto:mosquitto /run/mosquitto
chmod 755 /run/mosquitto

# Create certificate authority
cd /etc/mosquitto
mkdir -p ca_certificates
cd ca_certificates/
openssl genrsa -out ca.key 4096
openssl req -new -x509 -days 36500 \
    -key ca.key -out ca.crt \
    -subj "/C=US/ST=Washington/L=Seattle/O=custom-ca/CN=custom-ca"

cd ..
chown -R mosquitto:mosquitto ca_certificates
chmod 755 ca_certificates
chmod 600 ca_certificates/ca.key
chmod 644 ca_certificates/ca.crt
