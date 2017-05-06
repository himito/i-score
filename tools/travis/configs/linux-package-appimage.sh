#!/bin/sh

docker pull vmoscore/i-score-package-linux
docker run --name buildvm vmoscore/i-score-package-linux /bin/bash Recipe
docker cp buildvm:/i-score.AppImage i-score.AppImage
