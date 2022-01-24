# 2021-LightDance-RPi

## Installation
### Setup zeromq

You must first install `cppzmq` to enable the usage of controller

-   Installation on Mac:
    Type in the following script:

    ```shell
    cd LightDance-RPi/scripts/
    bash install_zeromq_mac.sh
    ```

    Reference link: https://github.com/zeromq/cppzmq

-   Installation on RPi:
    Type in the following script:

    ```shell
    cd LightDance-RPi/scripts/
    bash install_zeromq_rpi.sh
    ```

    Reference link: http://osdevlab.blogspot.com/2015/12/how-to-install-zeromq-package-in.html

### Install Python Dependencies
```
# At LightDance-RPi/
pip3 install -r requirements.txt
```

### Formatter & Linter
```
bash scripts/py_formatter.sh

bash scripts/py_lint.sh
```

## Run
### Run Controller
Bulid
```
bash compile.sh
```

Run
```
./test_controller/controller
```

### Run CLI
```
python3 cli/cli.py
```