module.exports = {
    apps : [{
      name   : "controller",
      script : "./controller/controller $DANCERNAME",
      env: {
        "DANCERNAME": "9_monkey",
      }
    }]
  }