const DANCER_NAME = "9_monkey"

module.exports = {
    apps: [    {
        name: "controller",
        script: "./startController.sh"
    },
    {
        name: "client",
        script: "./client/client.py",
        exec_mode: "fork",
        wait_ready: true,
        autorestart: true,
        interpreter : "python3",
    },
    {
        name: "cli",
        script: "./tmuxStart.sh"
    }
    ]
}