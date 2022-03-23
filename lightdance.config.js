const DANCER_NAME = "9_monkey"

module.exports = {
    apps: [{
        name: "controller",
        script: "./controller/controller $DANCER_NAME",
        env: {
            "DANCER_NAME": DANCER_NAME,
        }
    },
    {
        name: "client",
        script: "python3 ./client/client.py",
    },
    {
        name: "setdancer",
        script: "sudo bash ./setDancer.sh; sleep 5",
    }
    ]
}