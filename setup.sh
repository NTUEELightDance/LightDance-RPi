#!/bin/bash

sudo cp system/controller.service /etc/systemd/system
sudo cp system/client.service /etc/systemd/system
sudo cp system/nthu_controller.service /etc/systemd/system

sudo systemctl daemon-reload
sudo systemctl start controller.service
sudo systemctl enable controller
sudo systemctl start client.service
sudo systemctl enable client
sudo systemctl start nthu_controller.service
sudo systemctl enable nthu_controller.service

