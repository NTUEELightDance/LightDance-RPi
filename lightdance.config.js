const DANCERNAME = "9_monkey"

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
        script: "python ./client/client.py",
    },
    {
        name: "setdancer",
        script: "./setDancer.sh",
    }
    ]
}