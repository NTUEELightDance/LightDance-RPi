set -euxo pipefail

# Sort imports
isort .
# Autoformat code
black . --experimental-string-processing

set +euxo pipefail