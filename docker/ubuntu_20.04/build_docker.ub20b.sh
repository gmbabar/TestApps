#!/bin/bash

# By default it takes "Dockerfile" as the file name
# to build the docker image. If you want to use a
# different file name, then use the -f option.

echo docker build -t gbabar/qma2_test -f Dockerfile.ub20 .
docker build -t gbabar/qma2_test -f Dockerfile.ub20 .

## Publish Docker
# echo docker push gbabar/qma2_test
# docker push gbabar/qma2_test

## Run Docker
# echo docker run -it --rm -v $(pwd):/home/qma2 gbabar/qma2_test
# docker run -it --rm -v $(pwd):/home/qma2 gbabar/qma2_test

