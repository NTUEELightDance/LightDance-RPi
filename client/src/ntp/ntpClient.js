const dgram = require("dgram");
const SERVER_PORT = 7122;
const SERVER_IP = "192.168.0.200"; // should be changed

/**
 * NtpClient on RPi
 */
class NtpClient {
  constructor() {
    this.client = dgram.createSocket("udp4");
    this.timeData = {
      t0: null,
      t1: null,
      t2: null,
      t3: null,
    };
    this.init();
    this.cb = null;
  }

  /**
   * initiate
   */
  init = () => {
    this.client.on("close", () => {
      console.log("socket closed");
    });
    this.client.on("error", (err) => {
      console.error(err);
    });
    this.client.on("message", (msg, rinfo) => {
      this.getMsg(msg, rinfo);
    });
  };

  /**
   * start time syncing
   * @param { function } cb callbCk function
   */
  startTimeSync = (cb) => {
    this.cb = cb;
    this.timeData.t0 = Date.now(); // client send time
    this.timeData.t1 = null;
    this.timeData.t2 = null;
    this.timeData.t3 = null;
    console.log("Start syncing ...", this.timeData);
    this.sendMsg(this.timeData);
  };

  /**
   * Set time
   * @param {} param0
   */
  setTime = (sysTime) => {
    this.timeData.t1 = sysTime; // server receive time
    this.timeData.t2 = sysTime; // server send time, set the same as t1 (almost same by testing)
    this.timeData.t3 = Date.now(); // client receive time

    const { t0, t1, t2, t3 } = this.timeData;
    console.log(`t0: ${t0}, t1:${t1}, t2:${t2}, t3:${t3}`);
    // Check Type
    if (typeof t0 != "number") {
      console.error(`t0(${t0}) is not a number !`);
      return;
    }
    if (typeof t1 != "number") {
      console.error(`t0(${t1}) is not a number !`);
      return;
    }
    if (typeof t2 != "number") {
      console.error(`t0(${t2}) is not a number !`);
      return;
    }
    if (typeof t3 != "number") {
      console.error(`t0(${t3}) is not a number !`);
      return;
    }
    // Calculate time shift
    const delay = Math.round((t3 - t0 - (t2 - t1)) / 2);
    const offset = Math.round(((t1 - t0)+(t2 - t3)) / 2);
    console.log(`delay: ${delay}, offset: ${offset}`)
    // TODO: set time
    require("child_process").exec(`sudo date +%s -s @${(t2+delay)/1000}`, (msg) => {
    console.log(msg)});
    this.cb(delay, offset);
  };

  /**
   * Get message from server server
   * message is a JSON containing timeData (t1, t2 is set by server)
   * @param {*} msg - array of number string (timestamp)
   * @param {*} rinfo
   */
  getMsg = (msg, rinfo) => {
    console.log(`receive ${msg} from ${rinfo.address}:${rinfo.port}`);
    // this.setTime(JSON.parse(msg));
    this.setTime(parseInt(msg));
  };

  /**
   * Send message to ntp server
   * @param {*} msg
   */
  sendMsg = (msg) => {
    this.client.send(JSON.stringify(msg), SERVER_PORT, SERVER_IP);
  };
}

module.exports = NtpClient;

// Usage: 
// const ntpClient = new NtpClient();
// ntpClient.startTimeSync((time) => {
//   console.log(`Set to time ${time}`);
// });

