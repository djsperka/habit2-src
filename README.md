# habit2-src
Application for running habituation-type looking experiments

## Building
```shell
cd distribution
# build x64 for mojave
python3 build-distribution-package.py -q 5.12.1 -g v2.2.7.1 -d 2.2.7.1 -s 2.2.7.1 -e _mac_x64 -t "Habit 2.2.7.1"
# build x64 for sierra
python3 build-distribution-package.py -q 5.9.1 -g v2.2.7.1 -d 2.2.7.1 -s 2.2.7.1 -e _mac_sierra_x64 -t "Habit 2.2.7.1"

