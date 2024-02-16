# 2024-LightDance-RPi

## Develope flow
Step 1. Fork a copy from the organization repo  to your personal repo

Step 2. Clone the repo to your local machine

```shell
# USERNAME – your Github user account name.
# flag --recursive to completely clone repos including hardware
git clone --recursive git@github.com:${USERNAME}/LightDance-RPi.git
cd LightDance-RPi
# set upstream
git remote add upstream git@github.com:NTUEELightDance/LightDance-RPi.git


# Make sure don’t accidentally merge
git config --add merge.ff only
git config --add pull.ff only
```

Step 3. Build your local project

```shell
cd controller/hardware/
git checkout origin/local_test
cd ..
mkdir build
cd build
cmake ..
make install
```