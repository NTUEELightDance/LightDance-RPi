set -euxo pipefail

# Check imports
isort -c .
# Check code format
black . --check --experimental-string-processing
# Check lint: Not checking the code in website
flake8 submarine-sdk/ dev-support/
# Check static typing for Python
mypy --ignore-missing-imports .

set +euxo pipefail