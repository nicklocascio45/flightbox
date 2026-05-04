#!/bin/bash
rm detector.tar
docker build -t detector .
docker save -o detector.tar detector:latest
aws s3 cp detector.tar s3://nicklocascio-transfer-files-bucket --profile development
aws s3 cp docker-compose.yml s3://nicklocascio-transfer-files-bucket --profile development
