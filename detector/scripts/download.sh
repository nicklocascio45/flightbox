#!/bin/bash
rm detector.tar docker-compose.yml
aws s3 cp s3://nicklocascio-transfer-files-bucket/detector.tar . --profile development
aws s3 cp s3://nicklocascio-transfer-files-bucket/docker-compose.yml . --profile development
docker load -i detector.tar
docker compose up -d
aws s3 rm s3://nicklocascio-transfer-files-bucket/detector.tar --profile development
aws s3 rm s3://nicklocascio-transfer-files-bucket/docker-compose.yml --profile development
