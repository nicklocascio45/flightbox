#########################################################
# 			NOTE: THIS CAN BE OPTIMIZED					#
# 		THIS IS SIMPLY COMMANDS THAT I'M RUNNING		#
# 		BUT I WILL AUTOMATE EVERYTHING E2E 				#
#########################################################

# Build and copy
docker build -t mosquitto .
docker save -o mosquitto.tar mosquitto:latest
aws s3 cp mosquitto.tar s3://nicklocascio-transfer-files-bucket --profile development

# On pi download and run
aws s3 cp s3://nicklocascio-transfer-files-bucket/mosquitto.tar . --profile development
docker load -i mosquitto.tar
hostname -I
docker run -dt -e MQ_PASSWORD=PASSWORD_PLACEHOLDER -e HOST_IP=IP_PLACEHOLDER -p 1883:1883 -p 8883:8883 mosquitto
docker cp CONTAINER_ID_PLACEHOLDER:/etc/mosquitto/ca_certificates/ca.crt .
aws s3 rm s3://nicklocascio-transfer-files-bucket/mosquitto.tar --profile development

# On container to sub to topic
mosquitto_sub -h localhost -p 1883 \
    -t "kitchen/+" \
    -u "iot" -P "PASSWORD_PLACEHOLDER"

# On local machine download cert and publish to pi container
scp -i ~/.ssh/rpi4 nicklocascio@rpi4:ca.crt .
mosquitto_pub -h IP_PLACEHOLDER -p 8883 \
    --cafile ca.crt \
    -t "kitchen/my-sensor" -m '{"temp":25}' \
    -u "iot" -P "PASSWORD_PLACEHOLDER"
