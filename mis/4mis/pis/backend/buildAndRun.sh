#!/bin/sh
mvn clean package && docker build -t com.airhacks/pis.bakery .
docker rm -f pis.bakery || true && docker run -d -p 8080:8080 -p 4848:4848 --name pis.bakery com.airhacks/pis.bakery 
