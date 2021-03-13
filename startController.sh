#!/bin/bash
# If the arguments are not enough, print the usage
if [[ $# -lt 1 ]]; then
    echo "Usage: $0 <dancer_name>"
    exit 0
fi

sudo ./controller/controller $1