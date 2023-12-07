#!/bin/bash


DOCKER=QMA2-DEV-TEST

echo "CMD: docker ps |grep -i $DOCKER"
docker ps |grep -i $DOCKER
echo "----------------------"
sleep 1

echo "----------------------"
echo "CMD: docker rm $DOCKER"
docker rm $DOCKER
sleep 1

echo "----------------------"
echo "CMD: docker-compose -f docker-compose.yml up -d"
#sudo docker-compose -f docker-compose.yml up -d
docker-compose -f docker-compose.yml up -d
sleep 1

echo "----------------------"
echo "CMD: docker ps |grep -i $DOCKER"
docker ps |grep -i $DOCKER
