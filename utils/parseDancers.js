/* eslint-disable camelcase */
/* eslint-disable no-console */
/**
 * Usage: node parseDancers.js <path_to_dancer.json> <output_path.json>
 */

const fs = require("fs");
const { exit } = require("process");


const testFolder = './data/dancers'
const inputPaths = []
const files = fs.readdirSync(testFolder);
files.forEach(file => {
  console.log(file)
  inputPaths.push(testFolder + "/" + file);
});


inputPaths.forEach(inputPath => {
  const outputPath = inputPath;

   // Read File
   console.log("Reading json from ... ", inputPath);
   let raw = null;
   try {
     raw = fs.readFileSync(inputPath);
   } catch (err) {
     console.error(`[Error] Can't open file ${inputPath}`);
     exit();
   }
   const dancer = JSON.parse(raw);
  const output = {};
   
  const ELPARTS = "ELPARTS";
  const LEDPARTS = "LEDPARTS";
  
  // elparts

  const DATA = {
    "S_HAT": 1,
    "S_GLASSES": 2,
    "S_L_COAT": 3,
    "S_R_COAT": 4,
    "S_COLLAR": 5,
    "S_NECKTIE": 6,
    "S_L_HAND": 7,
    "S_R_HAND": 8,
    "B_MASK": 11,
    "B_L_ARM": 12,
    "B_R_ARM": 13,
    "B_BODY": 14,
    "B_L_FOREARM": 15,
    "B_R_FOREARM": 16,
    "G_MASK": 11,
    "G_L_ARM": 12,
    "G_R_ARM": 13,
    "G_BODY": 14,
    "G_L_FOREARM": 15,
    "G_R_FOREARM": 16,
    "S_BELT": 18,
    "B_BELT": 19,
    "G_BELT": 19,
    "S_L_PANT": 21,
    "S_R_PANT": 22,
    "S_L_SHOE": 23,
    "S_R_SHOE": 24,
    "B_L_PANT": 27,
    "B_R_PANT": 28,
    "B_L_LEG": 29,
    "B_R_LEG": 30,
    "B_L_SHOE": 31,
    "B_R_SHOE": 32,
    "G_L_PANT": 27,
    "G_R_PANT": 28,
    "G_L_LEG": 29,
    "G_R_LEG": 30,
    "G_L_SHOE": 31,
    "G_R_SHOE": 32,
  }
  output[ELPARTS] = {};
  const elparts = dancer[ELPARTS];
  Object.keys(elparts).map((name) => {
    output[ELPARTS][name] = DATA[name];
  })
  
  // ledparts
  output[LEDPARTS] = {
    "LED_NECKTIE": { "id": 0, "len": 56 },
    "LED_L_SHOULDER": { "id": 1, "len": 18 },
    "LED_R_SHOULDER": { "id": 2, "len": 18 },
    "LED_R_SHOE": { "id": 3, "len": 40 },
    "LED_L_SHOE": { "id": 4, "len": 40 },
    "LED_BELT": { "id": 5, "len": 52 }
  };
  
  
  // Write File
  fs.writeFile(outputPath, JSON.stringify(output), () => {
    console.log(`Writing new file to ... ${outputPath}`);
  });
})
